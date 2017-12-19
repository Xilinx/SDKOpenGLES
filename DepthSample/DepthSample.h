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
 * @file Depth.h
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

#ifndef FETCH_DEPTH_H
#define FETCH_DEPTH_H

#define GLES_VERSION 2
#include <GLES2/gl2.h>
#include <string>


using namespace std;

string resourceDirectory              = "Supportingfiles/";
string vertexShaderCubeFilename       = "DepthSamplebox.vert";
string fragmentShaderCubeFilename     = "DepthSamplebox.frag";
string vertexShaderGlassFilename      = "DepthSamplevertical.vert";
string fragmentShaderGlassFilename    = "DepthSamplevertical.frag";
string fragmentShaderGlassRttFilename = "DepthSampleverticalExt.frag";

GLuint programCubeID;
GLint iLocCubePosition;
GLint iLocCubeColor;
GLint iLocCubeMVP;

GLuint programGlassID;
GLint iLocGlassPosition;
GLint iLocGlassColor;
GLint iLocGlassMVP;
GLint iLocNear;
GLint iLocFar;
GLint iLocGlassMaterial;

int windowWidth = -1;
int windowHeight = -1;


const float colorsCube[] =
{
    1.0, 0.0, 0.0, /* red */
    1.0, 0.0, 0.0, /* red */
    1.0, 0.0, 0.0, /* red */
    1.0, 0.0, 0.0, /* red */
    1.0, 0.0, 0.0, /* red */
    1.0, 0.0, 0.0, /* red */

    0.0, 1.0, 1.0, /* blue */
    0.0, 1.0, 1.0, /* blue */
    0.0, 1.0, 1.0, /* blue */
    0.0, 0.0, 1.0, /* blue */
    0.0, 0.0, 1.0, /* blue */
    0.0, 0.0, 1.0, /* blue */

    1.0, 1.0, 1.0, /* white */
    1.0, 1.0, 1.0, /* white */
    1.0, 1.0, 1.0, /* white */
    1.0, 1.0, 1.0, /* white */
    1.0, 1.0, 1.0, /* white */
    1.0, 1.0, 1.0, /* white */

    1.0, 1.0, 0.0, /* yellow */
    1.0, 1.0, 0.0, /* yellow */
    1.0, 1.0, 0.0, /* yellow */
    1.0, 1.0, 0.0, /* yellow */
    1.0, 1.0, 0.0, /* yellow */
    1.0, 1.0, 0.0, /* yellow */

    1.0, 0.0, 1.0, /* cyan */
    1.0, 0.0, 1.0, /* cyan */
    1.0, 0.0, 1.0, /* cyan */
    0.0, 1.0, 1.0, /* cyan */
    0.0, 1.0, 1.0, /* cyan */
    0.0, 1.0, 1.0, /* cyan */

    1.0, 0.2, 1.0, /* magenta */
    1.0, 0.2, 1.0, /* magenta */
    1.0, 0.2, 1.0, /* magenta */
    1.0, 0.2, 1.0, /* magenta */
    1.0, 0.2, 1.0, /* magenta */
    1.0, 0.2, 1.0, /* magenta */
};


const float verticesGlass[] =
{
    0.3, -0.5,  0,
    0.3,  0.8,  0,
    -0.3, -0.5,  0,
    -0.3,  0.8,  0,
};

const float verticesCube[] =
{
    -0.5,  0.5, -0.5,
    0.5, -0.5, -0.5,
    -0.5, -0.5, -0.5,
    -0.5,  0.5, -0.5,
    0.5,  0.5, -0.5,
    0.5, -0.5, -0.5,
    -0.5,  0.5,  0.5,
    -0.5, -0.5, -0.5,
    -0.5, -0.5,  0.5,
    -0.5,  0.5,  0.5,
    -0.5,  0.5, -0.5,
    -0.5, -0.5, -0.5,
    -0.5,  0.5,  0.5,
    0.5,  0.5, -0.5,
    -0.5,  0.5, -0.5,
    -0.5,  0.5,  0.5,
    0.5,  0.5,  0.5,
    0.5,  0.5, -0.5,
    0.5,  0.5, -0.5,
    0.5, -0.5,  0.5,
    0.5, -0.5, -0.5,
    0.5,  0.5, -0.5,
    0.5,  0.5,  0.5,
    0.5, -0.5,  0.5,
    0.5,  0.5,  0.5,
    -0.5, -0.5,  0.5,
    0.5, -0.5,  0.5,
    0.5,  0.5,  0.5,
    -0.5,  0.5,  0.5,
    -0.5, -0.5,  0.5,
    -0.5, -0.5, -0.5,
    0.5, -0.5,  0.5,
    -0.5, -0.5,  0.5,
    -0.5, -0.5, -0.5,
    0.5, -0.5, -0.5,
    0.5, -0.5,  0.5,
};

const float colorsGlass[] =
{
    0.2, 1.0, 0.0, 0.2, /* green */
    0.2, 1.0, 0.0, 0.2, /* green */
    1.0, 0.0, 0.0, 0.2, /* green */
    1.0, 0.0, 0.0, 0.2, /* green */
};



#endif /* FETCH_DEPTH_H */
