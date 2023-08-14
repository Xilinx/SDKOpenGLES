precision mediump float;
attribute vec4 av4position;
attribute vec2 av3colour;
uniform mat4 mvp;
varying vec2 textureOut;
void main()
{
  textureOut =  av3colour;
  gl_Position =  mvp * av4position;
}

