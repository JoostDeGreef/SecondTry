#pragma once

#include <stdalign.h>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <new>

namespace VectorOperations
{
  // res[:] = a[:] + b[:]
  void Add(double * res, const double * const a, const double * const b, const size_t length);
  void Add(float * res, const float * const a, const float * const b, const size_t length);
  template<typename T>
  void Add(T * res, const T * const a, const T * const b, const size_t length)
  {
    for(size_t i=0;i<length;++i)
    {
      res[i] = a[i] + b[i];
    }
  }
  // res[:] = a[:] + s
  void Add(double * res, const double * const a, const double & s, const size_t length);
  void Add(float * res, const float * const a, const float & s, const size_t length);
  template<typename T>
  void Add(T * res, const T * const a, const T & s, const size_t length)
  {
    for(size_t i=0;i<length;++i)
    {
      res[i] = a[i] + s;
    }
  }

  // res[:] = a[:] - b[:]
  void Subtract(double * res, const double * const a, const double * const b, const size_t length);
  void Subtract(float * res, const float * const a, const float * const b, const size_t length);
  template<typename T>
  void Subtract(T * res, const T * const a, const T * const b, const size_t length)
  {
    for(size_t i=0;i<length;++i)
    {
      res[i] = a[i] - b[i];
    }
  }
  // res[:] = a[:] - s
  void Subtract(double * res, const double * const a, const double & s, const size_t length);
  void Subtract(float * res, const float * const a, const float & s, const size_t length);
  template<typename T>
  void Subtract(T * res, const T * const a, const T & s, const size_t length)
  {
    for(size_t i=0;i<length;++i)
    {
      res[i] = a[i] - s;
    }
  }

  // res[:] += a * b[:]
  void ScalarMultiplyAdd(double * res, const double a, const double * const b, const size_t length);
  void ScalarMultiplyAdd(float * res, const float a, const float * const b, const size_t length);
  template<typename T>
  void ScalarMultiplyAdd(T * res, const T a, const T * const b, const size_t length)
  {
    for(size_t i = 0; i<length; ++i)
    {
      res[i] += a * b[i];
    }
  }

  // res[:] = a[:] * s
  // void Multiply(double * res, const double * const a, const double & s, const size_t length);
  // void Multiply(float * res, const float * const a, const float & s, const size_t length);
  template<typename T>
  void Multiply(T * res, const T * const a, const T & s, const size_t length)
  {
    for(size_t i=0;i<length;++i)
    {
      res[i] = a[i] * s;
    }
  }

  // res[:] = value
  void Fill(double * res, const size_t length, const double value);
  void Fill(float * res, const size_t length, const float value);
  template<typename T>
  void Fill(T * res, const size_t length, const T value)
  {
    for(size_t i=0;i<length;++i)
    {
      res[i] = value;
    }
  }

  // dst[:] = src[:]
  template<typename T>
  void Copy(T * dst, const T * const src, const size_t length)
  {
    for(size_t i=0;i<length;++i)
    {
      dst[i] = src[i];
    }
  }
};

template<typename T>
class Matrix
{
public:
  typedef T FieldType;
  typedef Matrix<T> MatrixType;

private:
  static constexpr size_t RequiredAlignment = 32; // AVX2 -> 32 byte alignment
  static constexpr size_t RoundColumns = RequiredAlignment/sizeof(FieldType);

  class Data
  {
  public:
    Data(const size_t size)
    {
        size_t dataSize = sizeof(Shared) + sizeof(T)*size - sizeof(T);
        m_shared = (Shared *)new (std::align_val_t(RequiredAlignment)) unsigned char [dataSize];
        m_shared->m_count = 1; 
        m_shared->m_size = size;
    }
    Data(const Data & data)
    {
        m_shared = data.m_shared;
        ++m_shared->m_count;
    }
    Data(Data && data)
    {
        m_shared = data.m_shared;
        data.m_shared = nullptr;
    }
    ~Data()
    {
      DecreaseRefCount();
    }    
    Data & operator = (const Data & data)
    {
      DecreaseRefCount();
      m_shared = data.m_shared;
      ++m_shared->m_count;
      return *this;
    }
    Data & operator = (Data && data)
    {
      std::swap(data.m_shared,m_shared);
      return *this;
    }

    T* Get() 
    {
        return &(m_shared->m_data);
    }

    int RefCount() const
    {
        return m_shared->m_count;
    }

  protected:
    void DecreaseRefCount()
    {
        if(m_shared && m_shared->m_count-- <= 0)
        {
            delete m_shared;
        }
    }
  private:
    struct Shared
    {
      size_t m_size;
      int m_count;
      alignas(RequiredAlignment) T m_data;
    } * m_shared;
  };

public:
  Matrix(const size_t rows, const size_t columns)
    : m_rows(rows)
    , m_columns(columns)
    , m_stride(((columns+RoundColumns-1)/RoundColumns)*RoundColumns)
    , m_data(m_rows*m_stride)
    , m_fields(new T* [rows])
  {
    for(size_t i = 0; i < m_rows; ++i)
    {
      m_fields[i] = m_data.Get() + i*m_stride;
    }
  }
  Matrix(const size_t rows, const size_t columns, const T value)
    : Matrix(rows,columns)
  {
    Fill(value);
  }
  Matrix(const MatrixType & other)
    : m_rows(other.m_rows)
    , m_columns(other.m_columns)
    , m_stride(other.m_stride)
    , m_data(other.m_data)
    , m_fields(new T* [other.m_rows])
  {
    for(size_t i = 0; i < m_rows; ++i)
    {
      m_fields[i] = other.m_fields[i];
    }
  }
  Matrix(MatrixType && other)
    : m_rows(other.m_rows)
    , m_columns(other.m_columns)
    , m_stride(other.m_stride)
    , m_data(other.m_data)
    , m_fields(other.m_fields)
  {
    other.m_fields = nullptr;
  }
  ~Matrix()
  {
    delete [] m_fields;
  }
 
  size_t Rows() const
  {
    return m_rows;
  }
  size_t Columns() const
  {
    return m_columns;
  }

  class Proxy
  {
    protected:
      friend MatrixType;
      Proxy(MatrixType & matrix,const size_t & row, const size_t & column)
        : m_matrix(matrix)
        , m_row(row)
        , m_column(column)
      {}
    private:
      MatrixType & m_matrix;
      const size_t & m_row;
      const size_t & m_column;
    public:
      operator T () const
      {
        return m_matrix.Get(m_row,m_column);
      }
      const T & operator = (const T & value)
      {
        m_matrix.Set(m_row,m_column,value);
        return value;
      }
  };
  const T operator () (const size_t & row, const size_t & column) const
  {
    return Get(row,column);
  }
  Proxy operator () (const size_t & row, const size_t & column)
  {
    return Proxy(*this,row,column);
  }
  const T Get(const size_t & row, const size_t & column) const
  {
    assert(row<m_rows && column<m_columns);
    return m_fields[row][column];
  }
  void Set(const size_t & row, const size_t & column, const T & value)
  {
    assert(row<m_rows && column<m_columns);
    MakeUnique();
    m_fields[row][column] = value;
  }

  MatrixType operator + (const MatrixType & other) const
  {
    return Sum(other);
  }
  MatrixType & operator += (const MatrixType & other)
  {
    return Add(other);
  }
  template<typename S>
  MatrixType operator + (const S & scalar) const
  {
    return Sum(scalar);
  }
  template<typename S>
  MatrixType & operator += (const S & scalar)
  {
    return Add(scalar);
  }

  MatrixType operator - (const MatrixType & other) const
  {
    return Minus(other);
  }
  MatrixType & operator -= (const MatrixType & other)
  {
    return Subtract(other);
  }
  template<typename S>
  MatrixType operator - (const S & scalar) const
  {
    return Minus(scalar);
  }
  template<typename S>
  MatrixType & operator -= (const S & scalar)
  {
    return Subtract(scalar);
  }

  MatrixType operator * (const MatrixType & other) const
  {
    return Multiply(other);
  }
  MatrixType & operator *= (const MatrixType & other) const
  {
    auto temp = Multiply(other);
    return (*this = temp);
  }
  template<typename S>
  MatrixType operator * (const S & scalar) const
  {
    return Multiply(scalar);
  }
  template<typename S>
  MatrixType & operator *= (const S & scalar)
  {
    return Multiplied(scalar);
  }

  MatrixType & Fill(const T value)
  {
    MakeUnique();
    for(size_t i = 0; i < m_rows; ++i)
    {
      VectorOperations::Fill(m_fields[i],m_columns,value);
    }
    return *this;
  }

  MatrixType & Add(const MatrixType & other)
  {
    assert(m_rows == other.m_rows && m_columns == other.m_columns);
    MakeUnique();
    for(size_t i = 0; i < m_rows; ++i)
    {
      VectorOperations::Add(m_fields[i],m_fields[i],other.m_fields[i],m_columns);
    }
    return *this;
  }
  MatrixType Sum(const MatrixType & other) const
  {
    assert(m_rows == other.m_rows && m_columns == other.m_columns);
    MatrixType res(m_rows,m_columns);
    for(size_t i = 0; i < m_rows; ++i)
    {
      VectorOperations::Add(res.m_fields[i],m_fields[i],other.m_fields[i],m_columns);
    }
    return res;
  }
  template<typename S>
  MatrixType & Add(const S & scalar)
  {
    MakeUnique();
    for(size_t i = 0; i < m_rows; ++i)
    {
      VectorOperations::Add(m_fields[i],m_fields[i],scalar,m_columns);
    }
    return *this;
  }
  template<typename S>
  MatrixType Sum(const S & scalar) const
  {
    MatrixType res(m_rows,m_columns,scalar);
    res += *this;
    return res;
  }

  MatrixType & Subtract(const MatrixType & other)
  {
    assert(m_rows == other.m_rows && m_columns == other.m_columns);
    MakeUnique();
    for(size_t i = 0; i < m_rows; ++i)
    {
      VectorOperations::Subtract(m_fields[i],m_fields[i],other.m_fields[i],m_columns);
    }
    return *this;
  }
  MatrixType Minus(const MatrixType & other) const
  {
    assert(m_rows == other.m_rows && m_columns == other.m_columns);
    MatrixType res(m_rows,m_columns);
    for(size_t i = 0; i < m_rows; ++i)
    {
      VectorOperations::Subtract(res.m_fields[i],m_fields[i],other.m_fields[i],m_columns);
    }
    return res;
  }
  template<typename S>
  MatrixType & Subtract(const S & scalar)
  {
    MakeUnique();
    for(size_t i = 0; i < m_rows; ++i)
    {
      VectorOperations::Subtract(m_fields[i],m_fields[i],scalar,m_columns);
    }
    return *this;
  }
  template<typename S>
  MatrixType Minus(const S & scalar) const
  {
    MatrixType res(*this);
    return res.Subtract(scalar);
  }

  MatrixType Multiply(const MatrixType & other) const
  {
    assert(m_columns == other.m_rows);
    MatrixType res(m_rows, other.m_columns, T(0));
    for(size_t i = 0; i < m_rows; ++i)
    {
      for(size_t j = 0; j < m_columns; ++j)
      {
         VectorOperations::ScalarMultiplyAdd(res.m_fields[i], m_fields[i][j], other.m_fields[j], other.m_columns);
      }
    }
    return res;
  }
  template<typename S>
  MatrixType Multiply(const S & scalar) const
  {
    MatrixType res(m_rows, m_columns);
    for(size_t i = 0; i < m_rows; ++i)
    {
      VectorOperations::Multiply(res.m_fields[i], m_fields[i], scalar, m_columns);
    }
    return res;
  }
  template<typename S>
  MatrixType & Multiplied(const S & scalar)
  {
    for(size_t i = 0; i < m_rows; ++i)
    {
      VectorOperations::Multiply(m_fields[i], m_fields[i], scalar, m_columns);
    }
    return *this;
  }

  MatrixType Transposed() const
  {
    MatrixType res(m_columns, m_rows);
    for(size_t i = 0; i < m_rows; ++i)
    {
      for(size_t j = 0; j < m_columns; ++j)
      {
        res.m_fields[i][j] = m_fields[j][i];
      }
    }
    return res;
  }
  MatrixType & Transpose()
  {
    return (*this = Transposed());
  }

  T Determinant() const
  {
    assert(m_columns == m_rows);
    switch(m_rows)
    {
      default:
        {
          T factor(1);
          MatrixType part(*this);
          part.m_rows--;
          part.m_columns--;
          for(size_t r=1;r<m_rows;++r)
          {
            part.m_fields[r-1] = m_fields[r] + 1;
          }
          T res = factor * m_fields[0][0] * part.Determinant();
          for(size_t r=1;r<m_rows;++r)
          {
            factor *= -1;
            part.m_fields[r-1] = m_fields[r-1] + 1;
            res += factor * m_fields[r][0] * part.Determinant();
          }
          return res;
        }
      case 3:
        return m_fields[0][0] * (m_fields[1][1]*m_fields[2][2] - m_fields[1][2]*m_fields[2][1])
             - m_fields[0][1] * (m_fields[1][0]*m_fields[2][2] - m_fields[1][2]*m_fields[2][0])
             + m_fields[0][2] * (m_fields[1][0]*m_fields[2][1] - m_fields[1][1]*m_fields[2][0]);
      case 2:
        return m_fields[0][0]*m_fields[1][1] - m_fields[0][1]*m_fields[1][0];
      case 1:
        return m_fields[0][0];
      case 0:
        return (T)0/(T)0;
    }
  }

private:
  MatrixType AdjointCore(const bool sign_minors,const bool transpose) const
  {
    assert(m_columns == m_rows);
    MatrixType res(m_rows,m_columns);
    switch(m_columns)
    {
      case 0:
        break;
      case 1: 
        res(0,0) = Get(0,0);
        break;
      case 2: 
        res(0,0) = Get(1,1);
        if(sign_minors)
        {
          if(transpose)
          {
            res(0,1) = -Get(0,1);
            res(1,0) = -Get(1,0);
          }
          else
          {
            res(1,0) = -Get(0,1);
            res(0,1) = -Get(1,0);
          }
        }
        else
        { 
          if(transpose)
          {
            res(0,1) = Get(0,1);
            res(1,0) = Get(1,0);
          }
          else
          {
            res(1,0) = Get(0,1);
            res(0,1) = Get(1,0);
          }
        }
        res(1,1) = Get(0,0);
        break;
      default:
        {
          MatrixType part = UniqueCopy();
          part.m_rows--;
          part.m_columns--;
          T sign = (m_columns && 1)?-1:1;
          for(int c=m_columns-1;c>=0;--c)
          {
            for(int r=m_rows-1;r>=0;--r)
            {
              if(sign_minors)
              {
                if(transpose)
                {
                  res.m_fields[c][r] = sign * part.Determinant();
                }
                else
                {
                  res.m_fields[r][c] = sign * part.Determinant();
                }
                sign = -sign;
              }
              else
              {
                if(transpose)
                {
                  res.m_fields[c][r] = part.Determinant();
                }
                else
                {
                  res.m_fields[r][c] = part.Determinant();
                }
              }
              if(r!=0)
              {
                std::swap(part.m_fields[r-1],part.m_fields[m_rows-1]);
              }
              else
              {
                for(size_t i = 0; i < m_rows; ++i)
                {
                  part.m_fields[i] = part.m_data.Get() + i*part.m_stride;
                }
              }
            }
            if(c!=0)
            {
              for(size_t r = 0; r < m_rows; ++r)
              {
                std::swap(part.m_fields[r][c-1],part.m_fields[r][m_columns-1]);
              }
            }
          }
        }
    }
    return res;
  }
public:  
  MatrixType Cofactor() const
  {
    MatrixType res = AdjointCore(true,false);
    return res;
  }
  MatrixType Minor() const
  {
    MatrixType res = AdjointCore(false,false);
    return res;
  }
  MatrixType Adjoint() const
  {
    MatrixType res = AdjointCore(false,true);
    return res;
  }
  MatrixType Inverse() const
  {
    MatrixType res = Adjoint();
    T det = res(0,0) * Get(0,0);
    T sign = 1;
    for(size_t c = 1;c<m_columns;++c)
    {
      sign = -sign;
      det += sign * res(c,0) * Get(0,c);
    }
    return (res *= (1/det));
  }

  friend std::ostream& operator<<(std::ostream& out, const MatrixType & matrix)
  {
    for(size_t i = 0; i < matrix.m_rows; ++i)
    {
      out << ((i)?";":"[");
      for(size_t j = 0; j < matrix.m_columns; ++j)
      {
        if(j) out << ",";
        out << matrix.m_fields[i][j];
      }
    }
    out << "]";
    return out;
  }
protected:
    void MakeUnique()
    {
      if(m_data.RefCount()>1)
      {
        Data data(m_rows*m_stride);
        T** fields = new T* [m_rows];
        for(size_t i = 0; i < m_rows; ++i)
        {
          fields[i] = data.Get() + i*m_stride;
        }
        for(size_t i = 0; i < m_rows; ++i)
        {
          for(size_t j = 0; j < m_columns; ++j)
          {
            fields[i][j] = m_fields[i][j];
          }
        }
        m_data = data;
        delete [] m_fields;
        m_fields = fields;
      }
    }
    MatrixType UniqueCopy() const
    {
      MatrixType res(m_rows,m_columns);
      for(size_t i = 0; i < res.m_rows; ++i)
      {
        VectorOperations::Copy(res.m_fields[i],m_fields[i],res.m_stride);
      }
      return res;
    }
private:
  size_t m_rows;
  size_t m_columns;
  const size_t m_stride;
  Data m_data;
  T** m_fields;
};

