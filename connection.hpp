#ifndef CONNECTION_HPP_INCLUDED
#define CONNECTION_HPP_INCLUDED

#include <vector>
#include <string>
#include <queue>
#include <boost/thread.hpp>
#include <boost/any.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <meta.hpp>

#include "memstream.hpp"
#include "handlemessage.hpp"
#include "future.hpp"
#include "metaext.hpp"

using namespace boost;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace std;

typedef shared_ptr<Future<any> > ReplyPtr;

class ObjectHandle;

class Connection{
    friend class COFnWrap;
private:
    shared_ptr<bool> _term;
    volatile bool _sendready;
    tcp::iostream _s;
    queue<string> _sendqueue;
    condition_variable _sendcond;
    mutex _sqmut;
    mutex _rpmut;
    string _nsname;
    ObjectID_t _objectid;

    map<string, ReplyPtr> _replymap;
    map<string, meta::MetaClass> _regclasses;
    map<ObjectID_t, shared_ptr<meta::MetaObject> > _regobjects;
    map<ObjectID_t, shared_ptr<ObjectHandle> > _usedobjects;

    thread _sendthread;
    thread _recvthread;

    void Sender();
    void Reciever();

    void Recv_BrokerVersion(iostream &s);
    void Recv_ListNamespaces(iostream &s);
    void Recv_SetNamespace(iostream &s);
    void Recv_ListClasses(iostream &s);
    void Recv_GetClassDef(iostream &s);
    void Recv_GetObjectDef(iostream &s);
    void Recv_RegisterClass(iostream &s);
    void Recv_RegisterObject(iostream &s);
    void Recv_ConstructObject(iostream &s);
    void Recv_CallStatic(iostream &s);
    void Recv_CallMethod(iostream &s);
    void Recv_ReleaseObject(iostream &s);

    void Recv_ReqInfo(iostream &s);
    void Recv_ReqConstruct(iostream &s);
    void Recv_ReqStaticCall(iostream &s);
    void Recv_ReqMethodCall(iostream &s);
    void Recv_FreeObject(iostream &s);

public:
    Connection();
    ~Connection();

    void Send(const string &str);
    void SendNow(const string &str);
    ReplyPtr SetupReply(const string &nm);
    void PutReply(const string &nm, any val);
    any GetReply(const string &nm);

    BrokerDetails GetBrokerDetails();
    string SetNamesapce(const string &ns);
    vector<string> ListNamespaces();
    vector<string> ListClasses(const string &ns);
    void RegisterClass(const meta::MetaClass &cls);
    meta::MetaObject RegisterObject(const meta::MetaObject &obj);
    vector< ::MethodInfo> GetObjectDef(ObjectID_t oid);
    void ReleaseObject(ObjectID_t oid);

    bool TryGetObject(ObjectID_t oid, shared_ptr<ObjectHandle> &obj);
    void AddObject(ObjectID_t oid, shared_ptr<ObjectHandle> obj);

    ObjectID_t AddOwnedObject(shared_ptr<meta::MetaObject> obj);
    void RemoveOwnedObject(ObjectID_t oid);

    meta::MetaClass GetClass(const string &ns, const string &cls);
};

class ObjectHandle{
    public:
        ObjectHandle(Connection &conn, ObjectID_t oid) : _conn(conn), _oid(oid) {}
        ~ObjectHandle(){
            _conn.ReleaseObject(_oid);
        }
    private:
        Connection &_conn;
        ObjectID_t _oid;
};

#endif // CONNECTION_HPP_INCLUDED
