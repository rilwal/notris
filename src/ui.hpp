#pragma once


#include <sdl.h>
#include <glm.hpp>

#include "util.hpp"
#include "audio.hpp"

// UI stuff

class IMenu {
public:
	Rect get_content_box() const {
		return m_content_box;
	}

	Rect get_border_box() const {
		return m_border_box;
	}

	// Gets mouse position relative to box
	// For use by children
	glm::ivec2 get_mouse_pos() const {
		return InputManager::get_mouse_pos() - m_content_box.pos + glm::ivec2(0, m_scroll);
	}

protected:
	Rect m_border_box;
	Rect m_content_box;

	int m_scroll = 0;

};


class IControl {
public:
	virtual void update() = 0;
	virtual void draw(SDL_Renderer* renderer) = 0;

	glm::ivec2 get_pos() const { return m_rect.pos; }
	virtual void set_pos(glm::ivec2 pos) { m_rect.pos = pos; }

	glm::ivec2 get_size() const { return m_rect.size; }

	Rect get_bounding_box() const {
		return m_rect;
	}

	void set_parent(IMenu* parent) {
		m_parent = parent;
	}

protected:
	IMenu* m_parent = nullptr;
	Rect m_rect = {};
	std::string m_label = "";
	std::string m_config_key = "";
};


class Seperator : public IControl {
public:
	Seperator();

	void update() override;
	void draw(SDL_Renderer* renderer) override;
};



class TextControl : public IControl {
public:

	TextControl(std::string text, float size = 1.0f);;

	void update() override;
	void draw(SDL_Renderer* renderer) override;

private:
	float m_size;
	std::string m_text;
};

struct KeySetting : public IControl {

	KeySetting(std::string config_key, std::string label);

	void update() override;
	void draw(SDL_Renderer* renderer) override;

	bool is_active() const;

	void set_pos(glm::ivec2 position) override;

private:
	Rect m_key_edit_rect;
	bool m_active;
};


struct Checkbox : public IControl {

	Checkbox(std::string config_key, std::string label, bool def);

	void update() override;
	void draw(SDL_Renderer* renderer) override;


	void set_pos(glm::ivec2 position) override;

private:
	Rect m_checkbox_rect;
	bool m_def;
};


struct VolumeControl : public IControl {
public:
	VolumeControl(std::string config_key, std::string label, std::string sample="");

	void update() override;
	void draw(SDL_Renderer* renderer) override;

	void set_pos(glm::ivec2 position) override;

private:
	Rect m_slider_rect = {};
	Rect m_skip_area = {};

	glm::ivec2 m_line_start = {};
	glm::ivec2 m_line_end = {};
	bool m_dragging = false;

	Ref<Audio> m_sample;

};