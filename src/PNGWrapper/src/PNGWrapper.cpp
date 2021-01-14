#include "png.h"
#include "PNGWrapper.h"
#include "fmt/core.h"

#include <fstream>

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
    class FileReader : public IBlobReader
    {
    public:
        FileReader(const std::string& filename)
            : m_stream(filename)
        {}

        virtual size_t Read(unsigned char* blobData, const size_t blobSize) override
        {
            if (!m_stream.bad() &&
                !m_stream.read((char*)blobData, blobSize).bad())
            { 
                return blobSize;
            }
            else
            {
                return 0;
            }
        }
    private:
        std::ifstream m_stream;
    } fileReader(filename);

    return ReadPNG((IBlobReader*)&fileReader);
}

RGBAImage& RGBAImage::ReadPNG(const char* const blob, const size_t blobSize)
{
    class BlobReader : public IBlobReader
    {
    public:
        BlobReader(const char* const blob, const size_t blobSize)
            : m_blob(blob)
            , m_size(blobSize)
            , m_index(0)
        {}

        virtual size_t Read(unsigned char* blobData, const size_t blobSize) override
        {
            if (m_index+blobSize<=m_size)
            {
                memcpy(blobData, m_blob + m_index, blobSize);
                m_index += blobSize;
                return blobSize;
            }
            else
            {
                return 0;
            }
        }
    private:
        const char* const m_blob;
        const size_t m_size;
              size_t m_index;
    } blobReader(blob,blobSize);

    return ReadPNG((IBlobReader*)&blobReader);
}

RGBAImage& RGBAImage::ReadPNG(const std::vector<char>& blob)
{
    return ReadPNG(blob.data(),blob.size());
}

unsigned char* RGBAImage::Row(const unsigned int index) const
{
    const int stride = m_width * 4; // RGBA
    png_uint_32 q = (m_height - index - 1) * stride;
    return m_data + q;
}

std::vector<char> RGBAImage::StorePNG()
{
    class Writer
    {
    public:
        Writer(const RGBAImage & image)
            : image(image)
            , res()
            , pngPtr(nullptr)
            , infoPtr(nullptr)
        {
            res.reserve(4096);
        }
        ~Writer()
        {
            png_destroy_write_struct(&pngPtr, &infoPtr);
        }

        //
        // Callbacks
        //
        static void Write(png_structp pngPtr, png_bytep data, png_size_t length)
        {
            ((Writer*)png_get_io_ptr(pngPtr))->Write(data, length);
        }
        static void HandleError(png_structp pngPtr, const char* error)
        {
            // todo: log error and throw
        }
        static void HandleWarning(png_structp pngPtr, const char* warning)
        {
            // todo: log warning and return
        }

        std::vector<char> Store()
        {
            pngPtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, this, HandleError, HandleWarning);
            infoPtr = pngPtr ? png_create_info_struct(pngPtr) : nullptr;
            
            if (infoPtr)
            {
                png_set_write_fn(pngPtr, (png_voidp)this, Write, nullptr);
                png_set_IHDR(
                    pngPtr,
                    infoPtr,
                    image.Width(), 
                    image.Height(),
                    8,
                    PNG_COLOR_TYPE_RGBA,
                    PNG_INTERLACE_NONE,
                    PNG_COMPRESSION_TYPE_DEFAULT,
                    PNG_FILTER_TYPE_DEFAULT
                );
                png_write_info(pngPtr, infoPtr);
                png_write_image(pngPtr, CreateRows().data());
                png_write_end(pngPtr, NULL);
            }
            else
            {
                // todo: out of memory error?
            }

            return res;
        }
    private:
        std::vector<png_bytep> CreateRows() const
        {
            std::vector<png_bytep> rowPtrs;
            rowPtrs.resize(image.Height(), nullptr);
            for (int i = 0; i < image.Height(); i++)
            {
                rowPtrs[i] = image.Row(i);
            }
            return rowPtrs;
        }

        void Write(png_bytep data, png_size_t length)
        {
            size_t index = res.size();
            res.resize(index + length);
            memcpy(res.data() + index, data, length);
        }

        const RGBAImage& image;
        std::vector<char> res;
        png_structp pngPtr;
        png_infop infoPtr;
    };

    return Writer(*this).Store();
}

RGBAImage& RGBAImage::ReadPNG(IBlobReader * reader)
{
    delete[] m_data;
    m_width = 0;
    m_height = 0;
    m_data = nullptr;

    static constexpr int PNGSIGSIZE = 8;

    class PNG
    {
        public:
            PNG(RGBAImage& image)
                : image(image)
                , pngPtr(nullptr)
                , infoPtr(nullptr)
            {}
            ~PNG()
            {
                png_destroy_read_struct(&pngPtr, &infoPtr, nullptr);
            }

            static bool Validate(IBlobReader* reader)
            {
                png_byte pngsig[PNGSIGSIZE];
                return
                    (PNGSIGSIZE == reader->Read(pngsig, PNGSIGSIZE)) &&
                    (0 == png_sig_cmp(pngsig, 0, PNGSIGSIZE));
            }

            //
            // Callbacks
            //
            static void ReadData(png_structp pngPtr, png_bytep data, png_size_t length)
            {
                png_voidp reader = png_get_io_ptr(pngPtr);
                if (length != ((IBlobReader*)reader)->Read(data, length))
                {
                    // todo: report error?
                }
            }
            static void HandleError(png_structp pngPtr, const char* error)
            {
                // todo: log error and throw
            }
            static void HandleWarning(png_structp pngPtr, const char* warning)
            {
                // todo: log warning and return
            }

            RGBAImage& Read(IBlobReader* reader)
            {
                if (!Validate(reader))
                {
                    // todo: report error: not a (valid) png?
                }
                else
                {
                    pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, this, HandleError, HandleWarning);
                    infoPtr = pngPtr ? png_create_info_struct(pngPtr) : nullptr;

                    if (infoPtr)
                    {
                        png_set_read_fn(pngPtr, (png_voidp)reader, ReadData);
                        png_set_sig_bytes(pngPtr, PNGSIGSIZE);
                        png_read_info(pngPtr, infoPtr);
                        ResizeTargetImage();
                        ConvertSourceToRGBA();
                        png_read_image(pngPtr, CreateRows().data());
                    }
                    else
                    {
                        // todo: out of memory error?
                    }
                }
                return image;
            }
    private:
        void ResizeTargetImage()
        {
            image.m_width = png_get_image_width(pngPtr, infoPtr);
            image.m_height = png_get_image_height(pngPtr, infoPtr);
            image.m_data = new unsigned char[4 * image.m_width * image.m_height];
        }
        void ConvertSourceToRGBA()
        {
            // Read any color_type into 8bit depth, RGBA format.
            // See http://www.libpng.org/pub/png/libpng-manual.txt
            png_uint_32 bit_depth = png_get_bit_depth(pngPtr, infoPtr);
            png_uint_32 channels = png_get_channels(pngPtr, infoPtr);
            png_uint_32 color_type = png_get_color_type(pngPtr, infoPtr);
            if (bit_depth == 16)
            {
                png_set_strip_16(pngPtr);
            }
            if (color_type == PNG_COLOR_TYPE_PALETTE)
            {
                png_set_palette_to_rgb(pngPtr);
            }
            if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
            {
                png_set_expand_gray_1_2_4_to_8(pngPtr);
            }
            if (png_get_valid(pngPtr, infoPtr, PNG_INFO_tRNS))
            {
                png_set_tRNS_to_alpha(pngPtr);
            }
            if (color_type == PNG_COLOR_TYPE_RGB ||
                color_type == PNG_COLOR_TYPE_GRAY ||
                color_type == PNG_COLOR_TYPE_PALETTE)
            {
                png_set_filler(pngPtr, 0xFF, PNG_FILLER_AFTER);
            }
            if (color_type == PNG_COLOR_TYPE_GRAY ||
                color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
            {
                png_set_gray_to_rgb(pngPtr);
            }
            png_read_update_info(pngPtr, infoPtr);
        }
        std::vector<png_bytep> CreateRows() const
        {
            std::vector<png_bytep> rowPtrs;
            rowPtrs.resize(image.Height(), nullptr);
            for (int i = 0; i < image.Height(); i++)
            {
                rowPtrs[i] = image.Row(i);
            }
            return rowPtrs;
        }

        RGBAImage& image;
        png_structp pngPtr;
        png_infop infoPtr;
    };

    return PNG(*this).Read(reader);
}


/*
std::vector<char> RGBAImage::StorePNG()
{
    class Writer
    {
    public:
        Writer()
        {
            res.reserve(4096);
        }

        //
        // Callbacks
        //
        static void Write(png_structp pngPtr, png_bytep data, png_size_t length)
        {
            ((Writer*)png_get_io_ptr(pngPtr))->Write(data, length);
        }
        static void HandleError(png_structp pngPtr, const char* error)
        {
            // todo
        }
        static void HandleWarning(png_structp pngPtr, const char* warning)
        {
            // todo
        }

        std::vector<char> res;

    private:
        void Write(png_bytep data, png_size_t length)
        {
            size_t index = res.size();
            res.resize(index + length);
            memcpy(res.data() + index, data, length);
        }
    } writer;


    png_structp pngPtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, &writer, Writer::HandleError, Writer::HandleWarning);
    if (!pngPtr)
    {
        // error
        return writer.res;
    }

    png_infop infoPtr = png_create_info_struct(pngPtr);
    if (!infoPtr)
    {
        // error
        png_destroy_write_struct(&pngPtr, nullptr);
        return writer.res;
    }

    png_set_write_fn(pngPtr, (png_voidp)&writer, Writer::Write, nullptr);

    // Output is 8bit depth, RGBA format.
    png_set_IHDR(
        pngPtr,
        infoPtr,
        m_width, m_height,
        8,
        PNG_COLOR_TYPE_RGBA,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
    );
    png_write_info(pngPtr, infoPtr);

    const int stride = m_width * 4; // RGBA
    std::vector<png_bytep> rowPtrs;
    rowPtrs.resize(m_height, nullptr);
    for (int i = 0; i < m_height; i++)
    {
        png_uint_32 q = (m_height - i - 1) * stride;
        rowPtrs[i] = m_data + q;
    }

    png_write_image(pngPtr, rowPtrs.data());
    png_write_end(pngPtr, NULL);

    png_destroy_write_struct(&pngPtr, &infoPtr);

    return writer.res;
}
*/