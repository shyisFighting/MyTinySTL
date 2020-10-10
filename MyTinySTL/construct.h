#ifndef MYTINYSTL_CONSTRUCT_H_
#define MYTINYSTL_CONSTRUCT_H_
// 这个文件主要包含了construct和destory
// construct分为接受不同类型参数的三种情况，其中构建类型是必要的
// destroy有两种情况，且都需要判断是否具有简单的默认析构函数,如果是默认的析构函数，实际上我们什么也不用做，
// 如果不是trivial destroy就需要显式调用，detroy_cat是为了一个对象序列,destroy_one是为了单个对象。
#include <new>

#include"type_traits.h"
#include"iterator.h"

// 对于pragma warning不明确
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100) 
#endif 

namespace mystl
{
//在给定的地址上构建对象，即placement new
template <class T>
void construct(T* ptr)
{
    ::new ((void *)ptr) T();
}
//仅有一个参数的构造函数
template <class T1,class T2>
void construct(T1 *ptr,T2& value)
{
    ::new ((void *)ptr) T1(value);
}
//任意类型，任意个数的构造函数,但是我不是特别明白
template <class T,class... Args>
void construct(T* ptr,Args&&... args)
{
    ::new ((void *)ptr) T(mystl::forward<Args>(args)...);
}

//destroy 析构函数
//通过传入的参数判断是否调用Ty的析构函数，当为true_type时，没有意义
//这一点应该与后面有关
template <class T>
void destory_one(T*,std::true_type){}
template<class T>
void destroy_one(T* pinter,std::false_type)
{
    if(pointer!=nullptr)
    {
        pointer->~T();
    }
}
template <class ForwardIter>
void destroy_cat(ForwardIter,ForwardIter,std::true_type){}
template <class ForwardIter>
void destroy_cat(ForwardIter first,ForwardIter last,std::false_type)
{
    for (; first != last;++first)
// 先对first迭代器进行提领操作，然后取得他的地址，迭代器是一种smart pointer，
// &*first并不是first
        destroy(&*first);
}
template <class T>
void destory(T* pointer)
{
//is_trivially_destructible判断是否拥有简单的析构函数，如果没有，则会调用析构函数~T（）
    destroy_one(pointer, std::is_trivially_destructible <T>());
}
//析构一个范围的同一种类型对象，同样判断是否需要调用其自身的~T（）
template <class ForwardIter>
void destroy(ForwardIter first,ForwardIter last)
{
    destroy_cat(first, last, std::is_trivially_destructible<typename iterator_traits<ForwardIter>::value_type>());
}
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif