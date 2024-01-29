#pragma once

#include <string>

#include <glm.hpp>

#include "util.hpp"

class Renderer {
public:
	void init();
	void shutdown();

	void draw_rect(Rect r);
	void draw_text(std::string message, glm::vec2 pos, Color c, bool center, float size);
};


#ifdef _WIN32 
#include "windows/renderer_windows.hpp"
#elif _GBA

#endif

