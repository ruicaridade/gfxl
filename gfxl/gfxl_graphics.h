#pragma once
#ifndef GFXL_GRAPHICS_H
#define GFXL_GRAPHICS_H

#include "gfxl_math.h"
#include "gfxl_common.h"

namespace gfxl
{
	struct CameraImpl;
	struct Camera
	{
		Vector3 position;
		Vector3 lookAt;
		CameraImpl* impl;
	};

	struct Vertex
	{
		Vector3 position;
		Vector3 normal;
		Vector2 texcoord;
	};

	struct Shader;
	struct Mesh;
	struct Texture2D;
	struct Cubemap;
	struct SpriteAtlas;
	struct SpriteBatch;

	enum class ShaderType : int
	{
#if GFXL_OPENGL

		Vertex = 0x8B31,
		Fragment = 0x8B30,
		Geometry = 0x8DD9

#endif
	};

	enum class Primitive : int
	{
#if GFXL_OPENGL

		Points = 0x0000,
		Lines = 0x0001,
		LineLoop = 0x0002,
		LineStrip = 0x0003,
		Triangles = 0x0004,
		TriangleStrip = 0x0005,
		TriangleFan = 0x0006

#endif
	};

	Camera* CreateCamera();
	Mesh* CreateMesh();
	Shader* CreateShader();
	Texture2D* CreateTexture2D();
	Cubemap* CreateCubemap();

	SpriteAtlas* CreateSpriteAtlas();
	SpriteBatch* CreateSpriteBatch();

	bool ShaderLoadAndCompile(Shader* shader, const char* filename, ShaderType type);
	bool ShaderLink(Shader* shader);
	void ShaderSetVar(const Shader* shader, const char* name, const Vector2& value);
	void ShaderSetVar(const Shader* shader, const char* name, const Vector3& value);
	void ShaderSetVar(const Shader* shader, const char* name, const Vector4& value);
	void ShaderSetVar(const Shader* shader, const char* name, const Matrix4& value);
	void ShaderSetVar(const Shader* shader, const char* name, float value);
	void ShaderSetVar(const Shader* shader, const char* name, int value);

	void MeshLoadFromModelFile(Mesh* mesh, const char* filename);
	void MeshUploadData(Mesh* mesh,
		const Vertex* vertices, uint32 vertexCount,
		const uint32* indices, uint32 indexCount);

	void CameraUpdate(Camera* camera);
	void CameraSetToPerspective(Camera* camera, float fov, float aspectRatio, float nearPlane, float farPlane);

	void Texture2DFromImageFile(Texture2D* texture, const char* filename);
	void Texture2DGetSize(const Texture2D* texture, int* width, int* height);

	void CubemapFromImageFiles(Cubemap* cubemap,
		const char* front,
		const char* back,
		const char* left,
		const char* right,
		const char* top,
		const char* bottom);

	void SpriteAtlasLoadFromImageFile(SpriteAtlas* atlas, const char* filename, int tileWidth, int tileHeight);

	void SpriteBatchBegin();
	void SpriteBatchRender(SpriteAtlas* atlas, int x, int y);
	void SpriteBatchEnd();

	void Bind(const Shader* shader);
	void Bind(const Texture2D* texture, int index);
	void Bind(const Cubemap* cubemap, int index);

	void Render(const Mesh* mesh, Primitive primitive = Primitive::Triangles);
	
	void Dispose(Shader* shader);
	void Dispose(Mesh* mesh);
	void Dispose(Camera* camera);
	void Dispose(Texture2D* texture);
	void Dispose(Cubemap* cubemap);
}

#endif