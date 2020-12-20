#ifndef AVAHI_H
#define AVAHI_H

#include <QObject>

#include <dbus-cxx-qt.h>

#include "avahi-dbus/ServerProxy.h"
#include "avahi-dbus/EntryGroupProxy.h"

class AvahiControl : public QObject
{
    Q_OBJECT
public:
    explicit AvahiControl(QObject *parent = nullptr);

Q_SIGNALS:

public Q_SLOTS:
    void registerWithAvahi();
    void stateChanged( int state, std::string error );

private:
    std::shared_ptr<DBus::Connection> m_conn;
    std::shared_ptr<DBus::Dispatcher> m_dispatcher;
    std::shared_ptr<Avahi::ServerProxy> m_avahiServer;
    std::shared_ptr<Avahi::EntryGroupProxy> m_entryProxy;
};

#endif // AVAHI_H
