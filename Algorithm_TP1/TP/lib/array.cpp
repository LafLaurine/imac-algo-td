#include "array.h"

#include <stdexcept>
#include <time.h>
#include <chrono>
#include <thread>
#include <QThread>

unsigned long Array::instruction_duration = 500;
std::list<Array*> Array::instances;

Array::Array(uint size)
	: _readAccess(0), _writeAccess(0)
{
	if (size>0)
	{
		data.resize(size);
		_hasBeenReaden.resize(size);
		_hasBeenWritten.resize(size);
	}
	for (uint i=0; i<size; ++i)
		data[i] = -1;
}

Array::Array(const Array &other)
	: data(other.data.begin(),other.data.end()),
	  _hasBeenReaden(other._hasBeenReaden.begin(),other._hasBeenReaden.end()),
	  _hasBeenWritten(other._hasBeenWritten.begin(),other._hasBeenWritten.end()),
	  _readAccess(other._readAccess),
	  _writeAccess(other._writeAccess)
{}

int& Array::get(const uint index)
{
	if(index >= data.size())
		throw std::out_of_range("get(): Given index is out of bound !");
	_hasBeenReaden[index] = true;
	_readAccess++;
	std::this_thread::sleep_for(std::chrono::milliseconds(Array::instruction_duration));
	return data[index];
}

int Array::__get__(const uint index) const
{
	return data[index];
}

void Array::set(const uint index, int value)
{
	if(index >= data.size())
		throw std::out_of_range("set(): Given index is out of bound !");
	_hasBeenWritten[index] = true;
    _writeAccess++;
    std::this_thread::sleep_for(std::chrono::milliseconds(Array::instruction_duration*2));
	data[index] = value;
}

void Array::swap(const uint index1, const uint index2)
{
    if(index1 >= data.size() || index2 >= data.size())
        throw std::out_of_range("set(): Given index is out of bound !");
    int tmp = data[index1];
    data[index1] = data[index2];
    data[index2] = tmp;
    _hasBeenWritten[index1] = _hasBeenWritten[index2] = true;
    _writeAccess += 2;
    _hasBeenReaden[index1] = _hasBeenReaden[index2] = true;
    _readAccess += 2;
    std::this_thread::sleep_for(std::chrono::milliseconds(Array::instruction_duration*3));
}

void Array::insert(const uint index, int value)
{
	uint i=index;
	while(i < data.size()-1 && data[i] >= 0)
		i++;
	while (i > index)
	{
		data[i]=data[i-1];
		++_writeAccess;
		_hasBeenWritten[i]=true;
		--i;
	}
	data[index]=value;
	++_writeAccess;
	_hasBeenWritten[index]=true;
}

void Array::registerArray(Array &array)
{
	instances.push_back(&array);
}

uint Array::totalReadAccess()
{
	uint sum=0;
	for (Array* array : instances)
		sum += array->readAccessCount();
	return sum;
}

uint Array::totalWriteAccess()
{
	uint sum=0;
	for (Array* array : instances)
		sum += array->writeAccessCount();
	return sum;
}

int& Array::operator[](const uint index)
{
	return get(index);
}

void Array::operator=(const Array &other)
{
	data.resize(other.size());
	_hasBeenReaden.resize(other.size());
	_hasBeenWritten.resize(other.size());

	data.assign(other.data.begin(),other.data.end());
	_hasBeenReaden.assign(_hasBeenReaden.size(), false);
	_hasBeenWritten.assign(_hasBeenReaden.size(), true);
}

void Array::fillRandom(const int min, const int max)
{
	const int _max = max - min;
	for (uint i=0; i<size(); ++i)
		data[i] = qrand() % _max + min;
}

void Array::fillSortedRandom(const int min, const int max)
{
    int _max = max - min;
    data[0] = qrand() % _max + min;
    for (uint i=1; i<size(); ++i)
    {
        _max = (max - data[i]) - min;
        data[i] = data[i] + qrand() % _max + min;
    }
}


bool Array::hasBeenReadenAt(const uint index)
{
	bool _result = _hasBeenReaden[index];
	_hasBeenReaden[index] = false;
	return _result;
}

bool Array::hasBeenWrittenAt(const uint index)
{
	bool _result = _hasBeenWritten[index];
	_hasBeenWritten[index] = false;
	return _result;
}
