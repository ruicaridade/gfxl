#pragma once
#ifndef GFXL_MATH_H
#define GFXL_MATH_H

#ifdef GFXL_GLM

#include <glm\glm.hpp>
#include <glm\vec2.hpp>
#include <glm\vec3.hpp>
#include <glm\vec4.hpp>
#include <glm\matrix.hpp>

#define PI					3.14159265358979323846f
#define Dot(v1, v2)			glm::dot(v1, v2)
#define Cross(v1, v2)		glm::cross(v1, v2)
#define Normalize(v)		glm::normalize(v)
#define Magnitude(v)		glm::length(v)
#define Cos(a)				glm::cos(a)
#define Sin(a)				glm::sin(a)
#define ACos(a)				glm::acos(a)
#define ASin(a)				glm::asin(a)
#define Tan(a)				glm::tan(a)
#define ATan(a)				glm::atan(a)
#define ToDegrees(rad)		glm::degrees(rad)
#define ToRadians(deg)		glm::radians(deg)
#define Clamp(v, min, max)	glm::clamp(v, min, max)
#define Floor(x)			glm::floor(x)
#define Ceil(x)				glm::ceil(x)
#define Min(x, y)			glm::min(x, y)
#define Max(x, y)			glm::max(x, y)
#define Abs(x)				glm::abs(x)

#endif

namespace gfxl
{
#ifdef GFXL_GLM

	typedef glm::vec2 Vector2;
	typedef glm::vec3 Vector3;
	typedef glm::vec4 Vector4;
	typedef glm::mat4 Matrix4;
	typedef glm::ivec2 Vector2i;
	typedef glm::ivec3 Vector3i;
	typedef glm::ivec4 Vector4i;

#endif
}

#endif