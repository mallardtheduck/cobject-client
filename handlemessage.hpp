#ifndef HANDLEMESSAGE_HPP_INCLUDED
#define HANDLEMESSAGE_HPP_INCLUDED

#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <meta.hpp>

#include "messages.hpp"
#include "metaext.hpp"

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace meta;

namespace cobject
{

    BrokerDetails GetBrokerDetails(tcp::iostream &s);
    string SetNamesapce(tcp::iostream &s, const string &ns);
    vector<string> ListNamespaces(tcp::iostream &s);
    void RegisterClass(tcp::iostream &s, const MetaClass &cls);

}
#endif // HANDLEMESSAGE_HPP_INCLUDED
