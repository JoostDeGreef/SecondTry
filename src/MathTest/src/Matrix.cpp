#include <immintrin.h>
#include "Matrix.h"

namespace VectorOperations
{
//   void Add(double * res, const double * const a, const double * const b,const size_t length)
//   {

//   }
//   void Add(float * res, const float * const a, const float * const b,const size_t length)
//   {

//   }

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
