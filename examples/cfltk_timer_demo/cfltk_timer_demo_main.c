/****************************************************************************
 * apps/examples/cfltk_timer_demo/cfltk_timer_demo_main.c
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

#include <stdio.h>
#include <time.h>

#include <cfltk/Fl.h>
#include <cfltk/Fl_Box.h>
#include <cfltk/Fl_Group.h>
#include <cfltk/Fl_Window.h>

/****************************************************************************
 * Private Data
 ****************************************************************************/

#define TICK_SECONDS 0.5

static Fl_Widget *g_counter_box;
static int g_ticks;
static struct timespec g_last;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/* Integer milliseconds, not double -- NuttX's default printf() build
 * doesn't support %f (CONFIG_LIBC_FLOATINGPOINT is off by default),
 * silently printing the literal text "*float*" instead of a number. */
static long elapsed_ms(const struct timespec *from, const struct timespec *to) {
    return (long)(to->tv_sec - from->tv_sec) * 1000L +
           (to->tv_nsec - from->tv_nsec) / 1000000L;
}

/* Fires every TICK_SECONDS via Fl_repeat_timeout() -- exercises
 * fl_backend_wait()'s bounded, real-sleeping wait (fl_nx_event.c):
 * printing the actual elapsed wall-clock time between ticks is the
 * proof it genuinely slept close to TICK_SECONDS rather than busy-
 * spinning (which would print near-zero gaps) or blocking forever
 * (which would never print again at all). */
static void tick_cb(void *data) {
    struct timespec now;
    char buf[32];
    (void)data;

    clock_gettime(CLOCK_MONOTONIC, &now);
    g_ticks++;
    printf("tick=%d elapsed_ms=%ld\n", g_ticks, elapsed_ms(&g_last, &now));
    g_last = now;

    snprintf(buf, sizeof(buf), "Ticks: %d", g_ticks);
    Fl_Widget_copy_label(g_counter_box, buf);
    Fl_Widget_redraw(g_counter_box);

    Fl_repeat_timeout(TICK_SECONDS, tick_cb, NULL);
}

static void on_close(Fl_Widget *w, void *data) {
    (void)data;
    Fl_Window_hide(w);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, FAR char *argv[]) {
    Fl_Window *win;

    (void)argc;
    (void)argv;

    win = Fl_Window_new(20, 20, 200, 100, "cfltk_timer_demo");
    g_counter_box = FL_WIDGET(Fl_Box_new(10, 10, 180, 30, "Ticks: 0"));
    Fl_Group_end(FL_GROUP(win));

    Fl_Widget_set_callback(FL_WIDGET(win), on_close, NULL);
    Fl_Window_show(FL_WIDGET(win));

    clock_gettime(CLOCK_MONOTONIC, &g_last);
    Fl_add_timeout(TICK_SECONDS, tick_cb, NULL);

    Fl_run();

    return 0;
}
