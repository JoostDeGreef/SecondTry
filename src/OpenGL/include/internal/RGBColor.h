#pragma once

#include "ColorValues.h"

namespace OpenGL
{
    template<typename VALUE_TYPE>
    class TRGBColor
    {
    public:
        typedef TRGBColor<VALUE_TYPE> this_type;
        typedef VALUE_TYPE value_type;

    private:
        value_type m_data[3];

    public:
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

        void SetR(const value_type& r) { m_data[0] = r; }
        void SetG(const value_type& g) { m_data[1] = g; }
        void SetB(const value_type& b) { m_data[2] = b; }
        void Set(const value_type& r, const value_type& g, const value_type& b)
        {
            m_data[0] = r;
            m_data[1] = g;
            m_data[2] = b;
        }

        void Copy(const this_type& other)
        {
            m_data[0] = other.m_data[0];
            m_data[1] = other.m_data[1];
            m_data[2] = other.m_data[2];
        }

        this_type Mix(const this_type& other)
        {
            return this_type(
                (value_type)0.5 * (m_data[0] + other.m_data[0]),
                (value_type)0.5 * (m_data[1] + other.m_data[1]),
                (value_type)0.5 * (m_data[2] + other.m_data[2]));
        }

        const value_type& GetR() const { return m_data[0]; }
        const value_type& GetG() const { return m_data[1]; }
        const value_type& GetB() const { return m_data[2]; }
        const value_type* GetRGB() const { return m_data; }

        const unsigned int GetInt() const
        {
            return (Value::ToByte(m_data[0]) << 16)
                + (Value::ToByte(m_data[1]) << 8)
                + (Value::ToByte(m_data[2]));
        }
        void SetInt(unsigned int color)
        {
            m_data[0] = Value::FromByte((color >> 16) & 255);
            m_data[1] = Value::FromByte((color >> 8) & 255);
            m_data[2] = Value::FromByte((color) & 255);
        }

        //void Serialize(Common::Serializer& s) const
        //{
        //    s.Write(GetInt(), 24);
        //}
        //static this_type Deserialize(Common::Serializer& s)
        //{
        //    this_type res;
        //    unsigned int v;
        //    s.Read(v, 24);
        //    res.SetInt(v);
        //    return res;
        //}

        bool operator == (const this_type& other) const
        {
            return
                m_data[0] == other.m_data[0] &&
                m_data[1] == other.m_data[1] &&
                m_data[2] == other.m_data[2];
        }

        static           this_type Random() { return this_type(Value::Rnd(), Value::Rnd(), Value::Rnd()); }
        static constexpr this_type Black() { return this_type(Value::Lo(), Value::Lo(), Value::Lo()); }
        static constexpr this_type Blue() { return this_type(Value::Lo(), Value::Lo(), Value::Hi()); }
        static constexpr this_type Green() { return this_type(Value::Lo(), Value::Hi(), Value::Lo()); }
        static constexpr this_type Red() { return this_type(Value::Hi(), Value::Lo(), Value::Lo()); }
        static constexpr this_type White() { return this_type(Value::Hi(), Value::Hi(), Value::Hi()); }
    private:

        using Value = TColorValue<value_type>;
    };

    template<typename VALUE_TYPE> class TRGBColor;
    using RGBColord = TRGBColor<double>;
    using RGBColorf = TRGBColor<float>;
    using RGBColorb = TRGBColor<unsigned char>;
}

