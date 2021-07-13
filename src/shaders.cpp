#include "shaders.h"
#include "external/glad.h"
#include "rlgl.h"


#include "raylib.h"

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif

Shader ShaderManager::projShader;
void ShaderManager::setProjShaderVals(const Texture& inputImage, const Texture& matTex, const float* bgColor) {
    static int inputImgLoc = GetShaderLocation(ShaderManager::projShader, "inputImageTexture");
    SetShaderValueTexture(ShaderManager::projShader, inputImgLoc, inputImage);
    static int matTexLoc = GetShaderLocation(ShaderManager::projShader, "invMatTex");
    SetShaderValueTexture(ShaderManager::projShader, matTexLoc, matTex);

    static int wLoc = GetShaderLocation(ShaderManager::projShader, "width");
    static int hLoc = GetShaderLocation(ShaderManager::projShader, "height");
    float w = (float)inputImage.width, h = (float)inputImage.height;
    SetShaderValue(ShaderManager::projShader, wLoc, &w, SHADER_UNIFORM_FLOAT);
    SetShaderValue(ShaderManager::projShader, hLoc, &h, SHADER_UNIFORM_FLOAT);

    static int bgColorLoc = GetShaderLocation(ShaderManager::projShader, "bgColor");
    SetShaderValueV(ShaderManager::projShader, bgColorLoc, bgColor, SHADER_UNIFORM_VEC4, 1);
}

void ShaderManager::setup() {
	projShader = LoadShader(0, TextFormat("assets/shaders/glsl%i/texProjCV.glsl", GLSL_VERSION));
}

void ShaderManager::destroy() {
    UnloadShader(projShader);
}

void ShaderManager::SetShaderValueMat3(const Shader& sh, int locInd, const void* value, int count) {
	glUseProgram(sh.id);
	glUniformMatrix3fv(locInd,count,false,(const GLfloat*)value);
}

void ShaderManager::RectVerts(const Rectangle& rec, const Colorf& c) {
    rlBegin(RL_QUADS);

    rlNormal3f(0.0f, 0.0f, 1.0f);
    rlColor4f(c.r, c.g, c.b, c.a);

    rlTexCoord2f(0,0);
    rlVertex2f(rec.x, rec.y);

    rlTexCoord2f(0,1);
    rlVertex2f(rec.x,rec.y+rec.height);

    rlTexCoord2f(1,1);
    rlVertex2f(rec.x+rec.width,rec.y+rec.height);

    rlTexCoord2f(1,0);
    rlVertex2f(rec.x+rec.width,rec.y);

    rlEnd();
}

void ShaderManager::RectVertsSrc(const Rectangle& rec, const Rectangle& src, const Colorf& c) {
    rlBegin(RL_QUADS);

    rlNormal3f(0.0f, 0.0f, 1.0f);
    rlColor4f(c.r, c.g, c.b, c.a);

    rlTexCoord2f( src.x,           src.y           );
    rlVertex2f(   rec.x,           rec.y           );

    rlTexCoord2f( src.x,           src.y+src.height);
    rlVertex2f(   rec.x,           rec.y+rec.height);

    rlTexCoord2f( src.x+src.width, src.y+src.height);
    rlVertex2f(   rec.x+rec.width, rec.y+rec.height);

    rlTexCoord2f( src.x+src.width, src.y            );
    rlVertex2f(   rec.x+rec.width, rec.y            );

    rlEnd();
}