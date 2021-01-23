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

        const unsigned int GetInt() const
        {
            return (Value::ToByte(R) << 16)
                 + (Value::ToByte(G) << 8)
                 + (Value::ToByte(B));
        }
        void SetInt(unsigned int color)
        {
            R = Value::FromByte((color >> 16) & 255);
            G = Value::FromByte((color >> 8) & 255);
            B = Value::FromByte((color) & 255);
        }

        bool operator == (const this_type& other) const
        {
            return
                R == other.R &&
                G == other.G &&
                B == other.B;
        }

        static           this_type Random() { return this_type(Value::Rnd(), Value::Rnd(), Value::Rnd()); }
        static constexpr this_type Black() { return this_type(Value::Lo(), Value::Lo(), Value::Lo()); }
        static constexpr this_type Blue() { return this_type(Value::Lo(), Value::Lo(), Value::Hi()); }
        static constexpr this_type Green() { return this_type(Value::Lo(), Value::Hi(), Value::Lo()); }
        static constexpr this_type Red() { return this_type(Value::Hi(), Value::Lo(), Value::Lo()); }
        static constexpr this_type White() { return this_type(Value::Hi(), Value::Hi(), Value::Hi()); }
    };

    template<typename VALUE_TYPE> class TRGBColor;
    using RGBColord = TRGBColor<double>;
    using RGBColorf = TRGBColor<float>;
    using RGBColorb = TRGBColor<unsigned char>;
}

