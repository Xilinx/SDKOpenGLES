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
 * @file YUV2RGBonGPUSample.cpp
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
#include "YUV2RGBonGPUSample.h"
#include "XGLSLCompile.h"
#include "XEGLIntf.h"
#include "XMatrixAPI.h"
#include "string"


#define SCENE_WIDTH 800
#define SCENE_HEIGHT 600

using std::string;

string resourceDirectory = "Supportingfiles/";
string vertexShaderFilename = "YUV2RGBonGPUSample.vert";
string fragmentShaderFilename = "YUV2RGBonGPUSample.frag";

GLuint programID;
GLint iLocPosition = -1;
GLint itimeLocation = -1;
GLint iLocMVP = -1;
GLint iLocTexture = -1;

//YUV file
GLuint id_y, id_u, id_v; // Texture id
GLuint textureUniformY, textureUniformU, textureUniformV;

const int pixel_w = 640, pixel_h = 480;
FILE *infile = NULL;
unsigned char buf[pixel_w*pixel_h * 3 / 2];
unsigned char *plane[3];



bool prepareGraphics(int width, int height)
{
	printf("setupGraphics(%d, %d)", width, height);
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

	iLocPosition = glGetAttribLocation(programID, "av4position");
	iLocTexture = glGetAttribLocation(programID, "av3colour");
	iLocMVP = glGetUniformLocation(programID, "mvp");
	textureUniformY = glGetUniformLocation(programID, "tex_y");
	textureUniformU = glGetUniformLocation(programID, "tex_u");
	textureUniformV = glGetUniformLocation(programID, "tex_v");

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glUseProgram (programID);
	glEnableVertexAttribArray(iLocPosition);
	glEnableVertexAttribArray(iLocTexture);
	glVertexAttribPointer(iLocPosition, 2, GL_FLOAT, GL_FALSE, 0, vertexVertices);
	glVertexAttribPointer(iLocTexture, 2, GL_FLOAT, GL_FALSE, 0, textureVertices);

	glGenTextures(1, &id_y);
	glBindTexture(GL_TEXTURE_2D, id_y);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &id_u);
	glBindTexture(GL_TEXTURE_2D, id_u);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &id_v);
	glBindTexture(GL_TEXTURE_2D, id_v);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	return true;
}

void GraphicsUpdate(float time)
{

	static float angleX = 0, angleY = 0, angleZ = 0;
	XMatrixAPI modelView = XMatrixAPI::createRotationX(angleX);
	XMatrixAPI rotation = XMatrixAPI::createRotationY(angleY);
	modelView = rotation * modelView;
	modelView[14] -= 1.5;

	XMatrixAPI perspective = XMatrixAPI::XMatrixAPIPerspective(45.0f, 800/600, 0.01f, 100.0f);
	XMatrixAPI modelViewPerspective = perspective * modelView;

	glUniformMatrix4fv(iLocMVP, 1, GL_FALSE, modelViewPerspective.getAsArray());


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	if (fread(buf, 1, pixel_w*pixel_h * 3 / 2, infile) != pixel_w*pixel_h * 3 / 2) {
		fseek(infile, 0, SEEK_SET);
		fread(buf, 1, pixel_w*pixel_h * 3 / 2, infile);
	}

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	//Y
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, id_y);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, pixel_w, pixel_h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, plane[0]);
	glUniform1i(textureUniformY, 0);
	//U
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, id_u);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, pixel_w / 2, pixel_h / 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, plane[1]);
	glUniform1i(textureUniformU, 1);
	//V
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, id_v);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, pixel_w / 2, pixel_h / 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, plane[2]);
	glUniform1i(textureUniformV, 2);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

}


int main(int argc, char* argv[])
{

	XPodium *podium = XPodium::getHandler();
	podium->prepareWindow(SCENE_WIDTH, SCENE_HEIGHT);
	CoreEGL::initializeEGL(CoreEGL::OPENGLES2);
	eglMakeCurrent(CoreEGL::display, CoreEGL::surface, CoreEGL::surface, CoreEGL::context);

	//Open YUV420P file
	string rawyuvfile = resourceDirectory + "out.yuv";

	if ((infile = fopen(rawyuvfile.c_str(), "rb")) == NULL) {
		printf("cannot open out.yuv file\n");
		return -1;
	}

	//YUV Data
	plane[0] = buf;
	plane[1] = plane[0] + pixel_w*pixel_h;
	plane[2] = plane[1] + pixel_w*pixel_h / 4;

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


