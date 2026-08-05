/****************************************************************************
 * apps/examples/cfltk_hello/cfltk_hello_main.c
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

#include <cfltk/Fl.h>
#include <cfltk/Fl_Window.h>

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/* cfltk's core never auto-hides a window on FL_CLOSE (see
 * Fl_Widget_do_callback_for() -- no registered callback means the
 * event is silently dropped), so without this Fl_run() below would
 * loop forever once the window's close button is clicked. */
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

    win = Fl_Window_new(20, 20, 200, 150, "cfltk_hello");
    Fl_Widget_set_callback(FL_WIDGET(win), on_close, NULL);
    Fl_Window_show(FL_WIDGET(win));

    Fl_run();

    return 0;
}
