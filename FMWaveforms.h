#ifndef FMWAVEFORMS_H
#define FMWAVEFORMS_H

#include "WaveformBase.h"

// ===== s1：空坐标（无波形）=====
class FMEmptyWaveform : public WaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override
    {
        x.clear();
        y.clear();
    }
    QString xLabel() const override { return "时间 (µs)"; }
    QString yLabel() const override { return "频率 (MHz)"; }
    double  xMin()   const override { return 0.0; }
    double  xMax()   const override { return 2000.0; }
    double  yMin()   const override { return 0.0; }
    double  yMax()   const override { return 2000.0; }
    double  xStep()  const override { return 200.0; }
    double  yStep()  const override { return 200.0; }
};

// ===== s2：调频发射信号（单条三角波）=====
class FMTransmissionWaveform : public WaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override;
    QString xLabel() const override { return "时间 (µs)"; }
    QString yLabel() const override { return "频率 (MHz)"; }
    double  xMin()   const override { return 0.0; }
    double  xMax()   const override { return 2000.0; }
    double  yMin()   const override { return 0.0; }
    double  yMax()   const override { return 2000.0; }
    double  xStep()  const override { return 200.0; }
    double  yStep()  const override { return 200.0; }
};

// ===== s3：调频接收信号（绿色发射 + 黄色延迟接收，双线）=====
class FMReceptionWaveform : public WaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override;      // 绿色：发射信号
    void generateExtra(QVector<double> &x, QVector<double> &y) override; // 黄色：接收信号（延迟100µs）
    bool needsExtraGraph() const override { return true; }

    QString xLabel() const override { return "时间 (µs)"; }
    QString yLabel() const override { return "频率 (MHz)"; }
    double  xMin()   const override { return 0.0; }
    double  xMax()   const override { return 2000.0; }
    double  yMin()   const override { return 0.0; }
    double  yMax()   const override { return 2000.0; }
    double  xStep()  const override { return 200.0; }
    double  yStep()  const override { return 200.0; }
};

// ===== s4：混频差频信号 =====
class MixedFrequencyWaveform : public WaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override;
    QString xLabel() const override { return "时间 (µs)"; }
    QString yLabel() const override { return "频率 (MHz)"; }
    double  xMin()   const override { return 0.0; }
    double  xMax()   const override { return 3.0; }
    double  yMin()   const override { return 0.0; }
    double  yMax()   const override { return 2000.0; }
    double  xStep()  const override { return 0.5; }
    double  yStep()  const override { return 200.0; }
};

// ===== s5：频率计输出信号 =====
class FrequencyMeterWaveform : public WaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override;
    void generateExtra(QVector<double> &x, QVector<double> &y) override; // 底部基线
    bool needsExtraGraph() const override { return true; }
    QString xLabel() const override { return "时间 (µs)"; }
    QString yLabel() const override { return "频率 (MHz)"; }
    double  xMin()   const override { return 0.0; }
    double  xMax()   const override { return 3.0; }
    double  yMin()   const override { return 0.0; }
    double  yMax()   const override { return 2000.0; }
    double  xStep()  const override { return 0.5; }
    double  yStep()  const override { return 200.0; }
};

#endif // FMWAVEFORMS_H
