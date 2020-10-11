//这个文件主要包含了三个函数：
//uninitialized_copy,uninitialized_fill,uninitialized_fill_n
//std::bool_type 应该是搞错了，这个应该是我们自己定义的，应该是mystl::bool_type吧

#ifndef MYTINYSTL_UNINITIALIZED_H_
#define MYTINYSTL_UNINITIALIZDE_H_

//#include "algobase.h"
//#include "construct.h"
//#include "iterator.h"
//#incldue "type_traits.h"
//#incldue "util.h"

namespace mystl
{
// uninitialized_copy
// 先设计两个不同的处理版本，分为是否拥有trivial construct两种情况
template <class InputIter,class ForwardIter>
ForwardIter unchecked_uninit_copy(InputIter first,InputIter last,ForwardIter result,std::true_type)
{
    return mystl::copy(first, last, result);
}
template<class InputIter,class ForwardIter>
ForwardIter unchecked_uninit_copy(InputIter first,InputIter last,ForwardIter result,std::false_type)
{
    auto cur = result;
    tyr
    {
        for (; first != last;++fisrt,++cur)
        {
            mystl::construct(&*cur, *first);
        }
    }
    //这几个函数都需要具有“commit or rollback”语义，也就是要么构造出所有元素，
    //要么不构造任何对象
    catch(...)
    {
        for (; result != cur;++result)
            mystl::destroy(&*result);
    //这里如果catch了，会返回错误construct后一个指针
    }
    return cur;
}
template<class InputIter,class ForwardIter>
ForwardIter uninitialized_copy(InputIter first,InputIter last,ForwardIter result)
{
    //这里的std::is_trivially_copy_assignable在内部应该定义了一个value_type，里面存放了bool_type,
    //在这里读出后需要生成一个临时对象作为类型推导。
    return mystl::unchecked_uninit_copy（first，last,result,std::is_trivially_copy_assignable<typename
    iterator_traits<ForwardIter>::value_type>());
}

// uninitialized_copy_n
// 大体逻辑与uninitialized_copy是一样的，多了一个表示长度的参数类型Size,少了一个输入迭代器（读迭代器）
template<class InputIter,class Size,class ForwardIter>
ForwardIter unchecked_uninit_copy_n(InputIter first,Size n,ForwardIter result,std::true_type)
{
    return mystl::copy_n(first, n, result).second;
}
template<class InputIter,class Size,class ForwardIter>
ForwardIter unchecked_uninit_copy_n(InputIter first,Size n,ForwardIter result,std::false_type)
{
    //这里不直接用result操作，是为了方便后如果出错可以从开始遍历摧毁
    auto cur = result;
    try
    {
        for (; n > 0;--n,++cur,++first)
        {
            mystl::construct(&*cur, *first);
        }
    }
    catch(...)
    {
        for (; result != cur;++result)
            mystl::destory(&*result);
    }
    return cur;
}
template<class InputIter,class Size,class ForwardIter>
ForwardIter uninitialized_copy_n(InputIter first,Size n,ForwardIter result)
{
    //这里怎么又变成InputIter的了，不过我感觉影响不大，因为类型应该是一样的
    return mystl::unchecked_uninit_copy_n(first,n,result,std::is_trivially_copy_assignable<
                                        typename iterator_traits<InputIter>::
                                        value_type>());
}
//uninitialized_fill
//这个函数是填充不是copy也不返回迭代器，因为可能有用的迭代器都在我们自己手里了，而copy的结尾指针我们是不知道的
template<class ForwardIter,class T>
void unchecked_uninit_fill(ForwardIter first,ForwardIter last,const T& value,std::true_type)
{
    mystl::fill(fisrt, last, value);
}
template <class ForwardIter, class T>
void unchecked_uninit_fill(ForwardIter first,ForwardIter last,const T& value,std::false_type)
{
    auto cur = fisrt;
    try
    {
        for (; cur != last;++cur)
        {
            mystl::construct(&*cur, value);
        }
    }
    catch(...)
    {
        for (; first != cur;++first)
            mystl::destory(&*result);
        //之前都没有抛出错误，所以之前如果犯错，会返回犯错位置的后一个指针
        throw;
    }
}
template<class ForwardIter,class T>
void uninitialized_fill(ForwardIter first,ForwardIter last,const T& value)
{
    mystl::unchecked_uninit_fill(first, last, value, std::is_trivially_copy_assignable<
                               typename iterator_traits<ForwardIter>::value_type>());
}
// uninitialized_fill_n,填充n项，显然这就需要返回末尾指针了
template <class ForwardIter, class Size, class T>
ForwardIter unchecked_uninit_fill_n(ForwardIter first, Size n, const T& value, std::true_type)
{
  //调用fill函数，之前是copy，他们两个的区别就不再说了
  return mystl::fill_n(first, n, value);
}

template <class ForwardIter, class Size, class T>
ForwardIter unchecked_uninit_fill_n(ForwardIter first, Size n, const T& value, std::false_type)
{
  auto cur = first;
  try
  {
    for (; n > 0; --n, ++cur)
    {
      mystl::construct(&*cur, value);
    }
  }
  catch (...)
  {
    //不知道是什么原因，但凡返回指针，我们都没有直接抛出错误，也许返回指针便于我们自己做其他处理
    for (; first != cur; ++first)
      mystl::destroy(&*first);
  }
  return cur;
}

template <class ForwardIter, class Size, class T>
ForwardIter uninitialized_fill_n(ForwardIter first, Size n, const T& value)
{
  return mystl::unchecked_uninit_fill_n(first, n, value, std::is_trivially_copy_assignable<
                                        typename iterator_traits<ForwardIter>::value_type>());
}
//uninitialized_move
template <class InputIter, class ForwardIter>
ForwardIter unchecked_uninit_move(InputIter first, InputIter last, ForwardIter result, std::true_type)
{
  return mystl::move(first, last, result);
}

template <class InputIter, class ForwardIter>
ForwardIter unchecked_uninit_move(InputIter first, InputIter last, ForwardIter result, std::false_type)
{
  ForwardIter cur = result;
  //注意move与copy的区别就体现于此
  try
  {
    for (; first != last; ++first, ++cur)
    {
      mystl::construct(&*cur, mystl::move(*first));
    }
  }
  catch (...)
  {
    //这里直接使用区间的destory函数，而没有一个个调用destroy
    mystl::destroy(result, cur);
  }
  return cur;
}

template <class InputIter, class ForwardIter>
ForwardIter uninitialized_move(InputIter first, InputIter last, ForwardIter result)
{
  return mystl::unchecked_uninit_move(first, last, result,std::is_trivially_move_assignable<
                                      typename iterator_traits<InputIter>::value_type>());
}


// uninitialized_move_n
// 同样返回迭代器
template <class InputIter, class Size, class ForwardIter>
ForwardIter unchecked_uninit_move_n(InputIter first, Size n, ForwardIter result, std::true_type)
{
  return mystl::move(first, first + n, result);
}

template <class InputIter, class Size, class ForwardIter>
ForwardIter unchecked_uninit_move_n(InputIter first, Size n, ForwardIter result, std::false_type)
{
  auto cur = result;
  try
  {
    for (; n > 0; --n, ++first, ++cur)
    {
      mystl::construct(&*cur, mystl::move(*first));
    }
  }
  catch (...)
  {
    //这里又是一个个destory的，为什么？并且还抛出了错误，这里应该是有些问题，写完move再回来看看.....
    for (; result != cur; ++result)
      mystl::destroy(&*result);
    throw;
  }
  return cur;
}

template <class InputIter, class Size, class ForwardIter>
ForwardIter uninitialized_move_n(InputIter first, Size n, ForwardIter result)
{
  return mystl::unchecked_uninit_move_n(first, n, result,std::is_trivially_move_assignable<
                                        typename iterator_traits<InputIter>::value_type>());
}
}
#endif 
