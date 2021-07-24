#ifndef _RL_TEXTURE
#define _RL_TEXTURE

#include <stdint.h>
#include "raylib.h"

#define SUPPORT_MSTEX 1

namespace raylib {
	class RenderTexture {
	public:
		typedef uint8_t SmoothAmt;
		enum {
			NO_SMOOTH = 0,
#if SUPPORT_MSTEX
			SMOOTH_1,
			SMOOTH_2
#endif
		};

		RenderTexture(int width, int height, SmoothAmt smth);
		~RenderTexture();

		const Texture2D& getTex() const;

		void BeginTextureMode();
		void EndTextureMode();
		bool isMSTex();

		void setSmoothing(SmoothAmt smth);
	private:
		RenderTexture2D renderTex;

		SmoothAmt smoothing;
#if SUPPORT_MSTEX
		uint32_t fboId = 0;
		uint32_t msTexId = 0;
#endif

		void setup(int width, int height, SmoothAmt smth);
		void setupTex(int width, int height);
#if SUPPORT_MSTEX
		void setupMSTex(int width, int height);
#endif

		void unload(bool unloadRenderTex = true);
		void unloadTex(bool unloadRenderTex = true);
#if SUPPORT_MSTEX
		void unloadMSTex(bool unloadRenderTex = true);
#endif
	};
}

#endif // !_RL_TEXTURE
