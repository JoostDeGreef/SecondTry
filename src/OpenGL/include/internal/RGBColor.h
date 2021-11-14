#pragma once

namespace OpenGL
{
    template<typename VALUE_TYPE>
    class TRGBColor
    {
    public:
        typedef TRGBColor<VALUE_TYPE> this_type;
        typedef VALUE_TYPE value_type;

    public:
        union
        {
            value_type RGB[3];
            struct
            {
                value_type R;
                value_type G;
                value_type B;
            };
        };

        TRGBColor()
        {}
        TRGBColor(const this_type& other)
        {
            Copy(other);
        }
        TRGBColor(const value_type& r, const value_type& g, const value_type& b)
        {
            Set(r, g, b);
        }
        TRGBColor(const unsigned int color)
        {
            SetInt(color);
        }

        void Set(const value_type& r, const value_type& g, const value_type& b)
        {
            R = r;
            G = g;
            B = b;
        }

        void Copy(const this_type& other)
        {
            R = other.R;
            G = other.G;
            B = other.B;
        }

        this_type Mix(const this_type& other)
        {
            return this_type(
                (value_type)0.5 * (R + other.R),
                (value_type)0.5 * (G + other.G),
                (value_type)0.5 * (B + other.B));
        }

    private:
        unsigned int ToByte(const double& value) { return Numerics::Clamp((unsigned int)(value*255),0u,255u); }
        unsigned int ToByte(const float& value) { return Numerics::Clamp((unsigned int)(value*255),0u,255u); }
        unsigned int ToByte(const unsigned char& value) { return value; }

        void FromByte(const unsigned int value, double& res) { res = value/255.0; }
        void FromByte(const unsigned int value, float& res) { res = value/255.0; }
        void FromByte(const unsigned int value, unsigned char& res) { res = value; }
        value_type FromByte(const unsigned int value) { value_type res; FromByte(value, res); return res; }
    public:
        const unsigned int GetInt() const
        {
            return (ToByte(R) << 16)
                 + (ToByte(G) << 8)
                 + (ToByte(B));
        }
        this_type& SetInt(unsigned int color)
        {
            R = FromByte((color >> 16) & 255);
            G = FromByte((color >> 8) & 255);
            B = FromByte((color) & 255);
            return *this;
        }

        bool operator == (const this_type& other) const
        {
            return
                R == other.R &&
                G == other.G &&
                B == other.B;
        }

        static           this_type Random() { return SetInt(Numerics::NormalizedRandomNumber(0xFFFFFFu)); }
        static constexpr this_type Black() { return SetInt(0); }
        static constexpr this_type Blue() { return SetInt(0x0000FF); }
        static constexpr this_type Green() { return SetInt(0x00FF00F); }
        static constexpr this_type Red() { return SetInt(0xFF0000); }
        static constexpr this_type White() { return SetInt(0xFFFFFF); }
    };

    template<typename VALUE_TYPE> class TRGBColor;
    using RGBColord = TRGBColor<double>;
    using RGBColorf = TRGBColor<float>;
    using RGBColorb = TRGBColor<unsigned char>;
}

