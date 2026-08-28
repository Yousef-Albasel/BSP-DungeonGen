#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;

uniform mat4 u_MVP;
uniform mat4 u_Model;

out vec2 v_TexCoord;
out vec3 v_FragPos;

void main()
{
    vec4 worldPosition = u_Model * vec4(position, 1.0);

    gl_Position = u_MVP * vec4(position, 1.0);
    v_FragPos = worldPosition.xyz;
    v_TexCoord = texCoord;
}

#shader fragment
#version 330 core

out vec4 color;

in vec2 v_TexCoord;
in vec3 v_FragPos;

uniform sampler2D u_Texture;
uniform sampler2D u_NormalMap;

uniform vec3 u_LightPosition;
uniform vec3 u_LightColor;
uniform vec3 u_AmbientColor;
uniform vec3 u_ViewPosition;
uniform float u_Shininess;
uniform float u_SpecularStrength;

void main()
{
    vec3 baseColor = texture(u_Texture, v_TexCoord).rgb;

    // Build tangent space from position and UV derivatives.
    vec3 dp1 = dFdx(v_FragPos);
    vec3 dp2 = dFdy(v_FragPos);
    vec2 duv1 = dFdx(v_TexCoord);
    vec2 duv2 = dFdy(v_TexCoord);

    float determinant = duv1.x * duv2.y - duv1.y * duv2.x;

    vec3 T = normalize(
        (dp1 * duv2.y - dp2 * duv1.y) / determinant);

    vec3 B = normalize(
        (-dp1 * duv2.x + dp2 * duv1.x) / determinant);

    vec3 N = normalize(cross(T, B));

    // Normal map color [0, 1] -> tangent-space normal [-1, 1].
    vec3 tangentNormal =
        normalize(texture(u_NormalMap, v_TexCoord).rgb * 2.0 - 1.0);

    vec3 worldNormal = normalize(mat3(T, B, N) * tangentNormal);

    vec3 L = normalize(u_LightPosition - v_FragPos);
    vec3 V = normalize(u_ViewPosition - v_FragPos);
    vec3 R = reflect(-L, worldNormal);

    float diffuse = max(dot(worldNormal, L), 0.0);
    float specular = pow(
        max(dot(V, R), 0.0),
        u_Shininess);

    vec3 lighting =
        u_AmbientColor +
        diffuse * u_LightColor +
        u_SpecularStrength * specular * u_LightColor;

    color = vec4(baseColor * lighting, 1.0);
}