#ifdef GFXL_OPENGL

#include <vector>
#include <string>
#include <fstream>
#include <stdio.h>

#pragma comment (lib, "opengl32.lib")
#include <glad\glad.h>

#include <gfxl.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>

namespace gfxl
{
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
	
	struct Texture
	{
		GLuint id;
	};

	struct CameraImpl
	{
		uint32 uniformBuffer;
		Matrix4 projection;
	};

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

	Texture * AllocTexture()
	{
		return new Texture();
	}

	bool ShaderLoadAndCompile(Shader* shader, const char* filename, ShaderType type)
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
			char info[1024];
			glGetShaderInfoLog(id, 1024, nullptr, info);
			Error(info);
			glDeleteShader(id);
			return false;
		}

		shader->sources.push_back(id);
		return true;
	}

	bool ShaderLink(Shader* shader)
	{
		shader->id = glCreateProgram();

		for (auto src : shader->sources)
			glAttachShader(shader->id, src);

		glLinkProgram(shader->id);

		GLint success;
		glGetProgramiv(shader->id, GL_LINK_STATUS, &success);

		if (!success)
		{
			char info[1024];
			glGetProgramInfoLog(shader->id, 1024, nullptr, info);
			Error(info);
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

	void ShaderSetVar(const Shader* shader, const char * name, const Vector2& value)
	{
		int location = glGetUniformLocation(shader->id, name);
		glUniform2fv(location, 1, glm::value_ptr(value));
	}

	void ShaderSetVar(const Shader* shader, const char * name, const Vector3& value)
	{
		int location = glGetUniformLocation(shader->id, name);
		glUniform3fv(location, 1, glm::value_ptr(value));
	}

	void ShaderSetVar(const Shader* shader, const char * name, const Vector4& value)
	{
		int location = glGetUniformLocation(shader->id, name);
		glUniform4fv(location, 1, glm::value_ptr(value));
	}

	void ShaderSetVar(const Shader* shader, const char * name, const Matrix4& value)
	{
		int location = glGetUniformLocation(shader->id, name);
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

	void ShaderSetVar(const Shader* shader, const char * name, float value)
	{
		int location = glGetUniformLocation(shader->id, name);
		glUniform1f(location, value);
	}

	void ShaderSetVar(const Shader* shader, const char * name, int value)
	{
		int location = glGetUniformLocation(shader->id, name);
		glUniform1i(location, value);
	}

	void MeshLoadFromModel(Mesh* mesh, const char * filename)
	{
		FILE *file = fopen(filename, "r");
		if (file == nullptr)
		{
			Error("[ERROR] File not found");
			return;
		}

		std::vector<Vector3> positions;
		std::vector<Vector3> normals;
		std::vector<Vector2> uvs;

		std::vector<Vertex> vertices;

		while (true)
		{
			char header[16];
			int cursor = fscanf(file, "%s", header);

			if (cursor == EOF)
				break;

			if (strcmp(header, "v") == 0)
			{
				Vector3 position;
				fscanf(file, "%f %f %f\n", &position.x, &position.y, &position.z);
				positions.push_back(position);
			}
			else if (strcmp(header, "vn") == 0)
			{
				Vector3 normal;
				fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				normals.push_back(normal);
			}
			else if (strcmp(header, "vt") == 0)
			{
				Vector2 uv;
				fscanf(file, "%f %f\n", &uv.x, &uv.y);
				uvs.push_back(uv);
			}
			else if (strcmp(header, "s") == 0)
			{
				char ignore[8];
				fgets(ignore, sizeof(ignore), file);
				printf(ignore);
			}
			else if (strcmp(header, "f") == 0)
			{
				char line[128];
				fgets(line, sizeof(line), file);

				int vp[3];
				int vt[3];
				int vn[3];

				int count = sscanf(line, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
					&vp[0], &vt[0], &vn[0],
					&vp[1], &vt[1], &vn[1],
					&vp[2], &vt[2], &vn[2]);
				
				bool hasTexCoords = count == 9;
				if (!hasTexCoords)
				{
					int count = sscanf(line, "%d//%d %d//%d %d//%d\n",
						&vp[0], &vn[0],
						&vp[1], &vn[1],
						&vp[2], &vn[2]);
				}

				Vertex v[3];
				v[0].position = positions[vp[0] - 1];
				v[1].position = positions[vp[1] - 1];
				v[2].position = positions[vp[2] - 1];

				if (hasTexCoords)
				{
					v[0].texcoord = uvs[vt[0] - 1];
					v[1].texcoord = uvs[vt[1] - 1];
					v[2].texcoord = uvs[vt[2] - 1];
				}

				v[0].normal = normals[vn[0] - 1];
				v[1].normal = normals[vn[1] - 1];
				v[2].normal = normals[vn[2] - 1];

				vertices.push_back(v[0]);
				vertices.push_back(v[1]);
				vertices.push_back(v[2]);
			}
		}

		MeshUploadData(mesh,
			vertices.data(), vertices.size(),
			nullptr, 0);
	}

	void MeshUploadData(Mesh* mesh,
		const Vertex* vertices, uint32 vertexCount,
		const uint32* indices, uint32 indexCount)
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

	void CameraUpdate(Camera* camera)
	{
		if (!camera->impl->uniformBuffer)
		{
			uint32 size = sizeof(Matrix4) * 2 + sizeof(Vector3);

			glGenBuffers(1, &camera->impl->uniformBuffer);
			glBindBuffer(GL_UNIFORM_BUFFER, camera->impl->uniformBuffer);
			glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
			glBindBufferRange(GL_UNIFORM_BUFFER, 0, camera->impl->uniformBuffer, 0, size);
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

		glBufferSubData(
			GL_UNIFORM_BUFFER,
			sizeof(Matrix4) * 2,
			sizeof(Vector3),
			glm::value_ptr(camera->position));

		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void CameraSetToPerspective(Camera* camera, float fov, float aspectRatio, float nearPlane, float farPlane)
	{
		camera->impl->projection = glm::perspective(fov, aspectRatio, nearPlane, farPlane);
	}

	void Bind(const Shader* shader)
	{
		if (shader == nullptr)
		{
			glUseProgram(0);
			return;
		}

		glUseProgram(shader->id);
	}

	void Render(const Mesh* mesh, Primitive primitive)
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

	void Dispose(Texture * texture)
	{
		glDeleteTextures(1, &texture->id);
		delete texture;
	}
}

#endif