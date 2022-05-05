#version 460 core

uniform vec3 hsv;
uniform vec2 screenSize;

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
    vec2 normalizedCoordinate = gl_FragCoord.xy / screenSize;
    float distanceFromCenter = distance(normalizedCoordinate, vec2(0.5)) + 0.3;

    vec3 centerColor = vec3(1.0, 0.0, 1.0);
    vec3 borderColor = vec3(0.0, 1.0, 1.0);

    vec3 original = rgb_to_hsv(vertexColor.xyz);
    vec3 modified = hsv_to_rgb(original + hsv);

//    fragmentColor = vec4(1.0);
//    fragmentColor = vec4(modified, vertexColor.a);                                                                  //particle color + hsv
//    fragmentColor = vec4(vec2(gl_FragCoord.xy / screenSize), gl_FragCoord.z, 1.0);                                  //color depends on fragment coordinate
//    fragmentColor = vec4(0.2, 0.5, 0.7, 0.5) + vec4(0.2, 0.3, 0.5, 0.5) * sqrt(distanceFromCenter);                 //color depends on distance from center
    fragmentColor = vec4((distanceFromCenter) * centerColor + ((1 - distanceFromCenter) * borderColor), 0.8);
} 
