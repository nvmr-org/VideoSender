#include <log4cxx/logger.h>
#include <QJsonDocument>
#include <QSettings>
#include <videosendermessage.h>
#include <QUuid>

#include "clientconnection.h"
#include "videosender.h"

static log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger( "org.nvmr.ClientConnection" );

ClientConnection::ClientConnection(QWebSocket* socket,
                                   VideoSender* vidSend,
                                   QObject *parent) :
    QObject(parent),
    m_socket( socket ),
    m_videoSender( vidSend ),
    m_isSendingVideo( false )
{
    connect( m_socket, &QWebSocket::disconnected,
             this, &ClientConnection::clientDisconnected );
    connect( m_socket, &QWebSocket::disconnected,
             this, &ClientConnection::socketDisconnected );

    connect(m_socket, &QWebSocket::binaryMessageReceived,
            this, &ClientConnection::processBinaryMessage);
}

bool ClientConnection::isConnected(){
    return m_socket->state() == QAbstractSocket::ConnectedState;
}

void ClientConnection::disconnectConnection(){
    m_socket->close();
}

void ClientConnection::processBinaryMessage(const QByteArray& message){
    QJsonDocument doc = QJsonDocument::fromJson( message );
    if( doc.isNull() ){
        LOG4CXX_ERROR( logger, "Docment is null!" );
        return;
    }

    if( logger->isDebugEnabled() ){
        std::string debugDoc = doc.toJson().toStdString();
        LOG4CXX_DEBUG( logger, "Got document: " << debugDoc );
    }

    VideoSenderMessage msg( doc.object() );

    if( msg.command() == "query" ){
        // Send the data back
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
        vidSettings.setRotation( settings.value( "video/rotation", 0 ).toInt() );

//        netSettings.setUdpHost( m_videoSender->ipAddr() );
//        netSettings.setUdpPort( m_videoSender->port() );
//        netSettings.setBroadcast( settings.value( "network/broadcast" ).toBool() );

        QJsonDocument doc( retmsg.jsonObj() );
        m_socket->sendBinaryMessage( doc.toJson() );
    }else if( msg.command() == "restart" ){
        Q_EMIT restartRequested();
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
        settings.setValue( "network/broadcast", msg.configuration().networkSettings().broadcast() );
    }else if( msg.command() == "send-video" ){
        stopSendingVideo();
        m_sendingPort = msg.streaminformation().port();
        m_videoSender->addEndpoint( m_socket->peerAddress(), m_sendingPort );
        m_isSendingVideo = true;
    }else if( msg.command() == "stop-video" ){
        stopSendingVideo();
    }
}

void ClientConnection::socketDisconnected(){
    stopSendingVideo();
}

void ClientConnection::stopSendingVideo(){
    if( !m_isSendingVideo ){
        return;
    }

    m_isSendingVideo = false;
    m_videoSender->removeEndpoint( m_socket->peerAddress(), m_sendingPort );
}

QHostAddress ClientConnection::peerAddress(){
    return m_socket->peerAddress();
}
