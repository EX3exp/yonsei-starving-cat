#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in vec3 Tangent;
in vec3 Bitangent;
uniform sampler2D texture_diffuse1;

void main()
{    
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    vec3 lightPos = vec3(0.0, 1.0, 0.0);
    vec3 viewPos = vec3(0.0, 0.0, -1.0);
    vec3 specularColor = vec3(1.0, 1.0, 1.0);
    float shininess = 32.0;
    // Ambient lighting
    vec3 ambient =  0.9 * texture(texture_diffuse1, TexCoords).rgb;
    
    // Diffuse lighting
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = lightColor * diff * texture(texture_diffuse1, TexCoords).rgb;
    
    // Specular lighting
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularColor * spec;
    
    vec3 lighting = ambient + diffuse + specular;
    
    FragColor = vec4(lighting, 1.0);
}