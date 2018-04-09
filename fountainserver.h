#ifndef FOUNTAINSERVER_H
#define FOUNTAINSERVER_H

#include <QTcpServer>
#include <QNetworkSession>
#include <QDebug>
#include <QDataStream>
#include <QList>

#define fountainServerDebug (1)
#define fountainServerForwarder (1)
#define fountainDeviceMode (0)


class fountainServer: public QObject
{
    Q_OBJECT


    QTcpServer *tcpServer = nullptr;
    QTcpSocket *tcpSocket = nullptr;
    int m_Serverport;
    QDataStream in;

    QList<QTcpSocket*> clientList;

public:
    fountainServer(QObject *parent = nullptr);

public slots:

    void newConnectionHandler();
    void fromSerialHandler(const QByteArray &data);
    void fromFountainDeviceHandler();
    void readyReadFromFountainDeviceHandler();


private slots:
    void readyReadFromUserHandler();



signals:

    void toSerial(QByteArray);
    void toFountainDevice(QByteArray);

};

#endif // FOUNTAINSERVER_H
