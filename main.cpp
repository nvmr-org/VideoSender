#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QSettings>
#include <QUuid>
#include <QTimer>

#include <log4cxx/logger.h>
#include <log4cxx/xml/domconfigurator.h>

#include <gst/gst.h>

#include <dbus-cxx.h>

#include "videosender.h"
#include "configurationserver.h"
#include "avahi.h"

static log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger( "org.nvmr.nvmrvideosender" );

static void initialize_logging(){
    QString configLocation =
                QStandardPaths::standardLocations( QStandardPaths::AppConfigLocation ).first();

    QDir d( configLocation );
    if( !d.exists() ){
        d.mkpath( "." );
    }

    QString logconfigFile = configLocation + "/logconfig.xml";
    log4cxx::xml::DOMConfigurator::configure( logconfigFile.toStdString() );
}

static void dbuscxx_logger_callback( const char* logger_name,
                                     const struct ::SL_LogLocation* location,
                                     const enum ::SL_LogLevel level,
                                     const char* log_string ){
    log4cxx::LoggerPtr dbusLogger = log4cxx::Logger::getLogger( logger_name );
    log4cxx::spi::LocationInfo locInfo( location->file, location->function, location->line_number );

    switch( level ){
    case SL_TRACE:
        dbusLogger->trace( log_string, locInfo );
        break;
    case SL_DEBUG:
        dbusLogger->debug( log_string, locInfo );
        break;
    case SL_INFO:
        dbusLogger->info( log_string, locInfo );
        break;
    case SL_WARN:
        dbusLogger->warn( log_string, locInfo );
        break;
    case SL_ERROR:
        dbusLogger->error( log_string, locInfo );
        break;
    case SL_FATAL:
        dbusLogger->fatal( log_string, locInfo );
        break;
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setOrganizationName( "NVMR" );

    initialize_logging();
    DBus::setLoggingFunction( dbuscxx_logger_callback );

    LOG4CXX_INFO( logger, "NVMR Video Sender starting up" );

    gst_init( nullptr, nullptr );

    // Generate UUID if one does not exist
    {
        QSettings settings;
        QUuid uuid = settings.value( "device-uuid", QUuid() ).toUuid();
        if( uuid.isNull() ){
            uuid = QUuid::createUuid();
            LOG4CXX_DEBUG( logger, "Creating UUID of " << uuid.toString().toStdString() );
            settings.setValue( "device-uuid", uuid );
        }
    }

    VideoSender send;
    QTimer::singleShot( 0, &send, &VideoSender::startVideo );

    ConfigurationServer srv;
    srv.setVideoSender( &send );

    AvahiControl avahi;
    QTimer::singleShot( 0, &avahi, &AvahiControl::registerWithAvahi );

    return a.exec();
}
