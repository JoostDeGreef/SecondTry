#pragma once

template<typename T>
class Limits
{
public:
    static const T CompareEpsilon;
    static const T MinValue;
    static const T MaxValue;
};

template<typename T> const T Limits<T>::CompareEpsilon = (T)1e-10;
template<typename T> const T Limits<T>::MinValue = std::numeric_limits<T>::lowest();
template<typename T> const T Limits<T>::MaxValue = std::numeric_limits<T>::max();
