#ifndef PHASEWAVEFORMS_H
#define PHASEWAVEFORMS_H

#include "WaveformBase.h"

// ===== 通用坐标轴配置（s2~s7 共用）=====
class PhaseWaveformBase : public WaveformBase
{
public:
    QString xLabel() const override { return "时间 (µs)"; }
    QString yLabel() const override { return "电压 (V)"; }
    double  xMin()   const override { return 0.0; }
    double  xMax()   const override { return 1300.0; }
    double  yMin()   const override { return 0.0; }
    double  yMax()   const override { return 5.0; }
    double  xStep()  const override { return 130.0; }
    double  yStep()  const override { return 0.6; }
};

// ===== s1：空坐标 =====
class PhaseEmptyWaveform : public PhaseWaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override
    {
        x.clear();
        y.clear();
    }
};

// ===== s2：发射信号1（从接近0开始上升，峰值在190µs）=====
class TransmitSignal1Waveform : public PhaseWaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override;
};

// ===== s3：发射信号2（从约3.8V开始，初相偏移）=====
class TransmitSignal2Waveform : public PhaseWaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override;
};

// ===== s4：接收信号1（从约2.5V开始，峰值在260µs）=====
class ReceiveSignal1Waveform : public PhaseWaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override;
};

// ===== s5：接收信号2（从约3.7V开始，峰值在200µs）=====
class ReceiveSignal2Waveform : public PhaseWaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override;
};

// ===== s6：基准振荡信号（从峰值4.43V开始下降）=====
class OscillationWaveform : public PhaseWaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override;
};

// ===== s7：接收机混频输出信号（从约4.35V开始，谷值在430µs）=====
class ReceiverMixerWaveform : public PhaseWaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override;
};

// ===== s8：相位器调零输出（直流0.58V + 高频噪声，X到1400µs）=====
class PhaseShifterZeroWaveform : public WaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override;
    QString xLabel() const override { return "时间 (µs)"; }
    QString yLabel() const override { return "电压 (V)"; }
    double  xMin()   const override { return 0.0; }
    double  xMax()   const override { return 1400.0; }
    double  yMin()   const override { return 0.0; }
    double  yMax()   const override { return 5.0; }
    double  xStep()  const override { return 140.0; }
    double  yStep()  const override { return 0.6; }
};

#endif // PHASEWAVEFORMS_H
