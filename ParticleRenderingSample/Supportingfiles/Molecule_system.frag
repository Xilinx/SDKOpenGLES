precision mediump float;

varying float v_ageFactor;
varying vec3 v_v3colour;
uniform sampler2D s_texture;

void main()
{
    vec4 texColour;
    vec2 texCoords;
    float alphaFactor;

    texCoords = vec2(gl_PointCoord.x, gl_PointCoord.y);

    /* Define an alpha modulation factor depending of the particle's age. */
    if(v_ageFactor  <= 0.5)
    {
        alphaFactor = 0.14 * v_ageFactor;
    }
    else
    {
        alphaFactor = -0.14 * v_ageFactor + 0.14;
    }

    vec4 baseColour = vec4(v_v3colour.x, v_v3colour.y, v_v3colour.z, 1.0);
    texColour =  texture2D(s_texture, texCoords);

    /* Modulate alpha component. */
    texColour.a =  texColour.r * alphaFactor;

    gl_FragColor = vec4(texColour.r, texColour.r, texColour.r, texColour.a) * baseColour;
}
