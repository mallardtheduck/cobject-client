#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <meta.hpp>

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;

int main()
{
    try
    {

        boost::asio::io_service io_service;

        tcp::resolver resolver(io_service);
        tcp::resolver::query query("127.0.0.1", "3300");
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
        tcp::resolver::iterator end;

        tcp::socket socket(io_service);
        boost::system::error_code error = boost::asio::error::host_not_found;
        while (error && endpoint_iterator != end)
        {
            socket.close();
            socket.connect(*endpoint_iterator++, error);
        }
        if (error)
            throw boost::system::system_error(error);

        //for(;;)
        {
            boost::array<char, 128> buf;
            boost::system::error_code error;

            size_t len = socket.read_some(boost::asio::buffer(buf), error);


            /*if (error == boost::asio::error::eof)
                break; // Connection closed cleanly by peer.
            else if (error)
                throw boost::system::system_error(error); // Some other error.*/

            std::cout.write(buf.data(), len);
        }
        for (char c='A';;c=(c=='Z'?'A':c+1)){
            array<char, 1> buf;
            boost::system::error_code error;
            buf[0]=c;
            socket.write_some(buffer(buf), error);
        }
    }
    catch (std::exception& e)
    {
        cout << "Error! " << demangle(typeid(e).name()) << endl;
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
