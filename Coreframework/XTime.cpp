/*******************************************************************************
 *
 * Copyright (C) 2023 Xilinx Inc.  All rights reserved.
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
 * @file Xtime.cpp
 *
 * This file implements timer related API for SDK
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


#include "XTime.h"

#if defined(_WIN32)

#include <cstdio>
#include <windows.h>

namespace SDKXilinx
{
    Timer::Timer()
    {
        LARGE_INTEGER l;
        QueryPerformanceFrequency(&l);
        invFreq = 1.0f / l.QuadPart;
        lastInterval = 0.0f;
        frameCount = 0;
        lastFpsUpdate = 0.0f;
        reset();
        fps = 0.0f;
        lastTime = 0.0f;
    }

    void Timer::reset()
    {
        LARGE_INTEGER l;
        QueryPerformanceCounter(&l);
        resetStamp = (((double)l.QuadPart) * invFreq);
    }

    float Timer::getTime()
    {
        LARGE_INTEGER l;
        QueryPerformanceCounter(&l);
        return (float)(((double)l.QuadPart) * invFreq - resetStamp);
    }

    float Timer::getInterval()
    {
        float time = getTime();
        float interval = time - lastInterval;
        lastInterval = time;
        return interval;
    }

    float Timer::getFPS()
    {
        float time = getTime();
        frameCount++;
        if (time-lastFpsUpdate > 1.0f)
        {
            fps = (float)frameCount / (time-lastFpsUpdate);
            lastFpsUpdate = time;
            frameCount = 0;
        }
        return fps;
    }
}
#else

#include <sys/time.h>

namespace SDKXilinx
{

    Timer::Timer()
        : startTime()
        , currentTime()
        , lastIntervalTime(0.0f)
        , frameCount(0)
        , fpsTime(0.0f)
        , fps(0.0f)
    {    
        startTime.tv_sec = 0;
        startTime.tv_usec = 0;
        currentTime.tv_sec = 0;
        currentTime.tv_usec = 0;

        reset();
    }

    void Timer::reset()
    {
        gettimeofday(&startTime, NULL);
        lastIntervalTime = 0.0;

        frameCount = 0;
        fpsTime = 0.0f;
    }

    float Timer::getTime()
    {
        gettimeofday(&currentTime, NULL);
        float seconds = (currentTime.tv_sec - startTime.tv_sec);
        float milliseconds = (float(currentTime.tv_usec - startTime.tv_usec)) / 1000000.0f;
        return seconds + milliseconds;
    }

    float Timer::getInterval()
    {
        float time = getTime();
        float interval = time - lastIntervalTime;
        lastIntervalTime = time;
        return interval;
    }

    float Timer::getFPS()
    {
        if (getTime() - fpsTime > 1.0f)
        {
            fps = static_cast<float>(frameCount) / (getTime() - fpsTime);
            frameCount = 0;
            fpsTime = getTime();
        }
        ++frameCount;
        return fps;
    }
#endif

    bool Timer::isTimePassed(float seconds)
    {
        float time = getTime();
        if (time - lastTime > seconds)
        {
            lastTime = time;
            return true;
        }
        return false;
    }

}
