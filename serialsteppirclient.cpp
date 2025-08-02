#include "serialsteppirclient.h"
#include <QDebug>

SerialSteppirClient::SerialSteppirClient(QString portName, int baud, QObject *parent)
    : SteppirClient(parent), portName(std::move(portName)), baud(baud)
{
    connect(&serial, &QSerialPort::readyRead, this, &SerialSteppirClient::onReadyRead);
    connect(&serial, &QSerialPort::errorOccurred, this, &SerialSteppirClient::onError);

    reconnectTimer.setInterval(3000);
    reconnectTimer.setSingleShot(false);
    connect(&reconnectTimer, &QTimer::timeout, this, &SerialSteppirClient::tryReconnect);
}

void SerialSteppirClient::connectToDevice() {
    if (serial.isOpen()) return;
    serial.setPortName(portName);
    serial.setBaudRate(baud);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);

    if (!serial.open(QIODevice::ReadWrite)) {
        qWarning() << "[Steppir][SERIAL] Open failed:" << serial.errorString();
        reconnectTimer.start();
    } else {
        qDebug() << "[Steppir][SERIAL] Connected on" << portName;
        emit connectionStateChanged(true);
    }
}

void SerialSteppirClient::disconnectFromDevice() {
    reconnectTimer.stop();
    if (serial.isOpen()) serial.close();
    emit connectionStateChanged(false);
}

void SerialSteppirClient::sendCommand(const QString &cmd) {
    if (!serial.isOpen()) return;
    QByteArray data = cmd.toUtf8();
    data.append('\r');
    serial.write(data);
    serial.flush();
}

void SerialSteppirClient::onReadyRead() {
    rxBuf += serial.readAll();
    int idx;
    while ((idx = rxBuf.indexOf('\n')) != -1 || (idx = rxBuf.indexOf('\r')) != -1) {
        QByteArray line = rxBuf.left(idx);
        rxBuf.remove(0, idx + 1);
        QString s = QString::fromUtf8(line).trimmed();
        if (!s.isEmpty()) emit receivedLine(s);
    }
}

void SerialSteppirClient::onError(QSerialPort::SerialPortError e) {
    if (e == QSerialPort::NoError) return;
    qWarning() << "[Steppir][SERIAL] Error:" << serial.errorString();
    emit connectionStateChanged(false);
    if (serial.isOpen()) serial.close();
    reconnectTimer.start();
}

void SerialSteppirClient::tryReconnect() {
    if (serial.isOpen()) {
        reconnectTimer.stop();
        return;
    }
    qDebug() << "[Steppir][SERIAL] Reconnecting...";
    connectToDevice();
}
