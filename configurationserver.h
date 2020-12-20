#ifndef CONFIGURATIONSERVER_H
#define CONFIGURATIONSERVER_H

#include <QObject>
#include <QtWebSockets/QtWebSockets>
#include <QList>

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
    void processBinaryMessage(QByteArray message);
    void socketDisconnected();

private:
    QWebSocketServer* m_server;
    VideoSender* m_videoSender;
    QList<QWebSocket*> m_clients;
};

#endif // CONFIGURATIONSERVER_H
