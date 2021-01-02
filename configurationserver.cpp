#include <log4cxx/logger.h>
#include <QByteArray>

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

    connect(pSocket, &QWebSocket::binaryMessageReceived, this, &ConfigurationServer::processBinaryMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &ConfigurationServer::socketDisconnected);

    m_clients.push_back( pSocket );
}

void ConfigurationServer::processBinaryMessage( QByteArray message ){
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

    //VideoSettings vidset = msg.configuration().videoSettings();

}

void ConfigurationServer::socketDisconnected(){
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());

    if (pClient) {
        m_clients.removeAll(pClient);
        pClient->deleteLater();
    }
}
