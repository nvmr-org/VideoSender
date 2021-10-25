#include <log4cxx/logger.h>

#include <QSettings>
#include <QTimer>

#include <gst/rtsp-server/rtsp-server.h>

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
      LOG4CXX_DEBUG( logger, "End of stream" );
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
    int flipDirection;

    {
        QSettings settings;
        m_width = settings.value( "video/width", 1280 ).toInt();
        m_height = settings.value( "video/height", 720 ).toInt();
        configInterval = settings.value( "video/config-interval", 1 ).toInt();
        m_framerate = settings.value( "video/framerate", 24 ).toInt();
        pt = settings.value( "video/pt", 96 ).toInt();
        flipDirection = settings.value( "video/flip", 0 ).toInt();
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

    GstElement* tee = gst_element_factory_make( "tee", "tee" );
    if( !tee ){
        LOG4CXX_ERROR( logger, "Unable to create tee" );
        error = true;
    }

    GstElement* fakesink = gst_element_factory_make( "fakesink", NULL );
    if( !fakesink){
        LOG4CXX_ERROR( logger, "Unable to create fakesink" );
        error = true;
    }

    if( error ){
        return;
    }

    g_object_set( rtph264pay, "config-interval", configInterval, nullptr );
    g_object_set( rtph264pay, "pt", pt, nullptr );

    gst_bin_add_many(GST_BIN (m_pipeline), v4l2Src, capsfilter, rtph264pay, tee, fakesink, nullptr);
    gst_element_link_many( v4l2Src, capsfilter, rtph264pay, tee, nullptr);
    gst_element_link_many( tee, fakesink, nullptr );

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

//    setupRTSPServer( tee );

    addEndpoint( QHostAddress::LocalHost, 8554 );

    m_rtspProcess.setProgram( "NVMRVideoSenderRTSPHelper" );
    m_rtspProcess.setArguments( QStringList() <<
                                "(udpsrc address=127.0.0.1 port=8554 ! "
                                "application/x-rtp,width=1280,height=720 ! "
                                "rtph264depay ! "
                                "rtph264pay pt=96 config-interval=1 name=pay0)");
    m_rtspProcess.start();
}

VideoSender::~VideoSender(){
    gst_object_unref( m_pipeline );
}

void VideoSender::setupRTSPServer( GstElement* tee ){
    /* create a server instance */
  m_server = gst_rtsp_server_new ();
  int port = 8554;
//  g_object_set (m_server, "service", port, NULL);
  gst_rtsp_server_set_address(m_server, "0.0.0.0");

  /* get the mount points for this server, every server has a default object
   * that be used to map uri mount points to media factories */
  m_mounts = gst_rtsp_server_get_mount_points (m_server);

  /* make a media factory for a test stream. The default media factory can use
   * gst-launch syntax to create pipelines.
   * any launch line works as long as it contains elements named pay%d. Each
   * element with pay%d names will be a stream */
  m_factory = gst_rtsp_media_factory_new ();
  gst_rtsp_media_factory_set_launch (m_factory, "(udpsrc address=127.0.0.1 name=udpsrc ! rtph264pay pt=96 config-interval=1 name=pay0 )");
//  gst_rtsp_media_factory_set_launch (m_factory, "(videotestsrc ! x264enc ! rtph264pay pt=96 config-interval=1 name=pay0 )");
  gst_rtsp_media_factory_set_shared (m_factory, TRUE);
//  gst_rtsp_media_factory_set_enable_rtcp(m_factory, false);

  /* notify when our media is ready, This is called whenever someone asks for
     * the media and a new pipeline with our appsrc is created */
//  g_signal_connect (m_factory, "media-configure", (GCallback) VideoSender::media_configure_cb,
//        this);


  /* attach the test factory to the /rpi-video url */
  gst_rtsp_mount_points_add_factory (m_mounts, "/rpi-video", m_factory);

  /* don't need the ref to the mapper anymore */
  g_object_unref (m_mounts);

  /* attach the server to the default maincontext */
  gst_rtsp_server_attach (m_server, NULL);
}

void VideoSender::startVideo(){
    LOG4CXX_DEBUG( logger, "Starting video" );
    gst_element_set_state (m_pipeline, GST_STATE_PLAYING);
}

void VideoSender::stopVideo(){
    LOG4CXX_DEBUG( logger, "Stopping video" );
    gst_element_set_state (m_pipeline, GST_STATE_NULL);
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

void VideoSender::addEndpoint( QHostAddress addr, int port ){
    LOG4CXX_DEBUG( logger, "Sending video to " << addr.toString().toStdString() << ":" << port );
    QString sinkName = nameForAddressAndPort( addr, port );
    GstElement* udpsink = gst_element_factory_make( "udpsink", sinkName.toStdString().c_str() );
    if( !udpsink ){
        LOG4CXX_ERROR( logger, "Can't create UDP sink??" );
        return;
    }

    std::string hostName = addr.toString().toStdString();
    g_object_set( udpsink, "sync", false, nullptr );
    g_object_set( udpsink, "host", hostName.c_str(), nullptr );
    g_object_set( udpsink, "port", port, nullptr );

    GstElement* tee = gst_bin_get_by_name( GST_BIN( m_pipeline ), "tee" );
    if( !tee ){
        LOG4CXX_ERROR( logger, "Can't get tee??" );
    }

    if( !gst_bin_add( GST_BIN(m_pipeline), udpsink ) ){
        LOG4CXX_ERROR( logger, "Can't add to bin" );
    }

    if( !gst_element_link( tee, udpsink ) ){
        LOG4CXX_ERROR( logger, "Can't link tee" );
    }

    gst_element_set_state( udpsink, GST_STATE_PLAYING );
}

void VideoSender::removeEndpoint( QHostAddress addr, int port ){
    LOG4CXX_DEBUG( logger, "Stopping video to " << addr.toString().toStdString() << ":" << port );
    QString sinkName = nameForAddressAndPort( addr, port );
    GstElement* udpsink = gst_bin_get_by_name( GST_BIN(m_pipeline), sinkName.toStdString().c_str() );
    if( !udpsink ){
        LOG4CXX_ERROR( logger, "Can't find the UDP sink to this address" );
        return;
    }

    GstStateChangeReturn ret = gst_element_set_state( udpsink, GST_STATE_PAUSED );
    if( ret != GST_STATE_CHANGE_SUCCESS ){
        LOG4CXX_ERROR( logger, "Can't set state to paused: " << ret );
        QTimer::singleShot( 300, [this,addr,port](){
            this->removeEndpoint(addr,port);
        });
        return;
    }

    GstElement* tee = gst_bin_get_by_name( GST_BIN( m_pipeline ), "tee" );
    if( !tee ){
    }
    gst_element_unlink( tee, udpsink );
    gst_bin_remove( GST_BIN( m_pipeline ), udpsink );
}

QString VideoSender::nameForAddressAndPort( QHostAddress addr, int port ){
    return addr.toString() + ":" + port;
}

void VideoSender::media_configure_cb (GstRTSPMediaFactory * factory,
                                    GstRTSPMedia * media,
                         gpointer user_data){
    VideoSender* vidsend = static_cast<VideoSender*>( user_data );
    vidsend->media_configure( factory, media );
}

void VideoSender::media_configure(GstRTSPMediaFactory * factory,
                         GstRTSPMedia * media){

    GstElement *element, *udpsrc;
    GstElement* tee;
    GstElement* proxysink;

    /* get the element used for providing the streams of the media */
    element = gst_rtsp_media_get_element (media);

    /* get our first element */
    udpsrc = gst_bin_get_by_name_recurse_up (GST_BIN (element), "udpsrc");
    if( !udpsrc ){
        LOG4CXX_ERROR( logger, "Can't get proxysrc" );
    }

    int port = 0;
    g_object_get(G_OBJECT(udpsrc), "port", &port, nullptr );
    addEndpoint( QHostAddress::LocalHost, port );

//    tee = gst_bin_get_by_name( GST_BIN( m_pipeline ), "tee" );
//    if( !tee ){
//        LOG4CXX_ERROR( logger, "Can't get tee??" );
//        return;
//    }

//    proxysink = gst_element_factory_make( "proxysink", nullptr );
//    if( !proxysink ){
//        LOG4CXX_ERROR( logger, "Can't create proxy sink??" );
//        return;
//    }

////    GstClock* clock = gst_system_clock_obtain();
////    gst_pipeline_use_clock(GST_PIPELINE(m_pipeline), clock );
////    gst_pipeline_use_clock(GST_PIPELINE(element), clock);
////    g_object_unref(clock);

//    gst_element_set_base_time(udpsrc, gst_element_get_base_time(tee));

//    // Link the two proxies together
//    g_object_set (udpsrc, "proxysink", proxysink, NULL);
////    g_object_set( proxysrc, "sync", false, nullptr );

//    if( !gst_bin_add( GST_BIN(m_pipeline), proxysink ) ){
//        LOG4CXX_ERROR( logger, "Can't add to bin" );
//    }

//    if( !gst_element_link( tee, proxysink ) ){
//        LOG4CXX_ERROR( logger, "Can't link tee" );
//    }

//    gst_element_set_state( proxysink, GST_STATE_PLAYING );

//    gst_object_unref (udpsrc);
//    gst_object_unref (element);

}
