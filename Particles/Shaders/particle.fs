#version 460 core

uniform vec3 uTime;
uniform vec3 uHSV;
uniform vec2 uResolution;

in vec4 vertexColor;

out vec4 fragmentColor;

vec3 rgb_to_hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv_to_rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
    vec2 uv = gl_FragCoord.xy / uResolution;
    float distanceFromCenter = distance(uv, vec2(0.5)) * 1.2;
    distanceFromCenter += 0.2;

    vec3 centerColor = vec3(0.0, 1.0, 1.0);
    vec3 borderColor = vec3(1.0, 0.0, 1.0);

    vec3 colorHSV = rgb_to_hsv(vertexColor.xyz);
    vec3 colorRGB = hsv_to_rgb(colorHSV + uHSV);

//    fragmentColor = vec4(1.0);                                                                                    //White 
//    fragmentColor = vec4(uv, 0.0, 1.0);                                                                           //UV based

//    fragmentColor = vertexColor;                                                                                  //Particle color
//    fragmentColor = vec4(colorRGB, vertexColor.a);                                                                //Particle color + HSV 
    fragmentColor = vec4(mix(centerColor, borderColor, distanceFromCenter), vertexColor.a);                          //Particle color distance dependent + HSV
} 
