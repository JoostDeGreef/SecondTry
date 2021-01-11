#pragma once

#include <string>
#include <stdexcept>
#include <sstream>

namespace ZLib
{
    enum class CompressionLevel
    {
        NO_COMPRESSION        =  0,
        BEST_SPEED            =  1,
        LOW                   =  3,
        MEDIUM                =  5,
        HIGH                  =  7,
        BEST_COMPRESSION      =  9,
        DEFAULT_COMPRESSION   = -1
    };

    std::string CompressString(
        const std::string& str,
        CompressionLevel compressionLevel = CompressionLevel::BEST_COMPRESSION);

    std::string CompressBlob(
        char const * const blob,
        const size_t blobSize,
        CompressionLevel compressionLevel = CompressionLevel::BEST_COMPRESSION);

    std::string DecompressString(const std::string& str);

};
