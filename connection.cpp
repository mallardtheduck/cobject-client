#include "connection.hpp"

#include <boost/bind.hpp>
#include <unistd.h>
#include "foreach.hpp"
#include "serialize.hpp"
#include "runcall.hpp"
#include "debug.hpp"

using namespace std;

namespace cobject
{

    Connection::Connection(const string &host, const string &port) :
            _term(new bool(false)),
            _sendready(false),
            _sconnect(host, port),
            _socket(_io_service, ip::tcp::v4(), _sconnect.rdbuf()->native_handle()),
            _sin(_socket),
	    _sout(_socket),
            _sendthread(bind(&Connection::Sender,this)),
            _recvthread(bind(&Connection::Reciever,this))
    {
    }

    Connection::~Connection()
    {
        *_term=true;
        _sendthread.interrupt();
        _recvthread.interrupt();
        sleep(1);
    }

    void Connection::Sender()
    {
        boost::shared_ptr<bool> term=_term;
        _sout.rdbuf()->pubsetbuf(0, 0);
        while (!_sendready);
        while (!*term)
        {
            unique_lock<mutex> lock(_sqmut);
            while (_sendqueue.empty() && !*term) _sendcond.wait(lock);
            string &str=_sendqueue.front();
            DEBUG(cout << "Sending " << str.length() << " bytes..." << endl);
            _sout.write(str.c_str(), str.length());
            _sout.flush();
	    //std::cout << "Error: " << _sout.error().message() << std::endl;
            _sendqueue.pop();
        }
    }

    void Connection::Reciever()
    {
        boost::shared_ptr<bool> term=_term;
        string welcome;
        Deserialize(_sin, welcome);
        DEBUG(cout << welcome << endl);
        _sendready=true;
        while (!*term)
        {
            MessageID_t msgid;
            Deserialize(_sin, msgid);
            DEBUG(cout << "Recieved MessageID " << msgid << " from broker." << endl);
            switch (msgid)
            {
            case Messages::BrokerDetails:
                Recv_BrokerVersion(_sin);
                break;
            case Messages::SetNamespace:
                Recv_SetNamespace(_sin);
                break;
            case Messages::ListNamespaces:
                Recv_ListNamespaces(_sin);
                break;
            case Messages::ListClasses:
                Recv_ListClasses(_sin);
                break;
            case Messages::RegisterClass:
                Recv_RegisterClass(_sin);
                break;
            case Messages::GetClassDef:
                Recv_GetClassDef(_sin);
                break;
            case Messages::GetObjectDef:
                Recv_GetObjectDef(_sin);
                break;
            case Messages::RegisterObject:
                Recv_RegisterObject(_sin);
                break;
            case Messages::ConstructObject:
                Recv_ConstructObject(_sin);
                break;
            case Messages::CallStatic:
                Recv_CallStatic(_sin);
                break;
            case Messages::CallMethod:
                Recv_CallMethod(_sin);
                break;
            case Messages::ReleaseObject:
                Recv_ReleaseObject(_sin);
                break;
            case Messages::Construct:
                Recv_ReqConstruct(_sin);
                break;
            case Messages::StaticCall:
                Recv_ReqStaticCall(_sin);
                break;
            case Messages::MethodCall:
                Recv_ReqMethodCall(_sin);
                break;
            case Messages::FreeObject:
                Recv_FreeObject(_sin);
                break;
            }
        }
    }

    void Connection::Send(const string &str)
    {
        lock_guard<mutex> lock(_sqmut);
        _sendqueue.push(str);
        _sendcond.notify_one();
    }

    ReplyPtr Connection::SetupReply(const string &nm)
    {
        lock_guard<mutex> lock(_rpmut);
        ReplyPtr fptr(new Future<any>());
        _replymap[nm]=fptr;
        return fptr;
    }

    void Connection::PutReply(const string &nm, any val)
    {
        lock_guard<mutex> lock(_rpmut);
        _replymap[nm]->Set(val);
    }

    any Connection::GetReply(const string &nm)
    {
        ReplyPtr ptr;
        {
            lock_guard<mutex> lock(_rpmut);
            ptr=_replymap[nm];
        }
        any ret=ptr->Get();
        _replymap.erase(nm);
        return ret;
    }

    BrokerDetails Connection::GetBrokerDetails()
    {
        stringstream out;
        SetupReply("BrokerDetails");
        Serialize(out, Messages::BrokerDetails);
        Send(out.str());
        any ret=GetReply("BrokerDetails");
        return any_cast<BrokerDetails>(ret);
    }

    void Connection::Recv_BrokerVersion(iostream &s)
    {
        BrokerDetails ret;
        Deserialize(s, ret.ProtocolVersion);
        Deserialize(s, ret.BrokerName);
        Deserialize(s, ret.Vendor);
        Deserialize(s, ret.DisplayVersion);
        PutReply("BrokerDetails", ret);
    }

    string Connection::SetNamespace(const string &ns)
    {
        stringstream out;
        SetupReply("SetNamespace");
        Serialize(out, Messages::SetNamespace);
        Serialize(out, ns);
        Send(out.str());
        any ret=GetReply("SetNamespace");
        _nsname=any_cast<string>(ret);
        return _nsname;
    }

    void Connection::Recv_SetNamespace(iostream &s)
    {
        string ns;
        Deserialize(s, ns);
        PutReply("SetNamespace", ns);
    }

    vector<string> Connection::ListNamespaces()
    {
        stringstream out;
        SetupReply("ListNamespaces");
        Serialize(out, Messages::ListNamespaces);
        Send(out.str());
        any ret=GetReply("ListNamespaces");
        return any_cast<vector<string> >(ret);
    }

    void Connection::Recv_ListNamespaces(iostream &s)
    {
        vector<string> ret;
        Deserialize(s, ret);
        PutReply("ListNamespaces", ret);
    }

    vector<string> Connection::ListClasses(const string &ns)
    {
        stringstream out;
        Serialize(out, Messages::ListClasses);
        Serialize(out, ns);
        string replyslot=string("ListClasses:")+ns;
        SetupReply(replyslot);
        Send(out.str());
        any ret=GetReply(replyslot);
        return any_cast<vector<string> >(ret);
    }

    void Connection::Recv_ListClasses(iostream &s)
    {
        string ns;
        Deserialize(s, ns);
        vector<string> lst;
        Deserialize(s, lst);
        PutReply(string("ListClasses:")+ns, lst);
    }

    void Connection::RegisterClass(const MetaClass &cls)
    {
        stringstream s;
        _regclasses[cls.GetName()]=cls;
        ClassInfo info=MetaClassInfo(cls);
        Serialize(s, Messages::RegisterClass);
        Serialize(s, info);
        string replyslot=string("RegisterClass:")+info.ClassName;
        SetupReply(replyslot);
        Send(s.str());
        any ret=GetReply(replyslot);
    }

    void Connection::Recv_RegisterClass(iostream &s)
    {
        string ClassName;
        Deserialize(s, ClassName);
        PutReply(string("RegisterClass:")+ClassName, true);
    }

    MetaObject Connection::RegisterObject(const MetaObject &obj)
    {
        ObjectID_t newoid=++_objectid;
        _regobjects[newoid]=boost::shared_ptr<MetaObject>(new MetaObject(obj));
        stringstream s;
        Serialize(s, Messages::RegisterObject);
        Serialize(s, newoid);
        Serialize(s, MetaClassInfo(obj).Methods);
        stringstream replyhandle;
        replyhandle << "RegisterObject:" << newoid;
        SetupReply(replyhandle.str());
        Send(s.str());
        any reply=GetReply(replyhandle.str());
        ObjectID_t asoid=any_cast<ObjectID_t>(reply);
        return ObjectIDToMetaObject(*this, asoid);
    }

    void Connection::Recv_RegisterObject(iostream &s)
    {
        ObjectID_t oid, newoid;
        Deserialize(s, oid);
        Deserialize(s, newoid);
        stringstream replyhandle;
        replyhandle << "RegisterObject:" << oid;
        PutReply(replyhandle.str(), newoid);
    }

    MetaClass Connection::GetClass(const string &ns, const string &cls)
    {
        stringstream out;
        Serialize(out, Messages::GetClassDef);
        Serialize(out, ns);
        Serialize(out, cls);
        string replyslot=string("GetClassDef:")+ns+"::"+cls;
        SetupReply(replyslot);
        Send(out.str());
        any ret=GetReply(replyslot);
        return MakeMetaClass(*this, ns, any_cast<ClassInfo>(ret));
    }

    void Connection::Recv_GetClassDef(iostream &s)
    {
        string ns, cls;
        Deserialize(s, ns);
        Deserialize(s, cls);
        ClassInfo info;
        Deserialize(s, info);
        string replyslot=string("GetClassDef:")+ns+"::"+cls;
        PutReply(replyslot, info);
    }

    vector< MethodInfo> Connection::GetObjectDef(ObjectID_t oid)
    {
        stringstream out;
        Serialize(out, Messages::GetObjectDef);
        Serialize(out, oid);
        stringstream replyslot;
        replyslot << "GetObjectDef:" << oid;
        SetupReply(replyslot.str());
        Send(out.str());
        any reply=GetReply(replyslot.str());
        return any_cast<vector<MethodInfo> >(reply);
    }

    void Connection::Recv_GetObjectDef(iostream &s)
    {
        ObjectID_t oid;
        Deserialize(s, oid);
        vector<MethodInfo> info;
        Deserialize(s, info);
        stringstream replyslot;
        replyslot << "GetObjectDef:" << oid;
        PutReply(replyslot.str(), info);
    }

    void Connection::Recv_ConstructObject(iostream &s)
    {
        CallID_t callid;
        ConstructResult res;
        Deserialize(s, callid);
        Deserialize(s, res.status);
        Deserialize(s, res.objectid);
        stringstream replyhandle;
        replyhandle << "Call:" << Messages::ConstructObject << ":" << callid;
        PutReply(replyhandle.str(), res);
    }

    void Connection::Recv_CallStatic(iostream &s)
    {
        TypedVal res;
        CallID_t callid;
        StatusCode_t status;
        Deserialize(s, callid);
        Deserialize(s, status);
        Deserialize(s, res);
        stringstream replyhandle;
        replyhandle << "Call:" << Messages::CallStatic << ":" << callid;
        PutReply(replyhandle.str(), res);
    }

    void Connection::Recv_CallMethod(iostream &s)
    {
        TypedVal res;
        CallID_t callid;
        StatusCode_t status;
        Deserialize(s, callid);
        Deserialize(s, status);
        Deserialize(s, res);
        stringstream replyhandle;
        replyhandle << "Call:" << Messages::CallMethod << ":" << callid;
        PutReply(replyhandle.str(), res);
    }

    void Connection::ReleaseObject(ObjectID_t oid)
    {
        stringstream out;
        Serialize(out, Messages::ReleaseObject);
        Serialize(out, oid);
        stringstream replyhandle;
        replyhandle << "ReleaseObject:" << oid;
        SetupReply(replyhandle.str());
        Send(out.str());
        GetReply(replyhandle.str());
    }

    void Connection::Recv_ReleaseObject(iostream &s)
    {
        ObjectID_t oid;
        Deserialize(s, oid);
        StatusCode_t status;
        Deserialize(s, status);
        stringstream replyhandle;
        replyhandle << "ReleaseObject:" << oid;
        PutReply(replyhandle.str(), status);
    }

    void Connection::Recv_ReqInfo(iostream &s)
    {
        stringstream out;
        Serialize(out, (uint32_t)0);
        Serialize(out, string("COBJECT-CLIENT"));
        Serialize(out, string("COBJECT"));
        Serialize(out, string("COBJECT-CLIENT DEV"));
        Send(out.str());
    }

    void Connection::Recv_ReqConstruct(iostream &s)
    {
        string clsname;
        Deserialize(s, clsname);
        CallID_t cid;
        Deserialize(s, cid);
        vector<TypedVal> args;
        Deserialize(s, args);
        RunConstruct(*this, cid, _regclasses[clsname], args);
    }

    void Connection::Recv_ReqStaticCall(iostream &s)
    {
        string clsname, mthname;
        Deserialize(s, clsname);
        Deserialize(s, mthname);
        CallID_t cid;
        Deserialize(s, cid);
        vector<TypedVal> args;
        Deserialize(s, args);
        RunStaticCall(*this, cid, _regclasses[clsname], mthname, args);
    }

    void Connection::Recv_ReqMethodCall(iostream &s)
    {
        ObjectID_t oid;
        Deserialize(s, oid);
        string mthname;
        Deserialize(s, mthname);
        CallID_t cid;
        Deserialize(s, cid);
        vector<TypedVal> args;
        Deserialize(s, args);
        RunMethodCall(*this, cid, _regobjects[oid], mthname, args);
    }

    void Connection::Recv_FreeObject(iostream &s)
    {
        ObjectID_t oid;
        Deserialize(s, oid);
        _regobjects.erase(oid);
    }

    bool Connection::TryGetObject(ObjectID_t oid, boost::shared_ptr<ObjectHandle> &obj)
    {
        if (_usedobjects.find(oid)!=_usedobjects.end())
        {
            obj=_usedobjects[oid];
            return true;
        }
        else return false;
    }

    void Connection::AddObject(ObjectID_t oid, boost::shared_ptr<ObjectHandle> obj)
    {
        _usedobjects[oid]=obj;
    }

    ObjectID_t Connection::AddOwnedObject(boost::shared_ptr<MetaObject> obj)
    {
        ObjectID_t oid=++_objectid;
        _regobjects[oid]=obj;
        return oid;
    }
}
