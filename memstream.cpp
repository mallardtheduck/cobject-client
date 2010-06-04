#include "memstream.hpp"
#include <memory>

/*
MemoryDevice::MemoryDevice() : size(0){
    data=reinterpret_cast<char*>(malloc(size));
}

streamsize MemoryDevice::write(const char *s, streamsize n){
    size_t oldsize=size;
    size+=n;
    data=reinterpret_cast<char*>(realloc(data, size));
    memcpy(data+oldsize, s, n);
    return n;
}

char* MemoryDevice::GetData(){
    return data;
}

size_t MemoryDevice::GetSize(){
    return size;
}
*/
