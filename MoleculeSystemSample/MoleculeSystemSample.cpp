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
 * @file MoleculeSystemSample.cpp
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
#include "MoleculeSystemSample.h"
#include "XGLSLCompile.h"
#include "XEGLIntf.h"
#include "XMatrixAPI.h"
#include "XTime.h"
#include <string>

using namespace SDKXilinx;
using namespace std;

#define GL_CHECK(x) \
        x; \
{ \
        GLenum glError = glGetError(); \
        if(glError != GL_NO_ERROR) { \
                printf("glGetError() = %i (0x%.8x) at %s:%i\n", glError, glError, __FILE__, __LINE__); \
                exit(1); \
        } \
}


#define WINDOW_W 800
#define WINDOW_H 600

#define PARTICLE_DATA_SIZE   9

/* Particle settings to play with */
/* Number of particles */
#define NUM_PARTICLES        2000
/* Radius of emitter disc */
#define EMITTER_RADIUS        0.04f
/* Maximum emission angle */
#define EMITTER_ANGLE        40.0f
/* Factor that multiplies the initial velocity of particles */
#define VELOCITY_FACTOR        2.8f
/* Factor that multiplies the lifetime of particles. */
#define LIFETIME_FACTOR        5.8f
/* Red component of particle base color. */
#define BASE_COLOUR_RED        0.7f
/* Green component of particle base color. */
#define BASE_COLOUR_GREEN    0.1f
/* Blue component of particle base color. */
#define BASE_COLOUR_BLUE    0.1f
/* Z component of gravity vector. */
#define GRAVITY_Z            -0.05f

/* Asset directories and filenames. */
string resourceDirectory = "Supportingfiles/";
string vertexShaderFilename = "Molecule_system.vert";
string fragmentShaderFilename = "Molecule_system.frag";
string textureFilename =  "texture";
string imageExtension = ".raw";

/* Texture. */
#define TEXTURE_WIDTH 128
#define TEXTURE_HEIGHT 128

  typedef struct
    {
        /**
         * \brief Particle lifetime.
         */
        float lifetime;
        /**
         * \brief Particle creation delay time.
         */
        float delay;
        /**
         * \brief Particle initial position.
         */
        CVec3f initPos;
        /**
         * \brief Particle initial velocity.
         */
        CVec3f initVel;
    } Particle;

    /**
     * \brief A class used to represent an emitter of random particles from a disc.
     */
    class DiscEmitter
    {
    private:
        /**
         * \brief The radius of the disc.
         */
        float discRadius;
        /**
         * \brief Maximum emission angle in degrees relative to disc normal (0,0,1).
         */
        float maxEmissionAngle;
    public:
        /**
         * \brief Default constructor.
         */
        DiscEmitter(void);
        /**
         * \brief Constructor.
         * param[in] rad The radius of the emitter disc.
         * param[in] angle Maximum emission angle in degrees [0, 90].
         */
        DiscEmitter(float discRadius, float angle);
        /**
         * \brief Produces a random particle.
         * \param[out] part A particle.
         */
        void getParticle(Particle &part);
    };

DiscEmitter::DiscEmitter(void)
    {
        discRadius = 0.5f;
        maxEmissionAngle = M_PI/10;
    }

    DiscEmitter::DiscEmitter(float rad, float angle):discRadius(rad),maxEmissionAngle(angle * M_PI / 180.0f)
    {}

    /* [Particle generation] */
    void DiscEmitter::getParticle(Particle &part)
    {
        /* Generate  a random number in the interval [0,1]. */
        float rad = (float)rand()/((float)RAND_MAX);

        /* Generate  a random number in the interval [0,1]. */
        float polarAngle = (float)rand()/((float)RAND_MAX);
        polarAngle *= 2 * M_PI;

        part.initPos.x = discRadius * rad * cos(polarAngle);
        part.initPos.y = discRadius * rad * sin(polarAngle);
        part.initPos.z = 0.0f;

        /* Generate  a random number in the interval [0,1]. */
        float azimuthAngle = (float)rand()/((float)RAND_MAX);
        azimuthAngle *= maxEmissionAngle;

        part.initVel.x = sin(azimuthAngle) * cos(polarAngle);
        part.initVel.y = sin(azimuthAngle) * sin(polarAngle);
        part.initVel.z = cos(azimuthAngle);

        part.lifetime = (float)rand()/((float)RAND_MAX);

        /* Generate  a random number in the interval [0,1]. */
        part.delay = (float)rand()/((float)RAND_MAX);

    }



/* Structure to hold all shader data. */
typedef struct
{
    /* Handle to a program object. */
    GLuint programID;

    /* Handle to texture object. */
    GLuint textureID;

    /* Attribute locations. */
    GLint iLocPosition;
    GLint iLocVelocity;
    GLint iLocParticleTimes;

    /* Uniform locations. */
    GLint iLocMVP;
    GLint iLocGravity;
    GLint iLocColour;

    /*Sampler location. */
    GLint iLocSampler;

    /* Elapsed time per frame. */
    float frameTime;

    /* Transformation matrix. */
    XMatrixAPI modelViewPerspective;

    /* Gravity. */
    CVec3f gravity;

    /* Base colour. */
    CVec3f baseColour;

    /* Particle data array. */
    float particleData[NUM_PARTICLES * PARTICLE_DATA_SIZE];

} UserData;

UserData *userData = NULL;

/* Initialization of all particle data. */
void initializeParticleDataArray()
{
    /* Use the above particle setting #defines to change particle parameters. */

    DiscEmitter emitter(EMITTER_RADIUS, EMITTER_ANGLE);
    Particle particle;
    int index;

    for (int i = 0; i < NUM_PARTICLES; i++)
    {
        emitter.getParticle(particle);

        index = PARTICLE_DATA_SIZE*i;

        userData->particleData[index]        = particle.initPos.x;
        userData->particleData[index + 1]    = particle.initPos.y;
        userData->particleData[index + 2]    = particle.initPos.z;

        userData->particleData[index + 3]    = VELOCITY_FACTOR*particle.initVel.x;
        userData->particleData[index + 4]    = VELOCITY_FACTOR*particle.initVel.y;
        userData->particleData[index + 5]    = VELOCITY_FACTOR*particle.initVel.z;

        userData->particleData[index + 6]    = 4.0f*particle.delay;
        userData->particleData[index + 7]    = LIFETIME_FACTOR*particle.lifetime;
        /* Initial particle age. */
        userData->particleData[index + 8]    = 0.0f;
    }
}

/* Update of particle age. */
void updateParticleAge()
{
    int temp;

    for (int i = 0; i < NUM_PARTICLES; i++)
    {
        temp = PARTICLE_DATA_SIZE * i;

        /* Increment particle age. */
        userData->particleData[temp + 8] += userData->frameTime;
        if( userData->particleData[temp + 8] > userData->particleData[temp + 7])
        {
            /* Reset particle age. */
            userData->particleData[temp + 8] = 0.0f;
        }
    }
}

void initialiseTextureFromRawGreyscaleAlphaFile(const string texturePath )
{
    GL_CHECK(glGenTextures(1, &userData->textureID));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, userData->textureID));

    string mainTexturePath = texturePath + imageExtension;

    unsigned char *textureData = NULL;

    FILE * file = NULL;

    /* Open texture file. */
    file = fopen( mainTexturePath.c_str(), "rb" );

    if(file)
    {
        /* Gray scale image. */
        int numBytes = TEXTURE_WIDTH * TEXTURE_HEIGHT;

        /* Allocate data buffer. */
        textureData = (unsigned char *) malloc( numBytes * sizeof(unsigned char));

        if(!textureData)
        {
            fclose(file);
            printf("Could not allocate memory for texture data.");
            exit(1);
        }

        /* Read texture file. */
        fread(textureData, numBytes, 1, file);
        fclose(file);
    }
    else
    {
        printf("Could not open texture file %s.", mainTexturePath.c_str());
        exit(1);
    }

    GL_CHECK(glTexImage2D (GL_TEXTURE_2D, 0, GL_LUMINANCE, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, textureData));
    free(textureData);

    GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));

    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
}

bool setupGraphics(int width, int height)
{
    /* [Create userData] */
    /* Create structure to hold all data. */
    userData = new UserData;
    /* [Create userData] */

    /* Ask OpenGL ES for the maximum number of vertex attributes supported. */
    GLint maxVertexAttribs = 0;
    GL_CHECK(glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs));
    printf("GL_MAX_VERTEX_ATTRIBS = %d", maxVertexAttribs);

    /* [Initialize shaders] */
    printf("setupGraphics(%d, %d)", width, height);

    /* Full paths to the shader and texture files. */
    string vertexShaderPath = resourceDirectory + vertexShaderFilename;
    string fragmentShaderPath = resourceDirectory + fragmentShaderFilename;
    string texturePath = resourceDirectory + textureFilename;

    GLuint vertexShaderID = 0;
    GLuint fragmentShaderID = 0;
    /* [Initialize shaders] */


    initialiseTextureFromRawGreyscaleAlphaFile(texturePath);

    /* [Intialize OpenGL ES] */
    /* [Process shaders] */
    /* Process shaders. */
    Shader::processShader(&vertexShaderID, vertexShaderPath.c_str(), GL_VERTEX_SHADER);
    printf("vertexShaderID = %d", vertexShaderID);
    Shader::processShader(&fragmentShaderID, fragmentShaderPath.c_str(), GL_FRAGMENT_SHADER);
    printf("fragmentShaderID = %d", fragmentShaderID);

    userData->programID = GL_CHECK(glCreateProgram());

    GL_CHECK(glAttachShader(userData->programID, vertexShaderID));
    GL_CHECK(glAttachShader(userData->programID, fragmentShaderID));
    GL_CHECK(glLinkProgram(userData->programID));
    GL_CHECK(glUseProgram(userData->programID));
    /* [Process shaders] */

    /* [Initialize particles] */
    initializeParticleDataArray();
    /* [Initialize particles] */

    /* [Attribute location] */
    /* Get attribute locations. */
    GL_CHECK(userData->iLocPosition = glGetAttribLocation(userData->programID, "a_v3Position"));
    printf("glGetAttribLocation(\"a_v3Position\") = %d\n", userData->iLocPosition);

    GL_CHECK(userData->iLocVelocity = glGetAttribLocation(userData->programID, "a_v3Velocity"));
    printf("glGetAttribLocation(\"a_v3Velocity\") = %d\n", userData->iLocVelocity);

    GL_CHECK(userData->iLocParticleTimes = glGetAttribLocation(userData->programID, "a_v3ParticleTimes"));
    printf("glGetAttribLocation(\"a_v3ParticleTimes\") = %d\n", userData->iLocParticleTimes);

    /* Get uniform locations. */
    userData->iLocMVP = GL_CHECK(glGetUniformLocation(userData->programID, "mvp"));
    printf("glGetUniformLocation(\"mvp\") = %d\n", userData->iLocMVP);

    userData->iLocGravity = GL_CHECK(glGetUniformLocation(userData->programID, "u_v3gravity"));
    printf("glGetUniformLocation(\"u_v3gravity\") = %d\n", userData->iLocGravity);

    userData->iLocSampler = GL_CHECK(glGetUniformLocation(userData->programID, "s_texture"));
    if (userData->iLocSampler == -1)
    {
        printf("Warning: glGetUniformLocation(\"s_texture\") = %d\n", userData->iLocSampler);
    }
    else
    {
        GL_CHECK(glUniform1i(userData->iLocSampler, 0));
    }


    userData->iLocColour = GL_CHECK(glGetUniformLocation(userData->programID, "u_v3colour"));
    printf("glGetUniformLocation(\"u_v3colour\") = %d\n", userData->iLocColour);
    /* [Attribute location] */

    /* [Viewport initialization] */
    GL_CHECK(glViewport(0, 0, width, height));

    /* Set clear screen color. */
    GL_CHECK(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    /* [Viewport initialization] */


    /* [Data initialization] */
    XMatrixAPI modelView = XMatrixAPI::createRotationX(-90);
    XMatrixAPI rotation = XMatrixAPI::createRotationZ(-90);
    modelView = rotation * modelView;

    /* Pull the camera back from emitter. */
    modelView[14] -= 50.0;
    /* Pull the camera down from emitter. */
    modelView[13] -= 1.8;

    XMatrixAPI perspective = XMatrixAPI::XMatrixAPIPerspective(45.0f, (float)width/(float)height, 0.01f, 100.0f);
    userData->modelViewPerspective = perspective * modelView;

    /* Define constant acceleration vector of the field where the particles are moving. */
    userData->gravity.x = userData->gravity.y = 0.0f;
    userData->gravity.z = GRAVITY_Z;

    /* Define particle base color. */
    userData->baseColour.x = BASE_COLOUR_RED;
    userData->baseColour.y = BASE_COLOUR_GREEN;
    userData->baseColour.z = BASE_COLOUR_BLUE;
    /* [Data initialization] */

    return true;
}

/* [Render frame] */
void renderFrame(float time)
{
     userData->baseColour.x = BASE_COLOUR_RED + time;
     userData->baseColour.y = BASE_COLOUR_GREEN + time;
     userData->baseColour.z = BASE_COLOUR_BLUE;


    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    updateParticleAge();

    GL_CHECK(glUseProgram(userData->programID));

    /* Pass particle data to the shader. */
    GL_CHECK(glVertexAttribPointer(userData->iLocPosition, 3, GL_FLOAT, GL_FALSE, PARTICLE_DATA_SIZE * sizeof(GLfloat), userData->particleData));
    GL_CHECK(glVertexAttribPointer(userData->iLocVelocity, 3, GL_FLOAT, GL_FALSE, PARTICLE_DATA_SIZE * sizeof(GLfloat), &userData->particleData[3]));
    GL_CHECK(glVertexAttribPointer(userData->iLocParticleTimes, 3, GL_FLOAT, GL_FALSE, PARTICLE_DATA_SIZE * sizeof(GLfloat), &userData->particleData[6]));

    GL_CHECK(glEnableVertexAttribArray(userData->iLocPosition));
    GL_CHECK(glEnableVertexAttribArray(userData->iLocVelocity));
    GL_CHECK(glEnableVertexAttribArray(userData->iLocParticleTimes));

    GL_CHECK(glUniformMatrix4fv(userData->iLocMVP, 1, GL_FALSE, userData->modelViewPerspective.getAsArray()));
    GL_CHECK(glUniform3fv(userData->iLocGravity, 1, (GLfloat *)&userData->gravity));
    GL_CHECK(glUniform3fv(userData->iLocColour, 1, (GLfloat *)&userData->baseColour));

    /* Bind the texture. */
    GL_CHECK(glActiveTexture(GL_TEXTURE0));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, userData->textureID));

    /* Set the sampler to point at the 0th texture unit. */
    GL_CHECK(glUniform1i(userData->iLocSampler, 0));

    /* Draw particles. */
    GL_CHECK(glDrawArrays(GL_POINTS, 0, NUM_PARTICLES));



    /* [Data initialization] */
    XMatrixAPI modelView = XMatrixAPI::createRotationZ(90);
    XMatrixAPI Translate = XMatrixAPI::createTranslation(0.5, 0.0, 0.0);
    modelView = modelView * Translate;
    /* Pull the camera back from emitter. */
    modelView[14] -= 3.0;
    /* Pull the camera down from emitter. */
    modelView[13] -= 0.4;
    XMatrixAPI perspective = XMatrixAPI::XMatrixAPIPerspective(45.0f, (float)800/(float)600, 0.01f, 100.0f);
    userData->modelViewPerspective = perspective * modelView;

     /* Draw particles. */
    GL_CHECK(glDrawArrays(GL_POINTS, 0, NUM_PARTICLES));


}
/* [Render frame] */


int main(void)
{
    XPodium *podium = XPodium::getHandler();
    podium->prepareWindow(WINDOW_W, WINDOW_H);
    CoreEGL::initializeEGL(CoreEGL::OPENGLES2);
    eglMakeCurrent(CoreEGL::display, CoreEGL::surface, CoreEGL::surface, CoreEGL::context);
 
     /* Timer variable to calculate FPS. */
    Timer fpsTimer;
    fpsTimer.reset();   

    /* Initialize OpenGL ES graphics subsystem. */
    setupGraphics(WINDOW_W, WINDOW_H);

    bool end = false;
    static float time = 0.0;
    /* The rendering loop to draw the scene. */
    while(!end)
    {

	userData->frameTime = fpsTimer.getInterval();
        /* If something has happened to the window, end the sample. */
        if(podium->checkWindow() != XPodium::WINDOW_IDLE)
        {
            end = true;
        }

        /* Render a single frame. */
        renderFrame(time);
	if(time <= 1.0)
		time+= 0.001;
	else
	   time = 0.0;

        /*
         * Push the EGL surface color buffer to the native window.
         * Causes the rendered graphics to be displayed on screen.
         */
        eglSwapBuffers(CoreEGL::display, CoreEGL::surface);
    }

    /* Shut down EGL. */
    CoreEGL::terminateEGL();

    /* Shut down windowing system. */
    podium->destroyWindow();

    /* Shut down the Platform object. */
    delete podium;
    /* [Release resources] */

    return 0;
}
