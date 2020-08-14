#ifndef MYTINYSTL_ALLOC_H_
#define MYTINYSTL_ALLOC_H_

// 这个头文件包含一个类 allco，用于分配和回收内存，以内存池的方式实现

// 好像在v2.0.0版本开始，被启用了，无所谓，反正练手用的

#include <new>
#include <cstddef>
#include <cstdio> 

namespace mystl 

{
	// 共用体：FreeList
	// 采用链表的方式管理内存碎片，分配与回收小内存（<=4k）区块
	union FreeList
	{
		union FreeList* next; //指向下一个区块
		// 指向共用体的指针，可以指向他自己
		char data[1]; //储存本块内存的首地址，char就是一个字节
	};
	
	// 不同内存范围的上调大小
	// 以EAlign128为例，保证他的内存对齐为8的倍数，有关于pragma pack(n)
	// 用来对齐已经完成储存的内存块
	enum 
	{
		EAlign128 = 8;
		EAlign256 = 16；
		EAlign512 = 32；
		EAlign1024= 64；
		EAlign2048 = 128；
		EAlign4096 = 256；
	}；
	
	// 小对象的内存大小
	enum { ESmallobjectBytes = 4096};
	
	// free lists 个数
	enum { EFreeListNumber = 56 };
	
	// 空间配置类 allco，用于分配和回收内存，以内存池的方式实现
	// 如果内存较大，超过 4096 bytes，直接调用std::malloc,std::free
	// 当内存较小时，以内存池管理，每次配置一大块内存，并维护对应的自由链表
	
	class allco
	{
		private:
		   static char* start_free;
		   static char* end_free;
		   static size_t heap_size;
		   
	       static FreeList* free_list[EFreeListNumber]; //自由链表
		   // 这是一个静态变量  为56个指向链表公共体的指针数组，始终存在于该类中；
		public:
		   // 动态分配函数
           static void* allocate(size_t n);
		   // 释放p指向的大小为n的空间，p不能为零
           static void deallocate(void* p,size_t n);
		   // 调整分配过的内存，重新分配内存
           static void* reallocate(void* p, size_t old_size,size_t new_size);
		   
		private:
		   // bytes对应上调大小
		   static size_t M_align(size_t bytes);
		   // 将bytes上调至对应区间大小
		   static size_t M_round_up(size_t bytes);
		   // 根据区块大小，选择第n个free lists
		   static size_t M_freelist_index(size_t bytes);
		   // 重新填充freelist
		   static void* M_refill(size_t n);
		   // 从内存池中取空间给free list 使用，条件不允许时，会调整nblock-------把对size_t的引用改为对int的引用
		   static char* M_chunk_alloc(size_t size, int &nobj);
		   
    };
	
	// 静态成员初始化
	
	char* alloc::start_free = nullptr;
	char* alloc::end_free = nullptr;
	size_t alloc::heap_size = 0;
	
	FreeList* alloc::free_list[EFreeListNumber] = {
		nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
		nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
		nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
		nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
		nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
		nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
		nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr
	};
	
	// 声明为内联函数，那为什么不直接定义在声明处？
	inline void* alloc::allocate(size_t n)
	{
		FreeList** my_free_list;
		FreeList* result;
		if (n > static_cast<size_t>(ESmallobjectBytes))
			return std::malloc(n);
		my_free_list = free_list + M_freelist_index(n);
		//my_free_list = free_list[M_freelist_index(n)]
		result = *my_free_list;
		//result = my_free_list;
		if (result == nullptr)
		{
			void* r = M_refill(M_round_up(n));
			return r;
		}
		*my_free_list = result->next;
		//my_free_list = result -> next;
		return result;
	}
	
	inline void alloc::deallocate(void* p, size_t n)
	{
		if (n > static_cast<size_t>(ESmallobjectBytes))
		{
			std::free(p);
			return;
		}
		FreeList* q = reinterpret_cast<FreeList*>(p);
		FreeList** my_free_list;
		my_free_list = free_list + M_freelist_index(n);
		q->next = *my_free_list;
		*my_free_list = q; 
		// FreeList* my_free_list;
		// // 这里根据区块大小选择又是什么意思？q->next还比较好理解，
		// // 选择的难道是空指针？？？？？？？？？？？？？？？
		// my_free_list = free_list[M_freelist_index(n)];
		// q->next = my_free_list;
		// // 这个my_free_list有什么用啊？？？？？？？？？？
		// // 不要忘了，deallocate是静态函数，可能是这里存在什么作用
		// my_free_list = q;
	}
	
	inline void* alloc::reallocate(void* p,size_t old_size,size_t new_size)
	{
		deallocate(p,old_size);
		p = allocate(new_size);
		return p;
	}
	
	//bytes 对应上调到多少的区间
	inline size_t alloc::M_align(size_t bytes)
	{
		if (bytes <= 512)
		{
			return bytes <= 256 ? bytes <= 128 ? EAlign128:EAlign256
			:EAlign512;
		}
		return bytes <= 2048 ? bytes<=1024 ? EAlign1024:EAlign2048
		:EAlign4096;
	}
	
	// 将bytes上调至对应区间大小
    inline size_t alloc::M_round_up(size_t bytes)
    {
		return ((bytes + M_align(bytes)-1) & ~ (M_align(bytes)-1));
	)
	
	// 根据区块大小，选择第n个freelist
	inline size_t alloc::M_freelist_index(size_t bytes)
	{
		if (bytes <= 512)
		{
			return bytes <= 256 ? bytes <= 128 ?
                ((bytes+EAlign128-1)/EAlign128-1)
                : (15+(bytes+EAlign256-129)/EAlign256
                : (23+(bytes+EAlign512-257)/EAlign512);
        }
        return bytes <= 2048 ? bytes <= 128 ?
		    : (31 + (bytes + EAlign1024 -513) /EAlign1024
		    : (39 + (bytes + EAlign2048 -1025) /EAlign2048
		    : (47 + (bytes + EAlign4096 -2049) /EAlign4096;
	}
	
	// 重新填充freelist
	void* alloc::M_refill(size_t n)
	{
             			
	}
	
	char* alloc::M_chunk_alloc(size_t size,int& nblock)   
	{
		char *result;
		size_t need_bytes = size * nblock;
		size_t pool_bytes = end_free - start_free;
		
		if (pool_bytes>=need_bytes)
		{
			result = start_free;
			start_free = result + need_bytes;
			return result;
		}
		else if (pool_bytes>=size)
		{
			nblock = pool_bytes / size;
			need_bytes = size * nblock;
			result = start_free;
			start_free = result + need_bytes;;
			return result;
		}
		else
		{
			if(pool_bytes>0)
			{
				FreeList** my_free_list = free_list + M_freelist_index(pool_bytes);
				((FreeList*)start_free)->next = *my_free_list;
				*my_free_list = (FreeList*)start_free;
			}
				
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
		