#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <string>
#include <vector>

class Image
{
private:
    int width;
    int height;
    std::vector<unsigned char> pixels;

public:
    Image();
    Image(int w, int h);
    ~Image();

    void load(const std::string& filepath);
    void save(const std::string& filepath);
    
    int getWidth() const;
    int getHeight() const;
    
    void setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);
    void getPixel(int x, int y, unsigned char& r, unsigned char& g, unsigned char& b) const;
    
    void resize(int newWidth, int newHeight);
    void clear();
};

#endif // IMAGE_HPP