#include "testclass.hpp"

#include <string>

using namespace std;
using namespace meta;
using namespace boost;

META_METHOD(hello, string, NO_PARAMS){
    return string("Hello world!");
}

META_METHOD(saytimes, string, MParams(string, int)){
    string ret;
    for(int i=0; i<args.get<1>(); i++){
        ret+=args.get<0>();
    }
    return ret;
}

META_METHOD(getobject, MetaObject, NO_PARAMS){
    MetaClass cls("returnclass");
    cls.AddMethod("hello", StdFn(hello));
    return New(cls);
}

META_METHOD(callhello, string, MParams(MetaObject)){
    MetaObject &obj=args.get<0>();
    return obj["hello"].Call<string>();
}

MetaClass TestClass(){
    static bool done=false;
    static MetaClass cls("testclass");
    if(!done){
        cls.AddMethod("hello", StdFn(hello));
        cls.AddMethod("saytimes", StdFn(saytimes));
        cls.AddMethod("getobject", StdFn(getobject));
        cls.AddMethod("callhello", StdFn(callhello));
        done=true;
    }
    return cls;
}
