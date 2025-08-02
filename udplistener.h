#ifndef UDPLISTENER_H
#define UDPLISTENER_H

#include <QObject>
#include <QUdpSocket>
#include <QJsonObject>

class UDPListener : public QObject
{
    Q_OBJECT
public:
    explicit UDPListener(QObject *parent = nullptr);
    void startListening(quint16 port = 12050);

signals:
    void rigStatusReceived(const QJsonObject &data);
    void rigStatusParsed(const QString &profile, double frequency, bool ptt);

private slots:
    void onReadyRead();

private:
    QUdpSocket udpSocket;
    void parseRigStatus(const QJsonObject &data);

};

#endif // UDPLISTENER_H
