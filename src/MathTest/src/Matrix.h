#pragma once

//#include <stdalign.h>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <new>
#include <initializer_list>

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
  void Multiply(double * res, const double * const a, const double & s, const size_t length);
  void Multiply(float * res, const float * const a, const float & s, const size_t length);
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
        // This would be correct, but VS has a bug which prevents the usage:
        // m_shared = (Shared *) new (std::align_val_t(RequiredAlignment)) unsigned char [dataSize];
        m_shared = static_cast<Shared *>(operator new[](dataSize, std::align_val_t{ RequiredAlignment }));
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
            // call aligned delete directly to work around a VS 'feature'.
            // delete [] (unsigned char *)m_shared;
            operator delete[](m_shared, std::align_val_t{ RequiredAlignment });
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

  Matrix(const MatrixType & other, const size_t row, const size_t column, const size_t rows, const size_t columns)
    : m_rows(rows)
    , m_columns(columns)
    , m_stride(other.m_stride)
    , m_data(other.m_data)
    , m_fields(new T* [rows])
  {
    for(size_t i = 0; i < m_rows; ++i)
    {
      m_fields[i] = &other.m_fields[row][column] + i*m_stride;
    }
  }
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
  Matrix(const size_t rows, const size_t columns, const std::initializer_list<T> & l)
    : Matrix(rows,columns)
  {
    Set(l);
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
 
  MatrixType & operator = (const MatrixType & other)
  {
    m_data = other.m_data;
    m_rows = other.m_rows;
    m_stride = other.m_stride;
    m_columns = other.m_columns;
    delete [] m_fields;
    m_fields = new T* [m_rows];
    for(size_t i = 0; i < m_rows; ++i)
    {
      m_fields[i] = other.m_fields[i];
    }

    return *this;
  }
  MatrixType & operator = (MatrixType && other)
  {
    std::swap(m_data,other.m_data);
    std::swap(m_rows,other.m_rows);
    std::swap(m_stride,other.m_stride);
    std::swap(m_columns,other.m_columns);
    std::swap(m_fields,other.m_fields);
    return *this;
  }
  MatrixType & operator = (const std::initializer_list<T> & l)
  {
    Set(l);
    return *this;
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
  template<typename... ARGS>
  void Set(const std::initializer_list<T> & l)
  {
    assert(l.size() == m_rows*m_columns);
    auto iter = l.begin();
    for(size_t i = 0; i < m_rows; ++i)
    {
      for(size_t j = 0; j < m_columns; ++j, ++iter)
      {
        m_fields[i][j] = *iter;
      }
    }
  }

  MatrixType & Identity()
  {
    Fill((T)0);
    for(size_t i = 0; i < m_rows && i < m_columns; ++i)
    {
      m_fields[i][i] = (T)1;
    }
    return *this;
  }

  MatrixType operator + (const MatrixType & other) const
  {
    return Sum(other);
  }
  MatrixType & operator += (const MatrixType & other)
  {
    return Add(other);
  }
  MatrixType operator + (const FieldType & scalar) const
  {
    return Sum(scalar);
  }
  MatrixType & operator += (const FieldType & scalar)
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
  MatrixType operator - (const FieldType & scalar) const
  {
    return Minus(scalar);
  }
  MatrixType & operator -= (const FieldType & scalar)
  {
    return Subtract(scalar);
  }

  MatrixType operator * (const MatrixType & other) const
  {
    return Multiply(other);
  }
  MatrixType & operator *= (const MatrixType & other) 
  {
    auto temp = Multiply(other);
    return (*this = temp);
  }
  MatrixType operator * (const FieldType & scalar) const
  {
    return Multiply(scalar);
  }
  MatrixType & operator *= (const FieldType & scalar)
  {
    return Multiplied(scalar);
  }

  MatrixType operator / (const FieldType & scalar) const
  {
    return Multiply(((T)1)/scalar);
  }
  MatrixType & operator /= (const FieldType & scalar)
  {
    return Multiplied(((T)1)/scalar);
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

  bool operator == (const MatrixType & other) const
  {
    assert(m_rows == other.m_rows && m_columns == other.m_columns);
    for(size_t i = 0; i < m_rows; ++i)
    {
      for(size_t j = 0; j < m_columns; ++j)
      {
        if(m_fields[i][j] != other.m_fields[i][j])
        {
          return false;
        }
      }
    }
    return true;
  }
  bool operator != (const MatrixType & other) const
  {
    assert(m_rows == other.m_rows && m_columns == other.m_columns);
    for(size_t i = 0; i < m_rows; ++i)
    {
      for(size_t j = 0; j < m_columns; ++j)
      {
        if(m_fields[i][j] != other.m_fields[i][j])
        {
          return true;
        }
      }
    }
    return false;
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
  MatrixType & Add(const FieldType & scalar)
  {
    MakeUnique();
    for(size_t i = 0; i < m_rows; ++i)
    {
      VectorOperations::Add(m_fields[i],m_fields[i],scalar,m_columns);
    }
    return *this;
  }
  MatrixType Sum(const FieldType & scalar) const
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
  MatrixType & Subtract(const FieldType & scalar)
  {
    MakeUnique();
    for(size_t i = 0; i < m_rows; ++i)
    {
      VectorOperations::Subtract(m_fields[i],m_fields[i],scalar,m_columns);
    }
    return *this;
  }
  MatrixType Minus(const FieldType & scalar) const
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
  MatrixType Multiply(const FieldType & scalar) const
  {
    MatrixType res(m_rows, m_columns);
    for(size_t i = 0; i < m_rows; ++i)
    {
      VectorOperations::Multiply(res.m_fields[i], m_fields[i], scalar, m_columns);
    }
    return res;
  }
  MatrixType & Multiplied(const FieldType & scalar)
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
        res.m_fields[j][i] = m_fields[i][j];
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
        return (T)0; // throw?
    }
  }

  MatrixType Adjoint() const
  {
    assert(m_columns == m_rows);
    MatrixType res(m_rows,m_columns);
    switch(m_columns)
    {
      case 0:
        break;
      case 1: 
        res.m_fields[0][0] = (T)1;
        break;
      case 2: 
        res.m_fields[0][0] =  Get(1,1);
        res.m_fields[0][1] = -Get(0,1);
        res.m_fields[1][0] = -Get(1,0);
        res.m_fields[1][1] =  Get(0,0);
        break;
      case 3: 
        res.m_fields[0][0] = Get(1,1)*Get(2,2) - Get(1,2)*Get(2,1);
        res.m_fields[1][0] = Get(1,2)*Get(2,0) - Get(1,0)*Get(2,2);
        res.m_fields[2][0] = Get(1,0)*Get(2,1) - Get(1,1)*Get(2,0);
        res.m_fields[0][1] = Get(0,2)*Get(2,1) - Get(0,1)*Get(2,2);
        res.m_fields[1][1] = Get(0,0)*Get(2,2) - Get(0,2)*Get(2,0);
        res.m_fields[2][1] = Get(0,1)*Get(2,0) - Get(0,0)*Get(2,1);
        res.m_fields[0][2] = Get(0,1)*Get(1,2) - Get(0,2)*Get(1,1);
        res.m_fields[1][2] = Get(0,2)*Get(1,0) - Get(0,0)*Get(1,2);
        res.m_fields[2][2] = Get(0,0)*Get(1,1) - Get(0,1)*Get(1,0);
        break;
      case 4:
        {
          T a = Get(0,0), b = Get(0,1), c = Get(0,2), d = Get(0,3);
          T e = Get(1,0), f = Get(1,1), g = Get(1,2), h = Get(1,3);
          T i = Get(2,0), j = Get(2,1), k = Get(2,2), l = Get(2,3);
          T m = Get(3,0), n = Get(3,1), o = Get(3,2), p = Get(3,3);
          res.m_fields[0][0] =  f*(k*p-l*o) + g*(l*n-j*p) + h*(j*o-k*n);
          res.m_fields[1][0] =  e*(l*o-k*p) + g*(i*p-l*m) + h*(k*m-i*o);
          res.m_fields[2][0] =  e*(j*p-l*n) + f*(l*m-i*p) + h*(i*n-j*m);
          res.m_fields[3][0] =  e*(k*n-j*o) + f*(i*o-k*m) + g*(j*m-i*n);
          res.m_fields[0][1] =  b*(l*o-k*p) + c*(j*p-l*n) + d*(k*n-j*o);
          res.m_fields[1][1] =  a*(k*p-l*o) + c*(l*m-i*p) + d*(i*o-k*m);
          res.m_fields[2][1] =  a*(l*n-j*p) + b*(i*p-l*m) + d*(j*m-i*n);
          res.m_fields[3][1] =  a*(j*o-k*n) + b*(k*m-i*o) + c*(i*n-j*m);
          res.m_fields[0][2] =  b*(g*p-h*o) + c*(h*n-f*p) + d*(f*o-g*n);
          res.m_fields[1][2] =  a*(h*o-g*p) + c*(e*p-h*m) + d*(g*m-e*o);
          res.m_fields[2][2] =  a*(f*p-h*n) + b*(h*m-e*p) + d*(e*n-f*m);
          res.m_fields[3][2] =  a*(g*n-f*o) + b*(e*o-g*m) + c*(f*m-e*n);
          res.m_fields[0][3] =  b*(h*k-g*l) + c*(f*l-h*j) + d*(g*j-f*k);
          res.m_fields[1][3] =  a*(g*l-h*k) + c*(h*i-e*l) + d*(e*k-g*i);
          res.m_fields[2][3] =  a*(h*j-f*l) + b*(e*l-h*i) + d*(f*i-e*j);
          res.m_fields[3][3] =  a*(f*k-g*j) + b*(g*i-e*k) + c*(e*j-f*i);
        }
        break;
      default:
        {
          MatrixType part = UniqueCopy();
          part.m_rows--;
          part.m_columns--;
          for(int c=m_columns-1;c>=0;--c)
          {
            T sign = (c & 1)?1:-1;
            if(m_columns & 1)
            {
              sign = -sign;
            }
            for(int r=m_rows-1;r>=0;--r)
            {
              res.m_fields[c][r] = sign * part.Determinant();
              sign = -sign;
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
  MatrixType Inverse() const
  {
    MatrixType res = Adjoint();
    T det = res(0,0) * Get(0,0);
    T sign = 1;
    for(size_t c = 1;c<m_columns;++c)
    {
      sign = -sign;
      det += res(c,0) * Get(0,c);
    }
    return (res *= (1/det));
  }

  MatrixType Slice(const size_t row = 0,const size_t column = 0,size_t rows = -1,size_t columns = -1) const
  {
    assert(row<m_rows);
    assert(column<m_columns);
    if(rows==-1)
    {
      rows=m_rows-row;
    }
    if(columns==-1)
    {
      columns=m_columns-column;
    }
    assert(row+rows<=m_rows);
    assert(column+columns<=m_columns);
    return MatrixType(*this,row,column,rows,columns);
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
  size_t m_stride;
  Data m_data;
  T** m_fields;
};

