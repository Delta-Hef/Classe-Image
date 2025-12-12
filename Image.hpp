#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <string>
#include <vector>
#include <stdexcept>
#include <ostream>

class Image
{
private:
    int width;
    int height;
    int channels;
    std::string model;
    std::vector<unsigned char> pixels;

    size_t getIndex(int x, int y, int c) const;

    static unsigned char clampToByte(int value)
    {
        if (value < 0) return 0;
        if (value > 255) return 255;
        return static_cast<unsigned char>(value);
    }

    void checkSameFormat(const Image& other) const;

public:
    Image();
    Image(int w, int h, int ch, const std::string& model = "NONE");
    Image(int w, int h, int ch, const std::string& model, unsigned char fillValue);
    Image(int w, int h, int ch, const std::string& model, const std::vector<unsigned char>& buffer);

    Image(const Image& other);
    Image& operator=(const Image& other);
    ~Image();

    void load(const std::string& filepath);
    void save(const std::string& filepath) const;

    inline int getWidth() const { return width; }
    inline int getHeight() const { return height; }
    inline int getChannels() const { return channels; }
    inline const std::string& getModel() const { return model; }

    inline void setModel(const std::string& m) { model = m; }

    void setWidth(int w);
    void setHeight(int h);
    void setChannels(int ch);

    void setPixel(int x, int y, int c, unsigned char value);
    unsigned char getPixel(int x, int y, int c) const;

    void resize(int newWidth, int newHeight);
    void clear();

    unsigned char& at(int x, int y, int c);
    const unsigned char& at(int x, int y, int c) const;

    inline unsigned char& operator()(int x, int y, int c) { return at(x, y, c); }
    inline const unsigned char& operator()(int x, int y, int c) const { return at(x, y, c); }

    // Opérations avec une autre image
    Image  operator+(const Image& other) const;
    Image& operator+=(const Image& other);

    Image  operator-(const Image& other) const;
    Image& operator-=(const Image& other);

    Image  operator^(const Image& other) const;
    Image& operator^=(const Image& other);

    // Opérations avec une valeur scalaire
    Image  operator+(int value) const;
    Image& operator+=(int value);

    Image  operator-(int value) const;
    Image& operator-=(int value);

    Image  operator^(int value) const;
    Image& operator^=(int value);

    // Opérations avec un "pixel" (vecteur de taille channels)
    Image  operator+(const std::vector<unsigned char>& pix) const;
    Image& operator+=(const std::vector<unsigned char>& pix);

    Image  operator-(const std::vector<unsigned char>& pix) const;
    Image& operator-=(const std::vector<unsigned char>& pix);

    Image  operator^(const std::vector<unsigned char>& pix) const;
    Image& operator^=(const std::vector<unsigned char>& pix);

    // Multiplication / division par un réel
    Image  operator*(double s) const;
    Image& operator*=(double s);

    Image  operator/(double s) const;
    Image& operator/=(double s);

    // Seuillage par une valeur : renvoie une image GRAY 0/255
    Image operator<(int threshold) const;
    Image operator<=(int threshold) const;
    Image operator>(int threshold) const;
    Image operator>=(int threshold) const;
    Image operator==(int threshold) const;
    Image operator!=(int threshold) const;

    // Inversion unaire ~
    Image operator~() const;

    // Utilitaires internes pour parcourir
    inline bool inBounds(int x, int y, int c) const
    {
        return x >= 0 && x < width && y >= 0 && y < height && c >= 0 && c < channels;
    }
};

// Implémentations inline

inline Image::Image()
    : width(0), height(0), channels(0), model("NONE"), pixels()
{
}

inline size_t Image::getIndex(int x, int y, int c) const
{
    return (static_cast<size_t>(y) * width + x) * channels + c;
}

inline void Image::setPixel(int x, int y, int c, unsigned char value)
{
    at(x, y, c) = value;
}

inline unsigned char Image::getPixel(int x, int y, int c) const
{
    return at(x, y, c);
}

inline void Image::clear()
{
    width = 0;
    height = 0;
    channels = 0;
    model = "NONE";
    pixels.clear();
}

std::ostream& operator<<(std::ostream& os, const Image& img);

#endif // IMAGE_HPP