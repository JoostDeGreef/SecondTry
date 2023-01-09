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
            value_type RGBA[3];
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

        const unsigned int GetInt() const
        {
            return (Value::ToByte(R) << 24)
                 + (Value::ToByte(G) << 16)
                 + (Value::ToByte(B) << 8)
                 + (Value::ToByte(A));
        }
        void SetInt(unsigned int color)
        {
            R = Value::FromByte((color >> 24) & 255);
            G = Value::FromByte((color >> 16) & 255);
            B = Value::FromByte((color >> 8) & 255);
            A = Value::FromByte((color) & 255);
        }

        bool operator == (const this_type& other) const
        {
            return
                R == other.R &&
                G == other.G &&
                B == other.B &&
                A == other.A;
        }

        static           this_type Random() { return this_type(Value::Rnd(), Value::Rnd(), Value::Rnd(), Value::Hi()); }
        static constexpr this_type Black() { return this_type(Value::Lo(), Value::Lo(), Value::Lo(), Value::Hi()); }
        static constexpr this_type Blue() { return this_type(Value::Lo(), Value::Lo(), Value::Hi(), Value::Hi()); }
        static constexpr this_type Green() { return this_type(Value::Lo(), Value::Hi(), Value::Lo(), Value::Hi()); }
        static constexpr this_type Red() { return this_type(Value::Hi(), Value::Lo(), Value::Lo(), Value::Hi()); }
        static constexpr this_type White() { return this_type(Value::Hi(), Value::Hi(), Value::Hi(), Value::Hi()); }
    };

    template<typename VALUE_TYPE> class TRGBAColor;
    using RGBAColord = TRGBAColor<double>;
    using RGBAColorf = TRGBAColor<float>;
    using RGBAColorb = TRGBAColor<unsigned char>;
}

