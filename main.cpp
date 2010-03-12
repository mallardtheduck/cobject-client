#include <iostream>
#include <string>
#include <sstream>
#include <boost/asio.hpp>
#include <meta.hpp>

#include "serialize.hpp"

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;

int main()
{
    try
    {
        tcp::iostream s("localhost", "3300");

        string msg;
        Deserialize(s, msg);
        cout << msg << endl;

        for (char c='A';;c=(c=='Z'?'A':c+1)){
            s << c;
        }
    }
    catch (std::exception& e)
    {
        cout << "Error! " << demangle(typeid(e).name()) << endl;
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
