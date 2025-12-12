#include "Image.hpp"
#include <algorithm>// std::min
#include <iostream>// pour operator<< (optionnel)
#include <fstream>

void Image::checkSameFormat(const Image& other) const
{
    if (channels != other.channels || model != other.model)
        throw std::invalid_argument("Images have different format (channels/model)");
}

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
    if (!inBounds(x, y, c)) {
        throw std::out_of_range("Coordinates out of range");
    }
    return pixels[getIndex(x, y, c)];
}

const unsigned char& Image::at(int x, int y, int c) const
{
    if (!inBounds(x, y, c)) {
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
    std::ifstream in(filepath, std::ios::binary);
    if (!in) throw std::runtime_error("Cannot open file for reading");

    int w, h, ch;
    std::string m;

    in.read(reinterpret_cast<char*>(&w), sizeof(int));
    in.read(reinterpret_cast<char*>(&h), sizeof(int));
    in.read(reinterpret_cast<char*>(&ch), sizeof(int));

    size_t modelSize = 0;
    in.read(reinterpret_cast<char*>(&modelSize), sizeof(size_t));
    m.resize(modelSize);
    in.read(&m[0], static_cast<std::streamsize>(modelSize));

    if (!in) throw std::runtime_error("Error while reading header");

    width = w;
    height = h;
    channels = ch;
    model = m;

    size_t total = static_cast<size_t>(width) * height * channels;
    pixels.resize(total);
    in.read(reinterpret_cast<char*>(pixels.data()), static_cast<std::streamsize>(total));

    if (!in) throw std::runtime_error("Error while reading pixel data");
}

void Image::save(const std::string& filepath) const
{
    std::ofstream out(filepath, std::ios::binary);
    if (!out) throw std::runtime_error("Cannot open file for writing");

    out.write(reinterpret_cast<const char*>(&width), sizeof(int));
    out.write(reinterpret_cast<const char*>(&height), sizeof(int));
    out.write(reinterpret_cast<const char*>(&channels), sizeof(int));

    size_t modelSize = model.size();
    out.write(reinterpret_cast<const char*>(&modelSize), sizeof(size_t));
    out.write(model.data(), static_cast<std::streamsize>(modelSize));

    if (!pixels.empty()) {
        out.write(reinterpret_cast<const char*>(pixels.data()),
                  static_cast<std::streamsize>(pixels.size()));
    }

    if (!out) throw std::runtime_error("Error while writing file");
}


static void computeMaxSize(const Image& a, const Image& b, int& outW, int& outH)
{
    outW = std::max(a.getWidth(),  b.getWidth());
    outH = std::max(a.getHeight(), b.getHeight());
}

static unsigned char getOrZero(const Image& img, int x, int y, int c)
{
    if (img.inBounds(x, y, c))
        return img.getPixel(x, y, c);
    return 0;
}

Image Image::operator+(const Image& other) const
{
    checkSameFormat(other);
    int newW, newH;
    computeMaxSize(*this, other, newW, newH);

    Image result(newW, newH, channels, model, 0);

    for (int y = 0; y < newH; ++y) {
        for (int x = 0; x < newW; ++x) {
            for (int c = 0; c < channels; ++c) {
                int sum = getOrZero(*this, x, y, c) + getOrZero(other, x, y, c);
                result(x, y, c) = clampToByte(sum);
            }
        }
    }
    return result;
}

Image& Image::operator+=(const Image& other)
{
    *this = *this + other;
    return *this;
}

Image Image::operator-(const Image& other) const
{
    checkSameFormat(other);
    int newW, newH;
    computeMaxSize(*this, other, newW, newH);

    Image result(newW, newH, channels, model, 0);

    for (int y = 0; y < newH; ++y) {
        for (int x = 0; x < newW; ++x) {
            for (int c = 0; c < channels; ++c) {
                int diff = getOrZero(*this, x, y, c) - getOrZero(other, x, y, c);
                result(x, y, c) = clampToByte(diff);
            }
        }
    }
    return result;
}

Image& Image::operator-=(const Image& other)
{
    *this = *this - other;
    return *this;
}

// Différence (on choisit différence absolue)
Image Image::operator^(const Image& other) const
{
    checkSameFormat(other);
    int newW, newH;
    computeMaxSize(*this, other, newW, newH);

    Image result(newW, newH, channels, model, 0);

    for (int y = 0; y < newH; ++y) {
        for (int x = 0; x < newW; ++x) {
            for (int c = 0; c < channels; ++c) {
                int a = getOrZero(*this, x, y, c);
                int b = getOrZero(other, x, y, c);
                int d = std::abs(a - b);
                result(x, y, c) = clampToByte(d);
            }
        }
    }
    return result;
}

Image& Image::operator^=(const Image& other)
{
    *this = *this ^ other;
    return *this;
}

Image Image::operator+(int value) const
{
    Image result(*this);
    for (size_t i = 0; i < pixels.size(); ++i) {
        result.pixels[i] = clampToByte(static_cast<int>(pixels[i]) + value);
    }
    return result;
}

Image& Image::operator+=(int value)
{
    for (size_t i = 0; i < pixels.size(); ++i) {
        pixels[i] = clampToByte(static_cast<int>(pixels[i]) + value);
    }
    return *this;
}

Image Image::operator-(int value) const
{
    Image result(*this);
    for (size_t i = 0; i < pixels.size(); ++i) {
        result.pixels[i] = clampToByte(static_cast<int>(pixels[i]) - value);
    }
    return result;
}

Image& Image::operator-=(int value)
{
    for (size_t i = 0; i < pixels.size(); ++i) {
        pixels[i] = clampToByte(static_cast<int>(pixels[i]) - value);
    }
    return *this;
}

Image Image::operator^(int value) const
{
    Image result(*this);
    for (size_t i = 0; i < pixels.size(); ++i) {
        int d = std::abs(static_cast<int>(pixels[i]) - value);
        result.pixels[i] = clampToByte(d);
    }
    return result;
}

Image& Image::operator^=(int value)
{
    for (size_t i = 0; i < pixels.size(); ++i) {
        int d = std::abs(static_cast<int>(pixels[i]) - value);
        pixels[i] = clampToByte(d);
    }
    return *this;
}

static void checkPixelSize(const Image& img, const std::vector<unsigned char>& pix)
{
    if (static_cast<int>(pix.size()) != img.getChannels())
        throw std::invalid_argument("Pixel size does not match number of channels");
}

Image Image::operator+(const std::vector<unsigned char>& pix) const
{
    checkPixelSize(*this, pix);
    Image result(*this);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int c = 0; c < channels; ++c) {
                int sum = at(x, y, c) + pix[c];
                result(x, y, c) = clampToByte(sum);
            }
        }
    }
    return result;
}

Image& Image::operator+=(const std::vector<unsigned char>& pix)
{
    checkPixelSize(*this, pix);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int c = 0; c < channels; ++c) {
                int sum = at(x, y, c) + pix[c];
                at(x, y, c) = clampToByte(sum);
            }
        }
    }
    return *this;
}

Image Image::operator-(const std::vector<unsigned char>& pix) const
{
    checkPixelSize(*this, pix);
    Image result(*this);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int c = 0; c < channels; ++c) {
                int diff = at(x, y, c) - pix[c];
                result(x, y, c) = clampToByte(diff);
            }
        }
    }
    return result;
}

Image& Image::operator-=(const std::vector<unsigned char>& pix)
{
    checkPixelSize(*this, pix);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int c = 0; c < channels; ++c) {
                int diff = at(x, y, c) - pix[c];
                at(x, y, c) = clampToByte(diff);
            }
        }
    }
    return *this;
}

Image Image::operator^(const std::vector<unsigned char>& pix) const
{
    checkPixelSize(*this, pix);
    Image result(*this);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int c = 0; c < channels; ++c) {
                int d = std::abs(at(x, y, c) - pix[c]);
                result(x, y, c) = clampToByte(d);
            }
        }
    }
    return result;
}

Image& Image::operator^=(const std::vector<unsigned char>& pix)
{
    checkPixelSize(*this, pix);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int c = 0; c < channels; ++c) {
                int d = std::abs(at(x, y, c) - pix[c]);
                at(x, y, c) = clampToByte(d);
            }
        }
    }
    return *this;
}

Image Image::operator*(double s) const
{
    Image result(*this);
    for (size_t i = 0; i < pixels.size(); ++i) {
        int v = static_cast<int>(pixels[i] * s);
        result.pixels[i] = clampToByte(v);
    }
    return result;
}

Image& Image::operator*=(double s)
{
    for (size_t i = 0; i < pixels.size(); ++i) {
        int v = static_cast<int>(pixels[i] * s);
        pixels[i] = clampToByte(v);
    }
    return *this;
}

Image Image::operator/(double s) const
{
    if (s == 0.0) throw std::invalid_argument("Division by zero");
    Image result(*this);
    for (size_t i = 0; i < pixels.size(); ++i) {
        int v = static_cast<int>(pixels[i] / s);
        result.pixels[i] = clampToByte(v);
    }
    return result;
}

Image& Image::operator/=(double s)
{
    if (s == 0.0) throw std::invalid_argument("Division by zero");
    for (size_t i = 0; i < pixels.size(); ++i) {
        int v = static_cast<int>(pixels[i] / s);
        pixels[i] = clampToByte(v);
    }
    return *this;
}

static unsigned char thresholdPixel(const unsigned char* vals, int ch, int threshold, char op)
{
    bool ok = true;
    for (int c = 0; c < ch; ++c) {
        int v = vals[c];
        bool cond = false;
        switch (op) {
        case '<':  cond = (v <  threshold); break;
        case 'l':  cond = (v <= threshold); break;
        case '>':  cond = (v >  threshold); break;
        case 'g':  cond = (v >= threshold); break;
        case '=':  cond = (v == threshold); break;
        case 'n':  cond = (v != threshold); break;
        }
        if (!cond) { ok = false; break; }
    }
    return ok ? 255 : 0;
}

Image Image::operator<(int threshold) const
{
    Image result(width, height, 1, "GRAY", 0);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            std::vector<unsigned char> tmp(channels);
            for (int c = 0; c < channels; ++c) tmp[c] = getPixel(x, y, c);
            result(x, y, 0) = thresholdPixel(tmp.data(), channels, threshold, '<');
        }
    }
    return result;
}

Image Image::operator<=(int threshold) const
{
    Image result(width, height, 1, "GRAY", 0);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            std::vector<unsigned char> tmp(channels);
            for (int c = 0; c < channels; ++c) tmp[c] = getPixel(x, y, c);
            result(x, y, 0) = thresholdPixel(tmp.data(), channels, threshold, 'l');
        }
    }
    return result;
}

Image Image::operator>(int threshold) const
{
    Image result(width, height, 1, "GRAY", 0);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            std::vector<unsigned char> tmp(channels);
            for (int c = 0; c < channels; ++c) tmp[c] = getPixel(x, y, c);
            result(x, y, 0) = thresholdPixel(tmp.data(), channels, threshold, '>');
        }
    }
    return result;
}

Image Image::operator>=(int threshold) const
{
    Image result(width, height, 1, "GRAY", 0);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            std::vector<unsigned char> tmp(channels);
            for (int c = 0; c < channels; ++c) tmp[c] = getPixel(x, y, c);
            result(x, y, 0) = thresholdPixel(tmp.data(), channels, threshold, 'g');
        }
    }
    return result;
}

Image Image::operator==(int threshold) const
{
    Image result(width, height, 1, "GRAY", 0);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            std::vector<unsigned char> tmp(channels);
            for (int c = 0; c < channels; ++c) tmp[c] = getPixel(x, y, c);
            result(x, y, 0) = thresholdPixel(tmp.data(), channels, threshold, '=');
        }
    }
    return result;
}

Image Image::operator!=(int threshold) const
{
    Image result(width, height, 1, "GRAY", 0);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            std::vector<unsigned char> tmp(channels);
            for (int c = 0; c < channels; ++c) tmp[c] = getPixel(x, y, c);
            result(x, y, 0) = thresholdPixel(tmp.data(), channels, threshold, 'n');
        }
    }
    return result;
}

Image Image::operator~() const
{
    Image result(*this);
    for (size_t i = 0; i < pixels.size(); ++i) {
        result.pixels[i] = static_cast<unsigned char>(255 - pixels[i]);
    }
    return result;
}

std::ostream& operator<<(std::ostream& os, const Image& img)
{
    os << "Image(" << img.getWidth() << "x" << img.getHeight()
       << "x" << img.getChannels() << ", " << img.getModel() << ")";
    return os;
}