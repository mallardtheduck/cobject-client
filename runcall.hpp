#ifndef RUNCALL_HPP_INCLUDED
#define RUNCALL_HPP_INCLUDED

#include "messages.hpp"
#include <meta.hpp>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "connection.hpp"
#include "serialize.hpp"

using namespace std;
using namespace meta;
using namespace boost;

namespace cobject
{

    class Connection;

    /*!
        Perform an object construction request
        \param conn     The connection
        \param callid   The CallID of the request
        \param cls      The name of the class
        \param args     The constructor arguements
    */
    void RunConstruct(Connection &conn, CallID_t callid, MetaClass &cls, vector<TypedVal> args);
    /*!
        Perform a static function call
        \param conn     The connection
        \param callid   The CallID of the request
        \param cls      The name of the class
        \param method   The name of the method
        \param args     The call arguements
    */
    void RunStaticCall(Connection &conn, CallID_t callid, MetaClass &cls, const string &method, vector<TypedVal> args);
    /*!
        Perform an object method call
        \param conn     The connection
        \param callid   The CallID of the request
        \param obj      The object to call
        \param method   The name of the method
        \param args     The call arguements
    */
    void RunMethodCall(Connection &conn, CallID_t callid, boost::shared_ptr<MetaObject> obj, const string &method, vector<TypedVal> args);
}
#endif // RUNCALL_HPP_INCLUDED
