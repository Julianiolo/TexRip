#include "RenderTexture.h"

#include "external/glad.h"
#include "rlgl.h"
#include <iostream>

void CheckOpenGLError(const char* stmt, const char* fname, int line)
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		printf("OpenGL error %08x, at %s:%i - for %s\n", err, fname, line, stmt);
		abort();
	}
}

#ifdef _DEBUG
#define GL_CHECK(stmt) do { \
            stmt; \
            CheckOpenGLError(#stmt, __FILE__, __LINE__); \
        } while (0)
#else
#define GL_CHECK(stmt) stmt
#endif

raylib::RenderTexture::RenderTexture(int width, int height, SmoothAmt smth) {
	setup(width, height, smth);
}
raylib::RenderTexture::~RenderTexture() {
	unload();
}

bool raylib::RenderTexture::isMSTex() {
	return smoothing != NO_SMOOTH;
}

void raylib::RenderTexture::setup(int width, int height, SmoothAmt smth) {
	smoothing = smth;
	if (!isMSTex()) {
		setupTex(width, height);
	}
	else {
#if SUPPORT_MSTEX
		setupMSTex(width,height);
#endif
	}
}
void raylib::RenderTexture::setupTex(int width, int height) {
	renderTex = LoadRenderTexture(width, height);
}
#if SUPPORT_MSTEX
void raylib::RenderTexture::setupMSTex(int width, int height) {
	GL_CHECK(glEnable(GL_MULTISAMPLE));

	renderTex = LoadRenderTexture(width, height);

	GL_CHECK(glGenTextures(1, &msTexId));
	GL_CHECK(glGenFramebuffers(1, &fboId));

	GL_CHECK(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, msTexId));
	GL_CHECK(glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, smoothing, GL_RGBA8, width, height, false));
}
#endif

void raylib::RenderTexture::unload(bool unloadRenderTex) {
	if (!isMSTex()) {
		unloadTex();
	}
	else {
#if SUPPORT_MSTEX
		unloadMSTex();
#endif
	}
}
void raylib::RenderTexture::unloadTex(bool unloadRenderTex) {
	if (unloadRenderTex) {
		UnloadRenderTexture(renderTex);
	}
}
#if SUPPORT_MSTEX
void raylib::RenderTexture::unloadMSTex(bool unloadRenderTex) {
	glDeleteFramebuffers(1, &fboId);
	glDeleteTextures(1,&msTexId);

	if (unloadRenderTex) {
		UnloadRenderTexture(renderTex);
	}
}
#endif

void raylib::RenderTexture::setSmoothing(SmoothAmt smth) {
	if (smth != smoothing) {
		int width = renderTex.texture.width, height = renderTex.texture.height;
		unload(false);
		setup(width, height, smth);
	}
}

const Texture2D& raylib::RenderTexture::getTex() const {
	return renderTex.texture;
}

void raylib::RenderTexture::BeginTextureMode() {
	if (!isMSTex()) {
		::BeginTextureMode(renderTex);
	}
	else {
#if SUPPORT_MSTEX
		rlDrawRenderBatchActive();      // Update and draw internal render batch
		
		GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, fboId));
		GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, msTexId, 0));

		rlViewport(0, 0, renderTex.texture.width, renderTex.texture.height); // Set viewport to framebuffer size

		rlMatrixMode(RL_PROJECTION);    // Switch to projection matrix
		rlLoadIdentity();               // Reset current matrix (projection)

										// Set orthographic projection to current framebuffer size
										// NOTE: Configured top-left corner as (0, 0)
		rlOrtho(0, renderTex.texture.width, renderTex.texture.height, 0, 0.0f, 1.0f);

		rlMatrixMode(RL_MODELVIEW);     // Switch back to modelview matrix
		rlLoadIdentity();               // Reset current matrix (modelview)
#endif
	}
}

void raylib::RenderTexture::EndTextureMode() {
	if (!isMSTex()) {
		::EndTextureMode();
	}
	else {
#if SUPPORT_MSTEX
		rlDrawRenderBatchActive();      // Update and draw internal render batch
		
		GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));

		GL_CHECK(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, renderTex.id)); //TexOut.id
		GL_CHECK(glBindFramebuffer(GL_READ_FRAMEBUFFER, fboId));
		GL_CHECK(glDrawBuffer(GL_COLOR_ATTACHMENT0)); //GL_BACK
		GL_CHECK(glBlitFramebuffer(0, 0, renderTex.texture.width, renderTex.texture.height, 0, 0, renderTex.texture.width, renderTex.texture.height, GL_COLOR_BUFFER_BIT, GL_LINEAR));

		GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));

		SetupViewport(0, 0);// Set viewport to default framebuffer size
#endif
	}
}

/*

//if (!isMSTex) {
return renderTex.texture;
//}
//else {
//	return TexOut;
//}

#if 0
TexOut.id = rlLoadTexture(NULL, width, height, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);
TexOut.width = width;
TexOut.height = height;
TexOut.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
TexOut.mipmaps = 1;
#endif // 0

//Texture2D TexOut;

*/