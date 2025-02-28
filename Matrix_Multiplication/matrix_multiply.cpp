/*******************************************************************************
 * AMD-Xilinx 2024
 * -----------------------------------------------------------------------------
 * File         : matrix_multiply.cpp
 * Description  : Demonstrates GPU-based matrix multiplication using a vertex
 *                shader with OpenGL ES 2.0
 *                Each output element is computed in the vertex shader and
 *                the results are normalized and rounded to two decimal places.
 *
 * Copyright (c) 2024, AMD-Xilinx. All rights reserved.
 *
 * License      : This software is provided "as is", without warranty of any kind,
 *                express or implied, including but not limited to the warranties
 *                of merchantability, fitness for a particular purpose and
 *                noninfringement. In no event shall AMD-Xilinx be liable for any
 *                claim, damages or other liability, whether in an action of
 *                contract, tort or otherwise, arising from, out of or in
 *                connection with the software or the use or other dealings in
 *                the software.
 *
 * -----------------------------------------------------------------------------
 ******************************************************************************/
/*
 * Author - Alok G - AMD
 *
 */

#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iomanip>

// Set matrix size (must match shader!)
#define MATRIX_SIZE 512

// Vertex shader that computes one output element per vertex.
// It expects a vertex attribute "aIndices" carrying (j, i) as floats,
// where j is the column and i is the row of the result.

const char* vertexShaderSource = R"(
#extension GL_OES_texture_float : enable
#define MATRIX_SIZE 512
attribute vec2 aIndices; // (j, i)
uniform float uSize;     // Should be MATRIX_SIZE as a float (i.e. 512.0)
uniform sampler2D uTextureA; // Matrix A stored as a LUMINANCE float texture
uniform sampler2D uTextureB; // Matrix B stored as a LUMINANCE float texture
varying float vResult;

void main() {
    float i = aIndices.y; // row index
    float j = aIndices.x; // column index
    float sum = 0.0;
    // Loop over k
    for (int k = 0; k < MATRIX_SIZE; k++) {
         float kf = float(k);
         // Sample A[i][k]
         vec2 texCoordA = vec2((kf + 0.5) / uSize, (i + 0.5) / uSize);
         // Sample B[k][j]
         vec2 texCoordB = vec2((j + 0.5) / uSize, (kf + 0.5) / uSize);
         float a = texture2D(uTextureA, texCoordA).r;
         float b = texture2D(uTextureB, texCoordB).r;
         sum += a * b;
    }
    // Normalize so that if all values were 1.0, sum becomes 1.0.
    float normalized = sum / uSize;
    // Round to two decimals.
    vResult = floor(normalized * 100.0 + 0.5) / 100.0;

    // Map the indices to normalized device coordinates.
    float x = ((j + 0.5) / uSize) * 2.0 - 1.0;
    float y = ((i + 0.5) / uSize) * 2.0 - 1.0;
    gl_Position = vec4(x, y, 0.0, 1.0);
    gl_PointSize = 1.0;
}

)";

// Simple fragment shader that outputs the computed value.
const char* fragmentShaderSource = R"(
precision mediump float;
varying float vResult;
void main() {
    // Output the computed result in the red channel (replicated to G and B)
    gl_FragColor = vec4(vResult, vResult, vResult, 1.0);
}
)";

// (The compileShader and createProgram functions are similar to before.)
GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1) {
            char* infoLog = new char[infoLen];
            glGetShaderInfoLog(shader, infoLen, nullptr, infoLog);
            std::cerr << "Error compiling shader: " << infoLog << std::endl;
            delete[] infoLog;
        }
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint createProgram(GLuint vertexShader, GLuint fragmentShader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLint infoLen = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1) {
            char* infoLog = new char[infoLen];
            glGetProgramInfoLog(program, infoLen, nullptr, infoLog);
            std::cerr << "Error linking program: " << infoLog << std::endl;
            delete[] infoLog;
        }
        glDeleteProgram(program);
        return 0;
    }
    return program;
}

// Create a texture from a float array using GL_LUMINANCE.
GLuint createTexture(float* data, int size) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, size, size, 0,
                 GL_LUMINANCE, GL_FLOAT, data);
    return texture;
}

// CPU matrix multiplication (for validation, if desired).
void cpuMatrixMultiply(float* A, float* B, float* C, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            float sum = 0.0f;
            for (int k = 0; k < size; k++) {
                sum += A[i * size + k] * B[k * size + j];
            }
            C[i * size + j] = floor(sum * 100.0f + 0.5f) / 100.0f; // round to 2 decimals
        }
    }
}

int main() {
    // Initialize X11 display and window.
    Display* xDisplay = XOpenDisplay(nullptr);
    if (!xDisplay) {
        std::cerr << "Failed to open X display" << std::endl;
        return 1;
    }
    Window root = DefaultRootWindow(xDisplay);
    XSetWindowAttributes swa;
    swa.event_mask = ExposureMask | PointerMotionMask | KeyPressMask;
    Window xWindow = XCreateWindow(xDisplay, root, 0, 0, MATRIX_SIZE, MATRIX_SIZE, 0,
                                   CopyFromParent, InputOutput, CopyFromParent,
                                   CWEventMask, &swa);
    XMapWindow(xDisplay, xWindow);
    Atom wm_delete_window = XInternAtom(xDisplay, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(xDisplay, xWindow, &wm_delete_window, 1);
    XStoreName(xDisplay, xWindow, "Vertex Shader Matrix Multiplication");

    // Initialize EGL.
    EGLDisplay eglDisplay = eglGetDisplay((EGLNativeDisplayType)xDisplay);
    if (eglDisplay == EGL_NO_DISPLAY) {
        std::cerr << "Failed to get EGL display" << std::endl;
        return 1;
    }
    EGLint major, minor;
    if (!eglInitialize(eglDisplay, &major, &minor)) {
        std::cerr << "Failed to initialize EGL" << std::endl;
        return 1;
    }
    EGLConfig eglConfig;
    EGLint numConfigs;
    EGLint configAttribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_NONE
    };
    if (!eglChooseConfig(eglDisplay, configAttribs, &eglConfig, 1, &numConfigs)) {
        std::cerr << "Failed to choose config" << std::endl;
        return 1;
    }
    EGLSurface eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig,
                                                   (EGLNativeWindowType)xWindow, nullptr);
    if (eglSurface == EGL_NO_SURFACE) {
        std::cerr << "Failed to create EGL surface" << std::endl;
        return 1;
    }
    EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    EGLContext eglContext = eglCreateContext(eglDisplay, eglConfig, EGL_NO_CONTEXT, contextAttribs);
    if (eglContext == EGL_NO_CONTEXT) {
        std::cerr << "Failed to create EGL context" << std::endl;
        return 1;
    }
    if (!eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext)) {
        std::cerr << "Failed to make context current" << std::endl;
        return 1;
    }

    // Print OpenGL ES info.
    std::cout << "OpenGL ES Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "OpenGL ES Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL ES Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Matrix size: " << MATRIX_SIZE << "x" << MATRIX_SIZE << std::endl;

    // Generate matrices A and B with random values (in [0,1]).
    std::srand(std::time(nullptr));
    std::vector<float> matrixA(MATRIX_SIZE * MATRIX_SIZE);
    std::vector<float> matrixB(MATRIX_SIZE * MATRIX_SIZE);
    for (int i = 0; i < MATRIX_SIZE * MATRIX_SIZE; i++) {
        matrixA[i] = static_cast<float>(std::rand()) / RAND_MAX;
        matrixB[i] = static_cast<float>(std::rand()) / RAND_MAX;
    }

    // Create textures for matrices A and B.
    GLuint textureA = createTexture(matrixA.data(), MATRIX_SIZE);
    GLuint textureB = createTexture(matrixB.data(), MATRIX_SIZE);

    // Build a vertex array for the output.
    // Each vertex is a vec2 containing (j, i) for the result matrix element.
    std::vector<float> vertexData;
    vertexData.reserve(MATRIX_SIZE * MATRIX_SIZE * 2);
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            vertexData.push_back(static_cast<float>(j));
            vertexData.push_back(static_cast<float>(i));
        }
    }

    // Create a VBO for our vertices.
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

    // Compile shaders and create program.
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    GLuint program = createProgram(vertexShader, fragmentShader);
    if (!program) {
        std::cerr << "Failed to create program" << std::endl;
        return 1;
    }
    glUseProgram(program);

    // Get attribute and uniform locations.
    GLint indicesLoc = glGetAttribLocation(program, "aIndices");
    GLint sizeLoc = glGetUniformLocation(program, "uSize");
    GLint textureALoc = glGetUniformLocation(program, "uTextureA");
    GLint textureBLoc = glGetUniformLocation(program, "uTextureB");

    // Set uniform values.
    glUniform1f(sizeLoc, static_cast<float>(MATRIX_SIZE));
    glUniform1i(textureALoc, 0);
    glUniform1i(textureBLoc, 1);

    // Bind the matrix textures to texture units.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureA);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureB);

    // Set up the vertex attribute.
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(indicesLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(indicesLoc);

    // Set viewport to match the matrix size.
    glViewport(0, 0, MATRIX_SIZE, MATRIX_SIZE);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw points – one per output matrix element.
    glDrawArrays(GL_POINTS, 0, MATRIX_SIZE * MATRIX_SIZE);
    eglSwapBuffers(eglDisplay, eglSurface);

    // Optionally read back a few pixels for debugging.
    std::vector<unsigned char> pixelData(MATRIX_SIZE * MATRIX_SIZE * 4);
    glReadPixels(0, 0, MATRIX_SIZE, MATRIX_SIZE, GL_RGBA, GL_UNSIGNED_BYTE, pixelData.data());
    std::cout << "Sample output (first 5 pixels):" << std::endl;
    for (int i = 0; i < 5; i++) {
        int idx = i * 4;
        float value = pixelData[idx] / 255.0f;
        std::cout << std::fixed << std::setprecision(2) << value << " ";
    }
    std::cout << std::endl;

    // (Optional) Compare with CPU-computed result.
    std::vector<float> cpuResult(MATRIX_SIZE * MATRIX_SIZE);
    cpuMatrixMultiply(matrixA.data(), matrixB.data(), cpuResult.data(), MATRIX_SIZE);
    // Normalize the CPU results for fair comparison.
    for (int i = 0; i < MATRIX_SIZE * MATRIX_SIZE; i++) {
        cpuResult[i] /= MATRIX_SIZE;
    }

    std::cout << "CPU result (first 5 elements):" << std::endl;
    for (int i = 0; i < 5; i++) {
        std::cout << std::fixed << std::setprecision(2) << cpuResult[i] << " ";
    }
    std::cout << std::endl;

    // Cleanup.
    glDeleteBuffers(1, &vbo);
    glDeleteTextures(1, &textureA);
    glDeleteTextures(1, &textureB);
    glDeleteProgram(program);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(eglDisplay, eglContext);
    eglDestroySurface(eglDisplay, eglSurface);
    eglTerminate(eglDisplay);
    XDestroyWindow(xDisplay, xWindow);
    XCloseDisplay(xDisplay);

    return 0;
}
