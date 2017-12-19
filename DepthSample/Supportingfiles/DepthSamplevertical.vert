attribute vec4 av4position;
attribute vec4 av4color;

uniform mat4 mvp;
uniform mediump float far;

varying vec4 vv4GlassColor;
varying float vfGlassLinearDepth;

void main()
{
    vv4GlassColor = av4color;
    gl_Position = mvp * av4position;
    vfGlassLinearDepth = gl_Position.w / far;
}
