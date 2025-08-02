// TcpSteppirClient.h
#pragma once
#include "SteppirClient.h"
#include <QTcpSocket>
#include <QTimer>

class TcpSteppirClient : public SteppirClient {
    Q_OBJECT

public:
    TcpSteppirClient(const QString &host, quint16 port, QObject *parent = nullptr);
    ~TcpSteppirClient();
    void connectToDevice() override;
    void disconnectFromDevice() override;
    void sendCommand(const QByteArray &cmd) override;
    bool isConnected() const override;
    void close() override;

signals:
    void connectionError(const QString &message);

private slots:
    void onReadyRead();
    void onDisconnected();
    void tryReconnect();
    void onConnected();
    void onSocketError(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket socket;
    QTimer reconnectTimer;
    QTimer pollTimer;
    QString host;
    quint16 port;
    QByteArray buffer;
    void sendStatusRequest();
};
