#ifndef FUTURE_HPP_INCLUDED
#define FUTURE_HPP_INCLUDED

#include <boost/thread.hpp>
using namespace boost;

//! A class to hold a value that will be filled in by another thread.
/*!
    This class works as follows - thread A creates a Future and passes a reference/pointer to it to thread B
    both threads continue processing. When thread A requires the value, it calls Get(). This will return immediately
    if thread B has already Set() a value, otherwise it blocks until it does.
*/
template<typename T> class Future{
    private:
        T _result;
        mutable condition_variable _cond;
        bool _avail;
        mutable mutex _mut;

    public:
        //! Constructor
        Future() : _avail(false){}
        /*!
            Get the held value
            \return The value held. If value has not been Set(), blocks until it is.
        */
        T& Get(){
            unique_lock<mutex> lock(_mut);
            while(!_avail) _cond.wait(lock);
            return _result;
        }
        /*!
            Get the held value (constant version)
            \return The value held. If value has not been Set(), blocks until it is.
        */
        const T& Get() const{
            unique_lock<mutex> lock(_mut);
            while(!_avail) _cond.wait(lock);
            return _result;
        }

        /*!
            Set the held value - and wake up a waiting thread if necissary.
            \param value    The value to set
        */
        void Set(T value){
            _result=value;
            {
                lock_guard<mutex> lock(_mut);
                _avail=true;
            }
            _cond.notify_all();
        }
};

#endif // FUTURE_HPP_INCLUDED
