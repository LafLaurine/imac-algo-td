#ifndef ARRAY_H
#define ARRAY_H

#include <vector>
#include <list>
#include <qglobal.h>

class Array;

class Array
{
public:
	Array(uint size=100);
	Array(const Array& other);

	void reserve(uint size) {data.reserve(size);}
	void push_back(int value)
	{
		data.push_back(value);
		_hasBeenReaden.push_back(false);
		_hasBeenWritten.push_back(true);
	}
	bool empty() const {return data.empty();}
	size_t size() const {return data.size();}

	int& get(const uint index);
	int __get__(const uint index) const;
    void set(const uint index, const int value);
    void swap(const uint index1, const uint index2);

	int& operator[](const uint index);
	void operator=(const Array& other);

	void fillRandom(const int min = 0, const int max=RAND_MAX);
    void fillSortedRandom(const int min = 0, const int max=RAND_MAX);

	bool hasBeenReadenAt(const uint index);
	bool hasBeenWrittenAt(const uint index);
    uint readAccessCount() const {return _readAccess;}
    uint writeAccessCount() const {return _writeAccess;}

	static unsigned long instruction_duration;
    void insert(const uint index, int value);

	static void registerArray(Array& array);
	static uint totalReadAccess();
	static uint totalWriteAccess();
private:
	std::vector<int> data;
	std::vector<bool> _hasBeenReaden;
	std::vector<bool> _hasBeenWritten;
    uint _readAccess;
    uint _writeAccess;

	static std::list<Array*> instances;
};

#endif // ARRAY_H
