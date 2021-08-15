#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H

#include <QObject>
#include <QWebSocket>

class VideoSender;

class ClientConnection : public QObject
{
    Q_OBJECT
public:
    explicit ClientConnection(QWebSocket* socket,
                              VideoSender*,
                              QObject *parent = nullptr);

    bool isConnected();
    void disconnectConnection();

Q_SIGNALS:
    void clientDisconnected();
    void restartRequested();

public Q_SLOTS:
    void processBinaryMessage(const QByteArray& message);
    void socketDisconnected();

private:
    QWebSocket* m_socket;
    VideoSender* m_videoSender;
};

#endif // CLIENTCONNECTION_H
