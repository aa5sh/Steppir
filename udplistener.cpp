#include "UDPListener.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

UDPListener::UDPListener(QObject *parent)
    : QObject(parent)
{
    connect(&udpSocket, &QUdpSocket::readyRead, this, &UDPListener::onReadyRead);
}

void UDPListener::startListening(quint16 port)
{
    if (!udpSocket.bind(QHostAddress::AnyIPv4, port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
        qWarning() << "Failed to bind UDP port:" << port;
    } else {
        qDebug() << "Listening for UDP on port" << port;
    }
}

void UDPListener::onReadyRead()
{
    while (udpSocket.hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket.pendingDatagramSize());
        udpSocket.readDatagram(datagram.data(), datagram.size());

        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(datagram, &err);
        if (err.error != QJsonParseError::NoError) {
            qWarning() << "JSON parse error:" << err.errorString();
            continue;
        }

        if (!doc.isObject()) {
            qWarning() << "Invalid JSON root.";
            continue;
        }

        QJsonObject root = doc.object();
        QString appid = root.value("appid").toString();
        QString msgtype = root.value("msgtype").toString();

        if (appid == "QLog" && msgtype == "rigstatus") {
            QJsonObject data = root.value("data").toObject();
            parseRigStatus(data);
            emit rigStatusReceived(data); // Optional original signal
        }
    }
}

void UDPListener::parseRigStatus(const QJsonObject &data)
{
    QString profile = data.value("profile").toString();
    QJsonArray vfoStates = data.value("vfostates").toArray();

    if (!vfoStates.isEmpty()) {
        QJsonObject vfo = vfoStates.first().toObject();
        QString freqStr = vfo.value("freq").toString();
        bool ptt = vfo.value("ptt").toBool();

        double frequency = freqStr.toDouble();

       // qDebug() << "Profile:" << profile;
       // qDebug() << "Frequency:" << frequency << "MHz";
       // qDebug() << "PTT:" << (ptt ? "ON" : "OFF");

        emit rigStatusParsed(profile, frequency, ptt); // optional signal
    } else {
        qWarning() << "No VFO state available in rigstatus.";
    }
}
