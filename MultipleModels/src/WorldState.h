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
	float gravity;
	glm::vec2 direction;
	glm::mat4 ballTranslate;
	glm::mat4 targetTranslate;
    

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
	glm::mat4 lightTemp;
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
	bool modelRotatingNegY;

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
	void init()
	{
		for(size_t i=0; i<NUM_TRACKED_FRAMES; i++)
			frameTimes[i] = 0.0f;
        Model m;
        Model n;
        Model ball;
		Model t;
		running = true;
        force=0.0f;
		gravity = 0.0f;
		m = Model();
		m.init("resources3/finalCannon.obj");
		m.setupAttributeBuffers();
        
        ball = Model();
        ball.init("resources3/finalBall.obj");
        ball.setupAttributeBuffers();
        
        
        n = Model();
        n.init("resources3/finalGround.obj");
        n.setupAttributeBuffers();

		t = Model();
		t.init("resources3/target.obj");
		t.setupAttributeBuffers();
        
		models.push_back(n);
        models.push_back(m);
        models.push_back(ball);
		models.push_back(t);
		
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
		cameraPos = glm::vec3(2.2,.8,0);
		cameraLook = glm::vec3(-1,.5,0);
		cameraUp = glm::vec3(0,1,0);
		
		lightPos = glm::vec4((center + (toMax*2.0f)), 1);
		lightPos[1] = (center + (toMax*6.0f))[1];
		lightIntensity = glm::vec3(1,1,1);
		
		printf("%f %f %f %f\n", lightPos.x, lightPos.y, lightPos.z, lightPos.w);

		lightRotate = glm::mat4(1);
		lightIncrement = glm::rotate(glm::mat4(1), -0.05f, glm::vec3(0,1,0));
		lightTemp = glm::rotate(glm::mat4(1), -1.0f, glm::vec3(0, 1, 0));

		lightRotate = lightTemp * lightRotate;
		
		modelRotate = glm::mat4(1);
		modelIncrementY = glm::rotate(glm::mat4(1), 0.02f, glm::vec3(0,1,0));
		modelIncrementNegY = glm::rotate(glm::mat4(1), 0.02f, glm::vec3(0, -1, 0));
        modelIncrementX = glm::rotate(glm::mat4(1), 0.02f, glm::vec3(1,0,0));
		modelTranslate = glm::translate(glm::mat4(1), -m.getCentroid());
		ballTranslate = glm::translate(glm::mat4(1), -ball.getCentroid());
		ballTranslate *= glm::translate(glm::mat4(1.0f), glm::vec3(-.1, .1, 0));
		targetTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(-8, 1.25, 0));
		targetTranslate *= glm::rotate(glm::mat4(1), -1.5f, glm::vec3(0, 0, 1));
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
		if (force>0 && insideRange(ballPosDiff.x, 0, 8) && insideRange(ballPosDiff.y, 0, 8) && insideRange(ballPosDiff.z, -3, 3)){
			this->updateBallTranslate();
			this->updateCamera();
			printf("ballPos: %f %f %f\n", ballPos.x, ballPos.y, ballPos.z);
		}

		glm::vec3 currentLightPos = glm::vec3(lightRotate*lightPos);
		lightView = glm::lookAt(currentLightPos, cameraLook, cameraUp);
		
		this->currentTime = t;
	}

	bool insideRange(float val,float low,float high)
	{
		return (val <= high&&val >= low);
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
	void zoomCamera(int delta)
	{
		float d = pow(0.95, delta);
		printf("%f\n", d);
		cameraPos = cameraPos * d;
	}
    
    void setForce(float force)
	{
		this->force = 2*force / RESOLUTION;
		this->gravity = this->force*0.05;
	}

	void setDirection(glm::vec2 dir){
		this->direction.x = dir.x/currentRes[0];
		this->direction.y = dir.y / currentRes[1];
		printf("direction: %f %f\n", direction.x, direction.y);
	}

	glm::mat4 getTargetTranslate() const{
		return targetTranslate;
	}
    
    void updateBallTranslate()
    {
		if (force > 0)
		{
//			ballTranslate *= glm::translate(glm::mat4(1.0f), glm::vec3(-glm::clamp(force, 0.1f, 1.0f), gravity, 0));
//			ballPos += glm::vec3(-glm::clamp(force, 0.0f, 1.0f), gravity, 0);

			ballTranslate *= glm::translate(glm::mat4(1.0f), glm::vec3(-glm::clamp(force, 0.1f, 1.0f), this->direction.y, this->direction.x));
			ballPos += glm::vec3(-glm::clamp(force, 0.0f, 1.0f), 0, 0);
		}
    }
	void updateCamera()
	{	
		this->direction.y -= gravity;
		glm::vec3 tranlateVec = glm::vec3(-glm::clamp(force, 0.1f, 1.0f), this->direction.y, this->direction.x);
		cameraPos += tranlateVec;
		cameraLook += tranlateVec;
	}

	glm::mat4 getBallTranslate() const
	{	return ballTranslate;}

	glm::vec3 getCamPos(){
		return cameraPos;
	}
};

#endif
