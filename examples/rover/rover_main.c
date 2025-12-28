/****************************************************************************
 * apps/examples/hello/hello_main.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <debug.h>
#include <poll.h>
#include <fcntl.h>

#include <nuttx/wireless/lpwan/sx127x.h>

#define DEV_NAME      "/dev/sx127x"
#define BUFFER_MAX    255
#define GGA_BUF_SZ    256
#define FRAME_SIZE    63


/****************************************************************************
 * Private Functions
 ****************************************************************************/

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

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
    struct sx127x_read_hdr_s data;
    char gga[GGA_BUF_SZ];
    int i = 0;
    int ret;
    int s_fd;
    int cnt = 0;
  
    s_fd = open("/dev/ttyS1", O_RDWR | O_NOCTTY);
    if (s_fd < 0)
      {
        perror("serial_open");
        return 1;
      }

    /* Open device */

    int fd;
    fd = open(DEV_NAME, O_RDWR | O_NONBLOCK);
    if (fd < 0)
      {
        int errcode = errno;
        printf("ERROR: Failed to open device %s: %d\n", DEV_NAME, errcode);
        return 0;
      }

    uint8_t modulation = SX127X_MODULATION_FSK;
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

    uint8_t opmode;

    /* We need to get GGA from RTK and send to Base Station */
    int done = 0;

    while (!done && i < 100)
     {
        i++;
        //printf("\rWaiting start message...%c", cnt % 4 == 0 ? '-' : cnt % 4 == 1 ? '\\' : cnt % 4 == 2 ? '|' : '/');
	cnt++;

        if (serial_read_gga(s_fd, gga, sizeof(gga)))
	  {
            char *p;
	    printf("\nGGA:\n%s\n\n", gga);
            opmode = SX127X_OPMODE_TX;
            ret = ioctl(fd, SX127XIOC_OPMODESET, (unsigned long)&opmode);
            if (ret < 0)
              {
                printf("failed change opmode to TX %d!\n", ret);
              }

	    /* GGA has more than 63 bytes, we need to divide it */
            p = &gga[0];
	    write(fd, p, FRAME_SIZE);

	    usleep(30000);

            p = &gga[60];
	    write(fd, p, FRAME_SIZE);
          } 

	/* Try to read from BS, if succeed we continue */

        opmode = SX127X_OPMODE_RX;
        ret = ioctl(fd, SX127XIOC_OPMODESET, (unsigned long)&opmode);
        if (ret < 0)
          {
            printf("failed change opmode to RX %d!\n", ret);
          }

	/* Wait some time to transceiver get message */
        usleep(500000);

        ret = read(fd, &data, sizeof(struct sx127x_read_hdr_s));
        if (ret < 0)
          {
            printf("Read failed %d!\n", ret);
          }

	/* If received something, consider it is start message */
	/* TODO: Make it more robust */
	
	if (data.datalen == FRAME_SIZE)
	  {
            done = 1;
	  }
     }

    printf("Start message received!\n");

    clock_t start = clock_systime_ticks();
    clock_t now;
    clock_t elapsed;
    while(1)
    {
              printf(".");
              if ((cnt % 30) == 0)
                printf("\n");
                  
              cnt++;

	      data.datalen = 0;
              ret = read(fd, &data, sizeof(struct sx127x_read_hdr_s));
              if (ret < 0)
                {
                  printf("Read failed %d!\n", ret);
                  goto errout;
                }

	      if (data.datalen == FRAME_SIZE)
	      {
                write(s_fd, data.data, data.datalen);
	        if (data.data[62] == 0x00)
	          {
                    /* Let's see if it is padding */
		    int j = 62;
		    while (data.data[j] == 0x00)
		    {
                      j--;
		    }
		  
		    if (data.data[j] == 0xAA)
		      {
                        now = clock_systime_ticks();
		        elapsed = now - start;
		        printf("\nElapsed = %d ",TICK2MSEC(elapsed));
                        start = clock_systime_ticks();
		      }
		  }
	      }

	      if (TICK2MSEC(elapsed) <= 1000)
	        {
                  if (serial_read_gga(s_fd, gga, sizeof(gga)))
	            {
                      opmode = SX127X_OPMODE_TX;
                      ret = ioctl(fd, SX127XIOC_OPMODESET, (unsigned long)&opmode);
                      if (ret < 0)
                        {         
                          printf("failed change opmode to TX %d!\n", ret);
                        }
            
                      usleep(10000);
		      i = 0;
                      while (i < 1)
                        {
                          char *p;
			  p = &gga[0];
	                  write(fd, p, FRAME_SIZE);

	                  usleep(30000);

			  p = &gga[60];
	                  write(fd, p, FRAME_SIZE);

	                  usleep(30000);
	                  i++;
                        } 

                      opmode = SX127X_OPMODE_RX;
                      ret = ioctl(fd, SX127XIOC_OPMODESET, (unsigned long)&opmode);
                      if (ret < 0)
                        {
                          printf("failed change opmode to RX %d!\n", ret);
                        }
		    }

                  elapsed = 900000000;
		}

	      usleep(5000);
    }
errout:
  close(fd);
  return 0;
}
