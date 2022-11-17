#pragma once

#include <stdalign.h>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <new>

namespace VectorOperations
{
  // void Add(double * res, const double * const a, const double * const b, const size_t length);
  // void Add(float * res, const float * const a, const float * const b, const size_t length);
  template<typename T>
  void Add(T * res, const T * const a, const T * const b, const size_t length)
  {
    for(size_t i=0;i<length;++i)
    {
      res[i] = a[i] + b[i];
    }
  }

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
    return m_fields[row][column];
  }
  Proxy operator () (const size_t & row, const size_t & column)
  {
    return Proxy(*this,row,column);
  }
  const T Get(const size_t & row, const size_t & column) const
  {
    return m_fields[row][column];
  }
  void Set(const size_t & row, const size_t & column, const T & value)
  {
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

  MatrixType & Fill(const T value)
  {
    MakeUnique();
    for(size_t i = 0; i < m_rows; ++i)
    {
      VectorOperations::Fill(m_fields[i],m_columns,value);
    }
    return *this;
  }

public:
  MatrixType & Add(const MatrixType & other)
  {
    AssertSizeEqual(other);
    MakeUnique();
    for(size_t i = 0; i < m_rows; ++i)
    {
      VectorOperations::Add(m_fields[i],m_fields[i],other.m_fields[i],m_columns);
    }
    return *this;
  }
  MatrixType Sum(const MatrixType & other) const
  {
    AssertSizeEqual(other);
    MatrixType res(m_rows,m_columns);
    for(size_t i = 0; i < m_rows; ++i)
    {
      VectorOperations::Add(res.m_fields[i],m_fields[i],other.m_fields[i],m_columns);
    }
    return res;
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
    void AssertSizeEqual(const MatrixType & other) const
    {
      // todo
    }
private:
  const size_t m_rows;
  const size_t m_columns;
  const size_t m_stride;
  Data m_data;
  T** m_fields;
};

