#pragma once

/*
	Asset handling for another-engine.

	Assets should have a standard API to load from file or from memory.
	All asset types should be managed here.
*/

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <concepts>

#include "util.hpp"

class IAsset {
public:
	bool _is_loaded = false;
	bool _hot_reload = false;

	std::string path;
	std::string asset_type;
	IAsset(std::string _asset_type) : asset_type(_asset_type)  {};

	virtual void load_from_file(const char* filename) = 0;
	virtual void reload() = 0;

	virtual void unload() = 0;

	inline bool is_loaded() { return _is_loaded; };
};



template <typename T>
concept Asset = std::derived_from<T, IAsset>;

template <Asset T>
std::vector<WeakRef<T>> asset_lib;

template <Asset T>
std::map<std::string, WeakRef<T>> path_to_asset_map;

class AssetManager {
private:

public:
	template <Asset T>
	Ref<T> LoadAsset(std::string filename, bool hot_reload = true) {
		Ref<T> asset = make_ref<T>();
		asset->path = filename;
		asset->load_from_file(filename.c_str());

		asset_lib<T>.push_back(make_weak_ref(asset));
		path_to_asset_map<T>[std::string(filename)] = make_weak_ref(asset);
		return asset;
	}

	template <Asset T>
	Ref<T> GetByPath(std::string filename) {
		if (path_to_asset_map<T>.contains(filename)) {
			if (Ref<T> ret = path_to_asset_map<T>[filename].lock())
				return ret;
			else
				std::cerr << "Dropped asset\n";
		}
		
		return LoadAsset<T>(filename);
	}

};

extern AssetManager asset_manager;

