#version 330 core
out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1; 
uniform sampler2D texture_height1; // ambient

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;



uniform float matarialShininess;


struct DirLight {
    bool isUsing; // when isUsing is false, do not compute and save resource
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    bool isUsing; // when isUsing is false, do not compute and save resource
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    bool isUsing; // when isUsing is false, do not compute and save resource
    vec3 position;  
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;
};


// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);


uniform DirLight dirLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;

void main()
{    
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 result = CalcDirLight(dirLight, norm, viewDir) + CalcPointLight(pointLight, norm, FragPos, viewDir) + CalcSpotLight(spotLight, norm, FragPos, viewDir);

    FragColor = vec4(result, 1.0);
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    if (light.isUsing) {
        vec3 lightDir = normalize(-light.direction);
        // diffuse shading
        float diff = max(dot(normal, lightDir), 0.0);

        // specular shading
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), matarialShininess);
        // combine results
        vec3 ambient = light.ambient * vec3(texture(texture_height1, TexCoords));
        vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
        vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords));
        return (ambient + diffuse + specular);
    }
    else {
        return 0.0;
    }
    
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    if (light.isUsing) {
        vec3 lightDir = normalize(light.position - fragPos);
        // diffuse shading
        float diff = max(dot(normal, lightDir), 0.0);
        // specular shading
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), matarialShininess);
        // attenuation
        float distance = length(light.position - fragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
        // combine results
        vec3 ambient = light.ambient * vec3(texture(texture_height1, TexCoords));
        vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
        vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords));
        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;
        return (ambient + diffuse + specular);
    }
    else {
        return 0.0;
    }
    
    
}

// calculates the color when using a spotlight.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    if (light.isUsing) {
        // ambient
        vec3 ambient = light.ambient * texture(texture_diffuse1, TexCoords).rgb;
    
        // diffuse 
        vec3 lightDir = normalize(light.position - FragPos);
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
    
        // specular
        vec3 reflectDir = reflect(-lightDir, normal);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), matarialShininess);
        vec3 specular = light.specular * spec * texture(texture_specular1, TexCoords).rgb;  
    
        // spotlight (soft edges)
        float theta = dot(lightDir, normalize(-light.direction)); 
        float epsilon = (light.cutOff - light.outerCutOff);
        float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
        diffuse  *= intensity;
        specular *= intensity;
    
        // attenuation
        float distance    = length(light.position - FragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
        ambient  *= attenuation; 
        diffuse   *= attenuation;
        specular *= attenuation;   
        
        return (ambient + diffuse + specular);
    }

    else{
        return 0.0;
    }
    
}
