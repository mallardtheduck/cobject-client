#ifndef TYPEMAP_HPP_INCLUDED
#define TYPEMAP_HPP_INCLUDED

#include "messages.hpp"
#include <typeinfo>

namespace cobject
{

    /*!
        Convert a C++ type_info value to a CrossObject type
        \param t    A C++ type_info value
        \return The CrossObject type equivelent to the type_info
    */
    Type_t MapType(const type_info &t);

    /*!
        Return the CrossObject type matching a C++ type
        \param T    A C++ type
        \return The CrossObject type equivelent to the type
    */
    template<typename T> Type_t MapType()
    {
        return MapType(typeid(T));
    }
	
	/*!
        Check if type is a CrossObject array
        \param type    A CrossObject type specifier
        \return true if the type is an array, false otherwise
    */
	bool IsArray(Type_t type);
	
	/*!
        Get the element type for an array
        \param type    A CrossObject type specifier
        \return the type of the element of the array
    */
	Type_t ArrayElementType(Type_t array);
}
#endif // TYPEMAP_HPP_INCLUDED
