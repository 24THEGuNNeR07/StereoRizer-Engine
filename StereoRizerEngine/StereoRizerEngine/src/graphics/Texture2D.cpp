#include <graphics/Texture.h>

using namespace stereorizer::core;
using namespace stereorizer::graphics;

void loadJPG(const std::string &imgPath)
{
	stbi_set_flip_vertically_on_load(false);
	int width, height, nrChannels;
	uint8_t *data = stbi_load(imgPath.c_str(), &width, &height, &nrChannels, 0);

	if(data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Error while loading texture." << std::endl;
	}

	stbi_image_free(data);
}

void loadPNG(const std::string &imgPath)
{
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrChannels;
	uint8_t *data = stbi_load(imgPath.c_str(), &width, &height, &nrChannels, 0);

	if(data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Error while loading texture." << std::endl;
	}

	stbi_image_free(data);
}

void loadTexture2D(const std::string &imgPath)
{
	if(StrUtils::endsWith(imgPath, ".jpg") || StrUtils::endsWith(imgPath, ".jpeg"))
	{
		loadJPG(imgPath);
	}
	else if(StrUtils::endsWith(imgPath, ".png"))
	{
		loadPNG(imgPath);
	}
	else
	{
		std::cout << "Image format not supported." << std::endl;
	}
}

Texture2D::Texture2D(const char *imgPath, TextureWrapping wrapping, TextureFiltering minFiltering, TextureFiltering maxFiltering) : path(imgPath)
{
	glGenTextures(1, &id);
	use();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (int32_t)wrapping);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (int32_t)wrapping);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (int32_t)minFiltering);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (int32_t)maxFiltering);

	loadTexture2D(imgPath);
}

void Texture2D::changeFiltering(TextureFiltering minFiltering, TextureFiltering maxFiltering)
{
	useIfNecessary();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (int32_t)minFiltering);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (int32_t)maxFiltering);
}

void Texture2D::changeWrapping(TextureWrapping wrapping, float *borderColor)
{
	useIfNecessary();
	if(wrapping == TextureWrapping::CLAMP_TO_BORDER)
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (int32_t)wrapping);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (int32_t)wrapping);
}

GLOBJ_DEFAULTS_MULTI_SPEC(Texture2D, glBindTexture, GL_TEXTURE_2D, glDeleteTextures)


GLOBJ_LAST_USED(Texture2D)