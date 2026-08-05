/****************************************************************************
 * apps/examples/cfltk_image_demo/cfltk_image_demo_main.c
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
#include <cfltk/Fl_Bitmap.h>
#include <cfltk/Fl_Box.h>
#include <cfltk/Fl_Group.h>
#include <cfltk/Fl_Pixmap.h>
#include <cfltk/Fl_RGB_Image.h>
#include <cfltk/Fl_Window.h>
#include <cfltk/fl_draw.h>

/****************************************************************************
 * Private Data
 ****************************************************************************/

/* Exercises d_draw_image() directly: a plain 40x40 RGB buffer, no
 * transparency, drawn straight into an Fl_Box via label.image. */
#define GRAD_W 40
#define GRAD_H 40
static unsigned char g_gradient[GRAD_H][GRAD_W][3];

/* Exercises d_draw_bitmask(): a 16x16 XBM-style 1-bit checkerboard,
 * (16+7)/8 = 2 bytes/row, LSB-first. */
#define BITMAP_W 16
#define BITMAP_H 16
static unsigned char g_checker[BITMAP_H][2];

/* Exercises d_read_image()+d_draw_image() together via
 * Fl_Pixmap.c's mask_composite(): a 16x16 XPM diamond, "." transparent
 * ("None"), "#" opaque red -- the transparent pixels must show
 * whatever was already drawn underneath (the window's own background
 * fill), not a solid color, proving the read-back actually read real
 * pixels rather than returning zeroed/garbage data. */
static const char *const g_diamond_xpm[] = {
    "16 16 2 1",
    ". c None",
    "# c #ff0000",
    "................",
    ".......##.......",
    "......####......",
    ".....######.....",
    "....########....",
    "...##########...",
    "..############..",
    ".##############.",
    ".##############.",
    "..############..",
    "...##########...",
    "....########....",
    ".....######.....",
    "......####......",
    ".......##.......",
    "................",
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void build_gradient(void) {
    int x, y;
    for (y = 0; y < GRAD_H; y++) {
        for (x = 0; x < GRAD_W; x++) {
            g_gradient[y][x][0] = (unsigned char)(x * 255 / (GRAD_W - 1));
            g_gradient[y][x][1] = (unsigned char)(y * 255 / (GRAD_H - 1));
            g_gradient[y][x][2] = 64;
        }
    }
}

static void build_checker(void) {
    int row, col;
    for (row = 0; row < BITMAP_H; row++) {
        unsigned int bits = 0;
        for (col = 0; col < BITMAP_W; col++) {
            if (((row / 2) + (col / 2)) & 1) bits |= (1u << col);
        }
        g_checker[row][0] = (unsigned char)(bits & 0xff);
        g_checker[row][1] = (unsigned char)((bits >> 8) & 0xff);
    }
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, FAR char *argv[]) {
    Fl_Window *win;
    Fl_Box *bg, *grad_box, *diamond_box, *checker_box;
    Fl_RGB_Image *rgb_img;
    Fl_Pixmap *pixmap_img;
    Fl_Bitmap *bitmap_img;

    (void)argc;
    (void)argv;

    build_gradient();
    build_checker();

    win = Fl_Window_new(20, 20, 220, 200, "cfltk_image_demo");

    /* Non-flat background color so mask_composite()'s read-back has
     * something distinctive to prove it actually sampled: a teal
     * flat-box covering the window. */
    bg = Fl_Box_new(0, 0, 220, 200, NULL);
    Fl_Widget_set_box(FL_WIDGET(bg), FL_FLAT_BOX);
    Fl_Widget_set_color(FL_WIDGET(bg), fl_rgb_color(0, 128, 128));

    rgb_img = Fl_RGB_Image_new((const unsigned char *)g_gradient, GRAD_W, GRAD_H, 3, 0);
    grad_box = Fl_Box_new(10, 10, GRAD_W, GRAD_H, NULL);
    Fl_Widget_set_image(FL_WIDGET(grad_box), (Fl_Image *)rgb_img);

    pixmap_img = Fl_Pixmap_new(g_diamond_xpm);
    diamond_box = Fl_Box_new(90, 10, 16, 16, NULL);
    Fl_Widget_set_image(FL_WIDGET(diamond_box), (Fl_Image *)pixmap_img);

    bitmap_img = Fl_Bitmap_new((const unsigned char *)g_checker, BITMAP_W, BITMAP_H);
    checker_box = Fl_Box_new(150, 10, BITMAP_W, BITMAP_H, NULL);
    Fl_Widget_set_image(FL_WIDGET(checker_box), (Fl_Image *)bitmap_img);

    Fl_Group_end(FL_GROUP(win));
    Fl_Window_show(FL_WIDGET(win));

    Fl_run();

    return 0;
}
