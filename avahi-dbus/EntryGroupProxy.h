#ifndef ENTRYGROUPPROXY_H
#define ENTRYGROUPPROXY_H

#include <dbus-cxx.h>
#include <memory>
#include <stdint.h>
#include <string>
#include "org_freedesktop_Avahi_EntryGroupProxy.h"
namespace Avahi {
class EntryGroupProxy
 : public DBus::ObjectProxy {
public:
EntryGroupProxy(std::shared_ptr<DBus::Connection> conn, std::string dest, std::string path, DBus::ThreadForCalling signalCallingThread = DBus::ThreadForCalling::DispatcherThread );
public:
    static std::shared_ptr<EntryGroupProxy> create(std::shared_ptr<DBus::Connection> conn, std::string dest, std::string path, DBus::ThreadForCalling signalCallingThread = DBus::ThreadForCalling::DispatcherThread );
    std::shared_ptr<Avahi::org_freedesktop_Avahi_EntryGroupProxy> getorg_freedesktop_Avahi_EntryGroupInterface( );
protected:
    std::shared_ptr<Avahi::org_freedesktop_Avahi_EntryGroupProxy> m_org_freedesktop_Avahi_EntryGroupProxy;
};
} /* namespace Avahi */
#endif /* ENTRYGROUPPROXY_H */
