#include <iostream>
#include <SDL.h>
#include "glad/gl.h"
#include <glm.hpp>
//#include "Shader.h"
//#include "Texture.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "Transform.h"
#include <../glm/gtc/matrix_transform.hpp>
//#include <GL/glew.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void mouse_callback(SDL_Event* event);
void scroll_callback(SDL_Event* event);
bool running = true;
//SHADER
class Shader
{
public:
    unsigned int ID;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr,
        const char* tessControlPath = nullptr, const char* tessEvalPath = nullptr)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::string geometryCode;
        std::string tessControlCode;
        std::string tessEvalCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        std::ifstream gShaderFile;
        std::ifstream tcShaderFile;
        std::ifstream teShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        tcShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        teShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
            // if geometry shader path is present, also load a geometry shader
            if (geometryPath != nullptr)
            {
                gShaderFile.open(geometryPath);
                std::stringstream gShaderStream;
                gShaderStream << gShaderFile.rdbuf();
                gShaderFile.close();
                geometryCode = gShaderStream.str();
            }
            if (tessControlPath != nullptr) {
                tcShaderFile.open(tessControlPath);
                std::stringstream tcShaderStream;
                tcShaderStream << tcShaderFile.rdbuf();
                tcShaderFile.close();
                tessControlCode = tcShaderStream.str();
            }
            if (tessEvalPath != nullptr) {
                teShaderFile.open(tessEvalPath);
                std::stringstream teShaderStream;
                teShaderStream << teShaderFile.rdbuf();
                teShaderFile.close();
                tessEvalCode = teShaderStream.str();
            }
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: "
                << e.what() << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // if geometry shader is given, compile geometry shader
        unsigned int geometry;
        if (geometryPath != nullptr)
        {
            const char* gShaderCode = geometryCode.c_str();
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &gShaderCode, NULL);
            glCompileShader(geometry);
            checkCompileErrors(geometry, "GEOMETRY");
        }
        // if tessellation shader is given, compile tessellation shader
        unsigned int tessControl;
        if (tessControlPath != nullptr)
        {
            const char* tcShaderCode = tessControlCode.c_str();
            tessControl = glCreateShader(GL_TESS_CONTROL_SHADER);
            glShaderSource(tessControl, 1, &tcShaderCode, NULL);
            glCompileShader(tessControl);
            checkCompileErrors(tessControl, "TESS_CONTROL");
        }
        unsigned int tessEval;
        if (tessEvalPath != nullptr)
        {
            const char* teShaderCode = tessEvalCode.c_str();
            tessEval = glCreateShader(GL_TESS_EVALUATION_SHADER);
            glShaderSource(tessEval, 1, &teShaderCode, NULL);
            glCompileShader(tessEval);
            checkCompileErrors(tessEval, "TESS_EVALUATION");
        }
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        if (geometryPath != nullptr)
            glAttachShader(ID, geometry);
        if (tessControlPath != nullptr)
            glAttachShader(ID, tessControl);
        if (tessEvalPath != nullptr)
            glAttachShader(ID, tessEval);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if (geometryPath != nullptr)
            glDeleteShader(geometry);

    }
    // activate the shader
    // ------------------------------------------------------------------------
    void use()
    {
        glUseProgram(ID);
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setVec2(const std::string& name, const glm::vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec2(const std::string& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string& name, const glm::vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string& name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string& name, const glm::mat2& mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string& name, const glm::mat3& mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string& name, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};


//</SHADER

/////////////////////////////////////
///////////////CAMERA///////////////
///////////////CAMERA///////////////
/////////////////////////////////////
// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 100.5f;
const float SENSITIVITY = 0.01f;
const float ZOOM = 20.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw/100.0f;
        Pitch = pitch/100.0f;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity; 
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));
    }
};


/////////////////////////////////////
///////////////CAMERA///////////////</>
///////////////CAMERA///////////////</>
/////////////////////////////////////






////////////////INPUT////////////////
  
////////////////</INPUT>/////////////////



// settings
const unsigned int SCR_WIDTH = 1800;
const unsigned int SCR_HEIGHT = 720;
const unsigned int NUM_PATCH_PTS = 4;

// camera - give pretty starting point
Camera camera(glm::vec3(1.0f, 1.0f, 10.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    -128.1f, -42.4f);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
////////////////////////////////////
////////////////////////////////////


int main(int argc, char* argv[])
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }
    std::cout << "Camera Pos: " << camera.Position.x << ", " << camera.Position.y << ", " << camera.Position.z << std::endl; 
    std::cout << "Camera Front: " << camera.Front.x << ", " << camera.Front.y << ", " << camera.Front.z << std::endl;


    // Set OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Create a window
    SDL_Window* window = SDL_CreateWindow("Flying Dragons", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1800, 720, SDL_WINDOW_OPENGL);
    if (!window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // Create an OpenGL context
    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context) {
        std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
   
        

    // Initialize GLAD
    
        if (!gladLoaderLoadGL()){
        std::cerr << "Failed to initialize GLAD" << std::endl;
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

        GLint maxTessLevel;
        glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessLevel);
        std::cout << "Max available tess level: " << maxTessLevel << std::endl;

        // configure global opengl state
        // -----------------------------
        glEnable(GL_DEPTH_TEST);

        // build and compile our shader program
        // ------------------------------------
        Shader tessHeightMapShader("Shaders/8.3.gpuheight.vert", "Shaders/8.3.gpuheight.frag", nullptr,            // if wishing to render as is
            "Shaders/8.3.gpuheightT.tesc", "Shaders/8.3.gpuheightT.tese");

        // load and create a texture
        // -------------------------
        unsigned int texture;
        glGenTextures(1, &texture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // load image, create texture and generate mipmaps
        int width, height, nrChannels;
        // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
        unsigned char* data = stbi_load("Textures/FRC.jpg", &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            tessHeightMapShader.setInt("heightMap", 0);
            std::cout << "Loaded heightmap of size " << height << " x " << width << std::endl;
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);

        // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------
        std::vector<float> vertices;

        unsigned rez = 20;
        for (unsigned i = 0; i <= rez - 1; i++)
        {
            for (unsigned j = 0; j <= rez - 1; j++)
            {
                vertices.push_back(-width / 2.0f + width * i / (float)rez); // v.x
                vertices.push_back(0.0f); // v.y
                vertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
                vertices.push_back(i / (float)rez); // u
                vertices.push_back(j / (float)rez); // v

                vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
                vertices.push_back(0.0f); // v.y
                vertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
                vertices.push_back((i + 1) / (float)rez); // u
                vertices.push_back(j / (float)rez); // v

                vertices.push_back(-width / 2.0f + width * i / (float)rez); // v.x
                vertices.push_back(0.0f); // v.y
                vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
                vertices.push_back(i / (float)rez); // u
                vertices.push_back((j + 1) / (float)rez); // v

                vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
                vertices.push_back(0.0f); // v.y
                vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
                vertices.push_back((i + 1) / (float)rez); // u
                vertices.push_back((j + 1) / (float)rez); // v
            }
        }
        std::cout << "Loaded " << rez * rez << " patches of 4 control points each" << std::endl;
        std::cout << "Processing " << rez * rez * 4 << " vertices in vertex shader" << std::endl;

        // first, configure the cube's VAO (and terrainVBO)
        unsigned int terrainVAO, terrainVBO;
        glGenVertexArrays(1, &terrainVAO);
        glBindVertexArray(terrainVAO);

        glGenBuffers(1, &terrainVBO);
        glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texCoord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
        glEnableVertexAttribArray(1);

        glPatchParameteri(GL_PATCH_VERTICES, NUM_PATCH_PTS);

        
        // -----------
        
        SDL_Event event;
        float deltaTime = 0.0f;
        float lastFrame = 0.0f;

                const int FPS = 60;
                const int frameDelay = 1000 / FPS;

        Uint32 frameStart;
        int frameTime;


        while (running) {
           // glViewport(0,0,800,800);
            // per-frame time logic
            // --------------------
            //running= !Input::Instance()->IsXClicked();
            
            //Uint32 lastFrame = 0, currentFrame;
           //float deltaTime;

            // Time calculations

                                            //* (5.0f * (deltaTime * 0.001));
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            float currentFrame = SDL_GetTicks();
            

            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        running = false;
                        break;
                    case SDLK_w:
                        camera.ProcessKeyboard(FORWARD, deltaTime); 
                        break;
                    case SDLK_s:
                        camera.ProcessKeyboard(BACKWARD, deltaTime);
                        break;
                    case SDLK_a:
                        camera.ProcessKeyboard(LEFT, deltaTime);
                        break;
                    case SDLK_d:
                        camera.ProcessKeyboard(RIGHT, deltaTime);
                        break;
                        // Add more key bindings as needed
                    case SDLK_r:
                        //wireframe
                        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                        break;
                    case SDLK_f:
                        //fill frame
                        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 
                        break;

                    case SDL_MOUSEWHEEL: 
                        scroll_callback(&event);
                        break;
                    }

                case SDL_MOUSEMOTION:
                    mouse_callback(&event);
                    break;
                    // Add other event types as needed
                }

            
            }



            deltaTime = (currentFrame - lastFrame) / 1000.0f; // Convert milliseconds to seconds
            lastFrame = currentFrame;

            frameStart = SDL_GetTicks();
            frameTime = SDL_GetTicks() - frameStart;
            if (frameDelay > frameTime) {
                SDL_Delay(frameDelay - frameTime);
            }
            
           

            // input
            // -----
            //processInput(window, running);

            // render
            // ------
          

            // be sure to activate shader when setting uniforms/drawing objects
            tessHeightMapShader.use();

            // view/projection transformations
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
            glm::mat4 view = camera.GetViewMatrix();
            tessHeightMapShader.setMat4("projection", projection);
            tessHeightMapShader.setMat4("view", view);

            // world transformation
            glm::mat4 model = glm::mat4(1.0f);
            tessHeightMapShader.setMat4("model", model);

            // render the terrain
            glBindVertexArray(terrainVAO);
            glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS * rez * rez);

            // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
            // -------------------------------------------------------------------------------
            //SDL_GL_SwapWindow(window);

     
            //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            SDL_GL_SwapWindow(window);
        }
       // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        // optional: de-allocate all resources once they've outlived their purpose:
        // ------------------------------------------------------------------------
        glDeleteVertexArrays(1, &terrainVAO);
        glDeleteBuffers(1, &terrainVBO);

        // glfw: terminate, clearing all previously allocated GLFW resources.
        // ------------------------------------------------------------------
        //instead of glfwTerminate();
        // Quit SDL
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
}


////

   


void checkForWindowResize() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
            int width = event.window.data1;
            int height = event.window.data2;
            glViewport(0, 0, width, height);
        }
    }
}

void handleMouseMovement() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_MOUSEMOTION) {
            float xoffset = event.motion.xrel;
            float yoffset = event.motion.yrel * -1; // reversed since y-coordinates go from bottom to top

            camera.ProcessMouseMovement(xoffset, yoffset);
        }
    }
}

void handleMouseScroll() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_MOUSEWHEEL) {
            camera.ProcessMouseScroll(event.wheel.y);
        }
    }
}


void mouse_callback(SDL_Event* event) {
    if (firstMouse) {
        lastX = event->motion.x;
        lastY = event->motion.y;
        firstMouse = false;
    }

    float xoffset = event->motion.x - lastX;
    float yoffset = lastY - event->motion.y; // reversed since y-coordinates go from bottom to top

    lastX = event->motion.x;
    lastY = event->motion.y;

    camera.ProcessMouseMovement(xoffset, yoffset);
}


void scroll_callback(SDL_Event* event) {
    // SDL mouse wheel events report the amount scrolled in event->wheel.y
    float yoffset = static_cast<float>(event->wheel.y);
    camera.ProcessMouseScroll(yoffset);
}
