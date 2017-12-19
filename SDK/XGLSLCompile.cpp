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
 * @file XGLSLCompile.cpp
 *
 * This file implements all the functions related to GLSL compilation.
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


#include "XGLSLCompile.h"
#include "XPodium.h"

#include <cstdio>
#include <cstdlib>



    char* Shader::loadShader(const char *filename)
    {
        FILE *file = fopen(filename, "rb");
        if(file == NULL)
        {
            printf("Cannot read file '%s'\n", filename);
            exit(1);
        }
        fseek(file, 0, SEEK_END);
        long length = ftell(file);
        fseek(file, 0, SEEK_SET); 
        char *shader = (char *)calloc(length + 1, sizeof(char));
        if(shader == NULL)
        {
            printf("Out of memory at %s:%i\n", __FILE__, __LINE__);
            exit(1);
        }
        size_t numberOfBytesRead = fread(shader, sizeof(char), length, file);
        if (numberOfBytesRead != length) 
        {
            printf("Error reading %s (read %d of %d)", filename, numberOfBytesRead, length);
            exit(1);
        }
        shader[length] = '\0';
        fclose(file);

        return shader;
    }

    void Shader::processShader(GLuint *shader, const char *filename, GLint shaderType)
    {  
        const char *strings[1] = { NULL };

        *shader = glCreateShader(shaderType);
        strings[0] = loadShader(filename);
        glShaderSource(*shader, 1, strings, NULL);

        free((void *)(strings[0]));
        strings[0] = NULL;

        glCompileShader(*shader);
        GLint status;
        glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);

        if(status != GL_TRUE) 
        {
            GLint length;
            char *debugSource = NULL;
            char *errorLog = NULL;

            glGetShaderiv(*shader, GL_SHADER_SOURCE_LENGTH, &length);
            debugSource = (char *)malloc(length);
            glGetShaderSource(*shader, length, NULL, debugSource);
            printf("Debug source START:\n%s\nDebug source END\n\n", debugSource);
            free(debugSource);

           glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &length);
            errorLog = (char *)malloc(length);
            glGetShaderInfoLog(*shader, length, NULL, errorLog);
            printf("Log START:\n%s\nLog END\n\n", errorLog);
            free(errorLog);

            printf("Compilation FAILED!\n\n");
            exit(1);
        }
    }
