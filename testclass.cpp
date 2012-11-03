#include "testclass.hpp"
#include "typemap.hpp"
#include <string>

using namespace std;
using namespace meta;
using namespace boost;

typedef std::map<string, boost::any> Hash;

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

META_METHOD(hash_count, int, MParams(Hash)){
	int t=0;
	for(auto i=args.get<0>().begin(); i!=args.get<0>().end(); ++i){
		++t;
	}
	cout << "hash_count" << endl;
	return t;
}

META_METHOD(hash_alphabet, Hash, NO_PARAMS){
	string k="a";
	Hash ret;
	for(int i=0; i<26; ++i){
		++k[0];
		ret[k]=i;
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
        cls.AddMethod("hash_count", StdFn(hash_count));
        cls.AddMethod("hash_alphabet", StdFn(hash_alphabet));
        done=true;
    }
    return cls;
}
