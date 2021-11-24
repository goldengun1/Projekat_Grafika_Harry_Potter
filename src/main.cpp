//test commit!

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>
#include <learnopengl/lights.h>

//#include <rg/Shader.h>
//#include <rg/Camera.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
unsigned int loadTexture(const char *path);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f,0.0f,3.0f));
glm::vec3 defaultFront = glm::vec3 (0.0f,0.0f,-1.0f);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool spotLightOn = false;
bool wand = false;

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Harry_Potter_and_the_OpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    //stbi_set_flip_vertically_on_load(true);


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glCullFace(GL_BACK);

    Shader objShader("resources/shaders/vertexShader.vert","resources/shaders/fragmentShader.frag");
    Shader modelShader("resources/shaders/modelVertexShader.vert","resources/shaders/modelFragmentShader.frag");
    Shader blendingShader("resources/shaders/blendingVertexShader.vert","resources/shaders/blendingFragmentShader.frag");

    Model snitch(FileSystem::getPath("resources/objects/golden_snitch/model.obj"));
    snitch.SetShaderTextureNamePrefix("material.");

    Model deathlyHallows(FileSystem::getPath("resources/objects/deathly_hallows/daethly_hallows.obj"));
    deathlyHallows.SetShaderTextureNamePrefix("material.");

    Model resStone(FileSystem::getPath("resources/objects/resurrection_stone/res_stone.obj"));
    resStone.SetShaderTextureNamePrefix("material.");
    glm::vec3 res_stone_Pos = glm::vec3(0.0f, 0.0f, 0.0f);

    Model elderWand(FileSystem::getPath("resources/objects/wand/newtwand.obj"));
    elderWand.SetShaderTextureNamePrefix("material.");

    Model dementor(FileSystem::getPath("resources/objects/dementor/untitled.obj"));
    dementor.SetShaderTextureNamePrefix("material.");

    Model triwizardCup(FileSystem::getPath("resources/objects/triwizard-cup/TRIWIZARD_CUP_LP.FBX"));
    triwizardCup.SetShaderTextureNamePrefix("material.");

    PointLight pointLight;
    pointLight.setLightComponents(glm::vec3(4.0), glm::vec3(0.2f), glm::vec3(0.9f), glm::vec3(1.0f));
    DirLight dirLight;
    dirLight.setLightComponents(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.05f), glm::vec3(0.2f), glm::vec3(0.5f));
    SpotLight spotLight;
    spotLight.setLightComponents(camera.Position, camera.Front, glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(1.0f));
    spotLight.setCutOff(glm::cos(glm::radians(15.0f)), glm::cos(glm::radians(25.0f)));

    float pyramid[] = {
                //coords         //TexCoords       //Normals
            0.0f, 0.0f,0.0f,     0.5f,1.0f,     0.0f, 2.0f, 2.0f,                //V0(red)
            -1.0f,-1.0f,1.0f,    0.0f,0.0f,     0.0f, 2.0f, 2.0f,                   //V1(green)
            1.0f,-1.0f,1.0f,     1.0f,0.0f,     0.0f, 2.0f, 2.0f,                 //V2(blue)

            0.0f, 0.0f,0.0f,     0.5f,1.0f,    2.0f, 2.0f, 0.0f,                  //V0(red)
            1.0f,-1.0f,1.0f,     0.0f,0.0f,    2.0f, 2.0f, 0.0f,                  //V2(blue)
            1.0f,-1.0f,-1.0f,    1.0f,0.0f,    2.0f, 2.0f, 0.0f,                   //V3(green)

            0.0f, 0.0f,0.0f,     0.5f,1.0f,    0.0f, 2.0f, -2.0f,                //V0(red)
            1.0f,-1.0f,-1.0f,    0.0f,0.0f,    0.0f, 2.0f, -2.0f,                  //V3(green)
            -1.0f,-1.0f,-1.0f,   1.0f,0.0f,    0.0f, 2.0f, -2.0f,                    //V4(blue)

            0.0f, 0.0f,0.0f,     0.5f,1.0f,    -2.0f, 2.0f, 0.0f,                 //V0(red)
            -1.0f,-1.0f,-1.0f,   0.0f,0.0f,    -2.0f, 2.0f, 0.0f,                    //V4(blue)
            -1.0f,-1.0f,1.0f,    1.0f,0.0f,    -2.0f, 2.0f, 0.0f                    //V1
    };

    float floor[]{
            10.0f, -0.5f,  10.0f,  10.0f,  0.0f,  0.0f, 1.0f, 0.0f,
            -10.0f, -0.5f, -10.0f,   0.0f, 10.0f,  0.0f, 1.0f, 0.0f,
            -10.0f, -0.5f,  10.0f,   0.0f,  0.0f,  0.0f, 1.0f, 0.0f,

            10.0f, -0.5f,  10.0f,  10.0f,  0.0f,  0.0f, 1.0f, 0.0f,
            10.0f, -0.5f, -10.0f,  10.0f, 10.0f,   0.0f, 1.0f, 0.0f,
            -10.0f, -0.5f, -10.0f,   0.0f, 10.0f,  0.0f, 1.0f, 0.0f
    };

    //float cube[]{
    //
    //    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    //    0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
    //    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    //    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    //    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    //    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    //
    //    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    //    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    //    0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    //    0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    //    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    //    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    //
    //    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    //    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    //    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    //    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    //    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    //    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    //
    //    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    //    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    //    0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    //    0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    //    0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    //    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    //
    //    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    //    0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
    //    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    //    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    //    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    //    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    //
    //    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    //    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    //    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    //    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    //    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    //    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    //};

    unsigned pyramidVBO, pyramidVAO,floorVBO,floorVAO;
    //pyramid setup
    glGenVertexArrays(1,&pyramidVAO);

    glBindVertexArray(pyramidVAO);

    glGenBuffers(1,&pyramidVBO);
    glBindBuffer(GL_ARRAY_BUFFER,pyramidVBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(pyramid),pyramid,GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE, 8* sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE, 8* sizeof(float),(void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8* sizeof(float),(void*)(5* sizeof(float)));
    glEnableVertexAttribArray(2);

    //floor setup
    glGenVertexArrays(1,&floorVAO);
    glBindVertexArray(floorVAO);

    glGenBuffers(1,&floorVBO);
    glBindBuffer(GL_ARRAY_BUFFER,floorVBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(floor),floor,GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE, 8* sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE, 8* sizeof(float),(void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8* sizeof(float),(void*)(5* sizeof(float)));
    glEnableVertexAttribArray(2);


    unsigned int pyramidTexDiffuse = loadTexture("resources/textures/gold_diffuse.jpg");
    unsigned int pyramidTexSpecular = loadTexture("resources/textures/gold_specular.jpg");
    unsigned int floorTexDiffuse = loadTexture("resources/textures/stone_floor_diffuse.jpg");
    unsigned int floorTexSpecular = loadTexture("resources/textures/stone_floor_specular.jpg");
    unsigned int cupTexDiffuse = loadTexture("resources/objects/triwizard-cup/TRIWIZARD_CUP_BC.png");
    unsigned int cupTexSpecular = loadTexture("resources/objects/triwizard-cup/TRIWIZARD_CUP_BC.png");

    objShader.use();
    objShader.setInt("material.texture_diffuse1",0);
    objShader.setInt("material.texture_specular1", 1);

    modelShader.use();
    modelShader.setInt("material.texture_diffuse1", 0);
    modelShader.setInt("material.texture_specular1", 1);

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.2f,0.2f,0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //draw pyramid
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, pyramidTexDiffuse);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, pyramidTexSpecular);


        glm::mat4 pyramidModel = glm::mat4 (1.0f);
        pyramidModel = glm::scale(pyramidModel, glm::vec3(0.5f));
        //pyramidModel = glm::translate(pyramidModel, glm::vec3(0.0f, 0.0f, 0.0f));
        glm::mat4 view = glm::mat4 (camera.GetViewMatrix());
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),(float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        pointLight.position = glm::vec3(sin(glfwGetTime()), 0.0f, cos(glfwGetTime()));

        objShader.use();
        objShader.setLights(dirLight, pointLight, spotLight);

        objShader.setVec3("spotLight.direction", camera.Front);
        objShader.setVec3("spotLight.position", camera.Position);

        objShader.setBool("spotLightOn", spotLightOn);
        objShader.setVec3("viewPosition", camera.Position);

        objShader.setFloat("material.shininess", 16.0f);

        objShader.setMat4("Model",pyramidModel);
        objShader.setMat4("View",view);
        objShader.setMat4("Projection",projection);

        glBindVertexArray(pyramidVAO);
        glDrawArrays(GL_TRIANGLES,0,12);

        //draw floor
        glm::mat4 floorModel = glm::mat4 (1.0f);
        objShader.setMat4("Model", floorModel);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexDiffuse);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, floorTexSpecular);
        objShader.setFloat("material.shininess", 2.0f);
        glBindVertexArray(floorVAO);
        glDrawArrays(GL_TRIANGLES,0,6);

        modelShader.use();
        modelShader.setLights(dirLight, pointLight, spotLight);

        modelShader.setVec3("spotLight.direction", camera.Front);
        modelShader.setVec3("spotLight.position", camera.Position);

        modelShader.setBool("spotLightOn", spotLightOn);
        modelShader.setVec3("viewPosition", camera.Position);

        modelShader.setFloat("material.shininess", 32.0f);

        modelShader.setMat4("view",view);
        modelShader.setMat4("projection",projection);

        //draw snitch
        glm::mat4 snitchModel = glm::mat4(1.0f);
        snitchModel = glm::translate(snitchModel, glm::vec3(1.0f, 0.0f, 1.0f));
        snitchModel = glm::scale(snitchModel,glm::vec3(0.2f));
        modelShader.setMat4("model", snitchModel);
        snitch.Draw(modelShader);

        //draw deathly hallows
        glm::mat4 deathlyHallowsModel = glm::mat4 (1.0f);
        deathlyHallowsModel = glm::translate(deathlyHallowsModel,res_stone_Pos + glm::vec3(0.0f,0.1f,0.0f));
        deathlyHallowsModel = glm::rotate(deathlyHallowsModel, (float)glfwGetTime(), glm::vec3(0.0f,1.0f,0.0f));
        deathlyHallowsModel = glm::scale(deathlyHallowsModel, glm::vec3(0.0004f));
        modelShader.setMat4("model",deathlyHallowsModel);
        deathlyHallows.Draw(modelShader);

        //draw dementor
        glm::mat4 demetorModel = glm::mat4(1.0f);
        demetorModel = glm::translate(demetorModel, glm::vec3(-1.0f, sin(glfwGetTime()*4.0f)/10.0f, 1.0f));
        demetorModel = glm::scale(demetorModel, glm::vec3(0.2f));
        modelShader.setMat4("model",demetorModel);
        dementor.Draw(modelShader);

        //draw triwizard cup
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cupTexDiffuse);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, cupTexSpecular);

        glm::mat4 cupModel = glm::mat4(1.0f);
        cupModel = glm::translate(cupModel, glm::vec3(1.5f, 0.0f, 1.5f));
        cupModel = glm::rotate(cupModel, glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
        cupModel = glm::scale(cupModel, glm::vec3(0.01f));
        modelShader.setMat4("model", cupModel);
        triwizardCup.Draw(modelShader);

        //draw elderwand
        glm::mat4 elderWandModel = glm::mat4 (1.0f);
        elderWandModel = glm::translate(elderWandModel,camera.Position);
        float angle = glm::acos((glm::dot(glm::normalize(camera.Front), defaultFront)) / (glm::length(camera.Front) * glm::length(defaultFront)));
        glm::vec3 axis = glm::normalize(glm::cross(camera.Front,defaultFront));
        elderWandModel = glm::rotate(elderWandModel,angle,-axis);
        elderWandModel = glm::translate(elderWandModel,glm::vec3(0.05f,-0.05f,-0.25f));
        //elderWandModel = glm::rotate(elderWandModel, glm::radians(20.0f),glm::vec3(1.0f,1.0f,0.0f));
        elderWandModel = glm::scale(elderWandModel,glm::vec3(0.02f));
        glClear(GL_DEPTH_BUFFER_BIT);
        modelShader.setMat4("model",elderWandModel);
        elderWand.Draw(modelShader);

        blendingShader.use();
        glm::mat4 resStoneModel = glm::mat4(1.0f);
        resStoneModel = glm::translate(resStoneModel,res_stone_Pos);
        resStoneModel = glm::scale(resStoneModel,glm::vec3(0.05f));
        blendingShader.setMat4("model",resStoneModel);

        blendingShader.setLights(dirLight, pointLight, spotLight);
        blendingShader.setVec3("spotLight.direction", camera.Front);
        blendingShader.setVec3("spotLight.position", camera.Position);

        blendingShader.setBool("spotLightOn", spotLightOn);
        blendingShader.setVec3("viewPosition", camera.Position);

        blendingShader.setFloat("material.shininess", 32.0f);

        blendingShader.setMat4("view",view);
        blendingShader.setMat4("projection",projection);
        resStone.Draw(blendingShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1,&pyramidVAO);
    glDeleteVertexArrays(1,&floorVAO);
    glDeleteBuffers(1,&pyramidVBO);
    glDeleteBuffers(1,&floorVBO);
    objShader.deleteProgram();
    modelShader.deleteProgram();
    blendingShader.deleteProgram();
    glfwTerminate();
    return 0;
}


void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if(glfwGetKey(window,GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
        camera.MovementSpeed = camera.DefaultSpeed * 5.0f;
    }
    if(glfwGetKey(window,GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE){
        camera.MovementSpeed = camera.DefaultSpeed;
    }

}


void framebuffer_size_callback(GLFWwindow *window, int width, int height) {

    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset,yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {

    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        camera.CreativeMode = !camera.CreativeMode;

        if(!camera.CreativeMode)
            std::cout<<"!!!CREATIVE MODE DISABLED!!!\n";
        else
            std::cout<<"!!!CREATIVE MODE ACTIVATED!!!\n";
    }

    if (key == GLFW_KEY_L && action == GLFW_PRESS) {
        spotLightOn = !spotLightOn;

        if(spotLightOn)
            std::cout<<"!!!SPOTLIGHT ENABLED!!!\n";
        else
            std::cout<<"!!!SPOTLIGHT DISABLED!!!\n";
    }
}

unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
