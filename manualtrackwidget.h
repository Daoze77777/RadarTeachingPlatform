#ifndef MANUALTRACKWIDGET_H
#define MANUALTRACKWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QtMath>

class ManualTrackWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ManualTrackWidget(QWidget *parent = nullptr);
    bool isEchoLocked() const { return m_echoLocked; }
    void feedKnobVoltage(double voltage);
    void feedEchoPos(double echoPos_us);
    void reset();
    void setManualInputEnabled(bool enabled) {
        m_slider->setEnabled(enabled);
        m_btnStart->setVisible(!enabled);
    }

signals:
    void gatePosChanged(double gatePos);
    void resetted();  // 新增

private slots:
    void onStartClicked();
    void onResetClicked();
    void onSliderChanged(int value);

private:
    void setupUI();
    void updateDisplay(double gatePos);

    QSlider     *m_slider;
    QPushButton *m_btnStart;
    QPushButton *m_btnReset;

    QLabel *m_lblGatePos;
    QLabel *m_lblVoltage;
    QLabel *m_lblDeltaT;
    QLabel *m_lblDeltaR;
    QLabel *m_lblStatus;

    bool m_echoLocked = false;  // 锁定回波位置
    bool m_started    = false;  // 是否已点开始
    double m_echoPos = 133.0;

    static constexpr double C_KM_US  = 0.3;
    static constexpr double X_MAX_US = 267.0;
};

#endif
