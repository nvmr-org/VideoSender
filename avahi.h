#ifndef AVAHI_H
#define AVAHI_H

#include <QObject>
#include <QUuid>

#include <dbus-cxx-qt.h>

#include "avahi-dbus/ServerProxy.h"
#include "avahi-dbus/EntryGroupProxy.h"

class AvahiControl : public QObject
{
    Q_OBJECT
public:
    explicit AvahiControl(QObject *parent = nullptr);

    void setUuid( QUuid uuid );

Q_SIGNALS:

public Q_SLOTS:
    void registerWithAvahi();
    void stateChanged( int state, std::string error );

private:
    std::vector<uint8_t> qstringToVector( QString str );

private:
    std::shared_ptr<DBus::Connection> m_conn;
    std::shared_ptr<DBus::Dispatcher> m_dispatcher;
    std::shared_ptr<Avahi::ServerProxy> m_avahiServer;
    std::shared_ptr<Avahi::EntryGroupProxy> m_entryProxy;
    QUuid m_uuid;
};

#endif // AVAHI_H
