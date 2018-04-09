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
}

void fountainServer::newConnectionHandler()
{

    clientList.append(tcpServer->nextPendingConnection());
     dataToFountainDevice.setDevice(clientList.last());
     dataToFountainDevice.setVersion(QDataStream::Qt_5_8);

    connect(clientList.last(), SIGNAL(readyRead()),this,SLOT(readyReadFromUserHandler()));


}

void fountainServer::readyReadFromUserHandler()
{

    dataToFountainDevice.startTransaction();

    QByteArray nextFortune;
    dataToFountainDevice >> nextFortune;

#if fountainServerForwarder
    emit toFountainDevice(nextFortune);
#endif

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

    dataToUser.startTransaction();

    QByteArray nextFortune;
    dataToUser >> nextFortune;

    foreach (QTcpSocket* theClient, clientList) {
            QByteArray block;
            QDataStream out(&block, QIODevice::WriteOnly);
            out.setVersion(QDataStream::Qt_5_8);
            out << nextFortune;
            theClient->write(block);
    }
}
