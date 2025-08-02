#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStyle>
#include <QPalette>
#include <tcpsteppirclient.h>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    udpListener(new UDPListener(this))
{
    ui->setupUi(this);

    connect(udpListener, &UDPListener::rigStatusParsed, this, &MainWindow::handleRigStatus);
    udpListener->startListening(12050);

    client = new TcpSteppirClient("192.168.2.136",4002,this);
    //client = new TcpSteppirClient("192.168.4.120",4001,this);

    connect(&lastUpdateCheckTimer, &QTimer::timeout, this, &MainWindow::checkSteppirUpdateTimeout);
    lastUpdateCheckTimer.start(1000); // check every second

    connect(client, &SteppirClient::receivedParsedStatus, this, &MainWindow::UpdateSteppirStatus);
    client->connectToDevice();
}

MainWindow::~MainWindow()
{
    if(client) {
        client->close();
        delete client;
        client = nullptr;
    }
    delete ui;
}


void MainWindow::SetButtonSelected(QPushButton *button, bool selected)
{
    if (!button)
        return;

    if (selected) {
        QPalette pal = button->palette();
        pal.setColor(QPalette::Button, QColor("#28a745")); // Green (Bootstrap-style)
        pal.setColor(QPalette::ButtonText, Qt::white);
        button->setAutoFillBackground(true);
        button->setPalette(pal);
        button->update();
    } else {
        button->setPalette(button->style()->standardPalette());
        button->setAutoFillBackground(false);
        button->update();
    }
}

void MainWindow::handleRigStatus(const QString &profile, double freq, bool ptt)
{
    lastQlogUpdate = QDateTime::currentDateTime();
    ui->statusQLog->setStyleSheet("QLabel { background-color: green; color: white; }");
//    qDebug() << "MainWindow updated rig status:";
//    qDebug() << "Profile:" << profile << "| Freq:" << freq << "| PTT:" << ptt;
    SetFrequency(freq);
}

void MainWindow::RequestSteppirStatus() {
    if (client && client->isConnected())
        client->sendCommand("?A");
}

void MainWindow::UpdateSteppirStatus(SteppirStatus status) {
    lastSteppirUpdate = QDateTime::currentDateTime();

    lastSteppirStatus = status;

    ui->statusSteppir->setStyleSheet("QLabel { background-color: green; color: white; }");

    ui->SteppirFreq->setText(QString::number(status.frequencyMHz, 'f', 3));
    if (status.activeMotors != 0)
    {
        ui->statusTuning->setStyleSheet("QLabel { background-color: red; color: white; }");
    }
    else
    {
        ui->statusTuning->setStyleSheet("QLabel { background-color: green; color: white; }");
    }

    if (status.direction == "Normal")
    {
        SetButtonSelected(ui->pNormal,true);
        SetButtonSelected(ui->p180,false);
        SetButtonSelected(ui->p34,false);
        SetButtonSelected(ui->pBiDir,false);
    }
    else if (status.direction == "180")
    {
        SetButtonSelected(ui->pNormal,false);
        SetButtonSelected(ui->p180,true);
        SetButtonSelected(ui->p34,false);
        SetButtonSelected(ui->pBiDir,false);
    }
    else if (status.direction == "Bi-directional")
    {
        SetButtonSelected(ui->pNormal,false);
        SetButtonSelected(ui->p180,false);
        SetButtonSelected(ui->p34,false);
        SetButtonSelected(ui->pBiDir,true);
    }
    else if (status.direction == "3/4 Wave")
    {
        SetButtonSelected(ui->pNormal,false);
        SetButtonSelected(ui->p180,false);
        SetButtonSelected(ui->p34,true);
        SetButtonSelected(ui->pBiDir,false);
    }

    if (status.frequencyMHz >= 3 and status.frequencyMHz <= 4)
    {
        SetButtonSelected(ui->p80m,true);
        SetButtonSelected(ui->p60m,false);
        SetButtonSelected(ui->p40m,false);
        SetButtonSelected(ui->p30m,false);
        SetButtonSelected(ui->p20m,false);
        SetButtonSelected(ui->p17m,false);
        SetButtonSelected(ui->p15m,false);
        SetButtonSelected(ui->p12m,false);
        SetButtonSelected(ui->p10m,false);
        SetButtonSelected(ui->p6m,false);
    }
    else if (status.frequencyMHz >= 5 and status.frequencyMHz <= 6)
    {
        SetButtonSelected(ui->p80m,false);
        SetButtonSelected(ui->p60m,true);
        SetButtonSelected(ui->p40m,false);
        SetButtonSelected(ui->p30m,false);
        SetButtonSelected(ui->p20m,false);
        SetButtonSelected(ui->p17m,false);
        SetButtonSelected(ui->p15m,false);
        SetButtonSelected(ui->p12m,false);
        SetButtonSelected(ui->p10m,false);
        SetButtonSelected(ui->p6m,false);
    }
    else if (status.frequencyMHz >= 7 and status.frequencyMHz <= 8)
    {
        SetButtonSelected(ui->p80m,false);
        SetButtonSelected(ui->p60m,false);
        SetButtonSelected(ui->p40m,true);
        SetButtonSelected(ui->p30m,false);
        SetButtonSelected(ui->p20m,false);
        SetButtonSelected(ui->p17m,false);
        SetButtonSelected(ui->p15m,false);
        SetButtonSelected(ui->p12m,false);
        SetButtonSelected(ui->p10m,false);
        SetButtonSelected(ui->p6m,false);
    }
    else if (status.frequencyMHz >= 10 and status.frequencyMHz <= 11)
    {
        SetButtonSelected(ui->p80m,false);
        SetButtonSelected(ui->p60m,false);
        SetButtonSelected(ui->p40m,false);
        SetButtonSelected(ui->p30m,true);
        SetButtonSelected(ui->p20m,false);
        SetButtonSelected(ui->p17m,false);
        SetButtonSelected(ui->p15m,false);
        SetButtonSelected(ui->p12m,false);
        SetButtonSelected(ui->p10m,false);
        SetButtonSelected(ui->p6m,false);
    }
    else if (status.frequencyMHz >= 14 and status.frequencyMHz <= 15)
    {
        SetButtonSelected(ui->p80m,false);
        SetButtonSelected(ui->p60m,false);
        SetButtonSelected(ui->p40m,false);
        SetButtonSelected(ui->p30m,false);
        SetButtonSelected(ui->p20m,true);
        SetButtonSelected(ui->p17m,false);
        SetButtonSelected(ui->p15m,false);
        SetButtonSelected(ui->p12m,false);
        SetButtonSelected(ui->p10m,false);
        SetButtonSelected(ui->p6m,false);
    }
    else if (status.frequencyMHz >= 18 and status.frequencyMHz <= 19)
    {
        SetButtonSelected(ui->p80m,false);
        SetButtonSelected(ui->p60m,false);
        SetButtonSelected(ui->p40m,false);
        SetButtonSelected(ui->p30m,false);
        SetButtonSelected(ui->p20m,false);
        SetButtonSelected(ui->p17m,true);
        SetButtonSelected(ui->p15m,false);
        SetButtonSelected(ui->p12m,false);
        SetButtonSelected(ui->p10m,false);
        SetButtonSelected(ui->p6m,false);
    }
    else if (status.frequencyMHz >= 21 and status.frequencyMHz <= 22)
    {
        SetButtonSelected(ui->p80m,false);
        SetButtonSelected(ui->p60m,false);
        SetButtonSelected(ui->p40m,false);
        SetButtonSelected(ui->p30m,false);
        SetButtonSelected(ui->p20m,false);
        SetButtonSelected(ui->p17m,false);
        SetButtonSelected(ui->p15m,true);
        SetButtonSelected(ui->p12m,false);
        SetButtonSelected(ui->p10m,false);
        SetButtonSelected(ui->p6m,false);
    }
    else if (status.frequencyMHz >= 24 and status.frequencyMHz <= 25)
    {
        SetButtonSelected(ui->p80m,false);
        SetButtonSelected(ui->p60m,false);
        SetButtonSelected(ui->p40m,false);
        SetButtonSelected(ui->p30m,false);
        SetButtonSelected(ui->p20m,false);
        SetButtonSelected(ui->p17m,false);
        SetButtonSelected(ui->p15m,false);
        SetButtonSelected(ui->p12m,true);
        SetButtonSelected(ui->p10m,false);
        SetButtonSelected(ui->p6m,false);
    }
    else if (status.frequencyMHz >= 28 and status.frequencyMHz <= 30)
    {
        SetButtonSelected(ui->p80m,false);
        SetButtonSelected(ui->p60m,false);
        SetButtonSelected(ui->p40m,false);
        SetButtonSelected(ui->p30m,false);
        SetButtonSelected(ui->p20m,false);
        SetButtonSelected(ui->p17m,false);
        SetButtonSelected(ui->p15m,false);
        SetButtonSelected(ui->p12m,false);
        SetButtonSelected(ui->p10m,true);
        SetButtonSelected(ui->p6m,false);
    }
    else if (status.frequencyMHz >= 50 and status.frequencyMHz <= 60)
    {
        SetButtonSelected(ui->p80m,false);
        SetButtonSelected(ui->p60m,false);
        SetButtonSelected(ui->p40m,false);
        SetButtonSelected(ui->p30m,false);
        SetButtonSelected(ui->p20m,false);
        SetButtonSelected(ui->p17m,false);
        SetButtonSelected(ui->p15m,false);
        SetButtonSelected(ui->p12m,false);
        SetButtonSelected(ui->p10m,false);
        SetButtonSelected(ui->p6m,true);
    }

    SetButtonSelected(ui->pAuto,status.trackFlag);
}

void MainWindow::checkSteppirUpdateTimeout()
{
    if (lastSteppirUpdate.isValid()) {
        if (lastSteppirUpdate.msecsTo(QDateTime::currentDateTime()) > 30000) {
            ui->statusSteppir->setStyleSheet("QLabel { background-color: red; color: white; }");
        }
    }

    if (lastQlogUpdate.isValid()) {
        if (lastQlogUpdate.msecsTo(QDateTime::currentDateTime()) > 30000) {
            ui->statusQLog->setStyleSheet("QLabel { background-color: red; color: white; }");
        }
    }
}

void MainWindow::on_pAuto_clicked()
{
    QByteArray cmd;

    // Prefix: @A (0x40 0x41), zero placeholder (0x00)
    cmd.append(char(0x40));  // '@'
    cmd.append(char(0x41));  // 'A'
    cmd.append(char(0x00));  // always 0

    // Frequency × 100, as 3-byte big-endian
    quint32 freqInt = static_cast<quint32>(lastSteppirStatus.frequencyMHz * 100000); // freq × 1000 × 100
    cmd.append(char((freqInt >> 16) & 0xFF));
    cmd.append(char((freqInt >> 8) & 0xFF));
    cmd.append(char(freqInt & 0xFF));

    cmd.append(char(0x00));  // ac field (ignored)

    // Direction
    quint8 dirByte = 0x00;
    if (lastSteppirStatus.direction == "Bi-directional") dirByte = 0x80;
    else if (lastSteppirStatus.direction == "180")       dirByte = 0x40;
    else if (lastSteppirStatus.direction == "3/4 Wave")   dirByte = 0x20;

    // else remains 0x00 for Normal
    cmd.append(char(dirByte));

    // Track flag: 0x55 = auto track on, 0x52 = home/reset
    cmd.append(lastSteppirStatus.trackFlag ? char(0x55) : char(0x52));

    cmd.append(char(0x00));  // filler
    cmd.append(char(0x0D));  // carriage return

    client->sendCommand(cmd);
}


void MainWindow::on_pCalibrate_clicked()
{
    QByteArray cmd;

    // Prefix: @A (0x40 0x41), zero placeholder (0x00)
    cmd.append(char(0x40));  // '@'
    cmd.append(char(0x41));  // 'A'
    cmd.append(char(0x00));  // always 0

    // Frequency × 100, as 3-byte big-endian
    quint32 freqInt = static_cast<quint32>(lastSteppirStatus.frequencyMHz * 100000); // freq × 1000 × 100
    cmd.append(char((freqInt >> 16) & 0xFF));
    cmd.append(char((freqInt >> 8) & 0xFF));
    cmd.append(char(freqInt & 0xFF));

    cmd.append(char(0x00));  // ac field (ignored)

    // Direction
    quint8 dirByte = 0x00;
    if (lastSteppirStatus.direction == "Bi-directional") dirByte = 0x80;
    else if (lastSteppirStatus.direction == "180")       dirByte = 0x40;
    else if (lastSteppirStatus.direction == "3/4 Wave")   dirByte = 0x20;

    // else remains 0x00 for Normal
    cmd.append(char(dirByte));

    // Track flag: 0x55 = auto track on, 0x52 = home/reset
    cmd.append(char(0x56));

    cmd.append(char(0x00));  // filler
    cmd.append(char(0x0D));  // carriage return

    client->sendCommand(cmd);
}


void MainWindow::on_pHome_clicked()
{
    QByteArray cmd;

    // Prefix: @A (0x40 0x41), zero placeholder (0x00)
    cmd.append(char(0x40));  // '@'
    cmd.append(char(0x41));  // 'A'
    cmd.append(char(0x00));  // always 0

    // Frequency × 100, as 3-byte big-endian
    quint32 freqInt = static_cast<quint32>(lastSteppirStatus.frequencyMHz * 100000); // freq × 1000 × 100
    cmd.append(char((freqInt >> 16) & 0xFF));
    cmd.append(char((freqInt >> 8) & 0xFF));
    cmd.append(char(freqInt & 0xFF));

    cmd.append(char(0x00));  // ac field (ignored)

    // Direction
    quint8 dirByte = 0x00;
    if (lastSteppirStatus.direction == "Bi-directional") dirByte = 0x80;
    else if (lastSteppirStatus.direction == "180")       dirByte = 0x40;
    else if (lastSteppirStatus.direction == "3/4 Wave")   dirByte = 0x20;
    // else remains 0x00 for Normal
    cmd.append(char(dirByte));

    // Track flag: 0x55 = auto track on, 0x52 = home/reset
    cmd.append(char(0x53));

    cmd.append(char(0x00));  // filler
    cmd.append(char(0x0D));  // carriage return

    client->sendCommand(cmd);
}


void MainWindow::on_pNormal_clicked()
{
    SetDirection("Normal");
}


void MainWindow::on_p180_clicked()
{
    SetDirection("180");
}


void MainWindow::on_pBiDir_clicked()
{
    SetDirection("Bi-directional");
}


void MainWindow::on_p34_clicked()
{
    SetDirection("3/4 Wave");
}


void MainWindow::on_pFastDown_clicked()
{
    SetFrequency(lastSteppirStatus.frequencyMHz - 0.05);
}

void MainWindow::SetDirection(QString dir)
{
    QByteArray cmd;

    // Prefix: @A (0x40 0x41), zero placeholder (0x00)
    cmd.append(char(0x40));  // '@'
    cmd.append(char(0x41));  // 'A'
    cmd.append(char(0x00));  // always 0

    // Frequency × 100, as 3-byte big-endian
    quint32 freqInt = static_cast<quint32>(lastSteppirStatus.frequencyMHz * 100000); // freq × 1000 × 100
    cmd.append(char((freqInt >> 16) & 0xFF));
    cmd.append(char((freqInt >> 8) & 0xFF));
    cmd.append(char(freqInt & 0xFF));

    cmd.append(char(0x00));  // ac field (ignored)

    quint8 dirByte = 0x00;
    if (dir == "Bi-directional") dirByte = 0x80;
    else if (dir == "180")       dirByte = 0x40;
    else if (dir == "3/4 Wave")   dirByte = 0x20;

    cmd.append(char(dirByte));

    cmd.append(char(0x00));

    cmd.append(char(0x00));  // filler
    cmd.append(char(0x0D));  // carriage return

    client->sendCommand(cmd);
}

void MainWindow::SetFrequency(double freq)
{
    if (freq < 3 || freq > 56)
    {
        return;
    }
    QByteArray cmd;

    // Prefix: @A (0x40 0x41), zero placeholder (0x00)
    cmd.append(char(0x40));  // '@'
    cmd.append(char(0x41));  // 'A'
    cmd.append(char(0x00));  // always 0

    // Frequency × 100, as 3-byte big-endian
    quint32 freqInt = static_cast<quint32>(freq * 100000); // freq × 1000 × 100
    cmd.append(char((freqInt >> 16) & 0xFF));
    cmd.append(char((freqInt >> 8) & 0xFF));
    cmd.append(char(freqInt & 0xFF));

    cmd.append(char(0x00));  // ac field (ignored)

    // Direction
    quint8 dirByte = 0x00;
    if (lastSteppirStatus.direction == "Bi-directional") dirByte = 0x80;
    else if (lastSteppirStatus.direction == "180")       dirByte = 0x40;
    else if (lastSteppirStatus.direction == "3/4 Wave")   dirByte = 0x20;
    // else remains 0x00 for Normal
    cmd.append(char(dirByte));

    // Track flag: 0x55 = auto track on, 0x52 = home/reset
    cmd.append(char(0x00));

    cmd.append(char(0x00));  // filler
    cmd.append(char(0x0D));  // carriage return

    client->sendCommand(cmd);
}
void MainWindow::on_pFastUp_clicked()
{
    SetFrequency(lastSteppirStatus.frequencyMHz + 0.05);
}


void MainWindow::on_pSlowDown_clicked()
{
    SetFrequency(lastSteppirStatus.frequencyMHz - 0.005);
}


void MainWindow::on_pSlowUp_clicked()
{
    SetFrequency(lastSteppirStatus.frequencyMHz + 0.005);
}


void MainWindow::on_p80m_clicked()
{
    SetFrequency(3.650);
}


void MainWindow::on_p60m_clicked()
{
    SetFrequency(5.125);
}


void MainWindow::on_p40m_clicked()
{
    SetFrequency(7.1);
}


void MainWindow::on_p30m_clicked()
{
    SetFrequency(10.125);
}


void MainWindow::on_p20m_clicked()
{
    SetFrequency(14.2);
}


void MainWindow::on_p17m_clicked()
{
    SetFrequency(18.125);
}


void MainWindow::on_p15m_clicked()
{
    SetFrequency(21.15);
}


void MainWindow::on_p12m_clicked()
{
    SetFrequency(24.925);
}


void MainWindow::on_p10m_clicked()
{
    SetFrequency(28.3);
}


void MainWindow::on_p6m_clicked()
{
    SetFrequency(50.3);
}


void MainWindow::on_pControllerSwap_clicked()
{
    qDebug() << "Controller Swap Clicked";

    // Disconnect old client safely
    if (client) {
        disconnect(client, nullptr, this, nullptr);  // Disconnect all signals from client
        client->close();                             // Gracefully close connection
        client->deleteLater();                       // Safe deletion after event loop
        client = nullptr;
    }

    // Determine new target
    QString newHost;
    quint16 newPort;

    if (ui->pControllerSwap->text() == "House") {
        ui->pControllerSwap->setText("Cottage");
        newHost = "192.168.4.120";
        newPort = 4001;
    } else {
        ui->pControllerSwap->setText("House");
        newHost = "192.168.2.136";
        newPort = 4002;
    }

    // Create new client
    client = new TcpSteppirClient(newHost, newPort, this);

    // Reconnect signals
    connect(client, &SteppirClient::receivedParsedStatus, this, &MainWindow::UpdateSteppirStatus);
    connect(client, &SteppirClient::connectionError, this, [](const QString &err) {
        qWarning() << "Steppir connection error:" << err;
    });

    // Initiate new connection
    client->connectToDevice();
}



