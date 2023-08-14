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
 * @file DepthSample.cpp
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

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "XLinuxPodium.h"
#include "XGLSLCompile.h"
#include "XEGLIntf.h"
#include "XMatrixAPI.h"
#include "XTexture.h"
#include "cstring"
#include <string>

#define SCENE_WIDTH 1280
#define SCENE_HEIGHT 720

using namespace std;

#include "DepthSample.h"

bool supportFramebufferFetch = false;
GLuint fbo = 0;
GLint iLocViewportSize;
GLint iLocCubeDepthSampleTexture;


void GraphicsUpdate(void)
{
    glUseProgram(programCubeID);

    glEnableVertexAttribArray(iLocCubePosition);
    glEnableVertexAttribArray(iLocCubeColor);

    glVertexAttribPointer(iLocCubePosition, 3, GL_FLOAT, GL_FALSE, 0, verticesCube);
    glVertexAttribPointer(iLocCubeColor, 3, GL_FLOAT, GL_FALSE, 0, colorsCube);

    static float angleY = 0;
    static float angleX = 0;
    XMatrixAPI modelView = XMatrixAPI::createRotationX(angleX);
    XMatrixAPI rotationY = XMatrixAPI::createRotationY(angleY);
    modelView = modelView * rotationY;


    CVec3f eyeVector   = {0.3, 0.1, 2.5};
    CVec3f lookAtPoint = {0.0, -1.0, 0.0};
    CVec3f upVector    = {0.0, 1.0, 0.0};

    XMatrixAPI viewMatrix = XMatrixAPI::XMatrixAPICameraLookAt(eyeVector, lookAtPoint, upVector);
    modelView = viewMatrix * modelView;

    const GLfloat fNear         = 0.01;
    const GLfloat fFar          = 3.0;
    const GLfloat glassMaterial = 1.8;

    XMatrixAPI perspective = XMatrixAPI::XMatrixAPIPerspective(45.0, windowWidth/(float)windowHeight, fNear, fFar);
    XMatrixAPI modelViewPerspective = perspective * modelView;

    glUniformMatrix4fv(iLocCubeMVP, 1, GL_FALSE, modelViewPerspective.getAsArray());

    angleY += 1;
    angleX += 1;

    if(angleY >= 360) angleY -= 360;
    if(angleY < 0) angleY += 360;

    if(angleX >= 360) angleX -= 360;
    if(angleX < 0) angleX += 360;


    if (supportFramebufferFetch)
    {
        glClear(GL_COLOR_BUFFER_BIT |
                         GL_DEPTH_BUFFER_BIT |
                         GL_STENCIL_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClear(GL_COLOR_BUFFER_BIT |
                         GL_DEPTH_BUFFER_BIT |
                         GL_STENCIL_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT |
                         GL_DEPTH_BUFFER_BIT |
                         GL_STENCIL_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }


    glUseProgram(programGlassID);
    glEnableVertexAttribArray(iLocGlassPosition);
    glEnableVertexAttribArray(iLocGlassColor);
    glVertexAttribPointer(iLocGlassPosition, 3, GL_FLOAT, GL_FALSE, 0, verticesGlass);
    glVertexAttribPointer(iLocGlassColor, 4, GL_FLOAT, GL_FALSE, 0, colorsGlass);
    modelView = XMatrixAPI::createTranslation(0.5, 0.1, 0.8);
    modelView = viewMatrix * modelView;
    modelViewPerspective = perspective * modelView;
    glUniformMatrix4fv(iLocGlassMVP, 1, GL_FALSE, modelViewPerspective.getAsArray());
    glUniform1f(iLocNear, fNear);
    glUniform1f(iLocFar, fFar);
    glUniform1f(iLocGlassMaterial, glassMaterial);

    if (!supportFramebufferFetch)
    {
        glUniform2f(iLocViewportSize, (float)windowWidth, (float)windowHeight);
        glUniform1i(iLocCubeDepthSampleTexture, 1);
    }

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

}

bool prepareGraphics(int width, int height)
{
    windowWidth = width;
    windowHeight = height;

    string vertexShaderCubePath    = resourceDirectory + vertexShaderCubeFilename;
    string fragmentShaderCubePath  = resourceDirectory + fragmentShaderCubeFilename;
    string vertexShaderGlassPath   = resourceDirectory + vertexShaderGlassFilename;
    string fragmentShaderGlassPath;

    if (supportFramebufferFetch)
    {
        fragmentShaderGlassPath = resourceDirectory + fragmentShaderGlassFilename;
    }
    else
    {
        fragmentShaderGlassPath = resourceDirectory + fragmentShaderGlassRttFilename;
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        GLuint textureId;
        glGenTextures(1, &textureId);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureId, 0);

        if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
        {
            printf("glCheckFramebufferStatue failed\n");
        }
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    GLuint fragmentShaderCubeID, vertexShaderCubeID;
    GLuint fragmentShaderGlassID, vertexShaderGlassID;
    Shader::processShader(&vertexShaderCubeID,    vertexShaderCubePath.c_str(),    GL_VERTEX_SHADER);
    Shader::processShader(&fragmentShaderCubeID,  fragmentShaderCubePath.c_str(),  GL_FRAGMENT_SHADER);
    Shader::processShader(&vertexShaderGlassID,   vertexShaderGlassPath.c_str(),   GL_VERTEX_SHADER);
    Shader::processShader(&fragmentShaderGlassID, fragmentShaderGlassPath.c_str(), GL_FRAGMENT_SHADER);

    programCubeID = glCreateProgram();
    programGlassID = glCreateProgram();

    glAttachShader(programCubeID, vertexShaderCubeID);
    glAttachShader(programCubeID, fragmentShaderCubeID);
    glLinkProgram(programCubeID);
    glAttachShader(programGlassID, vertexShaderGlassID);
    glAttachShader(programGlassID, fragmentShaderGlassID);
    glLinkProgram(programGlassID);

    iLocCubePosition = glGetAttribLocation(programCubeID, "av4position");
    iLocCubeColor = glGetAttribLocation(programCubeID, "av3color");
    iLocCubeMVP = glGetUniformLocation(programCubeID, "mvp");

    iLocGlassPosition = glGetAttribLocation(programGlassID, "av4position");
    iLocGlassColor = glGetAttribLocation(programGlassID, "av4color");

    iLocGlassMVP = glGetUniformLocation(programGlassID, "mvp");
    iLocNear = glGetUniformLocation(programGlassID, "near");
    iLocFar = glGetUniformLocation(programGlassID, "far");
    iLocGlassMaterial = glGetUniformLocation(programGlassID, "glassMaterial");

    if (!supportFramebufferFetch)
    {
        iLocViewportSize     = glGetUniformLocation(programGlassID, "viewportSize");
        iLocCubeDepthSampleTexture = glGetUniformLocation(programGlassID, "cubeDepthSampleTexture");

    }

    glUseProgram(programCubeID);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.0f, 0.0f, 0.2f, 1.0f);

    return true;
}


int main(void)
{

    XPodium *podium = XPodium::getHandler();
    podium->prepareWindow(SCENE_WIDTH, SCENE_HEIGHT);
    CoreEGL::initializeEGL(CoreEGL::OPENGLES2);
    eglMakeCurrent(CoreEGL::display, CoreEGL::surface, CoreEGL::surface, CoreEGL::context);
    const char *extensions = (const char *)glGetString(GL_EXTENSIONS);
    if (std::strstr(extensions, "GL_ARM_shader_framebuffer_fetch_depth_stencil"))
    {
        supportFramebufferFetch = true;
    }
    else if (!std::strstr(extensions, "GL_OES_depth_texture"))
    {
        printf("OpenGL ES 2.0 implementation neither support ARM_shader_framebuffer_fetch_depth_stencil nor support OES_depth_texture extension.");
        exit(1);
    }

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
