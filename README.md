# SDKOpenGLES
Open Source Software development Kit for Graphics which can run on Mali 400 GPU on Xilinx Zu+ Platform.


# Cube Sample
Multi Cube rendering Sample GPU
This sample contains simple cube rendering technique on GPU using OpenGL ES 2.0 API’s.
Colors are Interpolated across the vertices and showcase rotation matrix application for creating animation 
effects. This is available in both versions X11 and FBDEV.

![alt text](https://github.com/Xilinx/SDKOpenGLES/blob/master/CubeSample/CubesSample.png)


# Depth Buffer Sample GPU
This sample is designed to use depth buffer and create an effect on GPU . It draws cube and vertical plane with overlapping depth.Animation is created using matrices which allows the objects to rotate.This is available in both versions X11 and FBDEV
![alt text](https://github.com/Xilinx/SDKOpenGLES/blob/master/DepthSample/Depth.png)


# EGL Image Texture Swapping
This sample is designed to showcase the usage of EGL Image extension. It defines a new EGL resource type that is suitable for
sharing 2D arrays of image data between client APIs, the EGLImage. Although the intended purpose is sharing 2D image data, the
underlying interface makes no assumptions about the format or purpose of the resource being shared, leaving those decisions to
the application and associated client APIs. This is available for FBDEV

![alt text](https://github.com/Xilinx/SDKOpenGLES/blob/master/EGLImageSample/EGL1.png)

![alt text](https://github.com/Xilinx/SDKOpenGLES/blob/master/EGLImageSample/EGL2.png)


# EGL Image Live Video streaming texture update - GPU
This sample is designed to showcase the usage of EGL Image extension in real time which will take live feed from camera mounted on ZCU+ board and streaming the data on /dev/video0 node. The application reads that streaming and use it as a texture in OpenGL ES application  and render and update the texture frame-per-frame using EGL Image instead of classic glteximage2D approach and is quite fast compared to classic approach. This is available for FBDEV.

![alt text](https://github.com/Xilinx/SDKOpenGLES/blob/master/EGLImageVideoSample/EGLVideo1.png)

![alt text](https://github.com/Xilinx/SDKOpenGLES/blob/master/EGLImageVideoSample/EGLVideo2.png)


# Frame Buffer Object (Render to Texture) Sample GPU
This sample contains Render to texture rendering technique sample on GPU using OpenGL ES 2.0 API’s.
FBO is used to create the color cube which is rendered to a texture and is being used in subsequent rendering pass as input texture to be applied across all six faces of bigger cube .Rotating matrix are used here to create animation effects on both cubes separately.This is available in both versions X11 and FBDEV
![alt text](https://github.com/Xilinx/SDKOpenGLES/blob/master/FBOSample/FBO.png)


# MIPMAP Levels & Compressed Texture Sample
This sample is designed to show the capability of GPU to decompress the compressed texture on the fly with multiple levels of MIPMAPS being used for textures. The rendering shows Texture MipMaps at multiple levels which were stored as ETC compressed.
This is available in both versions X11 and FBDEV.
![alt text](https://github.com/Xilinx/SDKOpenGLES/blob/master/MipMapSample/MIPMAP.png)


# Multi Particle rendering Sample GPU
This sample is designed to create particle system effect 
on GPU . It draws multiple particles on screen which are created using a small texture , each particle has a 
lifetime and color which can be controlled by the application. Animation is created using matrices which allows the particles to diverge in multiple locations and sustain based on there lifetime token.This is available in both versions X11 and FBDEV
![alt text](https://github.com/Xilinx/SDKOpenGLES/blob/master/MoleculeSystemSample/Particles.png)


Simple Triangle rendering Sample GPU
This sample contains simple triangle rendering technique on GPU using OpenGL ES 2.0 API’s.Colors are Interpolated across the vertices and showcase rotation matrix application for creating animation effects.This is available in both versions X11 and FBDEV.
![alt text](https://github.com/Xilinx/SDKOpenGLES/blob/master/TriangleSample/Triangle.png)

# Waveform Pattern Generation using GLSL Fragment Shader on GPU
This sample is designed to show the capability of GPU to Waveform rendering using GLSL Shader code only
Without using lot of vertices. The rendering show multiple waveforms on the screen which are implemented using math functions in Pixel Shader.This is available in both versions X11 and FBDEV
![alt text](https://github.com/Xilinx/SDKOpenGLES/blob/master/WaverenderSample/Waveform.png)


# YUV TO RGB CONVERSION ON GPU Shader core - GLSL
This sample is designed to show the capability of GPU to do fast Color conversion from YUV to RGB 
in GLSL Shader code on the Fly. And render the converted Image on the framebuffer continuously frame by frame.
This is available in both versions X11 and FBDEV
![alt text](https://github.com/Xilinx/SDKOpenGLES/blob/master/YUV2RGBonGPUSample/Yuv2rgb.png)



# Test File size was large so download this out.yuv file form link below & place it under "/YUV2RGBonGPUSample/Supportingfiles/" Folder

https://drive.google.com/open?id=1x-iDJurB3rB9TrF_Wa-4uARr8YtTxfzc
