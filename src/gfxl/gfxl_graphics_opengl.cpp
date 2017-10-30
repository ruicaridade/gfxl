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

#define STB_IMAGE_IMPLEMENTATION
#include <stb\stb_image.h>

#define GFXL_SHADER_VERTEX		GL_VERTEX_SHADER
#define GFXL_SHADER_FRAGMENT	GL_FRAGMENT_SHADER
#define GFXL_SHADER_GEOMETRY	GL_GEOMETRY_SHADER

namespace gfxl
{
	struct CameraImpl
	{
		uint32 uniformBuffer;
		Matrix4 projection;
	};

	struct Shader
	{
		GLuint id;
		GLuint sources[6];
		GLuint count;
	};

	struct Mesh
	{
		GLuint vertexArray;
		GLuint vertexBuffer;
		GLuint indexBuffer;

		GLuint vertexCount;
		GLuint indexCount;
	};
	
	struct Texture2D
	{
		GLuint id;
		int width;
		int height;
	};

	struct Cubemap
	{
		GLuint id;
	};

	struct SpriteBatch
	{
		// TODO: Maybe keep a list things we should be rendering.
	};

	struct SpriteAtlas
	{
		Texture2D texture;
		Vector2i size;
	};

	Camera* CreateCamera()
	{
		Camera* camera = (Camera*)malloc(sizeof(Camera));
		*camera = {};
		camera->impl = (CameraImpl*)malloc(sizeof(CameraImpl));
		*camera->impl = {};
		return camera;
	}

	Mesh* CreateMesh()
	{
		return (Mesh*)malloc(sizeof(Mesh));
	}

	Shader* CreateShader()
	{
		Shader* ptr = (Shader*)malloc(sizeof(Shader));
		ptr->count = 0;
		return ptr;
	}

	Texture2D* CreateTexture2D()
	{
		return (Texture2D*)malloc(sizeof(Texture2D));
	}

	Cubemap* CreateCubemap()
	{
		return (Cubemap*)malloc(sizeof(Cubemap));
	}

	SpriteBatch* CreateSpriteBatch()
	{
		return (SpriteBatch*)malloc(sizeof(SpriteBatch));
	}

	SpriteAtlas* CreateSpriteAtlas()
	{
		return (SpriteAtlas*)malloc(sizeof(SpriteAtlas));
	}

	bool ShaderLoadAndCompile(Shader* shader, const char* filename, ShaderType type)
	{
		GLuint id = glCreateShader((GLenum)type);

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
			Message(info);
			glDeleteShader(id);
			return false;
		}

		shader->sources[shader->count++] = id;
		return true;
	}

	bool ShaderLink(Shader* shader)
	{
		shader->id = glCreateProgram();

		for (int i = 0; i < shader->count; i++)
			glAttachShader(shader->id, shader->sources[i]);

		glLinkProgram(shader->id);

		GLint success;
		glGetProgramiv(shader->id, GL_LINK_STATUS, &success);

		if (!success)
		{
			char info[1024];
			glGetProgramInfoLog(shader->id, 1024, nullptr, info);
			Message(info);
			glDeleteProgram(shader->id);

			for (int i = 0; i < shader->count; i++)
				glDeleteShader(shader->sources[i]);

			return false;
		}

		for (int i = 0; i < shader->count; i++)
			glDeleteShader(shader->sources[i]);

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
			Message("[ERROR] File not found");
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

	void Texture2DFromImageFile(Texture2D* texture, const char * filename)
	{
		glGenTextures(1, &texture->id);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture->id);

		int channels;
		unsigned char* data = stbi_load(filename, &texture->width, &texture->height, &channels, STBI_rgb_alpha);

		if (!data)
		{
			Message("Failed to load image file.");
			glDeleteTextures(1, &texture->id);
			return;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture2DGetSize(const Texture2D* texture, int* width, int* height)
	{
		*width = texture->width;
		*height = texture->height;
	}

	static void CubemapLoadFace(GLenum face, const char* filename)
	{
		int width, height, channels;
		unsigned char* data = nullptr;
		data = stbi_load(filename, &width, &height, &channels, 0);

		if (data == nullptr)
		{
			Message("Failed to load cubemap face.");
			return;
		}

		glTexImage2D(face, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}

	void CubemapFromImageFiles(Cubemap* cubemap, const char* front, const char* back, const char* left, 
		const char* right, const char* top, const char* bottom)
	{
		glGenTextures(1, &cubemap->id);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->id);

		CubemapLoadFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X, right);
		CubemapLoadFace(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, left);
		CubemapLoadFace(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, top);
		CubemapLoadFace(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, bottom);
		CubemapLoadFace(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, back);
		CubemapLoadFace(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, front);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
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

	void Bind(const Texture2D* texture, int index)
	{
		glActiveTexture(GL_TEXTURE0 + index);
		glBindTexture(GL_TEXTURE_2D, texture->id);
	}

	void Bind(const Cubemap* cubemap, int index)
	{
		glActiveTexture(GL_TEXTURE0 + index);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->id);
	}

	void Render(const Mesh* mesh, Primitive primitive)
	{
		glBindVertexArray(mesh->vertexArray);

		if (mesh->indexBuffer == 0 || mesh->indexCount == 0)
		{
			glDrawArrays((GLenum)primitive, 0, mesh->vertexCount);
			return;
		}

		glDrawElements((GLenum)primitive, mesh->indexCount, GL_UNSIGNED_INT, 0);
	}

	void Dispose(Shader* shader)
	{
		glDeleteProgram(shader->id);
		free(shader);
	}

	void Dispose(Mesh* mesh)
	{
		if (mesh->vertexArray)
			glDeleteVertexArrays(1, &mesh->vertexArray);

		if (mesh->vertexBuffer)
			glDeleteBuffers(1, &mesh->vertexBuffer);

		if (mesh->indexBuffer)
			glDeleteBuffers(1, &mesh->indexBuffer);

		free(mesh);
	}

	void Dispose(Camera* camera)
	{
		free(camera->impl);
		free(camera);
	}

	void Dispose(Texture2D* texture)
	{
		glDeleteTextures(1, &texture->id);
		free(texture);
	}

	void Dispose(Cubemap* cubemap)
	{
		glDeleteTextures(1, &cubemap->id);
		free(cubemap);
	}
}

#endif