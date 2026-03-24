#ifndef OSCILLOSCOPEWIDGET_H
#define OSCILLOSCOPEWIDGET_H

#include <QWidget>
#include "qcustomplot.h" // 确保路径正确
#include <QTimer>
#include <QRandomGenerator>

class OscilloscopeWidget : public QWidget {
    Q_OBJECT

public:
    enum WaveType {
        None,
        TriggerPulse,
        PulseModulation,
        IntermediateFrequency,
        LocalOscillator,
        RFEmit,
        RFReceive,
        Baseband,
        Detector,
        Threshold,
        Branch,
        Differential,
        Tributary,
        Distance,
        DistanceNs   // 新增
    };

    explicit OscilloscopeWidget(QWidget *parent = nullptr);
    void setData(const QString& stepId);

public slots:
    void onRadarAnimationFinished(double pulseTimeUs);
    void onDistanceWaveformRequested(double timeNs);


private slots:
    void onRefreshTick(); // 刷新数据和噪声


private:
    void setupPlot();
    void drawDistanceNsWaveform();

    QCustomPlot *m_plot;
    QCPGraph    *m_graph;
    QTimer      *m_timer;
    WaveType     m_currentMode = None;
    double m_distancePulseTime = 0.0; // 存储脉冲时间
    double m_distanceTimeNs    = 0.0;  // 新增
};

#endif
