/****************************************************************************
 * apps/examples/cfltk_dnd_demo/cfltk_dnd_demo_main.c
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
#include <stdlib.h>
#include <string.h>

#include <cfltk/Fl.h>
#include <cfltk/Fl_Box.h>
#include <cfltk/Fl_Group.h>
#include <cfltk/Fl_Window.h>
#include <cfltk/fl_draw.h>

/****************************************************************************
 * Private Types
 ****************************************************************************/

/* cfltk's C API has no per-instance "override this widget's event
 * handling" call (real subclassing, as upstream C++ FLTK would use for
 * a custom drag source/target, isn't reachable from C) -- but
 * Fl_Widget_init() itself takes an arbitrary Fl_WidgetOps vtable (every
 * concrete widget, e.g. Fl_Box_init()/fl_box_ops, is already built this
 * way), which is public. So this demo builds its own minimal ones,
 * reusing Fl_Box_draw() for the visual (a plain box+label, nothing
 * DND-specific about how it looks) and supplying a custom handle().
 */
typedef struct { Fl_Widget widget; } DragSrc;
typedef struct { Fl_Widget widget; } DropZone;

/* Button's original size, so it can be moved (not resized) into the
 * drop zone -- kept separate from the zone's own, deliberately larger,
 * dimensions. */
#define BTN_W 90
#define BTN_H 40
#define BTN_X0 10
#define BTN_Y0 25

/* Drop zone: proportionally larger than the button on every side, so
 * "the button now sits inside a visibly bigger rectangle" is
 * unambiguous at a glance -- not just "moved somewhere". */
#define ZONE_X 130
#define ZONE_Y 15
#define ZONE_W (BTN_W + 30)
#define ZONE_H (BTN_H + 30)

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const char k_drag_text[] = "Hello DND";
static DragSrc *g_src;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int dragsrc_handle(Fl_Widget *self, int event) {
    (void)self;
    if (event == FL_PUSH) {
        /* Fl_dnd() (Fl.c) reads its payload from clipboard buffer 0 --
         * matches upstream's own Fl::dnd() contract, no separate
         * "set drag payload" call exists. The payload itself isn't
         * the point of this demo (see droptarget_handle() below, which
         * actually moves the button on a successful drop) but the
         * backend still requires *something* to copy/paste through
         * the exact same path a real drag-and-drop payload would use. */
        Fl_copy(k_drag_text, (int)strlen(k_drag_text), 0);
        printf("drag started, dropped=%d\n", Fl_dnd());
        return 1;
    }
    return 0;
}

static const Fl_WidgetOps dragsrc_ops = {
    Fl_Box_draw, dragsrc_handle,
    NULL, /* resize: Fl_Widget's default */
    NULL, /* show: Fl_Widget's default */
    NULL, /* hide: Fl_Widget's default */
    Fl_Widget_base_destroy,
    NULL, /* as_group */
    NULL  /* as_window */
};

static DragSrc *DragSrc_new(int x, int y, int w, int h, const char *label) {
    DragSrc *self = (DragSrc *)malloc(sizeof(DragSrc));
    Fl_Widget_init(&self->widget, &dragsrc_ops, x, y, w, h, label);
    Fl_Widget_set_box(&self->widget, FL_UP_BOX);
    return self;
}

/* The actual pass/fail signal for this demo: a plain click on DragSrc
 * (no real drag) never dispatches FL_DND_ENTER/FL_PASTE here at all
 * (Fl_belowmouse() stays on DragSrc itself the whole time, so nothing
 * in the drop zone's rect is ever hit-tested) -- only a real
 * press-move-release drag that ends with the pointer inside this
 * zone's rectangle does. On that drop, DragSrc is physically moved
 * (Fl_Widget_resize(), same x/y semantics as a real window move) to
 * sit centered inside the zone -- something a click alone can never
 * produce, and something visible on screen, not just in console
 * output. FL_DND_ENTER/FL_DND_LEAVE also highlight/revert the zone's
 * own color so the moment of crossing into it is visible mid-drag,
 * before the drop even completes. */
static int dropzone_handle(Fl_Widget *self_w, int event) {
    switch (event) {
        case FL_DND_ENTER:
            Fl_Widget_set_color(self_w, FL_YELLOW);
            Fl_Widget_redraw(self_w);
            return 1;
        case FL_DND_LEAVE:
            Fl_Widget_set_color(self_w, FL_GRAY);
            Fl_Widget_redraw(self_w);
            return 1;
        case FL_DND_DRAG:
            return 1;
        case FL_PASTE: {
            Fl_Widget *btn = FL_WIDGET(g_src);
            int new_x = ZONE_X + (ZONE_W - BTN_W) / 2;
            int new_y = ZONE_Y + (ZONE_H - BTN_H) / 2;
            Fl_Widget_resize(btn, new_x, new_y, BTN_W, BTN_H);
            Fl_Widget_redraw(btn);
            Fl_Widget_set_color(self_w, FL_GREEN);
            Fl_Widget_redraw(self_w);
            printf("dropped inside zone, button moved to (%d,%d)\n", new_x, new_y);
            return 1;
        }
        default:
            return 0;
    }
}

static const Fl_WidgetOps dropzone_ops = {
    Fl_Box_draw, dropzone_handle,
    NULL, /* resize: Fl_Widget's default */
    NULL, /* show: Fl_Widget's default */
    NULL, /* hide: Fl_Widget's default */
    Fl_Widget_base_destroy,
    NULL, /* as_group */
    NULL  /* as_window */
};

static DropZone *DropZone_new(int x, int y, int w, int h) {
    DropZone *self = (DropZone *)malloc(sizeof(DropZone));
    Fl_Widget_init(&self->widget, &dropzone_ops, x, y, w, h, "Drop Zone");
    Fl_Widget_set_box(&self->widget, FL_BORDER_BOX);
    Fl_Widget_set_color(&self->widget, FL_GRAY);
    return self;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, FAR char *argv[]) {
    Fl_Window *win;

    (void)argc;
    (void)argv;

    win = Fl_Window_new(20, 20, 260, 90, "cfltk_dnd_demo");

    /* Drop zone is added (and thus drawn/hit-tested) first, so the
     * button -- added after -- stacks visually on top of it if their
     * areas ever overlap after a move, same z-order convention any
     * other cfltk group uses. */
    DropZone_new(ZONE_X, ZONE_Y, ZONE_W, ZONE_H);
    g_src = DragSrc_new(BTN_X0, BTN_Y0, BTN_W, BTN_H, "Drag me");

    Fl_Group_end(FL_GROUP(win));
    Fl_Window_show(FL_WIDGET(win));

    Fl_run();

    return 0;
}
