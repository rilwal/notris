
#include "util.hpp"

TTF_Font* G_FONT;

std::map<SDL_Keycode, bool> InputManager::m_key_state;
std::map<SDL_Keycode, bool> InputManager::m_last_key_state;

uint32_t InputManager::m_mouse_state;
uint32_t InputManager::m_previous_mouse_state;

glm::ivec2 InputManager::m_mouse_pos;
glm::ivec2 InputManager::m_scroll;

std::function<void(SDL_Keycode)> InputManager::m_next_key_callback;
std::function<void(void)> InputManager::m_next_key_fail_callback;

// And some for SDL Rect
SDL_Rect make_rect(glm::ivec2 top_left, glm::ivec2 size) {
	SDL_Rect r;
	r.x = top_left.x;
	r.y = top_left.y;
	r.w = size.x;
	r.h = size.y;
	return r;
}

void draw_rect(SDL_Renderer* renderer, Rect r) {
	SDL_RenderFillRect(renderer, r.sdl());
}

void draw_rect(SDL_Renderer* renderer, glm::ivec2 pos, glm::ivec2 size) {
	SDL_Rect r = make_rect(pos, size);
	SDL_RenderFillRect(renderer, &r);
}

void InputManager::update(float delta_time) {
	m_last_key_state = m_key_state;

	m_previous_mouse_state = m_mouse_state;
	m_mouse_state = SDL_GetMouseState(&m_mouse_pos.x, &m_mouse_pos.y);

	m_scroll = { };
}

glm::ivec2 InputManager::get_mouse_pos() {
	return m_mouse_pos;
}

void InputManager::get_next_key(std::function<void(SDL_Keycode)> callback, std::function<void(void)> fail_callback) {
	if (m_next_key_callback && m_next_key_fail_callback) {
		m_next_key_fail_callback();
	}
	m_next_key_callback = callback;
	m_next_key_fail_callback = fail_callback;
}

void InputManager::handle_input_event(SDL_Event& e) {
	if (e.type == SDL_KEYDOWN) {
		if (m_next_key_callback) {
			m_next_key_callback(e.key.keysym.sym);
			m_next_key_callback = nullptr;
		}

		m_key_state[e.key.keysym.sym] = true;
	}
	else if (e.type == SDL_KEYUP) {
		m_key_state[e.key.keysym.sym] = false;
	}
	else if (e.type == SDL_MOUSEWHEEL) {
		m_scroll = { e.wheel.x, e.wheel.y };
	}
}

bool InputManager::key_down(SDL_Keycode ks) {
	return m_key_state[ks];
}

bool InputManager::key_up(SDL_Keycode ks) {
	return m_last_key_state[ks] && !m_key_state[ks];
}

bool InputManager::key_pressed(SDL_Keycode ks) {
	return !m_last_key_state[ks] && m_key_state[ks];
}

glm::ivec2 get_text_size(std::string message, float size) {
	glm::ivec2 ret = {};
	TTF_SizeText(G_FONT, message.c_str(), &ret.x, &ret.y);
	ret = (glm::ivec2)((glm::vec2)ret * size);
	return ret;
}


std::map<std::string, SDL_Texture*> text_render_cache;


void draw_text(SDL_Renderer* renderer, std::string message, glm::vec2 pos, Color c, bool center, float size) {
	if (text_render_cache.contains(message)) {
		auto text_texture = text_render_cache[message];

		glm::ivec2 text_size = {};
		SDL_QueryTexture(text_texture, nullptr, nullptr, &text_size.x, &text_size.y);

		text_size = (glm::vec2)text_size * size;
		if (center) pos.x -= text_size.x / 2;

		auto text_rect = make_rect(pos, text_size);
		SDL_RenderCopy(renderer, text_texture, nullptr, &text_rect);
		return;
	}

	SDL_Surface* score_surface = TTF_RenderText_Blended(G_FONT, message.c_str(), c.sdl());
	SDL_Texture* score_texture = SDL_CreateTextureFromSurface(renderer, score_surface);
	SDL_FreeSurface(score_surface);
	
	glm::ivec2 text_size = {};
	SDL_QueryTexture(score_texture, nullptr, nullptr, &text_size.x, &text_size.y);

	text_size = (glm::vec2)text_size * size;
	if (center) pos.x -= text_size.x / 2;

	auto text_rect = make_rect(pos, text_size);
	SDL_RenderCopy(renderer, score_texture, nullptr, &text_rect);

	text_render_cache[message] = score_texture;
}


std::vector<uint8_t> load_file(std::string filename, size_t offset, size_t len) {
	FILE* file;
	std::vector<uint8_t> contents;

#ifdef _WIN32
	errno_t error = fopen_s(&file, filename.c_str(), "rb");

	if (error != 0) {
		fprintf(stderr, "Failed to open file %s.\n", filename.c_str());
	}
#else
	file = fopen(filename.c_str(), "r");
#endif

	if (file == 0) {
		fprintf(stderr, "Failed to open file %s.\n", filename.c_str());
		return contents;
	}

	fseek(file, 0, SEEK_END);
	size_t filesize = ftell(file);
	rewind(file);

	if (len == -1) len = filesize;

	contents.resize(len + 1);

	fseek(file, (long)offset, SEEK_CUR);

	fread(contents.data(), 1, len, file);
	fclose(file);

	contents[len] = '\0';

	return contents;
}

void save_file(std::string filename, const std::vector<uint8_t> data) {
	save_file(filename, data.data(), data.size());
}

void save_file(std::string filename, const uint8_t* data, size_t len) {
	FILE* file;

#ifdef _WIN32
	errno_t error = fopen_s(&file, filename.c_str(), "wb");

	if (error != 0) {
		fprintf(stderr, "Failed to open file %s for writing.\n", filename.c_str());
		return;
	}
#else
	file = fopen(filename.c_str(), "wb");
#endif

	if (!file) {
		fprintf(stderr, "Failed to open file %s.\n", filename.c_str());
		return;
	}

	fwrite(data, 1, len, file);
}

