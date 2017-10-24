#ifdef GFXL_OPENGL

#include <gfxl.h>

#pragma comment (lib, "opengl32.lib")
#include <glad\glad.h>

namespace gfxl
{
	struct Shader
	{
		uint32 id;
	};

	struct Mesh
	{
		uint32 vertexArray;
		uint32 vertexBuffer;
		uint32 indexBuffer;

		uint32 vertexCount;
		uint32 indexCount;
	};

	void ShaderAttach(Shader* shader, const char* filename, ShaderType type)
	{

	}

	void ShaderCompile(Shader* shader)
	{

	}

	void ShaderUse(Shader* shader) 
	{

	}

	void ShaderDispose(Shader* shader)
	{

	}

	void MeshData(Mesh* mesh,
		Vertex* vertices, uint32 vertexCount,
		uint32* indices, uint32 indexCount)
	{
		//glGenBuffers(1, &m_ivbo);

		glGenVertexArrays(1, &m_vao);

		glGenBuffers(1, &mesh->vertexBuffer);
		glGenBuffers(1, &mesh->indexBuffer);

		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, color));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, uv));
		glEnableVertexAttribArray(3);
		
		// TODO: Instancing
		//glBindBuffer(GL_ARRAY_BUFFER, m_ivbo);
		//glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix4), (void*)(0));
		//glEnableVertexAttribArray(4);
		//glVertexAttribDivisor(4, 1);

		//glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix4), (void*)(sizeof(GLfloat) * 4));
		//glEnableVertexAttribArray(5);
		//glVertexAttribDivisor(5, 1);

		//glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix4), (void*)(sizeof(GLfloat) * 8));
		//glEnableVertexAttribArray(6);
		//glVertexAttribDivisor(6, 1);

		//glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix4), (void*)(sizeof(GLfloat) * 12));
		//glEnableVertexAttribArray(7);
		//glVertexAttribDivisor(7, 1);

		glBindVertexArray(0);
	}

	void MeshDispose(Mesh* mesh)
	{

	}
}

#endif