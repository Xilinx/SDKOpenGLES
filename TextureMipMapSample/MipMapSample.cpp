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
 * @file MipMapSample.cpp
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


#define LOAD_MIPMAPS
#undef DISABLE_MIPMAPS

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
#include <string>

using namespace std;

#define SCENE_WIDTH 800
#define SCENE_HEIGHT 600

#include "MipMapSample.h"



bool prepareGraphics(int w, int h)
{
    string texturePath = resourceDirectory + textureFilename;
    string vertexShaderPath = resourceDirectory + vertexShaderFilename; 
    string fragmentShaderPath = resourceDirectory + fragmentShaderFilename;
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (!XTexture::XisETCSupported(true))
    {
        printf("ETC1 not supported");
        return false;
    }

#ifdef LOAD_MIPMAPS
    XTexture::XloadCompressedMipmaps(texturePath.c_str(), imageExtension.c_str(), &textureID);
#else /* LOAD_MIPMAPS */
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    string mainTexturePath = texturePath + "0" + imageExtension;
    unsigned char *textureData;
    Texture::loadData(mainTexturePath.c_str(), &textureData);
    ETCHeaderforTexture loadedETCHeaderforTexture = ETCHeaderforTexture(textureData);
    glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_ETC1_RGB8_OES,
             loadedETCHeaderforTexture.getWidth(), loadedETCHeaderforTexture.getHeight(), 0,
             loadedETCHeaderforTexture.getPaddedWidth() * loadedETCHeaderforTexture.getPaddedHeight() >> 1,
             textureData + 16);
    free(textureData);

#    ifdef DISABLE_MIPMAPS
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
#    else /* DISABLE_MIPMAPS */
    glGenerateMipmap(GL_TEXTURE_2D);
#    endif /* DISABLE_MIPMAPS */
#endif /* LOAD_MIPMAPS */

    Shader::processShader(&vertexShaderID, vertexShaderPath.c_str(), GL_VERTEX_SHADER);
    printf("vertexShaderID = %d", vertexShaderID);
    Shader::processShader(&fragmentShaderID, fragmentShaderPath.c_str(), GL_FRAGMENT_SHADER);
    printf("fragmentShaderID = %d", fragmentShaderID);
    programID = glCreateProgram();
    if (!programID) 
    {
        printf("Could not create program.");
        return false;
    }
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);
    glUseProgram(programID);

    iLocPosition = glGetAttribLocation(programID, "a_v4Position");
    if(iLocPosition == -1)
    {
        exit(1);
    }
    glEnableVertexAttribArray(iLocPosition);

    iLocTexCoord = glGetAttribLocation(programID, "a_v2TexCoord");
    if(iLocTexCoord == -1)
    {
        printf("Warning: Attribute not found: \"a_v2TexCoord\"");
    }
    else
    {
        glEnableVertexAttribArray(iLocTexCoord);
    }

    iLocSampler = glGetUniformLocation(programID, "u_s2dTexture");
    if(iLocSampler == -1)
    {
        printf("Warning: Uniform not found: \"u_s2dTexture\"");
    }
    else
    {
        glUniform1i(iLocSampler, 0);
    }

    glClearColor(0.0f, 0.0f, 0.2f, 1.0);

    return true;
}

void GraphicsUpdate(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(programID);

    glEnableVertexAttribArray(iLocPosition);
    glVertexAttribPointer(iLocPosition, 3, GL_FLOAT, GL_FALSE, 0, vertices);

    if(iLocTexCoord != -1)
    {
        glVertexAttribPointer(iLocTexCoord, 2, GL_FLOAT, GL_FALSE, 0, textureCoordinates);
        glEnableVertexAttribArray(iLocTexCoord);
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glDrawElements(GL_TRIANGLE_STRIP, sizeof(indices) / sizeof(GLubyte), GL_UNSIGNED_BYTE, indices);

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

    glDeleteTextures(1, &textureID);
    CoreEGL::terminateEGL();
    podium->destroyWindow();
    delete podium;

    return 0;
}
