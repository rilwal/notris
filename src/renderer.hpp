#pragma once

#include <string>

#include <glm.hpp>
#include <SDL.h>
#include <SDL_ttf.h>

#include "util.hpp"

class Renderer {
public:
	~Renderer();

	void init(SDL_Window* window);
	void update();
	void shutdown();

	void set_background_color(Color c) { m_background_color = c; }

	void draw_rect(Rect r, Color c);
	void draw_rect(glm::ivec2 pos, glm::ivec2 size, Color c);

	void draw_line(glm::ivec2 start, glm::ivec2 end, Color c);

	glm::ivec2 get_text_size(std::string message, float size = 1.0f);
	void draw_text(std::string message, glm::vec2 pos, Color c, bool center = false, float size = 1.0f);

	// TODO: make it private!
	SDL_Renderer* m_renderer;
	float m_scale = 1.0f;

private:
	void set_color(Color c);

	bool m_initialized = false;

	Color m_background_color = {};


	TTF_Font* m_eight_bit_arcade_in;
	TTF_Font* m_eight_bit_arcade_out;
};

