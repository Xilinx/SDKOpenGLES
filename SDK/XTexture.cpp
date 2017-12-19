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
 * @file Texture.cpp
 *
 * This file implements all the functions related to texture compression techniques.
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



#include "XTexture.h"
#include "XCompressionTexture.h"
#include "XPodium.h"

#include <GLES2/gl2ext.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

using std::string;



    void XTexture::XloadPKMData(const char *filename, ETCHeaderforTexture* etcHeader, unsigned char **textureData)
    {
        const int       sizeOfETCHeaderforTexture = 16; 
        unsigned char*  tempTextureData = NULL;

        XloadData(filename, &tempTextureData);

        if (textureData == NULL)
        {
            exit(1);
        }
        if (etcHeader == NULL)
        {
            exit(1);
        }

        if (tempTextureData != NULL)
        {
            ETCHeaderforTexture tempEtcHeader(tempTextureData);

            *etcHeader   = tempEtcHeader;
            *textureData = tempTextureData + sizeOfETCHeaderforTexture;
        }
        else
        {
            exit(1);
        }
    }

    void XTexture::XloadCompressedMipmaps(const char *filenameBase, const char *filenameSuffix, GLuint *textureID)
    {
        glGenTextures(1, textureID);
        glBindTexture(GL_TEXTURE_2D, *textureID);
        
        string filename = filenameBase + string("0") + filenameSuffix;
        unsigned char *data = NULL;
        XloadData(filename.c_str(), &data);
        ETCHeaderforTexture loadedETCHeaderforTexture = ETCHeaderforTexture(data);

        int width = loadedETCHeaderforTexture.getWidth();
        int height = loadedETCHeaderforTexture.getHeight();
        int numberOfMipmaps = 1;
        while((width > 1) || (height > 1))
        {
            numberOfMipmaps ++;
            if(width > 1) width >>= 1;
            if(height > 1) height >>= 1;
        }
    
        glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_ETC1_RGB8_OES, loadedETCHeaderforTexture.getWidth(), loadedETCHeaderforTexture.getHeight(), 0, (loadedETCHeaderforTexture.getPaddedWidth() * loadedETCHeaderforTexture.getPaddedHeight()) >> 1, data + 16);
        free(data);
        data = NULL;

        for(int allMipmaps = 1; allMipmaps < numberOfMipmaps; allMipmaps++)
        {
            char *level = NULL;
            int input = allMipmaps;
            int inputLength = 0;

            do
            {
                input /= 10;
                inputLength ++;
            }
            while(input != 0);

            level = (char *)calloc(inputLength + 1, sizeof(char));
            if(level == NULL)
            {
                exit(1);
            }
            sprintf(level, "%i", allMipmaps);

            filename = filenameBase + string(level) + filenameSuffix;
            XloadData(filename.c_str(), &data);
            free(level);
            level = NULL;
            loadedETCHeaderforTexture = ETCHeaderforTexture(data);

            glCompressedTexImage2D(GL_TEXTURE_2D, allMipmaps, GL_ETC1_RGB8_OES, loadedETCHeaderforTexture.getWidth(), loadedETCHeaderforTexture.getHeight(), 0, (loadedETCHeaderforTexture.getPaddedWidth() * loadedETCHeaderforTexture.getPaddedHeight()) >> 1, data + 16);
            free(data);
            data = NULL;
        }
    }

    void XTexture::XgetCompressedTextureFormats(GLint** textureFormats, int* numberOfTextureFormats)
    {
        glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, numberOfTextureFormats);

        *textureFormats = (GLint *)calloc(*numberOfTextureFormats, sizeof(GLint));
        if(*textureFormats == NULL)
        {
            exit(1);
        }

        glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, *textureFormats);
    }

    bool XTexture::XisETCSupported(bool verbose)
    {
        bool supportETC = false;
        GLint *textureFormat = NULL;
        GLint numberOfTextureFormats = 0;
        XgetCompressedTextureFormats(&textureFormat, &numberOfTextureFormats);

        if (verbose != 0)
        {
            for(int allTextureFormats = 0; allTextureFormats < numberOfTextureFormats; allTextureFormats++)
            {
                switch(textureFormat[allTextureFormats])
                {
                    case GL_ETC1_RGB8_OES:
                        break;
                    default:
                        break;
                }
            }
        }  

        for(int allTextureFormats = 0; allTextureFormats < numberOfTextureFormats; allTextureFormats++)
        {
            if (textureFormat[allTextureFormats] == GL_ETC1_RGB8_OES)
            {
                supportETC = true;
            }
        }

        free(textureFormat);
        textureFormat = NULL;

        if(!supportETC)
        {
            printf("BAD ETC\n");
        }
        return supportETC;
    }

    void XTexture::XcreateTexture(unsigned int width, unsigned int height, GLvoid **textureData)
    {
        unsigned char *randomTexture = new unsigned char [width * height * 4];
        if(randomTexture == NULL)
        {
            exit(1);
        }

        for(unsigned int allTexels = 0; allTexels < width * height; allTexels ++)
        {
            randomTexture[allTexels * 4 + 3] = 255;
            for (int allChannels = 0; allChannels < 3; allChannels++)
            {
                int randomNumber = (int)(255 * (rand() / (float)RAND_MAX));
                randomTexture[allTexels * 4 + allChannels] = randomNumber;
            }
        }

        *textureData = randomTexture;
    }

    void XTexture::XcreateTexture(unsigned int width, unsigned int height, unsigned int red, GLvoid **textureData)
    {
        unsigned char* newTexture = new unsigned char [width * height];

        if(newTexture == NULL)
        {
            exit(1);
        }

        for (unsigned int texelIndex = 0; texelIndex < width * height; ++texelIndex)
        {
            newTexture[texelIndex] = red;
        }
        *textureData = newTexture;
    }

    void XTexture::XcreateTexture(unsigned int width, unsigned int height, short red, short **textureData)
    {
        *textureData = new short [width * height];

        if (*textureData == NULL)
        {
            exit(1);
        }

        for (unsigned int texelIndex = 0; texelIndex < width * height; ++texelIndex)
        {
            (*textureData)[texelIndex] = red;
        }
    }

    void XTexture::XdeleteTextureData(GLvoid** textureData)
    {
        delete[] (unsigned char*)*textureData;
    }

    void XTexture::XloadData(const char *filename, unsigned char **textureData)
    {
    
        FILE *file = fopen(filename, "rb");
        if(file == NULL)
        {
            exit(1);
        }
        fseek(file, 0, SEEK_END);
        unsigned int length = ftell(file);
        unsigned char *loadedTexture = (unsigned char *)calloc(length, sizeof(unsigned char));
        if(loadedTexture == NULL)
        {
            exit(1);
        }
        fseek(file, 0, SEEK_SET);
        size_t read = fread(loadedTexture, sizeof(unsigned char), length, file);
        if(read != length)
        {
            exit(1);
        }
        fclose(file);

        *textureData = loadedTexture;
    }

    void XTexture::XreversePixelLine(float* destination, const float* source, int lineWidth)
    {
        const int rgbComponentsCount = 3;

        for (int pixelIndex = 0; pixelIndex < lineWidth; ++pixelIndex)
        {
            memcpy(destination + pixelIndex * rgbComponentsCount,
                   source + (lineWidth - pixelIndex - 1) * rgbComponentsCount, 
                   rgbComponentsCount * sizeof(float));
        }
    }
