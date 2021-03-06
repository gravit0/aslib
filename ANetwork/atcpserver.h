#ifndef ATCPSERVER_H
#define ATCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <acore.h>
#include <QMutex>
#include <QLinkedList>
#include <QTimer>
#include <functional>
enum ClientState
{
    UncorrenctState,
    NoAuthState,
    AuthState,
    waitCloseInServer,
    WaitCliseInClient
};

class ATCPServer;
class validClient :public QObject
{
    Q_OBJECT
public:
    virtual ~validClient()
    {
        socket->deleteLater();
    }
    validClient()
    {
        numUsingCommands = 0;
        state = UncorrenctState;
    }
    ATCPServer* servers;
    void DeleteMe();
    QLinkedList<validClient*>::iterator iterator,iterator2;

    QMutex mutex;
    QTcpSocket* socket;
    ClientState state;
    int numUsingCommands,ThreadID;
    bool isAuth;
};

struct ArrayCommand
{
    QByteArray command;
    QTcpSocket* client;
    bool operator ==(ArrayCommand h)
    {
        if(client==h.client) return true;
        else return false;
    }
};
class ATCPServer;
class ServerThread : public QThread
{
    Q_OBJECT
private:
    void run();
public:
    ServerThread(ATCPServer* serv);
    void UseCommand();
    QMutex arrayMutex;
    QLinkedList<ArrayCommand> ArrayCommands;
    QLinkedList<validClient*> ClientsListt;
    ATCPServer* server;
    bool isStaticThread,isSleep;
    int currentIdThread,currentCommandID;
signals:
    void sendToClient(validClient* client, QByteArray str);
    void CloseClient(validClient* clientID);
public slots:
    void NewCommand(int idThread);
};
class ATCPServer : public QObject
{
    Q_OBJECT
public:
    ATCPServer();
    ~ATCPServer();
    QLinkedList<validClient*> ClientsList;
    QMutex mutex;
    QList<ServerThread*> ThreadList;
    bool launch(QHostAddress host, int port);
    //bool launchMetaSever(QHostAddress host, int port);
    validClient *getClient(QTcpSocket* socket);
    QLinkedList<validClient*>::iterator GetIDClient(QTcpSocket* socket);
    virtual void UseCommand(QByteArray sCommand, validClient* nClient,ServerThread* thisThread)
    {
        qDebug() << sCommand+" from "+nClient->socket->peerAddress().toString()+":"+
                    QString::number(nClient->socket->peerPort())+" thread "+thisThread->currentIdThread;
    }
    QTcpServer* serverd;
    QList<QTcpServer*> addonServer;
    int MinThread,MaxThread;
    virtual validClient* NewValidClient()
    {
        return new validClient();
    }
signals:
    void signalCommand(int idThread);
    void signalClientConnected(validClient* user);
public slots:
    void clientConnected();
    void clientDisconnect();
    void clientReadyRead();
    void sendToClient(validClient* clientID, QByteArray str);
    void CloseClient(validClient* clientID);
};
#endif // ATCPSERVER_H
