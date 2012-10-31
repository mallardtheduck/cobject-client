#include "serialize.hpp"

#include <stdexcept>

using namespace std;

namespace cobject
{

    void Serialize(ostream &s, const string &v)
    {
        Length_t len=v.length();
        Serialize(s, len);
        s.write(v.c_str(), len);
    }

    void Serialize(ostream &s, const wstring &v)
    {
        Length_t len=v.length();
        Serialize(s, len);
        s.write((char*)v.c_str(), len*sizeof(wchar_t));
    }

    void Serialize(ostream &s, MethodInfo info)
    {
        Serialize(s, info.MethodName);
        Serialize(s, info.IsStatic);
        Serialize(s, info.ReturnType);
        Serialize(s, info.ParamTypes);
    }

    void Serialize(ostream &s, ClassInfo info, bool name)
    {
        if (name) Serialize(s, info.ClassName);
        Serialize<Length_t>(s, info.Methods.size());
        for (size_t i=0; i<info.Methods.size(); i++)
        {
            Serialize(s, info.Methods[i]);
        }
    }
	
	void SerializeArray(ostream &s, const TypedVal &v)
	{
		Type_t elementType=ArrayElementType(v.type);
		if (elementType==Types::Int8)    	  Serialize(s, any_cast<vector<int8_t> >(v.val));
        else if (elementType==Types::Int16)   Serialize(s, any_cast<vector<int16_t> >(v.val));
        else if (elementType==Types::Int32)   Serialize(s, any_cast<vector<int32_t> >(v.val));
        else if (elementType==Types::Int64)   Serialize(s, any_cast<vector<int64_t> >(v.val));
        else if (elementType==Types::UInt8)   Serialize(s, any_cast<vector<uint8_t> >(v.val));
        else if (elementType==Types::UInt16)  Serialize(s, any_cast<vector<uint16_t> >(v.val));
        else if (elementType==Types::UInt32)  Serialize(s, any_cast<vector<uint32_t> >(v.val));
        else if (elementType==Types::UInt64)  Serialize(s, any_cast<vector<uint64_t> >(v.val));
        else if (elementType==Types::UTF8)    Serialize(s, any_cast<vector<char> >(v.val));
        else if (elementType==Types::UTF16)   Serialize(s, any_cast<vector<wchar_t> >(v.val));
        else if (elementType==Types::Float32) Serialize(s, any_cast<vector<float> >(v.val));
        else if (elementType==Types::Float64) Serialize(s, any_cast<vector<double> >(v.val));
        else if (elementType==Types::String)  Serialize(s, any_cast<vector<string> >(v.val));
        else if (elementType==Types::WString) Serialize(s, any_cast<vector<wstring> >(v.val));
        else if (elementType==Types::Bool)    Serialize(s, any_cast<vector<bool> >(v.val));
        else if (elementType==Types::Object)  Serialize(s, any_cast<vector<ObjectID_t> >(v.val));
		else if (IsArray(elementType))		  /*FIXME: Do something useful for arrays-of-arrays...*/;
        else if (elementType==Types::Void)    /*Do Nothing*/;
        else throw runtime_error(string("Invalid type: ") + elementType);
	}

    void Serialize(ostream &s, const TypedVal &v)
    {
        Serialize(s, v.type);
        if (v.type==Types::Int8)         Serialize(s, any_cast<int8_t>(v.val));
        else if (v.type==Types::Int16)   Serialize(s, any_cast<int16_t>(v.val));
        else if (v.type==Types::Int32)   Serialize(s, any_cast<int32_t>(v.val));
        else if (v.type==Types::Int64)   Serialize(s, any_cast<int64_t>(v.val));
        else if (v.type==Types::UInt8)   Serialize(s, any_cast<uint8_t>(v.val));
        else if (v.type==Types::UInt16)  Serialize(s, any_cast<uint16_t>(v.val));
        else if (v.type==Types::UInt32)  Serialize(s, any_cast<uint32_t>(v.val));
        else if (v.type==Types::UInt64)  Serialize(s, any_cast<uint64_t>(v.val));
        else if (v.type==Types::UTF8)    Serialize(s, any_cast<char>(v.val));
        else if (v.type==Types::UTF16)   Serialize(s, any_cast<wchar_t>(v.val));
        else if (v.type==Types::Float32) Serialize(s, any_cast<float>(v.val));
        else if (v.type==Types::Float64) Serialize(s, any_cast<double>(v.val));
        else if (v.type==Types::String)  Serialize(s, any_cast<string>(v.val));
        else if (v.type==Types::WString) Serialize(s, any_cast<wstring>(v.val));
        else if (v.type==Types::Bool)    Serialize(s, any_cast<bool>(v.val));
        else if (v.type==Types::Object)  Serialize(s, any_cast<ObjectID_t>(v.val));
		else if (IsArray(v.type))		 SerializeArray(s, v);
        else if (v.type==Types::Void)    /*Do Nothing*/;
        else throw runtime_error(string("Invalid type: ") + v.type);
    }

    void Deserialize(istream &s, string &v)
    {
        Length_t size;
        Deserialize(s, size);
        v.reserve(size);
        for (Length_t i=0; i<size; i++)
        {
            char c;
            s.read(&c, sizeof(c));
            v.push_back(c);
        }
    }

    void Deserialize(istream &s, wstring &v)
    {
        Length_t size;
        Deserialize(s, size);
        v.reserve(size);
        for (Length_t i=0; i<size; i++)
        {
            wchar_t c;
            s.read((char*)&c, sizeof(c));
            v.push_back(c);
        }
    }

    void Deserialize(istream &s, MethodInfo &info)
    {
        Deserialize(s, info.MethodName);
        Deserialize(s, info.IsStatic);
        Deserialize(s, info.ReturnType);
        Deserialize(s, info.ParamTypes);
    }

    void Deserialize(istream &s, ClassInfo &info, bool name)
    {
        if (name) Deserialize(s, info.ClassName);
        Length_t num;
        Deserialize(s, num);
        for (Length_t i=0; i<num; i++)
        {
            MethodInfo minfo;
            Deserialize(s, minfo);
            info.Methods.push_back(minfo);
        }
    }

    void Deserialize(istream &s, TypedVal &v)
    {
        Deserialize(s, v.type);
        string q=v.type;
        if (v.type==Types::Int8)
        {
            int8_t val;
            Deserialize(s, val);
            v.val=val;
        }
        else if (v.type==Types::Int16)
        {
            int16_t val;
            Deserialize(s, val);
            v.val=val;
        }
        else if (v.type==Types::Int32)
        {
            int32_t val;
            Deserialize(s, val);
            v.val=val;
        }
        else if (v.type==Types::Int64)
        {
            int64_t val;
            Deserialize(s, val);
            v.val=val;
        }
        else if (v.type==Types::UInt8)
        {
            uint8_t val;
            Deserialize(s, val);
            v.val=val;
        }
        else if (v.type==Types::UInt16)
        {
            uint16_t val;
            Deserialize(s, val);
            v.val=val;
        }
        else if (v.type==Types::UInt32)
        {
            uint32_t val;
            Deserialize(s, val);
            v.val=val;
        }
        else if (v.type==Types::UInt64)
        {
            uint64_t val;
            Deserialize(s, val);
            v.val=val;
        }
        else if (v.type==Types::UTF8)
        {
            char val;
            Deserialize(s, val);
            v.val=val;
        }
        else if (v.type==Types::UTF16)
        {
            wchar_t val;
            Deserialize(s, val);
            v.val=val;
        }
        else if (v.type==Types::Float32)
        {
            float val;
            Deserialize(s, val);
            v.val=val;
        }
        else if (v.type==Types::Float64)
        {
            double val;
            Deserialize(s, val);
            v.val=val;
        }
        else if (v.type==Types::String)
        {
            string val;
            Deserialize(s, val);
            v.val=val;
        }
        else if (v.type==Types::WString)
        {
            wstring val;
            Deserialize(s, val);
            v.val=val;
        }
        else if (v.type==Types::Bool)
        {
            bool val;
            Deserialize(s, val);
            v.val=val;
        }
        else if (v.type==Types::Object)
        {
            ObjectID_t val;
            Deserialize(s, val);
            v.val=val;
        }
        else if (v.type==Types::Void)
        {
            /*Do Nothing*/
        }
        else throw runtime_error(string("Invalid type: ") + v.type);
    }
}
