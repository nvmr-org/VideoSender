#ifndef ORG_FREEDESKTOP_AVAHI_ENTRYGROUPPROXY_H
#define ORG_FREEDESKTOP_AVAHI_ENTRYGROUPPROXY_H

#include <dbus-cxx.h>
#include <memory>
#include <stdint.h>
#include <string>
namespace Avahi {
class org_freedesktop_Avahi_EntryGroupProxy
 : public DBus::InterfaceProxy {
protected:
org_freedesktop_Avahi_EntryGroupProxy(std::string name );
public:
    static std::shared_ptr<Avahi::org_freedesktop_Avahi_EntryGroupProxy> create(std::string name = "org.freedesktop.Avahi.EntryGroup" );
    void Free( );
    void Commit( );
    void Reset( );
    int32_t GetState( );
    bool IsEmpty( );
    void AddService(int32_t _interface, int32_t protocol, uint32_t flags, std::string name, std::string type, std::string domain, std::string host, uint16_t port, std::vector<std::vector<uint8_t>> txt );
    void AddServiceSubtype(int32_t _interface, int32_t protocol, uint32_t flags, std::string name, std::string type, std::string domain, std::string subtype );
    void UpdateServiceTxt(int32_t _interface, int32_t protocol, uint32_t flags, std::string name, std::string type, std::string domain, std::vector<std::vector<uint8_t>> txt );
    void AddAddress(int32_t _interface, int32_t protocol, uint32_t flags, std::string name, std::string address );
    void AddRecord(int32_t _interface, int32_t protocol, uint32_t flags, std::string name, uint16_t clazz, uint16_t type, uint32_t ttl, std::vector<uint8_t> rdata );
    std::shared_ptr<DBus::SignalProxy<void(int32_t,std::string)>> signal_StateChanged( );
protected:
    std::shared_ptr<DBus::MethodProxy<void()>>  m_method_Free;
    std::shared_ptr<DBus::MethodProxy<void()>>  m_method_Commit;
    std::shared_ptr<DBus::MethodProxy<void()>>  m_method_Reset;
    std::shared_ptr<DBus::MethodProxy<int32_t()>>  m_method_GetState;
    std::shared_ptr<DBus::MethodProxy<bool()>>  m_method_IsEmpty;
    std::shared_ptr<DBus::MethodProxy<void(int32_t,int32_t,uint32_t,std::string,std::string,std::string,std::string,uint16_t,std::vector<std::vector<uint8_t>>)>>  m_method_AddService;
    std::shared_ptr<DBus::MethodProxy<void(int32_t,int32_t,uint32_t,std::string,std::string,std::string,std::string)>>  m_method_AddServiceSubtype;
    std::shared_ptr<DBus::MethodProxy<void(int32_t,int32_t,uint32_t,std::string,std::string,std::string,std::vector<std::vector<uint8_t>>)>>  m_method_UpdateServiceTxt;
    std::shared_ptr<DBus::MethodProxy<void(int32_t,int32_t,uint32_t,std::string,std::string)>>  m_method_AddAddress;
    std::shared_ptr<DBus::MethodProxy<void(int32_t,int32_t,uint32_t,std::string,uint16_t,uint16_t,uint32_t,std::vector<uint8_t>)>>  m_method_AddRecord;
    std::shared_ptr<DBus::SignalProxy<void(int32_t,std::string)>> m_signalproxy_StateChanged;
};
} /* namespace Avahi */
#endif /* ORG_FREEDESKTOP_AVAHI_ENTRYGROUPPROXY_H */
