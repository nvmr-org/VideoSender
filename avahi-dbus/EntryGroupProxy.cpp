#include "EntryGroupProxy.h"

using Avahi::EntryGroupProxy;

EntryGroupProxy::EntryGroupProxy(std::shared_ptr<DBus::Connection> conn, std::string dest, std::string path, DBus::ThreadForCalling signalCallingThread ) : DBus::ObjectProxy( conn, dest, path ){
m_org_freedesktop_Avahi_EntryGroupProxy = Avahi::org_freedesktop_Avahi_EntryGroupProxy::create( "org.freedesktop.Avahi.EntryGroup" );
this->add_interface( m_org_freedesktop_Avahi_EntryGroupProxy );

}
std::shared_ptr<EntryGroupProxy> EntryGroupProxy::create(std::shared_ptr<DBus::Connection> conn, std::string dest, std::string path, DBus::ThreadForCalling signalCallingThread ){
std::shared_ptr<EntryGroupProxy> created = std::shared_ptr<EntryGroupProxy>( new EntryGroupProxy( conn, dest, path, signalCallingThread ) );
conn->register_object_proxy( created );
return created;

}
std::shared_ptr<Avahi::org_freedesktop_Avahi_EntryGroupProxy> EntryGroupProxy::getorg_freedesktop_Avahi_EntryGroupInterface( ){
return m_org_freedesktop_Avahi_EntryGroupProxy;

}
