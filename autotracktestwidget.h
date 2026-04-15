#ifndef AUTOTRACKTESTWIDGET_H
#define AUTOTRACKTESTWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QTimer>
#include <QtMath>

class AutoTrackTestWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AutoTrackTestWidget(QWidget *parent = nullptr);
    void feedDistance(int rawDistance);
    void reset();

signals:
    void gatePosChanged(double gatePos);
    void trackingStarted();   // 开始跟踪时发射
    void trackingReset();     // 重置时发射

private slots:
    void onStartClicked();
    void onResetClicked();
    void onTrackTick();

private:
    void setupUI();
    void updateDisplay(double gatePos);

    QPushButton *m_btnStart;
    QPushButton *m_btnReset;

    QLabel *m_lblTargetR;
    QLabel *m_lblEchoPos;
    QLabel *m_lblGatePos;
    QLabel *m_lblDeltaT;
    QLabel *m_lblDeltaR;
    QLabel *m_lblStatus;

    QTimer *m_trackTimer;

    double m_echoPos  = 133.0;
    double m_gatePos  = 0.0;
    double m_targetR  = 0.0;
    bool   m_tracking = false;
    bool   m_locked   = false;

    static constexpr double C_KM_US  = 0.3;
    static constexpr double X_MAX_US = 267.0;
};

#endif
