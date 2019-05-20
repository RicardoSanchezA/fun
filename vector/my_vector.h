#ifndef _MY_VECTOR_H_
#define _MY_VECTOR_H_

#include <cstddef>
#include <cstdint>
#include <string.h>
#include <iostream>

#define VECTOR_DEFAULT_SIZE 10

template<typename T>
class my_vector {
public:
    my_vector();
    my_vector(const my_vector<T>& rhs);
    ~my_vector();

    T* begin();
    T* end();
    T front() const;
    T back() const;
    T operator[](int) const;
    T at(int) const;
    void push_back(const T& value);
    void pop_back();
    void clear();
    unsigned size() const;
    bool empty() const;
    void print() const;
private:
    void increase_capacity(); 
private:
    unsigned _capacity;
    unsigned _size;
    T* _array;
    T* _begin;
    T* _end;
};

/*
 **************************************
 ****** Constructor & Destructor ******
 **************************************
 */
template<typename T>
my_vector<T>::my_vector() :
    _capacity(VECTOR_DEFAULT_SIZE),
    _size(0),
    _array(new T[_capacity]),
    _begin(_array),
    _end(_array)
{

}

template<typename T>
my_vector<T>::~my_vector()
{
    delete [] _array;
}

/*
 **************************
 **** Public functions ****
 **************************
 */
template<typename T>
T*
my_vector<T>::begin()
{
    return _begin;
}

template<typename T>
T*
my_vector<T>::end()
{
    return _end;
}

template<typename T>
T
my_vector<T>::front() const
{
    return _array[0]; 
}

template<typename T>
T
my_vector<T>::back() const
{
    return _array[_size - 1];
}

template<typename T>
T
my_vector<T>::at(int i) const
{
    return _array[i];
}

template<typename T>
T
my_vector<T>::operator[](int i) const
{
    return _array[i];
}

template<typename T>
void
my_vector<T>::push_back(const T& item)
{
    ++_size;
    if (_capacity <= _size) {
        increase_capacity();
    }
    *_end = item;
    ++_end;
}

template<typename T>
void
my_vector<T>::pop_back()
{
    if (_size > 0) {
        --_size;
        --_end;
    }
}

template<typename T>
void
my_vector<T>::clear()
{
    memset(_array, 0, _capacity * sizeof(T));
    _size = 0;
    _begin = _end = _array;
}

template<typename T>
void
my_vector<T>::print() const
{
    T *ptr = _begin;
    while (ptr != _end) {
        std::cout << *ptr << " ";
        ++ptr;
    }
    std::cout << "\n";
}

template<typename T>
unsigned
my_vector<T>::size() const
{
    return _size;
}

template<typename T>
bool
my_vector<T>::empty() const
{
    return _size == 0;
}

/*
 **************************
 **** Helper functions ****
 **************************
 */
template<typename T>
void
my_vector<T>::increase_capacity()
{
    unsigned new_capacity = _capacity << 1;
    /* Allocate new array, copy data, and delete old one */
    T* new_array = new T[new_capacity];
    memcpy(new_array, _array, _capacity * sizeof(T));
    delete [] _array;
    _array = new_array;
    /* Set begin and end pointers appropriately */
    _begin = _array;
    _end = &_array[_size - 1];
    _capacity = new_capacity;
}

#endif /* _MY_VECTOR_H_ */
