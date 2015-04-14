#pragma once

// From the code project.

// The polymorphic base
template <typename T1, typename Result>
struct delegate_base { // Ref counting added 2002-09-22
    int ref_count; // delegate_base's are refcounted

    delegate_base() : ref_count(0) { }

    void addref() {
        ++ref_count;
    }

    void release() {
        if(--ref_count < 0)
            delete this;
    }

    virtual ~delegate_base() { } // Added 2002-09-22
    virtual Result operator()(T1 v1) = 0;
};

// The actual implementation of the delegate
template <typename Class, typename T1, typename Result>
struct delegate_impl : public delegate_base<T1, Result> {
    typedef Result (Class::* MethodType)(T1);
    Class*          obj;
    MethodType      method;

    delegate_impl(Class* obj_, MethodType method_) : 
            obj(obj_), method(method_) { }

    Result operator()(T1 v1) {
        return (obj->*method)(v1);
    }
};

template <typename T1, typename Result>
struct delegate {
    // Notice the type: delegate_base<T1, 
    // Result> - no Class in sight!
    delegate_base<T1, Result>*        pDelegateImpl;

    // The templated constructor - The presence of Class 
    // does not "pollute" the class itself
    template <typename Class, typename T1, typename Result>
    delegate(Class* obj, Result (Class::* method)(T1)) 
        : pDelegateImpl(new delegate_impl<Class, 
        T1, Result>(obj, method)) { 
            pDelegateImpl->addref(); // Added 2002-09-22
        }

        // Copy constructor and assignment operator 
        // added 2002-09-27
        delegate(const delegate<T1, 
            Result>& other) {
            pDelegateImpl = other.pDelegateImpl;
            pDelegateImpl->addref();
        }

        delegate<T1, Result>& operator=(const delegate<T1, 
            Result>& other) {
            pDelegateImpl->release();
            pDelegateImpl = other.pDelegateImpl;
            pDelegateImpl->addref();
            return *this;
        }

        ~delegate() { pDelegateImpl->release(); 
            } // Added & modified 2002-09-22

        // Forward the delegate to the delegate implementation
        Result operator()(T1 v1) {
            return (*pDelegateImpl)(v1);
        }
};