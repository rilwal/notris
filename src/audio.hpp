#pragma once

#include <SDL_mixer.h>

#include "asset_manager.hpp"

struct Audio : public IAsset {
	enum class Channel : int {
		SFX,
		BGM,
		COUNT
	};

	Audio() : IAsset("Audio"), m_music_handle(0) {};

	virtual void load_from_file(const char* filename) override;
	virtual void reload() override;

	virtual void unload() override;

	void play(Channel channel=Channel::SFX, int loops=0);

private:
	Mix_Chunk* m_music_handle;
};
