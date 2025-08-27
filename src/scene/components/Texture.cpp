#include "Texture.hpp"

Texture::Texture(const std::string& name) :
    Component(name)
{}

std::type_index Texture::getType()
{
	return typeid(Texture);
}

auto Texture::getImage() -> Image*
{
	return image;
}

void Texture::setImage(Image& image)
{
	this->image = &image;
}
