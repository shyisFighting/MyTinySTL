#ifndef MYTINYSTL_TYPE_TRAITS_H_
#define MYTINYSTL_TYPE_TRAITS_H_

//这个头文件用于提取内存信息

#include<type_traits>

namespace mystl
{
	//生成两个储存真值和假值的结构
	template <class T,T v>
	struct m_integral_constant
	{
		static constexpr T value = v;
	};
	
	//针对bool类型的一个偏特化，枚举完其常量类型
	template <bool b>
	using m_bool_constant = m_integral_constant<bool,b>;
	
	typedef m_bool_constant<true> m_true_type;
	typedef m_bool_constant<false> m_false_type;
	
	template <class T1, class T2>
	struct pair;
	
	template <class T>
	struct is_pair : mystl::m_false_type{};
	
	//这是is_pair的一个全特化，当为pair类型时，其继承true常数
	template <class T1,class T2>
	struct is_pair<mystl::pair<T1,T2>>:mystl::m_true_type{};
}

#endif