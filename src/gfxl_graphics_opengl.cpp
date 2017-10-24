#ifdef GFXL_OPENGL

#include <vector>
#include <string>
#include <fstream>
#include <stdio.h>
#include <gfxl.h>

#pragma comment (lib, "opengl32.lib")
#include <glad\glad.h>

#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>

namespace gfxl
{
	static char error[1024];

	struct Shader
	{
		GLuint id;
		std::vector<GLuint> sources;
	};

	struct Mesh
	{
		GLuint vertexArray;
		GLuint vertexBuffer;
		GLuint indexBuffer;

		GLuint vertexCount;
		GLuint indexCount;
	};

	struct CameraImpl
	{
		uint32 uniformBuffer;
		Matrix4 projection;
	};

	const char* GetError()
	{
		return error;
	}

	Camera* AllocCamera()
	{
		Camera* camera = new Camera();
		camera->impl = new CameraImpl();
		return camera;
	}

	Mesh* AllocMesh()
	{
		return new Mesh();
	}

	Shader* AllocShader()
	{
		return new Shader();
	}

	bool AttachSourceAndCompile(Shader* shader, const char* filename, ShaderType type)
	{
		GLenum glType = 0;
		if (type == ShaderType::Vertex)
			glType = GL_VERTEX_SHADER;
		else if (type == ShaderType::Fragment)
			glType = GL_FRAGMENT_SHADER;
		else if (type == ShaderType::Geometry)
			glType = GL_GEOMETRY_SHADER;

		GLuint id = glCreateShader(glType);

		std::ifstream file(filename);
		std::string contents((std::istreambuf_iterator<char>(file)),
			std::istreambuf_iterator<char>());
		
		const GLchar* source = contents.c_str();
		glShaderSource(id, 1, &source, nullptr);
		glCompileShader(id);

		GLint success = 0;
		glGetShaderiv(id, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(id, 1024, nullptr, error);
			glDeleteShader(id);
			return false;
		}

		shader->sources.push_back(id);
		return true;
	}

	bool LinkShader(Shader* shader)
	{
		shader->id = glCreateProgram();

		for (auto src : shader->sources)
			glAttachShader(shader->id, src);

		glLinkProgram(shader->id);

		GLint success;
		glGetProgramiv(shader->id, GL_LINK_STATUS, &success);

		if (!success)
		{
			glGetProgramInfoLog(shader->id, 1024, nullptr, error);
			glDeleteProgram(shader->id);

			for (auto src : shader->sources)
				glDeleteShader(src);

			return false;
		}

		for (auto src : shader->sources)
			glDeleteShader(src);

		shader->sources.clear();
		return true;
	}

	void Bind(Shader* shader)
	{
		if (shader == nullptr)
		{
			glUseProgram(0);
			return;
		}

		glUseProgram(shader->id);
	}

	void UploadDataToMesh(Mesh* mesh,
		Vertex* vertices, uint32 vertexCount,
		uint32* indices, uint32 indexCount)
	{
		glGenVertexArrays(1, &mesh->vertexArray);
		glBindVertexArray(mesh->vertexArray);
		
		glGenBuffers(1, &mesh->vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexCount, vertices, GL_STATIC_DRAW);

		if (indices != nullptr && indexCount > 0)
		{
			glGenBuffers(1, &mesh->indexBuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indexCount, indices, GL_STATIC_DRAW);
		}

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texcoord));
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);

		mesh->vertexCount = vertexCount;
		mesh->indexCount = indexCount;
	}

	void UpdateCamera(Camera* camera)
	{
		if (!camera->impl->uniformBuffer)
		{
			glGenBuffers(1, &camera->impl->uniformBuffer);
			glBindBuffer(GL_UNIFORM_BUFFER, camera->impl->uniformBuffer);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(Matrix4) * 2, nullptr, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
			glBindBufferRange(GL_UNIFORM_BUFFER, 0, camera->impl->uniformBuffer, 0, sizeof(Matrix4) * 2);
		}

		glBindBuffer(GL_UNIFORM_BUFFER, camera->impl->uniformBuffer);

		Matrix4 view = glm::lookAt(camera->position, camera->lookAt, Vector3(0, 1, 0));
		
		glBufferSubData(
			GL_UNIFORM_BUFFER,
			0,
			sizeof(Matrix4),
			glm::value_ptr(view));

		glBufferSubData(
			GL_UNIFORM_BUFFER,
			sizeof(Matrix4),
			sizeof(Matrix4),
			glm::value_ptr(camera->impl->projection));

		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void SetCameraToPerspective(Camera* camera, float fov, float aspectRatio, float nearPlane, float farPlane)
	{
		camera->impl->projection = glm::perspective(fov, aspectRatio, nearPlane, farPlane);
	}

	void Render(Mesh* mesh, Primitive primitive)
	{
		GLenum glPrimitive = 0;
		if (primitive == Primitive::Triangles)
			glPrimitive = GL_TRIANGLES;

		glBindVertexArray(mesh->vertexArray);

		if (mesh->indexBuffer == 0 || mesh->indexCount == 0)
		{
			glDrawArrays(glPrimitive, 0, mesh->vertexCount);
			return;
		}

		glDrawElements(glPrimitive, mesh->indexCount, GL_UNSIGNED_INT, 0);
	}

	void Dispose(Shader* shader)
	{
		glDeleteProgram(shader->id);
		delete shader;
	}

	void Dispose(Mesh* mesh)
	{
		if (mesh->vertexArray)
			glDeleteVertexArrays(1, &mesh->vertexArray);

		if (mesh->vertexBuffer)
			glDeleteBuffers(1, &mesh->vertexBuffer);

		if (mesh->indexBuffer)
			glDeleteBuffers(1, &mesh->indexBuffer);

		delete mesh;
	}

	void Dispose(Camera* camera)
	{
		delete camera->impl;
		delete camera;
	}
}

#endif