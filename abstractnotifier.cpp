#include "abstractnotifier.h"


const QString AbstractNotifier::CMDFILE = "WINMEDIA.TMP";

//types
const QString AbstractNotifier::TYPE_NOTIFY = "NOTIFY";
const QString AbstractNotifier::TYPE_DB = "DB";
const QString AbstractNotifier::TYPE_RAMI = "RAMI";
const QString AbstractNotifier::TYPE_ERR = "ERR";

const QString AbstractNotifier::TARGET_PANEL = "panel";
const QString AbstractNotifier::TARGET_CARTRIDGE = "cartridge";

const QString AbstractNotifier::JSON_TYPE = "type";
const QString AbstractNotifier::JSON_TARGET = "target";
const QString AbstractNotifier::JSON_DATA = "data";

const QString AbstractNotifier::RAMI_COLUMN = "column";
const QString AbstractNotifier::RAMI_ROW = "row";
const QString AbstractNotifier::RAMI_STATE = "state";

const AbstractNotifier::NotifType AbstractNotifier::notifType;

AbstractNotifier::AbstractNotifier(){
//    connect(this, &AbstractNotifier::newNotification,this,&AbstractNotifier::notification);
//    connect(this,&AbstractNotifier::newQuery,this,&AbstractNotifier::query);
    setConnected(false);
    connect(this,&AbstractNotifier::newRami,this,&AbstractNotifier::parseRami);
//    emit disconnected();
}

AbstractNotifier::AbstractNotifier(QSharedPointer<QTcpSocket> sock)
    : AbstractNotifier()
{
    setSocket(sock);
}

void AbstractNotifier::setSocket(QSharedPointer<QTcpSocket> sock){
    m_sock = sock;
    qDebug() << "setSocket called";
    connect(&(*m_sock),&QTcpSocket::readyRead,this,&AbstractNotifier::parse);
    setConnected(true);
//    emit connected();
}

void AbstractNotifier::parse(){
    qDebug() << "parse";
    QByteArray arr=m_sock->readAll();
    if(!arr.endsWith('\n'))
        m_buffer.append(arr);
    else{
        m_buffer.append(arr);
        auto list = m_buffer.split('\n');
        m_buffer.clear();
        for(auto i : list){
            qDebug() << "data received: " << i;
            qDebug() << "from " << m_sock;
            auto jsonDoc = QJsonDocument::fromJson(i);
            qDebug() << "jsonDoc: " << jsonDoc;
            auto jsonObj = jsonDoc.object();
            if(jsonObj[JSON_TYPE].toString() == TYPE_NOTIFY){
                emit newNotification(jsonObj[JSON_TARGET].toString(),jsonObj[JSON_DATA]);
            }
            else if(jsonObj[JSON_TYPE].toString() == TYPE_DB){
                emit newQuery(jsonObj[JSON_TARGET].toString(),jsonObj[JSON_DATA]);
            }
            else if(jsonObj[JSON_TYPE].toString() == TYPE_RAMI){
                emit newRami(jsonObj[JSON_DATA]);
            }
            else if(jsonObj[JSON_TYPE].toString() == TYPE_ERR){
                emit newError(jsonObj[JSON_DATA].toString());
            }
        }
    }
}

void AbstractNotifier::parseRami(QJsonValue value){
    QVariantMap result;
    QJsonObject obj = value.toObject();
    result[RAMI_COLUMN]=obj[RAMI_COLUMN].toInt();
    result[RAMI_ROW]=obj[RAMI_ROW].toInt();
    result[RAMI_STATE]=obj[RAMI_STATE].toBool();
    emit commandReceived(result);
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
    QWeakPointer<QTcpSocket> wp(m_sock);
    QSharedPointer<QTcpSocket> sock = wp.toStrongRef();
    if(sock){
        qDebug() << "sending " << result.toJson();
        qDebug() << "to " << sock;
        sock->write(result.toJson(QJsonDocument::Compact)+'\n');
    }
}

void AbstractNotifier::sendRami(int row, int column, bool state){
    qDebug() << "sendRami called";
    QJsonObject obj;
    obj.insert(RAMI_COLUMN,column);
    obj.insert(RAMI_ROW,row);
    obj.insert(RAMI_STATE,state);
    send(obj,AbstractNotifier::TYPE_RAMI,"");
}

void AbstractNotifier::sendRami(QVariantMap params){
    int row = params["row"].toInt();
    int column = params["column"].toInt();
    bool state = params["state"].toBool();
    sendRami(row,column,state);
}

void AbstractNotifier::disconnect(){
    qDebug() << "disconnecting notifier";
//    m_sock.clear();
//    QObject::disconnect(this,0,0,0);
    QWeakPointer<QTcpSocket> wp(m_sock);
    QSharedPointer<QTcpSocket> sock = wp.toStrongRef();
    if(sock)
        sock->abort();
    setConnected(false);
}
