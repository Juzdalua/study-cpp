#pragma once
#include <assert.h>

template <typename T>
class CArr
{
private:
    T *pData;
    int count;
    int maxCount;

public:
    void RPush(const T &data);
    void resize(int resizeCount);
    T &operator[](int idx);

public:
    CArr();
    ~CArr();
};

template <typename T>
CArr<T>::CArr() : pData(nullptr),
                  count(0), maxCount(1)
{
    pData = new T[1];
}

template <typename T>
CArr<T>::~CArr()
{
    delete[] pData;
}

template <typename T>
void CArr<T>::RPush(const T &data)
{
    if (this->maxCount <= this->count)
    {
        resize(this->maxCount * 2);
    }
    this->pData[this->count] = data;
    this->count++;
}

template <typename T>
void CArr<T>::resize(int resizeCount)
{
    if (this->maxCount >= resizeCount)
    {
        assert(false);
    }

    T *pNewData = new T[resizeCount];
    for (int i = 0; i < this->count; i++)
    {
        pNewData[i] = this->pData[i];
    }
    delete[] this->pData;

    this->pData = pNewData;
    this->maxCount = resizeCount;
}

template <typename T>
T &CArr<T>::operator[](int idx)
{
    return this->pData[idx];
}