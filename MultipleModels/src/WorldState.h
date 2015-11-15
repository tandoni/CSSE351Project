#ifndef __WORLDSTATE_H
#define __WORLDSTATE_H
#include "Model.h"


#define NUM_TRACKED_FRAMES 10

class WorldState
{
private:
	const float gravity = -0.005;
	float frameTimes[NUM_TRACKED_FRAMES];
	bool running;
    float force;
	glm::mat4 ballTranslate;
    

public:
	
	float currentTime;
	
	glm::vec3 ballPos;
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
	glm::mat4 modelIncrementNegY;
	glm::mat4 modelTranslate;
	glm::mat4 cameraMatrix;
	glm::mat4 lightView;
	bool lightRotating;
	bool modelRotatingX;
    bool modelRotatingY;
    bool gravity_on;
	bool modelRotatingNegY;
	float distance = 12;
	float traveled = 0;

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
//		printf("model loaded\n");
//		printf("min [%.2f %.2f %.2f]\n", min[0], min[1], min[2]);
//		printf("max [%.2f %.2f %.2f]\n", max[0], max[1], max[2]);
//		printf("cen [%.2f %.2f %.2f]\n", center[0], center[1], center[2]);
//		printf("dim [%.2f %.2f %.2f]\n", dim[0], dim[1], dim[2]);
//		float camDistance = std::max(dim[0], dim[1]);
		cameraPos = glm::vec3(5,1,1);
		cameraLook = glm::vec3(0,1,0);
		cameraUp = glm::vec3(0,1,0);
		
		lightPos = glm::vec4((center+(toMax*2.0f)), 1);
		lightPos[1] = (center+(toMax*6.0f))[1];
		lightIntensity = glm::vec3(1,1,1);
		
		lightRotate = glm::mat4(1);
		lightIncrement = glm::rotate(glm::mat4(1), -0.05f, glm::vec3(0,1,0));
		
		modelRotate = glm::mat4(1);
		modelIncrementY = glm::rotate(glm::mat4(1), 0.02f, glm::vec3(0,1,0));
		modelIncrementNegY = glm::rotate(glm::mat4(1), 0.02f, glm::vec3(0, -1, 0));
        modelIncrementX = glm::rotate(glm::mat4(1), 0.02f, glm::vec3(1,0,0));
		modelTranslate = glm::translate(glm::mat4(1), -m.getCentroid());
		ballTranslate = glm::translate(glm::mat4(1), -ball.getCentroid());
		ballPos = -ball.getCentroid();
		
		lightRotating = false;
		modelRotatingY = false;
        modelRotatingX = false;
		modelRotatingNegY = false;
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
            modelRotate = modelIncrementY * modelRotate; 

		if (modelRotatingNegY)
			modelRotate = modelIncrementNegY * modelRotate; 
        
        if(modelRotatingX)
            modelRotate=modelIncrementX*modelRotate;
        
		//printf("%f %f %f\n", -models[1].getCentroid().x, -models[1].getCentroid().y, -models[1].getCentroid().z);
		glm::vec3 ballPosDiff = glm::abs(ballPos + models[1].getCentroid());
		if (ballPosDiff.x < distance && ballPosDiff.y>-0.3){
			traveled += force;
			this->updateBallTranslate();
			if (force>0)
				this->updateCamera();
			printf("Travled: %f\n", this->traveled);
			printf("ballPos: %f %f %f\n", ballPos.x, ballPos.y, ballPos.z);
		}

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
	
	void toggleModelRotateY(){ 
		modelRotatingY = !modelRotatingY; 
	}

	void toggleModelRotateNegY(){
		modelRotatingNegY = !modelRotatingNegY;
	}

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
	{
		this->force = 2*force / RESOLUTION;
	}
    
    void updateBallTranslate()
    {
		if (force > 0)
		{
			ballTranslate *= glm::translate(glm::mat4(1.0f), glm::vec3(-glm::clamp(force, 0.0f, 1.0f), gravity, 0));
			ballPos += glm::vec3(-glm::clamp(force, 0.0f, 1.0f), gravity, 0);
		}
    }
	void updateCamera()
	{	
		glm::vec3 tranlateVec = glm::vec3(-glm::clamp(force, 0.0f, 1.0f), gravity, 0);
		cameraPos += tranlateVec;
		cameraLook += tranlateVec;
	}

	glm::mat4 getBallTranslate() const
	{	return ballTranslate;}
    
    void updateYTranslate()
    {
        #define Z_SENSTIVITY 0.02f
        
		ballTranslate *= glm::translate(glm::mat4(1.0f), glm::vec3(0, gravity, 0));
    }
};

#endif
