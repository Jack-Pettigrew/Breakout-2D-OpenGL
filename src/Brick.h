#pragma once
#include "Includes.h"

class Brick		// Class for Brick objects
{
public:

	glm::vec3 pos;
	glm::vec3 scale;
	bool hit = false;

	// constructor
	Brick()
	{
	};

	// defining vertices for the brick
	GLfloat vertices[32] = {
		//vertices				colours					textures
		0.0f, 0.0f, 0.0f,		0.0f, 0.5f, 0.0f,		1.0f, 1.0f,
		0.1f, 0.0f, 0.0f,		0.5f, 0.0f, 0.0f,		0.0f, 1.0f,
		0.0f, 0.1f, 0.0f,		0.0f, 0.0f, 0.5f,		1.0f, 0.0f,
		0.1f, 0.1f, 0.0f,		0.5f, 0.0f, 0.5f,		0.0f, 0.0f
	};

	//define indices for the brick
	GLuint indices[6] = {
		0, 1, 2,
		1, 2, 3
	};

	//set up vertex buffer object
	GLuint VBO;
	//set up vertex array object
	GLuint VAO;
	//set up index buffer object
	GLuint EBO;

	void setBuffers()
	{
		//
		//OpenGL buffers
		//set up 1 for the triangle
		glGenBuffers(1, &VBO);
		// Initialization code using Vertex Array Object (VAO) (done once (unless the object frequently changes))
		glGenVertexArrays(1, &VAO);
		//initialise the index buffer
		glGenBuffers(1, &EBO);

		// Bind Vertex Array Object
		glBindVertexArray(VAO);
		// Copy our vertices array in a buffer for OpenGL to use
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		//set up the EBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		// Then set our vertex attributes pointers
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		//set the colour attribute pointer
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		//the texture one
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		//Unbind the VAO
		glBindVertexArray(0);
	}

	void render()
	{
		//draw the square 
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

};
