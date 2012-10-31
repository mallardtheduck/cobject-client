#include "typemap.hpp"

#include <typeinfo>
#include <boost/cstdint.hpp>
#include <string>
#include <meta/meta.hpp>

using namespace boost;

namespace cobject
{

    Type_t MapType(const std::type_info &t)
    {
        if (t==typeid(int8_t)) return Types::Int8;
        if (t==typeid(int16_t)) return Types::Int16;
        if (t==typeid(int32_t)) return Types::Int32;
        if (t==typeid(int64_t)) return Types::Int64;
        if (t==typeid(uint8_t)) return Types::UInt8;
        if (t==typeid(uint16_t)) return Types::UInt16;
        if (t==typeid(uint32_t)) return Types::UInt32;
        if (t==typeid(uint64_t)) return Types::UInt64;
        if (t==typeid(char)) return Types::UTF8;
        if (t==typeid(wchar_t)) return Types::UTF16;
        if (t==typeid(float)) return Types::Float32;
        if (t==typeid(double)) return Types::Float64;
        if (t==typeid(std::string)) return Types::String;
        if (t==typeid(std::wstring)) return Types::WString;
        if (t==typeid(bool)) return Types::Bool;
        if (t==typeid(meta::MetaObject)) return Types::Object;
        if (t==typeid(NullType)) return Types::Void;
        return MapArrayType(t);
    }
	
	Type_t MapArrayType(const std::type_info &t)
	{
		if (t==typeid(vector<int8_t>)) return Types::ArrayPrefix + Types::Int8;
        if (t==typeid(vector<int16_t>)) return Types::ArrayPrefix + Types::Int16;
        if (t==typeid(vector<int32_t>)) return Types::ArrayPrefix + Types::Int32;
        if (t==typeid(vector<int64_t>)) return Types::ArrayPrefix + Types::Int64;
        if (t==typeid(vector<uint8_t>)) return Types::ArrayPrefix + Types::UInt8;
        if (t==typeid(vector<uint16_t>)) return Types::ArrayPrefix + Types::UInt16;
        if (t==typeid(vector<uint32_t>)) return Types::ArrayPrefix + Types::UInt32;
        if (t==typeid(vector<uint64_t>)) return Types::ArrayPrefix + Types::UInt64;
        if (t==typeid(vector<char>)) return Types::ArrayPrefix + Types::UTF8;
        if (t==typeid(vector<wchar_t>)) return Types::ArrayPrefix + Types::UTF16;
        if (t==typeid(vector<float>)) return Types::ArrayPrefix + Types::Float32;
        if (t==typeid(vector<double>)) return Types::ArrayPrefix + Types::Float64;
        if (t==typeid(vector<std::string>)) return Types::ArrayPrefix + Types::String;
        if (t==typeid(vector<std::wstring>)) return Types::ArrayPrefix + Types::WString;
        if (t==typeid(vector<bool>)) return Types::ArrayPrefix + Types::Bool;
        if (t==typeid(vector<meta::MetaObject>)) return Types::ArrayPrefix + Types::Object;
        if (t==typeid(vector<NullType>)) return Types::ArrayPrefix + Types::Void;
		return Types::ArrayPrefix + Types::Unknown;
	}
	
	bool IsArray(Type_t type)
	{
		if(type.length() < Types::ArrayPrefix.length()) return false;
		for(size_t i=0; i < Types::ArrayPrefix.length(); ++i)
		{
			if(type[i] != Types::ArrayPrefix[i]) return false;
		}
		return true;
	}
	
	Type_t ArrayElementType(Type_t array)
	{
		if(!IsArray(array)) throw runtime_error("Not an array!");
		return array.substr(Types::ArrayPrefix.length());
	}
}
