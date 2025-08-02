#pragma once
#include "steppirclient.h"
#include <QSerialPort>
#include <QTimer>

class SerialSteppirClient : public SteppirClient {
    Q_OBJECT
public:
    SerialSteppirClient(QString portName, int baud = 9600, QObject *parent = nullptr);

    void connectToDevice() override;
    void disconnectFromDevice() override;
    void sendCommand(const QString &cmd) override;
    bool isConnected() const override { return serial.isOpen(); }

private slots:
    void onReadyRead();
    void onError(QSerialPort::SerialPortError e);
    void tryReconnect();

private:
    QSerialPort serial;
    QTimer      reconnectTimer;
    QString     portName;
    int         baud;
    QByteArray  rxBuf;
};
