#ifndef PULSEWAVEFORMS_H
#define PULSEWAVEFORMS_H

#include "WaveformBase.h"

// ===== s2：触发脉冲 =====
class TriggerPulseWaveform : public WaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override;
    QString xLabel() const override { return "时间 (µs)"; }
    QString yLabel() const override { return "电压 (V)"; }
    double  xMax()   const override { return 3050.0; }
    double  yMin()   const override { return -0.1; }
    double  yMax()   const override { return 1.3; }
};

// ===== s3：脉冲调制 =====
class PulseModulationWaveform : public WaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override;
    QString xLabel() const override { return "时间 (µs)"; }
    QString yLabel() const override { return "电压 (V)"; }
    double  xMax()   const override { return 3050.0; }
    double  yMin()   const override { return -0.1; }
    double  yMax()   const override { return 1.3; }
};

// ===== s4：中频信号 =====
class IntermediateFrequencyWaveform : public WaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override;
    QString xLabel() const override { return "时间 (µs)"; }
    QString yLabel() const override { return "电压 (V)"; }
    double  xMax()   const override { return 102.0; }
    double  yMin()   const override { return -0.1; }
    double  yMax()   const override { return 1.3; }
};

// ===== s5：本振信号 =====
class LocalOscillatorWaveform : public WaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override;
    QString xLabel() const override { return "频率 (GHz)"; }
    QString yLabel() const override { return "功率 (dBm)"; }
    double xMin() const override { return 9.30; }
    double  xMax()   const override { return 9.381; }
    double  yMin()   const override { return -150.0; }
    double  yMax()   const override { return 30.0; }
};

// ===== s6：射频发射 =====
class RFEmitWaveform : public WaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override;
    QString xLabel() const override { return "频率 (GHz)"; }
    QString yLabel() const override { return "功率 (dBm)"; }
    double xMin() const override { return 9.30; }
    double  xMax()   const override { return 9.50; }
    double  yMin()   const override { return -100.0; }
    double  yMax()   const override { return 52.0; }
};

// ===== s7：射频接收 =====
class RFReceiveWaveform : public WaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override;
    QString xLabel() const override { return "频率 (GHz)"; }
    QString yLabel() const override { return "功率 (dBm)"; }
    double xMin() const override { return 9.30; }
    double  xMax()   const override { return 9.50; }
    double  yMin()   const override { return -100.0; }
    double  yMax()   const override { return 52.0; }
};

// ===== s8：基带信号 =====
class BasebandWaveform : public WaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override;
    QString xLabel() const override { return "时间 (µs)"; }
    QString yLabel() const override { return "电压 (V)"; }
    double  xMax()   const override { return 8.0; }
    double  yMin()   const override { return 0.0; }
    double  yMax()   const override { return 1.0; }
};

// ===== s9：检波器 =====
class DetectorWaveform : public WaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override;
    QString xLabel() const override { return "时间 (µs)"; }
    QString yLabel() const override { return "电压 (V)"; }
    double  xMax()   const override { return 50.0; }
    double  yMin()   const override { return 0.0; }
    double  yMax()   const override { return 1.0; }
};

// ===== s10：门限 =====
class ThresholdWaveform : public WaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override;
    QString xLabel() const override { return "时间 (µs)"; }
    QString yLabel() const override { return "电压 (V)"; }
    double  xMax()   const override { return 101.0; }
    double  yMin()   const override { return 0.0; }
    double  yMax()   const override { return 1.0; }
};

// ===== s11：和支路 =====
class BranchWaveform : public WaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override;
    QString xLabel() const override { return "时间 (µs)"; }
    QString yLabel() const override { return "电压 (V)"; }
    double  xMax()   const override { return 80.0; }
    double  yMin()   const override { return 0.0; }
    double  yMax()   const override { return 1.0; }
};

// ===== s12：微分 =====
class DifferentialWaveform : public WaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override;
    QString xLabel() const override { return "时间 (µs)"; }
    QString yLabel() const override { return "电压 (V)"; }
    double  xMax()   const override { return 80.0; }
    double  yMin()   const override { return -0.4; }
    double  yMax()   const override { return 0.4; }
};

// ===== s13：差支路 =====
class TributaryWaveform : public WaveformBase
{
public:
    void generate(QVector<double> &x, QVector<double> &y) override;
    QString xLabel() const override { return "时间 (µs)"; }
    QString yLabel() const override { return "电压 (V)"; }
    double  xMax()   const override { return 80.0; }
    double  yMin()   const override { return 0.0; }
    double  yMax()   const override { return 0.8; }
};

// ===== s14原理演示：距离动画完成后的脉冲波形 =====
class DistancePulseWaveform : public WaveformBase
{
public:
    explicit DistancePulseWaveform(double peakTimeUs = 0.0)
        : m_peakTimeUs(peakTimeUs) {}

    void setPeakTime(double t) { m_peakTimeUs = t; }
    double getPeakTime() const { return m_peakTimeUs; }

    void generate(QVector<double> &x, QVector<double> &y) override;
    QString xLabel() const override { return "时间 (µs)"; }
    QString yLabel() const override { return "电压 (V)"; }
    double  xMax()   const override { return 1000.0; }
    double  yMin()   const override { return 0.0; }
    double  yMax()   const override { return 1.0; }

private:
    double m_peakTimeUs = 0.0;
};

// ===== s14测试验证：用户取得距离后的脉冲波形 =====
class DistanceUsWaveform : public WaveformBase
{
public:
    explicit DistanceUsWaveform(double peakTimeUs = 0.0)
        : m_peakTimeUs(peakTimeUs) {}

    void setPeakTime(double t) { m_peakTimeUs = t; }

    void generate(QVector<double> &x, QVector<double> &y) override;
    QString xLabel() const override { return "时间 (µs)"; }
    QString yLabel() const override { return "电压 (V)"; }
    double  xMax()   const override { return qMax(m_peakTimeUs * 1.5, 4.0); }
    double  yMin()   const override { return 0.0; }
    double  yMax()   const override { return 1.0; }

private:
    double m_peakTimeUs = 0.0;
};

#endif // PULSEWAVEFORMS_H
