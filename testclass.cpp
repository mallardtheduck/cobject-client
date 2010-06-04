#include "testclass.hpp"

#include <string>

using namespace std;
using namespace meta;
using namespace boost;

META_METHOD(hello, string, NO_PARAMS){
    return "Hello world!";
}

META_METHOD(saytimes, string, Q(tuple<string, int>)){
    string ret;
    for(int i=0; i<args.get<1>(); i++){
        ret+=args.get<0>();
    }
    return ret;
}

MetaClass TestClass(){
    static bool done=false;
    static MetaClass cls("testclass");
    if(!done){
        cls.AddMethod("hello", StdFn(hello));
        cls.AddMethod("saytimes", StdFn(saytimes));
        done=true;
    }
    return cls;
}
