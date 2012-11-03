#ifndef THROWERROR_HPP_INCLUDED
#define THROWERROR_HPP_INCLUDED

#include <stdexcept>
#include <string>
#include <sstream>

#define THROW_ERROR(msg) do{ doThrow(__FILE__, __LINE__, __FUNCTION__, msg); throw 0; }while(0)

using namespace std;

inline void doThrow(string file, int line, string func, string msg){
	stringstream ss;
	ss << file << ":" << line << " - " << func << "(): " << msg << endl;
	throw runtime_error(ss.str());
}

#endif //THROWERROR_HPP_INCLUDED