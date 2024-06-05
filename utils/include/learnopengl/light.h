#ifndef LIGHT_H
#define LIGHT_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


struct Dir_Light {
    bool isUsing = false; // when isUsing is false, do not compute and save resource
    glm::vec3 direction = glm::vec3(0.06, 0.08, 0.71);

    float ambient= 0.25;
    float diffuse= 0.7;
    float specular= 1.f;
};

struct Point_Light {
    bool isUsing = false; // when isUsing is false, do not compute and save resource
    glm::vec3 position = glm::vec3(0.06, 0.08, 0.71);

    float constant = 0.9f;
    float linear = 0.09f;
    float quadratic = 0.032f;

    float ambient = 0.25;
    float diffuse = 0.7;
    float specular = 1.f;
};

struct Spot_Light {
    bool isUsing = false; // when isUsing is false, do not compute and save resource
    glm::vec3 position = glm::vec3(0.06, 0.08, 0.71);
    glm::vec3 direction = glm::vec3(0.06, 0.08, 0.71);

    float cutOff = 4.f;
    float outerCutOff = 6.f;

    float ambient = 0.25;
    float diffuse = 0.7;
    float specular = 1.f;

    float constant = 0.9f;
    float linear = 0.09f;
    float quadratic = 0.032f;
};

class Light {
public:
    Light(float shininess=32.f) : materialShininess(shininess) 
    {
    }

    void setDirectionalLight(glm::vec3 direction, float ambient = 0.25f, float diffuse = 0.7f, float specular = 1.f) 
    {
        dir_Light.isUsing = true;
        dir_Light.ambient = ambient;
        dir_Light.diffuse = diffuse;
        dir_Light.specular = specular;
    }

    void setPointLight(glm::vec3 position, float constant = 0.9f, float linear = 0.09f, float quadratic = 0.032f, float ambient = 0.25f, float diffuse = 0.7f, float specular = 1.f) 
    {
		point_Light.isUsing = true;
		point_Light.position = position;
		point_Light.constant = constant;
		point_Light.linear = linear;
		point_Light.quadratic = quadratic;
		point_Light.ambient = ambient;
		point_Light.diffuse = diffuse;
		point_Light.specular = specular;
	}

    void setSpotLight(glm::vec3 position, glm::vec3 direction, float cutOff = 4.f, float outerCutOff = 6.f, float constant = 0.9f, float linear = 0.09f, float quadratic = 0.032f, float ambient = 0.25f, float diffuse = 0.7f, float specular = 1.f) 
    {
        spot_Light.isUsing = true;
        spot_Light.position = position;
        spot_Light.direction = direction;
        spot_Light.cutOff = cutOff;
        spot_Light.outerCutOff = outerCutOff;
        spot_Light.constant = constant;
        spot_Light.linear = linear;
        spot_Light.quadratic = quadratic;
        spot_Light.ambient = ambient;
        spot_Light.diffuse = diffuse;
        spot_Light.specular = specular;
    }


    void addLightToShader(Shader& shader) 
    {
        shader.use();
        shader.setFloat("matarialShininess", materialShininess);

        // set dirLight
        shader.setBool("dirLight.isUsing", dir_Light.isUsing);
        shader.setVec3("dirLight.direction", dir_Light.direction);

        shader.setVec3("dirLight.ambient", glm::vec3(dir_Light.ambient, dir_Light.ambient, dir_Light.ambient));
        shader.setVec3("dirLight.diffuse", glm::vec3(dir_Light.diffuse, dir_Light.diffuse, dir_Light.diffuse));
        shader.setVec3("dirLight.specular", glm::vec3(dir_Light.specular, dir_Light.specular, dir_Light.specular));


        // set pointLight
        shader.setBool("pointLight.isUsing", point_Light.isUsing);
        shader.setVec3("pointLight.position", point_Light.position);

        shader.setFloat("pointLight.constant", point_Light.constant);
        shader.setFloat("pointLight.linear", point_Light.linear);
        shader.setFloat("pointLight.quadratic", point_Light.quadratic);

        shader.setVec3("pointLight.ambient", glm::vec3(point_Light.ambient, point_Light.ambient, point_Light.ambient));
        shader.setVec3("pointLight.diffuse", glm::vec3(point_Light.diffuse, point_Light.diffuse, point_Light.diffuse));
        shader.setVec3("pointLight.specular", glm::vec3(point_Light.specular, point_Light.specular, point_Light.specular));


        // set spotLight
        shader.setBool("spotLight.isUsing", spot_Light.isUsing);
        shader.setVec3("spotLight.position", spot_Light.position);
        shader.setVec3("spotLight.direction", spot_Light.direction);

        shader.setFloat("spotLight.cutOff", spot_Light.cutOff);
        shader.setFloat("spotLight.outerCutOff", spot_Light.outerCutOff);

        shader.setFloat("spotLight.constant", spot_Light.constant);
        shader.setFloat("spotLight.linear", spot_Light.linear);
        shader.setFloat("spotLight.quadratic", spot_Light.quadratic);

        shader.setVec3("spotLight.ambient", glm::vec3(spot_Light.ambient, spot_Light.ambient, spot_Light.ambient));
        shader.setVec3("spotLight.diffuse", glm::vec3(spot_Light.diffuse, spot_Light.diffuse, spot_Light.diffuse));
        shader.setVec3("spotLight.specular", glm::vec3(spot_Light.specular, spot_Light.specular, spot_Light.specular));
    }

    
private:
    float materialShininess;
    Dir_Light dir_Light = Dir_Light();
    Point_Light point_Light = Point_Light();
    Spot_Light spot_Light = Spot_Light();
    
};
#endif
