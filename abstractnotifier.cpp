#include "abstractnotifier.h"


const QString AbstractNotifier::CMDFILE = "WINMEDIA.TMP";

const QString AbstractNotifier::NOTIFY = "NOTIFY";
const QString AbstractNotifier::DB = "DB";
const QString AbstractNotifier::PANEL = "panel";
const QString AbstractNotifier::CARTRIDGE = "cartridge";
const QString AbstractNotifier::JSON_TYPE = "type";
const QString AbstractNotifier::JSON_TARGET = "target";
const QString AbstractNotifier::JSON_DATA = "data";

const AbstractNotifier::NotifType AbstractNotifier::notifType;

AbstractNotifier::AbstractNotifier(){
    connect(this, &AbstractNotifier::newNotification,this,&AbstractNotifier::notification);
    connect(this,&AbstractNotifier::newQuery,this,&AbstractNotifier::query);
}

AbstractNotifier::AbstractNotifier(QSharedPointer<QTcpSocket> sock)
    : AbstractNotifier()
{
    setSocket(sock);
}

void AbstractNotifier::setSocket(QSharedPointer<QTcpSocket> sock){
    m_sock = sock;
    qDebug() << "setSocket called";
    qDebug() << m_sock;
    connect(&(*m_sock),&QTcpSocket::readyRead,this,&AbstractNotifier::parse);
}

void AbstractNotifier::parse(){
    qDebug() << "parse";
    QByteArray arr=m_sock->readAll();
    auto list = arr.split('\n');
    for(auto i : list){
        qDebug() << "data received: " << i;
        auto jsonDoc = QJsonDocument::fromJson(i);
        qDebug() << "jsonDoc: " << jsonDoc;
        auto jsonObj = jsonDoc.object();
        if(jsonObj[JSON_TYPE].toString() == NOTIFY){
            emit newNotification(jsonObj[JSON_TARGET].toString(),jsonObj[JSON_DATA]);
        }
        else if(jsonObj[JSON_TYPE].toString() == DB){
            emit newQuery(jsonObj[JSON_TARGET].toString(),jsonObj[JSON_DATA]);
        }
    }
}

QJsonDocument AbstractNotifier::wrapWithType(QJsonValue data,QString type,QString target){
    QJsonDocument result;
    QJsonObject wrapper;
    wrapper.insert(JSON_TYPE,type);
    wrapper.insert(JSON_DATA,data);
    wrapper.insert(JSON_TARGET,target);
    result.setObject(wrapper);
    return result;
}

void AbstractNotifier::send(QJsonValue value, QString type, QString target){
    QJsonDocument result;
    if(value.isArray())
        result = wrapWithType(value.toArray(),type,target);
    else if(value.isString())
        result = wrapWithType(value.toString(),type,target);
    else if(value.isObject())
        result = wrapWithType(value.toObject(),type,target);
    //TODO: error?
    m_sock->write(result.toJson(QJsonDocument::Compact)+'\n');
}
