#include "zlib.h"
#include "ZLibWrapper.h"
#include "fmt/core.h"

#define BUFFER_SIZE 16384

std::string ZLib::CompressBlob(
    char const * const blob,
    const size_t blobSize,
    CompressionLevel compressionLevel)
{
    z_stream zs;
    memset(&zs, 0, sizeof(zs));

    if (deflateInit(&zs, static_cast<int>(compressionLevel)) != Z_OK)
    {
        throw(std::runtime_error("deflateInit failed while compressing."));
    }

    zs.next_in = (Bytef*)blob;
    zs.avail_in = (int)blobSize; 

    int ret;
    char outbuffer[BUFFER_SIZE];
    std::string outstring;

    do
    {
        zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
        zs.avail_out = (unsigned int)sizeof(outbuffer);

        ret = deflate(&zs, Z_FINISH);

        if (outstring.size() < zs.total_out)
        {
            outstring.append(outbuffer, zs.total_out - outstring.size());
        }
    } while (ret == Z_OK);

    deflateEnd(&zs);

    if (ret != Z_STREAM_END)
    {
        throw(std::runtime_error(fmt::format("Exception during zlib compression: ({0}) {1} ",ret,zs.msg) ));
    }

    return outstring;
}

std::string ZLib::CompressString(
    const std::string& str,
    CompressionLevel compressionLevel)
{
    return CompressBlob(str.data(),str.size(),compressionLevel);
}

std::string ZLib::DecompressString(const std::string& str)
{
    z_stream zs;
    memset(&zs, 0, sizeof(zs));

    if (inflateInit(&zs) != Z_OK)
    {
        throw(std::runtime_error("inflateInit failed while decompressing."));
    }

    zs.next_in = (Bytef*)str.data();
    zs.avail_in = (unsigned int)str.size();

    int ret;
    char outbuffer[BUFFER_SIZE];
    std::string outstring;

    do 
    {
        zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        ret = inflate(&zs, 0);

        if (outstring.size() < zs.total_out) 
        {
            outstring.append(outbuffer, zs.total_out - outstring.size());
        }

    } while (ret == Z_OK);

    inflateEnd(&zs);

    if (ret != Z_STREAM_END) 
    {
        throw(std::runtime_error(fmt::format("Exception during zlib decompression: ({0}) {1}", ret, zs.msg)));
    }

    return outstring;
}
