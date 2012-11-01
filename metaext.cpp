#include "metaext.hpp"
#include "serialize.hpp"

#include <stdexcept>

using namespace std;
using namespace meta;

namespace cobject
{

    CallID_t COFnWrap::_callid=0;

    Type_t GetReturnType(const meta::MethodInfo &mth)
    {
        return MapType(mth.GetReturnType()->Info());
    }

    vector<Type_t> GetParamTypes(const meta::MethodInfo &mth)
    {
        vector<Type_t> ret;
        vector<PolyWrapper<ITypeInfo> > tps=mth.GetParamTypes();
        for (size_t i=0; i<tps.size(); i++)
        {
            ret.push_back(MapType(tps[i]->Info()));
        }
        return ret;
    }

    ClassInfo MetaClassInfo(const MetaClass &cls)
    {
        ClassInfo ret;
        ret.ClassName=cls.GetName();
        map<string, meta::MethodInfo> minfo=cls.GetMethodInfo();
        foreach(Q(pair<string, meta::MethodInfo>) p, minfo)
        {
            MethodInfo i;
            i.MethodName=p.first;
            i.IsStatic=cls.IsStatic(p.first);
            i.ReturnType=GetReturnType(p.second);
            i.ParamTypes=GetParamTypes(p.second);
            ret.Methods.push_back(i);
        }
        return ret;
    }

    MetaClass MakeMetaClass(Connection &conn, const string &nsname, ClassInfo cls)
    {
        MetaClass ret(cls.ClassName);
        foreach(MethodInfo &i, cls.Methods)
        {
            ret.AddMethod(i.MethodName, COFnWrap(conn, nsname, cls.ClassName, i));
        }
        MethodInfo mi;
        mi.MethodName="ctor";
        mi.ReturnType=Types::Void;
        if (!ret.HasMethod("ctor")) ret.AddMethod("ctor", COFnWrap(conn, nsname, cls.ClassName, mi));
        return ret;
    }

    PolyWrapper<ITypeInfo> TypeToTypeInfo(const Type_t &type)
    {
        if (type==Types::Int8)
            return TypeID2<int8_t>();
        if (type==Types::Int16)
            return TypeID2<int16_t>();
        if (type==Types::Int32)
            return TypeID2<int32_t>();
        if (type==Types::Int64)
            return TypeID2<int64_t>();
        if (type==Types::UInt8)
            return TypeID2<uint8_t>();
        if (type==Types::UInt16)
            return TypeID2<uint16_t>();
        if (type==Types::UInt32)
            return TypeID2<uint32_t>();
        if (type==Types::UInt64)
            return TypeID2<uint64_t>();
        if (type==Types::UTF8)
            return TypeID2<char>();
        if (type==Types::UTF16)
            return TypeID2<wchar_t>();
        if (type==Types::String)
            return TypeID2<string>();
        if (type==Types::WString)
            return TypeID2<wstring>();
        if (type==Types::Bool)
            return TypeID2<bool>();
        if (type==Types::Object)
            return TypeID2<MetaObject>();
        if(IsArray(type)) return ArrayTypeToTypeInfo(type);
        return TypeID2<NullType>();
    }
    
    PolyWrapper<ITypeInfo> ArrayTypeToTypeInfo(const Type_t &type)
    {
    	Type_t elementType=ArrayElementType(type);
        if (elementType==Types::Int8)
            return TypeID2<vector<int8_t> >();
        if (elementType==Types::Int16)
            return TypeID2<vector<int16_t> >();
        if (elementType==Types::Int32)
            return TypeID2<vector<int32_t> >();
        if (elementType==Types::Int64)
            return TypeID2<vector<int64_t> >();
        if (elementType==Types::UInt8)
            return TypeID2<vector<uint8_t> >();
        if (elementType==Types::UInt16)
            return TypeID2<vector<uint16_t> >();
        if (elementType==Types::UInt32)
            return TypeID2<vector<uint32_t> >();
        if (elementType==Types::UInt64)
            return TypeID2<vector<uint64_t> >();
        if (elementType==Types::UTF8)
            return TypeID2<vector<char> >();
        if (elementType==Types::UTF16)
            return TypeID2<vector<wchar_t> >();
        if (elementType==Types::String)
            return TypeID2<vector<string> >();
        if (elementType==Types::WString)
            return TypeID2<vector<wstring> >();
        if (elementType==Types::Bool)
            return TypeID2<vector<bool> >();
        if (elementType==Types::Object)
            return TypeID2<vector<MetaObject> >();
        return TypeID2<vector<NullType> >();
    }

    MetaObject ObjectIDToMetaObject(Connection &conn, ObjectID_t oid)
    {
        vector<MethodInfo> objectdef=conn.GetObjectDef(oid);
        stringstream clsname;
        clsname << "OBJECT" << oid;
        MetaClass cls(clsname.str());
        MetaObject obj=New(cls);
        foreach(MethodInfo &i, objectdef)
        {
            obj.AddMethod(i.MethodName, COFnWrap(conn, "NOT_STATIC_OR_CTOR", clsname.str(), i));
        }
        boost::shared_ptr<ObjectHandle> objh;
        if (!conn.TryGetObject(oid, objh))
        {
            objh.reset(new ObjectHandle(conn, oid));
            conn.AddObject(oid, objh);
        }
        obj.GetState().Members["__OBJECTHANDLE"] = objh;
        obj.GetState().Members["__OBJECT_ID"] = oid;
        return obj;
    }

    COFnWrap::COFnWrap(Connection &c, const string &nsname, const string &clsname, MethodInfo &i) : _conn(c), _info(i), _nsname(nsname), _classname(clsname)
    {
        SetStatic(_info.IsStatic);
    }

    bool COFnWrap::TypeCheck(const ITypeInfo &, const ITypeInfo &)
    {
        return true;
    }

    any COFnWrap::Call(MetaInfo &info, any args)
    {
        return Call(info, info.Context.ManyArgs.Get()());
    }

    any COFnWrap::Call(MetaInfo &info, many args)
    {
        MessageID_t msgID;
        vector<MetaObject> objects; //< Objects that need to be held in scope.
        if (_info.MethodName=="ctor") msgID=Messages::ConstructObject;
        else if (IsStatic()) msgID=Messages::CallStatic;
        else msgID=Messages::CallMethod;
        stringstream out;
        Serialize(out, msgID);
        if (msgID==Messages::CallStatic || msgID==Messages::ConstructObject)
        {
            Serialize(out, _nsname);
            Serialize(out, _classname);
        }
        if (msgID==Messages::CallMethod)
        {
            ObjectID_t oid;
            oid=any_cast<ObjectID_t>(info.State.Members["__OBJECT_ID"]);
            Serialize(out, oid);
        }
        if (msgID==Messages::CallMethod || msgID==Messages::CallStatic)
        {
            Serialize(out, _info.MethodName);
        }
        CallID_t callid=++_callid;
        Serialize(out, callid);
        vector<TypedVal> pargs;
        for (size_t i=0; i<_info.ParamTypes.size();++i)
        {
            TypedVal tv(_info.ParamTypes[i], args[i]);
            if (tv.type==Types::Object)
            {
                MetaObject &obj=any_cast<MetaObject&>(tv.val);
                meta::MetaState state=obj.GetState();
                if (state.Members.find("__OBJECT_ID")==state.Members.end())
                {
                    MetaObject nobj=_conn.RegisterObject(obj);
                    objects.push_back(nobj);
                    tv.val=any_cast<ObjectID_t>(nobj.GetState().Members["__OBJECT_ID"]);
                }
                else
                {
                    tv.val=any_cast<ObjectID_t>(obj.GetState().Members["__OBJECT_ID"]);
                }
            }
            pargs.push_back(tv);
        }
        Serialize(out, pargs);
        stringstream replyhandle;
        replyhandle << "Call:" << msgID << ":" << callid;
        _conn.SetupReply(replyhandle.str());
        _conn.Send(out.str());
        any reply=_conn.GetReply(replyhandle.str());
        if (msgID==Messages::ConstructObject)
        {
            ConstructResult res=any_cast<ConstructResult>(reply);
            if (res.status==StatusCodes::Success) info.State.Members["__OBJECT_ID"]=res.objectid;
            return any(Nothing);
        }
        if (msgID==Messages::CallStatic || msgID==Messages::CallMethod)
        {
            TypedVal res=any_cast<TypedVal>(reply);
            if (res.type!=Types::Object)return res.val;
            else
            {
                MetaObject obj=ObjectIDToMetaObject(_conn, any_cast<ObjectID_t>(res.val));
                return obj;
            }
        }
        throw std::runtime_error("Call failure");
    }

    PolyWrapper<ITypeInfo> COFnWrap::GetReturnType()
    {
        return TypeToTypeInfo(_info.ReturnType);
    }

    vector<PolyWrapper<ITypeInfo> > COFnWrap::GetParamTypes()
    {
        vector<PolyWrapper<ITypeInfo> > ret;
        foreach(Type_t &t, _info.ParamTypes)
        {
            ret.push_back(TypeToTypeInfo(t));
        }
        return ret;
    }
}
