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
 * @file TextureMipMap.h
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


#ifndef ETCMIPMAP_H
#define ETCMIPMAP_H

#define GLES_VERSION 2

#include <GLES2/gl2.h>
#include <string>

using namespace std;


string resourceDirectory = "Supportingfiles/";
string textureFilename = "xilinx_mip_";
string imageExtension = ".pkm";

string vertexShaderFilename = "textureMipMap.vert";
string fragmentShaderFilename = "textureMipMap.frag";

GLuint textureID = 0;

GLuint vertexShaderID = 0;
GLuint fragmentShaderID = 0;
GLuint programID = 0;
GLint iLocPosition = -1;
GLint iLocTexCoord = -1;
GLint iLocSampler = -1;

static const GLubyte indices[] =
{
     0,  1,  2,  3,    
     3,  4,
     4,  5,  6,  7,
     7,  8,
     8,  9, 10, 11,
    11, 12,
    12, 13, 14, 15,
    15, 16,
    16, 17, 18, 19,   
    19, 20,   
    20, 21, 22, 23,  
    23, 24,
    24, 25, 26, 27,   
    27, 28,   
    28, 29, 30, 31,   
    31, 32,
    32, 33, 34, 35,
};


static const GLfloat textureCoordinates[] =
{
    0.0f, 0.0f, /* 0 */
    0.0f, 1.0f, /* 1 */
    1.0f, 0.0f, /* 2 */
    1.0f, 1.0f, /* 3 */
    0.0f, 0.0f, /* 4 */
    0.0f, 1.0f, /* 5 */
    1.0f, 0.0f, /* 6 */
    1.0f, 1.0f, /* 7 */
    0.0f, 0.0f, /* 8 */
    0.0f, 1.0f, /* 9 */
    1.0f, 0.0f, /* 10 */
    1.0f, 1.0f, /* 11 */
    0.0f, 0.0f, /* 12 */
    0.0f, 1.0f, /* 13 */
    1.0f, 0.0f, /* 14 */
    1.0f, 1.0f, /* 15 */
    0.0f, 0.0f, /* 16 */
    0.0f, 1.0f, /* 17 */
    1.0f, 0.0f, /* 18 */
    1.0f, 1.0f, /* 19 */
    0.0f, 0.0f, /* 20 */
    0.0f, 1.0f, /* 21 */
    1.0f, 0.0f, /* 22 */
    1.0f, 1.0f, /* 23 */
    0.0f, 0.0f, /* 24 */
    0.0f, 1.0f, /* 25 */
    1.0f, 0.0f, /* 26 */
    1.0f, 1.0f, /* 27 */
    0.0f, 0.0f, /* 28 */
    0.0f, 1.0f, /* 29 */
    1.0f, 0.0f, /* 30 */
    1.0f, 1.0f, /* 31 */
    0.0f, 0.0f, /* 32 */
    0.0f, 1.0f, /* 33 */
    1.0f, 0.0f, /* 34 */
    1.0f, 1.0f, /* 35 */
};


static const GLfloat vertices[] =
{
    -1.000000f,  1.0000f, 0.0f, /* 0 */
    -1.000000f,  0.4666f, 0.0f, /* 1 */
    -0.200000f,  1.0000f, 0.0f, /* 2 */
    -0.200000f,  0.4666f, 0.0f, /* 3 */
    -0.200000f,  1.0000f, 0.0f, /* 4 */
    -0.200000f,  0.7333f, 0.0f, /* 5 */
     0.200000f,  1.0000f, 0.0f, /* 6 */
     0.200000f,  0.7333f, 0.0f, /* 7 */
     0.200000f,  1.0000f, 0.0f, /* 8 */
     0.200000f,  0.8666f, 0.0f, /* 9 */
     0.400000f,  1.0000f, 0.0f, /* 10 */
     0.400000f,  0.8666f, 0.0f, /* 11 */
     0.400000f,  1.0000f, 0.0f, /* 12 */
     0.400000f,  0.9333f, 0.0f, /* 13 */
     0.500000f,  1.0000f, 0.0f, /* 14 */
     0.500000f,  0.9333f, 0.0f, /* 15 */
     0.500000f,  1.0000f, 0.0f, /* 16 */
     0.500000f,  0.9666f, 0.0f, /* 17 */
     0.550000f,  1.0000f, 0.0f, /* 18 */
     0.550000f,  0.9666f, 0.0f, /* 19 */
     0.550000f,  1.0000f, 0.0f, /* 20 */
     0.550000f,  0.9833f, 0.0f, /* 21 */
     0.575000f,  1.0000f, 0.0f, /* 22 */
     0.575000f,  0.9833f, 0.0f, /* 23 */
     0.575000f,  1.0000f, 0.0f, /* 24 */
     0.575000f,  0.9916f, 0.0f, /* 25 */
     0.587500f,  1.0000f, 0.0f, /* 26 */
     0.587500f,  0.9916f, 0.0f, /* 27 */
     0.587500f,  1.0000f, 0.0f, /* 28 */
     0.587500f,  0.9958f, 0.0f, /* 29 */
     0.593750f,  1.0000f, 0.0f, /* 30 */
     0.593750f,  0.9958f, 0.0f, /* 31 */
     0.593750f,  1.0000f, 0.0f, /* 32 */
     0.593750f,  0.9958f, 0.0f, /* 33 */
     0.596875f,  1.0000f, 0.0f, /* 34 */
     0.596875f,  0.9958f, 0.0f, /* 35 */
};

#endif /* ETCMIPMAP_H */
