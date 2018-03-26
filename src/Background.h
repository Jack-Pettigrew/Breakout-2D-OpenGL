#pragma once
#include <GL/glew.h>
#include <array>

class Background
{
public:

	// Buffer + Array Objects
	GLuint VBO;
	GLuint VAO;
	GLuint EBO;
	
	// Object Vertices
	GLfloat vertices[32] = {
		//vertices				colours				texture coordinates
		-0.5f, 0.0f, 0.0f,		0.0f, 0.5f, 0.0f,   1.0f, 1.0f,
		1.5f, 0.0f, 0.0f,		0.5f, 0.0f, 0.0f,   0.0f, 1.0f,
		-0.5f, 1.0f, 0.0f,		0.0f, 0.0f, 0.5f,   1.0f, 0.0f,
		1.5f, 1.0f, 0.0f,		0.5f, 0.0f, 0.5f,   0.0f, 0.0f
	};

	// Shared Indices for Square
	GLuint indices[6] = {
		0, 1, 2,
		1, 2, 3
	};

	// Constructor
	Background() {};

	void setBuffers()
	{
		// Create VBO, VAO, EBO
		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &EBO);

		// Bind VAO
		glBindVertexArray(VAO);

		// Store vertices in VBO
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// EBO Setup
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// Vertex Attrib Pointers
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		// Colour Attrib Pointers
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		// Texture Attrib Pointers
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		// Unbind the VAO
		glBindVertexArray(0);
	}

	void render()
	{
		// Render 
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

};
