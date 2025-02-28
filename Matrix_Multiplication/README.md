##Compile 
g++ -o matrix_multiply matrix_multiply.cpp -lGLESv2 -lEGL -lX11 -lm

##Note

CPU SIDE                      |                  GPU SIDE (VERTEX SHADER)
                                               |
Generate random matrices A & B                 |  For each output element C[i][j]:
     |                                         |       |
     v                                         |       v
Upload matrices as textures                    |  Receive indices (j,i) as vertex attribute
     |                                         |       |
     v                                         |       v
Create vertex array with (j,i) pairs           |  For k=0 to MATRIX_SIZE-1:
(one vertex per output element)                |       |
     |                                         |       v
     v                                         |  Sample A[i][k] from textureA
Set up OpenGL/EGL context                      |       |
     |                                         |       v
     v                                         |  Sample B[k][j] from textureB
Compile shaders                                |       |
     |                                         |       v
     v                                         |  Accumulate: sum += A[i][k] * B[k][j]
Pass matrices as textures                      |       |
     |                                         |       v
     v                                         |  Normalize result (sum/MATRIX_SIZE)
Draw points (one per matrix element)           |       |
     |                                         |       v
     v                                         |  Round to two decimal places
Read back results from framebuffer             |       |
                                               |       v
                                               |  Set vertex position for rendering
                                               |       |
                                               |       v
                                               |  Pass result to fragment shader
                                               |       |
                                               |       v
                                               |  Fragment shader outputs result as color
