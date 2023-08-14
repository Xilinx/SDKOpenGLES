precision lowp float;
uniform float iGlobalTime;
vec2 iResolution = vec2(160,160);
vec2 iResolutionX = vec2(250,250);
vec2 iResolutionY = vec2(350,350);
vec4 fragColor;


float plot(vec2 st, float pct){
  return  abs(smoothstep( pct-0.005, pct, st.y) -  smoothstep( pct, pct+0.005, st.y));
}

float ecg(float x)
{

    x =  x - ceil(x/2.8 - 0.5)*2.8;

   return 0.2*(exp((-pow(x + 1.4, 2.)) / (2.*0.02)) + exp((-pow(x - 1.4,2.)) / (2.*0.02))) + (1. - abs(x / 0.1) - x)*exp((-pow(7.*x,2.)) / 2.);
}


void main()
{
        vec2 uv = gl_FragCoord.xy / iResolution.xy;
        vec2 uv2 = gl_FragCoord.xy / iResolutionX.xy;
        vec2 uv3 = gl_FragCoord.xy / iResolutionY.xy;
        uv2.y -= 0.4;
        uv3.y -= 0.6;
        gl_FragColor = vec4(0., step( 0.245,mod(uv.x, 1./4.)) , 0., 1.);
        gl_FragColor += vec4(0., step( 0.245,mod(uv.y, 1./4.)) , 0., 1.);
        gl_FragColor +=  plot(uv, ecg(uv.x*2. + (iGlobalTime) / 3.5)/2. + 0.5) * vec4(10.,10.,0., 1.);
        gl_FragColor +=  plot(uv2, ecg(uv2.x*2. + (iGlobalTime) / 3.5)/2. + 0.5) * vec4(10.,0.,0., 1.);
        gl_FragColor +=  plot(uv3, ecg(uv3.x*2. + (iGlobalTime) / 3.5)/2. + 0.5) * vec4(10.,0.,10., 1.);
}
