#ifndef DATA_SERVER_H
#define DATA_SERVER_H

#include <QTcpServer>
#include <QtNetwork>

namespace data
{

/**
 * \brief The Server class.
 *
 * Class that helps get access token
 */
class AuthServer : public QTcpServer
{
    Q_OBJECT
private:
    typedef QTcpServer BaseClass;

public:
    explicit AuthServer(QObject* parent = 0);
    ~AuthServer();

    ushort getPort() const { return m_port; }

    bool start();

signals:
    void authCodeReceived(const QString& accessToken);

private slots:
    void onNewConnection();

    void tcpReady();
    void tcpError(QAbstractSocket::SocketError);
    void tcpDisconnected();
    void tcpStateChanged(QAbstractSocket::SocketState state);

private:
    ushort getFreeOpenPort() const;

    QTcpSocket* m_activeSocket;
    ushort m_port;
    QString m_accessToken;
};

} // namespace data

#endif // DATA_SERVER_H
