#pragma once
#include <string>
#include <vector>
using namespace std;

#include <stdlib.h>
#include <new>
#include <limits>
#include <map>

namespace MyAllocator {
	template <class T>
	struct allocator {
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef T& reference;
		typedef const T& const_reference;
		typedef T value_type;

		template <class U> struct rebind { typedef allocator<U> other; };
		allocator() throw() {}
		allocator(const allocator&) throw() {}

		template <class U> allocator(const allocator<U>&) throw() {}

		~allocator() throw() {}

		pointer address(reference x) const { return &x; }
		const_pointer address(const_reference x) const { return &x; }

		pointer allocate(size_type s, void const * = 0) {
			if (0 == s)
				return NULL;
			pointer temp = (pointer)malloc(s * sizeof(T));
			if (temp == NULL)
				throw std::bad_alloc();
			return temp;
		}

		void deallocate(pointer p, size_type) {
			free(p);
		}

		size_type max_size() const throw() {
			return std::numeric_limits<size_t>::max() / sizeof(T);
		}

		void construct(pointer p, const T& val) {
			new((void *)p) T(val);
		}

		void destroy(pointer p) {
			p->~T();
		}
	};
}

struct ptrdesc
{
	void* ptr;
	size_t size;
};

typedef vector<ptrdesc, MyAllocator::allocator<ptrdesc>>  vector_malloc;

class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();

	
	static void releaseAllocatedPointers();
	static unsigned int getAllocatedSpace();
	static void MemoryAllocated(void* p, size_t size);
	static void MemoryDeleted(void* p);

private:
	
	
	static vector_malloc m_allocatedPointers;
};

//overlad new and delete
void* operator new(size_t size);
void* operator new[](size_t size);
void operator delete(void* p);
void operator delete[](void* p);