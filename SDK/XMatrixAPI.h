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
 * @file XMatrixAPI.h
 *
 * This file implements all the functions related to Matrix calculations for application.
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

#ifndef MATRIX_H
#define MATRIX_H


#include "XCVector.h"

class XMatrixAPI
    {
    private: 
        float elements[16];
        static XMatrixAPI multiply(XMatrixAPI *left, XMatrixAPI *right);
        static const float identityArray[];
    public:       
        float* getAsArray(void);
   	XMatrixAPI(void);
        float& operator[] (unsigned element);
        XMatrixAPI operator* (XMatrixAPI right);
        XMatrixAPI& operator=(const XMatrixAPI &another);
        XMatrixAPI(const float* array);
        static XMatrixAPI identityXMatrixAPI;
        static CVec4f vertexTransform(CVec4f *vector, XMatrixAPI *XMatrixAPI);
        static CVec3f vertexTransform(CVec3f *vector, XMatrixAPI *XMatrixAPI);
        static void XMatrixAPITranspose(XMatrixAPI *XMatrixAPI);
        static XMatrixAPI createRotationX(float angle);
        static XMatrixAPI createRotationY(float angle);
        static XMatrixAPI createRotationZ(float angle);
        static XMatrixAPI createTranslation(float x, float y, float z);
        static XMatrixAPI createScaling(float x, float y, float z);
        void print(void);
        static XMatrixAPI XMatrixAPIPerspective(float FOV, float ratio, float zNear, float zFar);
	static XMatrixAPI XMatrixAPICameraLookAt(CVec3f eye, CVec3f center, CVec3f up); 
        static XMatrixAPI XMatrixAPIOrthographic(float left, float right, float bottom, float top, float zNear, float zFar);
        static XMatrixAPI XMatrixAPIInvert(XMatrixAPI *XMatrixAPI);
        static float XMatrixAPIDeterminant(float *XMatrixAPI);
        static float XMatrixAPIDeterminant(XMatrixAPI *XMatrixAPI);
        static XMatrixAPI XMatrixAPIScale(XMatrixAPI *XMatrixAPI, float scale);
   	
     };

#endif
