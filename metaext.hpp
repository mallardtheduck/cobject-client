#ifndef METAEXT_HPP_INCLUDED
#define METAEXT_HPP_INCLUDED

#include <vector>
#include <meta.hpp>
#include "../cobject-shared/typemap.hpp"
#include "messages.hpp"
#include "connection.hpp"

using namespace std;
using namespace meta;

class Connection;

Type_t GetReturnType(const meta::MethodInfo &mth);
vector<Type_t> GetParamTypes(const meta::MethodInfo &mth);

ClassInfo MetaClassInfo(const MetaClass &cls);
MetaClass MakeMetaClass(Connection &conn, ClassInfo info);

PolyWrapper<ITypeInfo> TypeToTypeInfo(const Type_t &type);

MetaObject ObjectIDToMetaObject(Connection &conn, ObjectID_t oid);

class COFnWrap : public IFnWrap{
    public:
        COFnWrap(Connection &c, const string &clsname, ::MethodInfo &i);
        virtual bool TypeCheck(const ITypeInfo &retType, const ITypeInfo &paramType);

        virtual any Call(MetaInfo &info, any args);
        virtual any Call(MetaInfo &info, many args);

        virtual PolyWrapper<ITypeInfo> GetReturnType();
        virtual vector<PolyWrapper<ITypeInfo> > GetParamTypes();

    private:
        Connection &_conn;
        ::MethodInfo _info;
        string _classname;
        static CallID_t _callid;
};

#endif // METAEXT_HPP_INCLUDED
