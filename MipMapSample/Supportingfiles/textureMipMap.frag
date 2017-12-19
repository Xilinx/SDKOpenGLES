#version 100
precision mediump float;

uniform sampler2D u_s2dTexture;

varying vec2 v_v2TexCoord;

void main()
{
    gl_FragColor = texture2D(u_s2dTexture, v_v2TexCoord);
}
