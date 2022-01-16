#pragma once

namespace OpenGL
{
    template<typename VALUE_TYPE>
    class TRGBAColor
    {
    public:
        typedef TRGBAColor<VALUE_TYPE> this_type;
        typedef VALUE_TYPE value_type;

    public:
        union
        {
            value_type RGBA[4];
            struct
            {
                value_type R;
                value_type G;
                value_type B;
                value_type A;
            };
        };

        TRGBAColor()
        {}
        TRGBAColor(const this_type& other)
        {
            Copy(other);
        }
        TRGBAColor(const value_type& r, const value_type& g, const value_type& b, const value_type& a)
        {
            Set(r, g, b, a);
        }
        TRGBAColor(const unsigned int color)
        {
            SetInt(color);
        }

        void Set(const value_type& r, const value_type& g, const value_type& b, const value_type& a)
        {
            R = r;
            G = g;
            B = b;
            A = a;
        }

        void Copy(const this_type& other)
        {
            R = other.R;
            G = other.G;
            B = other.B;
            A = other.A;
        }

        this_type Mix(const this_type& other)
        {
            return this_type(
                (value_type)0.5 * (R + other.R),
                (value_type)0.5 * (G + other.G),
                (value_type)0.5 * (B + other.B),
                (value_type)0.5 * (A + other.A));
        }

    private:
        unsigned int ToByte(const double& value) { return Core::Numerics::Clamp((unsigned int)(value*255),0u,255u); }
        unsigned int ToByte(const float& value) { return Core::Numerics::Clamp((unsigned int)(value*255),0u,255u); }
        unsigned int ToByte(const unsigned char& value) { return value; }

        void FromByte(const unsigned int value, double& res) { res = value/255.0; }
        void FromByte(const unsigned int value, float& res) { res = value/255.0; }
        void FromByte(const unsigned int value, unsigned char& res) { res = value; }
        value_type FromByte(const unsigned int value) { value_type res; FromByte(value, res); return res; }
    public:
        const unsigned int GetInt() const
        {
            return (ToByte(R) << 24)
                 + (ToByte(G) << 16)
                 + (ToByte(B) << 8)
                 + (ToByte(A));
        }
        this_type& SetInt(unsigned int color)
        {
            R = FromByte((color >> 24) & 255);
            G = FromByte((color >> 16) & 255);
            B = FromByte((color >> 8) & 255);
            A = FromByte((color) & 255);
            return *this;
        }

        bool operator == (const this_type& other) const
        {
            return
                R == other.R &&
                G == other.G &&
                B == other.B &&
                A == other.A;
        }

        static           this_type Random() { return SetInt((Core::Numerics::NormalizedRandomNumber(0xFFFFFFu)<<8) + 0xFF); }
        static constexpr this_type Black() { return SetInt(0); }
        static constexpr this_type Blue() { return SetInt(0x0000FFFF); }
        static constexpr this_type Green() { return SetInt(0x00FF00FFF); }
        static constexpr this_type Red() { return SetInt(0xFF0000FF); }
        static constexpr this_type White() { return SetInt(0xFFFFFFFF); }
    };

    template<typename VALUE_TYPE> class TRGBAColor;
    using RGBAColord = TRGBAColor<double>;
    using RGBAColorf = TRGBAColor<float>;
    using RGBAColorb = TRGBAColor<unsigned char>;
}

