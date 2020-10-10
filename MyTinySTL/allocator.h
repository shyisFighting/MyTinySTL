// 在SGI STL中真正发挥作用的是alloc而非此文件allocator，此文件符合STL部分标准，但SGI本身不使用，也不建议我们使用，因为它仅仅简单的
// 包裹了::operator new 和 ::operator delete,效率不佳，SGI真正使用的alloc不接受参数，而allocator接受参数
#ifndef MYTINYSTL_ALLOCATOR_H_
#define MYTINYSTL_ALLOCATOR_H_

#include "construct.h"
#include "util.h"

namespace mystl
{
//从这里就可以看出allocater是需要一开始就接受空间申请对象类型参数的，
//allocator会以此建立所有函数。
template <class T>
class allocator
{
public:
    typedef T value_type;
    typedef T *pointer;
    typedef const T *const_pointer;
    typedef T &reference;
    typedef const T &const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

public:
    static T *allocate();
    // 这里的size_type不是几字节的意思，而是多少个对象的意思，size_t的本质是
    // unsigned类型。
    static T *allocate(size_type n);
    
    static void deallocate(T *ptr);
    // 不太明白这一项存在的意义，一次性删除多个对象吗 ？暂时不写
    //static void deallocate(T *ptr, size_type n);
    
    static void construct(T *ptr);
    static void construct(T *ptr, const T &value);
    //这一个函数的参数，不明白，不过应该是有关于右值直接move的函数
    static void construct(T *ptr, T &&calue);
    template <class..Args>
    static void construct(T *ptr, Args &&... args);

    static void destory(T *ptr);
    static void destory(T *first,T *last)；
};

template<class T>
T* allocator<T>::allocate()
{
    return static_cast<T *>(::operator new(sizeof(T)));
}

template<class T>
T* allocator<T>::allocate(size_type n)
{
    if(n==o)
        return nullptr;
    return static_cast<T *>(::operator new(n * sizeof(T)));
}

template<class T>
void allocator<T>::deallocate(T* ptr)
{
    //如果是空指针，不需要处理
    if(ptr == nullptr)
        return;
    ::operator delete(ptr);
}
template<class T>
void allocator<T>::construct(T* ptr)
{
    mystl::construct(ptr);
}
template <class T>
void allocator<T>::construct(T* ptr, const T& value)
{
  mystl::construct(ptr, value);
}
template<class T>
void allocator<T>::construct(T* ptr,T &&value)
{
    //看完算法部分再看这一个函数
    mystl::construct(ptr, mystl::move(value));
}
template<class T>
template<class...Args>
void allocator<T>::construct(T *ptr,Args&& ...args)
{
    //这里的forward应该也是算法部分的内容，可能是专门用于拆包的函数
    mystl::construct(ptr, mystl::forward<Args>(args)...);
}

template <class T>
void allocator<T>::destory(T*ptr)
{
    mystl::destory(ptr);
}
template<class T>
void allocator<T>::destroy(T* first,T* last)
{
    mystl::destory(first, last);
}
}
#endif