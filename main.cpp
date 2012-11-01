#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <boost/asio.hpp>
#include <meta/meta.hpp>
#include <google/profiler.h>

//#include "serialize.hpp"
#include "foreach.hpp"
#include "connection.hpp"

#include "testclass.hpp"

using namespace std;
using namespace boost;
using namespace std::chrono;

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
        
        vector<int> vec;
        vec.push_back(5); vec.push_back(11); vec.push_back(4);
        int t=myobject["array_total"].Call<int>(vec);
        cout << "Result of call: " << t << "" << endl;
        
        vector<int> o=myobject["array_ones"].Call<vector<int> >(5);
        cout << "Size of result of call: " << o.size() << "" << endl;
        
       	high_resolution_clock::time_point start=high_resolution_clock::now();
       	ProfilerStart("/tmp/cobject-client-test-profile");
        for(int i=0; i<1000; ++i){
        	myobject["test"].Call();
        }
        ProfilerStop();
        high_resolution_clock::time_point end=high_resolution_clock::now();
        high_resolution_clock::duration duration=end - start;
        double ms_ratio=1000.0 * (double)((double)high_resolution_clock::duration::period::num / (double)high_resolution_clock::duration::period::den);
        double duration_ms=(double)duration.count() * ms_ratio;
        cout << "Time taken: " << duration_ms << endl;
        while(true) sleep(100);

    }
    catch (std::exception& e)
    {
        cout << "Error! " << demangle(typeid(e).name()) << endl;
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
