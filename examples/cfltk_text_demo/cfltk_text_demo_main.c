/****************************************************************************
 * apps/examples/cfltk_text_demo/cfltk_text_demo_main.c
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

#include <cfltk/Enumerations.h>
#include <cfltk/Fl.h>
#include <cfltk/Fl_Box.h>
#include <cfltk/Fl_Group.h>
#include <cfltk/Fl_Window.h>

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void on_close(Fl_Widget *w, void *data) {
    (void)data;
    Fl_Window_hide(w);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, FAR char *argv[]) {
    Fl_Window *win;
    Fl_Box *b1, *b2, *b3;

    (void)argc;
    (void)argv;

    win = Fl_Window_new(20, 20, 260, 180, "cfltk_text_demo");

    b1 = Fl_Box_new(10, 10, 240, 30, "Helvetica 14 (default)");

    b2 = Fl_Box_new(10, 50, 240, 30, "Courier bold 18");
    Fl_Widget_set_labelfont(FL_WIDGET(b2), FL_COURIER_BOLD);
    Fl_Widget_set_labelsize(FL_WIDGET(b2), 18);

    b3 = Fl_Box_new(10, 90, 240, 30, "Times italic 12");
    Fl_Widget_set_labelfont(FL_WIDGET(b3), FL_TIMES_ITALIC);
    Fl_Widget_set_labelsize(FL_WIDGET(b3), 12);

    Fl_Group_end(FL_GROUP(win));

    (void)b1;

    Fl_Widget_set_callback(FL_WIDGET(win), on_close, NULL);
    Fl_Window_show(FL_WIDGET(win));

    Fl_run();

    return 0;
}
