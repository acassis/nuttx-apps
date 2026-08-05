/****************************************************************************
 * apps/examples/cfltk_double_buffer_demo/cfltk_double_buffer_demo_main.c
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
#include <cfltk/Fl_Double_Window.h>
#include <cfltk/Fl_Group.h>
#include <cfltk/Fl_Window.h>

/****************************************************************************
 * Private Data
 ****************************************************************************/

static Fl_Widget *g_win;
static Fl_Widget *g_counter_box;
static int g_clicks;

/* Alternates the window between two sizes on every click, forcing
 * fl_backend_window_reshape()/fl_backend_window_flush() in
 * cfltk/src/backend/nx/fl_nx_window.c to regenerate the offscreen
 * buffer (resize_offscreen()) rather than only exercising the
 * create-once path a static-size window would. */
static const int k_widths[2]  = { 220, 300 };
static const int k_heights[2] = { 140, 190 };

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void on_close(Fl_Widget *w, void *data) {
    (void)data;
    Fl_Window_hide(w);
}

static void on_click(Fl_Widget *w, void *data) {
    char buf[32];
    int slot;
    (void)w;
    (void)data;

    g_clicks++;
    slot = g_clicks & 1;
    printf("clicks=%d size=%dx%d\n", g_clicks, k_widths[slot], k_heights[slot]);

    snprintf(buf, sizeof(buf), "Clicks: %d", g_clicks);
    Fl_Widget_copy_label(g_counter_box, buf);
    Fl_Widget_redraw(g_counter_box);

    Fl_Window_resize(g_win, FL_WIDGET(g_win)->x, FL_WIDGET(g_win)->y,
                      k_widths[slot], k_heights[slot]);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, FAR char *argv[]) {
    Fl_Double_Window *win;
    Fl_Button *btn;

    (void)argc;
    (void)argv;

    /* The only difference from cfltk_button_demo: Fl_Double_Window_new()
     * instead of Fl_Window_new() -- exercises fl_nx_window.c's
     * offscreen-buffered draw/flush path (resize_offscreen(), the
     * per-flush BitBlt() from the memory DC onto the real window DC)
     * and fl_nx_offscreen.c's Fl_Offscreen create/delete/begin/end/copy,
     * neither of which any other cfltk_* demo reaches. */
    win = Fl_Double_Window_new(20, 20, k_widths[0], k_heights[0],
                                "cfltk_double_buffer_demo");
    g_win = FL_WIDGET(win);

    g_counter_box = FL_WIDGET(Fl_Box_new(10, 10, k_widths[0] - 20, 30, "Clicks: 0"));

    btn = Fl_Button_new(10, 50, k_widths[0] - 20, 40, "Click me (resizes too)");
    Fl_Widget_set_callback(FL_WIDGET(btn), on_click, NULL);

    Fl_Group_end(FL_GROUP(g_win));

    Fl_Widget_set_callback(g_win, on_close, NULL);
    Fl_Window_show(g_win);

    Fl_run();

    return 0;
}
