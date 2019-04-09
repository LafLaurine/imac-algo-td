#include "array.h"
#include "mainwindow.h"


#include <QThread>


AbstractArray::AbstractArray(size_t size)
	: _readAccess(0), _writeAccess(0)
{
	resize(size);
}

AbstractArray::AbstractArray(const AbstractArray &other)
	: _hasBeenReaden(other._hasBeenReaden.begin(),other._hasBeenReaden.end()),
	  _hasBeenWritten(other._hasBeenWritten.begin(),other._hasBeenWritten.end()),
	  _readAccess(other._readAccess),
	  _writeAccess(other._writeAccess)
{}

void AbstractArray::resize(size_t size)
{
	if (size>0)
	{
		_hasBeenReaden.resize(size);
		_hasBeenWritten.resize(size);
	}
}

bool AbstractArray::hasBeenReadenAt(const size_t index)
{
	bool _result = _hasBeenReaden[index];
	_hasBeenReaden[index] = false;
	return _result;
}

bool AbstractArray::hasBeenWrittenAt(const size_t index)
{
	bool _result = _hasBeenWritten[index];
	_hasBeenWritten[index] = false;
	return _result;
}

Array::Array(size_t size)
	: TemplateArray<int> (size)
{
	clear(0, size);
}

Array::Array(const TemplateArray<int>& other)
	: TemplateArray<int> (other)
{}

Array::Array(const std::vector<int>& other)
	: TemplateArray<int> (other)
{}

size_t Array::effectiveSize() const
{
	int size=0;
	for (int value : _data)
		if (value < 0)
			return size;
		else
			size++;
	return size;
}

void Array::fillRandom(const int min, const int max)
{
	const int _max = max - min;
	for (size_t i=0; i<size(); ++i)
		_data[i] = qrand() % _max + min;
}

void Array::fillSortedRandom(const int min, const int max)
{
	int _max = (max - min) / 2;
	_data[0] = qrand() % _max + min;
	for (size_t i=1; i<size(); ++i)
	{
		_max = ((max - _data[i-1]) - min) / 2;
		_data[i] = _data[i-1] + qrand() % _max + min;
	}
}

QString Array::toString() const
{
	QStringList list;
	for (const ElementType& value : _data)
	{
		list.append(QString::number(value));
	}
	return QString("[%1]").arg(list.join(", "));
}

