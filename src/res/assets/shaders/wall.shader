#shader vertex

#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 u_MVP;
uniform mat4 u_Model;

out vec2 v_TexCoord;
out vec3 v_FragPos;
out vec3 v_Normal;

void main()
{
    vec4 worldPosition = u_Model * vec4(position, 1.0);

    gl_Position = u_MVP * vec4(position, 1.0);
    v_FragPos = worldPosition.xyz;
    v_TexCoord = texCoord;
    v_Normal = mat3(u_Model) * normal;
}


#shader fragment

#version 330 core

out vec4 color;

in vec2 v_TexCoord;
in vec3 v_FragPos;
in vec3 v_Normal;

uniform sampler2D u_Texture;
uniform sampler2D u_NormalMap;

uniform int u_DebugMode;
uniform vec3 u_LightDirection;
uniform vec3 u_LightColor;
uniform vec3 u_AmbientColor;
uniform vec3 u_ViewPosition;

uniform float u_Shininess;
uniform float u_SpecularStrength;

void main()
{
    // --------------------------------------------------
    // Base texture
    // --------------------------------------------------

    vec3 baseColor = texture(u_Texture, v_TexCoord).rgb;


    // --------------------------------------------------
    // Tangent-space normal mapping via screen-space derivatives
    // --------------------------------------------------

    vec3 dp1  = dFdx(v_FragPos);
    vec3 dp2  = dFdy(v_FragPos);
    vec2 duv1 = dFdx(v_TexCoord);
    vec2 duv2 = dFdy(v_TexCoord);

    float det = duv1.x * duv2.y - duv1.y * duv2.x;
    vec3 T = normalize((dp1 * duv2.y - dp2 * duv1.y) / det);
    vec3 B = normalize((-dp1 * duv2.x + dp2 * duv1.x) / det);
    vec3 N = cross(T, B);

    vec3 geoNormal = normalize(v_Normal);
    if (dot(N, geoNormal) < 0.0)
    {
        N = -N;
        B = -B;
    }
    N = normalize(N);

    mat3 TBN = mat3(T, B, N);

    // Normal map: [0, 1] -> [-1, 1]
    vec3 tangentNormal = texture(u_NormalMap, v_TexCoord).rgb * 2.0 - 1.0;
    tangentNormal = normalize(tangentNormal);

    vec3 worldNormal = normalize(TBN * tangentNormal);


    // --------------------------------------------------
    // Lighting
    // --------------------------------------------------

    vec3 L = normalize(-u_LightDirection);
    vec3 V = normalize(u_ViewPosition - v_FragPos);
    vec3 R = reflect(-L, worldNormal);

    float diffuse  = max(dot(worldNormal, L), 0.0);
    float specular = pow(max(dot(V, R), 0.0), u_Shininess);

    vec3 lighting =
        u_AmbientColor +
        diffuse * u_LightColor +
        u_SpecularStrength * specular * u_LightColor;


    // --------------------------------------------------
    // Final color
    // --------------------------------------------------

    if (u_DebugMode == 1)
        color = vec4(worldNormal * 0.5 + 0.5, 1.0);
    else
        color = vec4(baseColor * lighting, 1.0);
}