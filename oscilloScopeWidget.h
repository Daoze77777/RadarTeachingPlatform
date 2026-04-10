#ifndef OSCILLOSCOPEWIDGET_H
#define OSCILLOSCOPEWIDGET_H

#include <QWidget>
#include <QMap>
#include <memory>
#include "qcustomplot.h"
#include <QTimer>
#include "WaveformBase.h"
#include "PulseWaveforms.h"

class OscilloscopeWidget : public QWidget {
    Q_OBJECT

public:
    explicit OscilloscopeWidget(QWidget *parent = nullptr);

    // 传入 waveform 字符串（对应 XML 里的 waveform 属性），空字符串清空波形
    void setData(const QString &waveform);

public slots:
    // 原理演示动画完成后触发（RadarRangingDisplay 信号）
    void onRadarAnimationFinished(double pulseTimeUs);
    // 测试验证用户点"开始"后触发（RadarDistanceWidget 信号）
    void onDistanceWaveformRequested(double timeUs);

private slots:
    void onRefreshTick();

private:
    void setupPlot();
    void registerWaveforms();           // 注册所有波形到 m_waveforms
    void applyAxisConfig();             // 根据当前波形配置坐标轴
    void applyExtraGraphs();            // Distance/DistanceUs 需要额外 graph

    QCustomPlot  *m_plot;
    QCPGraph     *m_graph;
    QTimer       *m_timer;

    // 波形注册表：waveform名 → 波形对象
    QMap<QString, std::shared_ptr<WaveformBase>> m_waveforms;

    // 当前激活的波形，nullptr 表示空白
    std::shared_ptr<WaveformBase> m_currentWaveform = nullptr;

    // Distance / DistanceUs 需要单独持有指针以便更新参数
    std::shared_ptr<DistancePulseWaveform> m_distancePulseWaveform;
    std::shared_ptr<DistanceUsWaveform>    m_distanceUsWaveform;

    // 标记当前是否为 DistanceUs 模式（该模式不参与 timer 刷新）
    bool m_isDistanceUsMode = false;
};

#endif // OSCILLOSCOPEWIDGET_H
