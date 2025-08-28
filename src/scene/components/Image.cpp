#include "Image.hpp"

Image::Image(const std::string& name) :
    Component(name)
{}

std::type_index Image::getType()
{
	return typeid(Image);
}

const std::vector<uint8_t>& Image::getData() const
{
	return data;
}

void Image::setData(const std::vector<uint8_t>& new_data)
{
	data = new_data;
}

uint32_t Image::getFormat() const
{
	return format;
}

void Image::setFormat(uint32_t new_format)
{
	format = new_format;
}

uint32_t Image::getWidth() const
{
	return width;
}

void Image::setWidth(uint32_t new_width)
{
	width = new_width;
}

uint32_t Image::getHeight() const
{
	return height;
}

void Image::setHeight(uint32_t new_height)
{
	height = new_height;
}
