#pragma once
#include "Includes.h"

class Ball
{
public:

	// Attributes
	glm::vec3 pos;
	glm::vec3 scale;

	// Buffer + Array Objects
	GLuint VBO;
	GLuint VAO;
	GLuint EBO;

	// Ball Vertices
	GLfloat vertices[240];

	// Shared Indices
	GLuint indices[87] = {
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 5,
		0, 5, 6,
		0, 6, 7,
		0, 7, 8,
		0, 8, 9,
		0, 9, 10,
		0, 10, 11,
		0, 11, 12,
		0, 12, 13,
		0, 13, 14,
		0, 14, 15,
		0, 15, 16,
		0, 16, 17,
		0, 17, 18,
		0, 18, 19,
		0, 19, 20,
		0, 20, 21,
		0, 21, 22,
		0, 22, 23,
		0, 23, 24,
		0, 24, 25,
		0, 25, 26,
		0, 26, 27,
		0, 27, 28,
		0, 28, 29
	};

	// Constructor - (radius)
	Ball(float radius)
	{
		// Setting Circle origin at 0,0,0
		vertices[0] = 0.0f;
		vertices[1] = 0.0f;
		vertices[2] = 0.0f;

		GLfloat angle = 0.0f;

		// Set remaining vertices based on radius
		for (int i = 0; i < 240; i += 8)
		{
			vertices[i] = radius * cos(angle);
			vertices[i + 1] = radius * sin(angle);
			vertices[i + 2] = 0.0f;
			//increase angle value in radians
			//(2*pi)/number of verts on circumference
			angle += (2 * 3.141) / 28.0f;
		}
	}

	// Sets all buffers for object
	void setBuffers()
	{
		// Create VBO, VAO, EBO
		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &EBO);
		
		// Bind VAO
		glBindVertexArray(VAO);

		// Stores Vertices in VBO
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

		//Unbind the VAO
		glBindVertexArray(0);

	}

	void render()
	{
		// Render
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 87, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
};
