#include "lib/TP4.h"

#include "lib/mainwindow.h"
#include <QApplication>
#include <time.h>
#include <stdio.h>

HeapTestMainWindow* w = nullptr;
using std::size_t;
using std::string;

void HuffmanHeap::insertHeapNode(int heapSize, char c, int frequences){}

void HuffmanNode::insertNode(HuffmanNode* node) {}

void HuffmanNode::processCodes(std::string baseCode){}

void charFrequences(string data, Array& frequences){}

void huffmanHeap(Array& frequences, HuffmanHeap& heap){}

Heap::Heap(size_t size)
	: BaseHeap(size)
{}

Heap::Heap(const Heap &other)
	: BaseHeap(other)
{}

Heap::Heap(const std::vector<int> &other)
	: BaseHeap(other)
{}

int Heap::leftChild(int nodeIndex)
{
    return (nodeIndex*2)+1;
}

int Heap::rightChild(int nodeIndex)
{
    return (nodeIndex*2)+2;
}

void Heap::insertHeapNode(int heapSize, int value)
{
    int i = heapSize;
    this->get(i) = value;
    while(i > 0 && (this->get(i) > this->get(i/2))) {
        swap(i,i/2);
        i = i/2;
    }
}

void Heap::heapify(int heapSize, int nodeIndex)
{
    if (nodeIndex >= heapSize)
        return;
    int largest = nodeIndex;
    int left = leftChild(nodeIndex);
    int right= rightChild(nodeIndex);

    if(left < heapSize) {
        if(this->get(largest) < this->get(left) )
            largest = left;
    }

    if(right < heapSize) {
        if(this->get(largest) < this->get(right))
            largest = right;
    }

    if (largest != nodeIndex)
    {
        swap(nodeIndex, largest);
        heapify(heapSize, largest);
    }
}

void Heap::buildHeap(Array& numbers)
{
    heapify(numbers.size(),numbers[0]);
}

void Heap::heapSort()
{
    for (int i=size()-1; i>0; --i)
    {
        swap(0, i);
        heapify(i, 0);
    }
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow::instruction_duration = 50;
	w = new HeapWindow<Heap>();
	w->show();

	return a.exec();
}
