#ifndef __WORLDSTATE_H
#define __WORLDSTATE_H
#include "Model.h"


#define NUM_TRACKED_FRAMES 10

class WorldState
{
private:
	float frameTimes[NUM_TRACKED_FRAMES];
	bool running;
    float force;
    

public:
	
	float currentTime;
	
	glm::vec3 cameraPos;
	glm::vec3 cameraLook;
	glm::vec3 cameraUp;
	glm::vec4 lightPos;
	glm::vec3 lightIntensity;
	glm::mat4 lightRotate;
	glm::mat4 lightIncrement;
    glm::mat4 modelRotate;
	glm::mat4 modelIncrementX;
    glm::mat4 modelIncrementY;
	glm::mat4 modelTranslate;
	glm::mat4 cameraMatrix;
	glm::mat4 lightView;
	bool lightRotating;
	bool modelRotatingX;
    bool modelRotatingY;
    bool gravity_on;

	float cursorScrollAmount;
	float center[3];
	float lastClickPos[2];
	float lastFrameDragPos[2];
	float cursorDragAmount[2];
	float cursorAbsolutePos[2];
	int currentRes[2];
	bool mouseButtonDown;
    Model ground;
    vector<Model> models;


public:
	WorldState()
	{ }

	void init()
	{
		for(size_t i=0; i<NUM_TRACKED_FRAMES; i++)
			frameTimes[i] = 0.0f;
        Model m;
        Model n;
        Model ball;
		running = true;
        force=0.0f;
        gravity_on=false;
		m = Model();
		m.init("resources/finalCannon.obj");
		m.setupAttributeBuffers();
        
        
        //        //MODEL 2 LOADING CODE **************************************
        ball = Model();
        ball.init("resources/finalBall.obj");
        ball.setupAttributeBuffers();
        
        
        n = Model();
        n.init("resources/finalGround.obj");
        n.setupAttributeBuffers();
        
		models.push_back(n);
        models.push_back(m);
        models.push_back(ball);
		
		glm::vec3 center = -m.getCentroid();
        glm::vec3 max = m.getMaxBound();
		glm::vec3 min = m.getMinBound();
		glm::vec3 dim = m.getDimension();
		glm::vec3 toMax = max-center;
		printf("model loaded\n");
		printf("min [%.2f %.2f %.2f]\n", min[0], min[1], min[2]);
		printf("max [%.2f %.2f %.2f]\n", max[0], max[1], max[2]);
		printf("cen [%.2f %.2f %.2f]\n", center[0], center[1], center[2]);
		printf("dim [%.2f %.2f %.2f]\n", dim[0], dim[1], dim[2]);
		float camDistance = std::max(dim[0], dim[1]);
		cameraPos = glm::vec3(5,1,1); //base camera pos
        //cameraPos = glm::rotate(cameraPos, 1.5, glm::vec3(1,0,0));
		cameraLook = glm::vec3(0,1,0);
		cameraUp = glm::vec3(0,1,0);
		
		lightPos = glm::vec4((center+(toMax*2.0f)), 1);
		lightPos[1] = (center+(toMax*6.0f))[1];
		lightIntensity = glm::vec3(1,1,1);
		
		lightRotate = glm::mat4(1);
		lightIncrement = glm::rotate(glm::mat4(1), -0.05f, glm::vec3(0,1,0));
		
		modelRotate = glm::mat4(1);
		modelIncrementY = glm::rotate(glm::mat4(1), 0.02f, glm::vec3(0,1,0));
        modelIncrementX = glm::rotate(glm::mat4(1), 0.02f, glm::vec3(1,0,0));
		modelTranslate = glm::translate(glm::mat4(1), -m.getCentroid());
		
		lightRotating = false;
		modelRotatingY = false;
        modelRotatingX=false;
        
//        glm::vec3 center2 = n.getCentroid();
//        glm::vec3 max2 = n.getMaxBound();
//        glm::vec3 min2 = n.getMinBound();
//        glm::vec3 dim2 = n.getDimension();
//        glm::vec3 toMax2 = max2-center2;
//        printf("model loaded\n");
//        printf("min [%.2f %.2f %.2f]\n", min2[0], min2[1], min2[2]);
//        printf("max [%.2f %.2f %.2f]\n", max2[0], max2[1], max2[2]);
//        printf("cen [%.2f %.2f %.2f]\n", center2[0], center2[1], center2[2]);
//        printf("dim [%.2f %.2f %.2f]\n", dim2[0], dim2[1], dim2[2]);
//        float camDistance2 = std::max(dim2[0], dim2[1]);
//        cameraPos = glm::vec3(0,(center2+(toMax2*5.0f))[1],camDistance2);
//        cameraLook = glm::vec3(0,0,0);
//        cameraUp = glm::vec3(0,1,0);
//        
//        lightPos = glm::vec4((center2+(toMax2*2.0f)), 1);
//        lightPos[1] = (center2+(toMax2*6.0f))[1];
//        lightIntensity = glm::vec3(1,1,1);
//        
//        lightRotate = glm::mat4(1);
//        lightIncrement = glm::rotate(glm::mat4(1), -0.05f, glm::vec3(0,1,0));
//        
//        modelRotate = glm::mat4(1);
//        modelIncrement = glm::rotate(glm::mat4(1), 0.02f, glm::vec3(0,1,0));
//        modelTranslate = glm::translate(glm::mat4(1), -n.getCentroid());
        

        
//
//        modelRotate = glm::mat4(1);
//        modelIncrement = glm::rotate(glm::mat4(1), 0.02f, glm::vec3(0,1,0));
//        modelTranslate = glm::translate(glm::mat4(1), -model.getCentroid());
	}
	
	void updateFrameTime(float timeAsSeconds)
	{
		for(size_t i=1; i<NUM_TRACKED_FRAMES; i++)
			frameTimes[i] = frameTimes[i-1];
		frameTimes[0] = timeAsSeconds;
	}
	
	void printFPS() const
	{
		float sum = 0.0f;
		float avg = 0.0f;
		float fps = 0.0f;
		
		for(size_t i=0; i<NUM_TRACKED_FRAMES; i++)
			sum += frameTimes[i];
		
		avg = sum / NUM_TRACKED_FRAMES;
		fps = 1.0f / avg;
		printf("fps %f\n", fps);
	}
	
	Model const & getModel(int i) const
	{ return this->models[i]; }
	
	void setRunning(bool r)
	{ running = r; }

	bool isRunning() const
	{ return running; }

	float getCurrentTime() const
	{ return this->currentTime; }

	void timeStep(float t)
	{
		float elapsed = t - this->currentTime;
		this->updateFrameTime(elapsed);
		
		//spin light
		if(lightRotating)
			lightRotate = lightIncrement * lightRotate;
		
		//spin model
		if(modelRotatingY)
            modelRotate = modelIncrementY * modelRotate; //glm::rotate(glm::mat4(1.0f), ROT_SENSITIVITY/2, glm::vec3(1,0,0));
        
        if(modelRotatingX)
            modelRotate=modelIncrementX*modelRotate;
        
        if(gravity_on)
        {
            this->updateYTranslate();
        }
        
        this->updateZTranslate();
		
		glm::vec3 currentLightPos = glm::vec3(lightRotate*lightPos);
		lightView = glm::lookAt(currentLightPos, cameraLook, cameraUp);
		
		this->currentTime = t;
	}
    
    size_t getNumModels() const
    {
        return this->models.size();
    }
	
	Model & getModel(int i)
	{ return this->models[i]; }
	
	glm::mat4 getModelTranslate() const
	{ return modelTranslate; }
	
	glm::mat4 getModelRotate() const
	{ return modelRotate; }
	
	glm::mat4 getLightRotate() const
	{ return lightRotate; }
	
	glm::vec4 getLightPos() const
	{ return this->lightPos; }
	
	glm::vec3 getLightIntensity() const
	{ return this->lightIntensity; }
	
	glm::mat4 getCameraMatrix() const
	{ return glm::lookAt(cameraPos, cameraLook, cameraUp); }
	
	glm::mat4 getLightViewMatrix() const
	{ return lightView; }
	
	glm::vec4 getCameraPos() const
    { return glm::vec4(this->cameraPos, 1); }
	
	void toggleModelRotateY()
	{ modelRotatingY = !modelRotatingY; }
    
    void toggleModelRotateX()
    { modelRotatingX = !modelRotatingX; }
	
	void toggleLightRotate()
	{ lightRotating = !lightRotating; }
    
    void toggleGravity()
    {gravity_on=!gravity_on;}
	void zoomCamera(int delta)
	{
		float d = pow(0.95, delta);
		printf("%f\n", d);
		cameraPos = cameraPos * d;
	}
    
    void setForce(float force)
    { this->force=force;}
    
    void updateZTranslate()
    {
        #define Z_SENSTIVITY 0.02f
        
        float tforce=this->force;
        
        tforce/=RESOLUTION;
        
        modelTranslate*=glm::translate(glm::mat4(1.0f),glm::vec3(0,0,-glm::clamp(tforce,0.0f,1.0f)));
    }
    
    void updateYTranslate()
    {
        #define Z_SENSTIVITY 0.02f
        
        modelTranslate*=glm::translate(glm::mat4(1.0f),glm::vec3(0,-0.2,0));
    }
};

#endif
