attribute vec4 a_v4Position;
attribute vec4 a_v4FillColor;
uniform mat4 mvp;

varying vec4 vecColor;

void main()
{
    vecColor = a_v4FillColor;
    gl_Position = mvp * a_v4Position;
}
