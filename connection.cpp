#include "connection.h"

Connection::Connection(){
    setConnected(false);
}

//TODO: verify errors?
void Connection::send(int row, int column, bool state){
    RamiProtocol::Params params;
    params.column=column;
    params.row=row;
    params.state=state;
    auto toSend = RamiProtocol::encrypt(params);
    m_sock->write((char *)&toSend);
    qDebug() << "Command sent";
}

void Connection::receive(){
    QVariantMap result;
    QByteArray str = m_sock->readAll();
    for(auto it = str.begin(); it != str.end(); it+=3){
        std::string subStr;
        subStr+=*it;
        subStr+=*(it+1);
        subStr+=*(it+2);
        auto tmp = RamiProtocol::decrypt(subStr);
        result["column"]=tmp.column;
        result["row"]=tmp.row;
        result["state"]=tmp.state;
        qDebug() << "Command received from WinMedia";
        emit commandReceived(result);
    }
}

void Connection::setSocket(QSharedPointer<QTcpSocket> socket){
    m_sock = socket;
    setConnected(true);
}

void Connection::disconnect(){
    QWeakPointer<QTcpSocket> wp(m_sock);
    QSharedPointer<QTcpSocket> sp = wp.toStrongRef();
    if(sp){
        sp->abort();
//        m_sock.clear();
        setConnected(false);
    }
}
