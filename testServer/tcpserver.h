#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "head.h"

extern QList<QTcpSocket*> tcpSocketConnetList;

class TcpServer : public QTcpServer
{

    Q_OBJECT
public:
    TcpServer();
    int run();
    // user
    bool getLoginResult(QString, QString);  // 获取登录状态
    bool getRegResult(QString, QString);  // 获取注册状态
    QString getSheet(int seq);  // 获取详单信息
    QString getAllSheet(); // 获取全部详单及其主要信息
    int getQueueNum(int); // 获取排队号
    int getPreCarNum(int); // 获取前车等待数
    // 管理员调用的函数
    bool getOpenResult(bool, QString, int); //启动/关闭充电桩
    QString getWaitArea();

protected:
    //函数重载
    void incomingConnection(qintptr socketDescriptor);

private:
    //QList<QTcpSocket*> tcpSocketConnetList;

protected slots:
    void Socket_Data_Processing(QString SendData,int descriptor);            //处理数据
    void Socket_Disconnected(int descriptor);                                //断开连接处理
signals:
    void WriteDataToClient(QString);
};

class TcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    TcpSocket();
    void socketRun();

private:
    int Descriptor;

protected slots:
    void ReceiveData_slot();            //处理readyRead信号读取数据
    void SendData_slot(QString);        //向client发
    void ClientDisconnected_slot();     //客户端断开连接触发disconnected信号，这个槽函数用来处理这个信号

signals:
    void GetDataFromClient(QString ,int);   //告诉server有数据处理
    void ClientDisConnected(int);           //告诉server有客户端断开连接

    void UpdateUserDB();     //server更新User表
    //server更新

};

#endif // TCPSERVER_H
