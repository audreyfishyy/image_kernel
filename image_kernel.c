#include "glad/glad.h"
#include "glfw/glfw3.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

const char* load_ascii_file(const char* path)
{

	FILE* f = fopen(path, "rb");
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	char* string = malloc(fsize + 1);
	fread(string, fsize, 1, f);
	fclose(f);

	string[fsize] = 0;
	return string;
}

uint32_t create_texture(const char* path)
{
	int32_t width = 0;
	int32_t height = 0;
	int32_t bytes_per_pixel = 0;
	uint8_t* data = stbi_load(path, &width, &height, &bytes_per_pixel, 0);
	if (!data)
	{
		return 0;
	}

	stbi__vertical_flip(data, width, height, bytes_per_pixel);

	uint32_t id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	// filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// mipmap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLenum color_format;
	switch (bytes_per_pixel)
	{
		case 1: color_format = GL_RED; 
			break;
		case 2: color_format = GL_RG;
			break;
		case 3: color_format = GL_RGB;
			break;
		case 4: color_format = GL_RGBA;
			break;

		default: color_format = GL_RGB;
			break;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, color_format, width, height, 0, color_format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
	return id;
}

uint32_t create_shader(const char* vertex_source, const char* fragment_source)
{
	int32_t success = 0;
	char info_log[512];
	
	const uint32_t vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertex_source, 0);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, sizeof(info_log), 0, info_log);
		printf("%s\n", info_log);
	}

	const uint32_t fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragment_source, 0);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, sizeof(info_log), 0, info_log);
		printf("%s\n", info_log);
	}

	uint32_t program = 0;
	program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(program, sizeof(info_log), 0, info_log);
		printf("%s\n", info_log);
	}

	glDetachShader(program, vertex);
	glDetachShader(program, fragment);
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	free(vertex_source);
	free(fragment_source);

	return program;
}

void draw_full_screen_quad()
{
	static uint32_t quad = 0;	
	static uint32_t quad_buffer = 0;
	static uint32_t quad_indices = 0;

	if (quad != 0)
	{
		glBindVertexArray(quad);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		return;
	}

	float vertices[] = {
	     1.0f,  1.0f, 1.0f, 1.0f,  // top right
	     1.0f, -1.0f, 1.0f, 0.0f,  // bottom right
	    -1.0f, -1.0f, 0.0f, 0.0f,  // bottom left
	    -1.0,  1.0f, 0.0f, 1.0f,   // top left 
	};
	unsigned int indices[] = {
	    0, 1, 3,
	    1, 2, 3,
	};

	glGenVertexArrays(1, &quad);
	glBindVertexArray(quad);

	glGenBuffers(1, &quad_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// vertex
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coordinate
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &quad_indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

int main(void)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(2560, 1440, "image_kernel", 0, 0);

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	printf("%s\n", glGetString(GL_VERSION));

	glViewport(0, 0, 2560, 1440);

	const char* vertex_source = load_ascii_file("vertex.glsl");
	const char* fragment_source = load_ascii_file("fragment.glsl");
	uint32_t shader = create_shader(vertex_source, fragment_source);

	uint32_t texture = create_texture("texture.jpg");
	float texture_size[2] = { 1920, 1080 };

	for (;;)
	{
		glfwPollEvents();

		// hot reload
		if (GetAsyncKeyState(VK_SPACE))
		{
			system("cls");

			glDeleteProgram(shader);
			vertex_source = load_ascii_file("vertex.glsl");
			fragment_source = load_ascii_file("fragment.glsl");
			shader = create_shader(vertex_source, fragment_source);
		}

		glUseProgram(shader);
		glUniform1ui(glGetUniformLocation(shader, "u_texture"), texture);
		glUniform2fv(glGetUniformLocation(shader, "u_texture_size"), 1, texture_size);
		
		draw_full_screen_quad();
		
		glfwSwapBuffers(window);
	}

	return 0;
}