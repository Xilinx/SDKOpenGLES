# GPU-Based Matrix Multiplication with OpenGL ES 2.0

This implementation demonstrates how to leverage the shader's parallel processing capabilities to perform efficient matrix multiplication using OpenGL ES 2.0 vertex shaders.

## How It Works

The implementation divides the computation between the CPU and GPU, with the heavy computational work offloaded to the GPU:

### Execution Flow

```
CPU Side                                  |  GPU Side (Vertex Shader)
------------------------------------------|------------------------------------------
Generate random matrices A & B            |  For each output element C[i][j]:
     |                                    |       |
     v                                    |       v
Upload matrices as textures               |  Receive indices (j,i) as vertex attribute
     |                                    |       |
     v                                    |       v
Create vertex array with (j,i) pairs      |  For k=0 to MATRIX_SIZE-1:
(one vertex per output element)           |       |
     |                                    |       v
     v                                    |  Sample A[i][k] from textureA
Set up OpenGL/EGL context                 |       |
     |                                    |       v
     v                                    |  Sample B[k][j] from textureB
Compile shaders                           |       |
     |                                    |       v
     v                                    |  Accumulate: sum += A[i][k] * B[k][j]
Pass matrices as textures                 |       |
     |                                    |       v
     v                                    |  Normalize result (sum/MATRIX_SIZE)
Draw points (one per matrix element)      |       |
     |                                    |       v
     v                                    |  Round to two decimal places
Read back results from framebuffer        |       |
                                          |       v
                                          |  Set vertex position for rendering
                                          |       |
                                          |       v
                                          |  Pass result to fragment shader
                                          |       |
                                          |       v
                                          |  Fragment shader outputs result as color
```


- **Parallel Computation**: Each element of the output matrix is calculated simultaneously by a separate vertex shader invocation
- **Texture-Based Storage**: Input matrices are stored as textures for efficient GPU access
- **Vertex as Computation Unit**: Repurposes the vertex shader's parallel nature for general computation


## Requirements

- OpenGL ES 2.0 compatible GPU
- EGL and X11 support
- C++ compiler with C++11 support
