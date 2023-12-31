#pragma once

#include <glad/glad.h>

#include <iostream>

#include "stb_image.h"

class Texture
{
public:
	unsigned int ID;
	int width, height;
	unsigned char* data;
	Texture();
	Texture(const char* texturePath, bool format_rgb);
};