/*******************************************************************************
 *
 * Copyright (C) 2017 Xilinx, Inc.  All rights reserved.
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
 * @file BasicCube.h
 *
 * This is an test example for OpenGL rendering using this SDK.
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
/******************************* Header Files ********************************/


#define GLES_VERSION 2

#include <GLES2/gl2.h>


/* Simple triangle. */
const float cubeVertices[] =
{

    -0.7,  0.7, -0.7,
    0.7, -0.7, -0.7,
    -0.7, -0.7, -0.7,
    -0.7,  0.7, -0.7,
    0.7,  0.7, -0.7,
    0.7, -0.7, -0.7,
    -0.7,  0.7,  0.7,
    -0.7, -0.7, -0.7,
    -0.7, -0.7,  0.7,
    -0.7,  0.7,  0.7,
    -0.7,  0.7, -0.7,
    -0.7, -0.7, -0.7,
    -0.7,  0.7,  0.7,
    0.7,  0.7, -0.7,
    -0.7,  0.7, -0.7,
    -0.7,  0.7,  0.7,
    0.7,  0.7,  0.7,
    0.7,  0.7, -0.7,
    0.7,  0.7, -0.7,
    0.7, -0.7,  0.7,
    0.7, -0.7, -0.7,
    0.7,  0.7, -0.7,
    0.7,  0.7,  0.7,
    0.7, -0.7,  0.7,
    0.7,  0.7,  0.7,
    -0.7, -0.7,  0.7,
    0.7, -0.7,  0.7,
    0.7,  0.7,  0.7,
    -0.7,  0.7,  0.7,
    -0.7, -0.7,  0.7,
    -0.7, -0.7, -0.7,
    0.7, -0.7,  0.7,
    -0.7, -0.7,  0.7,
    -0.7, -0.7, -0.7,
    0.7, -0.7, -0.7,
    0.7, -0.7,  0.7,


};

/* Per corner colors for the triangle (Red, Green, Green). */
const float cubeColors[] =
{
    1.0, 0.0, 0.0, /* red */
    1.0, 0.0, 1.0, /* blue */
    1.0, 0.0, 0.0, /* green */
    1.0, 0.0, 0.0, /* red */
    1.0, 0.0, 0.0, /* yellow */
    1.0, 0.0, 1.0, /* blue */
    1.0, 1.0, 0.0, /* white */
    1.0, 1.0, 0.0, /* green */
    1.0, 1.0, 0.0, /* cyan */
    1.0, 1.0, 0.0, /* white */
    1.0, 1.0, 0.0, /* red */
    1.0, 1.0, 0.0, /* green */
    1.0, 1.0, 1.0, /* white */
    1.0, 1.0, 0.0, /* yellow */
    1.0, 0.0, 0.0, /* red */
    1.0, 1.0, 1.0, /* white */
    0.0, 0.0, 0.0, /* black */
    1.0, 1.0, 0.0, /* yellow */
    1.0, 1.0, 0.0, /* yellow */
    1.0, 0.0, 1.0, /* magenta */
    0.0, 0.0, 1.0, /* blue */
    1.0, 1.0, 0.0, /* yellow */
    0.0, 0.0, 0.0, /* black */
    1.0, 0.0, 1.0, /* magenta */
    0.0, 0.0, 0.0, /* black */
    0.0, 1.0, 1.0, /* cyan */
    1.0, 0.0, 1.0, /* magenta */
    0.0, 0.0, 0.0, /* black */
    1.0, 1.0, 1.0, /* white */
    0.0, 1.0, 1.0, /* cyan */
    0.0, 1.0, 0.0, /* green */
    1.0, 0.0, 1.0, /* magenta */
    0.0, 1.0, 1.0, /* cyan */
    0.0, 1.0, 0.0, /* green */
    0.0, 0.0, 1.0, /* blue */
    1.0, 0.0, 1.0, /* magenta */

};
                             
