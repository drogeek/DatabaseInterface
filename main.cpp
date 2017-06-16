#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QTimer>
#include <QSqlError>
#include <QDebug>
#include <QSettings>
#include <QGuiApplication>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlProperty>
#include <QSystemTrayIcon>
#include <QWindow>
#include <QMenu>
#include <QAction>
#include <QSharedMemory>
#include <QMessageBox>
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
    QApplication app(argc,0);
    QSharedMemory sharedMemory;

    sharedMemory.setKey("WinConnect");
    if(sharedMemory.create(1) == false)
    {
       QMessageBox::warning(NULL, "Warning!", "WinConnect is already running!");
       app.exit(); // exit already a process running
       return 0;
    }

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
    QTcpSocket co;
    co.connectToHost("51.254.204.76",1337);
    /**DB CONNECTION**/
    DataBaseAccess db;
    QObject::connect(&db,&DataBaseAccess::error,[&notifier](QString err){
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

    /**APP CONNECTION**/
    const uint TIMEOUT = 30000;
    QSharedPointer<QTcpSocket> appSocket(new QTcpSocket());
    QTimer reconnectionTimer, heartBeatTimer;
    QObject::connect(&options,&OptionXML::appConfigChanged,&(*appSocket),&QTcpSocket::disconnectFromHost);
    QObject::connect(&reconnectionTimer, QTimer::timeout, [appSocket,&options](){
        qDebug() << "try to reconnect to App";
        appSocket->connectToHost(
            QHostAddress(options.value("app/ip").toString()),
            options.value("app/port").toInt()
            );
        appSocket->waitForConnected(1000);
    });
    QObject::connect(&heartBeatTimer,QTimer::timeout, &notifier, &AbstractNotifier::disconnect);
    QObject::connect(&(*appSocket),&QTcpSocket::readyRead,[&heartBeatTimer](){
        qDebug() << "restarting heartbeat timer";
        heartBeatTimer.start(TIMEOUT);
    });

    QObject::connect(&(*appSocket),&QTcpSocket::disconnected,[&notifier, appSocket, &options, &reconnectionTimer](){
        notifier.disconnect();
        reconnectionTimer.start(1000);
    });
    reconnectionTimer.start(1000);
    QObject::connect(&(*appSocket),&QTcpSocket::connected,[&appSocket,&notifier,&reconnectionTimer,&heartBeatTimer](){
        heartBeatTimer.start(TIMEOUT);
        reconnectionTimer.stop();
        notifier.setSocket(appSocket);
    });

    /**WINMEDIA CONNECTION**/
    Connection connectionRami;
    options.setValue("local/ip","127.0.0.1");
    QObject::connect(&notifier,&ServerNotifier::commandReceived,[&connectionRami,&notifier](QVariantMap map){
       int column = map[ServerNotifier::RAMI_COLUMN].toInt();
       int row = map[ServerNotifier::RAMI_ROW].toInt();
       bool state = map[ServerNotifier::RAMI_STATE].toBool();

       if(connectionRami.connected())
           connectionRami.send(row,column,state);
       else
           notifier.send("Not connected to WinMedia", ServerNotifier::TYPE_ERR,"");
    });
    QObject::connect(&connectionRami,
                    &Connection::commandReceived,
                    &notifier,
                    static_cast<void (ServerNotifier::*)(QVariantMap)>(&ServerNotifier::sendRami)
    );


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

    /*** System tray ***/
    if(QSystemTrayIcon::isSystemTrayAvailable()){
        QSystemTrayIcon * systemTray = new QSystemTrayIcon(QIcon("Logo_WinConnect_SystemTray.png"));
        systemTray->show();
        QMenu * menu = new QMenu();
        QAction * quitAction = new QAction("&Quit");
        QObject::connect(quitAction, &QAction::triggered,QCoreApplication::quit);
        menu->addAction(quitAction);
        systemTray->setContextMenu(menu);
        QObject::connect(systemTray,&QSystemTrayIcon::activated,[&engine](QSystemTrayIcon::ActivationReason reason){
            if(reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick){
                QObject * obj = engine.rootObjects()[0];
                bool isPrinted = QQmlProperty::read(obj,"visible").toBool();
                QQmlProperty::write(obj,"visible", !isPrinted);
            }
        });
    }
    /*** ***/

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
           QObject::connect(&(*socket),&QTcpSocket::disconnected,connectionRami,[connectionRami,notifier](){
               connectionRami->disconnect();
               if(notifier->connected())
                   notifier->send("Disconnected from WinMedia",ServerNotifier::TYPE_ERR,"");
           });
        });
    }
    else
        qDebug() << server->errorString();
}
