
#include "settings.hpp"

#include <cstdio>

#include "parser.hpp"

SettingsManager sm;

void SettingsManager::save_settings(std::string filename) {
	FILE* settings_file;
	fopen_s(&settings_file, filename.c_str(), "w");
	
	if (!settings_file) {
		fprintf(stderr, "Failed to save settings file!\n");
		return;
	}

	for (auto& [name, setting] : m_settings<bool>) {
		fprintf(settings_file, "bool %s = %s\n", name.c_str(), setting ? "true" : "false");
	}

	for (auto& [name, setting] : m_settings<float>) {
		fprintf(settings_file, "float %s = %f\n", name.c_str(), setting);
	}

	for (auto& [name, setting] : m_settings<SDL_Keycode>) {
		fprintf(settings_file, "key %s = %d\n", name.c_str(), setting);
	}

	fclose(settings_file);
}

void SettingsManager::load_settings(std::string filename) {
	std::vector<uint8_t> data = load_file(filename);
	
	const char* it = (const char*)data.data();

	while (it && *it) {
		// settigns file format: {type} {name} = {val}
		skip_whitespace(it);
		auto type = consume_next_token(it);
		skip_whitespace(it);
		auto name = consume_next_token(it);
		skip_whitespace(it);
		consume_token(it, "=");
		skip_whitespace(it);

		if (type == "bool") {
			if (consume_next_token(it) == "true") {
				set_config_setting(name, true);
			}
			else {
				set_config_setting(name, false);
			}
		}
		else if (type == "float") {
			set_config_setting(name, consume_float(it));
		}
		else if (type == "int") {
			set_config_setting(name, consume_int(it));
		}
		else if (type == "key") {
			set_config_setting<SDL_Keycode>(name, consume_int(it));
		}

		skip_to_newline(it);
	}

}

SettingsMenu::SettingsMenu() {
	int title_height = get_text_size("Settings", 1.4f).y;

	m_border_box = { position, size };
	m_content_box = { position + glm::ivec2(c_board_padding) + glm::ivec2(0, title_height), size - glm::ivec2(c_board_padding * 2) - glm::ivec2(0, title_height)};
	m_dragging_scrollbar = false;
	m_scrollbar_drag_offset = 0;

	controls = {
		std::make_shared<TextControl>("General", 0.8f),
		std::make_shared<Checkbox>("enable_ghost", "Enable Ghost Piece", true),
		std::make_shared<Checkbox>("enable_fall_animation", "Block Fall Animation", true),

		std::make_shared<Seperator>(),

		std::make_shared<TextControl>("Audio", 0.8f),
		std::make_shared<VolumeControl>("volume",	  "Master Volume", "assets/sfx/lock.mp3"),
		std::make_shared<VolumeControl>("bgm_volume", "BGM Volume", "assets/sfx/lock.mp3"),
		std::make_shared<VolumeControl>("sfx_volume", "SFX Volume", "assets/sfx/lock.mp3"),


		std::make_shared<Seperator>(),

		std::make_shared<TextControl>("Controls", 0.8f),

		std::make_shared<KeySetting>("key_left", "Left"),
		std::make_shared<KeySetting>("key_right", "Right"),
		std::make_shared<KeySetting>("key_rotate_left", "Rotate Left"),
		std::make_shared<KeySetting>("key_rotate_right", "Rotate Right"),
		std::make_shared<KeySetting>("key_soft_drop", "Soft Drop"),
		std::make_shared<KeySetting>("key_swap", "Hold"),
	};

	// Flow controls
	glm::ivec2 cursor = glm::ivec2(0);

	for (auto& control : controls) {
		control->set_parent(this);
		control->set_pos(cursor);
		cursor.y += control->get_size().y + 16;
	}


	m_content_size = { m_content_box.w, cursor.y };
}

void SettingsMenu::update(float delta_time) {
	constexpr int c_scroll_speed = 5;

	for (auto& control : controls) {
		control->update();
	}

	int old_scroll = m_scroll;
	m_scroll = std::clamp(m_scroll - InputManager::get_scroll_offset().y * c_scroll_speed, 0, m_content_size.y - m_content_box.h);

	if (old_scroll != m_scroll) {
		// we scrolled, so show the scrollbar!
		m_scrollbar_timer = 2.0f;
	}

	if (m_scrollbar_timer >= 0)
		m_scrollbar_timer -= delta_time;

	sm.save_settings("settings.ini");
}


void SettingsMenu::draw(SDL_Renderer* renderer) {
	static SDL_Texture* settings_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, m_content_size.x, m_content_size.y);

	set_draw_color(renderer, c_fg);
	draw_rect(renderer, position, size);

	// Title
	draw_text(renderer, "Settings", m_border_box.pos + glm::ivec2(m_border_box.w / 2, 0), c_bg, true, 1.4f);

	// Scrollbar
	Rect scroll_area = Rect(m_content_box.x + m_content_box.w - 8, m_content_box.y - 15, 30, m_content_box.h + 30);

	float scroll_ratio = (float)m_content_box.y / m_content_size.y;

	if (scroll_area.contains(InputManager::get_mouse_pos()) || m_dragging_scrollbar) m_scrollbar_timer = 2;

	bool show_scrollbar = scroll_ratio < 1.0f && m_scrollbar_timer >= 0;

	if (show_scrollbar) {
		auto mouse_pos = InputManager::get_mouse_pos();

		int scrollbar_len = (int)(scroll_ratio * m_content_size.y);
		int scrollbar_pos = m_scroll;

		Rect scrollbar_rect = Rect({ m_content_box.x + m_content_box.w + 7, m_content_box.y + scrollbar_pos, 3, scrollbar_len });
		
		if(InputManager::mouse_down(1) && m_dragging_scrollbar) {
			m_scroll = mouse_pos.y - m_content_box.y - m_scrollbar_drag_offset;
		}
		else if (InputManager::mouse_down(1) && scrollbar_rect.expand(10).contains(mouse_pos)) {
			m_dragging_scrollbar = true;
		}
		else if (InputManager::mouse_down(1) && scroll_area.contains(mouse_pos)) {
			// Jump the scrollbar to the mouse
			if (mouse_pos.y < scrollbar_rect.y) {
				// jump up!
				m_scroll = mouse_pos.y - m_content_box.y;
				m_scrollbar_drag_offset = 20;
			}
			else {
				m_scroll = mouse_pos.y - m_content_box.y + scrollbar_len;
				m_scrollbar_drag_offset = scrollbar_len - 20;

			}
			m_dragging_scrollbar = true;
		}
		else {
			m_dragging_scrollbar = false;
			m_scrollbar_drag_offset = mouse_pos.y - scrollbar_rect.y;
		}

		m_scroll = std::clamp(m_scroll, 0, m_content_size.y - m_content_box.h);


		Color scrollbar_color = lerp(c_bg, c_fg, std::clamp(1.f - (m_scrollbar_timer * 10), 0.f, 1.f));

		set_draw_color(renderer, scrollbar_color);
		draw_rect(renderer, scrollbar_rect);
	}

	SDL_SetRenderTarget(renderer, settings_texture);
	set_draw_color(renderer, c_fg);
	SDL_RenderClear(renderer);

	for (auto& control : controls) {
		control->draw(renderer);
	}

	SDL_SetRenderTarget(renderer, NULL);
	Rect blit_section = { 0, m_scroll, m_content_box.w, m_content_box.h};
	SDL_RenderCopy(renderer, settings_texture, blit_section.sdl(), m_content_box.sdl());
}

