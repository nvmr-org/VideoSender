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
    int videoWidth;
    int videoHeight;
    int configInterval;
    int pt;
    QString udpHost;
    int udpPort;
    int videoFramerate;

    {
        QSettings settings;
        videoWidth = settings.value( "video/width", 1280 ).toInt();
        videoHeight = settings.value( "video/height", 720 ).toInt();
        configInterval = settings.value( "video/convig-interval", 96 ).toInt();
        videoFramerate = settings.value( "video/framerate", 24 ).toInt();
        pt = settings.value( "video/pt", 96 ).toInt();
        udpHost = settings.value( "network/udp-host" ).toString();
        udpPort = settings.value( "network/udp-port", 8230 ).toInt();
    }

    GstElement* v4l2Src = gst_element_factory_make( "v4l2src", "v4l2src" );
    if( !v4l2Src ){
        LOG4CXX_ERROR( logger, "Unable to create v4l2src" );
        error = true;
    }
    GstElement* capsfilter = gst_element_factory_make( "capsfilter", nullptr );
    if( !capsfilter ){
        LOG4CXX_ERROR( logger, "Unable to create capsfilter" );
        error = true;
    }
    GstElement* h264parse = gst_element_factory_make( "h264parse", nullptr );
    if( !h264parse ){
        LOG4CXX_ERROR( logger, "Unable to create h264parse" );
        error = true;
    }
    GstElement* rtph264pay = gst_element_factory_make( "rtph264pay", nullptr );
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

    GstCaps* caps = gst_caps_new_simple("video/x-h264",
            "framerate", GST_TYPE_FRACTION, videoFramerate, 1,
            "width", G_TYPE_INT, videoWidth,
            "height", G_TYPE_INT, videoHeight,
            NULL);
    g_object_set(G_OBJECT(capsfilter), "caps", caps, NULL);

    g_object_set( rtph264pay, "config-interval", configInterval, nullptr );
    g_object_set( rtph264pay, "pt", pt, nullptr );

    g_object_set( udpsink, "sync", false, nullptr );
    {
        std::string strHost = udpHost.toStdString();
        g_object_set( udpsink, "host", strHost.c_str(), nullptr );
    }
    g_object_set( udpsink, "port", udpPort, nullptr );

    gst_bin_add_many(GST_BIN (m_pipeline), v4l2Src, capsfilter, h264parse, rtph264pay, udpsink, NULL);
    gst_element_link_many( v4l2Src, capsfilter, h264parse, rtph264pay, udpsink, NULL);

    GstBus* bus = gst_pipeline_get_bus (GST_PIPELINE (m_pipeline));
    guint bus_watch_id = gst_bus_add_watch (bus, my_bus_callback, NULL);
    gst_object_unref (bus);
}

VideoSender::~VideoSender(){
    gst_object_unref( m_pipeline );
}

void VideoSender::startVideo(){
    gst_element_set_state (m_pipeline, GST_STATE_PLAYING);
}

void VideoSender::stopVideo(){
    gst_element_set_state (m_pipeline, GST_STATE_NULL);
}

void VideoSender::setIpAddr( QString ipAddr ){
    GstElement* udpsink = gst_bin_get_by_name( GST_BIN( m_pipeline ), "udpsink" );
    if( !udpsink ){
        LOG4CXX_ERROR( logger, "Unable to get udpsink from pipeline" );
        return;
    }


}

void VideoSender::setPort( int port ){

}
