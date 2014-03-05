#include "socketstream.hpp"

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::iostreams;

namespace cobject
{

    socket_device::socket_device(tcp::socket *socket) : _socket(socket) {}

    streamsize socket_device::read(char *s, streamsize n)
    {
        return _socket->read_some(buffer(s, n));
    }


    streamsize socket_device::write(const char *s, streamsize n)
    {
        return _socket->write_some(buffer(s, n));
    }

    socketstream::socketstream(tcp::socket *socket)
    {
        this->open(socket_device(socket));
    }

    socketstream::socketstream(tcp::socket &socket)
    {
        this->open(socket_device(&socket));
    }
}
