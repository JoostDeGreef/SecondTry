#include <immintrin.h>
#include "Matrix.h"

namespace VectorOperations
{
  void Add(double * res, const double * const a, const double * const b,const size_t length)
  {
    for(size_t i=0;i<(length+3)/4;++i)
    {
        __m256d A = _mm256_load_pd(a+i*4);
        __m256d B = _mm256_load_pd(b+i*4);
        __m256d RES = _mm256_add_pd(A,B);
        _mm256_store_pd(res+i*4,RES);
    }
  }
  void Add(float * res, const float * const a, const float * const b,const size_t length)
  {
    for(size_t i=0;i<(length+7)/8;++i)
    {
        __m256 A = _mm256_load_ps(a+i*8);
        __m256 B = _mm256_load_ps(b+i*8);
        __m256 RES = _mm256_add_ps(A,B);
        _mm256_store_ps(res+i*8,RES);
    }
  }

  void Subtract(double * res, const double * const a, const double * const b,const size_t length)
  {
    for(size_t i=0;i<(length+3)/4;++i)
    {
        __m256d A = _mm256_load_pd(a+i*4);
        __m256d B = _mm256_load_pd(b+i*4);
        __m256d RES = _mm256_sub_pd(A,B);
        _mm256_store_pd(res+i*4,RES);
    }
  }
  void Subtract(float * res, const float * const a, const float * const b,const size_t length)
  {
    for(size_t i=0;i<(length+7)/8;++i)
    {
        __m256 A = _mm256_load_ps(a+i*8);
        __m256 B = _mm256_load_ps(b+i*8);
        __m256 RES = _mm256_sub_ps(A,B);
        _mm256_store_ps(res+i*8,RES);
    }
  }

  void ScalarMultiplyAdd(double * res, const double a, const double * const b, const size_t length)
  {
    __m256d A = _mm256_set1_pd(a);
    for(size_t i=0;i<(length+3)/4;++i)
    {
        __m256d B = _mm256_load_pd(b+i*4);
        __m256d RES = _mm256_load_pd(res+i*4);
        RES = _mm256_fmadd_pd(A,B,RES);
        _mm256_store_pd(res+i*4,RES);
    }
  }
  void ScalarMultiplyAdd(float * res, const float a, const float * const b, const size_t length)
  {
    __m256 A = _mm256_set1_ps(a);
    for(size_t i=0;i<(length+7)/8;++i)
    {
        __m256 B = _mm256_load_ps(b+i*8);
        __m256 RES = _mm256_load_ps(res+i*8);
        RES = _mm256_fmadd_ps(A,B,RES);
        _mm256_store_ps(res+i*8,RES);
    }
  }

  void Fill(double * res, const size_t length, const double value)
  {
    __m256d temp = _mm256_set1_pd(value);
    for(size_t i=0;i<(length+3)/4;++i)
    {
        _mm256_store_pd(res+i*4,temp);
    }
  }
  void Fill(float * res, const size_t length, const float value)
  {
    __m256 temp = _mm256_set1_ps(value);
    for(size_t i=0;i<(length+7)/8;++i)
    {
        _mm256_store_ps(res+i*8,temp);
    }
  }

};
