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

    bool alreadyThere = false;
    for( std::shared_ptr<ClientConnection> client : m_clients ){
        if( client->peerAddress() == pSocket->peerAddress() ){
            alreadyThere = true;
            break;
        }
    }

    if( alreadyThere ){
        LOG4CXX_DEBUG( logger, "Ignoring new connection: it already exists" );
        pSocket->close();
        pSocket->deleteLater();
        return;
    }

    std::shared_ptr<ClientConnection> clientConn( new ClientConnection(pSocket, m_videoSender) );

    connect( clientConn.get(), &ClientConnection::clientDisconnected,
             this, &ConfigurationServer::clientDisconnected );
    connect( clientConn.get(), &ClientConnection::restartRequested,
             this, &ConfigurationServer::disconnectAndRestart );

    m_clients.push_back( clientConn );
}

void ConfigurationServer::disconnectAndRestart(){
    for( std::shared_ptr<ClientConnection> client : m_clients ){
        client->disconnectConnection();
    }
    QTimer::singleShot( 100, [](){
        QCoreApplication::instance()->exit( 2 );
    });
}

void ConfigurationServer::clientDisconnected(){
    std::remove_if( std::begin(m_clients),
                    std::end(m_clients),
                    [](std::shared_ptr<ClientConnection> conn){
        return !conn->isConnected();
    });
}
