
#include "renderer.hpp"

Renderer::~Renderer() {
	if (m_initialized)
		shutdown();
}

void Renderer::init(SDL_Window* window) {
	m_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	m_initialized = true;

	m_eight_bit_arcade_in = TTF_OpenFont("assets/8-bit Arcade In.ttf", 64);
	m_eight_bit_arcade_out = TTF_OpenFont("assets/8-bit Arcade Out.ttf", 64);
}

void Renderer::update() {
	SDL_RenderPresent(m_renderer);
	set_color(m_background_color);
	SDL_RenderClear(m_renderer);
}

void Renderer::shutdown() {
	SDL_DestroyRenderer(m_renderer);

	if (m_eight_bit_arcade_in) TTF_CloseFont(m_eight_bit_arcade_in);
	if (m_eight_bit_arcade_out) TTF_CloseFont(m_eight_bit_arcade_out);
}

void Renderer::draw_rect(Rect r, Color c) {
	FRect float_rect = r.fp();
	float_rect.pos *= m_scale;
	float_rect.size *= m_scale;

	set_color(c);
	SDL_RenderFillRectF(m_renderer, float_rect.sdl());
}


void Renderer::draw_rect(glm::ivec2 pos, glm::ivec2 size, Color c) {
	draw_rect(Rect(pos, size), c);
}

void Renderer::draw_line(glm::ivec2 start, glm::ivec2 end, Color c) {
	SDL_RenderDrawLine(m_renderer, start.x, start.y, end.x, end.y);

}


void Renderer::set_color(Color c) {
	SDL_SetRenderDrawColor(m_renderer, c.r, c.g, c.b, c.a);
}



glm::ivec2 Renderer::get_text_size(std::string message, float size) {
	glm::ivec2 ret = {};
	TTF_SizeText(m_eight_bit_arcade_in, message.c_str(), &ret.x, &ret.y);
	ret = (glm::ivec2)((glm::vec2)ret * size);
	return ret;
}


std::map<std::string, SDL_Texture*> text_render_cache;


void Renderer::draw_text(std::string message, glm::vec2 pos, Color c, bool center, float size) {
	if (!text_render_cache.contains(message)) {
		SDL_Surface* text_surface = TTF_RenderText_Blended(m_eight_bit_arcade_in, message.c_str(), c.sdl());
		SDL_Texture* text_texture = SDL_CreateTextureFromSurface(m_renderer, text_surface);
		SDL_FreeSurface(text_surface);

		text_render_cache[message] = text_texture;
	}
		
	SDL_Texture* text_texture = text_render_cache[message];

	glm::ivec2 text_size = {};
	SDL_QueryTexture(text_texture, nullptr, nullptr, &text_size.x, &text_size.y);

	text_size = (glm::vec2)text_size * size;
	if (center) pos.x -= text_size.x / 2;

	
	Rect text_rect(pos, text_size);
	FRect float_rect = text_rect.fp();

	float_rect.pos *= m_scale;
	float_rect.size *= m_scale;
	
	SDL_RenderCopyF(m_renderer, text_texture, nullptr, float_rect.sdl());
}