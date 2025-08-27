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

unsigned int Image::getFormat() const
{
	return format;
}

void Image::setFormat(unsigned int new_format)
{
	format = new_format;
}

unsigned int Image::getWidth() const
{
	return width;
}

void Image::setWidth(unsigned int new_width)
{
	width = new_width;
}

unsigned int Image::getHeight() const
{
	return height;
}

void Image::setHeight(unsigned int new_height)
{
	height = new_height;
}
