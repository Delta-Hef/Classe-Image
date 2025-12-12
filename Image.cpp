#include "Image.hpp"
#include <algorithm>

Image::Image(int w, int h, int ch, const std::string& model)
    : width(w), height(h), channels(ch), model(model), pixels()
{
    if (w < 0 || h < 0 || ch < 0) throw std::invalid_argument("Negative dimension");
    pixels.resize(static_cast<size_t>(w) * h * ch);
}

Image::Image(int w, int h, int ch, const std::string& model, unsigned char fillValue)
    : width(w), height(h), channels(ch), model(model), pixels()
{
    if (w < 0 || h < 0 || ch < 0) throw std::invalid_argument("Negative dimension");
    pixels.assign(static_cast<size_t>(w) * h * ch, fillValue);
}

Image::Image(int w, int h, int ch, const std::string& model, const std::vector<unsigned char>& buffer)
    : width(w), height(h), channels(ch), model(model), pixels()
{
    if (w < 0 || h < 0 || ch < 0) throw std::invalid_argument("Negative dimension");
    size_t expected = static_cast<size_t>(w) * h * ch;
    if (buffer.size() != expected) throw std::invalid_argument("Buffer size does not match dimensions");
    pixels = buffer;
}

Image::Image(const Image& other)
    : width(other.width), height(other.height), channels(other.channels), model(other.model), pixels(other.pixels)
{
}

Image& Image::operator=(const Image& other)
{
    if (this == &other) return *this;
    width = other.width;
    height = other.height;
    channels = other.channels;
    model = other.model;
    pixels = other.pixels;
    return *this;
}

Image::~Image() {}

unsigned char& Image::at(int x, int y, int c)
{
    if (x < 0 || x >= width || y < 0 || y >= height || c < 0 || c >= channels) {
        throw std::out_of_range("Coordinates out of range");
    }
    return pixels[getIndex(x, y, c)];
}

const unsigned char& Image::at(int x, int y, int c) const
{
    if (x < 0 || x >= width || y < 0 || y >= height || c < 0 || c >= channels) {
        throw std::out_of_range("Coordinates out of range");
    }
    return pixels[getIndex(x, y, c)];
}

void Image::resize(int newWidth, int newHeight)
{
    if (newWidth < 0 || newHeight < 0) throw std::invalid_argument("Negative dimension");
    if (channels <= 0) throw std::logic_error("Channels not set or invalid");

    std::vector<unsigned char> newPixels(static_cast<size_t>(newWidth) * newHeight * channels);
    
    int copyW = std::min(width, newWidth);
    int copyH = std::min(height, newHeight);
    
    for (int y = 0; y < copyH; ++y) {
        for (int x = 0; x < copyW; ++x) {
            for (int c = 0; c < channels; ++c) {
                size_t oldIdx = getIndex(x, y, c);
                size_t newIdx = (static_cast<size_t>(y) * newWidth + x) * channels + c;
                newPixels[newIdx] = pixels[oldIdx];
            }
        }
    }
    
    width = newWidth;
    height = newHeight;
    pixels.swap(newPixels);
}

void Image::setWidth(int w) 
{ 
    resize(w, height); 
}

void Image::setHeight(int h) 
{ 
    resize(width, h); 
}

void Image::setChannels(int ch) 
{ 
    if (ch <= 0) throw std::invalid_argument("Channels must be positive"); 
    channels = ch; 
    pixels.clear(); 
    pixels.resize(static_cast<size_t>(width) * height * channels);
}

void Image::load(const std::string& filepath)
{
    
}

void Image::save(const std::string& filepath)
{
    
}