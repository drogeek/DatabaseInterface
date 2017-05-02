#include "databaseaccess.h"
#ifdef __linux__
    const QString DataBaseAccess::DRIVER = "ODBC Driver 13 for SQL Server";
#elif _WIN32
    const QString DataBaseAccess::DRIVER = "SQL Server";
#endif

const QString DataBaseAccess::NAME = "CartridgeApplication";
//TODO: find the real ip
const QString DataBaseAccess::IP = "127.0.0.1";

DataBaseAccess::DataBaseAccess()
{
    m_db = QSqlDatabase::addDatabase("QODBC");
    setConnected(false);
    connect(this,&DataBaseAccess::configChanged,this,&DataBaseAccess::open);
}

DataBaseAccess::DataBaseAccess(QString ip, qint16 port, QString user, QString pass)
    :DataBaseAccess()
{
    setDatabase(ip,port,user,pass);
}

void DataBaseAccess::setDatabase(QString ip, qint16 port, QString user, QString pass){
    qDebug() <<"Driver={"+DRIVER+"};\
                Server="+ip+","+QString("%1").arg(port)+";\
                Uid="+user+";\
                Pwd="+pass+";";

    m_db.setDatabaseName(
                "Driver={"+DRIVER+"};\
                Server="+ip+","+QString("%1").arg(port)+";\
                Uid="+user+";\
                Pwd="+pass+";"
    );
    emit configChanged();
}

void DataBaseAccess::open(){
    if(m_db.open()){
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
            emit error(query.lastError().text());
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
                    emit error(insert.lastError().text());
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
                QObject::connect(&m_timer,&QTimer::timeout, updateModify);
                m_timer.start(5000);
            }
        }
        setConnected(true);
    }
    else{
        qDebug() << m_db.lastError();
        setConnected(false);
        emit error(m_db.lastError().text());
    }
}
