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

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
    struct sx127x_read_hdr_s data;
    int i;
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
    fd = open(DEV_NAME, O_RDWR);
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

    uint8_t opmode = SX127X_OPMODE_RX;
    ret = ioctl(fd, SX127XIOC_OPMODESET, (unsigned long)&opmode);
    if (ret < 0)
      {
        printf("failed change opmode to RX %d!\n", ret);
        goto errout;
      }

    clock_t start = clock_systime_ticks();
    clock_t now;
    clock_t elapsed;
    while(1)
    {
              printf(".");
              if ((cnt % 30) == 0)
                printf("\n");
                  
              cnt++;

              ret = read(fd, &data, sizeof(struct sx127x_read_hdr_s));
              if (ret < 0)
                {
                  printf("Read failed %d!\n", ret);
                  goto errout;
                }

              write(s_fd, data.data, data.datalen);
	      if (data.data[62] == 0x00)
	        {
                  /* Let's see if it is padding */
		  int i = 62;
		  while (data.data[i] == 0x00)
		  {
                    i--;
		  }
		  
		  if (data.data[i] == 0xAA)
		    {
                      now = clock_systime_ticks();
		      elapsed = now - start;
		      printf("\nElapsed = %d ",TICK2MSEC(elapsed));
		      start = clock_systime_ticks();
		    }
		}
	      if (TICK2MSEC(elapsed) <= 1000)
	        {

    opmode = SX127X_OPMODE_TX;
    ret = ioctl(fd, SX127XIOC_OPMODESET, (unsigned long)&opmode);
    if (ret < 0)
      {
        printf("failed change opmode to RX %d!\n", ret);
        goto errout;
      }

    usleep(10000);

    data.data[0] = 'A';
    data.data[1] = 'B';
    data.data[2] = 'C';
    data.data[3] = 'D';
    data.datalen = 4;

    i = 0;
    while (i < 3)
      {
              ret = write(fd, &data.data[0], 63);
	      usleep(30000);
	      i++;
      }
	      printf(" Send %d bytes", ret);

    opmode = SX127X_OPMODE_RX;
    ret = ioctl(fd, SX127XIOC_OPMODESET, (unsigned long)&opmode);
    if (ret < 0)
      {
        printf("failed change opmode to RX %d!\n", ret);
        goto errout;
      }
    elapsed = 900000000;
		}
	      usleep(5000);
    }
errout:
  close(fd);
  return 0;
}
