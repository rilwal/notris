#pragma once

#include <vector>

#include "glm.hpp"
#include <SDL_ttf.h>


#include "config.hpp"
#include "color.hpp"
#include "piece.hpp"
#include "renderer.hpp"


struct Board {
public:
	Board();

	void reset();

	Color read_color(glm::ivec2 pos);
	void write_color(glm::ivec2 pos, Color c);

	void check_for_clears();

	void shuffle_bag(std::vector<Piece>& bag);

	void settle_piece();
	void next_piece();
	void hard_drop();

	void dim() { dimmed = true; };
	void undim() { dimmed = false; };


	bool tile_position_ok(glm::ivec2 position);
	bool piece_position_ok(const Piece& p, glm::ivec2 position);

	bool move_active_piece(glm::ivec2 vector);
	bool rotate_active_piece(bool clockwise);

	void update(float delta_time);

	void draw(Renderer& renderer);

	int get_score() { return score; };

	void display_message(std::string message, float time);

	float get_drop_speed();
private:

	void update_play(float delta_time);
	void update_line_clear_animation(float delta_time);
	void update_game_over(float delta_time);

	void draw_play(Renderer& renderer);
	void draw_line_clear_animation(Renderer& renderer);
	void draw_pieces(Renderer& renderer, float lightness);
	void draw_game_over(Renderer& renderer);


	Color board_data[c_board_width * c_board_height];

	glm::ivec2 active_piece_position = {};
	Piece active_piece = {};
	bool is_soft_dropping = false;
	float time_since_drop = 0;

	std::vector<Piece> bag;
	std::vector<Piece> next_bag;

	bool t_spin = false;
	bool back_to_back = false;

	bool dimmed = false;

	bool can_swap = true;
	Piece hold = NONE;

	int bag_idx = 0;

	int score = 0;
	int level = 1;

	int rows_to_fall[c_board_height];

	int lines_cleared = 0;

	float lca_time = 10.0;

	std::string message;
	float message_time;
	//float time_in_lca = 0.0f;

	enum {
		GAMEPLAY,
		LINE_CLEAR_ANIMATION,
		GAME_OVER
	} state = GAMEPLAY;

};
