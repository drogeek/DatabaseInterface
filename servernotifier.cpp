#include "servernotifier.h"

const QString ServerNotifier::QUERY = QString("\
           SELECT Name\
           FROM [Winmedia].[dbo].[Panel]\
           WHERE IPanel = %1"
        );

ServerNotifier::ServerNotifier()
    :AbstractNotifier(){
    connect(this,&ServerNotifier::newNotification,this,&notification);
    connect(this,&ServerNotifier::newQuery,this,&query);
}

ServerNotifier::ServerNotifier(QSharedPointer<QTcpSocket> sock)
    : AbstractNotifier(sock)
{
}

void ServerNotifier::notification(QString target,QJsonValue value){
    QJsonObject obj = value.toObject();
    if(obj[JSON_TYPE].toString() == ServerNotifier::notifType.CELLHASCHANGED){
        //TODO: notify
        qDebug() << "supposed to refresh view in WinMedia";
    }
    else if(obj[JSON_TYPE].toString() == ServerNotifier::notifType.LOOPHASCHANGED){
        //TODO: notify
        qDebug() << "supposed to refresh view in WinMedia";
    }
    else if(obj[JSON_TYPE].toString() == ServerNotifier::notifType.TABHASCHANGED){
        qDebug() << "notify via shared Memory tab changed";
        auto tabId = obj[JSON_DATA].toInt();
        qDebug() << "tabId: " << tabId;
        QSqlQuery query;
        auto result = query.exec(QUERY.arg(tabId));
        if(result){
            query.next();
            auto tabName = query.value(0).toString();
            qDebug() << "tabName: " << tabName;
            //TODO: allows the user to list and select a different one
            QString fileStr = "//127.0.0.1/WinMedia";
            QDir dir(fileStr);
            if(dir.exists()){
                qDebug() << dir.entryList();
                QFile file(CMDFILE);
                QDir::setCurrent(dir.path());
                file.open(QIODevice::WriteOnly);
                file.write("!. Cartridge-Load "+QString(tabName).toUtf8());
            }
            else
                qDebug() << "dir doesn't exist";
        }

        else
            qDebug() << query.lastError();
    }
    else
        qDebug() << "received an invalid notification";
}

void ServerNotifier::query(QString target,QJsonValue value){
    qDebug() << "query received";
    try{
        auto jsonArray = Query2Json::exec(value.toString().toUtf8());
        send(jsonArray,TYPE_DB,target);
    }
    catch( std::exception & e){
        send(e.what(),TYPE_ERR,"");
    }
}

void ServerNotifier::send(QJsonValue value, QString type, QString target){
    qDebug() << "data sent";
    AbstractNotifier::send(value,type,target);
}
