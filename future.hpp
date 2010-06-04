#ifndef FUTURE_HPP_INCLUDED
#define FUTURE_HPP_INCLUDED

#include <boost/thread.hpp>
using namespace boost;

template<typename T> class Future{
    private:
        T _result;
        mutable condition_variable _cond;
        bool _avail;
        mutable mutex _mut;

    public:
        Future() : _avail(false){}
        /*Future(const Future<T>& rhs){
            Set(rhs.Get());
        }*/
        T& Get(){
            unique_lock<mutex> lock(_mut);
            while(!_avail) _cond.wait(lock);
            return _result;
        }
        const T& Get() const{
            unique_lock<mutex> lock(_mut);
            while(!_avail) _cond.wait(lock);
            return _result;
        }

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
