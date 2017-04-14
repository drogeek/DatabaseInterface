#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QTimer>
#include <QSqlError>
#include <QDebug>
#include "servernotifier.h"
#include "query2json.h"
#define __WINMEDIA_DEBUG
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

#ifdef __linux__
    const QString DRIVER = "ODBC Driver 13 for SQL Server";
#elif _WIN32
    const QString DRIVER = "SQL Server";
#endif
    QTcpServer server;


    //TODO: to be replaced by data provided in a file
    const QString DBSERVER = "127.0.0.1";
    const QString DBPORT = "1433";
    const QString DBUSER = "test";
    const QString DBPASSWORD = "test";
    const QString NAME = "CartridgeApplication";
    const QString IP = "127.0.0.1";
    const int PORT = 1337;


    QStringList list;
    list.append("test");
    list.append("test2");
    QJsonArray arr = QJsonArray::fromStringList(list);
    QJsonObject obj;
    obj.insert("ok",QJsonValue("test3"));
    obj.insert("arr",arr);
    qDebug() << obj;
#ifdef __WINMEDIA_DEBUG
    //Show available drivers
    QStringList drivers = QSqlDatabase::drivers();
    QStringList::const_iterator constIter;
    qDebug() << "Available drivers:";
    for(constIter=drivers.constBegin(); constIter != drivers.constEnd(); constIter++){
        qDebug() << (*constIter).toLocal8Bit().constData();
    }
#endif

/* ******DB PART****** */
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName(
                "Driver={"+DRIVER+"};\
                Server="+DBSERVER+","+DBPORT+";\
                Uid="+DBUSER+";\
                Pwd="+DBPASSWORD+";"
    );

    QTimer timer;
    if(db.open()){
        qDebug() << "Opening of DB successful";
        //Record ourselves in the Computer table on the DB
        QSqlQuery query;
        qDebug() << QString("SELECT Ip \
                            FROM [Winmedia].[dbo].[Computer] \
                            WHERE Name = '%1'").arg(NAME);
        auto result=query.exec(QString("SELECT Name,Ip \
                                FROM [Winmedia].[dbo].[Computer] \
                                WHERE Name = '%1'").arg(NAME));
        if(!result){
            qDebug() << query.lastError();
        }
        else{
            query.next();
            if(!query.isValid()){
                qDebug() << "Can't find you in the DB, adding you";
                QSqlQuery insert;
                auto result = insert.exec(QString(
                                     "INSERT INTO [WinMedia].[dbo].[Computer] (Name,Ip,Modify,X,Y,Command,Properties) \
                                     VALUES ('"+NAME+"','"+IP+"',getutcdate(),0,0,'','')")
                                    );
                if(!result){
                    qDebug() << insert.lastError();
                }
            }
            else{
                auto updateModify = [NAME](){
                    QSqlQuery update;
                    auto result = update.exec(
                                "UPDATE [WinMedia].[dbo].[Computer] \
                                SET modify=getutcdate() \
                                WHERE name = '"+NAME+"'"
                            );
                    if(!result){
                        qDebug() << update.lastError();
                    }
                };
                updateModify();
                QObject::connect(&timer,&QTimer::timeout, updateModify);
                timer.start(5000);
            }
        }

    }
    else
        qDebug() << db.lastError();
/* ************ */

/* ******SERVER PART****** */
    server.listen(QHostAddress(IP),PORT);
    qDebug() << server.errorString();
    ServerNotifier notifier;
    QObject::connect(&server, &QTcpServer::newConnection, [&server,&notifier](){
       qDebug() << "new connection";
       QSharedPointer<QTcpSocket> socket(server.nextPendingConnection());
       notifier.setSocket(socket);
    });
/* ************ */

    return a.exec();
}
