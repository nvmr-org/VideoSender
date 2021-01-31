#ifndef VIDEOSENDER_H
#define VIDEOSENDER_H

#include <QObject>
#include <gst/gst.h>

class VideoSender : public QObject
{
    Q_OBJECT
public:
    explicit VideoSender(QObject *parent = nullptr);
    ~VideoSender();

    void setIpAddr( QString ipAddr );
    void setPort( int port );

    int videoWidth() const;
    int videoHeight() const;
    int configInterval() const;
    int framerate() const;
    int pt() const;
    QString ipAddr() const;
    int port() const;

Q_SIGNALS:

public Q_SLOTS:
    void startVideo();
    void stopVideo();

private:
    void configureCaps();

private:
    GstElement* m_pipeline;

    int m_width;
    int m_height;
    int m_framerate;
};

#endif // VIDEOSENDER_H
