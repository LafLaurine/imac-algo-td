#include "heap.h"

BaseHeap::BaseHeap(size_t size)
	: Array(size)
{}

BaseHeap::BaseHeap(const BaseHeap &other)
	: Array(other)
{}

BaseHeap::BaseHeap(const std::vector<int> &other)
	: Array(other)
{}

int BaseHeap::leftChild(int nodeIndex)
{
	{return 0;}
}

int BaseHeap::rightChild(int nodeIndex) {return 0;}

void BaseHeap::insertHeapNode(int heapSize, int value) {}

void BaseHeap::heapify(int heapSize, int nodeIndex) {}

void BaseHeap::buildHeap(Array& numbers) {}

void BaseHeap::heapSort() {}
