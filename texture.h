#pragma once

#include <glad/glad.h>

#include <iostream>

#include "stb_image.h"

class Texture
{
public:
	unsigned int ID;
	int width, height, nrChannels;
	unsigned char* data;

	Texture(const char* texturePath, bool format_rgb);
};