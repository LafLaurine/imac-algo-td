#ifndef HEAP_H
#define HEAP_H

#include "array.h"

using std::size_t;


class BaseHeap : public Array
{
public:
	BaseHeap(size_t size=100);
	BaseHeap(const BaseHeap& other);
	BaseHeap(const std::vector<int>& other);

	virtual ~BaseHeap() {}

	virtual int leftChild(int nodeIndex);

	virtual int rightChild(int nodeIndex);

	virtual void insertHeapNode(int heapSize, int value);

	virtual void heapify(int heapSize, int nodeIndex);

	virtual void buildHeap(Array& numbers);

	virtual void heapSort();
};

#endif // HEAP_H
