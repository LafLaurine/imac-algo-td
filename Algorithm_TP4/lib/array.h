#ifndef ARRAY_H
#define ARRAY_H

#include <vector>
#include <stdexcept>
#include <time.h>
#include <chrono>
#include <thread>

#include <qglobal.h>

#include <QStringList>

#include "mainwindow.h"

using std::size_t;

class AbstractArray
{
public:
	AbstractArray(size_t size=100);
	AbstractArray(const AbstractArray& other);

	virtual ~AbstractArray() {}

	virtual void resize(size_t size);
	virtual void reserve(size_t size) = 0;
	virtual bool empty() const = 0;
	virtual size_t size() const = 0;
	virtual size_t effectiveSize() const = 0;
	virtual QString toString() const = 0;

	virtual void swap(const size_t index1, const size_t index2) = 0;

	bool hasBeenReadenAt(const size_t index);
	bool hasBeenWrittenAt(const size_t index);
	size_t readAccessCount() const {return _readAccess;}
	size_t writeAccessCount() const {return _writeAccess;}


protected:
	std::vector<bool> _hasBeenReaden;
	std::vector<bool> _hasBeenWritten;
	size_t _readAccess;
	size_t _writeAccess;
};

template <typename T>
class TemplateArray : public AbstractArray
{
public:
	typedef	T ElementType;
	typedef AbstractArray Base;

	TemplateArray(size_t size=100)
		: AbstractArray(size)
	{
		resize(size);
	}

	TemplateArray(const TemplateArray<ElementType>& other)
		: AbstractArray(other), _data(other._data)
	{}
	TemplateArray(const std::vector<ElementType>& other)
		: AbstractArray(other.size()), _data(other.cbegin(), other.cend())
	{}

	virtual ~TemplateArray() {}

	virtual void clear(int start, int end)
		{}

	virtual void resize(size_t size)
	{
		Base::resize(size);
		int oldSize = _data.size();
		if (size>0)
		{
			_data.resize(size);
			clear(oldSize, size);
		}
	}

	virtual void reserve(size_t size)
	{
		_data.reserve(size);
	}

	virtual bool empty() const
	{
		return _data.empty();
	}

	void push_back(const ElementType& value);

	bool contains(const ElementType& value) const;

	size_t size() const
	{
		return _data.size();
	}

	ElementType& get(const size_t index)
	{
		if(index >= _data.size())
			throw std::out_of_range("get(): Given index is out of bound !");
		_hasBeenReaden[index] = true;
		_readAccess++;
		std::this_thread::sleep_for(std::chrono::milliseconds(MainWindow::instruction_duration));
		return _data[index];
	}

	const ElementType& __get__(const size_t index) const
	{
		return _data[index];
	}

	ElementType& __get__(const size_t index)
	{
		return _data[index];
	}

	void set(const size_t index, const ElementType& value)
	{
		__set__(index, value);
	}

	void __set__(const size_t index, const ElementType& value)
	{
		if(index >= _data.size())
			throw std::out_of_range("set(): Given index is out of bound !");
		_data[index] = value;
	}

	void swap(const size_t index1, const size_t index2)
	{
		if(index1 >= _data.size() || index2 >= _data.size())
			throw std::out_of_range("set(): Given index is out of bound !");
		const ElementType tmp = _data[index1];
		_data[index1] = _data[index2];
		_data[index2] = tmp;
		_hasBeenWritten[index1] = _hasBeenWritten[index2] = true;
		_writeAccess += 2;
		_hasBeenReaden[index1] = _hasBeenReaden[index2] = true;
		_readAccess += 2;
		std::this_thread::sleep_for(std::chrono::milliseconds(MainWindow::instruction_duration*2));
	}

	bool contains(int value) const
	{
		for (int v : _data)
			if (v == value)
				return true;
		return false;
	}

	const ElementType& operator[](size_t index) const
	{
		return __get__(index);
	}

	ElementType& operator[](size_t index)
	{
		return __get__(index);
	}


	void operator=(const TemplateArray<ElementType>& other)
	{
		_data.assign(other._data.begin(), other._data.end());
		_hasBeenReaden.assign(other._hasBeenReaden.begin(),other._hasBeenReaden.end());
		_hasBeenWritten.assign(other._hasBeenWritten.begin(),other._hasBeenWritten.end());
		_readAccess = other._readAccess;
		_writeAccess = other._writeAccess;
	}

	void insert(const size_t index, const ElementType& value)
	{
		size_t i=index;
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

	const std::vector<ElementType>& data() const
	{
		return _data;
	}

protected:
	std::vector<ElementType> _data;
};

class Array : public TemplateArray<int>
{
public:
	Array(size_t size=100);
	Array(const TemplateArray<int>& other);
	Array(const std::vector<int>& other);

	virtual ~Array() {}

	void clear(int start, int end)
	{
		for (size_t i=start; i<end; ++i)
			_data[i] = -1;
	}

	size_t effectiveSize() const;

	void fillRandom(const int min, const int max);

	void fillSortedRandom(const int min, const int max);

	QString toString() const;
};

#endif // ARRAY_H
