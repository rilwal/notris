#include "piece.hpp"

OffsetTable standard_offsets = {
	{
		{{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}}, // Standard
		{{ 0, 0}, { 1, 0}, { 1,-1}, { 0, 2}, { 1, 2}}, // Right
		{{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}}, // Flipped
		{{ 0, 0}, {-1, 0}, {-1,-1}, { 0, 2}, {-1, 2}}, // Left
	},
};

OffsetTable i_offsets = {
	{
		{{ 0, 0}, {-1, 0}, { 2, 0}, {-1, 0}, { 2, 0}}, // Standard
		{{-1, 0}, { 0, 0}, { 0, 0}, { 0, 1}, { 0,-2}}, // Right
		{{-1, 1}, { 1, 1}, {-2, 1}, { 1, 0}, {-2, 0}}, // Flipped
		{{ 0, 1}, { 0, 1}, { 0, 1}, { 0,-1}, { 0, 2}}, // Left
	},
};

OffsetTable o_offsets = {
	{
		{{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}}, // Standard
		{{ 0,-1}, { 0,-1}, { 0,-1}, { 0,-1}, { 0,-1}}, // Right
		{{-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}}, // Flipped
		{{-1, 0}, {-1, 0}, {-1, 0}, {-1, 0}, {-1, 0}}, // Left
	},
};

glm::ivec2 rotate_offset(glm::ivec2 offset, int rotation) {
	// rotates piece offsets around the center
	switch (rotation) {
	case 0:
		return offset;
	case 1:
		return glm::ivec2(-offset.y, offset.x);
	case 2:
		return glm::ivec2(-offset.x, -offset.y);
	case 3:
		return glm::ivec2(offset.y, -offset.x);
	}

	return offset;
}

void Piece::rotate(bool clockwise) {
	for (int i = 0; i < 4; i++) {
		auto offset = offsets[i];
		offsets[i] = clockwise ? glm::ivec2(-offset.y, offset.x) : glm::ivec2(offset.y, -offset.x);
	}

	rotation_state = (rotation_state + (clockwise ? 1 : 3)) % 4;
}

void Piece::draw(SDL_Renderer* renderer, glm::ivec2 pos, glm::ivec2 offset, float brightness) {
	Color backup = get_draw_color(renderer);
	Color copy = color;
	copy.lighten(brightness);
	set_draw_color(renderer, copy);
	for (int i = 0; i < 4; i++) {
		if (pos.y + offsets[i].y < 0) continue; // don't render off the top
		Rect r(board_pos + c_block_margin + pos * block_stride + (c_block_size + c_block_margin) * offsets[i] + offset, glm::ivec2(c_block_size));
		draw_rect(renderer, r);
	}
	set_draw_color(renderer, backup);
}
