// SteppirClient.h
#pragma once
#include <QObject>

struct SteppirStatus {
    double frequencyMHz = 0.0;
    quint8 activeMotors = 0;
    QString direction;
    bool trackFlag = false;
};
Q_DECLARE_METATYPE(SteppirStatus)

class SteppirClient : public QObject {
    Q_OBJECT
public:
    explicit SteppirClient(QObject *parent = nullptr) : QObject(parent) {}
    virtual void connectToDevice() = 0;
    virtual void disconnectFromDevice() = 0;
    virtual void sendCommand(const QByteArray &cmd) = 0;
    virtual bool isConnected() const = 0;
    virtual void close() = 0;

signals:
    void receivedStatus(const QString &line);
    void connectionStateChanged(bool connected);
    void receivedParsedStatus(const SteppirStatus &status);
    void connectionError(const QString &error);
};
