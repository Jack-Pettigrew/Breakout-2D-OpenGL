#include <iostream>
#include <random>
#include "Background.h"
#include "GameOver.h"
#include "Ball.h"
#include "Paddle.h"
#include "ShaderClass.h"
#include "TextureClass.h"
#include "Brick.h"
#include "Heart.h"
#include "Score.h"
#include "Includes.h"

// Method Declarations
bool CollisionPaddle(Paddle paddle, Ball ball);
int CollisionBrick(Brick brick, Ball ball);
void Cleanup(SDL_GLContext glcontext);

int main(int argc, char *argv[]) {

	// Init SDL
#pragma region Init SDL & GL

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {

		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init Error: %s\n", SDL_GetError());

		return 1;
	}
	SDL_Log("SDL initialised OK!\n");

	// Window Creation - modified for OpenGL
	SDL_Window *window = nullptr;
	// Acquires the resolution of the Display
	SDL_DisplayMode dm, aspectRatio;
	SDL_GetCurrentDisplayMode(0, &dm);
	int width = dm.w;
	int height = dm.h;
	// Makes window half the width and height of the primary display
	window = SDL_CreateWindow("Jack Pettigrew - 15593452 - 2D Breakout", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width/2, height/2, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	// ...check if window = NULL
	if (window == nullptr) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindow init error: %s\n", SDL_GetError());
		return 1;
	}

	// Create and Assign OpenGl Context to Window
	SDL_GLContext glcontext = SDL_GL_CreateContext(window);

	// Get OpenGL version
	std::cout << "OpenGL version is" << glGetString(GL_VERSION) << std::endl;
	
	// OpenGL specific values
	GLenum err = glewInit(); // GL error controller
#pragma endregion

	// Game Objects
	Paddle paddle;				// Paddle
	Ball ball(0.02f);			// Ball - raidus of 0.06f
	Background background1;		// Background
	Background background2;		// Background 2
	Brick boarder;				// Boarders
	Brick boarder2;				// ^
	Brick boarder3;				// ^
	Brick boarder4;				// ^
	Brick brick[5][7];			// 5x5 Brick Array
	GameOver GameOver;			// Game Over Object
	Heart heart[3];				// Hearts Array
	Score score[2];				// Score Array

	// Textures - hold, load, set buffers
#pragma region Texture

	Texture paddleTex;										
	Texture ballTex;
	Texture bgTex;
	Texture bgTex2;
	Texture brickTex;
	Texture goTex;
	Texture heartTex;
	Texture emptyTex;
	Texture scoreTex[10];
	Texture boarderTex;
	paddleTex.load("..//content//thePaddle.png");				// Loads in textures for object
	paddleTex.setBuffers();										// Set buffers for textures
	ballTex.load("..//content//bricks.png");
	ballTex.setBuffers();
	bgTex.load("..//content//space.png");
	bgTex.setBuffers();
	bgTex2.load("..//content//earth.png");
	bgTex2.setBuffers();
	brickTex.load("..//content//theBricks.png");
	brickTex.setBuffers();
	goTex.load("..//content//gameover.png");
	goTex.setBuffers();
	heartTex.load("..//content//heart.png");
	heartTex.setBuffers();
	emptyTex.load("..//content//emptyHeart.png");
	emptyTex.setBuffers();
	scoreTex[0].load("..//content//0.png");
	scoreTex[1].load("..//content//1.png");
	scoreTex[2].load("..//content//2.png");
	scoreTex[3].load("..//content//3.png");
	scoreTex[4].load("..//content//4.png");
	scoreTex[5].load("..//content//5.png");
	scoreTex[6].load("..//content//6.png");
	scoreTex[7].load("..//content//7.png");
	scoreTex[8].load("..//content//8.png");
	scoreTex[9].load("..//content//9.png");
	for (int i = 0; i < 10; i++)
	{
		scoreTex[i].setBuffers();
	}
	boarderTex.load("..//content//metal.png");
	boarderTex.setBuffers();

#pragma endregion

	// Assigning Shaders
	Shader vSh("..//src//shader_projection.vert");		// Shader to control objects transforms (Transform)
	Shader fSh("..//src//shader_projection.frag");		// Shader to control texture on object  (Apperance)

	// Compile the shader code
	vSh.getShader(1);									// 1 is for vertex
	fSh.getShader(2);									// 2 is for fragment
							
	// Create Shader Program Variable
	GLuint shaderProgram;
	shaderProgram = glCreateProgram();					// Creates foundation of shaderProgram
	glAttachShader(shaderProgram, vSh.shaderID);		// Attaches .vert and .frag shaders to shaderProgram
	glAttachShader(shaderProgram, fSh.shaderID);		// ^
	glLinkProgram(shaderProgram);						// Link shaders to shaderProgram

	// Delete uneeded Shaders
	glDeleteShader(vSh.shaderID);						
	glDeleteShader(fSh.shaderID);

	// Uniform variables - Sets the shader Uniforms
	int modelLocation;
	int viewLocation;
	int projectionLocation;
	
	// Use THIS Shader
	glUseProgram(shaderProgram);						

	// Set Buffers for each Object
	paddle.setBuffers();
	ball.setBuffers();
	background1.setBuffers();
	background2.setBuffers();
	GameOver.setBuffers();
	score[0].setBuffers();
	score[1].setBuffers();
	boarder.setBuffers();
	boarder2.setBuffers();
	boarder3.setBuffers();
	boarder4.setBuffers();

	// Brick Buffers
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			brick[i][j].setBuffers();
		}
	}

	// Heart Buffers
	for (int i = 0; i < 3; i++)
		heart[i].setBuffers();

	// Transform Matrices
	glm::mat4 modelMatrix;
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	glm::mat4 mScale;
	glm::mat4 backgroundScale;
	glm::mat4 bg2Scale;
	glm::vec3 scaleFactor;
	glm::vec3 b_scaleFactor;
	glm::vec3 b2_scaleFactor;
	glm::mat4 paddleTranslate;				// Paddle's Placement
	glm::mat4 brickTranslate;				// Brick's Translate
	glm::mat4 ballStart;					// Ball's Start
	glm::mat4 backgroundTranslate;			// Background Placement
	glm::mat4 bg2Translate;					// Background Placement
	glm::mat4 ballTranslate;				// Ball's Translate
	glm::mat4 gameoverTranslate;			// GameOver's Translate
	glm::mat4 heartTranslate;				// Heart Spacing Translation
	glm::mat4 heartMove;					// Heart Inital Position
	glm::mat4 scoreTranslate1;				// Score 1 Translate
	glm::mat4 scoreTranslate2;				// Score 2 Translate
	glm::mat4 scoreScale;					// Score Scale
	glm::mat4 gameoverScale;				// Game Over Scale
	glm::mat4 boarderTranslate;				// Boarder Translate
	glm::mat4 boarderTranslate2;			// Boarder Translate
	glm::mat4 boarderTranslate3;			// Boarder Translate
	glm::mat4 boarderTranslate4;			// Boarder Translate
	glm::mat4 boarderScale;					// Boarder Scale
	glm::mat4 boarderScale2;				// Boarder Scale

	// Scales Setting
	scaleFactor = { 0.5f, 1.5f, 1.5f };
	mScale = glm::scale(mScale, glm::vec3(scaleFactor));

	// Background Scale Setting
	b_scaleFactor = { 1.0f, 1.0f, 1.0f };
	b2_scaleFactor = { 0.5f, 1.5f, 1.0f };
	backgroundScale = glm::scale(backgroundScale, glm::vec3(b_scaleFactor));
	bg2Scale = glm::scale(bg2Scale, glm::vec3(b2_scaleFactor));
	backgroundTranslate = glm::translate(backgroundTranslate, glm::vec3(0.0f, 0.0f, 0.0f));

	//	Set view matrix to identity
	viewMatrix = glm::mat4(1.0f);

	//	Orthographic projection (0 - 1 coordinate positions)
	projectionMatrix = glm::ortho(0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 100.0f);

	// Transform Vertices Setting
	glm::vec3 paddleMove{ 0.04f, 0.0f, 0.0f };				// Paddle vec3
	glm::vec3 ballMove{ 0.005f, -0.01f, 0.0f };				// Ball vec3
	glm::vec3 brickMove{ 0.03f, 0.0f, 0.0f };				// Brick vec3
	glm::vec3 parallax{ 0.01f, 0.0f ,0.0f };				// Background 1 Parallax vec3
	glm::vec3 parallax2{ 0.005f, 0.0f, 0.0f };				// Background 2 Parallax vec3
	glm::vec3 follow{ ballMove.x, 0.0f, 0.0f };				// Follow vec3
	glm::vec3 heartParallax{ 0.002f, 0.0f, 0.0f };			// Heart Parallax vec3
	glm::vec3 scoreParallax{ 0.002f, 0.0f, 0.0f };			// Score Parallax vec3
	
	// Starting Translations
	paddleTranslate = glm::translate(paddleTranslate, glm::vec3(0.45f, 0.9f, 0.0f));			// Paddle start position
	brickTranslate = glm::translate(brickTranslate, glm::vec3(0.13f, 0.0f, 0.0f));				// Brick Translate
	ballTranslate = glm::translate(ballTranslate, glm::vec3(0.5f, 0.85f, 0.0f));				// Ball start position
	ballStart = ballTranslate;																	// Ball Restart position
	backgroundTranslate = glm::translate(backgroundTranslate, glm::vec3(0.0f, 0.0f, 0.0f));		// Background 1 start position
	bg2Translate = glm::translate(bg2Translate, glm::vec3(0.1f, 0.4f, 0.0f));					// Background 2 start position
	heartMove = glm::translate(heartMove, glm::vec3(0.1f, 0.9f, 0.0f));							// Hearts Start Position
	scoreTranslate1 = glm::translate(scoreTranslate1, glm::vec3(0.55f, 0.1f, 0.0f));			// Score 1 Translate
	scoreTranslate2 = glm::translate(scoreTranslate2, glm::vec3(0.77f, 0.1f, 0.0f));			// Score 2 Translate
	scoreScale = glm::scale(scoreScale, glm::vec3(1.2f, 3.0f, 1.0f));							// Scale Scale
	gameoverTranslate = glm::translate(gameoverTranslate, glm::vec3(0.5f, 0.5f, 0.0f));			// GameOver Translate
	gameoverScale = glm::scale(gameoverScale, glm::vec3(0.5f, 0.5f, 1.0f));						// GameOver Scale
	boarderTranslate = glm::translate(boarderTranslate, glm::vec3(-0.49f, 0.0f, 0.0f));			// Boarder Translate
	boarderTranslate2 = glm::translate(boarderTranslate2, glm::vec3(0.99f, 0.0f, 0.0f));		// Boarder Translate
	boarderTranslate3 = glm::translate(boarderTranslate3, glm::vec3(0.0f, -0.99f, 0.0f));		// Boarder Translate
	boarderTranslate4 = glm::translate(boarderTranslate4, glm::vec3(0.0f, 0.99f, 0.0f));		// Boarder Translate
	boarderScale = glm::scale(boarderScale, glm::vec3(5.0f, 10.0f, 1.0f));						// Boarder Scale
	boarderScale2 = glm::scale(boarderScale, glm::vec3(2.0f, 1.0f, 1.0f));

	// Game Loop - Setup
	SDL_Event event;
	bool windowOpen = true;						// Game loop bool
	bool ballReady = true;						// Ball follow 
	bool gameOver = false;						// Game Over loop
	int lives = 3;								// Player's Lives
	int scoreCount = 0;

	// *Game Loop*
	while (windowOpen)
	{
		// Setting Background Colour
		glClearColor(1.0f, 1.0f, 1.0f, 0.0f);		// background colour
		glClear(GL_COLOR_BUFFER_BIT);				// should be GL_COLOR_BUFFER_BIT

		// Select Shader Program
		glUseProgram(shaderProgram);

		// Transform Uniforms
		modelLocation = glGetUniformLocation(shaderProgram, "uModel");
		viewLocation = glGetUniformLocation(shaderProgram, "uView");
		projectionLocation = glGetUniformLocation(shaderProgram, "uProjection");

		// Game Over Loop
		while (gameOver)
		{
			if (SDL_PollEvent(&event))
			{
				if (event.type == SDL_QUIT)
				{
					windowOpen = false;
					gameOver = false;
				}
			}
		}

		// Background 1 Transform
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(backgroundTranslate * backgroundScale));
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		glBindTexture(GL_TEXTURE_2D, bgTex.texture);
		background1.render();

		// Score Transform
#pragma region Score Transform

		if (scoreCount < 10)
		{
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(scoreTranslate1 * scoreScale));
			glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
			glBindTexture(GL_TEXTURE_2D, scoreTex[scoreCount].texture);
			score[0].render();
		}
		else if (scoreCount < 20)
		{
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(scoreTranslate1 * scoreScale));
			glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
			glBindTexture(GL_TEXTURE_2D, scoreTex[1].texture);
			score[0].render();

			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(scoreTranslate2 * scoreScale));
			glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
			glBindTexture(GL_TEXTURE_2D, scoreTex[scoreCount - 10].texture);
			score[1].render();
		}
		else if (scoreCount < 30)
		{
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(scoreTranslate1 * scoreScale));
			glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
			glBindTexture(GL_TEXTURE_2D, scoreTex[2].texture);
			score[0].render();

			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(scoreTranslate2 * scoreScale));
			glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
			glBindTexture(GL_TEXTURE_2D, scoreTex[scoreCount - 20].texture);
			score[1].render();
		}
		else if (scoreCount < 40)
		{
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(scoreTranslate1 * scoreScale));
			glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
			glBindTexture(GL_TEXTURE_2D, scoreTex[3].texture);
			score[0].render();

			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(scoreTranslate2 * scoreScale));
			glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
			glBindTexture(GL_TEXTURE_2D, scoreTex[scoreCount - 30].texture);
			score[1].render();
		}
		else if (scoreCount < 50)
		{
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(scoreTranslate1 * scoreScale));
			glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
			glBindTexture(GL_TEXTURE_2D, scoreTex[4].texture);
			score[0].render();

			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(scoreTranslate2 * scoreScale));
			glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
			glBindTexture(GL_TEXTURE_2D, scoreTex[scoreCount - 40].texture);
			score[1].render();
		}
		else if (scoreCount < 60)
		{
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(scoreTranslate1 * scoreScale));
			glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
			glBindTexture(GL_TEXTURE_2D, scoreTex[5].texture);
			score[0].render();

			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(scoreTranslate2 * scoreScale));
			glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
			glBindTexture(GL_TEXTURE_2D, scoreTex[scoreCount - 50].texture);
			score[1].render();
		}
		else if (scoreCount < 70)
		{
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(scoreTranslate1 * scoreScale));
			glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
			glBindTexture(GL_TEXTURE_2D, scoreTex[6].texture);
			score[0].render();

			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(scoreTranslate2 * scoreScale));
			glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
			glBindTexture(GL_TEXTURE_2D, scoreTex[scoreCount - 60].texture);
			score[1].render();
		}

#pragma endregion

		// Background 2 Transform
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(bg2Translate * bg2Scale));
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		glBindTexture(GL_TEXTURE_2D, bgTex2.texture);
		background2.render();

		//Boarder Transforms
#pragma region Boarder
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(boarderTranslate * boarderScale));
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		glBindTexture(GL_TEXTURE_2D, boarderTex.texture);
		boarder.render();

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(boarderTranslate2 * boarderScale));
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		glBindTexture(GL_TEXTURE_2D, boarderTex.texture);
		boarder2.render();

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(boarderTranslate3 * boarderScale2));
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		glBindTexture(GL_TEXTURE_2D, boarderTex.texture);
		boarder3.render();

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(boarderTranslate4 * boarderScale2));
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		glBindTexture(GL_TEXTURE_2D, boarderTex.texture);
		boarder4.render();
#pragma endregion


		// Heart Transform
		for (int i = 0; i < 3; i++)
		{
			heartTranslate = glm::translate(heartTranslate, glm::vec3(0.19f, 0.0f, 0.0f));
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(heartTranslate * heartMove * gameRotate));
			glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
			if (i < lives)
				glBindTexture(GL_TEXTURE_2D, heartTex.texture);
			else
				glBindTexture(GL_TEXTURE_2D, emptyTex.texture);
			heart[i].render();
		}
		heartTranslate = glm::mat4(1.0f);

		// Paddle Transforms
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(paddleTranslate));
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		glBindTexture(GL_TEXTURE_2D, paddleTex.texture);
		paddle.render();

		paddle.pos = glm::vec3(paddleTranslate[3]);

		// Brick Transforms - Layout
		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < 7; j++)
			{	
				modelMatrix = brickTranslate * mScale;

				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(brickTranslate));
				glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
				glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
				

				brick[i][j].pos.x = brickTranslate[3].x;
				brick[i][j].pos.y = brickTranslate[3].y;

				// If Ball hits && Brick hasn't been hit yet... 
				if (CollisionBrick(brick[i][j], ball) && brick[i][j].hit == false)
				{
					// ...do this
					SDL_Log("Hit Brick");
					scoreCount++;
					ballMove.y = -ballMove.y;
					brick[i][j].hit = true;
				}

				// If brick has been hit...
				if (brick[i][j].hit == true)
				{
					//...move brick
					brickTranslate = glm::translate(brickTranslate, glm::vec3(1.0f, 0.0f, 0.0));
				}
				else
				{
					// Render
					brickTranslate = glm::translate(brickTranslate, glm::vec3(0.13f, 0.0f, 0.0f));
					glBindTexture(GL_TEXTURE_2D, brickTex.texture);
					brick[i][j].render();
				}
	
			}
			brickTranslate = glm::mat4(1.0f);
			brickTranslate = glm::translate(brickTranslate, glm::vec3(0.0f, 0.6f - ((i + 1) / 5.0f), 0.0f));
		}
		brickTranslate = glm::mat4(1.0f);

		// Ball Transforms
		ball.pos = glm::vec3(ballTranslate[3]);
		if (!ballReady)
		{
			// Wall Collision Detection
			// Top
			if (ball.pos.y <= 0.01f)
			{
				ballMove.y = abs(-ballMove.y);
				SDL_Log("Wall-Ball Collision: %f", ballTranslate[3].y);
			}
			// Bottom
			if (ball.pos.y >= 1.0f)
			{
				ballMove.y = abs(-ballMove.y);
				lives -= 1;
				if (lives < 1)
					gameOver = true;
				SDL_Log("Missed Ball Y: %f! Lives Left: %i! Resetting the Position!", ball.pos.y, lives);
				ballTranslate = glm::translate(paddleTranslate, glm::vec3(0.05f, -0.04f,0.0f));
				ballReady = true;
			}
			// Right
			if (ball.pos.x >= 0.98f)
			{
				ballMove.x = -ballMove.x;
				SDL_Log("Wall-Ball Collision: %f", ballTranslate[3].x);
		
			}
			// Left
			if (ball.pos.x <= 0.02f)
			{
				ballMove.x = abs(-ballMove.x);
				SDL_Log("Wall-Ball Collision: %f", ballTranslate[3].x);
			}

			// Constant ball movement
			ballTranslate = glm::translate(ballTranslate, ballMove);
		}
		// Render
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(ballTranslate));
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		glBindTexture(GL_TEXTURE_2D, ballTex.texture);
		ball.render();

		// Paddle-Ball Collision
		if (CollisionPaddle(paddle, ball))
		{
			ballMove.y = -abs(ballMove.y);
		}

		// Game Over Image Transforms
		if (gameOver)
		{
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(gameoverTranslate * gameoverScale));
			glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
			glBindTexture(GL_TEXTURE_2D, goTex.texture);
			GameOver.render();
		}
				
		// Swap front and back buffers
		SDL_GL_SwapWindow(window);

		// Input Processing
		if (SDL_PollEvent(&event))
		{
			// Program Quitting
			if (event.type == SDL_QUIT)
			{
				windowOpen = false;
			}

			// Window Resizing
			if (event.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				int w, h;
				SDL_GetWindowSize(window, &w, &h);
				glViewport(0, 0, w, h);
				SDL_Log("Resized Window");			
			}

			// Inputs
			switch (event.key.keysym.sym)
			{ 
			case SDLK_a:
				if (paddle.pos.x >= 0.05f)
				{
					// Objects Movement
					paddleTranslate = glm::translate(paddleTranslate, -paddleMove);
					backgroundTranslate = glm::translate(backgroundTranslate, -parallax);
					bg2Translate = glm::translate(bg2Translate, -parallax2);
					heartMove = glm::translate(heartMove, -heartParallax);
					scoreTranslate1 = glm::translate(scoreTranslate1, -scoreParallax);
					scoreTranslate2 = glm::translate(scoreTranslate2, -scoreParallax);

					// Ball follows the paddle
					if (ballReady)
						ballTranslate = glm::translate(ballTranslate, -paddleMove);
				}
				break;
			case SDLK_d:
				if ((paddle.pos.x + 0.1f) <= 0.99f)
				{
					// Objects Movement
					paddleTranslate = glm::translate(paddleTranslate, paddleMove);
					backgroundTranslate = glm::translate(backgroundTranslate, parallax);
					bg2Translate = glm::translate(bg2Translate, parallax2);
					heartMove = glm::translate(heartMove, heartParallax);
					scoreTranslate1 = glm::translate(scoreTranslate1, scoreParallax);
					scoreTranslate2 = glm::translate(scoreTranslate2, scoreParallax);

					// Ball follows the paddle
					if (ballReady)
						ballTranslate = glm::translate(ballTranslate, paddleMove);
				}
				break;
			case SDLK_f:
				SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
				SDL_Log("Fullscreen");
				break;
			case SDLK_g:
				SDL_SetWindowFullscreen(window, 0);
				SDL_Log("Windowed");
				break;
			case SDLK_ESCAPE:
				windowOpen = false;
				break;
			case SDLK_SPACE:
				ballReady = false;
				break;
			}
		}
	}

	// Clean up
	Cleanup(glcontext);

	return 0;
}

// Paddle + Ball Collision
bool CollisionPaddle(Paddle paddle, Ball ball)
{
	float leftA, leftB, rightA, rightB, topA, topB, bottomA, bottomB;

	// Paddle + tuned offset
	leftA = paddle.pos.x;
	rightA = paddle.pos.x + 0.1f;
	topA = paddle.pos.y - 0.02f; 
	bottomA = paddle.pos.y + 0.05f;

	//SDL_Log("PADDLE: left %f | right %f | top %f | bottom %f" , leftA, rightA, topA, bottomA);

	// Ball
	leftB = ball.pos.x;
	rightB = ball.pos.x;
	topB = ball.pos.y;
	bottomB = ball.pos.y;

	//SDL_Log("BALL: left %f | right %f | top %f | bottom %f", leftB, rightB, topB, bottomB);

	// **********************
	
	if (bottomA <= topB)
	{
		return false;
	}
	if (topA >= bottomB)
	{
		return false;
	}
	if (rightA <= leftB)
	{
		return false;
	}
	if (leftA >= rightB)
	{
		return false;
	}

	return true;
}

// Brick + Ball Collision
int CollisionBrick(Brick brick, Ball ball)
{
	float leftA, leftB, rightA, rightB, topA, topB, bottomA, bottomB;

	// Brick + tuned offset
	leftA = brick.pos.x;
	rightA = brick.pos.x + 0.1f;
	topA = brick.pos.y;
	bottomA = brick.pos.y + 0.05f;

	//SDL_Log("X: %f , Y: %f", brick.pos.x, brick.pos.y);

	// Ball
	leftB = ball.pos.x;
	rightB = ball.pos.x + ball.scale.y;
	topB = ball.pos.y;
	bottomB = ball.pos.y + ball.scale.y;

	if (bottomA <= topB)
	{
		return false;
	}

	if (topA >= bottomB)
	{

		return false;
	}

	if (rightA <= leftB)
	{
		return false;
	}

	if (leftA >= rightB)
	{
		return false;
	}
	return true;
}

// SDL + GL Clean Up
void Cleanup(SDL_GLContext glcontext)
{
	// Clean up
	SDL_Log("Finished. Cleaning up and closing down\n");

	// Once finished with OpenGL functions, the SDL_GLContext can be deleted.
	SDL_GL_DeleteContext(glcontext);

	SDL_Quit();
}

