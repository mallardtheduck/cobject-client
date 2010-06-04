#include <iostream>
#include <string>
#include <sstream>
#include <boost/asio.hpp>
#include <meta.hpp>

#include "handlemessage.hpp"
#include "serialize.hpp"
#include "foreach.hpp"
#include "connection.hpp"

#include "testclass.hpp"

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace meta;

int main()
{
    try
    {
        Connection conn;

        BrokerDetails det=conn.GetBrokerDetails();
        cout << "Broker details:" << endl;
        cout << "  Protocol version: " << det.ProtocolVersion << endl;
        cout << "  Broker name: " << det.BrokerName << endl;
        cout << "  Vendor: " << det.Vendor << endl;
        cout << "  Version: " << det.DisplayVersion << endl;

        string ns=conn.SetNamesapce("TestApp");
        cout << "Namespace: " << ns << endl;

        conn.RegisterClass(TestClass());

        vector<string> nsl=conn.ListNamespaces();
        cout << "Known Namespaces:" << endl;
        foreach(string &s, nsl){
            cout << "  " << s << endl;
            cout << "  Classes:" << endl;
            vector<string> lcls=conn.ListClasses(s);
            foreach(string &c, lcls){
                cout << "     " << c << endl;
            }
        }

        MetaClass myclass=conn.GetClass(ns, TestClass().GetName());
        cout << "Class name: '" << myclass.GetName() << "'" << endl;
        MetaObject myobject=New(myclass);
        string hi=myobject["hello"].Call<string>();
        cout << "Result of call: '" << hi << "'" << endl;
        while(true);

    }
    catch (std::exception& e)
    {
        cout << "Error! " << demangle(typeid(e).name()) << endl;
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
