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
 * @file FBOSample.cpp
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
#include "FBOSample.h"
#include "XGLSLCompile.h"
#include "XEGLIntf.h"
#include "XMatrixAPI.h"
#include <string>

using namespace std;

#define SCENE_WIDTH 800
#define SCENE_HEIGHT 600


string resourceDirectory = "Supportingfiles/";
string vertexShaderFilename = "FrameBufferObject_cube.vert";
string fragmentShaderFilename = "FrameBufferObject_cube.frag";

#define FBO_WIDTH    256
#define FBO_HEIGHT    256

GLuint vertexShaderID = 0;
GLuint fragmentShaderID = 0;
GLuint programID = 0;
GLint iLocPosition = -1;
GLint iLocTextureMix = -1;
GLint iLocTexture = -1;
GLint iLocFillColor = -1;
GLint iLocTexCoord = -1;
GLint iLocProjection = -1;
GLint iLocModelview = -1;

static float angleX = 0;
static float angleY = 0;
static float angleZ = 0;
XMatrixAPI rotationX;
XMatrixAPI rotationY;
XMatrixAPI rotationZ;
XMatrixAPI translation;
XMatrixAPI modelView;
XMatrixAPI projection;
XMatrixAPI projectionFBO;

GLuint iFBO = 0;

GLuint iFBOTex = 0;

int windowWidth = -1;
int windowHeight = -1;

bool prepareGraphics(int width, int height)
{
    windowWidth = width;
    windowHeight = height;

    string vertexShaderPath = resourceDirectory + vertexShaderFilename; 
    string fragmentShaderPath = resourceDirectory + fragmentShaderFilename;

    projection = XMatrixAPI::XMatrixAPIPerspective(45.0f, windowWidth/(float)windowHeight, 0.01f, 100.0f);
    projectionFBO = XMatrixAPI::XMatrixAPIPerspective(45.0f, (FBO_WIDTH / (float)FBO_HEIGHT), 0.01f, 100.0f);
    translation = XMatrixAPI::createTranslation(0.0f, 0.0f, -2.0f);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glGenTextures(1, &iFBOTex);
    glBindTexture(GL_TEXTURE_2D, iFBOTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FBO_WIDTH, FBO_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glGenFramebuffers(1, &iFBO);

    glBindFramebuffer(GL_FRAMEBUFFER, iFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, iFBOTex, 0);
    GLenum iResult = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(iResult != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Framebuffer incomplete at %s:%i\n", __FILE__, __LINE__);
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    Shader::processShader(&vertexShaderID, vertexShaderPath.c_str(), GL_VERTEX_SHADER);
    Shader::processShader(&fragmentShaderID, fragmentShaderPath.c_str(), GL_FRAGMENT_SHADER);

    programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);
    glUseProgram(programID);

    iLocPosition = glGetAttribLocation(programID, "a_v4Position");
    if(iLocPosition == -1)
    {
        printf("Attribute not found at %s:%i\n", __FILE__, __LINE__);
        return false;
    }
    glEnableVertexAttribArray(iLocPosition);

    iLocTextureMix = glGetUniformLocation(programID, "u_fTex");
    if(iLocTextureMix == -1)
    {
        printf("Warning: Uniform not found at %s:%i\n", __FILE__, __LINE__);
    }
    else 
    {
        glUniform1f(iLocTextureMix, 0.0);
    }

    iLocTexture = glGetUniformLocation(programID, "u_s2dTexture");
    if(iLocTexture == -1)
    {
        printf("Warning: Uniform not found at %s:%i\n", __FILE__, __LINE__);
    }
    else 
    {
        glUniform1i(iLocTexture, 0);
    }

    iLocFillColor = glGetAttribLocation(programID, "a_v4FillColor");
    if(iLocFillColor == -1)
    {
        printf("Warning: Attribute not found at %s:%i\n", __FILE__, __LINE__);
    }
    else 
    {
        glEnableVertexAttribArray(iLocFillColor);
    }

    iLocTexCoord = glGetAttribLocation(programID, "a_v2TexCoord");
    if(iLocTexCoord == -1)
    {
        printf("Warning: Attribute not found at %s:%i\n", __FILE__, __LINE__);
    }
    else 
    {
        glEnableVertexAttribArray(iLocTexCoord);
    }

    iLocProjection = glGetUniformLocation(programID, "u_m4Projection");
    if(iLocProjection == -1)
    {
        printf("Warning: Uniform not found at %s:%i\n", __FILE__, __LINE__);
    }
    else 
    {
       	glUniformMatrix4fv(iLocProjection, 1, GL_FALSE, projection.getAsArray());
    }

    iLocModelview = glGetUniformLocation(programID, "u_m4Modelview");
    if(iLocModelview == -1)
    {
        printf("Warning: Uniform not found at %s:%i\n", __FILE__, __LINE__);
    }
 
    return true;
}

void GraphicsUpdate(void)
{
    glUseProgram(programID);
    glEnableVertexAttribArray(iLocPosition);
    glVertexAttribPointer(iLocPosition, 3, GL_FLOAT, GL_FALSE, 0, cubeVertices);
    
    if(iLocFillColor != -1)
    {
        glEnableVertexAttribArray(iLocFillColor);
        glVertexAttribPointer(iLocFillColor, 4, GL_FLOAT, GL_FALSE, 0, cubeColors);
    }

    if(iLocTexCoord != -1)
    {
        glEnableVertexAttribArray(iLocTexCoord);
        glVertexAttribPointer(iLocTexCoord, 2, GL_FLOAT, GL_FALSE, 0, cubeTextureCoordinates);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, iFBO);
    glViewport(0, 0, FBO_WIDTH, FBO_HEIGHT);

    glClearColor(1.0f, 0.8f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    rotationX = XMatrixAPI::createRotationX(-angleZ);
    rotationY = XMatrixAPI::createRotationY(-angleY);
    rotationZ = XMatrixAPI::createRotationZ(-angleX);

    modelView = translation * rotationX;
    modelView = modelView * rotationY;
    modelView = modelView * rotationZ;

    glUniformMatrix4fv(iLocModelview, 1, GL_FALSE, modelView.getAsArray());
    glUniformMatrix4fv(iLocProjection, 1, GL_FALSE, projectionFBO.getAsArray());

    if(iLocTextureMix != -1)
    {
        glUniform1f(iLocTextureMix, 0.0);
    }

    glDrawElements(GL_TRIANGLE_STRIP, sizeof(cubeIndices) / sizeof(GLubyte), GL_UNSIGNED_BYTE, cubeIndices);

    glBindFramebuffer(GL_FRAMEBUFFER,0);

    glViewport(0, 0, windowWidth, windowHeight);

    glClearColor(0.0f, 0.0f, 0.2f, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    rotationX = XMatrixAPI::createRotationX(angleX);
    rotationY = XMatrixAPI::createRotationY(angleY);
    rotationZ = XMatrixAPI::createRotationZ(angleZ);

    modelView = translation * rotationX;
    modelView = modelView * rotationY;
    modelView = modelView * rotationZ;

    glUniformMatrix4fv(iLocModelview, 1, GL_FALSE, modelView.getAsArray());
    glUniformMatrix4fv(iLocProjection, 1, GL_FALSE, projection.getAsArray());

    if(iLocTextureMix != -1)
    {
        glUniform1f(iLocTextureMix, 1.0);
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, iFBOTex);

    glDrawElements(GL_TRIANGLE_STRIP, sizeof(cubeIndices) / sizeof(GLubyte), GL_UNSIGNED_BYTE, cubeIndices);

    angleX += 0.2;
    angleY += 0.2;
    angleZ += 0;

    if(angleX >= 360) angleX -= 360;
    if(angleY >= 360) angleY -= 360;
    if(angleZ >= 360) angleZ -= 360;
}

int main(void)
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
