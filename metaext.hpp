#ifndef METAEXT_HPP_INCLUDED
#define METAEXT_HPP_INCLUDED

#include <vector>
#include <meta.hpp>
#include "../cobject-shared/typemap.hpp"
#include "messages.hpp"
#include "connection.hpp"

using namespace std;
using namespace meta;

namespace cobject
{

    class Connection;

    /*!
        Get the return type for a Meta method as a CrossObject Type_t
        \param mth  The Meta method
        \return The method return type
    */
    Type_t GetReturnType(const meta::MethodInfo &mth);
    /*!
        Get the parameter types for a Meta method as a CrossObject vector<Type_t>
        \param mth  The Meta method
        \return A list of the return types
    */
    vector<Type_t> GetParamTypes(const meta::MethodInfo &mth);

    /*!
        Get a CrossObject ClassInfo structure from a Meta class
        \param cls  The Meta class
        \return A CrossObject ClassInfo structure
    */
    ClassInfo MetaClassInfo(const MetaClass &cls);
    /*!
        Create a class proxy from a ClassInfo structure
        \param conn     The connection to associate the new class proxy with
        \param info     The ClassInfo structure
        \return A class proxy
    */
    MetaClass MakeMetaClass(Connection &conn, ClassInfo info);

    /*!
        Convert a CrossObject Type_t to a Meta ITypeInfo object
        \param type     A CrossObject Type_t
        \return A Meta ITypeInfo object
    */
    PolyWrapper<ITypeInfo> TypeToTypeInfo(const Type_t &type);

    /*!
        Create an object proxy from an ObjectID
        \param conn     The connection to use
        \param oid      The ID of the object
        \return An object proxy for the object
    */
    MetaObject ObjectIDToMetaObject(Connection &conn, ObjectID_t oid);

    /*!
        A Meta IFnWrap to act as a method proxy
        This class sends makes a CrossObject protocol method/function/construction request based on
        arguements passed to it.
    */
    class COFnWrap : public IFnWrap
    {
    public:
        /*!
            Constructor
            \param c        The connection to use for calls
            \param clsname  The name of the class
            \param i        The CrossObject MethodInfo
        */
        COFnWrap(Connection &c, const string &clsname, MethodInfo &i);
        /*!
            Stub for type checking.
            \note Typechecking is not supported by this IFnWrap, so this function always returns true.
            \param retType      The return type to test
            \param paramTyp     The parameter tuple type to test
            \return true - as typechecking is not supported by this IFnWrap
        */
        virtual bool TypeCheck(const ITypeInfo &retType, const ITypeInfo &paramType);

        /*!
            Call with tuple arguements
            \param info     The call information
            \param args     The call arguements as a tuple wrapped in an any
            \return The method return value
        */
        virtual any Call(MetaInfo &info, any args);
        /*!
            Call with vector<any> arguements
            \param info     The call information
            \param args     The call arguements as a vector<any>
            \return The method return value
        */
        virtual any Call(MetaInfo &info, many args);

        /*!
            Get the return type
            \return The  return type
        */
        virtual PolyWrapper<ITypeInfo> GetReturnType();
        /*!
            Get the parameter types
            \param The parameter types
        */
        virtual vector<PolyWrapper<ITypeInfo> > GetParamTypes();

    private:
        Connection &_conn;
        MethodInfo _info;
        string _classname;
        static CallID_t _callid;
    };

}
#endif // METAEXT_HPP_INCLUDED
