#include "avahi.h"

#include <QSettings>
#include <log4cxx/logger.h>

static log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger( "org.nvmr.AvahiControl" );

AvahiControl::AvahiControl(QObject *parent) : QObject(parent)
{
    m_dispatcher = DBus::Qt::QtDispatcher::create();
    m_conn = m_dispatcher->create_connection( DBus::BusType::SYSTEM );
}

void AvahiControl::registerWithAvahi(){
    m_avahiServer = Avahi::ServerProxy::create( m_conn, "org.freedesktop.Avahi", "/" );

    DBus::Path newGroup =
            m_avahiServer->getorg_freedesktop_Avahi_ServerInterface()->EntryGroupNew();

    LOG4CXX_DEBUG( logger, "New group is: " << newGroup.c_str() );

    m_entryProxy =
            Avahi::EntryGroupProxy::create( m_conn, "org.freedesktop.Avahi", newGroup );

    m_entryProxy->getorg_freedesktop_Avahi_EntryGroupInterface()
            ->signal_StateChanged()
            ->connect( sigc::mem_fun( *this, &AvahiControl::stateChanged ) );

    std::vector<std::vector<uint8_t>> txtData;
    QSettings settings;
    txtData.push_back( qstringToVector( "videoname=" + settings.value( "video/name" ).toString() ) );
    std::string serviceName( "VideoSender-" );
    serviceName.append( m_uuid.toString( QUuid::StringFormat::WithoutBraces ).toStdString() );
    m_entryProxy->getorg_freedesktop_Avahi_EntryGroupInterface()
            ->AddService( -1, -1, 0, serviceName, "_nvmr_video_sender._tcp", std::string(), std::string(), 9036, txtData );

    m_entryProxy->getorg_freedesktop_Avahi_EntryGroupInterface()
            ->Commit();
}

void AvahiControl::stateChanged( int state, std::string error ){
    LOG4CXX_DEBUG( logger, "Avahi state changed: " << state << " Error: " << error );
}

void AvahiControl::setUuid(QUuid uuid){
    m_uuid = uuid;
}

std::vector<uint8_t> AvahiControl::qstringToVector( QString str ){
    QByteArray ba = str.toLocal8Bit();
    std::vector<uint8_t> ret;

    std::copy( ba.begin(), ba.end(), std::back_inserter(ret) );

    return ret;
}
