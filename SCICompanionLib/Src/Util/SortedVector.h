#pragma once

template <class T, class Compare = std::less<T> >
struct sorted_vector {
    std::vector<T> V;
    Compare cmp;
    typedef typename std::vector<T>::iterator iterator;
    typedef typename std::vector<T>::const_iterator const_iterator;
    iterator begin() { return V.begin(); }
    iterator end() { return V.end(); }
    const_iterator begin() const { return V.begin(); }
    const_iterator end() const { return V.end(); }

    typedef T value_type;
    
    bool empty() const { return V.empty(); }
    size_t size() const { return V.size(); }

    sorted_vector(const Compare& c = Compare())
        : V(), cmp(c)
    {
        // Provides a slight speed up in our scenario
        V.reserve(20);
    }

    template <class InputIterator>
    sorted_vector(InputIterator first, InputIterator last,
        const Compare& c = Compare())
        : V(first, last), cmp(c)
    {
        std::sort(begin(), end(), cmp);
    }

    std::pair<iterator, bool> insert(const T& t)
    {
        iterator i = std::lower_bound(begin(), end(), t, cmp);
        if (i == end() || cmp(t, *i))
        {
            return std::pair<iterator, bool>(V.insert(i, t), true);
        }
        else
        {
            return std::pair<iterator, bool>(i, false);
        }
    }

    iterator insert(iterator positionHint, const T& t)
    {
        // positionHint is a hint. For performance, we could do something with it.
        return insert(t).first;
    }

    void insert(iterator start, iterator end)
    {
        while (start != end)
        {
            insert(*start);
            ++start;
        }
    }

    void erase(const T& t)
    {
        iterator i = std::lower_bound(begin(), end(), t, cmp);
        if (i != end())
        {
            V.erase(i);
        }
    }

    const_iterator find(const T& t) const
    {
        const_iterator i = std::lower_bound(begin(), end(), t, cmp);
        return i == end() || cmp(t, *i) ? end() : i;
    }


    bool operator==(const sorted_vector<T, Compare> &other) const
    {
        return V == other.V;
    }

    bool operator!=(const sorted_vector<T, Compare> &other) const
    {
        return !(*this == other);
    }

};