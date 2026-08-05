/****************************************************************************
 * apps/examples/cfltk_button_demo/cfltk_button_demo_main.c
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

#include <cfltk/Fl.h>
#include <cfltk/Fl_Box.h>
#include <cfltk/Fl_Button.h>
#include <cfltk/Fl_Group.h>
#include <cfltk/Fl_Window.h>

/****************************************************************************
 * Private Data
 ****************************************************************************/

static Fl_Widget *g_counter_box;
static int g_clicks;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void on_close(Fl_Widget *w, void *data) {
    (void)data;
    Fl_Window_hide(w);
}

static void on_click(Fl_Widget *w, void *data) {
    char buf[32];
    (void)w;
    (void)data;

    g_clicks++;
    printf("clicks=%d\n", g_clicks);

    snprintf(buf, sizeof(buf), "Clicks: %d", g_clicks);
    Fl_Widget_copy_label(g_counter_box, buf);
    Fl_Widget_redraw(g_counter_box);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, FAR char *argv[]) {
    Fl_Window *win;
    Fl_Button *btn;

    (void)argc;
    (void)argv;

    win = Fl_Window_new(20, 20, 200, 130, "cfltk_button_demo");

    g_counter_box = FL_WIDGET(Fl_Box_new(10, 10, 180, 30, "Clicks: 0"));

    btn = Fl_Button_new(10, 50, 180, 40, "Click me");
    Fl_Widget_set_callback(FL_WIDGET(btn), on_click, NULL);

    Fl_Group_end(FL_GROUP(win));

    Fl_Widget_set_callback(FL_WIDGET(win), on_close, NULL);
    Fl_Window_show(FL_WIDGET(win));

    Fl_run();

    return 0;
}
