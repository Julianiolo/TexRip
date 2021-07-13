#ifndef _SHADERS_H
#define _SHADERS_H

#include "raylib.h"

class ShaderManager {
public:
	struct Colorf {
		float r;
		float g;
		float b;
		float a;
	};

	static Shader projShader;
	static void setProjShaderVals(const Texture& inputImage, const Texture& matTex, const float* bgColor);

	static void setup();
	static void destroy();

	static void SetShaderValueMat3(const Shader& sh, int locInd, const void* value, int count);
	static void RectVerts(const Rectangle& rec, const Colorf& c = { 1,1,1,1 });
	static void RectVertsSrc(const Rectangle& rec, const Rectangle& src, const Colorf& c = { 1,1,1,1 });
};

#endif