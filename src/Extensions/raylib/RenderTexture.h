#ifndef _RL_TEXTURE
#define _RL_TEXTURE

#include <stdint.h>
#include "raylib.h"

namespace raylib {
	class RenderTexture {
	public:
		typedef uint8_t SmoothAmt;
		enum {
			NO_SMOOTH = 0,
			SMOOTH_1,
			SMOOTH_2
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
		uint32_t fboId = 0;
		uint32_t msTexId = 0;

		void setup(int width, int height, SmoothAmt smth);
		void setupTex(int width, int height);
		void setupMSTex(int width, int height);

		void unload(bool unloadRenderTex = true);
		void unloadTex(bool unloadRenderTex = true);
		void unloadMSTex(bool unloadRenderTex = true);
	};
}

#endif // !_RL_TEXTURE
