#ifndef MYTINYSTL_ITERATOR_H_
#define MYTINYSTL_ITERATOR_H_

// 迭代器设计，包含一些模板结构体和全局函数

#include<cstddef>

#include "type_traits.h"

namespace mystl
{
	//规定五种迭代器类型结构，出于判断的目的，不需要什么实质性的内容，
	//继承关系来源于他们的功能
	struct input_iterator_tag {};
	struct output_iterator_tag ();
	struct forward_iterator_tag : public input input_iterator_tag {};
	struct bidirectional_iterator_tag : public forward_iterator_tag {};
	struct random_access_iterator_tag : public bidirectional_iterator_tag {};
    
	// iterator 模板
	//category 迭代器类型，T数据类型，distance指针相减的数据类型在cstddef头文件里
	// pointer 地址 reference 引用
	template <class Category, class T , class Distance = ptrdiff_t,
	class Pointer = T*, class Reference = T&>
	struct iterator
	{
		// 统一别名（统一接口）
		typedef Category iterator_category;
		typedef T value_type;
		typedef Pointer pointer;
		typedef Reference reference;
		typedef Distance difference_type;
	}；
	
	// iterator traits
	
	template <class T>
	struct has_iterator_cat
	{
		private:
		   struct two { char a; char b; };
		   template <class U> static two test(...);
		   template <class U> static char test(typename U::iterator_category* = 0);
	    public:
		   static const bool value = sizeof(test<T>(0)) == sizefo(char);
    }
	
	template <class Iterator, bool>
	struct iterator_traits_impl {};
	
	template <class Iterator>
	struct iterator_traits_impl<Iterator,true>
	{
		typedef typename Iterator::iterator_category iterator_category;
		typedef typename Iterator::value_type  value_type;
		typedef typename Iterator::pointer  pointer;
		typedef typename Iterator::reference reference;
		typedef typename Iterator::difference_type difference_type;
	}

    template <class Iterator,bool>
    struct iterator_traits_helper {};	
	
	template <class Iterator>
	struct iterator_traits_helper<Iterator,true>:
	public iterator_traits_impl<Iterator,
	std::is_convertible<typename Iterator::iterator_category,input_iterator_tag>::value ||
	std::is_convertible<typename Iterator::iterator_category,output_iterator_tag::value>
	{};
	
	// 萃取迭代器的特性
	// 通过cat判断传入类是否有category，有则将true传入helper，然后通过true版本创建
	// helper，并在其可以转换为input/output时，创建iterator特性 接口实现   但问题是他应该已经有category了才对，为什么还需要实现，
	// 可能与后面的实现有关
	template <class Iterator>
	struct iterator_traits:public iterator_traits_helper<Iterator,has_iterator_cat<Iterator>::value> {};
	
	
	//针对原生指针的偏特化版本
	template <class T>
	struct iterator_traits<T*>
	{
		typedef random_access_iterator_tag  iterator_category;
		typedef T  value_type;
		typedef T* pointer;
		typedef T& reference;
		typedef ptrdiff_t   difference_type;
	};
	
	template <class T>
	struct iterator_traits<const T*>
	{
		typedef random_access_iterator_tag iterator_category;
		typedef T  value_type;
		typedef const T* pointer;
		typedef const T& reference;
		typedef ptrdiff_t  difference_type;
	};
	
	template <class T, class U, bool = has_iterator_cat<iterator_traits<T>>::value>
	struct has_iterator_cat_of:
	public m_bool_constant<std::is_convertible<typename iterator_traits<T>::iterator_category,U>::value>
	{};
	
	// 萃取某种迭代器
	template <class T, class U>
	struct has_iterator_cat_of<T,U,false>:public m_false_type{};
	
	template <class Iter>
	struct is_input_iterator : public has_iterator_cat_of<Iter,input_iterator_tag>{};
	
	template <class Iter>
	struct is_output_iterator : public has_iterator_cat_of<Iter,output_iterator_tag>{};
	
	template <class Iter>
	struct is_forward_iterator: public has_iterator_cat_of<Iter,forward_iterator_tag>{};
	
	template <class Iter>
	struct is_bidirectional_iterator:public has_iterator_cat_of<Iter,bidirectional_iterator_tag>{};
	
	template <class Iter>
	struct is_random_access_iterator:public has_iterator_cat_of<Iter,random_access_iterator_tag>{};
	
	template <class Iterator>
	struct is_iterator:
	    public m_bool_constant<is_input_iterator<Iterator>::value ||
		  is_output_iterator<Iterator>::value>{};
		  
	//萃取某个迭代器的category
	template <class Iterator>
	typename iterator_traits<Iterator>::iterator_category*
	iterator_category(const Iterator&)
	{
		// 目前还不是非常确定，不过这里应该是调用构造函数，并返回结果的指针，但是在这里更名有什么意义呢。下面都没有选择改名
		typedef typename iterator_traits<Iterator>::iterator_category category;
		return Category();
	}
	
	//萃取某个迭代器的 distance_type
	template <class Iterator>
	typename iterator_traits<Iterator>::difference_type*
	distance_type(const Iterator&)
	{
		return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
	}
	
	// 萃取某个迭代器的value_type
	template <class Iterator>
	typename iterator_traits<Iterator>::value_type*
	value_type(const Iterator&)
	{
		return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
	}

    //一下函数用于计算迭代器间的距离

    // distance 的input_iterator_tag的版本
    template <class InputIterator>
    typename iterator_traits<InputIterator>::difference_type
    distance_dispatch(InputIterator first,InputIterator last,input_iterator_tag)
	{
		typename iterator_traits<InputIterator>::difference_type n = 0;
		while (first!=last)
		{
			++first;
			++n;
		}
		return n;
	}
	
	// distance的random_access_iterator_tag 的版本
	template <class RandomIter>
	typename iterator_traits<RandomIter>::difference_type
	distance_dispatch(RandomIter first,RandomIter last,random_access_iterator_tag)
	{
		return last-first;
	}
	
	// 迭代器的名称本质上是一个指针，通过确定第一个迭代器的类型就可以判断两个迭代器间的距离。
	template <class InputIterator>
	typename iterator_traits<InputIterator>::difference_type
	distance(InputIterator first,InputIterator last)
	{
		return distance_dispatch(first,last,iterator_category(first));
	}
	
	//以下函数用于让迭代器前进n个距离
	
	//advance的input_iterator_tag的版本
	template <class InputIterator,class Distance>
	void advance_dispatch(InputIterator& i,input_iterator_tag)
	{
		while(n--)
			++i;
	}
	
	//advance 的 bidirectional_iterator_tag的版本
	template <class BidirectionalIterator,class Distance>
	void advance_dispatch(BidirectionalIterator& i,Distance n,bidirectional_iterator_tag)
	{
		if (n>=0)
			while(n--) ++i;
		else
			while(n++) --i;
	}
	
	//advance的random_access_iterator_tag的版本
	template <class RandomIter,class Distance>
	void advance_dispatch(RandomIter& i,Distance n,random_access_iterator_tag)
	{
		i+=n;
	}
	
	template <class InputIterator,class Distance>
	void advance(InputIterator& i, Distance n)
	{
		advance_dispatch(i,n,iterator_category(i));
	}
	/**************************************************************/
	
	// 模板类：reverse_iterator
	// 代表反向迭代器，是前进为后退，后退为前进
	template <class Iterator>
	class reverse_iterator
	{
		private:
		    Iterator current; //对应的正向迭代器
		
		public:
		    typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
            typedef typename iterator_traits<Iterator>::value_type        value_type;
            typedef typename iterator_traits<Iterator>::difference_type   difference_type;
            typedef typename iterator_traits<Iterator>::pointer           pointer;
            typedef typename iterator_traits<Iterator>::reference         reference;
            
			typedef Iterator   iterator_type;
			typedef reverse_iterator<Iterator>   self;
	    
		public:
		    //构造函数
			reverse_iterator(){}
			// 不希望它可以进行隐式转化。
			explicit reverse_iterator(iterator_type i):current(i){}
			// 在复制时用到，否则会使用浅copy
			reverse_iterator(const self& rhs):current(rhs.current){}
	
	    public:
		    //取出对应的正向迭代器
			iterator_type base() const
			{ return current;} 
			
			//重载操作符
			reference operator*() const
			{//实际对应正向迭代器的前一个位置
			    auto tmp = current;
				return *--tmp;
			}
			
			pointer operator->() const
			{
				return &(operator*());
			}
			
			//前进变为后退
			
			self& operator++()
			{
				--current;
				return *this;
			}
			
			self operator++(int)
			{
				self tmp = *this;
				--current;
				return tmp;
			}
			
			//后退变为前进
			self& operator--()
			{
				++current;
				return *this;
			}
			
			self operator--(int)
			{
				self tmp = *this;
				++current;
				return tmp;
			}
			
			self& operator+=(difference_type n)
			{
				current -= n;
				return *this;
			}
			
			self operator+(difference_type n) const
			{
				return self(current - n);
			}
			
			self& operator-=(difference_type n)
			{
				current += n;
				return *this;
			}
			
			self operator-(difference_type n) const
			{
				return self(current + n);
			}
			
			reference operator[](difference_type n) const
			{
				return *(*this+n);
			}
	};
	
	// 重载operator-
	template <class Iterator>
	typename reverse_iterator<Iterator>::difference_type
	operator-(const reverse_iterator<Iterator>& lhs,const reverse_iterator<Iterator>& rhs)
	{
        return rhs.base()-lhs.base();
    }
    
    //重载比较操作符
    template <class Iterator>
    bool operator==(const reverse_iterator<Iterator>& lhs,
                    const reverse_iterator<Iterator>& rhs)
	{
        return lhs.base() == rhs.base();
	}
	
	template <class Iterator>
    bool operator<(const reverse_iterator<Iterator>& lhs,
                   const reverse_iterator<Iterator>& rhs)
    {
		// 不知道对不对，这里是不是要反序？
        return rhs.base() > lhs.base();
    }

    template <class Iterator>
    bool operator!=(const reverse_iterator<Iterator>& lhs,
                    const reverse_iterator<Iterator>& rhs)
    {
        return !(lhs == rhs);
    }

    template <class Iterator>
    bool operator>(const reverse_iterator<Iterator>& lhs,
                   const reverse_iterator<Iterator>& rhs)
    {
        return rhs < lhs;
    }

    template <class Iterator>
    bool operator<=(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs)
    {
        return !(rhs < lhs);
    }

    template <class Iterator>
    bool operator>=(const reverse_iterator<Iterator>& lhs,
                    const reverse_iterator<Iterator>& rhs)
    {
        return !(lhs < rhs);
    }

}   // namespace mystl

#endif // !MYTINYSTL_ITERATOR_H_
     		
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	

	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	