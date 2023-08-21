#version 100
precision mediump float;

uniform sampler2D u_s2dTexture;
uniform float u_fTex;

varying vec4 v_v4FillColor;
varying vec2 v_v2TexCoord;

void main()
{
    vec4 v4Texel =  texture2D(u_s2dTexture, v_v2TexCoord);
    gl_FragColor =  mix(v_v4FillColor, v4Texel, u_fTex);
}
