#include "pch.h"
#include "HashMap.h"
#include <iomanip>
#include <sstream>

// ������� ��� ������������ ��������� ������ �������
// c - ������ ������
// pos - ��������� �������
// f - �������, ������������� � ������� �������� �� ���� ������
//	��������� �� ���� ��� ���������:
//	1. ������ �������� � �������
//  2. ������� � ������� (����� ���� ��� ����������� �������, ��� � �� ����������� �������)
//  3. ����, ������������ ���� �������:
//		true - �� ��������� ������� �� ����� �������
//		false - �� ����� ������� �� ��������� ������� - 1
//  ���� ������� f ���������� true, �� ����� ������� �����������, ����� - ������������ ������.
template <typename C, typename F>
void CyclicFor(C & c, const int pos, const F & f)
{
	bool complited = false;
	for (int i = pos; !complited && i != c.size(); ++i)
	{
		if (f(i, c[i], true))
		{
			complited = true;
		}
	}
	
	for (int i = 0; !complited && i != pos; ++i)
	{
		if (f(i, c[i], false))
		{
			complited = true;
		}
	}
}

HashTable::HashTable()
{
}

void HashTable::SetLoadFactor(float factor)
{
	if (factor >= 0.0 && factor <= 1.0)
	{
		m_loadFactor = factor;
	}
}

void HashTable::Insert(uint64_t key)
{
	//if (m_size / (float)m_data.size() > m_loadFactor)
	{
		//HashTable table;
	}
	
	// ���� ������� ���������, �� �� ��������� �������
	if (IsFull())
	{
		std::cout << u8"��������: ���� ������� �������� ���� " << key << u8" � ����������� �������!" << std::endl;
		return;
	}

	// ���� ������ ������ ��� �������
	int pos = hash(key);
#ifdef _DEBUG
	// std::cout << "key <-> hash : " << key << " <-> " << pos << std::endl;
#endif
	
	Item * itemToInsert = nullptr;
	CyclicFor(m_data, pos, [&itemToInsert](int /*pos*/, Item & item, bool /*walkingToEnd*/)
	{
		if (item.isEmpty)
		{
			itemToInsert = &item;
			return true;
		}
		return false;
	});

	// ���������� � ��������� ������ ����
	// � ��������, ��� ��� �� �����
	itemToInsert->isEmpty = false;
	itemToInsert->key = key;
	++m_size;

	// ���� ������� ����������� �� �������, �������� �� ����������� ���-��������,
	// ������ ��������� �������� �� ����� key
	if (std::addressof(m_data[pos]) != itemToInsert)
	{
		std::cout << u8"��� ������� ���� ���������� ��������: ����� " << m_data[pos].key << u8" � " << key << u8" ����� ��� " << pos << u8"." << std::endl;
		++m_collisions;
	}
	
	if (m_size == m_data.size())
	{
		std::cout << u8"��� ������� ���������." << std::endl;
	}
}

bool HashTable::Contains(uint64_t key) const
{
	int pos = hash(key);
	const Item * item = FindItem(pos, key);
	if (!item || item->isEmpty)
	{
#ifdef _DEBUG
		auto it = std::find_if(m_data.begin(), m_data.end(), [key](Item item)
		{
			return !item.isEmpty && key == item.key;
		});
		if (it != m_data.end())
		{
			//std::cerr << "Contains(" << key << ") returned false, but key was found on " << std::distance(m_data.begin(), it) << " position!" << std::endl;
		}
#endif
		return false;
	}
	return true;
}

void HashTable::Remove(uint64_t key)
{
	int removingKeyPos = hash(key);
	auto item = FindItemToModify(removingKeyPos, key);
	if (!item || item->isEmpty)
	{
#ifdef _DEBUG
		auto it = std::find_if(m_data.begin(), m_data.end(), [key](Item item)
		{
			return !item.isEmpty && key == item.key;
		});
		if (it != m_data.end())
		{
			std::cerr << "Removing failed: key " << key << " was found on " << std::distance(m_data.begin(), it) << " position instead of " << removingKeyPos << "!" << std::endl;
		}
		else
		{
			std::cerr << "Removing failed: key " << key << " was not found!" << std::endl;
		}
#endif
		return;
	}
	
	int emptyCellPos = removingKeyPos;
	int movedCells = 0;
		
	// �������� �� ��������� ����������� �� ��� ���,
	// ���� �� ������� ������ ������.
	// ���� ������� ������, ��� ���-�������� ������ ��� ������ i-�� ������,
	// �� ������ ��� ������ � i-�� �������.
	CyclicFor(m_data, removingKeyPos, [this, &emptyCellPos, &movedCells, removingKeyPos](int i, Item & item, bool walkingToEnd)
	{
		if (item.isEmpty)
		{
			return true;
		}
	
		if (i == removingKeyPos)
		{
			item.Clear();
			emptyCellPos = i;
		}
		else
		{
			int _hash = hash(item.key);
			
			if (_hash != i)
			{
				if (i > _hash && i > emptyCellPos + (emptyCellPos >= _hash ? 0 : m_data.size()) ||
					i < _hash && i + m_data.size() > emptyCellPos + (emptyCellPos >= _hash ? 0 : m_data.size()))
				{
					m_data[emptyCellPos] = std::move(item);
					emptyCellPos = i;
					++movedCells;		
				}
			}
		}
		return false;
	});
	
	--m_size;
	
	if (movedCells > 0)
	{
		std::cerr << u8"�������� ����� " << key << u8" �������������� ����� " << movedCells  << u8" �����." << std::endl;
	}
}

string HashTable::PrintAllData() const
{
	auto max = std::max_element(m_data.begin(), m_data.end(), [](Item lhs, Item rhs) { return lhs.key < rhs.key; });
	auto str_max = std::to_string(max->key);

	std::stringstream ss;

	int l1, l2;
	l1 = std::to_string(m_data.size()).length() - 1;
	l2 = str_max.length();
	
	auto stub1 = string(str_max.length(), ' ');
	auto stub2 = string(l1, ' ');
	
	int i = 0;
	for (auto data : m_data)
	{
		if (data.isEmpty)
		{
			ss << "| " << std::setw(l1) << i << " | " <<  stub1 << " | " << stub2 << " |\n";
		}
		else
		{
			ss << "| " << std::setw(l1) << i << " | " <<  std::setw(l2) << data.key << " | " << std::setw(l1) << hash(data.key) << " |\n";
		}
		
		++i;
	}

	return ss.str();
}

HashTable::Item::Item()
	: key(0)
	, isEmpty(true)
{
}

HashTable::Item::Item(Item&& other) noexcept
	: Item()
{
	swap(other);
}

HashTable::Item& HashTable::Item::operator=(Item&& other) noexcept
{
	Item temp(std::move(other));
	swap(temp);
	return *this;
}

HashTable::Item::Item(const Item& other)
	: key(other.key)
	, isEmpty(other.isEmpty)
{	
}

HashTable::Item& HashTable::Item::operator=(const Item& other)
{
	Item temp(other);
	swap(temp);
	return *this;
}

void HashTable::Item::swap(Item& other) noexcept
{
	using std::swap;
	swap(other.isEmpty, isEmpty);
	swap(other.key, key);
}

void HashTable::Item::Clear()
{
	isEmpty = true;
	key = decltype(key)();
}

void HashTable::swap(Item& lhs, Item& rhs) noexcept
{
	lhs.swap(rhs);
}

int HashTable::hash(uint64_t key) const
{
	return key % m_data.size();
	
	uint64_t value = 0;
	
	auto bytes = &reinterpret_cast<const unsigned char&>(key);
	for (int i = 0; i < sizeof(key); ++i)
	{
		value ^= static_cast<uint64_t>(bytes[i]);
		value *= 31;
	}
	
	return int(value % m_data.size());
}

const HashTable::Item* HashTable::FindItem(int pos, uint64_t key) const
{
	// ���� ���� ������ ������, ���� ������� ����:
	const Item * foundItem = nullptr;
	CyclicFor(m_data, pos, [&foundItem, key](int /*pos*/, const Item & item, bool /*walkingToEnd*/)
	{
		if (item.isEmpty || item.key == key)
		{
			foundItem = &item;
			return true;
		}
		return false;
	});
	
	// ��� ������� ���� ����������� ���������, �� �������� �� �������
	return foundItem;
}

HashTable::Item* HashTable::FindItemToModify(int pos, uint64_t key)
{
	return const_cast<Item *>(FindItem(pos, key));
}