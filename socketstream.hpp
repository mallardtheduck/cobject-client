#ifndef SOCKETSTREAM_HPP_INCLUDED
#define SOCKETSTREAM_HPP_INCLUDED

#include <iostream>
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/asio.hpp>

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::iostreams;

namespace cobject
{

    //! IOStream device for boost::asio socket
    /*! For some reason, boost::asio includes socket streams and asyncrous socket objects, but no way to convert
        from one to the other. This IOStream device wraps a boost::asio asynchrous socket to allow it to be used
        as a stream.
    */
    class socket_device
    {
    public:
        typedef char char_type;                     //!< Character type for steam
        typedef bidirectional_device_tag category;  //!< Type of stream

        /*!
            Constructor
            \param socket   Pointer to boost::asio asynchrous socket
        */
        socket_device(tcp::socket *socket);
        /*!
            Read from device (socket)
            \param s    Pointer to buffer at least n bytes wide
            \param n    Number of bytes to attempt to read
            \return Actual number of bytes read
        */
        streamsize read(char *s, streamsize n);
        /*!
            Write to device (socket)
            \param s    Pointer to data to send
            \param n    Size of data to send
            \return Actual number of bytes sent
        */
        streamsize write(const char *s, streamsize n);
    private:
        tcp::socket *_socket;
    };

    //! Socket stream, stream class using socket_device
    class socketstream : public stream<socket_device>
    {
    public:
        /*!
            Constructor
            \param socket   Pointer to asio socket
        */
        socketstream(tcp::socket *socket);
        /*!
            Constructor
            \param socket   Asio socket
        */
        socketstream(tcp::socket &socket);
    };

}

#endif // SOCKETSTREAM_HPP_INCLUDED
