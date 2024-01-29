
#include "board.hpp"

#include <random>
#include <algorithm>

#include "settings.hpp"
#include "piece.hpp"
#include "util.hpp"
#include "asset_manager.hpp"
#include "audio.hpp"

Board::Board() {
	reset();
}

void Board::reset() {
	for (int y = 0; y < c_board_height; y++) {
		for (int x = 0; x < c_board_width; x++) {
			board_data[y * c_board_width + x] = c_nothing;
		}
	}

	hold = NONE;
	can_swap = true;

	t_spin = false;
	back_to_back = false;
	score = 0;
	level = 1;
	message = "";
	message_time = 0;
	lca_time = 0;
	bag_idx = 0;


	bag = { I, O, L, J, S, Z, T };
	next_bag = { I, O, L, J, S, Z, T };

	shuffle_bag(bag);
	shuffle_bag(next_bag);

	next_piece();


	state = GAMEPLAY;
}

Color Board::read_color(glm::ivec2 pos) {
	if (pos.x < 0 || pos.y < 0 || pos.x >= c_board_width || pos.y >= c_board_height) return c_nothing;

	return board_data[pos.x + pos.y * c_board_width];
}


void Board::write_color(glm::ivec2 pos, Color c) {
	if (pos.x < 0 || pos.y < 0 || pos.x >= c_board_width || pos.y >= c_board_height) return;
	board_data[pos.x + pos.y * c_board_width] = c;
}


void Board::check_for_clears() {
	int clears = 0;
	bool is_perfect_clear = true;

	for (int y = c_board_height - 1; y >= 0; y--) {
		//int real_y_coord = c_board_height - (y + 1);
		bool is_clear = true;
		bool empty = true;
		for (int x = 0; x < c_board_width; x++) {
			
			if (read_color({ x, y }) == c_nothing) {
				is_clear = false;
			}
			else {
				empty = false;
			}
		}

		if (!(is_clear || empty)) is_perfect_clear = false;

		if (is_clear) {
			rows_to_fall[y] = clears;

			clears++;
			for (int x = 0; x < c_board_width; x++) {
				write_color({ x, y }, c_nothing);
			}
		}
		else {
			rows_to_fall[y] = clears;
		}
	}


	if (clears > 0) {
		// let's calculate scoring here
		// Based on Puyo Puyo Tetris scoring
		// Info from: https://harddrop.com/wiki/Puyo_Puyo_Tetris#Scoring
		if (clears == 1) {
			// single
			if (t_spin) {
				score += 800;
			}
			else {
				score += 100;
			}

			if (is_perfect_clear) score += 800;
		}
		else if (clears == 2) {
			// double
			if (t_spin) {
				display_message("TSPIN", 2);
				score += 1200;

			}
			else {
				score += 300;
			}

			if (is_perfect_clear) score += 1000;

		}
		else if (clears == 3) {
			// triple

			if (t_spin) {
				if (back_to_back) {
					score += 1600 * 3 / 2;
					display_message("B2B TSPIN TRIPLE", 2);
				}
				else {
					score += 1600;
					display_message("TSPIN TRIPLE", 2);
				}
			}
			else {
				score += 500;
				display_message("Triple", 2);
			}

			if (is_perfect_clear) score += 1800;


		}
		else if (clears == 4) {
			// QUAD
			if (back_to_back) {
				// B2B QUAD
				score += 800 * 3 / 2;
				display_message("B2B QUAD", 2);
			}
			else {
				// Regular QUAD
				score += 800;
				display_message("QUAD", 2);
			}


			if (is_perfect_clear) score += 2000;
		}

		if (is_perfect_clear) {
			display_message("PERFECT", 2);
		}

		back_to_back = t_spin || clears == 4;

		lca_time = sm.get_config_setting<bool>("enable_fall_animation", true) ? calc_animation_time(clears) : 0;
		state = LINE_CLEAR_ANIMATION;

		lines_cleared += clears;

		if (lines_cleared > 20) {
			lines_cleared -= 20;
			level += 1;
		}
	}
}

void Board::shuffle_bag(std::vector<Piece>& bag) {
	auto rd = std::random_device{};
	auto rng = std::default_random_engine{ rd() };
	std::shuffle(std::begin(bag), std::end(bag), rng);
}



void Board::settle_piece() {
	for (int i = 0; i < 4; i++) {
		auto offset = active_piece_position + active_piece.offsets[i];

		write_color(offset, active_piece.color);

		if (offset.y < 0 && offset.x > 3 && offset.x < 7) state = GAME_OVER;
	}

	check_for_clears();

	time_since_drop = 0;
	next_piece();
}

void Board::next_piece() {
	can_swap = true;
	active_piece_position = glm::vec2(c_board_width / 2 - 1, -1);
	active_piece = bag[bag_idx++];

	if (!piece_position_ok(active_piece, active_piece_position)) {
		SDL_Quit();
	}

	if (bag_idx == bag.size()) {
		std::swap(bag, next_bag);
		shuffle_bag(next_bag);
		bag_idx = 0;
	}
}

void Board::hard_drop() {
	while (move_active_piece({ 0, 1 }));
	settle_piece();
}

bool Board::tile_position_ok(glm::ivec2 position) {
	if (position.x < 0 || position.x >= c_board_width || position.y < -2 || position.y >= c_board_height) {
		return false;
	}

	if (position.y < 0) return true;

	if (board_data[position.x + position.y * c_board_width] != c_nothing) return false;
	return true;
}

bool Board::piece_position_ok(const Piece& p, glm::ivec2 position) {
	for (int i = 0; i < 4; i++) {
		if (!tile_position_ok(position + p.offsets[i])) return false;
	}

	return true;
}

bool Board::move_active_piece(glm::ivec2 vector) {
	if (piece_position_ok(active_piece, active_piece_position + vector)) {
		active_piece_position += vector;
		t_spin = false;
		return true;
	}

	return false;
}

bool Board::rotate_active_piece(bool clockwise) {
	Piece test = active_piece;
	test.rotate(clockwise);

	for (int i = 0; i < 5; i++) {
		auto from_offset = active_piece.ot->offsets[active_piece.rotation_state][i] * glm::ivec2(1, -1);
		auto to_offset = test.ot->offsets[test.rotation_state][i] * glm::ivec2(1, -1);

		auto test_offset = from_offset - to_offset;

		if (piece_position_ok(test, active_piece_position + test_offset)) {
			active_piece = test;
			active_piece_position += test_offset;

			if (active_piece == T) {
				// Check for T-Spin
				int occupied = 0;

				auto check = [this](glm::ivec2 offset) { return (read_color(active_piece_position + offset) != c_nothing) ? 1 : 0; };

				occupied += check({ -1, -1 });
				occupied += check({ -1,  1 });
				occupied += check({ 1, -1 });
				occupied += check({ 1,  1 });

				if (occupied >= 3) {
					t_spin = true;
				}

			}
			return true;
		}
	}

	return false;
}


void Board::update(float delta_time) {
		switch (state) {
	case GAMEPLAY:
		update_play(delta_time);
		break;
	case LINE_CLEAR_ANIMATION:
		update_line_clear_animation(delta_time);
		break;
	case GAME_OVER:
		update_game_over(delta_time);
		break;
	}

	if (message_time > 0) {
		message_time -= delta_time;
	}
}

void Board::update_line_clear_animation(float delta_time) {
	lca_time -= delta_time;

	if (lca_time <= 0.0f) {	
		// once the animation is done, move all the pieces to their new positions internally
		// and go back to GAMEPLAY state
	
		for (int y = c_board_height - 1; y >= 0; y--) {
			int n = rows_to_fall[y];
			
			for (int x = 0; x < c_board_width; x++) {
				Color c = read_color({ x, y });
				write_color({ x,y }, c_nothing);

				write_color({ x,y+n }, c);
			}
		}

		state = GAMEPLAY;
	}
}

void Board::update_game_over(float delta_time) {
	if (InputManager::key_pressed(SDLK_r)) {
		reset();
	}
}


void Board::update_play(float delta_time) {
	static auto rotate_sound = asset_manager.GetByPath<Audio>("assets/sfx/lock.mp3");
	static auto lock_sound = asset_manager.GetByPath<Audio>("assets/sfx/click.mp3");
	static auto hard_drop_sound = asset_manager.GetByPath<Audio>("assets/sfx/hard_drop.mp3");

	const SDL_Keycode key_left = sm.get_config_setting<SDL_Keycode>("key_left", SDLK_a);
	const SDL_Keycode key_right = sm.get_config_setting<SDL_Keycode>("key_right", SDLK_d);

	const SDL_Keycode key_rotate_left = sm.get_config_setting<SDL_Keycode>("key_rotate_left", SDLK_j);
	const SDL_Keycode key_rotate_right = sm.get_config_setting<SDL_Keycode>("key_rotate_right", SDLK_l);

	const SDL_Keycode key_soft_drop = sm.get_config_setting<SDL_Keycode>("key_soft_drop", SDLK_s);
	const SDL_Keycode key_hard_drop = sm.get_config_setting<SDL_Keycode>("key_hard_drop", SDLK_w);

	const SDL_Keycode key_swap = sm.get_config_setting<SDL_Keycode>("key_swap", SDLK_i);


	time_since_drop += delta_time;

	static bool das_left = false;
	static float left_das_timer = 0.0f;

	static bool das_right = false;
	static float right_das_timer = 0.0f;

	if (InputManager::key_pressed(key_left)) {
		move_active_piece({ -1, 0 });
	}

	if (InputManager::key_down(key_left)) {
		left_das_timer += delta_time;

		if (das_left) {
			if (left_das_timer > das_repeat_delay) {
				left_das_timer -= das_repeat_delay;
				move_active_piece({ -1, 0 });
			}
		}
		else if (left_das_timer >= das_start_delay) {
			das_left = true;
			left_das_timer -= das_start_delay;
			move_active_piece({ -1, 0 });
		}
	}
	else {
		left_das_timer = 0;
		das_left = false;
	}

	if (InputManager::key_pressed(key_right)) {
		move_active_piece({ 1, 0 });
	}


	if (InputManager::key_down(key_right)) {
		right_das_timer += delta_time;

		if (das_right) {
			if (right_das_timer > das_repeat_delay) {
				right_das_timer -= das_repeat_delay;
				move_active_piece({ 1, 0 });
			}
		}
		else if (right_das_timer >= das_start_delay) {
			das_right = true;
			right_das_timer -= das_start_delay;
			move_active_piece({ 1, 0 });
		}
	}
	else {
		right_das_timer = 0;
		das_right = false;
	}


	if (InputManager::key_pressed(key_hard_drop)) {
		hard_drop();
		hard_drop_sound->play();

	}

	if (InputManager::key_pressed(key_rotate_left) || InputManager::key_pressed(SDLK_q)) {
		if (rotate_active_piece(true)) {
			time_since_drop = 0;
			rotate_sound->play();
		}
	}

	if (InputManager::key_pressed(key_rotate_right) || InputManager::key_pressed(SDLK_e)) {
		if (rotate_active_piece(false)) {
			time_since_drop = 0;
			rotate_sound->play();
		}
	}

	if (InputManager::key_pressed(key_swap)) {
		if (can_swap) {
			if (hold == NONE) {
				hold = active_piece;
				next_piece();
			}
			else {
				can_swap = false;
				std::swap(active_piece, hold);
				while (hold.rotation_state != 0) hold.rotate(true);
				active_piece_position = glm::vec2(c_board_width / 2 - 1, -1);
			}
		}
	}


	is_soft_dropping = InputManager::key_down(key_soft_drop);

	static float accum = 0;
	accum += delta_time;

	float threshold = get_drop_speed() * (is_soft_dropping ? 0.05f : 1);

	if (accum > threshold) {
		if (move_active_piece({ 0, 1 })) {
			time_since_drop = 0;
		}
		else {
			if (time_since_drop > .5) {
				settle_piece();
			}
		}
		accum = 0;
	}
}

float Board::get_drop_speed() {
	// 1 G is 1.000 / 60 (FPS) = 0.016666

	switch (level) {
	case 1:
		return 1.0f;
	case 2:
		return 0.793f;
	case 3:
		return 0.618f;
	case 4:
		return 0.473f;
	case 5:
		return 0.355f;
	case 6:
		return 0.262f;
	case 7:
		return 0.190f;
	case 8:
		return 0.135f;
	case 9:
		return 0.094f;
	case 10:
		return 0.064f;
	case 11:
		return 0.043f;
	case 12:
		return 0.028f;
	case 13:
		return 0.018f;
	case 14:
		return 0.011f;
	case 15:
		return 0.007f;
	}
	return 0.0016666f;

}

void Board::draw(SDL_Renderer* renderer) {
	// First, do some common drawing
	set_draw_color(renderer, c_board_bg);
	draw_rect(renderer, board_frame_pos, board_size);

	set_draw_color(renderer, c_black);
	draw_rect(renderer, board_pos, board_size - glm::ivec2(c_board_padding * 2));

	// render a message if we have one
	if (message_time > 0) {
		Color message_color = c_fg;
		if (message_time < 0.2) {
			// let's fade out the message
			message_color = lerp(message_color, c_bg, 1.0f - (message_time / 0.2f));
		}

		// Center in the right margin!
		glm::ivec2 position = board_frame_pos + board_size;
		position += glm::ivec2((c_window_size.x - position.x) / 2, 0);
		draw_text(renderer, message, position, message_color, true, 0.7f);
	}


	char level_buf[32];
	snprintf(level_buf, sizeof(level_buf), "LEVEL %d", level);
	draw_text(renderer, level_buf, { board_frame_pos.x / 2, board_size.y + board_frame_pos.y }, c_fg, true, 0.8f);


	// Then draw based on the state
	switch (state) {
		case GAMEPLAY:
			draw_play(renderer);
			return;
		case LINE_CLEAR_ANIMATION:
			draw_line_clear_animation(renderer);
			return;
		case GAME_OVER:
			draw_game_over(renderer);
	}
}

void Board::display_message(std::string m, float time) {
	message = m;
	message_time = time;
}

void Board::draw_line_clear_animation(SDL_Renderer * renderer) {
	set_draw_color(renderer, c_board_bg);
	for (int y = 0; y < c_board_height; y++) {
		for (int x = 0; x < c_board_width; x++) {
			draw_rect(renderer, board_pos + c_block_margin + glm::ivec2{ x, y } *block_stride, block_size);
		}
	}


	for (int y = c_board_height - 1; y >= 0; y--) {
		int n = rows_to_fall[y];

		if (n == 0) {
			for (int x = 0; x < c_board_width; x++) {
				Color c = board_data[y * c_board_width + x];
				if (c != c_nothing) {
					if (dimmed) {
						c.lighten(0.6f);
					}
					set_draw_color(renderer, c);
					draw_rect(renderer, board_pos + c_block_margin + glm::ivec2{ x, y } *block_stride, block_size);
				}
			}
		}

		float total_animation_time = n * lca_fall_time_per_line;
		float animation_progress = ease((total_animation_time - lca_time) / total_animation_time);
		int dist_to_fall = block_stride.y * n;
		
		int current_y = int(dist_to_fall * animation_progress);

		for (int x = 0; x < c_board_width; x++) {
			Color c = board_data[y * c_board_width + x];
			if (c != c_nothing) {

				if (dimmed) {
					c.lighten(0.6f);
				}

				set_draw_color(renderer, c_black);
				draw_rect(renderer, board_pos + c_block_margin + glm::ivec2{ x, y } * block_stride - glm::ivec2{ c_block_margin } + glm::ivec2(0, current_y), block_size + glm::ivec2{ c_block_margin } *2);

				set_draw_color(renderer, c);
				draw_rect(renderer, board_pos + c_block_margin + glm::ivec2{ x, y } * block_stride + glm::ivec2(0, current_y), block_size);
			}
		}
	}
}

void Board::draw_pieces(SDL_Renderer* renderer, float lightness) {
	for (int y = 0; y < c_board_height; y++) {
		for (int x = 0; x < c_board_width; x++) {
			Color c = board_data[y * c_board_width + x];
			if (c != c_nothing) {
				c.lighten(lightness);
				set_draw_color(renderer, c);
				draw_rect(renderer, board_pos + c_block_margin + glm::ivec2{ x, y } *block_stride, block_size);
			}
			else {
				set_draw_color(renderer, c_board_bg);
				draw_rect(renderer, board_pos + c_block_margin + glm::ivec2{ x, y } *block_stride, block_size);
			}
		}
	}
}

void Board::draw_game_over(SDL_Renderer* renderer) {

	draw_pieces(renderer, 0.6f);
	draw_text(renderer, "GAME OVER", board_frame_pos + board_size / 2 - glm::ivec2(0, c_block_size * 1.5), c_fg, true);

}


void Board::draw_play(SDL_Renderer* renderer) {


	draw_pieces(renderer, dimmed ? .6f : 1.0f);


	// Draw 5 upcoming pieces
	for (int i = 0; i < 5; i++) {
		int idx = bag_idx + i;
		Piece piece;
		if (idx >= bag.size()) {
			piece = next_bag[idx - bag.size()];
		}
		else {
			piece = bag[idx];
		}
		piece.draw(renderer, { 14 , i * 3 + 1 });
	}

	// Draw hold
	if (hold != NONE) {
		hold.draw(renderer, { -4, 2 });
	}



	// Show piece preview
	if (sm.get_config_setting<bool>("enable_ghost", true)) {

		float lighten_amount = .4f;
		Piece ghost = active_piece;
		ghost.color.lighten(lighten_amount);


		glm::ivec2 ghost_piece_pos = active_piece_position;

		while (piece_position_ok(ghost, ghost_piece_pos + glm::ivec2{0, 1})) {
			ghost_piece_pos.y++;
		}

		ghost.draw(renderer, ghost_piece_pos);
	}

	active_piece.draw(renderer, active_piece_position, { 0, 0 }, dimmed ? .6f : 1.1f);
}
