#include "org_freedesktop_Avahi_EntryGroupProxy.h"

using Avahi::org_freedesktop_Avahi_EntryGroupProxy;

org_freedesktop_Avahi_EntryGroupProxy::org_freedesktop_Avahi_EntryGroupProxy(std::string name ) : DBus::InterfaceProxy( name ){
m_method_Free = this->create_method<void()>("Free");
m_method_Commit = this->create_method<void()>("Commit");
m_method_Reset = this->create_method<void()>("Reset");
m_method_GetState = this->create_method<int32_t()>("GetState");
m_method_IsEmpty = this->create_method<bool()>("IsEmpty");
m_method_AddService = this->create_method<void(int32_t,int32_t,uint32_t,std::string,std::string,std::string,std::string,uint16_t,std::vector<std::vector<uint8_t>>)>("AddService");
m_method_AddServiceSubtype = this->create_method<void(int32_t,int32_t,uint32_t,std::string,std::string,std::string,std::string)>("AddServiceSubtype");
m_method_UpdateServiceTxt = this->create_method<void(int32_t,int32_t,uint32_t,std::string,std::string,std::string,std::vector<std::vector<uint8_t>>)>("UpdateServiceTxt");
m_method_AddAddress = this->create_method<void(int32_t,int32_t,uint32_t,std::string,std::string)>("AddAddress");
m_method_AddRecord = this->create_method<void(int32_t,int32_t,uint32_t,std::string,uint16_t,uint16_t,uint32_t,std::vector<uint8_t>)>("AddRecord");
m_signalproxy_StateChanged = this->create_signal<void(int32_t,std::string)>( "StateChanged" );

}
std::shared_ptr<Avahi::org_freedesktop_Avahi_EntryGroupProxy> org_freedesktop_Avahi_EntryGroupProxy::create(std::string name ){
return std::shared_ptr<Avahi::org_freedesktop_Avahi_EntryGroupProxy>( new Avahi::org_freedesktop_Avahi_EntryGroupProxy( name ));

}
void org_freedesktop_Avahi_EntryGroupProxy::Free( ){
(*m_method_Free)();

}
void org_freedesktop_Avahi_EntryGroupProxy::Commit( ){
(*m_method_Commit)();

}
void org_freedesktop_Avahi_EntryGroupProxy::Reset( ){
(*m_method_Reset)();

}
int32_t org_freedesktop_Avahi_EntryGroupProxy::GetState( ){
return (*m_method_GetState)();

}
bool org_freedesktop_Avahi_EntryGroupProxy::IsEmpty( ){
return (*m_method_IsEmpty)();

}
void org_freedesktop_Avahi_EntryGroupProxy::AddService(int32_t _interface, int32_t protocol, uint32_t flags, std::string name, std::string type, std::string domain, std::string host, uint16_t port, std::vector<std::vector<uint8_t>> txt ){
(*m_method_AddService)(_interface,protocol,flags,name,type,domain,host,port,txt);

}
void org_freedesktop_Avahi_EntryGroupProxy::AddServiceSubtype(int32_t _interface, int32_t protocol, uint32_t flags, std::string name, std::string type, std::string domain, std::string subtype ){
(*m_method_AddServiceSubtype)(_interface,protocol,flags,name,type,domain,subtype);

}
void org_freedesktop_Avahi_EntryGroupProxy::UpdateServiceTxt(int32_t _interface, int32_t protocol, uint32_t flags, std::string name, std::string type, std::string domain, std::vector<std::vector<uint8_t>> txt ){
(*m_method_UpdateServiceTxt)(_interface,protocol,flags,name,type,domain,txt);

}
void org_freedesktop_Avahi_EntryGroupProxy::AddAddress(int32_t _interface, int32_t protocol, uint32_t flags, std::string name, std::string address ){
(*m_method_AddAddress)(_interface,protocol,flags,name,address);

}
void org_freedesktop_Avahi_EntryGroupProxy::AddRecord(int32_t _interface, int32_t protocol, uint32_t flags, std::string name, uint16_t clazz, uint16_t type, uint32_t ttl, std::vector<uint8_t> rdata ){
(*m_method_AddRecord)(_interface,protocol,flags,name,clazz,type,ttl,rdata);

}
std::shared_ptr<DBus::SignalProxy<void(int32_t,std::string)>> org_freedesktop_Avahi_EntryGroupProxy::signal_StateChanged( ){
return m_signalproxy_StateChanged;

}
