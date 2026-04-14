#ifndef FMRANGINGWIDGET_H
#define FMRANGINGWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QtMath>

class FMRangingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FMRangingWidget(QWidget *parent = nullptr);
    void feedDistance(int rawDistance);
    void reset();
    void setManualInputEnabled(bool enabled) {
        m_spinDistance->setEnabled(enabled);
        m_btnStart->setEnabled(enabled);
    }

signals:
    void waveformRequested(double deltaF_MHz);

private slots:
    void onStartClicked();
    void onResetClicked();

private:
    void setupUI();
    void calculate(double R_km);

    QDoubleSpinBox *m_spinDistance;
    QPushButton    *m_btnStart;
    QPushButton    *m_btnReset;

    QLabel *m_lblR;
    QLabel *m_lblDeltaF;
    QLabel *m_lblDelay;
    QLabel *m_lblRmax;
    QLabel *m_lblRsolved;

    // 固定参数
    static constexpr double DELTA_F_MHZ = 1000.0; // 频偏
    static constexpr double T_HALF_US   = 500.0;  // 三角波半周期
    static constexpr double C_KM_US     = 0.3;    // 光速
    // Rmax = C * T/2 / (2*1) = 0.3*500/2 = 75km
    static constexpr double RMAX_KM     = 75.0;
};

#endif
