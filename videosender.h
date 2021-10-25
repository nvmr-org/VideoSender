#ifndef VIDEOSENDER_H
#define VIDEOSENDER_H

#include <QObject>
#include <QHostAddress>
#include <QProcess>
#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>

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
    void setupRTSPServer( GstElement* tee  );
    static void media_configure_cb (GstRTSPMediaFactory * factory,
                                    GstRTSPMedia * media,
                                    gpointer user_data);
    void media_configure(GstRTSPMediaFactory * factory,
                         GstRTSPMedia * media);

private:
    GstElement* m_pipeline;
    GstRTSPServer *m_server;
    GstRTSPMountPoints *m_mounts;
    GstRTSPMediaFactory *m_factory;
    QProcess m_rtspProcess;

    int m_width;
    int m_height;
    int m_framerate;
};

#endif // VIDEOSENDER_H
