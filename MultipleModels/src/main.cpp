//TODO Replace with your usernames!
#define printusers() printf("Program by USER1+USER2\n");

#define _CRT_SECURE_NO_WARNINGS
#define RESOLUTION 512
#define TARGET_FPS 30                // controls spin update rate
#define TIME_BETWEEN_UPDATES 0.015   // seconds between motion updates
#define PRINT_FPS_INTERVAL 10.0f


#ifdef __APPLE__
#include <fcntl.h>
#include <unistd.h>
#endif

#define GL3W
#ifdef GLEW
#define GLEW_STATIC
#include "glew/glew.h"
#endif
#ifdef GL3W
#include "gl3w/gl3w.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <SFML/Window.hpp>
//#define _USE_MATH_DEFINES
//#include <math.h>
#define PI 3.14159f

#include "GLLoader.h"
#include "ShaderManager.h"
#include "GLHelper.h"
#include "RenderEngine.h"
#include "WorldState.h"

#include <SFML/Audio.hpp>


class Program4
{
public:
	Program4()
	{
		getWindowContext();
		
		initLoader();
		float ver = initLoader();
		if( ver < 1.0f ) {
			printf("OpenGL is not supported.\n");
			exit(1);
		}
		printf("OpenGL version %.1f is supported.\n", ver);

		state.init();

		state.currentRes[0] = RESOLUTION;
		state.currentRes[1] = RESOLUTION;
		render.init(state);
		resize(window->getSize().x, window->getSize().y);
		//render.buildRenderBuffers(window->getSize().x, window->getSize().y);
		
		previousPos = glm::vec2(0);
		buttonDown[0]=false;
		buttonDown[1]=false;
		buttonDown[2]=false;
		
		sf::Clock c;
		float lastFrame = c.restart().asSeconds();
		float lastPrint = lastFrame;
		float targetFrameTime = 1.0f/(float)TARGET_FPS;
		
        
//        sf::Music music;
//        
//        if(music.openFromFile("resources/chains.ogg")) {
//            music.play();
//        }
        
        
		while (state.isRunning())
		{			
			window->setActive();
			float currentTime = c.getElapsedTime().asSeconds();
			float sinceLastFrame = currentTime - lastFrame;
			float sleepTime = targetFrameTime - sinceLastFrame;
			if(sleepTime > 0)
				sf::sleep(sf::seconds(sleepTime));
			
			currentTime = c.getElapsedTime().asSeconds();
			lastFrame = currentTime;
			float sinceLastPrint = currentTime - lastPrint;
			
			handleEvents(state, render);
			state.timeStep(currentTime);
			
			if(sinceLastPrint > PRINT_FPS_INTERVAL) {
				lastPrint = currentTime;
				state.printFPS();
			}
			
			render.display(state);
			window->display();
		}
	}
	
private:
	sf::Window * window;
	RenderEngine render;
	WorldState state;
	
	sf::Clock timer;
	float lastUpdate;
	float motionTime;
	glm::ivec2 previousPos;
    glm::vec2 prevPos;
	bool buttonDown[3];
	glm::vec3 camInit = state.getCamPos();

	void handleEvents(WorldState & state, RenderEngine & render)
	{
		sf::Event event;
		
		while (window->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				state.setRunning(false);
			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))
				state.setRunning(false);
			
			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::E)){
				state.toggleModelRotateNegY();
            }

			if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::E)){
				state.toggleModelRotateNegY();
			}

			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Q)){
				state.toggleModelRotateY();
			}

			if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Q)){
				state.toggleModelRotateY();
			}

            if((event.type == sf::Event::TextEntered) && (event.text.unicode == 'x')){
                state.toggleModelRotateX();
            }
			if((event.type == sf::Event::TextEntered) && (event.text.unicode == 't'))
				state.toggleLightRotate();
			
			if(event.type == sf::Event::Resized) {
				resize(event.size.width, event.size.height);
			}

            if((event.type == sf::Event::TextEntered) && (event.text.unicode == 'z')){
                state.cameraPos.y+=0.1;
                state.cameraLook.y+=0.1;
            }
            if((event.type == sf::Event::TextEntered) && (event.text.unicode == 's')){
                state.cameraPos.x-=0.1;
                state.cameraLook.x-=0.1;
            }
           if((event.type == sf::Event::TextEntered) && (event.text.unicode == 'c')){
                state.cameraPos.y-=0.1;
                state.cameraLook.y-=0.1;
            }
            if((event.type == sf::Event::TextEntered) && (event.text.unicode == 'w')){
                state.cameraPos.x+=0.1;
                state.cameraLook.x+=0.1;
            }
            if((event.type == sf::Event::TextEntered) && (event.text.unicode == 'a')){
                state.cameraPos.z-=0.1;
                state.cameraLook.z-=0.1;
            }
            if((event.type == sf::Event::TextEntered) && (event.text.unicode == 'd')){
                state.cameraPos.z+=0.1;
                state.cameraLook.z+=0.1;
            }

            if((event.type == sf::Event::TextEntered) && (event.text.unicode == 'g')){
                state.toggleGravity();
            }

			if ((event.type == sf::Event::TextEntered) && (event.text.unicode == 'r')){
				if (state.getGravity() == true){
					state.toggleGravity();
				}

				state.setForce(0);
				state.cameraPos.x = camInit.x;
				state.cameraPos.y = camInit.y;
				state.cameraPos.z = camInit.z;
				prevPos = glm::vec2(0);
			}

			if(event.type == sf::Event::MouseButtonPressed){
                prevPos=glm::vec2(event.mouseButton.x,event.mouseButton.y);
			}

			if(event.type == sf::Event::MouseButtonReleased)
            {
                state.setForce(glm::distance(prevPos,glm::vec2(event.mouseButton.x,event.mouseButton.y)));
				state.setDirection(prevPos);
				state.mouseButtonDown = false;
            }

			if(event.type == sf::Event::MouseMoved && state.mouseButtonDown){
				state.cursorDragAmount[0] += state.lastFrameDragPos[0] - event.mouseMove.x;
				state.cursorDragAmount[1] += state.lastFrameDragPos[1] - (state.currentRes[1]-event.mouseMove.y);
				state.lastFrameDragPos[0] = event.mouseMove.x;
				state.lastFrameDragPos[1] = (state.currentRes[1]-event.mouseMove.y);          
			}

			if(event.type == sf::Event::MouseWheelMoved)
			{
				state.zoomCamera(event.mouseWheel.delta);
				state.cursorScrollAmount += event.mouseWheel.delta;
			}

			if(event.type == sf::Event::MouseMoved)
			{
				state.cursorAbsolutePos[0] = event.mouseMove.x;
				state.cursorAbsolutePos[1] = (state.currentRes[1]-event.mouseMove.y);

			}

		}
	}

	void resize(size_t x, size_t y)
	{
		//render.buildRenderBuffers(x, y);
		state.currentRes[0] = x;
		state.currentRes[1] = y;
        
	}
	
	void getWindowContext()
	{
		sf::err().rdbuf(NULL); //hide errors
		
#ifdef __APPLE__
		int nullFD = open("/dev/null", O_WRONLY);
		int oldFD = dup(2); // Duplicate the old file descriptor, so it can be restored
		dup2(nullFD, 2); // Redirect
#endif
		
		sf::VideoMode mode(RESOLUTION, RESOLUTION, 32);
#ifdef __linux__
		sf::ContextSettings settings(32, 0, 0, 3, 3);
#else
		sf::ContextSettings settings(32, 0, 0, 3, 3, sf::ContextSettings::Core);
#endif
		window = new sf::Window(mode, "SFML application", sf::Style::Default, settings);
		
#ifdef __APPLE__
		dup2(oldFD, 2); // Redirect back
		close(oldFD); // Not needed anymore
#endif
	}
};

int main()
{
	printusers();
	Program4 prog;
	
	return EXIT_SUCCESS;
}
