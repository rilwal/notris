#pragma once


#include <map>
#include <string>

#include <glm.hpp>
#include <SDL.h>

#include "util.hpp"
#include "config.hpp"
#include "audio.hpp"
#include "ui.hpp"


template<typename T>
std::map<std::string, T> m_settings;



struct SettingsManager {
	void load_settings(std::string filename);
	void save_settings(std::string filename);

	template<typename T>
	T get_config_setting(std::string key) {
		if (m_settings<T>.contains(key))
			return m_settings<T>[key];

		return T();
	}


	template<typename T>
	T& get_config_setting(std::string key, T def) {
		if (m_settings<T>.contains(key))
			return m_settings<T>[key];

		m_settings<T>[key] = def;
		return m_settings<T>[key];
	}

	template<typename T>
	void set_config_setting(std::string key, T val) {
		m_settings<T>[key] = val;
	}
};


extern SettingsManager sm;




struct SettingsMenu : public IMenu {
	glm::ivec2 size = { 400, 600 };
	glm::ivec2 position = c_window_size / 2 - size / 2;
	glm::ivec2 m_content_size;
	std::list<std::shared_ptr<IControl>> controls;
	float m_scrollbar_timer = 0.0f;

	int m_scrollbar_drag_offset;
	bool m_dragging_scrollbar;

	SettingsMenu();

	void update(float delta_time);

	void draw(SDL_Renderer* renderer);
};

