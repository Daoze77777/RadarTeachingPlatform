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
        Distance
    };

    explicit OscilloscopeWidget(QWidget *parent = nullptr);
    void setData(const QString& stepId);

private slots:
    void onRefreshTick(); // 刷新数据和噪声

private:
    void setupPlot();

    QCustomPlot *m_plot;
    QCPGraph    *m_graph;
    QTimer      *m_timer;
    WaveType     m_currentMode = None;
};

#endif
