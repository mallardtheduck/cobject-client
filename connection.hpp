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

namespace cobject
{

    typedef shared_ptr<Future<any> > ReplyPtr; //!< Typedef for reply slot

    class ObjectHandle;

    //! Connection class
    /*!
        This class handles a connection to a CrossObject broker. It handles all incoming messages,
        records all state information and holds all registered classes/objects.
    */
    class Connection
    {
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
        //! Constructor
        /*!
            Create a new connection.
            \param host The host to connect to
            \param port The TCP port to connect to
        */
        Connection(const string &host="127.0.0.1", const string &port="3300");
        //! Destructor
        ~Connection();

        /*!
            Asynchrously send data through the underlying socket connection
        */
        void Send(const string &str);

        /*!
            Create a "reply slot" for recieving an incoming message.
            \param nm   The unique name to be used for this reply slot - must agree with the format used by the
                        recieving code.
            \return A pointer to the reply slot
        */
        ReplyPtr SetupReply(const string &nm);
        /*!
            Put data in a reply slot - also wakes up code waiting for the reply
            \param nm   The name of the reply slot
            \param val  The data to put in it
        */
        void PutReply(const string &nm, any val);
        /*!
            Get data from reply slot. If no data is yet available, block unit it is.
            \param nm   The name of the reply slot
            \return  The data in the reply slot
        */
        any GetReply(const string &nm);

        /*!
            Get version information about the broker connected to
            \return The broker version details
        */
        BrokerDetails GetBrokerDetails();
        /*!
            Set the namespace for this application/connection
            \param ns   The requested namespace
            \return The actual assigned namespace
        */
        string SetNamespace(const string &ns);
        /*!
            List the namespaces known by the broker
            \return A list of known namespaces
        */
        vector<string> ListNamespaces();
        /*!
            List the classes in a namespace
            \param ns   The namespace to query
            \return A list of classes registered in the namespace
        */
        vector<string> ListClasses(const string &ns);
        /*!
            Register a class with the broker
            \param cls  The class to register
        */
        void RegisterClass(const meta::MetaClass &cls);
        /*!
            Register an object with the broker
            \param obj  The object to register
            \return A call-proxy for the registered object - initially this will be the only reference
                    to the object in existence, so it must be kept until another reference (i.e. in
                    another application) can be created.
        */
        meta::MetaObject RegisterObject(const meta::MetaObject &obj);
        /*!
            Get the definition of an object used by this application
            \param oid  The ID of the object
            \return A list of the object's method specifacations
        */
        vector<MethodInfo> GetObjectDef(ObjectID_t oid);
        /*!
            Release an object from being used by this connection
            \param oid  The ID of the obejct
        */
        void ReleaseObject(ObjectID_t oid);

        /*!
            Attempt to get an existing object handle for an object ID
            \param oid  The object ID
            \param obj  A pointer to hold the object handle
            \return true if an object handle was found and assigned to obj, false otherwise
        */
        bool TryGetObject(ObjectID_t oid, shared_ptr<ObjectHandle> &obj);
        /*!
            Add a new handle to the list of objects referenced
            \param oid  The object ID
            \param obj  A pointer to the object handle
        */
        void AddObject(ObjectID_t oid, shared_ptr<ObjectHandle> obj);

        /*!
            Add an object to the list of objects owned by this connection - without registering it
            This is used during the construction of new objects.
            \param obj  The object to add
            \return An ObjectID for referring to the object
        */
        ObjectID_t AddOwnedObject(shared_ptr<meta::MetaObject> obj);
        /*!
            Remove an object from the list of objects owned.
            Used in object destruction
            \param oid  The ObjectID to remove
        */
        void RemoveOwnedObject(ObjectID_t oid);

        /*!
            Get a class registered with the broker
            \param ns   The namespace that the class is registered in
            \param cls  The name of the class
            \return A proxy for the class
        */
        meta::MetaClass GetClass(const string &ns, const string &cls);
    };

    //! A handle to an object used by this connection
    class ObjectHandle
    {
    public:
        /*!
            Constructor
            \param conn     The connection
            \param oid      The ObjectID of the object
        */
        ObjectHandle(Connection &conn, ObjectID_t oid) : _conn(conn), _oid(oid) {}
        /*!
            Destructor - releases object
        */
        ~ObjectHandle()
        {
            _conn.ReleaseObject(_oid);
        }
    private:
        Connection &_conn;
        ObjectID_t _oid;
    };

}

#endif // CONNECTION_HPP_INCLUDED
