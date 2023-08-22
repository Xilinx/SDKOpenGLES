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
 * @file Xvector.h
 *
 * This file implements all basic Vector types
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


#ifndef XVECTOR_H
#define XVECTOR_H

#include <cmath>


    class CVec2
    {
    public:
        int x, y;
    };


    class CVec3
    {
    public:
        int x, y, z;
    };

    class CVec4
    {
    public:
        int x, y, z, w;
    };

    class CVec2f
    {
    public:
        float x, y;
    };

    class CVec3f
    {
    public:
        float x, y, z;

        void normalize(void)
        {
            float length = sqrt(x * x + y * y + z * z);

            x /= length;
            y /= length;
            z /= length;
        }

        static CVec3f cross(const CVec3f& vector1, const CVec3f& vector2)
	    {
		    CVec3f crossProduct;

		    crossProduct.x = (vector1.y * vector2.z) - (vector1.z * vector2.y);
		    crossProduct.y = (vector1.z * vector2.x) - (vector1.x * vector2.z);
		    crossProduct.z = (vector1.x * vector2.y) - (vector1.y * vector2.x);

		    return crossProduct;
	    }

 
       static float dot(CVec3f& vector1, CVec3f& vector2)
        {
            return (vector1.x * vector2.x + vector1.y * vector2.y + vector1.z * vector2.z);
        }
    };


    class CVec4f
    {
    public:
        float x, y, z, w;

        void normalize(void)
        {
            float length = sqrt(x * x + y * y + z * z + w * w);

            x /= length;
            y /= length;
            z /= length;
            w /= length;
        }
    };
#endif /* XVECTOR_H */

