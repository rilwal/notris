#pragma once

#include "SDL.h"

#include <map>
#include <string>
#include <functional>
#include <optional>
#include <memory>

#include <glm.hpp>
#include "SDL_ttf.h"


#include "color.hpp"


// References!
template<typename T>
using Ref = std::shared_ptr<T>;

template<typename T>
Ref<T> make_ref() {
	return std::make_shared<T>();
}


template<typename T>
using WeakRef = std::weak_ptr<T>;

template<typename T>
WeakRef<T> make_weak_ref(Ref<T>& a) {
	return std::weak_ptr<T>(a);
}

template<typename T>
using Opt = std::optional<T>;



std::vector<uint8_t> load_file(std::string filename, size_t offset = 0, size_t len = -1);
void save_file(std::string filename, const std::vector<uint8_t> data);
void save_file(std::string filename, const uint8_t* data, size_t len);


class InputManager {
public:

	static void update(float delta_time);

	static void handle_input_event(SDL_Event& e);

	static bool key_down(SDL_Keycode ks);
	static bool key_up(SDL_Keycode ks);
	static bool key_pressed(SDL_Keycode ks);

	static glm::ivec2 get_mouse_pos();

	static void get_next_key(std::function<void(SDL_Keycode)> callback, std::function<void(void)> fail_callback=nullptr);


	static bool mouse_pressed(int btn) {
		return SDL_BUTTON(btn) & m_mouse_state & ~m_previous_mouse_state;
	}

	static bool mouse_down(int btn) {
		return SDL_BUTTON(btn) & m_mouse_state;
	}

	static glm::ivec2 get_scroll_offset() {
		return m_scroll;
	}

private:
	static std::map<SDL_Keycode, bool> m_key_state;
	static std::map<SDL_Keycode, bool> m_last_key_state;

	static glm::ivec2 m_mouse_pos;
	static glm::ivec2 m_scroll;

	static uint32_t m_mouse_state;
	static uint32_t m_previous_mouse_state;

	static std::function<void(SDL_Keycode)> m_next_key_callback;
	static std::function<void(void)> m_next_key_fail_callback;
};


struct FRect {
	union {
		struct {
			float x, y, w, h;
		};

		struct {
			glm::vec2 pos;
			glm::vec2 size;
		};
	};

	SDL_FRect* sdl() {
		// bad code, but very convenient!
		// Be very careful about the lifetime of this pointer!!
		return (SDL_FRect*)this;
	}
};

struct Rect {
	union {
		struct {
			int x, y, w, h;
		};

		struct {
			glm::ivec2 pos;
			glm::ivec2 size;
		};
	};

	Rect() : x(0), y(0), w(0), h(0) {};
	Rect(glm::ivec2 pos, glm::ivec2 size) : pos(pos), size(size) {};
	Rect(int x, int y, int w, int h) : x(x), y(y), w(w), h(h) {};

	FRect fp() const { return FRect{ (float)x, (float)y, (float)w, (float)h }; }

	bool contains(glm::ivec2 pt) const {
		glm::vec2 a = pos;
		glm::vec2 b = pos + size;
		return pt.x >= a.x && pt.x < b.x&& pt.y >= a.y && pt.y < b.y;
	}
	 
	Rect expand(int amount) {
		return { pos - amount, size + amount * 2 };
	}

	SDL_Rect* sdl() {
		// bad code, but very convenient!
		// Be very careful about the lifetime of this pointer!!
		return (SDL_Rect*)this;
	}
};


// And some for SDL Rect
SDL_Rect make_rect(glm::ivec2 top_left, glm::ivec2 size);

void draw_rect(SDL_Renderer* renderer, glm::ivec2 pos, glm::ivec2 size);
void draw_rect(SDL_Renderer* renderer, Rect r);

