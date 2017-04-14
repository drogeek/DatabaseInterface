#include "servernotifier.h"

ServerNotifier::ServerNotifier()
    :AbstractNotifier(){

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
        auto tabName = obj[JSON_DATA].toString();
        //TODO: allows the user to list and select a different one
        QString fileStr = "//DESKTOP-8EBR5IK/WinMedia";
        QDir dir(fileStr);
        if(dir.exists()){
            qDebug() << dir.entryList();
            QFile file(CMDFILE);
            QDir::setCurrent(dir.path());
            file.open(QIODevice::WriteOnly);
            file.write("!. Cartridge-Load "+tabName.toUtf8());
        }
        else
            qDebug() << "dir doesn't exist";
    }
    else
        qDebug() << "received an invalid notification";
}

void ServerNotifier::query(QString target,QJsonValue value){
    qDebug() << "query received";
    auto jsonArray = Query2Json::exec(value.toString().toUtf8());
    send(jsonArray,DB,target);
}


