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
 * @file XMatrixAPI.cpp
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
/******************************* Source Files ********************************/



#include "XMatrixAPI.h"
#include "XMathematics.h"
#include "XPodium.h"
#include <cmath>
#include <cstring>
#include <cstdio>
#include <cstdlib>

    const float XMatrixAPI::identityArray[16] =
    {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    };

    XMatrixAPI XMatrixAPI::identityXMatrixAPI = XMatrixAPI(identityArray);

    XMatrixAPI::XMatrixAPI(const float* array)
    {
        memcpy(elements, array, 16 * sizeof(float));
    }

    XMatrixAPI::XMatrixAPI(void)
    {
    }

    float& XMatrixAPI::operator[] (unsigned element)
    { 
        if (element > 15)
        {
            printf("XMatrixAPI only has 16 elements, tried to access element %d", element);
            exit(1);
        } 
        return elements[element]; 
    }

    XMatrixAPI XMatrixAPI::operator* (XMatrixAPI right)
    {
        return multiply(this, &right);
    }

    XMatrixAPI& XMatrixAPI::operator= (const XMatrixAPI &another)
    {
        if(this != &another)
        {
            memcpy(this->elements, another.elements, 16 * sizeof(float));
        }

        return *this;
    }

    float* XMatrixAPI::getAsArray(void)
    {
        return elements;
    }

    float XMatrixAPI::XMatrixAPIDeterminant(float *XMatrixAPI)
    {
        float result = 0.0f;

        result  = XMatrixAPI[0] * (XMatrixAPI[4] * XMatrixAPI[8] - XMatrixAPI[7] * XMatrixAPI[5]);
        result -= XMatrixAPI[3] * (XMatrixAPI[1] * XMatrixAPI[8] - XMatrixAPI[7] * XMatrixAPI[2]);
        result += XMatrixAPI[6] * (XMatrixAPI[1] * XMatrixAPI[5] - XMatrixAPI[4] * XMatrixAPI[2]);

        return result;
    }

    float XMatrixAPI::XMatrixAPIDeterminant(XMatrixAPI *xmatrix)
    {
        float XMatrixAPI3x3[9];
        float determinant3x3 = 0.0f;
        float result = 0.0f;

        XMatrixAPI3x3[0] = xmatrix->elements[ 5];
        XMatrixAPI3x3[1] = xmatrix->elements[ 6];
        XMatrixAPI3x3[2] = xmatrix->elements[ 7];
        XMatrixAPI3x3[3] = xmatrix->elements[ 9];
        XMatrixAPI3x3[4] = xmatrix->elements[10];
        XMatrixAPI3x3[5] = xmatrix->elements[11];
        XMatrixAPI3x3[6] = xmatrix->elements[13];
        XMatrixAPI3x3[7] = xmatrix->elements[14];
        XMatrixAPI3x3[8] = xmatrix->elements[15];
        determinant3x3 = XMatrixAPIDeterminant(XMatrixAPI3x3);
        result += xmatrix->elements[0] * determinant3x3;

        XMatrixAPI3x3[0] = xmatrix->elements[ 1];
        XMatrixAPI3x3[1] = xmatrix->elements[ 2];
        XMatrixAPI3x3[2] = xmatrix->elements[ 3];
        XMatrixAPI3x3[3] = xmatrix->elements[ 9];
        XMatrixAPI3x3[4] = xmatrix->elements[10];
        XMatrixAPI3x3[5] = xmatrix->elements[11];
        XMatrixAPI3x3[6] = xmatrix->elements[13];
        XMatrixAPI3x3[7] = xmatrix->elements[14];
        XMatrixAPI3x3[8] = xmatrix->elements[15];
        determinant3x3 = XMatrixAPIDeterminant(XMatrixAPI3x3);
        result -= xmatrix->elements[4] * determinant3x3;

        XMatrixAPI3x3[0] = xmatrix->elements[ 1];
        XMatrixAPI3x3[1] = xmatrix->elements[ 2];
        XMatrixAPI3x3[2] = xmatrix->elements[ 3];
        XMatrixAPI3x3[3] = xmatrix->elements[ 5];
        XMatrixAPI3x3[4] = xmatrix->elements[ 6];
        XMatrixAPI3x3[5] = xmatrix->elements[ 7];
        XMatrixAPI3x3[6] = xmatrix->elements[13];
        XMatrixAPI3x3[7] = xmatrix->elements[14];
        XMatrixAPI3x3[8] = xmatrix->elements[15];
        determinant3x3 = XMatrixAPIDeterminant(XMatrixAPI3x3);
        result += xmatrix->elements[8] * determinant3x3;

        XMatrixAPI3x3[0] = xmatrix->elements[ 1];
        XMatrixAPI3x3[1] = xmatrix->elements[ 2];
        XMatrixAPI3x3[2] = xmatrix->elements[ 3];
        XMatrixAPI3x3[3] = xmatrix->elements[ 5];
        XMatrixAPI3x3[4] = xmatrix->elements[ 6];
        XMatrixAPI3x3[5] = xmatrix->elements[ 7];
        XMatrixAPI3x3[6] = xmatrix->elements[ 9];
        XMatrixAPI3x3[7] = xmatrix->elements[10];
        XMatrixAPI3x3[8] = xmatrix->elements[11];
        determinant3x3 = XMatrixAPIDeterminant(XMatrixAPI3x3);
        result -= xmatrix->elements[12] * determinant3x3;

        return result;
    }



    XMatrixAPI XMatrixAPI::createScaling(float x, float y, float z)
    {
        XMatrixAPI result = identityXMatrixAPI;
        
        result.elements[ 0] = x;
        result.elements[ 5] = y;
        result.elements[10] = z;

        return result;
    }

    XMatrixAPI XMatrixAPI::createTranslation(float x, float y, float z)
    {
        XMatrixAPI result = identityXMatrixAPI;
        
        result.elements[12] = x;
        result.elements[13] = y;
        result.elements[14] = z;

        return result;
    }

    XMatrixAPI XMatrixAPI::XMatrixAPIPerspective(float FOV, float ratio, float zNear, float zFar)
    {
        XMatrixAPI result = identityXMatrixAPI;

        FOV = 1.0f / tan(FOV * 0.5f);

        result.elements[ 0] = FOV / ratio;
        result.elements[ 5] = FOV;
        result.elements[10] = -(zFar + zNear) / (zFar - zNear);
        result.elements[11] = -1.0f;
        result.elements[14] = (-2.0f * zFar * zNear) / (zFar - zNear);
        result.elements[15] = 0.0f;

        return result;
    }

    XMatrixAPI XMatrixAPI::XMatrixAPICameraLookAt(CVec3f eye, CVec3f center, CVec3f up)
	{
		XMatrixAPI result = identityXMatrixAPI;

		CVec3f cameraX, cameraY;

        CVec3f cameraZ = {center.x - eye.x, center.y - eye.y, center.z - eye.z};
        cameraZ.normalize();

        cameraX = CVec3f::cross(cameraZ, up);
        cameraX.normalize();

		cameraY = CVec3f::cross(cameraX, cameraZ);

		result[0]  = cameraX.x;
		result[1]  = cameraY.x;
		result[2]  = -cameraZ.x;

		result[4]  = cameraX.y;
		result[5]  = cameraY.y;
		result[6]  = -cameraZ.y;

		result[8]  = cameraX.z;
		result[9]  = cameraY.z;
		result[10] = -cameraZ.z;

		result[12] = -eye.x;
		result[13] = -eye.y;
		result[14] = -eye.z;

		return result;
	}

    XMatrixAPI XMatrixAPI::XMatrixAPIOrthographic(float left, float right, float bottom, float top, float zNear, float zFar)
    {
        XMatrixAPI result = identityXMatrixAPI;

        result.elements[ 0] = 2.0f / (right - left);
        result.elements[12] = -(right + left) / (right - left);

        result.elements[ 5] = 2.0f / (top - bottom);
        result.elements[13] = -(top + bottom) / (top - bottom);

        result.elements[10] = -2.0f / (zFar - zNear);
        result.elements[14] = -(zFar + zNear) / (zFar - zNear);

        return result;
    }

    XMatrixAPI XMatrixAPI::createRotationX(float angle)
    {
        XMatrixAPI result = identityXMatrixAPI;

        result.elements[ 5] = cos(degreesToRadians(angle));
        result.elements[ 9] = -sin(degreesToRadians(angle));
        result.elements[ 6] = sin(degreesToRadians(angle));
        result.elements[10] = cos(degreesToRadians(angle));

        return result;
    }

    XMatrixAPI XMatrixAPI::createRotationY(float angle)
    {
        XMatrixAPI result = identityXMatrixAPI;

        result.elements[ 0] = cos(degreesToRadians(angle));
        result.elements[ 8] = sin(degreesToRadians(angle));
        result.elements[ 2] = -sin(degreesToRadians(angle));
        result.elements[10] = cos(degreesToRadians(angle));

        return result;
    }

    XMatrixAPI XMatrixAPI::createRotationZ(float angle)
    {
        XMatrixAPI result = identityXMatrixAPI;

        result.elements[0] = cos(degreesToRadians(angle));
        result.elements[4] = -sin(degreesToRadians(angle));
        result.elements[1] = sin(degreesToRadians(angle));
        result.elements[5] = cos(degreesToRadians(angle));

        return result;
    }

    XMatrixAPI XMatrixAPI::multiply(XMatrixAPI *left, XMatrixAPI *right)
    {
        XMatrixAPI result;

        for(int row = 0; row < 4; row ++)
        {
            for(int column = 0; column < 4; column ++)
            {
                float accumulator = 0.0f;
                for(int allElements = 0; allElements < 4; allElements ++)
                {
                    accumulator += left->elements[allElements * 4 + row] * right->elements[column * 4 + allElements];
                }
                result.elements[column * 4 + row] = accumulator;
            }
        }

        return result;
    }

    CVec4f XMatrixAPI::vertexTransform(CVec4f *vertex, XMatrixAPI *xmatrix)
    {
        CVec4f result;
        
        result.x  = vertex->x * xmatrix->elements[ 0];
        result.x += vertex->y * xmatrix->elements[ 4];
        result.x += vertex->z * xmatrix->elements[ 8];
        result.x += vertex->w * xmatrix->elements[12];

        result.y  = vertex->x * xmatrix->elements[ 1];
        result.y += vertex->y * xmatrix->elements[ 5];
        result.y += vertex->z * xmatrix->elements[ 9];
        result.y += vertex->w * xmatrix->elements[13];

        result.z  = vertex->x * xmatrix->elements[ 2];
        result.z += vertex->y * xmatrix->elements[ 6];
        result.z += vertex->z * xmatrix->elements[10];
        result.z += vertex->w * xmatrix->elements[14];

        result.w  = vertex->x * xmatrix->elements[ 3];
        result.w += vertex->y * xmatrix->elements[ 7];
        result.w += vertex->z * xmatrix->elements[11];
        result.w += vertex->w * xmatrix->elements[15];

        return result;
    }

    CVec3f XMatrixAPI::vertexTransform(CVec3f *vertex, XMatrixAPI *xmatrix)
    {
        CVec3f result;
        CVec4f extendedVertex;

        extendedVertex.x = vertex->x;
        extendedVertex.y = vertex->y;
        extendedVertex.z = vertex->z;
        extendedVertex.w = 1.0f;

        result.x  = extendedVertex.x * xmatrix->elements[ 0];
        result.x += extendedVertex.y * xmatrix->elements[ 4];
        result.x += extendedVertex.z * xmatrix->elements[ 8];
        result.x += extendedVertex.w * xmatrix->elements[12];

        result.y  = extendedVertex.x * xmatrix->elements[ 1];
        result.y += extendedVertex.y * xmatrix->elements[ 5];
        result.y += extendedVertex.z * xmatrix->elements[ 9];
        result.y += extendedVertex.w * xmatrix->elements[13];

        result.z  = extendedVertex.x * xmatrix->elements[ 2];
        result.z += extendedVertex.y * xmatrix->elements[ 6];
        result.z += extendedVertex.z * xmatrix->elements[10];
        result.z += extendedVertex.w * xmatrix->elements[14];

        return result;
    }

   XMatrixAPI XMatrixAPI::XMatrixAPIInvert(XMatrixAPI *xmatrix)
    {
	XMatrixAPI result;
        float XMatrixAPI3x3[9];

        XMatrixAPI3x3[0] = xmatrix->elements[ 5];
        XMatrixAPI3x3[1] = xmatrix->elements[ 6];
        XMatrixAPI3x3[2] = xmatrix->elements[ 7];
        XMatrixAPI3x3[3] = xmatrix->elements[ 9];
        XMatrixAPI3x3[4] = xmatrix->elements[10];
        XMatrixAPI3x3[5] = xmatrix->elements[11];
        XMatrixAPI3x3[6] = xmatrix->elements[13];
        XMatrixAPI3x3[7] = xmatrix->elements[14];
        XMatrixAPI3x3[8] = xmatrix->elements[15];
        result.elements[0] = XMatrixAPIDeterminant(XMatrixAPI3x3);

        XMatrixAPI3x3[0] = xmatrix->elements[ 1];
        XMatrixAPI3x3[1] = xmatrix->elements[ 2];
        XMatrixAPI3x3[2] = xmatrix->elements[ 3];
        XMatrixAPI3x3[3] = xmatrix->elements[ 9];
        XMatrixAPI3x3[4] = xmatrix->elements[10];
        XMatrixAPI3x3[5] = xmatrix->elements[11];
        XMatrixAPI3x3[6] = xmatrix->elements[13];
        XMatrixAPI3x3[7] = xmatrix->elements[14];
        XMatrixAPI3x3[8] = xmatrix->elements[15];
        result.elements[4] = -XMatrixAPIDeterminant(XMatrixAPI3x3);

        XMatrixAPI3x3[0] = xmatrix->elements[ 1];
        XMatrixAPI3x3[1] = xmatrix->elements[ 2];
        XMatrixAPI3x3[2] = xmatrix->elements[ 3];
        XMatrixAPI3x3[3] = xmatrix->elements[ 5];
        XMatrixAPI3x3[4] = xmatrix->elements[ 6];
        XMatrixAPI3x3[5] = xmatrix->elements[ 7];
        XMatrixAPI3x3[6] = xmatrix->elements[13];
        XMatrixAPI3x3[7] = xmatrix->elements[14];
        XMatrixAPI3x3[8] = xmatrix->elements[15];
        result.elements[8] = XMatrixAPIDeterminant(XMatrixAPI3x3);

        XMatrixAPI3x3[0] = xmatrix->elements[ 1];
        XMatrixAPI3x3[1] = xmatrix->elements[ 2];
        XMatrixAPI3x3[2] = xmatrix->elements[ 3];
        XMatrixAPI3x3[3] = xmatrix->elements[ 5];
        XMatrixAPI3x3[4] = xmatrix->elements[ 6];
        XMatrixAPI3x3[5] = xmatrix->elements[ 7];
        XMatrixAPI3x3[6] = xmatrix->elements[ 9];
        XMatrixAPI3x3[7] = xmatrix->elements[10];
        XMatrixAPI3x3[8] = xmatrix->elements[11];
        result.elements[12] = -XMatrixAPIDeterminant(XMatrixAPI3x3);

        XMatrixAPI3x3[0] = xmatrix->elements[ 4];
        XMatrixAPI3x3[1] = xmatrix->elements[ 6];
        XMatrixAPI3x3[2] = xmatrix->elements[ 7];
        XMatrixAPI3x3[3] = xmatrix->elements[ 8];
        XMatrixAPI3x3[4] = xmatrix->elements[10];
        XMatrixAPI3x3[5] = xmatrix->elements[11];
        XMatrixAPI3x3[6] = xmatrix->elements[12];
        XMatrixAPI3x3[7] = xmatrix->elements[14];
        XMatrixAPI3x3[8] = xmatrix->elements[15];
        result.elements[1] = -XMatrixAPIDeterminant(XMatrixAPI3x3);

        XMatrixAPI3x3[0] = xmatrix->elements[ 0];
        XMatrixAPI3x3[1] = xmatrix->elements[ 2];
        XMatrixAPI3x3[2] = xmatrix->elements[ 3];
        XMatrixAPI3x3[3] = xmatrix->elements[ 8];
        XMatrixAPI3x3[4] = xmatrix->elements[10];
        XMatrixAPI3x3[5] = xmatrix->elements[11];
        XMatrixAPI3x3[6] = xmatrix->elements[12];
        XMatrixAPI3x3[7] = xmatrix->elements[14];
        XMatrixAPI3x3[8] = xmatrix->elements[15];
        result.elements[5] = XMatrixAPIDeterminant(XMatrixAPI3x3);

        XMatrixAPI3x3[0] = xmatrix->elements[ 0];
        XMatrixAPI3x3[1] = xmatrix->elements[ 2];
        XMatrixAPI3x3[2] = xmatrix->elements[ 3];
        XMatrixAPI3x3[3] = xmatrix->elements[ 4];
        XMatrixAPI3x3[4] = xmatrix->elements[ 6];
        XMatrixAPI3x3[5] = xmatrix->elements[ 7];
        XMatrixAPI3x3[6] = xmatrix->elements[12];
        XMatrixAPI3x3[7] = xmatrix->elements[14];
        XMatrixAPI3x3[8] = xmatrix->elements[15];
        result.elements[9] = -XMatrixAPIDeterminant(XMatrixAPI3x3);

        XMatrixAPI3x3[0] = xmatrix->elements[ 0];
        XMatrixAPI3x3[1] = xmatrix->elements[ 2];
        XMatrixAPI3x3[2] = xmatrix->elements[ 3];
        XMatrixAPI3x3[3] = xmatrix->elements[ 4];
        XMatrixAPI3x3[4] = xmatrix->elements[ 6];
        XMatrixAPI3x3[5] = xmatrix->elements[ 7];
        XMatrixAPI3x3[6] = xmatrix->elements[ 8];
        XMatrixAPI3x3[7] = xmatrix->elements[10];
        XMatrixAPI3x3[8] = xmatrix->elements[11];
        result.elements[13] = XMatrixAPIDeterminant(XMatrixAPI3x3);

        XMatrixAPI3x3[0] = xmatrix->elements[ 4];
        XMatrixAPI3x3[1] = xmatrix->elements[ 5];
        XMatrixAPI3x3[2] = xmatrix->elements[ 7];
        XMatrixAPI3x3[3] = xmatrix->elements[ 8];
        XMatrixAPI3x3[4] = xmatrix->elements[ 9];
        XMatrixAPI3x3[5] = xmatrix->elements[11];
        XMatrixAPI3x3[6] = xmatrix->elements[12];
        XMatrixAPI3x3[7] = xmatrix->elements[13];
        XMatrixAPI3x3[8] = xmatrix->elements[15];
        result.elements[2] = XMatrixAPIDeterminant(XMatrixAPI3x3);

        XMatrixAPI3x3[0] = xmatrix->elements[ 0];
        XMatrixAPI3x3[1] = xmatrix->elements[ 1];
        XMatrixAPI3x3[2] = xmatrix->elements[ 3];
        XMatrixAPI3x3[3] = xmatrix->elements[ 8];
        XMatrixAPI3x3[4] = xmatrix->elements[ 9];
        XMatrixAPI3x3[5] = xmatrix->elements[11];
        XMatrixAPI3x3[6] = xmatrix->elements[12];
        XMatrixAPI3x3[7] = xmatrix->elements[13];
        XMatrixAPI3x3[8] = xmatrix->elements[15];
        result.elements[6] = -XMatrixAPIDeterminant(XMatrixAPI3x3);

        XMatrixAPI3x3[0] = xmatrix->elements[ 0];
        XMatrixAPI3x3[1] = xmatrix->elements[ 1];
        XMatrixAPI3x3[2] = xmatrix->elements[ 3];
        XMatrixAPI3x3[3] = xmatrix->elements[ 4];
        XMatrixAPI3x3[4] = xmatrix->elements[ 5];
        XMatrixAPI3x3[5] = xmatrix->elements[ 7];
        XMatrixAPI3x3[6] = xmatrix->elements[12];
        XMatrixAPI3x3[7] = xmatrix->elements[13];
        XMatrixAPI3x3[8] = xmatrix->elements[15];
        result.elements[10] = XMatrixAPIDeterminant(XMatrixAPI3x3);

        XMatrixAPI3x3[0] = xmatrix->elements[ 0];
        XMatrixAPI3x3[1] = xmatrix->elements[ 1];
        XMatrixAPI3x3[2] = xmatrix->elements[ 3];
        XMatrixAPI3x3[3] = xmatrix->elements[ 4];
        XMatrixAPI3x3[4] = xmatrix->elements[ 5];
        XMatrixAPI3x3[5] = xmatrix->elements[ 7];
        XMatrixAPI3x3[6] = xmatrix->elements[ 8];
        XMatrixAPI3x3[7] = xmatrix->elements[ 9];
        XMatrixAPI3x3[8] = xmatrix->elements[11];
        result.elements[14] = -XMatrixAPIDeterminant(XMatrixAPI3x3);

        XMatrixAPI3x3[0] = xmatrix->elements[ 4];
        XMatrixAPI3x3[1] = xmatrix->elements[ 5];
        XMatrixAPI3x3[2] = xmatrix->elements[ 6];
        XMatrixAPI3x3[3] = xmatrix->elements[ 8];
        XMatrixAPI3x3[4] = xmatrix->elements[ 9];
        XMatrixAPI3x3[5] = xmatrix->elements[10];
        XMatrixAPI3x3[6] = xmatrix->elements[12];
        XMatrixAPI3x3[7] = xmatrix->elements[13];
        XMatrixAPI3x3[8] = xmatrix->elements[14];
        result.elements[3] = -XMatrixAPIDeterminant(XMatrixAPI3x3);

        XMatrixAPI3x3[0] = xmatrix->elements[ 0];
        XMatrixAPI3x3[1] = xmatrix->elements[ 1];
        XMatrixAPI3x3[2] = xmatrix->elements[ 2];
        XMatrixAPI3x3[3] = xmatrix->elements[ 8];
        XMatrixAPI3x3[4] = xmatrix->elements[ 9];
        XMatrixAPI3x3[5] = xmatrix->elements[10];
        XMatrixAPI3x3[6] = xmatrix->elements[12];
        XMatrixAPI3x3[7] = xmatrix->elements[13];
        XMatrixAPI3x3[8] = xmatrix->elements[14];
        result.elements[7] = XMatrixAPIDeterminant(XMatrixAPI3x3);

        XMatrixAPI3x3[0] = xmatrix->elements[ 0];
        XMatrixAPI3x3[1] = xmatrix->elements[ 1];
        XMatrixAPI3x3[2] = xmatrix->elements[ 2];
        XMatrixAPI3x3[3] = xmatrix->elements[ 4];
        XMatrixAPI3x3[4] = xmatrix->elements[ 5];
        XMatrixAPI3x3[5] = xmatrix->elements[ 6];
        XMatrixAPI3x3[6] = xmatrix->elements[12];
        XMatrixAPI3x3[7] = xmatrix->elements[13];
        XMatrixAPI3x3[8] = xmatrix->elements[14];
        result.elements[11] = -XMatrixAPIDeterminant(XMatrixAPI3x3);

        XMatrixAPI3x3[0] = xmatrix->elements[ 0];
        XMatrixAPI3x3[1] = xmatrix->elements[ 1];
        XMatrixAPI3x3[2] = xmatrix->elements[ 2];
        XMatrixAPI3x3[3] = xmatrix->elements[ 4];
        XMatrixAPI3x3[4] = xmatrix->elements[ 5];
        XMatrixAPI3x3[5] = xmatrix->elements[ 6];
        XMatrixAPI3x3[6] = xmatrix->elements[ 8];
        XMatrixAPI3x3[7] = xmatrix->elements[ 9];
        XMatrixAPI3x3[8] = xmatrix->elements[10];
        result.elements[15] = XMatrixAPIDeterminant(XMatrixAPI3x3);

        XMatrixAPITranspose(&result);

        result = XMatrixAPIScale(&result, 1.0f / XMatrixAPIDeterminant(xmatrix));

        return result;
	return 0;
    }

    XMatrixAPI XMatrixAPI::XMatrixAPIScale(XMatrixAPI *xmatrix, float scale)
    {
        XMatrixAPI result;
        
        for(int allElements = 0; allElements < 16; allElements ++)
        {
            result.elements[allElements] = xmatrix->elements[allElements] * scale;
        }

        return result;
    }

    void XMatrixAPI::XMatrixAPITranspose(XMatrixAPI *xmatrix)
    {
        float temp;

        temp = xmatrix->elements[1];
        xmatrix->elements[1] = xmatrix->elements[4];
        xmatrix->elements[4] = temp;

        temp = xmatrix->elements[2];
        xmatrix->elements[2] = xmatrix->elements[8];
        xmatrix->elements[8] = temp;

        temp = xmatrix->elements[3];
        xmatrix->elements[3] = xmatrix->elements[12];
        xmatrix->elements[12] = temp;

        temp = xmatrix->elements[6];
        xmatrix->elements[6] = xmatrix->elements[9];
        xmatrix->elements[9] = temp;

        temp = xmatrix->elements[7];
        xmatrix->elements[7] = xmatrix->elements[13];
        xmatrix->elements[13] = temp;

        temp = xmatrix->elements[11];
        xmatrix->elements[11] = xmatrix->elements[14];
        xmatrix->elements[14] = temp;
    }

