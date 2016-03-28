#include "AuthServer.h"

#include "Data/debugAsserts.h"

#include <QMessageBox>
#include <QNetworkAccessManager>

namespace data
{

AuthServer::AuthServer(QObject* parent) :
    BaseClass(parent),
    m_activeSocket(nullptr),
    m_port(0)
{
    VERIFY(connect(this, SIGNAL(newConnection()), this, SLOT(onNewConnection())));
}

AuthServer::~AuthServer()
{
    close();
}

bool AuthServer::start()
{
    m_port = getFreeOpenPort();
    return listen(QHostAddress::Any, m_port);
}

void AuthServer::onNewConnection()
{
    QTcpSocket* socket = nextPendingConnection();
    qDebug() << "AuthServer::onNewConnection" << socket;
    if (!socket)
    {
        return;
    }

    if (m_activeSocket)
    {
        socket->close();
        return;
    }

    m_activeSocket = socket;
    m_activeSocket->setSocketOption(QAbstractSocket::KeepAliveOption, true);

    VERIFY(connect(m_activeSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(tcpError(QAbstractSocket::SocketError))));
    VERIFY(connect(m_activeSocket, SIGNAL(readyRead()), this, SLOT(tcpReady())));
    VERIFY(connect(m_activeSocket, SIGNAL(disconnected()), this, SLOT(tcpDisconnected())));
    VERIFY(connect(m_activeSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(tcpStateChanged(QAbstractSocket::SocketState))));
}

void AuthServer::tcpDisconnected()
{
    qDebug() << "AuthServer::tcpDisconnected" << sender();
    m_activeSocket->deleteLater();
    m_activeSocket = nullptr;
}

void AuthServer::tcpStateChanged(QAbstractSocket::SocketState state)
{
    qDebug() << "AuthServer::tcpStateChanged" << sender() << state;
}

void AuthServer::tcpReady()
{
    qDebug() << "AuthServer::tcpReady" << sender();
    QByteArray byteArray = m_activeSocket->read(m_activeSocket->bytesAvailable());
    QString textData(byteArray);
    qDebug() << textData;
    const QString CODE_SUB_STRING("?code=");
    int pos = textData.indexOf(CODE_SUB_STRING);
    if (pos >= 0) {
        pos += CODE_SUB_STRING.length();
        int end = textData.indexOf(" ", pos);
        if (end >= 0) {
            QString authCode = textData.mid(pos, end - pos);
            emit authCodeReceived(authCode);
            return;
        }
    }

    emit authFailureReceived();
}

void AuthServer::tcpError(QAbstractSocket::SocketError error)
{
    qDebug() << "AuthServer::tcpError" << sender() << error;
    emit authFailureReceived();
}

ushort AuthServer::getFreeOpenPort() const
{
    ushort openPort = 0;
    QTcpServer server;
    for (ushort i = 1024; i < std::numeric_limits<ushort>::max(); ++i)
    {
        if (server.listen(QHostAddress::LocalHost, i))
        {
            openPort = i;
            break;
        }
    }
    ASSERT(openPort, "Can not find open port");
    return openPort;
}

} // namespace data
