/*******************************************************************************
 *
 * Copyright (C) 2023 Xilinx, Inc.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * Use of the Software is limited solely to applications:
 * (a) running on a Xilinx device, or
 * (b) that interact with a Xilinx device through a bus or interconnect.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * XILINX CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Except as contained in this notice, the name of the Xilinx shall not be used
 * in advertising or otherwise to promote the sale, use or other dealings in
 * this Software without prior written authorization from Xilinx.
 *
*******************************************************************************/
/******************************************************************************/
/**
 *
 * @file XLinuxPodium.cpp
 *
 * This file implements all the functions related to Linux X11 windowing system API's for application.
 *
 * @note        None.
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   Who            Date            Changes
 * ----- ----           --------        -----------------------------------------------
 * 1.0   Alok G         10/06/17        Initial release.
 * </pre>
 *
*******************************************************************************/
/******************************* Source Files ********************************/


#include "XLinuxPodium.h"

    XPodium* XLinuxPodium::instance = NULL;

    XLinuxPodium::XLinuxPodium(void)
    {

    }

    XPodium* XLinuxPodium::getHandler(void)
    {
        if (instance == NULL)
        {
            instance = new XLinuxPodium();
        }
        return instance;
    }

    void XLinuxPodium::prepareWindow(int width, int height)
    {
        windowWidth = width;
        windowHeight = height;
    }

    bool XLinuxPodium::createX11Window(void)
    {
        XSetWindowAttributes windowAttributes;
        XSizeHints sizeHints;
        XEvent event;
        XVisualInfo visualInfoTemplate;

        unsigned long mask;
        long screen;

        int visualID, numberOfVisuals;

        display = XOpenDisplay(NULL);

        screen = DefaultScreen(display);

        eglGetConfigAttrib(CoreEGL::display, CoreEGL::config, EGL_NATIVE_VISUAL_ID, &visualID);
        visualInfoTemplate.visualid = visualID;
        visual = XGetVisualInfo(display, VisualIDMask, &visualInfoTemplate, &numberOfVisuals);

        if (visual == NULL)
        {
            printf("Couldn't get X visual info\n");
            return false;
        }

        colormap = XCreateColormap(display, RootWindow(display, screen), visual->visual, AllocNone);

        windowAttributes.colormap = colormap;
        windowAttributes.background_pixel = 0xFFFFFFFF;
        windowAttributes.border_pixel = 0;
        windowAttributes.event_mask = StructureNotifyMask | ExposureMask;

        mask = CWBackPixel | CWBorderPixel | CWEventMask | CWColormap;

        window = XCreateWindow(display, RootWindow(display, screen), 0, 0, windowWidth, windowHeight,
            0, visual->depth, InputOutput, visual->visual, mask, &windowAttributes);
        sizeHints.flags = USPosition;
        sizeHints.x = 10;
        sizeHints.y = 10;

        XSetStandardProperties(display, window, "Xilinx OpenGL ES SDK", "", None, 0, 0, &sizeHints);
        XMapWindow(display, window);
        XIfEvent(display, &event, wait_for_map, (char*)&window);
        XSetWMColormapWindows(display, window, &window, 1);
        XFlush(display);

        XSelectInput(display, window, KeyPressMask | ExposureMask | EnterWindowMask
                     | LeaveWindowMask | PointerMotionMask | VisibilityChangeMask | ButtonPressMask
                     | ButtonReleaseMask | StructureNotifyMask);

        return true;
    }

    void XLinuxPodium::destroyWindow(void)
    {
        XDestroyWindow(display, window);
        XFreeColormap(display, colormap);
        XFree(visual);
        XCloseDisplay(display);
    }

    XPodium::WindowStatus XLinuxPodium::checkWindow(void)
    {
        XEvent event;

        while (XPending(display) > 0) 
        {
            XNextEvent(display, &event);

            if (event.type == ButtonPress)
            {
                return XPodium::WINDOW_CLICK;
            }
        }
        return XPodium::WINDOW_IDLE;
    }

    Bool XLinuxPodium::wait_for_map(Display *display, XEvent *event, char *windowPointer) 
    {
        return (event->type == MapNotify && event->xmap.window == (*((Window*)windowPointer)));
    }
