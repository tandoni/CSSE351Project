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
	Model terrain;
	vector<Model> models;

public:
	
	float currentTime;
	
	glm::vec3 cameraPos;
	glm::vec3 cameraLook;
	glm::vec3 cameraUp;
	
	glm::vec4 lightPos;
	glm::vec3 lightIntensity;
	
	glm::mat4 lightRotate;
	glm::mat4 lightIncrement;
	glm::mat4 modelTranslate;
	glm::mat4 cameraMatrix;
	glm::mat4 lightView;
	
	bool lightRotating;
	bool modelRotating;

	float cursorScrollAmount;
	float center[3];
	float lastClickPos[2];
	float lastFrameDragPos[2];
	float cursorDragAmount[2];
	float cursorAbsolutePos[2];
	int currentRes[2];
	bool mouseButtonDown;

public:
	WorldState()
	{ }

	void init(const char *filename)
	{
		for(size_t i=0; i<NUM_TRACKED_FRAMES; i++)
			frameTimes[i] = 0.0f;
		
		running = true;
		terrain = Model();
		terrain.init(filename);
		terrain.setupAttributeBuffers();
		
		glm::vec3 center = terrain.getCentroid();
		glm::vec3 max = terrain.getMaxBound();
		glm::vec3 min = terrain.getMinBound();
		glm::vec3 dim = terrain.getDimension();
		glm::vec3 toMax = max-center;
		printf("model loaded\n");
		printf("min [%.2f %.2f %.2f]\n", min[0], min[1], min[2]);
		printf("max [%.2f %.2f %.2f]\n", max[0], max[1], max[2]);
		printf("cen [%.2f %.2f %.2f]\n", center[0], center[1], center[2]);
		printf("dim [%.2f %.2f %.2f]\n", dim[0], dim[1], dim[2]);
		float camDistance = std::max(dim[0], dim[1]);
		cameraPos = glm::vec3(0,(center+(toMax*5.0f))[1],camDistance);
		cameraLook = glm::vec3(0,0,0);
		cameraUp = glm::vec3(0,1,0);
		
		lightPos = glm::vec4((center+(toMax*2.0f)), 1);
		lightPos[1] = (center+(toMax*6.0f))[1];
		lightIntensity = glm::vec3(1,1,1);
		
		lightRotate = glm::mat4(1);
		lightIncrement = glm::rotate(glm::mat4(1), -0.05f, glm::vec3(0,1,0));

		//modelIncrement = glm::rotate(glm::mat4(1), 0.02f, glm::vec3(0,1,0));
		
		lightRotating = false;
		modelRotating = false;
	}
	
	void init(const char *filename, const char *vertPath, const char *fragPath)
	{
		init(filename);
		setTerrainShader(vertPath, fragPath);
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
	{ return models[i]; }
	
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
		//if(modelRotating)
			//modelRotate = modelIncrement * modelRotate;
		
		glm::vec3 currentLightPos = glm::vec3(lightRotate*lightPos);
		lightView = glm::lookAt(currentLightPos, cameraLook, cameraUp);
		
		this->currentTime = t;
	}

	size_t getNumModels() const
	{
		return this->models.size();
	}
	
	Model & getModel(int i)
	{ return models[i]; }
	
	glm::mat4 getModelTranslate() const
	{ return modelTranslate; }
	
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
	
	void toggleModelRotate()
	{ modelRotating = !modelRotating; }
	
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
		this->force = force;
	}

	void addModel(Model m)
	{
		this->models.push_back(m);
	}

	Model getTerrain()
	{
		return this->terrain;
	}

	void setTerrainShader(const char *vertPath, const char *fragPath)
	{
		this->terrain.setShader(vertPath,fragPath);
	}

	void updateZTranslate()
	{
		#define Z_SENSTIVITY 0.02f

		float tforce = this->force;

		tforce /= RESOLUTION;

		modelTranslate *= glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -glm::clamp(tforce, 0.0f, 1.0f)));
	}
};

#endif
