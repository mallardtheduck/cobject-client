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
        return Types::Unknown;
    }
}
