#include "PhaseWaveforms.h"
#include <QtMath>
#include <QRandomGenerator>

// ===== 内部辅助函数：生成相位法正弦波 =====
static void generateSineWave(QVector<double> &x, QVector<double> &y,
                              double center, double amp,
                              double period, double phase,
                              double xMax = 1300.0,
                              double noiseAmp = 0.015)
{
    const int points = 500;
    x.resize(points);
    y.resize(points);

    for (int i = 0; i < points; ++i) {
        x[i] = i * (xMax / (points - 1));
        double base   = center + amp * qSin(2.0 * M_PI / period * x[i] + phase);
        double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * noiseAmp;
        y[i] = qBound(0.0, base + jitter, 5.0);
    }
}

// 所有正弦波：中心2.5V，幅度1.93V（峰4.43V，谷0.57V），周期650µs

// ===== s2：发射信号1（从接近0开始上升，峰值在190µs）=====
void TransmitSignal1Waveform::generate(QVector<double> &x, QVector<double> &y)
{
    generateSineWave(x, y, 2.5, 1.93, 650.0, -M_PI / 2.0 + 0.15);
}

// ===== s3：发射信号2（从约3.8V开始）=====
void TransmitSignal2Waveform::generate(QVector<double> &x, QVector<double> &y)
{
    generateSineWave(x, y, 2.5, 1.93, 650.0, -0.35);
}

// ===== s4：接收信号1（从约2.5V开始，峰值在260µs）=====
void ReceiveSignal1Waveform::generate(QVector<double> &x, QVector<double> &y)
{
    generateSineWave(x, y, 2.5, 1.93, 650.0,
                     -M_PI / 2.0 + (2.0 * M_PI / 650.0) * 195.0);
}

// ===== s5：接收信号2（从约3.7V开始，峰值在200µs）=====
void ReceiveSignal2Waveform::generate(QVector<double> &x, QVector<double> &y)
{
    generateSineWave(x, y, 2.5, 1.93, 650.0,
                     -M_PI / 2.0 + (2.0 * M_PI / 650.0) * 155.0);
}

// ===== s6：基准振荡信号（从峰值4.43V开始下降）=====
void OscillationWaveform::generate(QVector<double> &x, QVector<double> &y)
{
    generateSineWave(x, y, 2.5, 1.93, 650.0, M_PI / 2.0);
}

// ===== s7：接收机混频输出信号（从约4.35V开始）=====
void ReceiverMixerWaveform::generate(QVector<double> &x, QVector<double> &y)
{
    generateSineWave(x, y, 2.5, 1.93, 650.0, M_PI / 2.0 - 0.1);
}

// ===== s8：相位器调零输出（直流0.58V + 高频噪声）=====
void PhaseShifterZeroWaveform::generate(QVector<double> &x, QVector<double> &y)
{
    const int    points   = 1000;
    const double baseline = 0.58;
    const double noise    = 0.025;

    x.resize(points);
    y.resize(points);

    for (int i = 0; i < points; ++i) {
        x[i] = i * (1400.0 / (points - 1));
        double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * noise;
        y[i] = qBound(0.0, baseline + jitter, 5.0);
    }
}

void PhaseResultWaveform::generate(QVector<double> &x, QVector<double> &y)
{
    // 上轨：T1=650µs正弦波，相位偏移m_phase1
    const int points = 500;
    x.resize(points); y.resize(points);
    for (int i = 0; i < points; ++i) {
        x[i] = i * (1400.0 / (points - 1));
        double base = 2.5 + 1.93 * qSin(2.0 * M_PI / 650.0 * x[i] + m_phase1);
        double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * 0.015;
        y[i] = qBound(0.0, base + jitter, 5.0);
    }
}

void PhaseResultWaveform::generateExtra(QVector<double> &x, QVector<double> &y)
{
    // 下轨：T2=700µs正弦波，相位偏移m_phase2
    const int points = 500;
    x.resize(points); y.resize(points);
    for (int i = 0; i < points; ++i) {
        x[i] = i * (1400.0 / (points - 1));
        double base = 2.5 + 1.93 * qSin(2.0 * M_PI / 700.0 * x[i] + m_phase2);
        double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * 0.015;
        y[i] = qBound(0.0, base + jitter, 5.0);
    }
}
