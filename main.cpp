#include <QCoreApplication>
#include <QtNetwork>
#include <QTcpSocket>
#include "fountainserver.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    fountainServer aServer;

    QTcpSocket *aFountainDevice = new QTcpSocket();

    aFountainDevice->connectToHost("10.0.0.2",8080);

    QObject::connect(&aServer,&fountainServer::toFountainDevice,[=](const QByteArray &data){

        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_8);
        out << data;
        aFountainDevice->write(block);
    });

    QObject::connect(aFountainDevice,&QTcpSocket::readyRead,&aServer,&fountainServer::fromFountainDeviceHandler);
    return a.exec();
}
