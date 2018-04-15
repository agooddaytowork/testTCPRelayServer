#include <QCoreApplication>
#include <QtNetwork>
#include <QTcpSocket>
#include "fountainserver.h"
#include <QAbstractSocket>


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    fountainServer aServer;

    QTcpSocket *aFountainDevice = new QTcpSocket();

    bool fountainDeviceStatus = false;

    aFountainDevice->connectToHost("10.0.0.2",8080);

    QObject::connect(aFountainDevice,&QTcpSocket::disconnected,[&](){

        qDebug() << "Disconneceted";
        fountainDeviceStatus = false;

    });

    QObject::connect(aFountainDevice,&QTcpSocket::connected,[&](){
        qDebug() << "Connected";

        fountainDeviceStatus = true;
        qDebug() << fountainDeviceStatus;
    });

    QObject::connect(&aServer,&fountainServer::receivedNewConnectionFromUser,[&](){
        qDebug() << "receivedNewConnectionFromUser + before checking device";
        if(fountainDeviceStatus == false)
        {
            qDebug() << "receivedNewConnectionFromUser + device not open";
            aFountainDevice->connectToHost("10.0.0.2",8080);
//            QObject::connect(aFountainDevice,&QTcpSocket::readyRead,&aServer,&fountainServer::readyReadFromFountainDeviceHandler);

        }
    });


    QObject::connect(&aServer,&fountainServer::toFountainDevice,[&](const QByteArray &data){
        qDebug() << "toFountainDevice + before checking device";
        if(fountainDeviceStatus == false)
        {
            aFountainDevice->connectToHost("10.0.0.2",8080);

        }
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_8);
        out << data;
        aFountainDevice->write(block);
    });

//    QObject::connect(aFountainDevice,&QTcpSocket::readyRead,&aServer,&fountainServer::readyReadFromFountainDeviceHandler);
    QObject::connect(aFountainDevice,&QTcpSocket::readyRead,&aServer,&fountainServer::fromFountainDeviceHandler);

    return a.exec();
}
