#include "ResourceManager.h"
ResourceManager::ResourceManager()
{

}

ResourceManager::~ResourceManager()
{

}


vector_malloc ResourceManager::m_allocatedPointers;

unsigned int ResourceManager::getAllocatedSpace()
{
	size_t totalSpace = 0;
	vector_malloc::iterator vIter;
	
	for (vIter = m_allocatedPointers.begin(); vIter < m_allocatedPointers.end(); vIter++)
		totalSpace += vIter->size;
	
	return totalSpace;
}

void ResourceManager::releaseAllocatedPointers()
{
	vector_malloc::iterator vIter;
	for (vIter = m_allocatedPointers.begin(); vIter < m_allocatedPointers.end(); vIter++)
	{
		if (vIter->ptr)
		{
			free(vIter->ptr);
			m_allocatedPointers.erase(vIter);
		}
	}
}

void ResourceManager::MemoryAllocated(void* p, size_t size)
{
	ptrdesc desc;
	desc.ptr = p;
	desc.size = size;

	m_allocatedPointers.push_back(desc);

}

void ResourceManager::MemoryDeleted(void* p)
{
	vector_malloc::iterator vIter;
	for (vIter = m_allocatedPointers.begin(); vIter < m_allocatedPointers.end(); vIter++)
	{
		if (vIter->ptr == p)
		{
			m_allocatedPointers.erase(vIter);
			break;
		}
	}


}

void* operator new(size_t size)
{
	void* p;
	p = malloc(size);

	if (!p)
	{
		bad_alloc ba;
		throw ba;
	}

	ResourceManager::MemoryAllocated(p, size);
	return p;
}

void* operator new[](size_t size)
{

	void* p;
	p = malloc(size);

	if (!p)
	{
		bad_alloc ba;
		throw ba;
	}

	ResourceManager::MemoryAllocated(p, size);
	return p;

}

void operator delete(void* p)
{
	free(p);
	ResourceManager::MemoryDeleted(p);
}

void operator delete[](void* p)
{
	
	free(p);
	ResourceManager::MemoryDeleted(p);
}