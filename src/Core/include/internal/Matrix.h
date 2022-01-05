#pragma once

namespace implementation
{
    template<typename DERIVED, typename T, int ROWS, int COLUMNS>
    class TMatrixData
    {
    private:
        std::array<T, ROWS*COLUMNS> m_data;

    public:
        typedef DERIVED this_type;
        typedef T element_type;

    private:
        this_type& Ref()
        {
            return *static_cast<this_type*>(this);
        }
    public:

        TMatrixData()
        {}
        TMatrixData(const this_type& other)
        {
            Copy(other);
        }
        TMatrixData(this_type&& other)
        {
            Swap(other);
        }

        template<typename ...Args>
        TMatrixData(const element_type& e0, Args... args)
        {
            Set(e0, args...);
        }

        this_type& operator = (const this_type& other)
        {
            return Copy(other);
        }
        this_type& operator = (this_type&& other)
        {
            return Swap(other);
        }

        constexpr size_t Elements() const { return ROWS * COLUMNS; }
        constexpr size_t Rows() const { return ROWS; }
        constexpr size_t Columns() const { return COLUMNS; }

        const element_type * const Raw() const { return m_data.data(); }
        element_type * Raw() { return m_data.data(); }
        const auto & Data() const { return m_data; }
        auto & Data() { return m_data; }
        const element_type & Data(const size_t &i) const { return m_data[i]; }
        element_type & Data(const size_t& i) { return m_data[i]; }

        const element_type& operator[](const size_t& element) const
        {
            return m_data[element];
        }
        element_type& operator[](const size_t& element)
        {
            return m_data[element];
        }

        this_type & Copy(const this_type& other)
        {
            m_data = other.m_data;
            return Ref();
        }
        this_type & Swap(this_type&& other)
        {
            other.m_data.swap(m_data);
            return Ref();
        }

    private:
        template<size_t INDEX>
        void _Set()
        {
        }
        template<size_t INDEX,typename ...Args>
        void _Set(const element_type& e0, Args... args)
        {
            m_data[INDEX] = e0;
            _Set<INDEX + 1>(args...);
        }
    public:
        template<typename ...Args>
        void Set(const element_type& e0, Args... args)
        {
            _Set<0>(e0, args...);
        }

    public:
        template<typename ...Args>
        void Fill(const element_type& value)
        {
            m_data.fill(value);
        }

    public:
        bool Equal(const this_type& other) const
        {
            for(size_t i=0;i<ROWS*COLUMNS;++i)
            {
                if(!Numerics::Equal(m_data[i],other.m_data[i]))
                {
                    return false;
                }
            }
            return true;
        }

    private:
        template<typename Functor>
        this_type Zip(const this_type& other, Functor op) const
        {
            this_type res;
            for (size_t i = 0; i < Elements(); ++i)
            {
                res[i] = op(Data(i),other[i]);
            }
            return res;
        }
        template<typename Functor>
        this_type & ZipAssign(const this_type& other, Functor op)
        {
            for (size_t i = 0; i < Elements(); ++i)
            {
                op(Data(i), other[i]);
            }
            return Ref();
        }
    public:

        this_type operator + (const this_type& other) const
        {
            return Zip(other, [](const element_type& a, const element_type& b) {return a + b; });
        }
        this_type& operator += (const this_type& other)
        {
            return ZipAssign(other, [](element_type& a, const element_type& b) {return a += b; });
        }

        this_type operator - (const this_type& other) const
        {
            return Zip(other, [](const element_type& a, const element_type& b) {return a - b; });
        }
        this_type& operator -= (const this_type& other)
        {
            return ZipAssign(other, [](element_type& a, const element_type& b) {return a -= b; });
        }

        this_type operator * (const element_type& value) const
        {
            this_type res;
            for (size_t i = 0; i < Elements(); ++i)
            {
                res[i] = m_data[i] * value;
            }
            return res;
        }
        this_type& operator *= (const element_type& value)
        {
            for (size_t i = 0; i < Elements(); ++i)
            {
                m_data[i] *= value;
            }
            return Ref();
        }

        this_type operator / (const element_type& value) const
        {
            this_type res;
            for (size_t i = 0; i < Elements(); ++i)
            {
                res[i] = m_data[i] / value;
            }
            return res;
        }
        this_type& operator /= (const element_type& value)
        {
            for (size_t i = 0; i < Elements(); ++i)
            {
                m_data[i] /= value;
            }
            return Ref();
        }


    };
}

//
// Statically sized matrix
//

template<typename T,int ROWS,int COLUMNS> 
class TMatrix : public implementation::TMatrixData<TMatrix<T, ROWS, COLUMNS>,T,ROWS,COLUMNS>
{
public:
    using implementation::TMatrixData<TMatrix<T, ROWS, COLUMNS>,T,ROWS,COLUMNS>::TMatrixData;

    const T& operator()(const size_t &row,const size_t &column) const 
    {
        return this->operator[](row*COLUMNS+column);
    }
    T & operator()(const size_t& row, const size_t& column)
    {
        return this->operator[](row* COLUMNS + column);
    }
};

//
// Statically sized vector
//

template<typename T, int FIELDS>
class TVector : public implementation::TMatrixData<TVector<T, FIELDS>, T, FIELDS, 1>
{
public:
    typedef TVector<T, FIELDS> this_type;
    typedef implementation::TMatrixData<this_type, T, FIELDS, 1> base_type;
    typedef T element_type;

public:    
    using implementation::TMatrixData<TVector<T, FIELDS>, T, FIELDS, 1>::TMatrixData;

    size_t Dimension() const { return base_type::Elements(); }

    element_type Length() const
    {
        return sqrt(LengthSquared());
    }
    element_type LengthSquared() const
    {
        element_type res = 0;
        for (const auto& element : base_type::Data())
        {
            res += pow(element, 2);
        }
        return res;
    }

    this_type& Normalize()
    {
        return *this /= Length();
    }
    this_type Normalized() const
    {
        element_type length = Length();
        assert(length > 0.000001);
        return *this / length;
    }

    element_type InnerProduct(const this_type& other) const
    {
        element_type res = 0;
        for (size_t i = 0; i < base_type::Elements(); ++i)
        {
            res += base_type::Data(i) * other.Data(i);
        }
        return res;
    }

    element_type OuterProduct(const this_type& other) const
    {
        this_type res;
        for (size_t i = 0; i < base_type::Elements(); ++i)
        {
            res[i] = base_type::Data(i) * other.Data(i);
        }
        return res;
    }

    template<typename RET = this_type>
    typename std::enable_if<FIELDS == 3, RET>::type Cross(const this_type& other) const
    {
        return this_type(
            base_type::Data(1) * other[2] - base_type::Data(2) * other[1],
            base_type::Data(2) * other[0] - base_type::Data(0) * other[2],
            base_type::Data(0) * other[1] - base_type::Data(1) * other[0]);
    }
};

//
// Commonly used template aliases
//

template<typename T>
using TVector3 = TVector<T, 3>;

using Vector3d = TVector3<double>;
using Vector3f = TVector3<float>;

template<typename T>
using TVector2 = TVector<T, 2>;

using Vector2d = TVector2<double>;
using Vector2f = TVector2<float>;
using Vector2i = TVector2<int>;

//
// Free functions
//

template<typename VALUE_TYPE>
inline TVector3<VALUE_TYPE> CrossProduct(const TVector3<VALUE_TYPE>& a, const TVector3<VALUE_TYPE>& b)
{
    return a.Cross(b);
}

template<typename VALUE_TYPE>
inline VALUE_TYPE ScalarTripleProduct(const TVector3<VALUE_TYPE>& a, const TVector3<VALUE_TYPE>& b, const TVector3<VALUE_TYPE>& c)
{
    return a.InnerProduct(CrossProduct(b, c));
}

template<typename VALUE_TYPE>
inline TVector3<VALUE_TYPE> VectorTripleProduct(const TVector3<VALUE_TYPE>& a, const TVector3<VALUE_TYPE>& b, const TVector3<VALUE_TYPE>& c)
{
    return CrossProduct(a, CrossProduct(b, c));
}
