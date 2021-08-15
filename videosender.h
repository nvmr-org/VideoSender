#ifndef VIDEOSENDER_H
#define VIDEOSENDER_H

#include <QObject>
#include <QHostAddress>
#include <gst/gst.h>

class VideoSender : public QObject
{
    Q_OBJECT
public:
    explicit VideoSender(QObject *parent = nullptr);
    ~VideoSender();

    int videoWidth() const;
    int videoHeight() const;
    int configInterval() const;
    int framerate() const;
    int pt() const;

    void addEndpoint( QHostAddress addr, int port );
    void removeEndpoint( QHostAddress addr, int port );

Q_SIGNALS:

public Q_SLOTS:
    void startVideo();
    void stopVideo();

private:
    void configureCaps();
    void configureBroadcast();
    QString nameForAddressAndPort( QHostAddress addr, int port );

private:
    GstElement* m_pipeline;

    int m_width;
    int m_height;
    int m_framerate;
};

#endif // VIDEOSENDER_H
