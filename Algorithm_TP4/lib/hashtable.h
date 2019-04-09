#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <string>
#include <vector>
#include <qglobal.h>

using std::string;

class HashTable
{
public:
	HashTable(uint size=100);
	HashTable(const HashTable& other);
	HashTable(const std::vector<string>& other);

	void resize(uint size);
	void reserve(uint size) {_data.reserve(size);}
	void push_back(string value)
	{
		_data.push_back(value);
	}
	bool empty() const {return _data.empty();}
	size_t size() const {return _data.size();}
	uint effectiveSize() const;
	QString toString() const;

	std::string &get(const uint index);
	void set(const uint index, std::string value);
	void swap(const uint index1, const uint index2);

	bool contains(std::string value) const;

	string& operator[](const uint index);
	void operator=(const HashTable& other);

	void fillRandom();

	void insert(const uint index, std::string value);
	const std::vector<std::string> &data() const;

	int hash(string element);
	void insert(string element);
	bool contains(string element);

	static string names[];
private:
	std::vector<string> _data;
};

#endif // HASHTABLE_H
