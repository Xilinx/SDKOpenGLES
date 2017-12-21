precision mediump float;
varying vec2 textureOut;
uniform sampler2D tex_y;
uniform sampler2D tex_u;
uniform sampler2D tex_v;
const vec3 R_cf = vec3(1.164383,  0.000000,  1.596027);
const vec3 G_cf = vec3(1.164383, -0.391762, -0.812968);
const vec3 B_cf = vec3(1.164383,  2.017232,  0.000000);
const vec3 offset = vec3(-0.0625, -0.5, -0.5);
void main()
{
vec3 yuv;
vec3 rgb;
yuv.x = texture2D(tex_y, textureOut).r;
yuv.y = texture2D(tex_u, textureOut).r;
yuv.z = texture2D(tex_v, textureOut).r;
yuv += offset;
rgb.r = dot(yuv, R_cf);
rgb.g = dot(yuv, G_cf);
rgb.b = dot(yuv, B_cf);
gl_FragColor = vec4(rgb, 1);
}

