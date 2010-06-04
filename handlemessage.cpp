#include "handlemessage.hpp"
#include "serialize.hpp"

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;

BrokerDetails GetBrokerDetails(tcp::iostream &s){
    Serialize(s, Messages::BrokerDetails);
    BrokerDetails ret;
    Deserialize(s, ret.ProtocolVersion);
    Deserialize(s, ret.BrokerName);
    Deserialize(s, ret.Vendor);
    Deserialize(s, ret.DisplayVersion);
    return ret;
}

string SetNamesapce(tcp::iostream &s, const string &ns){
    Serialize(s, Messages::SetNamespace);
    Serialize(s, ns);
    string ret;
    MessageID_t id;
    Deserialize(s, id);
    assert(id==Messages::SetNamespace);
    Deserialize(s, ret);
    return ret;
}

vector<string> ListNamespaces(tcp::iostream &s){
    Serialize(s, Messages::ListNamespaces);
    vector<string> ret;
    MessageID_t id;
    Deserialize(s, id);
    assert(id==Messages::ListNamespaces);
    Deserialize(s, ret);
    return ret;
}

void RegsiterClass(tcp::iostream &s, const MetaClass &cls){
    ClassInfo info=MetaClassInfo(cls);
    Serialize(s, Messages::RegisterClass);
    Serialize(s, info.ClassName);
    Serialize<Length_t>(s, info.Methods.size());
    for(size_t i=0; i<info.Methods.size(); i++){
        Serialize(s, info.Methods[i].MethodName);
        Serialize(s, info.Methods[i].ReturnType);
        Serialize(s, info.Methods[i].ParamTypes);
    }
    MessageID_t id; string ClassName;
    Deserialize(s, id);
    Deserialize(s, ClassName);
    assert(id==Messages::RegisterClass);
    assert(ClassName==info.ClassName);
}
