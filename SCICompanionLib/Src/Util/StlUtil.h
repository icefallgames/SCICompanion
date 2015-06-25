#pragma once


template<typename _InIt>
_InIt match_name(_InIt _First, _InIt _Last, const std::string &name)
{
    for (;_First != _Last; _First++)
    {
        if ((*_First)->GetName() == name)
        {
            break;
        }
    }
    return _First;
}

template < typename T >
T* match_name(const std::vector<std::unique_ptr<T>> &container, const std::string &name)
{
	for (auto &thing : container)
	{
		if (thing->GetName() == name)
		{
			return thing.get();
		}
	}
	return nullptr;
}

template<typename _InIt>
_InIt match_name_dot(_InIt _First, _InIt _Last, const std::string &name)
{
    for (;_First != _Last; _First++)
    {
        if ((*_First).GetName() == name)
        {
            break;
        }
    }
    return _First;
}

template<typename _InIt>
bool matches_name(_InIt _First, _InIt _Last, const std::string &name)
{
	_First = match_name(_First, _Last, name);
	return _First != _Last;
}

template<typename T>
void add_smart_to_raw(const std::vector<std::unique_ptr<T>> &src, std::vector<T*> &dest)
{
	std::transform(src.begin(), src.end(), std::back_inserter(dest),
		[](const std::unique_ptr<T> &thing)
	{
		return thing.get();
	});
}

// Used for looking up items in const array where a string maps something to another value.
template<typename _TKey, typename _TValue>
struct key_value_pair
{
    _TKey key;
    _TValue value;
};

template<typename _T>
_T LookupStringValue(const key_value_pair<PCSTR, _T> *rgArray, size_t size, const std::string &name, _T defaultValue = nullptr)
{
    _T value = defaultValue;
    bool fRet = FALSE;
    for (size_t i = 0; !fRet && i < size; i++)
    {
        fRet = (name == rgArray[i].key);
        if (fRet)
        {
            value = rgArray[i].value;
        }
    }
    return value;
}

template<typename _TKey, typename _TValue>
_TValue LookupKeyValue(const key_value_pair<_TKey, _TValue> *rgArray, size_t size, const _TKey &key, const _TValue &defValue)
{
    _TValue value = defValue;
    for (size_t i = 0; i < size; i++)
    {
        if (key == rgArray[i].key)
        {
            value = rgArray[i].value;
            break;
        }
    }
    return value;
}

template<typename _TKey, typename _TValue>
_TKey LookupKey(const key_value_pair<_TKey, _TValue> *rgArray, size_t size, const _TValue &value, const _TKey &defKey)
{
    _TKey key = defKey;
    for (size_t i = 0; i < size; i++)
    {
        if (value == rgArray[i].value)
        {
            key = rgArray[i].key;
            break;
        }
    }
    return key;
}

template<typename _TKey, typename _TValue>
bool FindKeyValue(const key_value_pair<_TKey, _TValue> *rgArray, size_t size, const _TKey &key, _TValue &value)
{
    bool fRet = false;
    for (size_t i = 0; !fRet && i < size; i++)
    {
        fRet = (key == rgArray[i].key);
        if (fRet)
        {
            value = rgArray[i].value;
        }
    }
    return fRet;
}

template<typename _TKey, typename _TValue>
bool FindKey(const key_value_pair<_TKey, _TValue> *rgArray, size_t size, const _TValue &value, _TKey &key)
{
    bool fRet = false;
    for (size_t i = 0; !fRet && i < size; i++)
    {
        fRet = (value == rgArray[i].value);
        if (fRet)
        {
            key = rgArray[i].key;
        }
    }
    return fRet;
}



void push_word(std::vector<BYTE> &output, WORD w);
void write_word(std::vector<BYTE> &output, size_t index, WORD w);
void push_string(std::vector<BYTE> &output, const std::string &str);
void push_string_rle(std::vector<BYTE> &output, const std::string &str);
void push_string_nt(std::vector<BYTE> &output, const std::string &str);
WORD push_temp(std::vector<BYTE> &output);

template<typename _TContainer, typename _InIt>
void push_strings_nt(_TContainer &cont, _InIt _First, _InIt _Last)
{
    for (;_First != _Last; ++_First)
    {
        push_string_nt(cont, *_First);
    }
}

// C++ provides for
//  - vector: a dynamically size array
//  - array: an array fixed at runtime
// But it does not provide for an array that has a fixed size determined at
//  runtime. This is what this does.
// It also has the performance advantage that it doesn't need to allocate
// values to the data when constructed (unlike vector).
// It's also a small size than vector (8 bytes instead of 16)
namespace sci
{
    template<typename _T>
    class array
    {
    public:
        array() : _size(0), _data(nullptr) {}
        array(size_t size) : array() { _allocateInternal(size); }

        array(const array &src)
        {
            _allocateInternal(src._size);
            if (_size > 0)
            {
                memcpy(_data, src._data, _size);
            }
        }

        array &operator=(const array &src)
        {
            delete[] _data;
            _allocateInternal(src._size);
            if (_size > 0)
            {
                memcpy(_data, src._data, _size);
            }
            return *this;
        }

        ~array() { delete[]  _data; }


        void allocate(size_t size)
        {
            delete[] _data;
            _allocateInternal(size);
        }

        void assign(const _T *begin, const _T *end)
        {
            assert((end - begin) <= (ptrdiff_t)_size);
            _T *curThis = _data;
            for (const _T *cur = begin; cur != end; ++cur, ++curThis)
            {
                *curThis = *cur;
            }
        }

        void swap(array &src)
        {
            std::swap(_data, src._data);
            std::swap(_size, src._size);
        }

        void fill(_T value)
        {
            for (size_t i = 0; i < _size; i++)
            {
                _data[i] = value;
            }
        }

        void fill(size_t position, size_t length, _T value)
        {
            assert((position + length) <= _size);
            for (size_t i = position; i < (position + length); i++)
            {
                _data[i] = value;
            }
        }

        _T *begin() { return _data; }
        _T *end() { return _data + _size; }

        size_t size() const { return _size; }
        bool empty() const { return _size == 0; }

        _T& operator[](size_t index)
        {
            return _data[index];
        }

        const _T& operator[](size_t index) const
        {
            return _data[index];
        }

    private:
        void _allocateInternal(size_t size)
        {
            if (size == 0)
            {
                _data = nullptr;
            }
            else
            {
                _data = new _T[size];
            }
            _size = size;
        }

        _T *_data;
        size_t _size;
    };
}

// A remove_if for associative containers.
template< typename ContainerT, typename PredicateT >
void erase_if(ContainerT& items, const PredicateT& predicate) {
    for (auto it = items.begin(); it != items.end();) {
        if (predicate(*it)) it = items.erase(it);
        else ++it;
    }
};

// We can to top and pop in one instruction for PODs. No worry about exceptions.
template<typename _T>
_T* pop_ptr(std::stack<_T*> &leStack)
{
    _T* ptr = leStack.top();
    leStack.pop();
    return ptr;
}

template<class T> inline
bool contains(const T& container, const typename T::reference value)
{
    return container.find(value) != container.end();
}
