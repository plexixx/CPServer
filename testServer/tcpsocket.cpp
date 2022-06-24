#include "tcpserver.h"
QList<QTcpSocket*> tcpSocketConnetList;

TcpSocket::TcpSocket()
{

}

void TcpSocket::socketRun()
{
    Descriptor = this->socketDescriptor();
    connect(this,SIGNAL(readyRead()),this,SLOT(ReceiveData_slot()));
    connect(this,SIGNAL(disconnected()),this,SLOT(ClientDisconnected_slot()));
}

//TcpSocket数据接收触发
void TcpSocket::ReceiveData_slot()
{
    qDebug() << "get data from client and read once";
    QByteArray buffer;
    buffer = readAll();
    qDebug() << "receive data: " << buffer;
    if(!buffer.isEmpty())
    {
        QString data =  QString::fromLocal8Bit(buffer);
        //数据处理
        emit GetDataFromClient(data, Descriptor);
    }
}

void TcpSocket::SendData_slot(QString buf)
{
    qDebug() << "send data: " << buf;
    if(!buf.isEmpty())
    {
        QByteArray buffer = buf.toUtf8();
        write(buffer);
    }
}

//TcpSocket断开链接触发
void TcpSocket::ClientDisconnected_slot()
{
    emit ClientDisConnected(Descriptor);
}
