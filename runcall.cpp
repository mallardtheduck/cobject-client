#include "runcall.hpp"

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/any.hpp>

#include "foreach.hpp"
#include "metaext.hpp"

using namespace boost;
using namespace std;
using namespace meta;

vector<any> VectorTVtoVectorAny(vector<TypedVal> vtv){
    vector<any> ret;
    foreach(TypedVal &tv, vtv){
        if(tv.type!=Types::Object){
            ret.push_back(tv.val);
        }
    }
    return ret;
}

void ProcessObjects(Connection &conn, const vector<TypedVal> &args, vector<any> &anyargs){
    assert(args.size()==anyargs.size());
    for(size_t i=0; i<args.size(); ++i){
        if(args[i].type==Types::Object){
            anyargs[i]=ObjectIDToMetaObject(conn, any_cast<ObjectID_t>(anyargs[i]));
        }
    }
}

void Do_RunConstruct(Connection *conn, CallID_t callid, MetaClass &cls, vector<TypedVal> args){
    shared_ptr<MetaObject> obj(NewPtr(cls));
    ObjectID_t oid=conn->AddOwnedObject(obj);
    stringstream out;
    Serialize(out, Messages::Construct);
    Serialize(out, callid);
    Serialize(out, StatusCodes::Success);
    Serialize(out, oid);
    conn->Send(out.str());
}

void Do_RunStaticCall(Connection *conn, CallID_t callid, MetaClass &cls, const string &method, vector<TypedVal> args){
    vector<any> anyargs=VectorTVtoVectorAny(args);
    ProcessObjects(*conn, args, anyargs);
    any ret=cls[method].Call(anyargs);
    shared_ptr<MetaObject> obj2;
    Type_t type=MapType(cls.GetMethodInfo(method).GetReturnType()->Info());
    if(type==Types::Object){
        MetaObject &obj=any_cast<MetaObject&>(ret);
        if(obj.GetState().Members.find("__OBJECT_ID")==obj.GetState().Members.end()){
            obj2.reset(new MetaObject(conn->RegisterObject(obj)));
            ret=obj2->GetState().Members["__OBJECT_ID"];
        }else{
            ret=obj.GetState().Members["__OBJECT_ID"];
        }
    }
    TypedVal tv;
    tv.val=ret;
    tv.type=type;
    stringstream out;
    Serialize(out, Messages::StaticCall);
    Serialize(out, callid);
    Serialize(out, StatusCodes::Success);
    Serialize(out, tv);
    conn->Send(out.str());
}

void Do_RunMethodCall(Connection *conn, CallID_t callid, shared_ptr<MetaObject> obj, const string &method, vector<TypedVal> args){
    vector<any> anyargs=VectorTVtoVectorAny(args);
    ProcessObjects(*conn, args, anyargs);
    any ret=(*obj)[method].Call(anyargs);
    shared_ptr<MetaObject> obj2;
    Type_t type=MapType(obj->GetMethodInfo(method).GetReturnType()->Info());
    if(type==Types::Object){
        MetaObject &obj=any_cast<MetaObject&>(ret);
        if(obj.GetState().Members.find("__OBJECT_ID")==obj.GetState().Members.end()){
            obj2.reset(new MetaObject(conn->RegisterObject(obj)));
            ret=obj2->GetState().Members["__OBJECT_ID"];
        }else{
            ret=obj.GetState().Members["__OBJECT_ID"];
        }
    }
    TypedVal tv;
    tv.val=ret;
    tv.type=type;
    stringstream out;
    Serialize(out, Messages::MethodCall);
    Serialize(out, callid);
    Serialize(out, StatusCodes::Success);
    Serialize(out, tv);
    conn->Send(out.str());
}

void RunConstruct(Connection &conn, CallID_t callid, MetaClass &cls, vector<TypedVal> args){
    thread dothread(bind(&Do_RunConstruct, &conn, callid, cls, args));
}

void RunStaticCall(Connection &conn, CallID_t callid, MetaClass &cls, const string &method, vector<TypedVal> args){
    thread dothread(bind(&Do_RunStaticCall, &conn, callid, cls, method, args));
}

void RunMethodCall(Connection &conn, CallID_t callid, shared_ptr<MetaObject> obj, const string &method, vector<TypedVal> args){
    thread dothread(bind(&Do_RunMethodCall, &conn, callid, obj, method, args));
}
