#ifndef OSCILLOSCOPEWIDGET_H
#define OSCILLOSCOPEWIDGET_H

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChart>
#include <QTimer>
#include <QRandomGenerator>

class OscilloscopeWidget : public QChartView {
    Q_OBJECT

public:
    // 定义支持的波形模式
    enum WaveType {
        None,           // 无信号（底噪）
        TargetSetup,    // s1: 目标设置（不规则包络）
        PulseTrigger,   // s2: 触发脉冲（窄方波）
        SineWave,       // 正交信号（正弦）
        RadarEcho       // 实际回波（高频抖动脉冲）
    };

    explicit OscilloscopeWidget(QWidget *parent = nullptr);
    virtual ~OscilloscopeWidget();

    // 核心接口：通过按钮 ID 动态切换
    void setData(const QString& stepId);

    // 设置波形参数
    void setParams(double amplitude, double frequency);

private slots:
    void onUpdateTick(); // 处理实时渲染和抖动

private:
    void setupChart();   // 初始化图表样式
    void updateWaveform(); // 计算并更新数据点

    // --- 波形数学库 ---
    double calcTargetWave(double x); // s1 逻辑
    double calcPulseWave(double x);  // s2 逻辑
    double calcSineWave(double x);   // s3 逻辑

private:
    QChart *m_chart;
    QLineSeries *m_series;
    QValueAxis *m_axisX;
    QValueAxis *m_axisY;
    QTimer *m_timer;

    WaveType m_currentMode;
    double m_amplitude;
    double m_frequency;
    double m_timeOffset; // 用于波形滚动
};

#endif // OSCILLOSCOPEWIDGET_H
