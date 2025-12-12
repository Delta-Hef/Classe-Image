#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <string>
#include <vector>
#include <stdexcept>

class Image
{
private:
    int width;
    int height;
    int channels;
    std::string model;
    std::vector<unsigned char> pixels;
    
    size_t getIndex(int x, int y, int c) const; 

public:
    Image();
    Image(int w, int h, int ch, const std::string& model = "NONE");
    Image(int w, int h, int ch, const std::string& model, unsigned char fillValue);
    Image(int w, h, int ch, const std::string& model, const std::vector<unsigned char>& buffer);

    Image(const Image& other);
    Image& operator=(const Image& other);
    ~Image();

    void load(const std::string& filepath);
    void save(const std::string& filepath);
    
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
};

inline Image::Image()
    : width(0), height(0), channels(0), model("NONE"), pixels()
{
}

inline size_t Image::getIndex(int x, int y, int c) const
{
    return (static_cast<size_t>(y) * width + x) * channels + c;
}

inline void Image::setPixel(int x, int y, int c, unsigned char value) { at(x, y, c) = value; }
inline unsigned char Image::getPixel(int x, int y, int c) const { return at(x, y, c); }
inline void Image::clear()
{
    width = 0;
    height = 0;
    channels = 0;
    model = "NONE";
    pixels.clear();
}

#endif // IMAGE_HPP