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
 * @file XTexture.h
 *
 * This file implements all the functions related to texture compression techniques.
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



#ifndef XTEXTURE_H
#define XTEXTURE_H

#include "XCompressionTexture.h"

#include <GLES2/gl2.h>

    class XTexture
    {
    private:
        static void XgetCompressedTextureFormats(GLint **textureFormats, int* numberOfTextureFormats);
    public:
        static bool XisETCSupported(bool verbose = false);
   	static void XcreateTexture(unsigned int width, unsigned int height, GLvoid **textureData);
	static void XcreateTexture(unsigned int width, unsigned int height, unsigned int red, GLvoid **textureData);
        static void XcreateTexture(unsigned int width, unsigned int height, short red, short **textureData);
        static void XdeleteTextureData(GLvoid **textureData);
        static void XloadData(const char *filename, unsigned char **textureData);
        static void XloadPKMData(const char *filename, ETCHeaderforTexture* etcHeader, unsigned char **textureData);
        static void XloadCompressedMipmaps(const char *filenameBase, const char *filenameSuffix, GLuint *textureID);
        static void XreversePixelLine(float* destination, const float* source, int lineWidth);
    };
#endif /* TEXTURE_H */
