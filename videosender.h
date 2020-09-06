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

signals:

public slots:
    void startVideo();
    void stopVideo();

private:
    GstElement* m_pipeline;
};

#endif // VIDEOSENDER_H
