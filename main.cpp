#include <iostream>
#include <string>
#include <sstream>
#include <boost/asio.hpp>
#include <meta.hpp>

//#include "serialize.hpp"
#include "foreach.hpp"
#include "connection.hpp"

#include "testclass.hpp"

using namespace std;
using namespace boost;

int main()
{
    try
    {
        cobject::Connection conn;

        cobject::BrokerDetails det=conn.GetBrokerDetails();
        cout << "Broker details:" << endl;
        cout << "  Protocol version: " << det.ProtocolVersion << endl;
        cout << "  Broker name: " << det.BrokerName << endl;
        cout << "  Vendor: " << det.Vendor << endl;
        cout << "  Version: " << det.DisplayVersion << endl;

        string ns=conn.SetNamespace("TestApp");
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

        meta::MetaClass myclass=conn.GetClass(ns, TestClass().GetName());
        cout << "Class name: '" << myclass.GetName() << "'" << endl;
        meta::MetaObject myobject=New(myclass);
        string hi=myobject["hello"].Call<string>();
        cout << "Result of call: '" << hi << "'" << endl;
        string hi2=myobject["saytimes"].Call<string>(MArgs(string("hi!"), 3));
        cout << "Result of call: '" << hi2 << "'" << endl;
        meta::MetaObject myobject2=myobject["getobject"].Call<meta::MetaObject>();
        foreach(Q(pair<string, meta::MethodInfo>) info, myobject2.GetMethodInfo()){
            cout << "Method: " << info.first << endl;
        }
        string hi3=myobject2["hello"].Call<string>();
        cout << "Result of call: '" << hi3 << "'" << endl;
        string hi4=myobject["callhello"].Call<string>(MArgs(myobject2));
        cout << "Result of call: '" << hi4 << "'" << endl;
        while(true) sleep(10);

    }
    catch (std::exception& e)
    {
        cout << "Error! " << demangle(typeid(e).name()) << endl;
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
