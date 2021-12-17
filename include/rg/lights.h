//
// Created by stefanija on 16.11.21..
//

#ifndef PROJECT_BASE_LIGHTS_H
#define PROJECT_BASE_LIGHTS_H

struct DirLight {
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    void setLightComponents(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular){
        this->direction = direction;
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
    }
};

struct PointLight {
    glm::vec3 position;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;

    void setLightComponents(glm::vec3 pos, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular){
        this->position = pos;
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
    }

    void setTerms(float constant, float linear, float quadratic){
        this->constant = constant;
        this->linear = linear;
        this->quadratic = quadratic;
    };
};

struct SpotLight {
    glm::vec3 position;
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;

    float cutOff;
    float outerCutOff;

    void setLightComponents(glm::vec3 direction, glm::vec3 pos, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular){
        this->direction = direction;
        this->position = pos;
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
    }

    void setTerms(float constant, float linear, float quadratic){
        this->constant = constant;
        this->linear = linear;
        this->quadratic = quadratic;
    };

    void setCutOff(float cutOff, float outerCutOff){
        this->cutOff = cutOff;
        this->outerCutOff = outerCutOff;
    }
};

#endif //PROJECT_BASE_LIGHTS_H
