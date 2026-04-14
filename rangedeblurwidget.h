#ifndef RANGEDEBLURWIDGET_H
#define RANGEDEBLURWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>

class RangeDeblurWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RangeDeblurWidget(QWidget *parent = nullptr);

    // 测试验证模式：串口喂距离（单位 0.1mm）
    void feedDistance(int rawDistance);

    void reset();

    void setManualInputEnabled(bool enabled) {
        m_spinDistance->setEnabled(enabled);
        m_btnStart->setEnabled(enabled);
    }

signals:
    // 通知示波器更新波形，携带周期和延迟
    void waveformRequested(int track, double period, double delay);

private slots:
    void onStartClicked();
    void onResetClicked();

private:
    void setupUI();
    void calculate(double R_km); // 核心计算，驱动界面更新

    // 输入
    QDoubleSpinBox *m_spinDistance;
    QPushButton    *m_btnStart;
    QPushButton    *m_btnReset;

    // 显示指标
    QLabel *m_lblR;         // 真实距离
    QLabel *m_lblRmax1;     // 最大不模糊距离1
    QLabel *m_lblRmax2;     // 最大不模糊距离2
    QLabel *m_lblR1;        // 模糊距离1
    QLabel *m_lblR2;        // 模糊距离2
    QLabel *m_lblT1;        // 延迟时间1
    QLabel *m_lblT2;        // 延迟时间2
    QLabel *m_lblRsolved;   // 退模糊解算结果

    // 参数
    static constexpr double T1_US   = 1200.0;   // 周期1 µs
    static constexpr double T2_US   = 1400.0;   // 周期2 µs
    static constexpr double C_KM_US = 0.3;     // 光速 km/µs
    double m_Rmax1;  // c*T1/2
    double m_Rmax2;  // c*T2/2

    bool   m_hasCaptured    = false;
    double m_capturedR_km   = 0.0;
};

#endif // RANGEDEBLURWIDGET_H
