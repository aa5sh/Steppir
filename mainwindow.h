#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "SteppirClient.h"
#include <QSettings>
#include <QMainWindow>
#include <QPushButton>
#include "udplistener.h"
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handleRigStatus(const QString &profile, double freq, bool ptt);

    void on_pAuto_clicked();

    void on_pCalibrate_clicked();

    void on_pHome_clicked();

    void on_pNormal_clicked();

    void on_p180_clicked();

    void on_pBiDir_clicked();

    void on_p34_clicked();

    void on_pFastDown_clicked();

    void on_pFastUp_clicked();

    void on_pSlowDown_clicked();

    void on_pSlowUp_clicked();

    void on_p80m_clicked();

    void on_p60m_clicked();

    void on_p40m_clicked();

    void on_p30m_clicked();

    void on_p20m_clicked();

    void on_p17m_clicked();

    void on_p15m_clicked();

    void on_p12m_clicked();

    void on_p10m_clicked();

    void on_p6m_clicked();

    void on_pControllerSwap_clicked();

private:
    Ui::MainWindow *ui;
    SteppirClient *client = nullptr;

    QSettings settings;
    void SetButtonSelected(QPushButton *button, bool selected);
    UDPListener *udpListener;
    void RequestSteppirStatus();
    void checkSteppirUpdateTimeout();
    void UpdateSteppirStatus(SteppirStatus status);
    SteppirStatus lastSteppirStatus;
    void loadDefaultSettings() {
        if (!settings.contains("transport"))
            settings.setValue("transport", "serial");
        if (!settings.contains("serialPort"))
            settings.setValue("serialPort", "");
        if (!settings.contains("ip"))
            settings.setValue("ip", "192.168.2.2");
        if (!settings.contains("port"))
            settings.setValue("port", 4001);
    }
    QTimer lastUpdateCheckTimer;
    QDateTime lastSteppirUpdate;
    QDateTime lastQlogUpdate;
    void SetDirection(QString dir);
    void SetFrequency(double freq);
};
#endif // MAINWINDOW_H
