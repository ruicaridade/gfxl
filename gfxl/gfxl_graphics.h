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

	// TODO: Move this to Common, perhaps.
	const char* GetError();

	Camera* AllocCamera();
	Mesh* AllocMesh();
	Shader* AllocShader();

	bool AttachSourceAndCompile(Shader* shader, const char* filename, ShaderType type);
	bool LinkShader(Shader* shader);
	
	void UploadDataToMesh(Mesh* mesh,
		Vertex* vertices, uint32 vertexCount,
		uint32* indices, uint32 indexCount);

	void UpdateCamera(Camera* camera);
	void SetCameraToPerspective(Camera* camera, float fov, float aspectRatio, float near, float far);

	void Bind(Shader* shader);
	
	void Render(Mesh* mesh, Primitive primitive = Primitive::Triangles);

	void Dispose(Shader* shader);
	void Dispose(Mesh* mesh);
	void Dispose(Camera* camera);
}

#endif