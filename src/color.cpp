#include "color.hpp"

Color get_draw_color(SDL_Renderer* renderer) {
	Color c = {};
	SDL_GetRenderDrawColor(renderer, &c.r, &c.g, &c.b, &c.a);
	return c;
}

void set_draw_color(SDL_Renderer* renderer, Color c) {
	SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
}
