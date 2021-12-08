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

unsigned int loadCubemap(vector<string> vector1);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f,0.0f,3.0f));
glm::vec3 defaultFront = glm::vec3 (0.0f,0.0f,-1.0f);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool firstMouse = true;
bool spotLightOn = false;
bool wand = false;
bool movement = true;
bool blurr = false;
//TODO add more spells for framebuffer post processing

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
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glCullFace(GL_BACK);

    //creating shaders
    Shader lightShader("resources/shaders/lightShader.vert", "resources/shaders/lightShader.frag");
    Shader objShader("resources/shaders/modelShader.vert","resources/shaders/modelShader.frag");
    Shader modelShader("resources/shaders/modelShader.vert","resources/shaders/modelShader.frag");
    Shader blendingShader("resources/shaders/blendingShader.vert","resources/shaders/blendingShader.frag");
    Shader skyboxShader("resources/shaders/skyboxShader.vert", "resources/shaders/skyboxShader.frag");
    Shader screenShader("resources/shaders/framebufferScreenShader.vert", "resources/shaders/framebufferScreenShader.frag");

    //loading models
    Model snitch(FileSystem::getPath("resources/objects/golden_snitch/model.obj"));
    snitch.SetShaderTextureNamePrefix("material.");

    Model deathlyHallows(FileSystem::getPath("resources/objects/deathly_hallows/daethly_hallows.obj"));
    deathlyHallows.SetShaderTextureNamePrefix("material.");

    Model resStone(FileSystem::getPath("resources/objects/resurrection_stone/res_stone.obj"));
    resStone.SetShaderTextureNamePrefix("material.");
    glm::vec3 res_stone_Pos = glm::vec3(-2.0f, 0.0f, 1.0f);

    Model elderWand(FileSystem::getPath("resources/objects/wand/newtwand.obj"));
    elderWand.SetShaderTextureNamePrefix("material.");

    Model dementor(FileSystem::getPath("resources/objects/dementor/untitled.obj"));
    dementor.SetShaderTextureNamePrefix("material.");

    Model triwizardCup(FileSystem::getPath("resources/objects/triwizard-cup/TRIWIZARD_CUP_LP.FBX"));
    triwizardCup.SetShaderTextureNamePrefix("material.");

    Model maze(FileSystem::getPath("resources/objects/maze/untitled.obj"));
    maze.SetShaderTextureNamePrefix("material.");
    glm::vec3 mazePos = glm::vec3(0.0f,-0.8f,-5.0f);

    //lights setup(those that are not changing)
    PointLight pointLight;
    pointLight.setLightComponents(glm::vec3(1.0f), glm::vec3(0.1f), glm::vec3(1.0f, 0.74f, 0.32f), glm::vec3(1.0f, 0.74f, 0.32f));
    PointLight bluePointLight;
    bluePointLight.setLightComponents(glm::vec3(1.0f), glm::vec3(0.1f), glm::vec3(0.0f, 0.8f, 1.0f), glm::vec3(0.0f, 0.8f, 1.0f));
    DirLight dirLight;
    dirLight.setLightComponents(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.05f), glm::vec3(0.2f), glm::vec3(0.5f));
    SpotLight spotLight;
    spotLight.setLightComponents(camera.Position, camera.Front, glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(1.0f));
    spotLight.setCutOff(glm::cos(glm::radians(15.0f)), glm::cos(glm::radians(25.0f)));

    float pyramid[] = {
            //coords         //TexCoords       //Normals
             0.0f, 0.0f,0.0f,     0.0f, 2.0f, 2.0f,    0.5f,1.0f,                   //V0(red)
            -1.0f,-1.0f,1.0f,    0.0f, 2.0f, 2.0f,    0.0f,0.0f,                      //V1(green)
            1.0f,-1.0f,1.0f,    0.0f, 2.0f, 2.0f,   1.0f,0.0f,                    //V2(blue)

            0.0f, 0.0f,0.0f,    2.0f, 2.0f, 0.0f,    0.5f,1.0f,                   //V0(red)
            1.0f,-1.0f,1.0f,    2.0f, 2.0f, 0.0f,    0.0f,0.0f,                   //V2(blue)
            1.0f,-1.0f,-1.0f,    2.0f, 2.0f, 0.0f,   1.0f,0.0f,                    //V3(green)

            0.0f, 0.0f,0.0f,    0.0f, 2.0f, -2.0f,    0.5f,1.0f,                 //V0(red)
            1.0f,-1.0f,-1.0f,   0.0f, 2.0f, -2.0f,    0.0f,0.0f,                   //V3(green)
            -1.0f,-1.0f,-1.0f,   0.0f, 2.0f, -2.0f,   1.0f,0.0f,                     //V4(blue)

            0.0f, 0.0f,0.0f,    -2.0f, 2.0f, 0.0f,   0.5f,1.0f,                   //V0(red)
            -1.0f,-1.0f,-1.0f,  -2.0f, 2.0f, 0.0f,   0.0f,0.0f,                      //V4(blue)
            -1.0f,-1.0f,1.0f,    -2.0f, 2.0f, 0.0f,   1.0f,0.0f                     //V1
    };

    float floor[]{
            10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f, 10.0f,  0.0f,
            -10.0f, -0.5f, -10.0f, 0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
            -10.0f, -0.5f,  10.0f, 0.0f, 1.0f, 0.0f,   0.0f,  0.0f,

            10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f, 10.0f,  0.0f,
            10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f, 10.0f, 10.0f,
            -10.0f, -0.5f, -10.0f, 0.0f, 1.0f, 0.0f ,   0.0f, 10.0f
    };

    float cube[]{
            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,

            -0.5f, -0.5f,  0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f, -0.5f,  0.5f,

            -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,

            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,

            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f, -0.5f,

            -0.5f,  0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f
    };

    float skyboxVertices[] = {
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
    };

    float screenQuad[] = {
            // positions                // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
            1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
            1.0f, -1.0f,  1.0f, 0.0f,
            1.0f,  1.0f,  1.0f, 1.0f
    };

    glm::vec3 pointLightPositions[] = {
            glm::vec3(5.0f, 10.0f, 2.0f),
            glm::vec3(-5.0f, 10.0f, 2.0f),
            glm::vec3(5.0f, 10.0f, -8.0f),
            glm::vec3(-5.0f, 10.0f, -8.0f)
    };

    unsigned pyramidVBO, pyramidVAO;
    unsigned floorVBO,floorVAO;
    unsigned lightVAO, lightVBO;
    unsigned skyboxVAO, skyboxVBO;
    unsigned screenVAO,screenVBO;

    //skybox setup
    glGenVertexArrays(1,&skyboxVAO);

    glBindVertexArray(skyboxVAO);

    glGenBuffers(1,&skyboxVBO);
    glBindBuffer(GL_ARRAY_BUFFER,skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(skyboxVertices),&skyboxVertices,GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE, 3 * sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    //light setup
    glGenVertexArrays(1,&lightVAO);

    glBindVertexArray(lightVAO);

    glGenBuffers(1,&lightVBO);
    glBindBuffer(GL_ARRAY_BUFFER,lightVBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(cube),&cube,GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE, 3 * sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    //pyramid setup
    glGenVertexArrays(1,&pyramidVAO);

    glBindVertexArray(pyramidVAO);

    glGenBuffers(1,&pyramidVBO);
    glBindBuffer(GL_ARRAY_BUFFER,pyramidVBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(pyramid),pyramid,GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE, 8 * sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE, 8 * sizeof(float),(void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),(void*)(6* sizeof(float)));
    glEnableVertexAttribArray(2);

    //floor setup
    glGenVertexArrays(1,&floorVAO);
    glBindVertexArray(floorVAO);

    glGenBuffers(1,&floorVBO);
    glBindBuffer(GL_ARRAY_BUFFER,floorVBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(floor),floor,GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE, 8 * sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE, 8 * sizeof(float),(void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),(void*)(6* sizeof(float)));
    glEnableVertexAttribArray(2);

    //screen quad setup
    glGenVertexArrays(1,&screenVAO);
    glBindVertexArray(screenVAO);

    glGenBuffers(1,&screenVBO);
    glBindBuffer(GL_ARRAY_BUFFER,screenVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenQuad),screenQuad,GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);



    unsigned int pyramidTexDiffuse = loadTexture("resources/textures/gold_diffuse.jpg");
    unsigned int pyramidTexSpecular = loadTexture("resources/textures/gold_specular.jpg");
    unsigned int floorTexDiffuse = loadTexture("resources/textures/earth.jpg");
    unsigned int floorTexSpecular = loadTexture("resources/textures/earth.jpg");
    unsigned int cupTexDiffuse = loadTexture("resources/objects/triwizard-cup/TRIWIZARD_CUP_BC.png");
    unsigned int cupTexSpecular = loadTexture("resources/objects/triwizard-cup/TRIWIZARD_CUP_BC.png");

    vector<std::string> faces {
        FileSystem::getPath("resources/textures/skybox/right.png"),
        FileSystem::getPath("resources/textures/skybox/left.png"),
        FileSystem::getPath("resources/textures/skybox/top.png"),
        FileSystem::getPath("resources/textures/skybox/bottom.png"),
        FileSystem::getPath("resources/textures/skybox/front.png"),
        FileSystem::getPath("resources/textures/skybox/back.png")
    };
    unsigned int cubemapTexture = loadCubemap(faces);

    objShader.use();
    objShader.setInt("material.texture_diffuse1",0);
    objShader.setInt("material.texture_specular1", 1);

    modelShader.use();
    modelShader.setInt("material.texture_diffuse1", 0);
    modelShader.setInt("material.texture_specular1", 1);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    screenShader.use();
    screenShader.setInt("screenTexture",0);

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    unsigned framebuffer;
    glGenFramebuffers(1,&framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER,framebuffer);

    unsigned textureColorBuffer;
    glGenTextures(1,&textureColorBuffer);
    glBindTexture(GL_TEXTURE_2D,textureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

    unsigned renderbuffer;
    glGenRenderbuffers(1,&renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER,renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "!!!FRAMEBUFFER CREATING FAILED!!!" << endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        //binding framebuffer before we start drawing everything
        glBindFramebuffer(GL_FRAMEBUFFER,framebuffer);
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.2f,0.2f,0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //draw pyramid
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, pyramidTexDiffuse);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, pyramidTexSpecular);

        glm::mat4 pyramidModel = glm::mat4 (1.0f);
        pyramidModel = glm::translate(pyramidModel, res_stone_Pos);
        pyramidModel = glm::scale(pyramidModel, glm::vec3(0.5f));
        glm::mat4 view = glm::mat4 (camera.GetViewMatrix());
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),(float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        //pointLight.position = glm::vec3(sin(glfwGetTime()), 0.0f, cos(glfwGetTime())) + res_stone_Pos;
        bluePointLight.position = glm::vec3(sin(glfwGetTime()) * 0.2f, 1.0f, cos(glfwGetTime()) * 0.2f) + mazePos;

        objShader.use();
        objShader.setLights(dirLight, pointLight, bluePointLight, spotLight);

        objShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        objShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        objShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        objShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        objShader.setVec3("pointLights[4].position", bluePointLight.position);

        objShader.setVec3("spotLight.direction", camera.Front);
        objShader.setVec3("spotLight.position", camera.Position);

        objShader.setBool("spotLightOn", spotLightOn);
        objShader.setVec3("viewPosition", camera.Position);

        objShader.setFloat("material.shininess", 16.0f);

        objShader.setMat4("model",pyramidModel);
        objShader.setMat4("view",view);
        objShader.setMat4("projection",projection);

        glBindVertexArray(pyramidVAO);
        glDrawArrays(GL_TRIANGLES,0,12);
        //draw floor
        glm::mat4 floorModel = glm::mat4 (1.0f);
        floorModel = glm::translate(floorModel, glm::vec3(0.0f,-0.1f,-3.0f));
        floorModel = glm::scale(floorModel, glm::vec3(0.7f));
        objShader.setMat4("model", floorModel);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexDiffuse);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, floorTexSpecular);
        objShader.setFloat("material.shininess", 2.0f);
        glBindVertexArray(floorVAO);
        glDrawArrays(GL_TRIANGLES,0,6);

        modelShader.use();
        modelShader.setLights(dirLight, pointLight, bluePointLight, spotLight);

        modelShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        modelShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        modelShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        modelShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        modelShader.setVec3("pointLights[4].position", bluePointLight.position);

        modelShader.setVec3("spotLight.direction", camera.Front);
        modelShader.setVec3("spotLight.position", camera.Position);

        modelShader.setBool("spotLightOn", spotLightOn);
        modelShader.setVec3("viewPosition", camera.Position);

        modelShader.setFloat("material.shininess", 32.0f);

        modelShader.setMat4("view",view);
        modelShader.setMat4("projection",projection);

        //draw snitch
        glm::mat4 snitchModel = glm::mat4(1.0f);
        snitchModel = glm::translate(snitchModel, glm::vec3(0.2f, 0.0f, -1.0f) + camera.Position);
        if (movement) {
            snitchModel = glm::translate(snitchModel, glm::vec3(cos(glfwGetTime()) / 3.0f,
                                                                sin(glfwGetTime()) * cos(glfwGetTime()) / 3.0f, 0.0f));
        }
        snitchModel = glm::scale(snitchModel,glm::vec3(0.1f));
        modelShader.setMat4("model", snitchModel);
        snitch.Draw(modelShader);

        //draw deathly hallows
        glm::mat4 deathlyHallowsModel = glm::mat4 (1.0f);
        deathlyHallowsModel = glm::translate(deathlyHallowsModel,res_stone_Pos + glm::vec3(0.0f,0.1f,0.0f));
        deathlyHallowsModel = glm::rotate(deathlyHallowsModel, (float)glfwGetTime(), glm::vec3(0.0f,1.0f,0.0f));
        deathlyHallowsModel = glm::scale(deathlyHallowsModel, glm::vec3(0.0004f));
        modelShader.setMat4("model",deathlyHallowsModel);
        deathlyHallows.Draw(modelShader);

        //draw dementors
        glm::vec3 levitatingFunc = glm::vec3 (0.0f, sin(glfwGetTime()*4.0f)/10.0f,0.0f);
        for(int i = 0;i<4;++i){
            glm::mat4 dementorModel = glm::mat4(1.0f);
            if(i == 3){
                dementorModel = glm::translate(dementorModel, levitatingFunc + glm::vec3(-1.0f,0.0f,0.0f));
                dementorModel = glm::rotate(dementorModel,glm::radians(20.0f),glm::vec3(0.0f,1.0f,0.0f));
                dementorModel = glm::scale(dementorModel, glm::vec3(0.2f));
                modelShader.setMat4("model",dementorModel);
            } else{
                dementorModel = glm::translate(dementorModel, mazePos + glm::vec3(0.0f,(float)i+2,-0.5f) + levitatingFunc);
                dementorModel = glm::translate(dementorModel,glm::vec3(sin(glfwGetTime())*i*tan(glfwGetTime()),0.0f,cos(glfwGetTime())*i));
                dementorModel = glm::rotate(dementorModel,glm::radians(90.0f),glm::vec3(1.0f,0.0f,0.0f));
                dementorModel = glm::scale(dementorModel, glm::vec3(0.2f));
                modelShader.setMat4("model",dementorModel);
            }
            dementor.Draw(modelShader);
        }

        //draw maze
        glm::mat4 mazeModel = glm::mat4 (1.0f);
        mazeModel = glm::translate(mazeModel,mazePos);
        mazeModel = glm::scale(mazeModel,glm::vec3(1.5f));
        modelShader.setMat4("model",mazeModel);
        maze.Draw(modelShader);

        //draw triwizard cup
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cupTexDiffuse);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, cupTexSpecular);

        glm::mat4 cupModel = glm::mat4(1.0f);
        cupModel = glm::translate(cupModel, mazePos + glm::vec3(0.0f,0.75f,0.0f));
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
        elderWandModel = glm::rotate(elderWandModel, glm::radians(20.0f),glm::vec3(1.0f,1.0f,0.0f));
        elderWandModel = glm::scale(elderWandModel,glm::vec3(0.02f));
        //glClear(GL_DEPTH_BUFFER_BIT);
        modelShader.setMat4("model",elderWandModel);
        elderWand.Draw(modelShader);


        //draw res stone
        blendingShader.use();
        glm::mat4 resStoneModel = glm::mat4(1.0f);
        resStoneModel = glm::translate(resStoneModel,res_stone_Pos);
        resStoneModel = glm::scale(resStoneModel,glm::vec3(0.05f));
        blendingShader.setMat4("model",resStoneModel);

        blendingShader.setLights(dirLight, pointLight, bluePointLight, spotLight);

        objShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        objShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        objShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        objShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        objShader.setVec3("pointLights[4].position", bluePointLight.position);

        blendingShader.setVec3("spotLight.direction", camera.Front);
        blendingShader.setVec3("spotLight.position", camera.Position);

        blendingShader.setBool("spotLightOn", spotLightOn);
        blendingShader.setVec3("viewPosition", camera.Position);

        blendingShader.setFloat("material.shininess", 32.0f);

        blendingShader.setMat4("view",view);
        blendingShader.setMat4("projection",projection);
        resStone.Draw(blendingShader);

        //draw light
        lightShader.use();

        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);
        lightShader.setVec3("lightColor", pointLight.diffuse);

        glDisable(GL_CULL_FACE);
        glBindVertexArray(lightVAO);
        for (unsigned int i = 0; i < 4; i++)
        {
            glm::mat4 lightModel = glm::mat4(1.0f);
            lightModel = glm::translate(lightModel, pointLightPositions[i]);
            lightModel = glm::scale(lightModel, glm::vec3(0.2f)); // Make it a smaller cube
            lightShader.setMat4("model", lightModel);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glEnable(GL_CULL_FACE);

        //draw skybox
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();

        skyboxShader.setMat4("view", glm::mat4(glm::mat3(view)));
        skyboxShader.setMat4("projection", projection);

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        glBindFramebuffer(GL_FRAMEBUFFER,0);
        glDisable(GL_DEPTH_TEST);
        glClearColor(1.0f,0.0f,0.0f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        screenShader.use();
        screenShader.setBool("blurr",blurr);
        glBindVertexArray(screenVAO);
        glBindTexture(GL_TEXTURE_2D,textureColorBuffer);
        glDrawArrays(GL_TRIANGLES,0,6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1,&pyramidVAO);
    glDeleteVertexArrays(1,&floorVAO);
    glDeleteVertexArrays(1,&lightVAO);
    glDeleteVertexArrays(1,&screenVAO);
    glDeleteBuffers(1,&pyramidVBO);
    glDeleteBuffers(1,&floorVBO);
    glDeleteBuffers(1,&lightVBO);
    glDeleteBuffers(1,&screenVBO);
    objShader.deleteProgram();
    modelShader.deleteProgram();
    blendingShader.deleteProgram();
    lightShader.deleteProgram();
    skyboxShader.deleteProgram();
    screenShader.deleteProgram();
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
        blurr = true;
    }
    if(glfwGetKey(window,GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE){
        camera.MovementSpeed = camera.DefaultSpeed;
        blurr = false;
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

    if (key == GLFW_KEY_M && action == GLFW_PRESS) {
        movement = !movement;

        if(movement)
            std::cout<<"!!!SNITCH IS FLYING!!! CATCH IT!!!\n";
        else
            std::cout<<"!!!SNITCH IS RESTING!!! NOW IT IS EASY.. \n";
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

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
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

unsigned int loadCubemap(vector<string> faces)
{
    unsigned int textureId;

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

    int width, height, nrChannels;
    unsigned char* data;

    for( int i = 0; i < faces.size(); ++i){
        data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data){
            GLenum format;
            if (nrChannels == 1)
                format = GL_RED;
            else if (nrChannels == 3)
                format = GL_RGB;
            else if (nrChannels == 4)
                format = GL_RGBA;

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        }else{
            std::cerr<<"FAILED TO LOAD CUBE MAP TEXTURE FACE"<<std::endl;
            return -1;
        }
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureId;
}