#ifndef MEMSTREAM_HPP_INCLUDED
#define MEMSTREAM_HPP_INCLUDED

#include <sstream>

using namespace std;

/*#include <iostream>
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/asio.hpp>

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::iostreams;

class MemoryDevice{
public:
    typedef char char_type;
    typedef sink_tag dual_seekable;

    MemoryDevice();
    streamsize write(const char *s, streamsize n);

    char* GetData();
    size_t GetSize();

private:
    char *data;
    size_t size;
};

typedef stream<MemoryDevice> MemStream;*/

#endif // MEMSTREAM_HPP_INCLUDED
