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
 * @file CubeSample.cpp
 *
 * This file implements test application for OpenGL rendering using SDK
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


#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "XLinuxPodium.h"
#include "CubeSample.h"
#include "XGLSLCompile.h"
#include "XEGLIntf.h"
#include "XMatrixAPI.h"
#include <string>

using namespace std;

#define SCENE_WIDTH 800
#define SCENE_HEIGHT 600


/* Asset directories and filenames. */
string resourceDirectory = "Supportingfiles/";
string vertexShaderFilename = "CubeSample.vert";
string fragmentShaderFilename = "CubeSample.frag";

/* Shader variables. */
GLuint programID;
GLint iLocPosition = -1;
GLint iLocFillColor = -1;
GLint iLocMVP = -1;

bool prepareGraphics(int width, int height)
{
    printf("prepareGraphics(%d, %d)", width, height);

    string vertexShaderPath = resourceDirectory + vertexShaderFilename; 
    string fragmentShaderPath = resourceDirectory + fragmentShaderFilename;

    GLuint vertexShaderID = 0;
    GLuint fragmentShaderID = 0;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader::processShader(&vertexShaderID, vertexShaderPath.c_str(), GL_VERTEX_SHADER);
    Shader::processShader(&fragmentShaderID, fragmentShaderPath.c_str(), GL_FRAGMENT_SHADER);
 
    programID = glCreateProgram();
    if (programID == 0)
    {
        printf("Could not create program.");
        return false;
    }

    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);
    glUseProgram(programID);

    iLocPosition = glGetAttribLocation(programID, "a_v4Position");
   
    iLocFillColor = glGetAttribLocation(programID, "a_v4FillColor");
   
    iLocMVP = glGetUniformLocation(programID, "mvp");

    glViewport(0, 0, width, height);

    glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
    glClearDepthf(1.0f);
    return true;
}

void GraphicsUpdate(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(programID);
    glVertexAttribPointer(iLocPosition, 3, GL_FLOAT, GL_FALSE, 0, cubeVertices);
    glEnableVertexAttribArray(iLocPosition);

    if(iLocFillColor != -1)
    {
        glVertexAttribPointer(iLocFillColor, 3, GL_FLOAT, GL_FALSE, 0, cubeColors);
        glEnableVertexAttribArray(iLocFillColor);
    }

	static float angleX = 0, angleY = 0, angleZ = 0;
        XMatrixAPI modelView = XMatrixAPI::createRotationX(angleX);
        XMatrixAPI rotation = XMatrixAPI::createRotationY(angleY);
        modelView = rotation * modelView;
        rotation = XMatrixAPI::createRotationZ(angleZ);
        XMatrixAPI translate1 = XMatrixAPI::createTranslation(-1.0,0.0,0.0);
        modelView =  translate1 *rotation * modelView;
        modelView[14] -= 4.5;
        XMatrixAPI perspective = XMatrixAPI::XMatrixAPIPerspective(45.0f, 1280/720, 0.01f, 100.0f);
        XMatrixAPI modelViewPerspective = perspective * modelView;
        glUniformMatrix4fv(iLocMVP, 1, GL_FALSE, modelViewPerspective.getAsArray());
        angleX += 4;
        angleY += 3;
        angleZ += 2;
        if(angleX >= 360) angleX -= 360;
        if(angleX < 0) angleX += 360;
        if(angleY >= 360) angleY -= 360;
        if(angleY < 0) angleY += 360;
        if(angleZ >= 360) angleZ -= 360;
        if(angleZ < 0) angleZ += 360;
    	glDrawArrays(GL_TRIANGLES, 0, 36);


	static float angle1X = 0, angle1Y = 0, angle1Z = 0;
        XMatrixAPI modelView1 = XMatrixAPI::createRotationX(angle1X);
        XMatrixAPI rotation1 = XMatrixAPI::createRotationY(angle1Y);
        modelView1 = rotation1 * modelView1;
        rotation1 = XMatrixAPI::createRotationZ(angle1Z);
        XMatrixAPI translate2 = XMatrixAPI::createTranslation(1.0,0.0,0.0);
        modelView1 =  translate2 *rotation1 * modelView1;
        modelView1[14] -= 4.5;
        XMatrixAPI perspective1 = XMatrixAPI::XMatrixAPIPerspective(45.0f, 1280/720, 0.01f, 100.0f);
        XMatrixAPI modelViewPerspective1 = perspective1 * modelView1;
        glUniformMatrix4fv(iLocMVP, 1, GL_FALSE, modelViewPerspective1.getAsArray());
        angle1X += 2;
        angle1Y += 3;
        angle1Z += 4;
        if(angle1X >= 360) angle1X -= 360;
        if(angle1X < 0) angle1X += 360;
        if(angle1Y >= 360) angle1Y -= 360;
        if(angle1Y < 0) angle1Y += 360;
        if(angle1Z >= 360) angle1Z -= 360;
        if(angle1Z < 0) angle1Z += 360;
        glDrawArrays(GL_TRIANGLES, 0, 36);

}


int main(int argc, char* argv[])
{
    
    XPodium *podium = XPodium::getHandler();
    podium->prepareWindow(SCENE_WIDTH, SCENE_HEIGHT);
    CoreEGL::initializeEGL(CoreEGL::OPENGLES2);
    eglMakeCurrent(CoreEGL::display, CoreEGL::surface, CoreEGL::surface, CoreEGL::context);
    prepareGraphics(SCENE_WIDTH, SCENE_HEIGHT);

    bool end = false;
    while(!end)
    {
        if(podium->checkWindow() != XPodium::WINDOW_IDLE)
        {
            end = true;
        }

	
        GraphicsUpdate();
	eglSwapBuffers(CoreEGL::display, CoreEGL::surface);
    }

    CoreEGL::terminateEGL();
    podium->destroyWindow();
    delete podium;

    return 0;
}


