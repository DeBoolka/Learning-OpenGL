#include <stdio.h>

#include <GL\glew.h>
#include<GL\GL.h>

#ifdef	_WIN32
#include <GL/wglew.h>
#else
#include <GL/glxew.h>
#endif

#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#include "tools.h"

#ifdef _DEBUG
#endif _DEBUG

#define BUFFER_OFFSET(i) ((void*)(i))

#define WINDOW_WIDTH	720
#define WINDOW_HEIGHT	720

GLenum  glError;
GLint	debug_string_length;
GLchar	debug_string[1024] = { 0 };


GLuint indicesBufferID;
//Треугольники, строющиеся по вершинам.
GLushort indicesBuffer[] =
{
	0, 1, 2,
	2, 0, 3,

	0, 4, 3,
	2, 4, 3,
	2, 4, 1,
	1, 4, 0,
};

GLuint verticesBufferID;
//Вершины
GLfloat verticesBuffer[] =
{
	-0.5f,	0.0f,	-0.5f,	1.0f, 0.0f, 0.0f,
	-0.5f,	0.0f,	0.5f,	0.0f, 1.0f, 0.0f,
	0.5f,	0.0f,	0.5f,	0.0f, 0.0f, 1.0f,
	0.5f,	0.0f,	-0.5f,	1.0f, 0.0f, 1.0f,
	0.0f,	1.0f,	0.0f,	1.0f, 1.0f, 1.0f,
};

char	*shader_source;

GLuint	fragment_shader;
GLuint	vertex_shader;

GLuint	shader_program;

GLuint	inVertexLocation;
GLuint	inColorsLocation;

GLuint vertexArrayObject;

int main(void)
{
	if (glfwInit() == GL_FALSE)
	{
		fprintf(stderr, "Ошибка при инициализации GLFW.");
		//std::cout << "Ошибка при инициализации GLFW." << std::endl;

		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL", nullptr, nullptr);

	if (window == NULL)
	{
		fprintf(stderr, "Невозможно создать окно GLFW.");
		//std::cout << "Невозможно создать окно GLFW." << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Ошибка при инициализации GLEW.");
		//std::cout << "Ошибка при инициализации GLEW." << std::endl;
		glfwTerminate();
		return -1;
	}

	glError = glGetError();

	if (glGetError()) // GL_NO_ERROR = 0
	{
		fprintf(stderr, "Что-то пошло не так.");
		//std::cout << "Что-то пошло не так." << std::endl;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Загрузка вершинного шейдера
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	shader_source = LoadShader("vertex.vsh");

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, (const char **)&shader_source, NULL);

	FreeShader(shader_source);

	glCompileShader(vertex_shader);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Загрузка фрагментного шейдера
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	shader_source = LoadShader("fragment.fsh");

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, (const char **)&shader_source, NULL);

	FreeShader(shader_source);

	glCompileShader(fragment_shader);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Связывание шейдеров
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	shader_program = glCreateProgram();

	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);

	glLinkProgram(shader_program);
	glUseProgram(shader_program);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Камера
	// поле зрения (Field of View, FOV) в 80 градусов
	// отношение w:h = 4:3
	// видим между 1 и 100 единицами перед собой
	glm::mat4 P = glm::perspective(80.f, (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 1.0f, 1000.0f);

	// камера находится в точке (0,0,5)
	// она смотрит на точку (0,0,0)
	// вектор, идущий из центра камеры к ее верху, равен (0, 1, 0)
	// то есть, камера расположена горизонтально
	glm::mat4 V = glm::lookAt(glm::vec3(0, 0, -2.0f), glm::vec3(0, 0, 0),
		glm::vec3(0, -1, 0));

	// модель повернута относительно оси OY на 30 градусов
	// по часовой стрелке, если смотреть вдоль этой оси
	glm::mat4 M = glm::rotate(glm::radians(5.f), glm::vec3(1, 0, 0));

	glm::mat4 MVP = P * V * M;


	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	glGenBuffers(1, &indicesBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesBuffer), (GLvoid *)indicesBuffer, GL_STATIC_DRAW);

	glGenBuffers(1, &verticesBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, verticesBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesBuffer), (GLvoid *)verticesBuffer, GL_STATIC_DRAW);

	inVertexLocation = glGetAttribLocation(shader_program, "inVertex");
	inColorsLocation = glGetAttribLocation(shader_program, "inColors");

	glVertexAttribPointer(inVertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, 0);
	glVertexAttribPointer(inColorsLocation, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, BUFFER_OFFSET(sizeof(GLfloat) * 3));

	glEnableVertexAttribArray(inVertexLocation);
	glEnableVertexAttribArray(inColorsLocation);

	glBindAttribLocation(shader_program, inVertexLocation, "inVertex");
	glBindAttribLocation(shader_program, inColorsLocation, "inColors");

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glError = glGetError();

	if (glError != GL_NO_ERROR)
	{
		fprintf(stderr, "Что-то пошло не так.");
		//std::cout << "Что-то пошло не так." << std::endl;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	float a = 0.0f;
	GLuint angle = glGetUniformLocation(shader_program, "alpha");
	GLint mvp = glGetUniformLocation(shader_program, "MVP");
	glUniformMatrix4fv(mvp, 1, GL_FALSE, &MVP[0][0]);
	while (glfwWindowShouldClose(window) == GL_FALSE)
	{
		a += 0.0006f;
		glUniform1f(angle, a);


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLES, sizeof(indicesBuffer) / sizeof (GLushort), GL_UNSIGNED_SHORT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
	}

	glfwTerminate();
}