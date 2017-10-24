#pragma once
#ifndef GFXL_GRAPHICS_H
#define GFXL_GRAPHICS_H

#include "glm\vec2.hpp"
#include "glm\vec3.hpp"
#include "glm\vec4.hpp"
#include "glm\matrix.hpp"

#include "gfxl_common.h"

namespace gfxl
{
	typedef glm::vec2 Vector2;
	typedef glm::vec3 Vector3;
	typedef glm::vec4 Vector4;
	typedef glm::mat4 Matrix4;

	struct Vertex
	{
		Vector3 position;
		Vector3 normal;
		Vector3 texcoord;
	};

	struct Shader;
	struct Mesh;

	enum class ShaderType
	{
		Vertex,
		Fragment,
		Geometry
	};

	void ShaderAttach(Shader* shader, const char* filename, ShaderType type);
	void ShaderCompile(Shader* shader);
	void ShaderUse(Shader* shader);
	void ShaderDispose(Shader* shader);

	void MeshData(Mesh* mesh,
		Vertex* vertices, uint32 vertexCount,
		uint32* indices, uint32 indexCount);
	void MeshDispose(Mesh* mesh);
}

#endif