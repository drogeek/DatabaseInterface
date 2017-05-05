#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QTimer>
#include <QSqlError>
#include <QDebug>
#include <QSettings>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "servernotifier.h"
#include "query2json.h"
#include "connection.h"
#include "databaseaccess.h"
#include "optionxml.h"
#define __WINMEDIA_DEBUG
void connectToWin(QSharedPointer<QTcpServer> server, Connection* connectionRami, ServerNotifier* notifier, OptionXML* options);
int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc,argv);


    QCoreApplication::setOrganizationName("WinMedia");
    QCoreApplication::setOrganizationDomain("winmedia.org");
    QCoreApplication::setApplicationName("Delegate");
    OptionXML options;

#ifdef __WINMEDIA_DEBUG
    //Show available drivers
    QStringList drivers = QSqlDatabase::drivers();
    QStringList::const_iterator constIter;
    qDebug() << "Available drivers:";
    for(constIter=drivers.constBegin(); constIter != drivers.constEnd(); constIter++){
        qDebug() << (*constIter).toLocal8Bit().constData();
    }
#endif

    ServerNotifier notifier;
    /**DB CONNECTION**/
    DataBaseAccess db;
    QObject::connect(&db,DataBaseAccess::error,[&notifier](QString err){
        if(notifier.connected())
            notifier.send(err, ServerNotifier::TYPE_ERR,"");
    });
    db.setDatabase(
                options.value("db/ip").toString(),
                options.value("db/port").toInt(),
                options.value("db/user").toString(),
                options.value("db/pass").toString()
                );
    QObject::connect(&options,&OptionXML::dbConfigChanged,[&options,&db](){
        db.setDatabase(
                options.value("db/ip").toString(),
                options.value("db/port").toInt(),
                options.value("db/user").toString(),
                options.value("db/pass").toString()
                );
    });
    //TODO: connect error to view

    /**APP CONNECTION**/
    QSharedPointer<QTcpSocket> appSocket(new QTcpSocket());
    QTimer appTimer;
    QObject::connect(&options,&OptionXML::appConfigChanged,&(*appSocket),&QTcpSocket::disconnectFromHost);
    QObject::connect(&appTimer, QTimer::timeout, [appSocket,&options](){
        qDebug() << "try to reconnect to App";
        appSocket->connectToHost(
            QHostAddress(options.value("app/ip").toString()),
            options.value("app/port").toInt()
            );
        appSocket->waitForConnected(1000);
    });
    QObject::connect(&(*appSocket),&QTcpSocket::disconnected,[&notifier, appSocket, &options, &appTimer](){
        notifier.disconnect();
        appTimer.start(1000);
    });
    appTimer.start(1000);
    QObject::connect(&(*appSocket),&QTcpSocket::connected,[&appSocket,&notifier,&appTimer](){
        appTimer.stop();
        notifier.setSocket(appSocket);
    });

    /**WINMEDIA CONNECTION**/
    Connection connectionRami;
    options.setValue("local/ip","127.0.0.1");

    QSharedPointer<QTcpServer> server(new QTcpServer());
    connectToWin(server,&connectionRami,&notifier, &options);
    QObject::connect(&options,&OptionXML::winConfigChanged,[&server,&notifier,&connectionRami,&options](){
        qDebug() << server->isListening();
        if (server->isListening()){
            server->close();
            connectionRami.disconnect();
            server = QSharedPointer<QTcpServer>(new QTcpServer());
        }
        connectToWin(server,&connectionRami,&notifier, &options);
    });

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("Rami", &connectionRami);
    engine.rootContext()->setContextProperty("Options", &options);
    engine.rootContext()->setContextProperty("Notifier", &notifier);
    engine.rootContext()->setContextProperty("Database", &db);

    engine.load(QUrl(QLatin1String("qrc:/main.qml")));
    return app.exec();
}

void connectToWin(QSharedPointer<QTcpServer> server, Connection* connectionRami, ServerNotifier* notifier, OptionXML* options){
    bool serverResp = server->listen(
                    QHostAddress(options->value("local/ip").toString()),
                    options->value("win/port").toInt()
                    );
    if(serverResp){
        QObject::connect(&(*server), &QTcpServer::newConnection, [server,notifier,connectionRami](){
           qDebug() << "new connection";

           QSharedPointer<QTcpSocket> socket(server->nextPendingConnection());
           connectionRami->setSocket(socket);
           QObject::connect(&(*socket),&QTcpSocket::readyRead,connectionRami,&Connection::receive);
           QObject::connect(&(*socket),&QTcpSocket::disconnected,connectionRami,&Connection::disconnect);
           QObject::connect(notifier,&ServerNotifier::commandReceived,[connectionRami,notifier](QVariantMap map){
               int column = map[ServerNotifier::RAMI_COLUMN].toInt();
               int row = map[ServerNotifier::RAMI_ROW].toInt();
               bool state = map[ServerNotifier::RAMI_STATE].toBool();

               if(connectionRami->connected())
                   connectionRami->send(row,column,state);
               else{
                   notifier->send("Not connected to WinMedia", ServerNotifier::TYPE_ERR,"");
               }
           });
           QObject::connect(connectionRami,
                            &Connection::commandReceived,
                            notifier,
                            static_cast<void (ServerNotifier::*)(QVariantMap)>(&ServerNotifier::sendRami)
           );
        });
    }
    else
        qDebug() << server->errorString();
}
