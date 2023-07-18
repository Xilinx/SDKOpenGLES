precision lowp float;

varying vec3 vv3color;

void main()
{
    gl_FragColor =  vec4(vv3color, 1.0);
}
