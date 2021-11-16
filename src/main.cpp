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
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f,0.0f,3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool spotLightOn = false;

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
    stbi_set_flip_vertically_on_load(true);


    glEnable(GL_DEPTH_TEST);

    Shader pyramidShader("resources/shaders/vertexShader.vert","resources/shaders/fragmentShader.frag");
    Shader modelShader("resources/shaders/modelVertexShader.vert","resources/shaders/modelFragmentShader.frag");

    Model snitch(FileSystem::getPath("resources/objects/golden_snitch/model.obj"));
    snitch.SetShaderTextureNamePrefix("material.");

    PointLight pointLight;
    pointLight.setLightComponents(glm::vec3(4.0), glm::vec3(0.2f), glm::vec3(0.9f), glm::vec3(1.0f));
    DirLight dirLight;
    dirLight.setLightComponents(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.05f), glm::vec3(0.2f), glm::vec3(0.5f));
    SpotLight spotLight;
    spotLight.setLightComponents(camera.Position, camera.Front, glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(1.0f));
    spotLight.setCutOff(glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.0f)));

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

    unsigned VBO, res_stoneVAO;
    glGenVertexArrays(1,&res_stoneVAO);

    glBindVertexArray(res_stoneVAO);

    glGenBuffers(1,&VBO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(pyramid),pyramid,GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE, 8* sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE, 8* sizeof(float),(void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8* sizeof(float),(void*)(5* sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int pyramidTexDiffuse = loadTexture("resources/textures/gold_diffuse.jpg");
    unsigned int pyramidTexSpecular = loadTexture("resources/textures/gold_specular.jpg");

    pyramidShader.use();
    pyramidShader.setInt("material.texture_diffuse1",0);
    pyramidShader.setInt("material.texture_specular1", 1);

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.2f,0.2f,0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, pyramidTexDiffuse);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, pyramidTexSpecular);


        glm::mat4 pyramidModel = glm::mat4 (1.0f);
        //pyramidModel = glm::translate(pyramidModel, glm::vec3(3.0f));
        //pyramidModel = rotate(pyramidModel,(float)glfwGetTime(),glm::vec3(0.0f,1.0f,0.0f));
        glm::mat4 view = glm::mat4 (camera.GetViewMatrix());
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),(float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        pointLight.position = glm::vec3(sin(glfwGetTime()), 0.0f, cos(glfwGetTime()));

        pyramidShader.use();
        pyramidShader.setLights(dirLight, pointLight, spotLight);

        pyramidShader.setVec3("spotLight.direction", camera.Front);
        pyramidShader.setVec3("spotLight.position", camera.Position);

        pyramidShader.setBool("spotLightOn", spotLightOn);
        pyramidShader.setVec3("viewPosition", camera.Position);

        pyramidShader.setFloat("material.shininess", 16.0f);

        pyramidShader.setMat4("Model",pyramidModel);
        pyramidShader.setMat4("View",view);
        pyramidShader.setMat4("Projection",projection);

        glBindVertexArray(res_stoneVAO);
        glDrawArrays(GL_TRIANGLES,0,12);

        modelShader.use();
        modelShader.setLights(dirLight, pointLight, spotLight);

        modelShader.setVec3("spotLight.direction", camera.Front);
        modelShader.setVec3("spotLight.position", camera.Position);

        modelShader.setBool("spotLightOn", spotLightOn);
        modelShader.setVec3("viewPosition", camera.Position);

        modelShader.setFloat("material.shininess", 32.0f);

        glm::mat4 snitchModel = glm::mat4(1.0f);
        snitchModel = glm::translate(pyramidModel, glm::vec3(0.0f, 0.3f, -0.15f));
        modelShader.setMat4("model", snitchModel);
        modelShader.setMat4("view",view);
        modelShader.setMat4("projection",projection);
        snitch.Draw(modelShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1,&res_stoneVAO);
    glDeleteBuffers(1,&VBO);
    pyramidShader.deleteProgram();
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
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

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset,yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
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
