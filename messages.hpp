#ifndef MESSAGES_HPP_INCLUDED
#define MESSAGES_HPP_INCLUDED

#include <boost/cstdint.hpp>
#include <string>
#include <vector>

using namespace boost;
using namespace std;

namespace cobject
{

    typedef uint16_t MessageID_t;       //!< Type for MessageID protocol elements
    typedef uint16_t StatusCode_t;      //!< Type for StatusCode protocol elements
    typedef uint32_t ObjectID_t;        //!< Type for ObjectID protocol elements
    typedef uint32_t CallID_t;          //!< Type for CallID protocol elements
    typedef uint32_t Length_t;          //!< Type for lengths in protocol
    typedef string Type_t;              //!< Type for type specifiers in protocol

    //! Protocol types
    namespace Types
    {
        const Type_t
        Int8 = "System.Int8",           //!< 8-bit (signed) integer
        Int16 = "System.Int16",         //!< 16-bit (signed) integer
        Int32 = "System.Int32",         //!< 32-bit (signed) integer
        Int64 = "System.Int64",         //!< 64-bit (signed) integer
        Int = Int32,                    //!< int as alias for 32-bit integer
        UInt8 = "System.Uint8",         //!< 8-bit unsigned integer
        UInt16 = "System.UInt16",       //!< 16-bit unsigned integer
        UInt32 = "System.UInt32",       //!< 32-bit unsigned integer
        UInt64 = "System.UInt64",       //!< 64-bit unsigned integer
        UInt = UInt32,                  //!< uint as alias for 32-bit unsigned integer
        UTF8 = "System.UTF8",           //!< UTF8 code point
        Char = UTF8,                    //!< char as alias for UTF8
        UTF16 = "System.UTF16",         //!< UTF16 code point
        WChar = UTF16,                  //!< wchar as alias for UTF16
        Float32 = "System.Float32",     //!< 32-bit float
        Float = Float32,                //!< float as alias for float32
        Float64 = "System.Float64",     //!< 64-bit float
        Double = Float64,               //!< double as alias for float64
        Number = Float64,               //!< number as alias for float64
        String = "System.String",       //!< UTF8 string
        WString = "System.WString",     //!< UTF16 string
        Bool = "System.Bool",           //!< boolean
        Object = "System.Object",       //!< Object
        Void = "System.Void",           //!< void (non-value)
        Unknown = "Unknown";            //!< unknown (erroneous!) type
    }

    //! Protocol MessageIDs
    namespace Messages
    {
        const MessageID_t
        BrokerDetails = 0,              //!< Request broker details
        ListNamespaces = 1,             //!< List registered namespaces
        SetNamespace = 2,               //!< Set application namespace
        ListClasses = 3,                //!< List classes in namespace
        GetClassDef = 4,                //!< Get class definition
        GetObjectDef = 5,               //!< Get object definition
        RegisterClass = 6,              //!< Register class
        RegisterObject = 7,             //!< Register object
        ConstructObject = 8,            //!< Construct object
        CallStatic = 9,                 //!< Call static function
        CallMethod = 10,                //!< Call object method
        ReleaseObject = 11,             //!< Release/free unused object

        ClientVersion = 20,             //!< Send client version details
        Construct = 21,                 //!< Request to construct object
        StaticCall = 22,                //!< Request to call static function
        MethodCall = 23,                //!< Request to call object method
        FreeObject = 24;                //!< Request to free/delete unused object
    }

    //! Protocol StatusCodes
    namespace StatusCodes
    {
        const StatusCode_t
        Success = 1,                    //!< Success
        Error = 0,                      //!< Failure/Error
        ConnectionError = -1,           //!< Unable to connect to host/owner of object
        InvalidOID = -2,                //!< Object ID unrecognised
        Unknowntype = -3,               //!< Data type not known
        UnknownMethod = -4,             //!< Unknown method
        UnknownClass = -5,              //!< Unknown class
        InvalidArguements = -6;         //!< Type/number of arguements mismatched.
        
    }

    //! Broker informatiaon structure
    struct BrokerDetails
    {
        uint16_t ProtocolVersion;       //!< Protocol version number
        string BrokerName;              //!< Broker product name
        string Vendor;                  //!< Broker vendor
        string DisplayVersion;          //!< Verion information for display to user
    };

    //! Method information structure
    struct MethodInfo
    {
        string MethodName;              //!< Method name
        Type_t ReturnType;              //!< Method return type
        bool IsStatic;                  //!< Method static boolean
        vector<Type_t> ParamTypes;      //!< Method parameters
    };

    //! Class information structure
    struct ClassInfo
    {
        string ClassName;               //!< Class name
        vector<MethodInfo> Methods;     //!< Class methods
    };

    //! Result of an object construction
    struct ConstructResult
    {
        StatusCode_t status;            //!< Construction status code
        ObjectID_t objectid;            //!< Constructed ObjectID
    };
}
#endif // MESSAGES_HPP_INCLUDED
