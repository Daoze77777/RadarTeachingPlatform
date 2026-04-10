#ifndef DEBLURWAVEFORMS_H
#define DEBLURWAVEFORMS_H

#include "WaveformBase.h"

// ===== 通用坐标轴配置（退模糊大部分波形共用）=====
class DeblurWaveformBase : public WaveformBase
{
public:
    QString xLabel() const override { return "时间 (µs)"; }
    QString yLabel() const override { return "电压 (V)"; }
    double  xMin()   const override { return 0.0; }
    double  yMin()   const override { return 0.0; }
    double  yMax()   const override { return 8.0; }
    double  yStep()  const override { return 1.0; }
};

// ===== s1：空坐标（X到2200µs）=====
class DeblurEmptyWaveform : public DeblurWaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override
    {
        x.clear();
        y.clear();
    }
    double xMax()  const override { return 2200.0; }
    double xStep() const override { return 200.0; }
};

// ===== s2：发射信号1（周期200µs，基线4V，尖峰6V，X到2200µs）=====
class DeblurTS1Waveform : public DeblurWaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override;
    double xMax()  const override { return 2200.0; }
    double xStep() const override { return 200.0; }
};

// ===== s3：接收信号1（双轨道，上轨同s2，下轨延迟33µs，X到2200µs）=====
class DeblurRS1Waveform : public DeblurWaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override;      // 上轨
    void generateExtra(QVector<double> &x, QVector<double> &y) override; // 下轨
    bool needsExtraGraph() const override { return true; }
    double xMax()  const override { return 2200.0; }
    double xStep() const override { return 200.0; }
};

// ===== s4：发射信号2（周期250µs，基线4V，尖峰6V，X到2750µs）=====
class DeblurTS2Waveform : public DeblurWaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override;
    double xMax()  const override { return 2750.0; }
    double xStep() const override { return 250.0; }
};

// ===== s5：接收信号2（双轨道，上轨同s4，下轨延迟133µs，X到2750µs）=====
class DeblurRS2Waveform : public DeblurWaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override;      // 上轨
    void generateExtra(QVector<double> &x, QVector<double> &y) override; // 下轨
    bool needsExtraGraph() const override { return true; }
    double xMax()  const override { return 2750.0; }
    double xStep() const override { return 250.0; }
};

// ===== s6：重合发射脉冲串（基线4V，仅1000µs处单峰6V，X到2200µs）=====
class CoincidentTransmittedWaveform : public DeblurWaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override;
    double xMax()  const override { return 2200.0; }
    double xStep() const override { return 200.0; }
};

// ===== s7：重合接收脉冲串（双轨道，X到2000µs）=====
class CoincidentReceivedWaveform : public DeblurWaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override;      // 上轨
    void generateExtra(QVector<double> &x, QVector<double> &y) override; // 下轨
    bool needsExtraGraph() const override { return true; }
    double xMax()  const override { return 2000.0; }
    double xStep() const override { return 200.0; }
};

#endif // DEBLURWAVEFORMS_H
