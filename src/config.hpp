#pragma once

#include <glm.hpp>

constexpr int c_window_width = 900;
constexpr int c_window_height = 900;

const glm::ivec2 c_window_size = { c_window_width, c_window_height };

// Size of a block in pixels
constexpr int c_block_size = 32;
constexpr int c_board_width = 10;
constexpr int c_board_height = 20;
constexpr int c_block_margin = 1;
constexpr int c_board_padding = 16;


constexpr bool c_show_preview = true;

constexpr float das_start_delay = 0.15f;
constexpr float das_repeat_delay = 0.05f;

constexpr float lca_fall_time_per_line = .1f;

const glm::ivec2 c_board_margin = glm::ivec2(256, 128);

constexpr int width = (c_block_size + c_block_margin) * c_board_width + c_block_margin + c_board_padding * 2;
constexpr int height = (c_block_size + c_block_margin) * c_board_height + c_block_margin + c_board_padding * 2;

const glm::ivec2 board_frame_pos = glm::ivec2(c_board_margin);
const glm::ivec2 board_pos = glm::ivec2(c_board_margin + c_board_padding);

const glm::ivec2 board_size = glm::ivec2(width, height);

const glm::ivec2 block_size = glm::ivec2(c_block_size);
const glm::ivec2 block_stride = glm::ivec2(c_block_size + c_block_margin);


inline float ease(float x) {
	// square easing for smooth drop animation (approximate gravity based drop)
	return x * x;
}

inline float calc_animation_time(int t) {
	// in order to give a more "physics" feeling,
	// let's time the animation according to kinematics
	return sqrt(2.f * t / 200.f);
}