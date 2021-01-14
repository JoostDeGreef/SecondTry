#pragma once

#include <string>
#include <stdexcept>
#include <sstream>
#include <vector>

class RGBAImage
{
public:
    class Pixel
    {
    public:
        Pixel(const unsigned char& r, const unsigned char& g, const unsigned char& b, const unsigned char& a)
            : R(r)
            , G(g)
            , B(b)
            , A(a)
        {}

        union 
        {
            unsigned int RGBA;
            struct
            {
                unsigned char R;
                unsigned char G;
                unsigned char B;
                unsigned char A;
            };
        };
    };
    class PixelProxy
    {
    public:
        PixelProxy(const unsigned char * rgba)
            : RGBA(rgba)
        {}

        PixelProxy& operator = (const Pixel & pixel)
        {
            memcpy((unsigned char*)RGBA,(unsigned char*)(&pixel.RGBA),4);
            return *this;
        }

        operator Pixel () const 
        {
            return Pixel(RGBA[0],RGBA[1],RGBA[2],RGBA[3]);
        }

    private:
        const unsigned char * RGBA;
    };

    RGBAImage(const RGBAImage& other);
    RGBAImage(RGBAImage&& other);
    RGBAImage(const int width, const int height);
    RGBAImage(const std::vector<char>& blob);
    RGBAImage(const std::string& filename); // PNG file
    RGBAImage(const char* const blob, const size_t blobSize); // PNG blob
    ~RGBAImage();
    RGBAImage & operator = (const RGBAImage& other);
    RGBAImage & operator = (RGBAImage&& other);

    bool operator == (const RGBAImage& other) const
    {
        return 
            m_width == other.m_width &&
            m_height == other.m_height &&
            0 == memcmp(m_data, other.m_data, m_width * m_height * 4);
    }
    bool hasData() const { return nullptr != m_data; }

    PixelProxy operator [] (const size_t & index) 
    {
        return PixelProxy(m_data + index * 4);
    }
    const PixelProxy operator [] (const size_t& index) const
    {
        return PixelProxy(m_data + index * 4);
    }

    auto Width() const { return m_width; }
    auto Height() const { return m_height; }

    RGBAImage& ReadPNG(const std::string & filename);
    RGBAImage& ReadPNG(const char * const blob, const size_t blobSize);
    RGBAImage& ReadPNG(const std::vector<char> & blob);

    std::vector<char> StorePNG();
private:
    class IBlobReader
    {
    public:
        virtual size_t Read(unsigned char* blobData, const size_t blobSize) = 0;
    };
    RGBAImage& ReadPNG(IBlobReader * reader);
    unsigned char* Row(const unsigned int index) const;

    unsigned char* m_data;
    int m_width;
    int m_height;
};

// for Google Test mainly
inline
std::ostream& operator<<(std::ostream& stream, RGBAImage const& image)
{
    stream << "RGBAImage(" << image.Width() << "," << image.Height();
    if (image.hasData())
    {
        for (int i = 0; i < image.Width() * image.Height(); ++i)
        {
            RGBAImage::Pixel pixel = image[i];
            stream << ",(" << (int)pixel.R << "," << (int)pixel.G << "," << (int)pixel.B << "," << (int)pixel.A << ")";
        }
    }
    else
    {
        stream << ",(null)";
    }
    stream << ")";
    return stream;
}

