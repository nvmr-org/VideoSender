#include "avahi.h"

#include <QSettings>
#include <QHostAddress>
#include <QNetworkInterface>
#include <log4cxx/logger.h>

static log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger( "org.nvmr.AvahiControl" );

AvahiControl::AvahiControl(QObject *parent) : QObject(parent)
{
    m_dispatcher = DBus::Qt::QtDispatcher::create();
    m_conn = m_dispatcher->create_connection( DBus::BusType::SYSTEM );

    m_updateTxtTimer.start( 800 );

    connect( &m_updateTxtTimer, &QTimer::timeout,
             this, &AvahiControl::updateTxtIfPossible );
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
    std::string serviceName( "VideoSender-" );
    serviceName.append( m_uuid.toString( QUuid::StringFormat::WithoutBraces ).toStdString() );
    m_entryProxy->getorg_freedesktop_Avahi_EntryGroupInterface()
            ->AddService( -1, -1, 0, serviceName, "_nvmr_video_sender._tcp", std::string(), std::string(), 9036, txtData );

    m_entryProxy->getorg_freedesktop_Avahi_EntryGroupInterface()
            ->Commit();

    updateTxtIfPossible();
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

void AvahiControl::updateTxtIfPossible(){
    std::string serviceName( "VideoSender-" );
    serviceName.append( m_uuid.toString( QUuid::StringFormat::WithoutBraces ).toStdString() );

    std::vector<std::vector<uint8_t>> txtData;
    QSettings settings;
    txtData.push_back( qstringToVector( "videoname=" + settings.value( "video/name" ).toString() ) );
    // TODO this is simply hardcoded to ipv4, and assumes 8554.  It should probably be less hardcoded,
    // perhaps by passing the options to the rtsp-helper program
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    bool foundIpv4Addr = false;
    for( QHostAddress addr : list ){
        if( addr.isBroadcast() || addr.isLoopback() || addr.isNull() ){
            continue;
        }

        if( addr.protocol() == QAbstractSocket::IPv4Protocol ){
            foundIpv4Addr = true;
            txtData.push_back( qstringToVector( "rtsp=rtsp://" + addr.toString() + ":8554/rpi-video" ) );
        }
    }

    if( !foundIpv4Addr ){
        return;
    }

    m_updateTxtTimer.stop();

    m_entryProxy->getorg_freedesktop_Avahi_EntryGroupInterface()
            ->UpdateServiceTxt( -1, -1, 0, serviceName, "_nvmr_video_sender._tcp", std::string(), txtData );

    m_updateTxtTimer.stop();
}
