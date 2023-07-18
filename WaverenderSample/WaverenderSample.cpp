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
 * @file WaverenderSample.cpp
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
#include "WaverenderSample.h"
#include "XGLSLCompile.h"
#include "XEGLIntf.h"
#include "XMatrixAPI.h"
#include <string>

using namespace std;

#define SCENE_WIDTH 800
#define SCENE_HEIGHT 600

string resourceDirectory = "Supportingfiles/";
string vertexShaderFilename = "WaverenderSample.vert";
string fragmentShaderFilename = "WaverenderSample.frag";

GLuint programID;
GLint iLocPosition = -1;
GLint itimeLocation = -1;
GLint iLocMVP = -1;



void GraphicsUpdate(float time)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(programID);
    glUniform1f (itimeLocation, time);
    glVertexAttribPointer(iLocPosition, 3, GL_FLOAT, GL_FALSE, 0, Vertices);
    glEnableVertexAttribArray(iLocPosition);

	static float angleX = 0, angleY = 0, angleZ = 0;
        XMatrixAPI modelView = XMatrixAPI::createRotationX(angleX);
        XMatrixAPI translate1 = XMatrixAPI::createTranslation(0.0,0.0,-1.5);

        modelView =  translate1 * modelView;

        XMatrixAPI perspective = XMatrixAPI::XMatrixAPIPerspective(45.0f, 1280/720, 0.01f, 100.0f);
        XMatrixAPI modelViewPerspective = perspective * modelView;

        glUniformMatrix4fv(iLocMVP, 1, GL_FALSE, modelViewPerspective.getAsArray());

        glDrawArrays(GL_TRIANGLES, 0, 36);

}

bool prepareGraphics(int width, int height)
{
    string vertexShaderPath = resourceDirectory + vertexShaderFilename; 
    string fragmentShaderPath = resourceDirectory + fragmentShaderFilename;

    GLuint vertexShaderID = 0;
    GLuint fragmentShaderID = 0;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    Shader::processShader(&vertexShaderID, vertexShaderPath.c_str(), GL_VERTEX_SHADER);
    printf("vertexShaderID = %d", vertexShaderID);
    Shader::processShader(&fragmentShaderID, fragmentShaderPath.c_str(), GL_FRAGMENT_SHADER);
    printf("fragmentShaderID = %d", fragmentShaderID);

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
    itimeLocation = glGetUniformLocation(programID, "iGlobalTime");
    iLocMVP = glGetUniformLocation(programID, "mvp");

    glViewport(0, 0, width, height);

    glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
    glClearDepthf(1.0f);
    return true;
}

static void usage(void)
{
        printf("Usage:\n");
        printf("  -Window <Window System name>  Set X11 or FBDEV\n");
        printf("  -info display OpenGL renderer info\n");
}


int main(int argc, char* argv[])
{
    
    XPodium *podium = XPodium::getHandler();
    podium->prepareWindow(SCENE_WIDTH, SCENE_HEIGHT);
    CoreEGL::initializeEGL(CoreEGL::OPENGLES2);
    eglMakeCurrent(CoreEGL::display, CoreEGL::surface, CoreEGL::surface, CoreEGL::context);
    
    prepareGraphics(SCENE_WIDTH, SCENE_HEIGHT);

    bool end = false;
    float i = 0.0f;
    while(!end)
    {
        if(podium->checkWindow() != XPodium::WINDOW_IDLE)
        {
            end = true;
        }
   
        GraphicsUpdate(i);
        eglSwapBuffers(CoreEGL::display, CoreEGL::surface);
   
        if(i<=10000)
           i= i + 0.04f;
        else
           i=0;

    }

    CoreEGL::terminateEGL();
    podium->destroyWindow();
    delete podium;

    return 0;
}


