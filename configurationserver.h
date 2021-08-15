#ifndef CONFIGURATIONSERVER_H
#define CONFIGURATIONSERVER_H

#include <QObject>
#include <QtWebSockets/QtWebSockets>
#include <QVector>

#include "clientconnection.h"

class VideoSender;

class ConfigurationServer : public QObject
{
    Q_OBJECT
public:
    explicit ConfigurationServer(QObject *parent = nullptr);

    void setVideoSender( VideoSender* vidsend );

Q_SIGNALS:

public Q_SLOTS:
private Q_SLOTS:
    void onNewConnection();
    void disconnectAndRestart();
    void clientDisconnected();

private:
    QWebSocketServer* m_server;
    VideoSender* m_videoSender;
    QVector<std::shared_ptr<ClientConnection>> m_clients;
};

#endif // CONFIGURATIONSERVER_H
