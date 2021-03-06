#include <log4cxx/logger.h>

#include <QSettings>

#include "videosender.h"

static log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger( "org.nvmr.VideoSender" );

static gboolean
my_bus_callback (GstBus * bus, GstMessage * message, gpointer data)
{
  switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_ERROR:{
      GError *err;
      gchar *debug;


      gst_message_parse_error (message, &err, &debug);
      LOG4CXX_ERROR( logger, err->message );
      g_error_free (err);
      g_free (debug);

      break;
    }
    case GST_MESSAGE_EOS:
      /* end-of-stream */
      //g_main_loop_quit (loop);
      break;
    default:
      /* unhandled message */
      //LOG4CXX_DEBUG( logger, "Message type " << GST_MESSAGE_TYPE(message) );
      break;
  }

  /* we want to be notified again the next time there is a message
   * on the bus, so returning TRUE (FALSE means we want to stop watching
   * for messages on the bus and our callback should not be called again)
   */
  return TRUE;
}

VideoSender::VideoSender(QObject *parent) : QObject(parent)
{
    m_pipeline = gst_pipeline_new( "pipeline" );
    bool error = false;
    int configInterval;
    int pt;
    QString udpHost;
    int udpPort;

    {
        QSettings settings;
        m_width = settings.value( "video/width", 1280 ).toInt();
        m_height = settings.value( "video/height", 720 ).toInt();
        configInterval = settings.value( "video/config-interval", 1 ).toInt();
        m_framerate = settings.value( "video/framerate", 24 ).toInt();
        pt = settings.value( "video/pt", 96 ).toInt();
        udpHost = settings.value( "network/udp-host" ).toString();
        udpPort = settings.value( "network/udp-port", 8230 ).toInt();
    }

    GstElement* v4l2Src = gst_element_factory_make( "v4l2src", "v4l2src" );
    if( !v4l2Src ){
        LOG4CXX_ERROR( logger, "Unable to create v4l2src" );
        error = true;
    }
    GstElement* capsfilter = gst_element_factory_make( "capsfilter", "capsfilter" );
    if( !capsfilter ){
        LOG4CXX_ERROR( logger, "Unable to create capsfilter" );
        error = true;
    }
    GstElement* rtph264pay = gst_element_factory_make( "rtph264pay", "rtph264pay" );
    if( !rtph264pay ){
        LOG4CXX_ERROR( logger, "Unable to create rtph264pay" );
        error = true;
    }
    GstElement* udpsink = gst_element_factory_make( "udpsink", "udpsink" );
    if( !udpsink ){
        LOG4CXX_ERROR( logger, "Unable to create udpsink" );
        error = true;
    }

    if( error ){
        return;
    }

    g_object_set( rtph264pay, "config-interval", configInterval, nullptr );
    g_object_set( rtph264pay, "pt", pt, nullptr );

    g_object_set( udpsink, "sync", false, nullptr );
    {
        std::string strHost = udpHost.toStdString();
        g_object_set( udpsink, "host", strHost.c_str(), nullptr );
    }
    g_object_set( udpsink, "port", udpPort, nullptr );
    g_object_set( udpsink, "auto-multicast", true, nullptr );

    gst_bin_add_many(GST_BIN (m_pipeline), v4l2Src, capsfilter, rtph264pay, udpsink, NULL);
    gst_element_link_many( v4l2Src, capsfilter, rtph264pay, udpsink, NULL);

    LOG4CXX_DEBUG( logger, "framerate: " << m_framerate << " width: " << m_width << " height: " << m_height );
    GstCaps* caps = gst_caps_new_simple("video/x-h264",
            "framerate", GST_TYPE_FRACTION, m_framerate, 1,
            "width", G_TYPE_INT, m_width,
            "height", G_TYPE_INT, m_height,
            NULL);
    g_object_set(G_OBJECT(capsfilter), "caps", caps, NULL);
    gst_caps_unref( caps );

    GstBus* bus = gst_pipeline_get_bus (GST_PIPELINE (m_pipeline));
    guint bus_watch_id = gst_bus_add_watch (bus, my_bus_callback, NULL);
    gst_object_unref (bus);
}

VideoSender::~VideoSender(){
    gst_object_unref( m_pipeline );
}

void VideoSender::startVideo(){
    GstElement* udpsink = gst_bin_get_by_name( GST_BIN( m_pipeline ), "udpsink" );
    if( !udpsink ){
        LOG4CXX_ERROR( logger, "Unable to get udpsink from pipeline" );
        return;
    }

    char* host = nullptr;
    int port = 0;

    g_object_get( udpsink, "host", &host, nullptr );
    g_object_get( udpsink, "port", &port, nullptr );
    LOG4CXX_DEBUG( logger, "Starting video " << host << ":" << port );
    gst_element_set_state (m_pipeline, GST_STATE_PLAYING);
}

void VideoSender::stopVideo(){
    LOG4CXX_DEBUG( logger, "Stopping video" );
    gst_element_set_state (m_pipeline, GST_STATE_NULL);
}

void VideoSender::setIpAddr( QString ipAddr ){
    GstElement* udpsink = gst_bin_get_by_name( GST_BIN( m_pipeline ), "udpsink" );
    if( !udpsink ){
        LOG4CXX_ERROR( logger, "Unable to get udpsink from pipeline" );
        return;
    }

    std::string strHost = ipAddr.toStdString();
    g_object_set( udpsink, "host", strHost.c_str(), nullptr );
}

void VideoSender::setPort( int port ){
    GstElement* udpsink = gst_bin_get_by_name( GST_BIN( m_pipeline ), "udpsink" );
    if( !udpsink ){
        LOG4CXX_ERROR( logger, "Unable to get udpsink from pipeline" );
        return;
    }

    g_object_set( udpsink, "port", port, nullptr );
}

int VideoSender::videoWidth() const {
    return m_width;
}

int VideoSender::videoHeight() const {
    return m_height;
}

void VideoSender::configureCaps() {
    GstElement* capsfilter = gst_bin_get_by_name( GST_BIN( m_pipeline ), "capsfilter" );

    GstCaps* caps = gst_caps_new_simple("video/x-h264",
            "framerate", GST_TYPE_FRACTION, m_framerate, 1,
            "width", G_TYPE_INT, m_width,
            "height", G_TYPE_INT, m_height,
            NULL);
    g_object_set(G_OBJECT(capsfilter), "caps", caps, NULL);
}

int VideoSender::configInterval() const {
    GstElement* rtph264pay = gst_bin_get_by_name( GST_BIN( m_pipeline ), "rtph264pay" );
    if( !rtph264pay ){
        LOG4CXX_ERROR( logger, "Unable to get rtph264pay from pipeline" );
        return -1;
    }

    int value;
    g_object_get( G_OBJECT(rtph264pay), "config-interval", &value, nullptr );
    return value;
}

int VideoSender::framerate() const{
    return m_framerate;
}

int VideoSender::pt() const {
    GstElement* rtph264pay = gst_bin_get_by_name( GST_BIN( m_pipeline ), "rtph264pay" );
    if( !rtph264pay ){
        LOG4CXX_ERROR( logger, "Unable to get rtph264pay from pipeline" );
        return -1;
    }

    int value;
    g_object_get( G_OBJECT(rtph264pay), "pt", &value, nullptr );
    return value;
}

QString VideoSender::ipAddr() const{
    GstElement* udpsink = gst_bin_get_by_name( GST_BIN( m_pipeline ), "udpsink" );
    if( !udpsink ){
        LOG4CXX_ERROR( logger, "Unable to get udpsink from pipeline" );
        return "";
    }

    char* ipAddr = nullptr;
    g_object_get( G_OBJECT(udpsink), "host", &ipAddr, nullptr );
    return QString::fromUtf8( ipAddr );
}

int VideoSender::port() const{
    GstElement* udpsink = gst_bin_get_by_name( GST_BIN( m_pipeline ), "udpsink" );
    if( !udpsink ){
        LOG4CXX_ERROR( logger, "Unable to get udpsink from pipeline" );
        return 0;
    }

    int port;
    g_object_get( G_OBJECT(udpsink), "port", &port, nullptr );
    return port;
}
