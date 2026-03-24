#ifndef RADARRANGINGDISPLAY_H
#define RADARRANGINGDISPLAY_H

#include <QWidget>
#include <QTimer>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "qcustomplot.h"

class QCPCurve;
class QCPItemLine;
class QCPItemEllipse;

class RadarRangingDisplay : public QWidget {
    Q_OBJECT
public:
    explicit RadarRangingDisplay(QWidget *parent = nullptr);
    void onResetClicked();

private slots:
    void onStartClicked();

    void updateAnimationTick();
    void onDistanceChanged(double distance);

private:
    void setupUI();
    void setupPlot();
    void updatePulseVisuals();

    QCustomPlot *m_plot;
    QDoubleSpinBox *m_spinDistance; // 手动设置距离
    QLabel *m_lblTime;             // 右侧显示脉冲时间
    QPushButton *m_btnStart;
    QPushButton *m_btnReset;

    QCPCurve       *m_pulseCurve;
    QCPItemLine    *m_targetLine;
    QVector<QCPCurve*> m_bgArcs;
    QCPItemEllipse *m_hitMarker; // 红色命中标记

    QTimer *m_timer;
    double m_targetDistance;
    double m_currentPos;
    bool m_isReturning;
    bool m_isRunning;
    const double C_KM_US = 0.3; // 光速常量

signals:
    void animationFinished(double pulseTimeUs); // 动画结束，传递脉冲时间
};
#endif
