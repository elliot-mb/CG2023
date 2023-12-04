#pragma once
#include <iostream>
#include <fstream>
#include <stdexcept>
#include "../../src/Utils.h"

class TextureMap {
public:
	size_t width;
	size_t height;
	std::vector<uint32_t> pixels;

    uint32_t pixel(int x, int y);

	TextureMap();
	explicit TextureMap(const std::string &filename);
	friend std::ostream &operator<<(std::ostream &os, const TextureMap &point);

};
