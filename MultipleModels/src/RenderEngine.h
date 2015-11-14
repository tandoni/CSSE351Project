#ifndef __RENDERENGINE
#define __RENDERENGINE

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "GLHelper.h"
#include "WorldState.h"

#define TEX_TYPE GL_TEXTURE_2D
//#define TEX_TYPE GL_TEXTURE_RECTANGLE

class RenderEngine
{
public:
    RenderEngine()
    {
        initialized = false;
    }
    
    ~RenderEngine()
    {
        if(initialized)
        {
            // Clean up the buffers
            //glDeleteBuffers(1, &positionBuffer);
            //glDeleteBuffers(1, &colorBuffer);
        }
    }
    
    void init(WorldState & state)
    {
        initialized = true;
        
        glEnable(GL_DEPTH_TEST);
        
        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);
        
//        setupTextures();
        
        setupShader();
    }
    
    void display(WorldState & state)
    {
//        glViewport(0, 0, mapSizeX, mapSizeY);
//        size_t shaderId;
//        
//        shaderId = 0;
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //draw
        for(int i=0;i<state.getNumModels();i++)
        {
            glUseProgram(shaderProg[i]);
            uploadUniforms(shaderProg[i], state);
            state.getModel(i).draw(shaderProg[i]);
        }
        
        glUseProgram(0);
        checkGLError("model");
    }
    
private:
    bool initialized;
    GLuint shaderProg[3];
    GLuint vertexArray;
    GLuint quadVertexArray;
    GLuint lightArray;
    GLuint frameBuffer;
    GLuint renderTexture;
    GLuint renderBuffer;
    size_t mapSizeX;
    size_t mapSizeY;
    GLuint textures[4];
    
    glm::vec2 size;
    
    float _near;
    float _far;
    float fov;
    
    void setupShader()
    {
        char const * cannonBallVertPath = "resources/cannonball.vert";
        char const * cannonBallFragPath = "resources/cannonball.frag";
        shaderProg[0] = ShaderManager::shaderFromFile(&cannonBallVertPath, &cannonBallFragPath, 1, 1);
        
        char const * cannonVertPath = "resources/cannon.vert";
        char const * cannonFragPath = "resources/cannon.frag";
        shaderProg[1] = ShaderManager::shaderFromFile(&cannonVertPath, &cannonFragPath, 1, 1);
        
        checkGLError("shader");
    }
    
//    void setupTextures()
//    {
//        //generate texture names
//        glGenTextures(4, textures);
////        sf::Image image;
//        
//        char const * imagePaths[4] = {"resources/rhit1.png", "resources/rhit2.png", "resources/rhit3.png", "resources/rhit4.png"};
//        
//        for(int i=0; i<4; i++)
//        {
//            if (!image.loadFromFile(imagePaths[i])) {
//                fprintf(stderr, "Could not load: %s\n", imagePaths[i]);
//                exit(2);
//            }
//            int texSizeX = image.getSize().x;
//            int texSizeY = image.getSize().y;
//            unsigned char * texData = (unsigned char*) image.getPixelsPtr();
//            
//            //TODO bind a texture object
//            
//            glBindTexture(GL_TEXTURE_2D, textures[i]);
//            
//            //TODO set min/mag sampling parameters
//            
//            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//            
//            //TODO set edge wrap parameters
//            
//            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//            
//            //TODO upload texture and generate mipmap
//            bool  mipmapEnabled = true;
//            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,texSizeX, texSizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
//            if(mipmapEnabled)
//            {
//                //mip mapping, upload 0 level, then build maps
//                glGenerateMipmap(GL_TEXTURE_2D);
//            }
//            else
//            {
//                //no mip mapping, upload 0 level only
//            }
//            
//            //TODO unbind the texture
//            glBindTexture(GL_TEXTURE_2D, 0);
//        }
//        
//        checkGLError("texture");
//    }

    
    void uploadUniforms(GLuint shaderId, WorldState const & state)
    {
        for(int i=0;i<state.getNumModels();i++)
        {
            glm::vec3 dim = state.getModel(i).getDimension();
            float maxDim = std::max(dim[0], std::max(dim[1], dim[2]));
            _near = maxDim*0.1f;
            _far  = maxDim*3.0f;
            fov = 0.9f;
            
            glm::mat4 P = glm::perspective(1.0f, fov, _near, _far);
            glm::mat4 mT = state.getModelTranslate();
            glm::mat4 mR = state.getModelRotate();
            glm::mat4 C = state.getCameraMatrix();
            glm::mat4 M = C*mR*mT;
            glm::mat3 N = glm::inverseTranspose(glm::mat3(M));
            glm::vec4 lightPos = state.getLightPos();
            glm::vec4 camPos = state.getCameraPos();
            glm::mat4 Lr = state.getLightRotate();
            glm::mat4 Lv = state.getLightViewMatrix();
            glm::vec3 velocity = glm::vec3(0,0,-0.5);
            glm::mat4 Lp;
            Lp = glm::perspective(1.0f, fov, _near, _far);
            Lp = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, 0.0f, 50.0f);
            
            //hacky light source size change
            GLfloat maxDis = state.getModel(i).getDimension()[2] * 3;
            GLfloat distScale = 1.0f / (glm::length(Lr*lightPos - camPos) / maxDis);
            glPointSize(glm::mix(1.0f, 10.0f, distScale));
            
            //printf("cam %f %f %f\n", camPos[0], camPos[1], camPos[2]);
            //printf("light %f %f %f\n", lightPos[0], lightPos[1], lightPos[2]);
            
            glUniformMatrix4fv(glGetUniformLocation(shaderId, "P"), 1, GL_FALSE, &P[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(shaderId, "Lp"), 1, GL_FALSE, &Lp[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(shaderId, "C"), 1, GL_FALSE, &C[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(shaderId, "mR"), 1, GL_FALSE, &mR[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(shaderId, "mT"), 1, GL_FALSE, &mT[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(shaderId, "M"), 1, GL_FALSE, &M[0][0]);
            glUniformMatrix3fv(glGetUniformLocation(shaderId, "N"), 1, GL_FALSE, &N[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(shaderId, "Lr"), 1, GL_FALSE, &Lr[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(shaderId, "Lv"), 1, GL_FALSE, &Lv[0][0]);
            glUniform4fv(glGetUniformLocation(shaderId, "lightPos"), 1, &lightPos[0]);
            glUniform4fv(glGetUniformLocation(shaderId, "camPos"), 1, &camPos[0]);
            if (shaderId == 0)
                glUniform3fv(glGetUniformLocation(0, "velocity"), 1, &velocity[0]);
            
            //		glUniform1f(glGetUniformLocation(shaderId, "elapsedTime"), state.currentTime);
            //		glUniform1f(glGetUniformLocation(shaderId, "near"), _near);
            //		glUniform1f(glGetUniformLocation(shaderId, "far"), _far);
            //		glUniform1f(glGetUniformLocation(shaderId, "fov"), fov);
            //		glUniform1f(glGetUniformLocation(shaderId, "cursorScrollAmount"), state.cursorScrollAmount);
            glUniform2f(glGetUniformLocation(shaderId, "resolution"), state.currentRes[0], state.currentRes[1]);
            //		glUniform3f(glGetUniformLocation(shaderId, "modelCenter"),  state.center[0], state.center[1], state.center[2]);
            //		glUniform3f(glGetUniformLocation(shaderId, "lookAtPos"),  state.cameraLook[0], state.cameraLook[1], state.cameraLook[2]);
            //		glUniform3f(glGetUniformLocation(shaderId, "cameraUp"),  state.cameraUp[0], state.cameraUp[1], state.cameraUp[2]);
            //		glUniform2f(glGetUniformLocation(shaderId, "cursorAbsolutePos"), state.cursorAbsolutePos[0], state.cursorAbsolutePos[1]);
            //		glUniform2f(glGetUniformLocation(shaderId, "cursorDragAmount"), state.cursorDragAmount[0], state.cursorDragAmount[1]);
            //		glUniform2f(glGetUniformLocation(shaderId, "lastClickPos"), state.lastClickPos[0], state.lastClickPos[1]);
            //		glUniform2f(glGetUniformLocation(shaderId, "lastFrameDragPos"), state.lastFrameDragPos[0], state.lastFrameDragPos[1]);
            //		glUniform1i(glGetUniformLocation(shaderId, "mouseButtonDown"), state.mouseButtonDown);
        }
    }
    
};

#endif