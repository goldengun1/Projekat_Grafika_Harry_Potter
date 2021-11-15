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

//#include <rg/Shader.h>
//#include <rg/Camera.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

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

struct PointLight {
    glm::vec3 position;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

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

    Shader shader("resources/shaders/vertexShader.vert","resources/shaders/fragmentShader.frag");
    Shader modelShader("resources/shaders/modelVertexShader.vert","resources/shaders/modelFragmentShader.frag");

    Model ourModel(FileSystem::getPath("resources/objects/golden_snitch/model.obj"));
    ourModel.SetShaderTextureNamePrefix("material.");

    PointLight pointLight;
    pointLight.ambient = glm::vec3(0.2f);
    pointLight.diffuse = glm::vec3(0.9f);
    pointLight.specular = glm::vec3(1.0, 1.0, 1.0);
    pointLight.constant = 1.0f;
    pointLight.linear = 0.09f;
    pointLight.quadratic = 0.032f;
    pointLight.position = glm::vec3(4.0, 4.0, 4.0);

    float res_stone[] = {
                //coords         //TexCoords       //Normals
            0.0f, 0.0f,0.0f,     0.5f,1.0f,                     //V0(red)
            -1.0f,-1.0f,1.0f,    0.0f,0.0f,                        //V1(green)
            1.0f,-1.0f,1.0f,     1.0f,0.0f,                      //V2(blue)

            0.0f, 0.0f,0.0f,     0.5f,1.0f,                      //V0(red)
            1.0f,-1.0f,1.0f,     0.0f,0.0f,                      //V2(blue)
            1.0f,-1.0f,-1.0f,    1.0f,0.0f,                       //V3(green)

            0.0f, 0.0f,0.0f,     0.5f,1.0f,                     //V0(red)
            1.0f,-1.0f,-1.0f,    0.0f,0.0f,                       //V3(green)
            -1.0f,-1.0f,-1.0f,   1.0f,0.0f,                         //V4(blue)

            0.0f, 0.0f,0.0f,     0.5f,1.0f,                      //V0(red)
            -1.0f,-1.0f,-1.0f,   0.0f,0.0f,                         //V4(blue)
            -1.0f,-1.0f,1.0f,    1.0f,0.0f,                        //V1

            //lower pyramid
            0.0f, -2.0f,0.0f,     0.5f,1.0f,                     //V0(red)
            -1.0f,-1.0f,1.0f,    0.0f,0.0f,                        //V1(green)
            1.0f,-1.0f,1.0f,     1.0f,0.0f,                      //V2(blue)

            0.0f, -2.0f,0.0f,     0.5f,1.0f,                      //V0(red)
            1.0f,-1.0f,1.0f,     0.0f,0.0f,                      //V2(blue)
            1.0f,-1.0f,-1.0f,    1.0f,0.0f,                       //V3(green)

            0.0f, -2.0f,0.0f,     0.5f,1.0f,                     //V0(red)
            1.0f,-1.0f,-1.0f,    0.0f,0.0f,                       //V3(green)
            -1.0f,-1.0f,-1.0f,   1.0f,0.0f,                         //V4(blue)

            0.0f, -2.0f,0.0f,     0.5f,1.0f,                      //V0(red)
            -1.0f,-1.0f,-1.0f,   0.0f,0.0f,                         //V4(blue)
            -1.0f,-1.0f,1.0f,    1.0f,0.0f                        //V1
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
    glBufferData(GL_ARRAY_BUFFER,sizeof(res_stone),res_stone,GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,5* sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,5* sizeof(float),(void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    //textures
    //Ako se koristi ovako napravljen resurection_stone onda moraju da se napisu i normale vertex-a zbog osvetljenja,
    //dok kod ucitavanja modela ne mora.(VIDETI STA JE BOLJE)
    unsigned int tex0;
    glGenTextures(1,&tex0);

    int width,height,nrComponents;
    unsigned char* data = stbi_load(FileSystem::getPath("resources/textures/texture_diffuse.png").c_str(),&width,&height,&nrComponents,0);
    if(data){
        GLenum format;
        if(nrComponents == 1)
            format = GL_RED;
        else if(nrComponents == 3)
            format = GL_RGB;
        else if(nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D,tex0);
        glTexImage2D(GL_TEXTURE_2D,0,format,width,height,0,format,GL_UNSIGNED_BYTE,data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GLFW_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GLFW_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

        stbi_image_free(data);
    }
    else{
        std::cerr<<"FAILED TO LOAD TEXTURE\n";
        stbi_image_free(data);
        //glfwTerminate();
    }

    shader.use();
    shader.setInt("t0",0);

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        // render
        // ------
        glClearColor(0.2f,0.2f,0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,tex0);

        // don't forget to enable shader before setting uniforms
        glm::mat4 model = glm::mat4 (1.0f);
        //model = rotate(model,(float)glfwGetTime(),glm::vec3(0.0f,1.0f,0.0f));
        glm::mat4 view = glm::mat4 (camera.GetViewMatrix());
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),(float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        shader.use();
        shader.setMat4("Model",model);
        shader.setMat4("View",view);
        shader.setMat4("Projection",projection);

        glBindVertexArray(res_stoneVAO);
        glDrawArrays(GL_TRIANGLES,0,36);

        modelShader.use();
        pointLight.position = glm::vec3(0.0f,sin(glfwGetTime()),cos(glfwGetTime()));
        modelShader.setVec3("pointLight.position", pointLight.position);
        modelShader.setVec3("pointLight.ambient", pointLight.ambient);
        modelShader.setVec3("pointLight.diffuse", pointLight.diffuse);
        modelShader.setVec3("pointLight.specular", pointLight.specular);
        modelShader.setFloat("pointLight.constant", pointLight.constant);
        modelShader.setFloat("pointLight.linear", pointLight.linear);
        modelShader.setFloat("pointLight.quadratic", pointLight.quadratic);
        modelShader.setVec3("viewPosition", camera.Position);
        modelShader.setFloat("material.shininess", 32.0f);

        modelShader.setMat4("model",model);
        modelShader.setMat4("view",view);
        modelShader.setMat4("projection",projection);
        ourModel.Draw(modelShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1,&res_stoneVAO);
    glDeleteBuffers(1,&VBO);
    shader.deleteProgram();
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

}
