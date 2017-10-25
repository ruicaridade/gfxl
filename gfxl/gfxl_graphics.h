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
		Vector2 texcoord;
	};
	
	struct Shader;
	struct Mesh;
	struct Texture;
	
	struct CameraImpl;
	struct Camera
	{
		Vector3 position;
		Vector3 lookAt;
		CameraImpl* impl;
	};

	enum class ShaderType : int
	{
		Vertex,
		Fragment,
		Geometry
	};

	enum class Primitive : int
	{
		Triangles
	};

	Camera* AllocCamera();
	Mesh* AllocMesh();
	Shader* AllocShader();
	Texture* AllocTexture();

	bool ShaderLoadAndCompile(Shader* shader, const char* filename, ShaderType type);
	bool ShaderLink(Shader* shader);
	void ShaderSetVar(const Shader* shader, const char* name, const Vector2& value);
	void ShaderSetVar(const Shader* shader, const char* name, const Vector3& value);
	void ShaderSetVar(const Shader* shader, const char* name, const Vector4& value);
	void ShaderSetVar(const Shader* shader, const char* name, const Matrix4& value);
	void ShaderSetVar(const Shader* shader, const char* name, float value);
	void ShaderSetVar(const Shader* shader, const char* name, int value);
	
	void MeshLoadFromModel(Mesh* mesh, const char* filename);
	void MeshUploadData(Mesh* mesh,
		const Vertex* vertices, uint32 vertexCount,
		const uint32* indices, uint32 indexCount);

	void CameraUpdate(Camera* camera);
	void CameraSetToPerspective(Camera* camera, float fov, float aspectRatio, float nearPlane, float farPlane);

	void Bind(const Shader* shader);
	
	void Render(const Mesh* mesh, Primitive primitive = Primitive::Triangles);

	void Dispose(Shader* shader);
	void Dispose(Mesh* mesh);
	void Dispose(Camera* camera);
	void Dispose(Texture* texture);
}

#endif