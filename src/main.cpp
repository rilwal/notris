

#include <map>
#include <vector>
#include <algorithm>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include <glm.hpp>

#include "audio.hpp"
#include "color.hpp"
#include "util.hpp"
#include "piece.hpp"
#include "board.hpp"
#include "settings.hpp"
#include "renderer.hpp"



int WinMain(int hInstance, int hPrevInstance, char* pCmdLine, int nCmdShow) {
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	Mix_Init(MIX_INIT_MP3);
	   
	SDL_Window* window = SDL_CreateWindow("NOTRIS", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, c_window_width, c_window_height, SDL_WINDOW_RESIZABLE);
	//SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	Renderer r = {};

	r.init(window);

	SDL_Renderer* renderer = r.m_renderer;
	r.set_background_color(c_0);

	SDL_Event e = {};

	bool running = true;

	TTF_Font* eight_bit_arcade_in  = TTF_OpenFont("assets/8-bit Arcade In.ttf", 64);
	TTF_Font* eight_bit_arcade_out = TTF_OpenFont("assets/8-bit Arcade Out.ttf", 64);

	int device = Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 512);

	auto title_surface_out = TTF_RenderText_Blended(eight_bit_arcade_out, "NOTRIS", c_3.sdl());
	auto title_surface = TTF_RenderText_Blended(eight_bit_arcade_in, "NOTRIS", c_1.sdl());

	SDL_BlitSurface(title_surface_out, nullptr, title_surface, nullptr);

	auto title_texture = SDL_CreateTextureFromSurface(renderer, title_surface);
	SDL_FreeSurface(title_surface_out);
	SDL_FreeSurface(title_surface);


	Board board;
	SettingsMenu menu(r);

	bool settings_open = false;

	uint32_t last_time = 0;
	uint32_t time = SDL_GetTicks();
	float delta_time = 0;

	glm::ivec2 window_size = glm::ivec2(c_window_width, c_window_height);


	auto main_bgm = asset_manager.GetByPath<Audio>("assets/bgm/main.mp3");
	main_bgm->play(Audio::Channel::BGM, -1);

	sm.load_settings("settings.ini");

	while (running) {
		last_time = time;
		time = SDL_GetTicks();
		delta_time = (float)(time - last_time) / 1000.f;
		
		InputManager::update(delta_time);

		while (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:
				running = false;
				break;
			case SDL_KEYDOWN:
			case SDL_KEYUP:
			case SDL_MOUSEWHEEL:
				InputManager::handle_input_event(e);
				break;

			case SDL_WINDOWEVENT:
				if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
					// If the window is resized:
					//	* first, find the smaller of the two dimensions, and use it for our size
					
					// Or, figure out which size was changed!

					int x = e.window.data1;
					int y = e.window.data2;
					
					if (x != window_size.x) {
						int new_y = x * c_window_height / c_window_width;
						SDL_SetWindowSize(window, x, new_y);

						r.m_scale = (float)x / c_window_width;
					}
					else {
						int new_x = y * c_window_width / c_window_height;
						SDL_SetWindowSize(window, new_x, y);
						r.m_scale = (float)y / c_window_width;

					}

					window_size = { x, y };

					//Rect r = { x / 2 - new_size / 2, y / 2 - new_size / 2, new_size, new_size };
					//SDL_RenderSetViewport(renderer, r.sdl());

				}
			}
		}


		Mix_Volume((int)Audio::Channel::BGM, (int)(MIX_MAX_VOLUME * sm.get_config_setting<float>("bgm_volume", 0.5) * sm.get_config_setting<float>("volume", 1.0)));
		Mix_Volume((int)Audio::Channel::SFX, (int)(MIX_MAX_VOLUME * sm.get_config_setting<float>("sfx_volume", 0.5) * sm.get_config_setting<float>("volume", 1.0)));

		
		if (InputManager::key_pressed(SDLK_ESCAPE)) {
			if (settings_open) {
				board.undim();
				settings_open = false;
			}
			else {
				board.dim();
				settings_open = true;
			}
		}

		if (settings_open) {
			menu.update(delta_time);
		}
		else {
			board.update(delta_time);
		}


		glm::ivec2 title_size = {};
		SDL_QueryTexture(title_texture, nullptr, nullptr, &title_size.x, &title_size.y);
		title_size *= 3;

		// Not sure what the 1.5 is about
		glm::ivec2 position = glm::ivec2{ c_window_width / 2 , c_board_margin.y / 2 } - glm::ivec2(title_size.x / 2, title_size.y / 1.5); 

		auto rect = make_rect(position, title_size);
		SDL_RenderCopy(renderer, title_texture, nullptr, &rect);


		char score_buff[20];
		snprintf(score_buff, sizeof(score_buff), "SCORE %d", board.get_score());

		glm::ivec2 score_position = glm::ivec2{ c_window_width / 2, board_frame_pos.y + board_size.y };

		r.draw_text(score_buff, score_position, c_fg, true);

		board.draw(r);

		if (settings_open) {
			menu.draw(r);
		}


		r.update();
	}

	TTF_CloseFont(eight_bit_arcade_in);
	TTF_CloseFont(eight_bit_arcade_out);

	SDL_DestroyWindow(window);

	return 0;
}