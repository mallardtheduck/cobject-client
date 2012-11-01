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

META_METHOD(test, NO_RETURN, NO_PARAMS){
	return Nothing;
}

META_METHOD(array_total, int, MParams(vector<int>)){
	int ret=0;
	foreach(int i, args.get<0>()){
		ret+=i;
	}
	return ret;
}

META_METHOD(array_ones, vector<int>, MParams(int)){
	int t=args.get<0>();
	vector<int> ret;
	for(int i=0; i<t; ++i){
		ret.push_back(1);
	}
	return ret;
}

MetaClass TestClass(){
    static bool done=false;
    static MetaClass cls("testclass");
    if(!done){
        cls.AddMethod("hello", StdFn(hello));
        cls.AddMethod("saytimes", StdFn(saytimes));
        cls.AddMethod("getobject", StdFn(getobject));
        cls.AddMethod("callhello", StdFn(callhello));
        cls.AddMethod("test", StdFn(test));
        cls.AddMethod("array_total", StdFn(array_total));
        cls.AddMethod("array_ones", StdFn(array_ones));
        done=true;
    }
    return cls;
}
