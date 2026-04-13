#ifndef TRACKINGWAVEFORMS_H
#define TRACKINGWAVEFORMS_H

#include "WaveformBase.h"
#include <QColor>
// ===== s1/s8：空坐标（X:0~80µs，Y:0~8V）=====
class TrackingEmptyWaveform : public WaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override
    {
        x.clear();
        y.clear();
    }
    QString xLabel() const override { return "时间 (µs)"; }
    QString yLabel() const override { return "电压 (V)"; }
    double  xMin()   const override { return 0.0; }
    double  xMax()   const override { return 80.0; }
    double  yMin()   const override { return 0.0; }
    double  yMax()   const override { return 8.0; }
    double  xStep()  const override { return 8.0; }
    double  yStep()  const override { return 1.0; }
};

// ===== s2：触发脉冲（X:0~3000µs，三个尖峰）=====
class TrackingTriggerWaveform : public WaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override;
    QString xLabel() const override { return "时间 (µs)"; }
    QString yLabel() const override { return "电压 (V)"; }
    double  xMin()   const override { return 0.0; }
    double  xMax()   const override { return 3000.0; }
    double  yMin()   const override { return -0.1; }
    double  yMax()   const override { return 1.2; }
};

// ===== s3/s10：回波信号（X:0~50µs，单高斯峰）=====
class EchoWaveform : public WaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override;
    QString xLabel() const override { return "时间 (µs)"; }
    QString yLabel() const override { return "电压 (V)"; }
    double  xMin()   const override { return 0.0; }
    double  xMax()   const override { return 50.0; }
    double  yMin()   const override { return 0.0; }
    double  yMax()   const override { return 1.2; }
    double  xStep()  const override { return 5.0; }
};

// ===== s4：锯齿电压（X:0~80µs，周期20µs，0~5V）=====
class TrackingSawtoothWaveform : public WaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override;
    QString xLabel() const override { return "时间 (µs)"; }
    QString yLabel() const override { return "电压 (V)"; }
    double  xMin()   const override { return 0.0; }
    double  xMax()   const override { return 80.0; }
    double  yMin()   const override { return 0.0; }
    double  yMax()   const override { return 8.0; }
    double  xStep()  const override { return 8.0; }
    double  yStep()  const override { return 1.0; }
};

// ===== s5：线性电位器输出（X:0~1300µs，直流4.35V）=====
class PotentiometerWaveform : public WaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override;
    QString xLabel() const override { return "时间 (µs)"; }
    QString yLabel() const override { return "电压 (V)"; }
    double  xMin()   const override { return 0.0; }
    double  xMax()   const override { return 1300.0; }
    double  yMin()   const override { return 0.0; }
    double  yMax()   const override { return 8.0; }
    double  xStep()  const override { return 130.0; }
    double  yStep()  const override { return 1.0; }
};

// ===== s6：手动跟踪信号（X:0~100µs，88µs处单峰5V）=====
class ManuallyTrackWaveform : public WaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override;
    QString xLabel() const override { return "时间 (µs)"; }
    QString yLabel() const override { return "电压 (V)"; }
    double  xMin()   const override { return 0.0; }
    double  xMax()   const override { return 100.0; }
    double  yMin()   const override { return 0.0; }
    double  yMax()   const override { return 8.0; }
    double  xStep()  const override { return 10.0; }
    double  yStep()  const override { return 1.0; }
};

// ===== s7：手动跟踪捕获目标（双线：黄色跟踪门+绿色回波）=====
class ManualCaptureWaveform : public WaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override;      // 黄色跟踪门
    void generateExtra(QVector<double> &x, QVector<double> &y) override; // 绿色回波
    bool needsExtraGraph() const override { return true; }
    QColor extraGraphColor() const override { return QColor(0, 180, 60); }
    QString xLabel() const override { return "时间 (µs)"; }
    QString yLabel() const override { return "电压 (V)"; }
    double  xMin()   const override { return 0.0; }
    double  xMax()   const override { return 267.0; }
    double  yMin()   const override { return 0.0; }
    double  yMax()   const override { return 5.0; }
    double  xStep()  const override { return 33.0; }
    double  yStep()  const override { return 1.0; }
};

#endif // TRACKINGWAVEFORMS_H
