#pragma once

#include "Shader.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.hpp"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define _USE_MATH_DEFINES
#include <math.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>

// all callback functions must be declared in 'C' style.
// therefore, we cannot use class methods as callback.
// whenever the window size changed, this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

namespace GL
{
	const std::string PATH_BG_VERT_SHADER = "res\\teapot\\back.vert";
	const std::string PATH_BG_FRAG_SHADER = "res\\teapot\\back.frag";
	const std::string PATH_TEAPOT_VERT_SHADER = "res\\teapot\\teapot.vert";
	const std::string PATH_TEAPOT_FRAG_SHADER = "res\\teapot\\teapot.frag";
	const std::string PATH_TEAPOT_MODEL = "res\\teapot\\teapot.obj";
	const std::string PATH_TEAPOT_TEXTURE = "res\\teapot\\texture.jpg";
	const unsigned int SCR_WIDTH = 1280;
	const unsigned int SCR_HEIGHT = 720;
	const float ZFAR = 10000.0f;
	const float ZNEAR = 0.00001f;
	const int FEATURE_TYPE_MARKER = 0;
	const int FEATURE_TYPE_NATURAL = 1;


	/// GLFW
	GLFWwindow* window;

	/// OpenGL
	// background
	Shader* backShader;
	unsigned int backTexture;
	unsigned int backVBO, backVAO, backEBO;
	// teapot
	Shader* teapotShader;
	unsigned int teapotTexture;
	unsigned int teapotUBO, teapotVAO;
	unsigned int* teapotVBO, * teapotEBO;
	// obj loader
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	int indexCount;
	// matrix
	glm::mat4 modelMatrix;
	glm::mat4 viewMatrix;
	glm::mat4 projMatrix;
	glm::mat4 mvpMatrix;

	bool Init(float fx, float fy, float cx, float cy, int featureType)
	{
		// Initialize glfw
		// OpenGL version : 3.3
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// Create main window
		window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Game Programming OpenCV", NULL, NULL);
		if (window == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return false;
		}
		glfwMakeContextCurrent(window);

		// Add callback function
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

		// glad: load all OpenGL function pointers
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return false;
		}

		// Configure global opengl state
		glEnable(GL_DEPTH_TEST);
		glCullFace(GL_BACK);

		// Set matrices
		modelMatrix = glm::rotate(glm::mat4(1.0f), (float)M_PI / 2.0f, glm::vec3(1, 0, 0));
		switch (featureType)
		{
		case FEATURE_TYPE_MARKER:
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, 0));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.001f, -0.001f, 0.001f));
			break;

		case FEATURE_TYPE_NATURAL:
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.15, 0, 0.09));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.001f, 0.001f, 0.001f));
			break;
		}

		// set projection matrix
		projMatrix[0][0] = 2.0 * fx / SCR_WIDTH;
		projMatrix[0][1] = 0.0;
		projMatrix[0][2] = 0.0;
		projMatrix[0][3] = 0.0;

		projMatrix[1][0] = 0.0;
		projMatrix[1][1] = 2.0 * fy / SCR_HEIGHT;
		projMatrix[1][2] = 0.0;
		projMatrix[1][3] = 0.0;

		projMatrix[2][0] = (2.0 * cx / SCR_WIDTH - 1.0);
		projMatrix[2][1] = -(2.0 * cy / SCR_HEIGHT - 1.0);
		projMatrix[2][2] = (ZFAR + ZNEAR) / (ZFAR - ZNEAR);
		projMatrix[2][3] = 1.0;

		projMatrix[3][0] = 0.0;
		projMatrix[3][1] = 0.0;
		projMatrix[3][2] = -2.0 * ZFAR * ZNEAR / (ZFAR - ZNEAR);
		projMatrix[3][3] = 0.0;

		return true;
	}

	bool CreateBackground()
	{
		backShader = new Shader(PATH_BG_VERT_SHADER.c_str(), PATH_BG_FRAG_SHADER.c_str());
		backShader->use();

		float vertices[] =
		{
			// positions             // texture coords
			 1.0f,  1.0f, 0.0f,      1.0f, 0.0f, // top right
			 1.0f, -1.0f, 0.0f,      1.0f, 1.0f, // bottom right
			-1.0f, -1.0f, 0.0f,      0.0f, 1.0f, // bottom left
			-1.0f,  1.0f, 0.0f,      0.0f, 0.0f  // top left 
		};

		unsigned int indices[] =
		{
			0, 1, 3, // first triangle
			1, 2, 3  // second triangle
		};

		// create GL buffers
		glGenVertexArrays(1, &backVAO);
		glGenBuffers(1, &backVBO);
		glGenBuffers(1, &backEBO);

		glBindVertexArray(backVAO);

		glBindBuffer(GL_ARRAY_BUFFER, backVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, backEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// texture coord attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		// load and create a texture 
		glGenTextures(1, &backTexture);
		glBindTexture(GL_TEXTURE_2D, backTexture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		backShader->use();
		backShader->setInt("texture1", 0);

		return true;
	}

	bool CreateTeapot()
	{
		teapotShader = new Shader(PATH_TEAPOT_VERT_SHADER.c_str(), PATH_TEAPOT_FRAG_SHADER.c_str());

		teapotShader->use();
		teapotShader->setVec3("lightPos", glm::vec3(-0.2, -1, -0.1));
		teapotShader->setVec3("viewPos", glm::vec3(0, 0, 0));
		//teapotShader->setVec3("objectColor", glm::vec3(0.25, 0.7, 0.4));
		teapotShader->setVec3("lightColor", glm::vec3(1, 1, 1));

		std::string inputfile = PATH_TEAPOT_MODEL;
		std::string err;

		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, inputfile.c_str());

		if (!err.empty()) { // `err` may contain warning message.
			std::cerr << err << std::endl;
		}

		if (!ret) {
			std::cout << "Failed to load teapot." << std::endl;
			return false;
		}

		// create GL buffers
		glGenVertexArrays(1, &teapotVAO);
		teapotVBO = new unsigned int[3];
		glGenBuffers(3, teapotVBO);
		// glGenBuffers(1, &teapotUBO);
		int s = shapes.size();
		teapotEBO = new unsigned int[s];
		glGenBuffers(s, teapotEBO);

		glBindVertexArray(teapotVAO);

		glBindBuffer(GL_ARRAY_BUFFER, teapotVBO[0]);
		glBufferData(GL_ARRAY_BUFFER, attrib.vertices.size() * sizeof(float), &attrib.vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, teapotVBO[1]);
		glBufferData(GL_ARRAY_BUFFER, attrib.normals.size() * sizeof(float), &attrib.normals[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, teapotVBO[2]);
		glBufferData(GL_ARRAY_BUFFER, attrib.texcoords.size() * sizeof(float), &attrib.texcoords[0], GL_STATIC_DRAW);

		indexCount = 0;
		for (int i = 0; i < s; i++)
		{
			std::vector<int> indices;
			for (int j = 0; j < shapes[i].mesh.indices.size(); j++)
			{
				indices.push_back(shapes[i].mesh.indices[j].vertex_index);

			}
			indexCount += indices.size();
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, teapotEBO[i]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);

		}
		// position attribute
		glBindBuffer(GL_ARRAY_BUFFER, teapotVBO[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);
		// normal
		glBindBuffer(GL_ARRAY_BUFFER, teapotVBO[1]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(1);
		// texture coord attribute
		glBindBuffer(GL_ARRAY_BUFFER, teapotVBO[2]);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(2);

		// load and create a texture 
		glGenTextures(1, &teapotTexture);
		glBindTexture(GL_TEXTURE_2D, teapotTexture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		int width, height, nrChannels;
		unsigned char* data = stbi_load(PATH_TEAPOT_TEXTURE.c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load teapot texture." << std::endl;
			return false;
		}
		stbi_image_free(data);

		GL::teapotShader->use();
		GL::teapotShader->setMat4("model", GL::modelMatrix);
		GL::teapotShader->setMat4("proj", GL::projMatrix);
		GL::teapotShader->setFloat("size", 1.0f);
		GL::teapotShader->setInt("teapotTexture", 1);

		return true;
	}

	void MakeViewMatrix(double rodrigues[], double t[])
	{
		viewMatrix[0][0] = rodrigues[0];
		viewMatrix[0][1] = rodrigues[1];
		viewMatrix[0][2] = rodrigues[2];
		viewMatrix[0][3] = 0.0;

		viewMatrix[1][0] = rodrigues[3];
		viewMatrix[1][1] = rodrigues[4];
		viewMatrix[1][2] = rodrigues[5];
		viewMatrix[1][3] = 0.0;

		viewMatrix[2][0] = rodrigues[6];
		viewMatrix[2][1] = rodrigues[7];
		viewMatrix[2][2] = rodrigues[8];
		viewMatrix[2][3] = 0.0;

		viewMatrix[3][0] = t[0];
		viewMatrix[3][1] = t[1];
		viewMatrix[3][2] = t[2];
		viewMatrix[3][3] = 1.0;

		static float changeCoordArray[16] = { 1,  0,  0, 0,
										  0, -1,  0, 0,
										  0,  0,  1, 0,
										  0,  0,  0, 1 };
		auto change_coord = glm::make_mat4(changeCoordArray);

		viewMatrix = change_coord * viewMatrix;
		viewMatrix = glm::rotate(viewMatrix, glm::radians(180.0f), glm::vec3(1.0, 0.0, 0.0));
	}

	void ProcessInput()
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
	}

	void Dispose()
	{
		glfwTerminate();
	}
}