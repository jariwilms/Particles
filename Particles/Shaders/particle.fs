#version 460 core

layout(std140, binding = 0) uniform uGlobal
{
    vec2 resolution;
    vec2 mouse;

    float time;
    float deltaTime;
};

uniform vec3 uHSV;

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
    vec2 uv = gl_FragCoord.xy / resolution;
    float distanceFromCenter = distance(uv, vec2(0.5));
    float centerOffset = 0.2;

    vec3 centerColor = vec3(0.0, 1.0, 1.0);
    vec3 borderColor = vec3(1.0, 0.0, 1.0);

    fragmentColor = vec4(mix(centerColor, borderColor, distanceFromCenter + centerOffset), 1.0);

//    vec3 colorHSV = rgb_to_hsv(vertexColor.xyz * vertexColor.a);
//    vec3 colorRGB = hsv_to_rgb(colorHSV + uHSV);

//    fragmentColor = vec4(vec2(uv), 0.0, 1.0);
//    fragmentColor = vertexColor;                                                                
//    fragmentColor = vec4(vertexColor.a);
//    fragmentColor = vec4(colorRGB, 1.0);
//    fragmentColor = vec4(mix(centerColor, borderColor * vec3(abs(mouse/ resolution * 2.0 - 1.0) * uv, 0.0), distanceFromCenter), vertexColor.a);
}
