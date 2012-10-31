#ifndef SERIALIZE_HPP_INCLUDED
#define SERIALIZE_HPP_INCLUDED

#include <iostream>
#include <vector>
#include <boost/cstdint.hpp>
#include <boost/any.hpp>

#include "messages.hpp"

using namespace std;
using namespace boost;

namespace cobject
{

    //! Class holding an any and type information
    /*!
        Holds an any value and a type. Used wherever these values are grouped together in the protocol.
    */
    class TypedVal
    {
    public:
        /*!
            Constructor
            \param t    The type
            \param v    The value
        */
        TypedVal(Type_t t, any v) : type(t), val(v) {}
        /*!
            Default constructor
        */
        TypedVal(){}
        Type_t type;        //!< The type
        any val;            //!< The value
    };

    /*!
        Serialize a string
        \param s    Stream to serialize into
        \param v    A string
    */
    void Serialize(ostream &s, const string &v);
    /*!
        Serialize a wstring
        \param s    Stream to serialize into
        \param v    A wstring
    */
    void Serialize(ostream &s, const wstring &v);
    /*!
        Serialize MethodInfo
        \param s    Stream to serialize into
        \param v    MethodInfo
    */
    void Serialize(ostream &s, MethodInfo info);
    /*!
        Serialize ClassInfo
        \param s    Stream to serialize into
        \param info ClassInfo
        \param name Serialize the name or not boolean
    */
    void Serialize(ostream &s, ClassInfo info, bool name=true);
    /*!
        Serialize a TypedVal
        \param s    Stream to serialize into
        \param v    A TypedVal
    */
    void Serialize(ostream &s, const TypedVal &v);

    /*!
        Generic serialization function
        \param s    Stream to serialize into
        \param v    Value to serialize
    */
    template<typename T> void Serialize(ostream &s, const T &v)
    {
        s.write((char*)&v, sizeof(v));
    }

    /*!
        Serialize a vector
        \param s    Stream to serialize into
        \param v    Vector to serialize
    */
    template<typename T> void Serialize(ostream &s, const vector<T> &v)
    {
        Length_t len=v.size();
        Serialize(s, len);
        for (size_t i=0; i<v.size(); i++)
        {
            Serialize(s, v.at(i));
        }
    }

    /*!
        Deserialize a string
        \param s    Stream to deserialize from
        \param v    string to deserialize into
    */
    void Deserialize(istream &s, string &v);
    /*!
        Deserialize a wstring
        \param s    Stream to deserialize from
        \param v    wstring to deserialize into
    */
    void Deserialize(istream &s, wstring &v);
    /*!
        Deserialize MethodInfo
        \param s    Stream to deserialize from
        \param info MethodInfo to deserialize into
    */
    void Deserialize(istream &s, MethodInfo &info);
    /*!
        Deserialize ClassInfo
        \param s    Stream to deserialize from
        \param info ClassInfo to deserialize into
        \param name Deserialize class name or not boolean
    */
    void Deserialize(istream &s, ClassInfo &info, bool name=true);
    /*!
        Deserialize a TypedVal
        \param s    Stream to deserialize from
        \param v    TypedVal to deserialize into
    */
    void Deserialize(istream &s, TypedVal &v);

    /*!
        Generic deserialization function
        \param s    Stream to deserialize from
        \param v    value to deserialize into
    */
    template<typename T> void Deserialize(istream &s, T &v)
    {
        s.read(reinterpret_cast<char*>(&v), sizeof(v));
    }

    /*!
        Deserialize a vector
        \param s    Stream to deserialize from
        \param v    vector to deserialize into
    */
    template<typename T> void Deserialize(istream &s, vector<T> &v)
    {
        Length_t size;
        Deserialize(s, size);
        v.reserve(size);
        for (Length_t i=0; i<size; i++)
        {
            T x;
            Deserialize(s, x);
            v.push_back(x);
        }
    }

}
#endif // SERIALIZE_HPP_INCLUDED
