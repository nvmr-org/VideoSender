#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QSettings>
#include <QUuid>
#include <QTimer>

#include <log4cxx/logger.h>
#include <log4cxx/xml/domconfigurator.h>

#include <gst/gst.h>

#include "videosender.h"
#include "configurationserver.h"

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

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setOrganizationName( "NVMR" );

    initialize_logging();

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

    return a.exec();
}
