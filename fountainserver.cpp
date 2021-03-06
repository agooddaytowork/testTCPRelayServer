#include "fountainserver.h"
#include <QTextCodec>
#include <QTcpSocket>

#include <QJsonDocument>
#include <QJsonObject>

fountainServer::fountainServer(QObject *parent): QObject(parent), tcpSocket(new QTcpSocket(this))
{

    tcpServer = new QTcpServer(this);

    if(tcpServer->listen(QHostAddress(QHostAddress::Any),8080))
    {
        qDebug() << "DZOO";
        connect(tcpServer,SIGNAL(newConnection()),this,SLOT(newConnectionHandler()));
    }

    connect(this,SIGNAL(stillAvailableDataFromUser()),this,SLOT(readyReadFromUserHandler()));
    connect(this,SIGNAL(requestSendDataToClient(QByteArray)),this,SLOT(sendDataToClient(QByteArray)));
}

void fountainServer::newConnectionHandler()
{

    clientList.append(tcpServer->nextPendingConnection());
    connect(clientList.last(), SIGNAL(readyRead()),this,SLOT(readyReadFromUserHandler()));
    connect(clientList.last(),SIGNAL(disconnected()),this,SLOT(clientDisconnectedHander()));

    dataToFountainDevice.setDevice(clientList.last());
    dataToFountainDevice.setVersion(QDataStream::Qt_5_8);

    emit receivedNewConnectionFromUser();
}

void fountainServer::readyReadFromUserHandler()
{

    qDebug() << "Enter readyReadFromUserHandler";
    QTcpSocket* readSocket = dynamic_cast<QTcpSocket*>(sender());
    if(readSocket)
    {
        dataToFountainDevice.setDevice(readSocket);

    }


    dataToFountainDevice.startTransaction();

    QByteArray out;


    if(!dataToFountainDevice.commitTransaction()) return;
    dataToFountainDevice >> out;



#if fountainServerForwarder
    emit toFountainDevice(out);
#endif

    if(clientList.last()->bytesAvailable() > 0) emit stillAvailableDataFromUser();
}

void fountainServer::clientDisconnectedHander()
{
    if(auto client = dynamic_cast<QTcpSocket *>(sender()))
    {
        clientList.removeAll(client);
    }
}


void fountainServer::fromSerialHandler(const QByteArray &data)
{


    foreach (QTcpSocket* theClient, clientList) {
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_8);
        out << data;
        theClient->write(block);
    }
}

void fountainServer::fromFountainDeviceHandler()
{

        QTcpSocket* readSocket = qobject_cast<QTcpSocket*>(sender());
        dataToUser.setDevice(readSocket);
        dataToUser.setVersion(QDataStream::Qt_5_8);

        connect(readSocket, SIGNAL(readyRead()),this,SLOT(readyReadFromFountainDeviceHandler()));

}
void fountainServer::readyReadFromFountainDeviceHandler()
{


    QTcpSocket* readSocket = dynamic_cast<QTcpSocket*>(sender());

    if(readSocket)
    {
        dataToUser.setDevice(readSocket);
        dataToUser.setVersion(QDataStream::Qt_5_8);
    }
    dataToUser.startTransaction();

    QByteArray nextFortune;
    if(!dataToUser.commitTransaction()) return;
    dataToUser >> nextFortune;

    emit requestSendDataToClient(nextFortune);


    if(readSocket && readSocket->bytesAvailable() >0)
    {
        readyReadFromFountainDeviceHandler();
    }
}

void fountainServer::sendDataToClient(const QByteArray &data)
{

    qDebug() << "client List lenght: " + QString::number(clientList.length());
    foreach (QTcpSocket* theClient, clientList) {
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_8);
        out << data;
        theClient->write(block);
    }
}
