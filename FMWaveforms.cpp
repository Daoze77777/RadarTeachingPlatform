#include "FMWaveforms.h"
#include <QtMath>
#include <QRandomGenerator>

// ===== 内部辅助函数：生成三角波频率值 =====
// t: 时间(µs)，delay: 时间偏移(µs)
// 三角波：峰值1400MHz，谷值400MHz，周期1000µs
static double triangleFreq(double t, double delay = 0.0)
{
    const double fMin   = 400.0;
    const double fMax   = 1400.0;
    const double period = 1000.0;
    const double amp    = fMax - fMin; // 1000 MHz

    // 带延迟的时间
    double td = t - delay;

    // 归一化到一个周期内（0~1）
    double phase = fmod(td / period, 1.0);
    if (phase < 0) phase += 1.0;

    // 三角波：0~0.5上升，0.5~1下降
    double tri;
    if (phase < 0.5)
        tri = phase * 2.0;        // 0 → 1
    else
        tri = (1.0 - phase) * 2.0; // 1 → 0

    return fMin + amp * tri;
}

// ===== FMTransmissionWaveform =====
void FMTransmissionWaveform::generate(QVector<double> &x, QVector<double> &y)
{
    const int points = 500;
    x.resize(points);
    y.resize(points);

    for (int i = 0; i < points; ++i) {
        x[i] = i * (2000.0 / (points - 1));
        double base   = triangleFreq(x[i]);
        double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * 8.0;
        y[i] = base + jitter;
    }
}

// ===== FMReceptionWaveform =====
// graph(0) 绿色：发射信号（无延迟）
void FMReceptionWaveform::generate(QVector<double> &x, QVector<double> &y)
{
    const int points = 500;
    x.resize(points);
    y.resize(points);

    for (int i = 0; i < points; ++i) {
        x[i] = i * (2000.0 / (points - 1));
        double base   = triangleFreq(x[i]);
        double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * 8.0;
        y[i] = base + jitter;
    }
}

// graph(1) 黄色：接收信号（延迟100µs）
void FMReceptionWaveform::generateExtra(QVector<double> &x, QVector<double> &y)
{
    const int    points = 500;
    const double delay  = 100.0; // µs
    x.resize(points);
    y.resize(points);

    for (int i = 0; i < points; ++i) {
        x[i] = i * (2000.0 / (points - 1));
        double base   = triangleFreq(x[i], delay);
        double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * 8.0;
        y[i] = base + jitter;
    }
}

// ===== MixedFrequencyWaveform =====
// 基线1500 Counts，在0.8µs和2.3µs处有向下凹陷，谷值500
void MixedFrequencyWaveform::generate(QVector<double> &x, QVector<double> &y)
{
    const int    points   = 600;
    const double baseline = 1500.0;
    const double dipDepth = 1000.0; // 从基线往下的深度（1500-500=1000）
    const double sigma    = 0.08;   // 凹陷宽度

    // 两个凹陷中心位置
    const QVector<double> dipCenters = {0.8, 2.3};

    x.resize(points);
    y.resize(points);

    for (int i = 0; i < points; ++i) {
        x[i] = i * (3.0 / (points - 1));

        double dip = 0.0;
        for (double center : dipCenters) {
            double dt = x[i] - center;
            dip += dipDepth * qExp(-dt * dt / (2.0 * sigma * sigma));
        }

        double noiseAmp = (dip > 100) ? 5.0 : 15.0;
        double jitter   = (QRandomGenerator::global()->generateDouble() * 2 - 1) * noiseAmp;

        y[i] = qBound(0.0, baseline - dip + jitter, 2000.0);
    }
}

// ===== FrequencyMeterWaveform =====
// 和 MixedFrequency 形状一样，但 Y 轴单位是 MHz，底部有接近0的基线
void FrequencyMeterWaveform::generate(QVector<double> &x, QVector<double> &y)
{
    const int    points   = 600;
    const double baseline = 1500.0;
    const double dipDepth = 1000.0;
    const double sigma    = 0.08;

    const QVector<double> dipCenters = {0.8, 2.3};

    x.resize(points);
    y.resize(points);

    // 底部基线（接近0）
    // QVector<double> baseX, baseY;
    // for (int i = 0; i < points; ++i) {
    //     double xi = i * (3.0 / (points - 1));
    //     baseX.append(xi);
    //     baseY.append(2.0 + (QRandomGenerator::global()->generateDouble() * 2 - 1) * 1.0);
    // }

    for (int i = 0; i < points; ++i) {
        x[i] = i * (3.0 / (points - 1));

        double dip = 0.0;
        for (double center : dipCenters) {
            double dt = x[i] - center;
            dip += dipDepth * qExp(-dt * dt / (2.0 * sigma * sigma));
        }

        double noiseAmp = (dip > 100) ? 5.0 : 15.0;
        double jitter   = (QRandomGenerator::global()->generateDouble() * 2 - 1) * noiseAmp;

        y[i] = qBound(0.0, baseline - dip + jitter, 2000.0);
    }
}

void FrequencyMeterWaveform::generateExtra(QVector<double> &x, QVector<double> &y)
{
    const int points = 600;
    x.resize(points);
    y.resize(points);

    for (int i = 0; i < points; ++i) {
        x[i] = i * (3.0 / (points - 1));
        y[i] = 2.0 + (QRandomGenerator::global()->generateDouble() * 2 - 1) * 1.0;
    }
}
