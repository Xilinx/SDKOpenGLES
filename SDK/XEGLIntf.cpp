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
 * @file XEGLIntf.cpp
 *
 * This file implements all the functions related to EGL bindings.
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


#include "XEGLIntf.h"
#include "XPodium.h"

#include <cstdlib>

    EGLDisplay CoreEGL::display;
    EGLContext CoreEGL::context;
    EGLSurface CoreEGL::surface;
    EGLConfig CoreEGL::config;

    EGLint CoreEGL::configAttributes[] =
    {
        EGL_SAMPLES,             4,
        EGL_ALPHA_SIZE,          0,
#ifdef ENABLE_FBDEV
        EGL_RED_SIZE,            8,
        EGL_GREEN_SIZE,          8,
        EGL_BLUE_SIZE,           8,
        EGL_BUFFER_SIZE,         32,
#else
        EGL_RED_SIZE,            8,
        EGL_GREEN_SIZE,          8,
        EGL_BLUE_SIZE,           8,
        EGL_BUFFER_SIZE,         32,
#endif
        EGL_STENCIL_SIZE,        0,
        EGL_RENDERABLE_TYPE,     0,    /* This field will be completed according to application request. */
        EGL_SURFACE_TYPE,        EGL_WINDOW_BIT ,
        EGL_DEPTH_SIZE,          16,
        EGL_NONE
    };

    EGLint CoreEGL::contextAttributes[] =
    {
        EGL_CONTEXT_CLIENT_VERSION, 0, 
        EGL_NONE, EGL_NONE,            
        EGL_NONE
    };

    EGLint CoreEGL::windowAttributes[] =
    {
        EGL_NONE
    };

  

    void CoreEGL::initializeEGL(OpenGLESVersion requestedAPIVersion)
    {
        XPodium* platform = XPodium::getHandler();

        EGLBoolean success = EGL_FALSE;

#ifdef ENABLE_FBDEV
        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
#else
        platform->display = XOpenDisplay(NULL);
        display = eglGetDisplay(platform->display);
#endif

        if(display == EGL_NO_DISPLAY)
        {
            EGLint error = eglGetError();
            printf("eglGetError(): %i (0x%.4x)\n", (int)error, (int)error);
            printf("No EGL Display available at %s:%i\n", __FILE__, __LINE__);
            exit(1);
        }

        success = eglInitialize(display, NULL, NULL);
        if(success != EGL_TRUE)
        {
            EGLint error = eglGetError();
            printf("eglGetError(): %i (0x%.4x)\n", (int)error, (int)error);
            printf("Failed to initialize EGL at %s:%i\n", __FILE__, __LINE__);
            exit(1);
        }

        if(requestedAPIVersion == OPENGLES1)
        {
            configAttributes[15] = EGL_OPENGL_ES_BIT;
            contextAttributes[1] = 1;
            contextAttributes[2] = EGL_NONE;
        }
        else if(requestedAPIVersion == OPENGLES2)
        {
            configAttributes[15] = EGL_OPENGL_ES2_BIT;
            contextAttributes[1] = 2;
            contextAttributes[2] = EGL_NONE;
        }
#ifdef ENABLE_FBDEV
        config = findConfig(true);
#else
        config = findConfig(false);
#endif

#ifdef ENABLE_X11
        ((XLinuxPodium*)(platform))->createX11Window();
#endif

        surface = eglCreateWindowSurface(display, config, (EGLNativeWindowType)(platform->window), windowAttributes);
        if(surface == EGL_NO_SURFACE)
        {
            EGLint error = eglGetError();
            printf("eglGetError(): %i (0x%.4x)\n", (int)error, (int)error);
            printf("Failed to create EGL surface at %s:%i\n", __FILE__, __LINE__);
            exit(1);
        }

        eglBindAPI(EGL_OPENGL_ES_API);

        context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttributes);
        if(context == EGL_NO_CONTEXT)
        {
            EGLint error = eglGetError();
            printf("eglGetError(): %i (0x%.4x)\n", (int)error, (int)error);
            printf("Failed to create EGL context at %s:%i\n", __FILE__, __LINE__);
            exit(1);
        }
    }

    void CoreEGL::setEGLSamples(EGLint requiredEGLSamples)
    {
        configAttributes[1] = requiredEGLSamples;
    }

    void CoreEGL::terminateEGL(void)
    {
        eglBindAPI(EGL_OPENGL_ES_API);
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, context);
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroyContext(display, context);
        eglDestroySurface(display, surface);
        eglTerminate(display);
    }

  EGLConfig CoreEGL::findConfig(bool strictMatch)
    {
        EGLConfig *configsArray = NULL;
        EGLint numberOfConfigs = 0;
        EGLBoolean success = EGL_FALSE;

        /* Enumerate available EGL configurations which match or exceed our required attribute list. */
        success = eglChooseConfig(display, configAttributes, NULL, 0, &numberOfConfigs);
        if(success != EGL_TRUE)
        {
            EGLint error = eglGetError();
            printf("eglGetError(): %i (0x%.4x)\n", (int)error, (int)error);
            printf("Failed to enumerate EGL configs at %s:%i\n", __FILE__, __LINE__);
            exit(1);
        }

        printf("Number of configs found is %d\n", numberOfConfigs);

        if (numberOfConfigs == 0)
        {
            printf("Disabling AntiAliasing to try and find a config.\n");
            configAttributes[1] = EGL_DONT_CARE;
            success = eglChooseConfig(display, configAttributes, NULL, 0, &numberOfConfigs);
            if(success != EGL_TRUE)
            {
                EGLint error = eglGetError();
                printf("eglGetError(): %i (0x%.4x)\n", (int)error, (int)error);
                printf("Failed to enumerate EGL configs at %s:%i\n", __FILE__, __LINE__);
                exit(1);
            }

            if (numberOfConfigs == 0)
            {
                printf("No configs found with the requested attributes.\n");
                exit(1);
            }
            else
            {
                printf("Configs found when antialiasing disabled.\n ");
            }
        }

        configsArray = (EGLConfig *)calloc(numberOfConfigs, sizeof(EGLConfig));
        if(configsArray == NULL)
        {
            printf("Out of memory at %s:%i\n", __FILE__, __LINE__);
            exit(1);
        }
        success = eglChooseConfig(display, configAttributes, configsArray, numberOfConfigs, &numberOfConfigs);
        if(success != EGL_TRUE)
        {
            EGLint error = eglGetError();
            printf("eglGetError(): %i (0x%.4x)\n", (int)error, (int)error);
            printf("Failed to enumerate EGL configs at %s:%i\n", __FILE__, __LINE__);
            exit(1);
        }

        bool matchFound = false;
        int matchingConfig = -1;

        if (strictMatch)
        {
            EGLint redSize = configAttributes[5];
            EGLint greenSize = configAttributes[7];
            EGLint blueSize = configAttributes[9];

            for(int configsIndex = 0; (configsIndex < numberOfConfigs) && !matchFound; configsIndex++)
            {
                EGLint attributeValue = 0;

                success = eglGetConfigAttrib(display, configsArray[configsIndex], EGL_RED_SIZE, &attributeValue);
                if(success != EGL_TRUE)
                {
                    EGLint error = eglGetError();
                    printf("eglGetError(): %i (0x%.4x)\n", (int)error, (int)error);
                    printf("Failed to get EGL attribute at %s:%i\n", __FILE__, __LINE__);
                    exit(1);
                }

                if(attributeValue == redSize)
                {
                    success = eglGetConfigAttrib(display, configsArray[configsIndex], EGL_GREEN_SIZE, &attributeValue);
                    if(success != EGL_TRUE)
                    {
                        EGLint error = eglGetError();
                        printf("eglGetError(): %i (0x%.4x)\n", (int)error, (int)error);
                        printf("Failed to get EGL attribute at %s:%i\n", __FILE__, __LINE__);
                        exit(1);
                    }

                    if(attributeValue == greenSize)
                    {
                        success = eglGetConfigAttrib(display, configsArray[configsIndex], EGL_BLUE_SIZE, &attributeValue);
                        if(success != EGL_TRUE)
                        {
                            EGLint error = eglGetError();
                            printf("eglGetError(): %i (0x%.4x)\n", (int)error, (int)error);
                            printf("Failed to get EGL attribute at %s:%i\n", __FILE__, __LINE__);
                            exit(1);
                        }

                        if(attributeValue == blueSize) 
                        {
                            matchFound = true;
                            matchingConfig = configsIndex;
                        }
                    }
                }
            }
        }
        else
        {
            matchingConfig = 0;
            matchFound = true;
        }

        if(!matchFound)
        {
            printf("Failed to find matching EGL config at %s:%i\n", __FILE__, __LINE__);
            exit(1);
        }

        EGLConfig configToReturn = configsArray[matchingConfig];

        free(configsArray);
        configsArray = NULL;

        return configToReturn;
    }
