#ifndef RUNCALL_HPP_INCLUDED
#define RUNCALL_HPP_INCLUDED

#include "messages.hpp"
#include <meta.hpp>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "connection.hpp"
#include "serialize.hpp"

class Connection;

using namespace std;
using namespace meta;
using namespace boost;

void RunConstruct(Connection &conn, CallID_t callid, MetaClass &cls, vector<TypedVal> args);
void RunStaticCall(Connection &conn, CallID_t callid, MetaClass &cls, const string &method, vector<TypedVal> args);
void RunMethodCall(Connection &conn, CallID_t callid, shared_ptr<MetaObject> obj, const string &method, vector<TypedVal> args);

#endif // RUNCALL_HPP_INCLUDED
