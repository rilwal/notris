
#include "ui.hpp"

#include "settings.hpp"
#include "config.hpp"

Seperator::Seperator() {
	m_rect.w = 400 - c_board_padding * 2;
	m_rect.h = 32;
}

void Seperator::update() {

}

void Seperator::draw(SDL_Renderer* renderer) {
	set_draw_color(renderer, c_bg);
	SDL_RenderDrawLine(renderer, m_rect.x, m_rect.y + 16, m_rect.x + 400 - c_board_padding * 2, m_rect.y + 16);
}

TextControl::TextControl(std::string text, float size) : m_text(text) {
	m_rect.size = get_text_size(text, size);
	m_size = size;
}

void TextControl::update() {

}

void TextControl::draw(SDL_Renderer* renderer) {
	draw_text(renderer, m_text, m_rect.pos, c_8, false, m_size);
}


KeySetting::KeySetting(std::string config_key, std::string label) {
	m_config_key = config_key;
	m_label = label;
	m_active = false;

	m_rect.size = { 400 - c_board_padding * 2, 32 };
	m_key_edit_rect.size = { 32, 32 };
}

void KeySetting::update() {
	if (InputManager::mouse_pressed(1) && m_key_edit_rect.contains(m_parent->get_mouse_pos())) {
		m_active = true;
		InputManager::get_next_key([this](SDL_Keycode k) { if (m_active) { if (k != SDLK_ESCAPE) sm.set_config_setting(m_config_key, k); m_active = false; }},
			[this]() {m_active = false; });
	}
}

void KeySetting::draw(SDL_Renderer* renderer) {
	draw_text(renderer, m_label, m_rect.pos, c_bg, false, 0.5);

	set_draw_color(renderer, c_white);
	draw_rect(renderer, m_key_edit_rect);

	if (!m_active) {
		char str[2] = { static_cast<char>(sm.get_config_setting<SDL_Keycode>(m_config_key)), 0 };
		draw_text(renderer, str, m_key_edit_rect.pos + glm::ivec2{ 2, -14 }, c_0);
	}
}

bool KeySetting::is_active() const {
	return m_active;
}

void KeySetting::set_pos(glm::ivec2 position) {
	m_rect.pos = position;
	m_key_edit_rect.pos = { position.x + m_rect.size.x - m_key_edit_rect.size.x, position.y };
}

Checkbox::Checkbox(std::string config_key, std::string label, bool def) {
	m_config_key = config_key;
	m_def = def;
	m_label = label;

	m_rect.size = { 400 - c_board_padding * 2, 32 };
	m_checkbox_rect.size = { 32, 32 };
}

void Checkbox::update() {
	if (InputManager::mouse_pressed(1) && m_checkbox_rect.contains(m_parent->get_mouse_pos())) {
		auto& val = sm.get_config_setting(m_config_key, m_def);
		val = !val;
	}
}

void Checkbox::draw(SDL_Renderer* renderer) {
	//set_draw_color(renderer, c_1);
	//draw_rect(renderer, m_rect);

	draw_text(renderer, m_label, m_rect.pos, c_bg, false, 0.5);

	set_draw_color(renderer, c_white);
	draw_rect(renderer, m_checkbox_rect);

	if (sm.get_config_setting(m_config_key, m_def)) {
		set_draw_color(renderer, c_0);
		draw_rect(renderer, m_checkbox_rect.pos + glm::ivec2(2), m_checkbox_rect.size - glm::ivec2(4));
	}


}

void Checkbox::set_pos(glm::ivec2 position) {
	m_rect.pos = position;
	m_checkbox_rect.pos = { position.x + m_rect.size.x - m_checkbox_rect.size.x, position.y };
}

VolumeControl::VolumeControl(std::string config_key, std::string label, std::string sample) {
	m_config_key = config_key;
	m_label = label;
	m_sample = asset_manager.GetByPath<Audio>(sample);
}


void VolumeControl::update() {
	auto mouse_pos = m_parent->get_mouse_pos();
	auto mouse_down = InputManager::mouse_down(1);

	if (mouse_down && m_slider_rect.contains(mouse_pos)) {
		m_dragging = true;
	}
	else if (InputManager::mouse_pressed(1) && m_skip_area.contains(mouse_pos)) {
		float pos = (float)(mouse_pos.x - m_line_start.x);
		float slider_len = (float)(m_line_end.x - m_line_start.x);
		float val = pos / slider_len;
		sm.get_config_setting(m_config_key, 1.0f) = std::clamp(val, 0.0f, 1.0f);

		m_dragging = true;
	}

	if (m_dragging && mouse_down) {
		float pos = float(mouse_pos.x - m_line_start.x);
		float slider_len = float(m_line_end.x - m_line_start.x);
		float val = pos / slider_len;
		sm.get_config_setting(m_config_key, 1.0f) = std::clamp(val, 0.0f, 1.0f);
	}
	else if (m_dragging) {
		m_sample->play(Audio::Channel::SFX);
		m_dragging = false;
	}

}

void VolumeControl::draw(SDL_Renderer* renderer) {
	set_draw_color(renderer, c_bg);

	draw_text(renderer, m_label, m_rect.pos, c_bg, false, 0.5);

	int line_len = m_line_end.x - m_line_start.x;

	SDL_RenderDrawLine(renderer, m_line_start.x, m_line_start.y, m_line_end.x, m_line_end.y);

	float& vol = sm.get_config_setting<float>(m_config_key, .5);
	glm::ivec2 slider_pos = m_line_start + glm::ivec2(line_len * vol, 0);
	glm::ivec2 slider_size = { 10, 20 };

	m_slider_rect = { slider_pos - slider_size / 2, slider_size };

	draw_rect(renderer, m_slider_rect);
}

void VolumeControl::set_pos(glm::ivec2 position) {
	m_rect.size = { m_parent->get_content_box().w, 32 };
	m_rect.pos = position;

	int label_width = get_text_size(m_label, 0.5f).x;

	int line_y = m_rect.y + m_rect.h / 2;

	m_line_start = { m_rect.x + label_width + c_board_padding, line_y };
	m_line_end = { m_rect.x + m_rect.w - 10, line_y }; // 10 pixels off for the slider

	m_skip_area = { m_line_start.x, m_line_start.y - 5, m_line_end.x - m_line_start.x, 10 };
}

