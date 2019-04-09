#ifndef ARRAY_H
#define ARRAY_H

#include <vector>
#include <qglobal.h>

using std::size_t;

class Array
{
public:
	Array(uint size=100);
	Array(const Array& other);
	Array(const std::vector<int>& other);

	void reserve(uint size) {_data.reserve(size);}
	void push_back(int value)
	{
		_data.push_back(value);
		_hasBeenReaden.push_back(false);
		_hasBeenWritten.push_back(true);
	}
	bool empty() const {return _data.empty();}
	size_t size() const {return _data.size();}
	uint effectiveSize() const;
	QString toString() const;

	int& get(const uint index);
	int __get__(const uint index) const;
	void set(const uint index, const int value);
	void __set__(const uint index, const int value);
	void swap(const uint index1, const uint index2);

	bool contains(int value) const;

	int& operator[](const uint index);
	void operator=(const Array& other);

	void fillRandom(const int min = 0, const int max=RAND_MAX);
	void fillSortedRandom(const int min = 0, const int max=RAND_MAX);

	bool hasBeenReadenAt(const uint index);
	bool hasBeenWrittenAt(const uint index);
	uint readAccessCount() const {return _readAccess;}
	uint writeAccessCount() const {return _writeAccess;}

	void insert(const uint index, int value);
	const std::vector<int>& data() const;

private:
	std::vector<int> _data;
	std::vector<bool> _hasBeenReaden;
	std::vector<bool> _hasBeenWritten;
	uint _readAccess;
	uint _writeAccess;
};

#endif // ARRAY_H
