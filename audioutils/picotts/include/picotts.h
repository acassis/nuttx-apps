/****************************************************************************
 * apps/audioutils/picotts/include/picotts.h
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

#ifndef __APPS_AUDIOUTILS_PICOTTS_INCLUDE_PICOTTS_H
#define __APPS_AUDIOUTILS_PICOTTS_INCLUDE_PICOTTS_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define PICOTTS_SAMPLE_FREQ_HZ 16000
#define PICOTTS_SAMPLE_BITS     16

/****************************************************************************
 * Public Types
 ****************************************************************************/

typedef void (*picotts_output_fn)(int16_t *samples, unsigned count);
typedef void (*picotts_error_notify_fn)(void);
typedef void (*picotts_idle_notify_fn)(void);

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/**
 * Initialises the PicoTTS engine and prepares to receive TTS requests.
 * A new thread is launched, which is used to run the TTS engine.
 *
 * @param prio  The priority of the TTS thread.
 * @param output_cb Callback function which gets invoked directly from the
 *                  TTS thread with a buffer of samples generated.  May be
 *                  NULL, in which case audio will be routed via nxaudio.
 * @param core   Reserved for future use (CPU affinity).  Pass -1.
 * @returns True on success, false on failure.
 */

bool picotts_init(unsigned prio, picotts_output_fn output_cb, int core);

/**
 * Adds text to be synthesised. The TTS engine will wait until it sees
 * an appropriate stop (e.g. sentence stop, \\0) before commencing the
 * speech generation.
 *
 * A queue is used to transfer the text to the TTS engine.  The queue
 * size may be configured via Kconfig.  Adding more text while the queue
 * is full will cause this function to block.
 *
 * @param txt  The pointer to the text to be spoken, in UTF8 format.  The
 *             text is copied, so the pointer may be invalidated immediately
 *             upon return from this call.
 * @param len  The number of bytes available in @c txt.
 */

void picotts_add(const char *txt, unsigned len);

/**
 * Wait for synthesis to complete. Times out after 10s.
 */
void picotts_wait_done(void);

/**
 * Stops the TTS engine thread and frees the used memory resources.
 * Call picotts_init() again to reinitialise, if needed.
 */

void picotts_shutdown(void);

/**
 * Sets a callback function to be invoked if picotts encounters an error
 * and aborts.
 *
 * @param cb The callback handler.  Invoked from the TTS thread prior to
 *           said thread exiting.  To resume TTS operation the callback
 *           should schedule a reinitialisation of PicoTTS, i.e.
 *           picotts_shutdown() followed by picotts_init().  Pass NULL to
 *           unregister a set callback function.
 */

void picotts_set_error_notify(picotts_error_notify_fn cb);

/**
 * Sets a callback function which gets called when the TTS engine enters
 * idle state again after having generated data.  May be used for resource
 * arbitration.
 *
 * @param cb The callback handler.  Invoked from the TTS thread when the
 *           engine goes idle.  Pass NULL to unregister a set callback
 *           function.
 */

void picotts_set_idle_notify(picotts_idle_notify_fn cb);

#ifdef __cplusplus
}
#endif

#endif /* __APPS_AUDIOUTILS_PICOTTS_INCLUDE_PICOTTS_H */
