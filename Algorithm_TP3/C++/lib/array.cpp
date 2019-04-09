#include "array.h"
#include "mainwindow.h"

#include <stdexcept>
#include <time.h>
#include <chrono>
#include <thread>
#include <QThread>


Array::Array(uint size)
	: _readAccess(0), _writeAccess(0)
{
	if (size>0)
	{
		_data.resize(size);
		_hasBeenReaden.resize(size);
		_hasBeenWritten.resize(size);
	}
	for (uint i=0; i<size; ++i)
		_data[i] = -1;
}

Array::Array(const Array &other)
	: _data(other._data.begin(),other._data.end()),
	  _hasBeenReaden(other._hasBeenReaden.begin(),other._hasBeenReaden.end()),
	  _hasBeenWritten(other._hasBeenWritten.begin(),other._hasBeenWritten.end()),
	  _readAccess(other._readAccess),
	  _writeAccess(other._writeAccess)
{}

uint Array::effectiveSize() const
{
	int size=0;
	for (int value : _data)
		if (value < 0)
			return size;
		else
			size++;
	return size;
}

QString Array::toString() const
{
	QStringList list;
	for (int value : _data)
	{
		list.append(QString::number(value));
	}
	return QString("[%1]").arg(list.join(", "));
}

int &Array::get(const uint index)
{
	if(index >= _data.size())
		throw std::out_of_range("get(): Given index is out of bound !");
	_hasBeenReaden[index] = true;
	_readAccess++;
	std::this_thread::sleep_for(std::chrono::milliseconds(MainWindow::instruction_duration));
	return _data[index];
}

int Array::__get__(const uint index) const
{
	return _data[index];
}

void Array::set(const uint index, int value)
{
	if(index >= _data.size())
		throw std::out_of_range("set(): Given index is out of bound !");
	_hasBeenWritten[index] = true;
	_writeAccess++;
	std::this_thread::sleep_for(std::chrono::milliseconds(MainWindow::instruction_duration*2));
	_data[index] = value;
}

void Array::__set__(const uint index, const int value)
{
	if(index >= _data.size())
		throw std::out_of_range("set(): Given index is out of bound !");
	_data[index] = value;
}

void Array::swap(const uint index1, const uint index2)
{
	if(index1 >= _data.size() || index2 >= _data.size())
		throw std::out_of_range("set(): Given index is out of bound !");
	int tmp = _data[index1];
	_data[index1] = _data[index2];
	_data[index2] = tmp;
	_hasBeenWritten[index1] = _hasBeenWritten[index2] = true;
	_writeAccess += 2;
	_hasBeenReaden[index1] = _hasBeenReaden[index2] = true;
	_readAccess += 2;
	std::this_thread::sleep_for(std::chrono::milliseconds(MainWindow::instruction_duration*2));
}

bool Array::contains(int value) const
{
	for (int v : _data)
		if (v == value)
			return true;
	return false;
}

void Array::insert(const uint index, int value)
{
	uint i=index;
	while(i < _data.size()-1 && _data[i] >= 0)
		i++;
	while (i > index)
	{
		_data[i]=_data[i-1];
		++_writeAccess;
		_hasBeenWritten[i]=true;
		--i;
	}
	_data[index]=value;
	++_writeAccess;
	_hasBeenWritten[index]=true;
}

const std::vector<int>& Array::data() const
{
	return _data;
}

int& Array::operator[](const uint index)
{
	return get(index);
}

void Array::operator=(const Array &other)
{
	_data.assign(other._data.begin(),other._data.end());
	_hasBeenReaden.assign(other._hasBeenReaden.begin(),other._hasBeenReaden.end());
	_hasBeenWritten.assign(other._hasBeenWritten.begin(),other._hasBeenWritten.end());
	_readAccess = other._readAccess;
	_writeAccess = other._writeAccess;
}

void Array::fillRandom(const int min, const int max)
{
	const int _max = max - min;
	for (uint i=0; i<size(); ++i)
		_data[i] = qrand() % _max + min;
}

void Array::fillSortedRandom(const int min, const int max)
{
	int _max = (max - min) / 2;
	_data[0] = qrand() % _max + min;
	for (uint i=1; i<size(); ++i)
	{
		_max = ((max - _data[i-1]) - min) / 2;
		_data[i] = _data[i-1] + qrand() % _max + min;
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
