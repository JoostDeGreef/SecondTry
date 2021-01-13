#include "png.h"
#include "PNGWrapper.h"
#include "fmt/core.h"

RGBAImage::RGBAImage(const RGBAImage& other)
    : m_data(nullptr)
    , m_width(other.m_width)
    , m_height(other.m_height)
{
    m_data = new unsigned char[4 * m_width * m_height];
    memcpy(m_data, other.m_data, 4 * m_width * m_height);
}

RGBAImage::RGBAImage(RGBAImage&& other)
    : m_data(nullptr)
    , m_width(0)
    , m_height(0)
{
    std::swap(m_data, other.m_data);
    std::swap(m_width, other.m_width);
    std::swap(m_height, other.m_height);
}

RGBAImage::RGBAImage(const int width, const int height)
    : m_data(nullptr)
    , m_width(width)
    , m_height(height)
{
    m_data = new unsigned char[4*m_width*m_height];
}

RGBAImage::RGBAImage(const std::string& filename)
    : m_data(nullptr)
    , m_width(0)
    , m_height(0)
{
    ReadPNG(filename);
}

RGBAImage::RGBAImage(const char* const blob, const size_t blobSize)
    : m_data(nullptr)
    , m_width(0)
    , m_height(0)
{
    ReadPNG(blob, blobSize);
}

RGBAImage::RGBAImage(const std::vector<char>& blob)
    : m_data(nullptr)
    , m_width(0)
    , m_height(0)
{
    ReadPNG(blob);
}

RGBAImage::~RGBAImage()
{
    delete[] m_data;
}

RGBAImage& RGBAImage::operator = (const RGBAImage& other)
{
    delete[] m_data;
    m_width = other.m_width;
    m_height = other.m_height;
    m_data = new unsigned char[4 * m_width * m_height];
    memcpy(m_data, other.m_data, 4 * m_width * m_height);
    return *this;
}

RGBAImage& RGBAImage::operator = (RGBAImage&& other)
{
    std::swap(m_data, other.m_data);
    std::swap(m_width, other.m_width);
    std::swap(m_height, other.m_height);
    return *this;
}

RGBAImage& RGBAImage::ReadPNG(const std::string& filename)
{
    return *this;
}

RGBAImage& RGBAImage::ReadPNG(const char* const blob, const size_t blobSize)
{
    return *this;
}

RGBAImage& RGBAImage::ReadPNG(const std::vector<char>& blob)
{
    return ReadPNG(blob.data(),blob.size());
}

std::vector<char> RGBAImage::StorePNG()
{
    return std::vector<char>();
}

