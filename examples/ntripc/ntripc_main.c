/*
 * Simple NTRIP client for Linux / NuttX
 * Supports:
 *  - Source table
 *  - RTCM streaming
 *  - Static GGA (-g)
 *  - Live GGA from serial (-d)
 *  - RTCM output to serial (-d)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <netdb.h>
#include <time.h>

#include <nuttx/wireless/lpwan/sx127x.h>

/* -------------------------------------------------- */

#define BUF_SZ     1024
#define GGA_BUF_SZ 256
#define CHUNK_SIZE 63  /* MAX amount of data SX1276 sends */

#define DEV_NAME "/dev/sx127x"

/* -------------------------------------------------- */

static int serial_open(const char *dev, int baud)
{
    int fd = open(dev, O_RDWR | O_NOCTTY);
    if (fd < 0)
        return -1;

    struct termios tio;
    tcgetattr(fd, &tio);

    cfmakeraw(&tio);

    speed_t sp = B115200;
    if (baud == 9600)   sp = B9600;
    if (baud == 19200)  sp = B19200;
    if (baud == 38400)  sp = B38400;
    if (baud == 57600)  sp = B57600;
    if (baud == 115200) sp = B115200;

    cfsetispeed(&tio, sp);
    cfsetospeed(&tio, sp);

    tio.c_cflag |= (CLOCAL | CREAD);
    tcsetattr(fd, TCSANOW, &tio);

    return fd;
}

/* -------------------------------------------------- */
/* Read a full NMEA line and return only GGA           */

static int serial_read_gga(int fd, char *gga, size_t max)
{
    static char line[GGA_BUF_SZ];
    static size_t idx = 0;
    char c;

    while (read(fd, &c, 1) == 1) {
        if (c == '\n') {
            line[idx] = 0;
            idx = 0;

            if (!strncmp(line, "$GPGGA", 6) ||
                !strncmp(line, "$GNGGA", 6)) {
                strncpy(gga, line, max - 1);
                gga[max - 1] = 0;
                return 1;
            }
        } else if (idx < sizeof(line) - 1) {
            line[idx++] = c;
        }
    }
    return 0;
}

/* -------------------------------------------------- */

static char *base64(const char *in)
{
    static const char tbl[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    static char out[256];

    unsigned int val = 0;
    int valb = -6;
    int pos = 0;

    for (; *in; in++) {
        val = (val << 8) + (unsigned char)*in;
        valb += 8;
        while (valb >= 0) {
            out[pos++] = tbl[(val >> valb) & 0x3F];
            valb -= 6;
        }
    }
    if (valb > -6)
        out[pos++] = tbl[((val << 8) >> (valb + 8)) & 0x3F];
    while (pos & 3)
        out[pos++] = '=';

    out[pos] = 0;
    return out;
}

/* -------------------------------------------------- */

int main(int argc, char **argv)
{
    const char *server = "qrtksa1.quectel.com";
    const char *user = "Soluevo_00_0000001";
    const char *pass = "itid8x5a";
    const char *mount = "AUTO";
    const char *serial_dev = "/dev/ttyS1";
    const char *gga_static = NULL;
    int baud = 115200;

    /* Open device */

    int fd;
    fd = open(DEV_NAME, O_RDWR);
    if (fd < 0)
      {
        int errcode = errno;
        printf("ERROR: Failed to open device %s: %d\n", DEV_NAME, errcode);
        return 0;
      }
    printf("%s opened...\n", DEV_NAME);

    uint8_t modulation = 1;
    int ret;
    ret = ioctl(fd, SX127XIOC_MODULATIONSET,
              (unsigned long)&modulation);
    if (ret < 0)
      {
        printf("failed change modulation %d!\n", ret);
        goto errout;
      }

    uint32_t frequency = 915000000;
    ret = ioctl(fd, WLIOC_SETRADIOFREQ, (unsigned long)&frequency);
    if (ret < 0)
      {
        printf("failed to change frequency %d!\n", ret);
        goto errout;
      }

    int8_t power = 20;
    ret = ioctl(fd, WLIOC_SETTXPOWER, (unsigned long)&power);
    if (ret < 0)
      {
        printf("failed to change power %d!\n", ret);
        goto errout;
      }
    printf("SX1276 configured...\n");

    int opt;
    while ((opt = getopt(argc, argv, "s:u:p:m:d:b:g:")) != -1) {
        switch (opt) {
        case 's': server = optarg; break;
        case 'u': user = optarg; break;
        case 'p': pass = optarg; break;
        case 'm': mount = optarg; break;
        case 'd': serial_dev = optarg; break;
        case 'b': baud = atoi(optarg); break;
        case 'g': gga_static = optarg; break;
        default:
            return 1;
        }
    }

    if (!server || !user || !pass) {
        fprintf(stderr, "Missing required parameters\n");
        return 1;
    }

    int serial_fd = -1;
    if (serial_dev) {
        serial_fd = serial_open(serial_dev, baud);
        if (serial_fd < 0) {
            perror("serial_open");
            return 1;
        }
    }
    if (serial_fd > 0)
      {
        printf("RTK serial opened...\n");
      }

    /* -------------------------------------------------- */
    /* Connect to caster                                  */

    struct addrinfo hints = {0}, *res;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(server, "2101", &hints, &res) != 0)
        return 1;

    int sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (connect(sock, res->ai_addr, res->ai_addrlen) < 0) {
        perror("connect");
        return 1;
    }
    printf("Connected to %s:%d", server, 2101);

    /* -------------------------------------------------- */
    /* Send GET                                           */

    char auth[128];
    snprintf(auth, sizeof(auth), "%s:%s", user, pass);

    char req[512];
    snprintf(req, sizeof(req),
        "GET /%s HTTP/1.0\r\n"
        "User-Agent: ntripc/1.0\r\n"
        "Authorization: Basic %s\r\n\r\n",
        mount ? mount : "",
        base64(auth));

    send(sock, req, strlen(req), 0);
    printf("GET Auth sent...\n");

    /* -------------------------------------------------- */
    /* ---- NTRIP ICY handling ----                       */

    char buf[BUF_SZ];
    int icy_ok = 0;

    while (!icy_ok) {
        int n = recv(sock, buf, sizeof(buf) - 1, 0);
        if (n <= 0)
            return 1;
        buf[n] = 0;

        if (!strncmp(buf, "ICY 200 OK", 10)) {
            icy_ok = 1;
        } else if (!mount) {
            /* Source table mode */
            write(STDOUT_FILENO, buf, n);
            return 0;
        }
    }
    printf("Received ICY 200!\n");

    /* -------------------------------------------------- */
    /* Main loop                                          */

    time_t last_gga = 0;

    while (1)
    {

        /* ---- GGA upstream ---- */
        if (serial_fd >= 0) {
            char gga[GGA_BUF_SZ];
            if (serial_read_gga(serial_fd, gga, sizeof(gga))) {
                send(sock, gga, strlen(gga), 0);
                send(sock, "\r\n", 2, 0);
                last_gga = time(NULL);
            }
        } else if (gga_static && time(NULL) - last_gga >= 5) {
            send(sock, gga_static, strlen(gga_static), 0);
            send(sock, "\r\n", 2, 0);
            last_gga = time(NULL);
        }

        /* ---- RTCM downstream ---- */
        int n = recv(sock, buf, sizeof(buf), 0);
        if (n <= 0)
            break;

	printf("RECV: %d\n", n);
        if (serial_fd >= 0)
            write(serial_fd, buf, n);
        else
            write(STDOUT_FILENO, buf, n);

        char *p;
	size_t cnt = 0;
	while (cnt < n)
        {
          size_t remaining = n - cnt;
          size_t to_write = remaining > CHUNK_SIZE ?
                            CHUNK_SIZE : remaining;

          ssize_t ret = write(fd, &buf[cnt], to_write);
          if (ret < 0)
            {
              perror("write");
              goto errout;
            }

          cnt += ret;
	}
    }

errout:
    close(fd);
    return 0;
}

