#include "hashtable.h"

#include "thread.h"
#include "mainwindow.h"

#include <QStringList>

string HashTable::names[] =
{
	"Yolo",
	"Barbara",
	"Yoann",
	"Antoine",
	"Clara",
	"Jules",
	"Ruben",
	"Margaux",
	"Pierre",
	"Laurelenn"
	"JoLeClodo",
	"Laura",
	"Flora",
	"Clara",
	"Nicolas",
	"Guillaume",
	"Mr.PeanutButter",
	"Eva",
	"Baptiste",
	"Solene",
	"Lucie",
	"Line",
	"Victorine",
	"Julien",
	"Roxanne"
};

HashTable::HashTable(uint size)
{
	resize(size);
}

HashTable::HashTable(const HashTable &other)
	: _data(other._data.begin(),other._data.end())
{}

HashTable::HashTable(const std::vector<string> &other)
	: _data(other.begin(),other.end())
{
	resize(other.size());
}

void HashTable::resize(uint size)
{
	int oldSize = _data.size();
	if (size>0)
	{
		_data.resize(size);
	}
	for (uint i=oldSize; i<size; ++i)
		_data[oldSize] = "\0";
}

uint HashTable::effectiveSize() const
{
	int size=0;
	for (string value : _data)
		if (value == "\0")
			return size;
		else
			size++;
	return size;
}

QString HashTable::toString() const
{
	QStringList list;
	for (string value : _data)
	{
		list.append(QString::fromStdString(value));
	}
	return QString("[%1]").arg(list.join(", "));
}

string &HashTable::get(const uint index)
{
	if(index >= _data.size())
		throw std::out_of_range("get(): Given index is out of bound !");
	std::this_thread::sleep_for(std::chrono::milliseconds(MainWindow::instruction_duration));
	return _data[index];
}

void HashTable::set(const uint index, string value)
{
	if(index >= _data.size())
		throw std::out_of_range("set(): Given index is out of bound !");
	std::this_thread::sleep_for(std::chrono::milliseconds(MainWindow::instruction_duration*2));
	_data[index] = value;
}

void HashTable::swap(const uint index1, const uint index2)
{
	if(index1 >= _data.size() || index2 >= _data.size())
		throw std::out_of_range("swap(): Given index is out of bound !");
	string tmp = _data[index1];
	_data[index1] = _data[index2];
	_data[index2] = tmp;
	std::this_thread::sleep_for(std::chrono::milliseconds(MainWindow::instruction_duration*2));
}

bool HashTable::contains(string value) const
{
	for (string v : _data)
		if (v == value)
			return true;
	return false;
}

void HashTable::insert(const uint index, string value)
{
	uint i=index;
	while(i < _data.size()-1 && _data[i] != "\0")
		i++;
	while (i > index)
	{
		_data[i]=_data[i-1];
		--i;
	}
	_data[index]=value;
}

const std::vector<string>& HashTable::data() const
{
	return _data;
}

string& HashTable::operator[](const uint index)
{
	return get(index);
}

void HashTable::operator=(const HashTable &other)
{
	_data.assign(other._data.begin(),other._data.end());
}

void HashTable::fillRandom()
{
	for (uint i=0; i<size(); ++i)
		_data[i] = names[qrand() % 20];
}
