// TcpSteppirClient.cpp
#include "TcpSteppirClient.h"
#include <QDebug>

TcpSteppirClient::TcpSteppirClient(const QString &host, quint16 port, QObject *parent)
    : SteppirClient(parent), host(host), port(port)
{
    connect(&socket, &QTcpSocket::readyRead, this, &TcpSteppirClient::onReadyRead);
    connect(&socket, &QTcpSocket::disconnected, this, &TcpSteppirClient::onDisconnected);
    connect(&socket, &QTcpSocket::connected, this, &TcpSteppirClient::onConnected);
    connect(&socket, &QTcpSocket::errorOccurred, this, &TcpSteppirClient::onSocketError);

    reconnectTimer.setInterval(3000);
    reconnectTimer.setSingleShot(false);
    pollTimer.setInterval(1500);
    pollTimer.setSingleShot(false);
    connect(&reconnectTimer, &QTimer::timeout, this, &TcpSteppirClient::tryReconnect);
    connect(&pollTimer, &QTimer::timeout, this, &TcpSteppirClient::sendStatusRequest);
}

void TcpSteppirClient::onSocketError(QAbstractSocket::SocketError socketError) {
    Q_UNUSED(socketError);  // optional if you only use `errorString()`
    qWarning() << "TcpSteppirClient socket error:" << socket.errorString();
    reconnectTimer.start();
    QString errMsg = socket.errorString();
    emit connectionError(errMsg);
}


void TcpSteppirClient::connectToDevice()
{
    qDebug() << "Connecting to Steppir at" << host << ":" << port;
    socket.connectToHost(host, port);
}

void TcpSteppirClient::disconnectFromDevice()
{
    qDebug() << "Disconnect";
    reconnectTimer.stop();
    socket.disconnectFromHost();
}

void TcpSteppirClient::sendCommand(const QByteArray &cmd)
{
    if (socket.state() == QAbstractSocket::ConnectedState) {
        socket.write(cmd);
    }
}

bool TcpSteppirClient::isConnected() const
{
    return socket.state() == QAbstractSocket::ConnectedState;
}

void TcpSteppirClient::onReadyRead() {
    buffer.append(socket.readAll());

    int endIdx;
    while ((endIdx = buffer.indexOf('\r')) != -1) {
        QByteArray frame = buffer.left(endIdx);
        buffer.remove(0, endIdx + 1); // remove processed frame and '\r'

        emit receivedStatus(QString::fromUtf8(frame));

        if (frame.length() == 10 && frame[0] == '@' && frame[1] == 'A') {
            SteppirStatus status;

            // Parse frequency (bytes 3-5)
            quint32 freq10 = (quint8)frame[3] << 16 |
                             (quint8)frame[4] << 8 |
                             (quint8)frame[5];
            status.frequencyMHz = freq10 / 100000.0;

            // Active motor flags (byte 6)
            status.activeMotors = (quint8)frame[6];

            quint8 byte = static_cast<quint8>(frame[7]);
            // Direction (byte 7)
            quint8 dir = (byte & 0xe0) >> 5;//(quint8)frame[7];
            if (dir == 2) //(dir & 0x02)
                status.direction = "180";
            else if (dir == 4) //(dir & 0x01)
                status.direction = "Bi-directional";
            else if (dir == 3) //(dir & 0x04)
                status.direction = "3/4 Wave";
            else
                status.direction = "Normal";

            quint8 track = byte >> 2;

            status.trackFlag = track;
           // qDebug() << frame;
            emit receivedParsedStatus(status);
        }
    }
}

void TcpSteppirClient::onDisconnected()
{
    emit connectionStateChanged(false);
    reconnectTimer.start();
    pollTimer.stop();
}

void TcpSteppirClient::tryReconnect()
{
    qDebug() << "Trying to reconnect...";
    if (socket.state() != QAbstractSocket::ConnectedState) {
        qDebug() << "Trying to reconnect...";
        socket.abort();
        socket.connectToHost(host, port);
    }
}

void TcpSteppirClient::onConnected() {
  //  emit connectionChanged(true);
    qDebug() << "Connected to Steppir at" << host << ":" << port;
    pollTimer.start();  // 👈 Start polling
    reconnectTimer.stop();
}

void TcpSteppirClient::sendStatusRequest() {
    if (socket.isOpen()) {
        QByteArray cmd;
        cmd.append(0x3F);
        cmd.append(0x41);
        cmd.append(0x0D);  // CR
        socket.write(cmd);
        socket.flush();
    }
}

void TcpSteppirClient::close() {
    if (socket.isValid()) {
        if (socket.isOpen()) {
            socket.disconnectFromHost();
            if (socket.state() != QAbstractSocket::UnconnectedState) {
                socket.waitForDisconnected(1000); // wait max 1s
            }
        }
        //socket.deleteLater();
    }
}

TcpSteppirClient::~TcpSteppirClient() {
    close();
}


