#shader vertex

#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 u_MVP;
uniform mat4 u_Model;

out vec2 v_TexCoord;
out vec3 v_FragPos;

void main()
{
    // Position in world space
    vec4 worldPosition = u_Model * vec4(position, 1.0);

    // Position in clip space
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
uniform sampler2D normalMap;

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
    // Tangent-space normal
    // --------------------------------------------------

    vec3 tangentNormal = texture(normalMap, v_TexCoord).rgb;

    // Convert [0, 1] -> [-1, 1]
    tangentNormal = tangentNormal * 2.0 - 1.0;
    tangentNormal.xy *= 2.0;

    tangentNormal = normalize(tangentNormal);



    vec3 T = vec3(1.0, 0.0, 0.0);
    vec3 B = vec3(0.0, 0.0, 1.0);
    vec3 N = vec3(0.0, 1.0, 0.0);

    mat3 TBN = mat3(T, B, N);

    // Convert tangent-space normal -> world space
    vec3 worldNormal = normalize(TBN * tangentNormal);


    // --------------------------------------------------
    // Light direction
    // --------------------------------------------------

    vec3 L = normalize(-u_LightDirection);


    // --------------------------------------------------
    // View direction
    // --------------------------------------------------

    vec3 V = normalize(u_ViewPosition - v_FragPos);


    // --------------------------------------------------
    // Diffuse lighting
    // --------------------------------------------------

    float diffuse = max(dot(worldNormal, L), 0.0);


    // --------------------------------------------------
    // Specular lighting
    // --------------------------------------------------

    vec3 R = reflect(-L, worldNormal);

    float specular = pow(
        max(dot(V, R), 0.0),
        u_Shininess
    );


    // --------------------------------------------------
    // Combine lighting
    // --------------------------------------------------

    vec3 ambient = u_AmbientColor;

    vec3 diffuseLight =
        diffuse * u_LightColor;

    vec3 specularLight =
        u_SpecularStrength *
        specular *
        u_LightColor;

    vec3 lighting =
        ambient +
        diffuseLight +
        specularLight;


    // --------------------------------------------------
    // Final color
    // --------------------------------------------------

    color = vec4(baseColor * lighting, 1.0);
}