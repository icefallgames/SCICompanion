#pragma once

// Use this from now on:
class CHint : public CObject
{
public:
    CHint(uint32_t thing) : Hint(thing) {}
    uint32_t Hint;
};

template<typename _THint>
CHint WrapHint(_THint t) { return CHint(static_cast<uint32_t>(t)); }

template<typename _THint>
_THint GetHint(CObject *pObj)
{
    if (pObj)
    {
        return static_cast<_THint>(((static_cast<CHint *>(pObj))->Hint));
    }
    return static_cast<_THint>(0);
}

template <typename _TObject>
class CHintWithObject : public CHint
{
public:
    CHintWithObject(uint32_t hint, const _TObject &object) : Object(object), CHint(hint)
    {
        // This is to prevent people from placing large objects in here to be copied.
        // I don't want to just take a reference to object, because these are commonly temporary values.
        // For large objects, use a pointer.
        static_assert(sizeof(_TObject) <= 8, "Consider using a pointer to pass information here.");
    }
    _TObject Object;    // Can't make this a reference! We put temp things here.
};

template<typename _TObject, typename _THint>
CHintWithObject<_TObject> WrapObject(_THint t, const _TObject &o) { return CHintWithObject<_TObject>(static_cast<uint32_t>(t), o); }

// The problem here is that we need to specify the hint in addition to the object. The only way we could get this to work is by
// saying our hints are always uint32_t
template<typename _TObject>
_TObject &UnwrapObject(CObject *pObj) { return ((CHintWithObject<_TObject> *)pObj)->Object; }



template< template < typename> class CObject >
std::false_type isCObject(...)
{
    return std::false_type();
}

template< template<typename> class CObject, typename T >
std::true_type isCObject(const CObject<T> &)
{
    return std::true_type();
}

template <template <typename> class CObject, typename T>
struct is_instantiation_of
{
    static decltype(isCObject<CObject>(std::declval<T>())) value;
};