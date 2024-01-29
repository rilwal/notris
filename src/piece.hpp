#pragma once

#include <glm.hpp>

#include "config.hpp"
#include "util.hpp"
#include "color.hpp"

enum RotationState {
	Standard,
	Right,
	Flipped,
	Left
};


struct OffsetTable {
	glm::ivec2 offsets[4][5];
};


extern OffsetTable standard_offsets;
extern OffsetTable i_offsets;
extern OffsetTable o_offsets;

struct Piece {
	// offsets for the four blocks
	glm::ivec2 offsets[4] = {};
	Color color = {};
	OffsetTable* ot = &standard_offsets;
	int rotation_state = 0;

	// very hacky, but works for our purposes
	bool operator==(const Piece& other) {
		return color == other.color;
	}

	// pos is the position in board coordinates,
	// offset is an offset in pixels for use in animations etc.
	void draw(SDL_Renderer* renderer, glm::ivec2 pos, glm::ivec2 offset={0,0}, float brightness=1.0);
	

	// Rotates a piece without regard to it's surrounding board
	// This function produces "true" rotations, so without offsets
	// it will not always create a sensible result
	void rotate(bool clockwise);

};

const Piece NONE = {};
const Piece I = { {{-1, 0}, { 0, 0 }, { 1, 0 }, { 2, 0 }}, c_12, &i_offsets };
const Piece O = { {{0, 0}, { 0, -1 }, { 1, -1 }, { 1, 0 }}, c_11, &o_offsets };
const Piece L = { {{-1, 0}, { 0, 0 }, { 1, 0 }, { 1, -1 }}, c_3 };
const Piece J = { {{-1, -1}, { -1, 0 }, { 0, 0 }, { 1, 0 }}, c_14 };
const Piece T = { {{-1, 0}, { 0, 0 }, { 1, 0 }, { 0, -1 }}, c_13 };
const Piece S = { { {-1, 0}, { 0, 0 }, { 0, -1 }, { 1, -1 }}, c_10 };
const Piece Z = { {{-1, -1}, { 0, -1 }, { 0, 0 }, { 1, 0 }}, c_9 };



glm::ivec2 rotate_offset(glm::ivec2 offset, int rotation);


