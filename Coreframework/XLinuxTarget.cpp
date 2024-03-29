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
 * @file XLinuxMali.cpp
 *
 * This file implements all the functions related to Linux XPodium class for application.
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



#include <cstdlib>
#include <sys/fcntl.h>
#include <sys/unistd.h>

#include "XLinuxTarget.h"

    XPodium* XLinuxMali::instance = NULL;

    XLinuxMali::XLinuxMali(void)
    {

    }

    XPodium* XLinuxMali::getHandler(void)
    {
        if (instance == NULL)
        {
            instance = new XLinuxMali();
        }
        return instance;
    }

    void XLinuxMali::prepareWindow(int width, int height)
    {
        Fwindow = (fbdev_window *)calloc(1, sizeof(fbdev_window));
        if(Fwindow == NULL)
        {
            printf("Out of memory at %s:%i\n", __FILE__, __LINE__);
            exit(1);
        }
        Fwindow->width = width;
        Fwindow->height = height;
    }

    void XLinuxMali::destroyWindow(void)
    {
        free(Fwindow);
    }

    XPodium::WindowStatus XLinuxMali::checkWindow(void)
    {
        return XPodium::WINDOW_IDLE;
    }
