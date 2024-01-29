#include "audio.hpp"


void Audio::load_from_file(const char* filename) {
	m_music_handle = Mix_LoadWAV(filename);
}

void Audio::reload() {
	unload();
	load_from_file(path.c_str());
}

void Audio::unload() {
	Mix_FreeChunk(m_music_handle);
}

void Audio::play(Channel channel, int loops) {
	Mix_PlayChannel((int)channel, m_music_handle, loops);
}
