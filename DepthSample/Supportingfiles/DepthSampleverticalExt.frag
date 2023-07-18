precision mediump float;

uniform float near;
uniform float far;
uniform float glassMaterial;

uniform vec2 viewportSize;
uniform sampler2D cubeDepthTexture;

varying lowp vec4 vv4GlassColor;
varying float vfGlassLinearDepth;

highp float z_eye_far_to_linear(highp float z, float n, float f)
{
    return 2.0 * n / (f + n - z * (f - n));
}

void main()
{
    highp float fCubeZ =  texture2D(cubeDepthTexture, gl_FragCoord.xy / viewportSize).r * 2.0 - 1.0;
    float fCubeLinearDepth =  z_eye_far_to_linear(fCubeZ, near, far);


    float fator =  clamp(vv4GlassColor.a + (fCubeLinearDepth - vfGlassLinearDepth) * glassMaterial, 0.0, 1.0);
    gl_FragColor =  vec4(vv4GlassColor.rgb, fator);
}

