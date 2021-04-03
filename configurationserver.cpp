#include <log4cxx/logger.h>
#include <QByteArray>
#include <QCoreApplication>

#include "configurationserver.h"
#include "videosender.h"
#include "videosendermessage.h"

static log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger( "org.nvmr.ConfigurationServer" );

ConfigurationServer::ConfigurationServer(QObject *parent) : QObject(parent),
    m_server( new QWebSocketServer(QStringLiteral("Video Sender Server"),
                                   QWebSocketServer::NonSecureMode, this) ),
    m_videoSender( nullptr )
{
    if ( !m_server->listen(QHostAddress::Any, 9036) ){
        LOG4CXX_ERROR( logger, "Unable to listen for incoming websocket connections" );
        return;
    }

    connect(m_server, &QWebSocketServer::newConnection,
                    this, &ConfigurationServer::onNewConnection);
}

void ConfigurationServer::setVideoSender( VideoSender* vidsend ){
    m_videoSender = vidsend;
}

void ConfigurationServer::onNewConnection()
{
    QWebSocket *pSocket = m_server->nextPendingConnection();

    LOG4CXX_DEBUG( logger, "New incoming connection from " << pSocket->peerAddress().toString().toStdString() );
    connect(pSocket, &QWebSocket::binaryMessageReceived, this, &ConfigurationServer::processBinaryMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &ConfigurationServer::socketDisconnected);

    m_clients.push_back( pSocket );
}

void ConfigurationServer::processBinaryMessage( const QByteArray& message ){
    QJsonDocument doc = QJsonDocument::fromJson( message );
    if( doc.isNull() ){
        LOG4CXX_ERROR( logger, "Docment is null!" );
        return;
    }

    if( logger->isDebugEnabled() ){
        std::string debugDoc = doc.toJson().toStdString();
        LOG4CXX_DEBUG( logger, "Got document: " << debugDoc );
    }

    QSettings settings;

    VideoSenderMessage msg( doc.object() );

    if( msg.command() == "query" ){
        // Send the data back
        for( QWebSocket* socket : m_clients ){
            VideoSenderMessage retmsg;

            QSettings settings;
            VideoSenderConfiguration& config = retmsg.mutuable_configuration();
            config.setUuid( settings.value( "device-uuid" ).toUuid().toString( QUuid::StringFormat::WithoutBraces ) );
            VideoSettings& vidSettings = config.mutable_videoSettings();
            NetworkSettings& netSettings = config.mutable_networkSettings();

            vidSettings.setWidth( m_videoSender->videoWidth() );
            vidSettings.setHeight( m_videoSender->videoHeight() );
            vidSettings.setConfigInterval( m_videoSender->configInterval() );
            vidSettings.setFramerate( m_videoSender->framerate() );
            vidSettings.setPt( m_videoSender->pt() );
            vidSettings.setId( settings.value( "video/id", -1 ).toInt() );
            vidSettings.setName( settings.value( "video/name" ).toString() );

            netSettings.setUdpHost( m_videoSender->ipAddr() );
            netSettings.setUdpPort( m_videoSender->port() );

            QJsonDocument doc( retmsg.jsonObj() );
            socket->sendBinaryMessage( doc.toJson() );
        }
    }else if( msg.command() == "restart" ){
        for( QWebSocket* socket : m_clients ){
            socket->close();
        }
        QTimer::singleShot( 100, [](){
            QCoreApplication::instance()->exit( 2 );
        });
    }else if( msg.command() == "set" ){
        QSettings settings;

        settings.setValue( "video/width", msg.configuration().videoSettings().width() );
        settings.setValue( "video/height", msg.configuration().videoSettings().height() );
        settings.setValue( "video/config-interval", msg.configuration().videoSettings().configInterval() );
        settings.setValue( "video/framerate", msg.configuration().videoSettings().framerate() );
        settings.setValue( "video/pt", msg.configuration().videoSettings().pt() );
        settings.setValue( "video/id", msg.configuration().videoSettings().id() );
        settings.setValue( "video/name", msg.configuration().videoSettings().name() );
        settings.setValue( "network/udp-host", msg.configuration().networkSettings().udpHost() );
        settings.setValue( "network/udp-port", msg.configuration().networkSettings().udpPort() );
    }

    //VideoSettings vidset = msg.configuration().videoSettings();

}

void ConfigurationServer::socketDisconnected(){
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());

    if (pClient) {
        m_clients.removeAll(pClient);
        pClient->deleteLater();
    }
}
