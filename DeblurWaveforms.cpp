#include "DeblurWaveforms.h"
#include <QtMath>
#include <QRandomGenerator>
#include <QMap>

// ===== 内部辅助函数：生成脉冲串 =====
// baseline: 基线电压
// peakVal:  尖峰电压
// period:   脉冲周期（µs）
// offset:   第一个脉冲相对0的偏移（µs），控制起始相位
// xMax:     X轴最大值
static void generatePulseTrain(QVector<double> &x, QVector<double> &y,
                               double baseline, double peakVal,
                               double period, double offset,
                               double xMax, double noiseAmp = 0.02)
{
    const int    points    = 800;
    const double peakWidth = 3.0;

    QMap<double, double> pts;

    // 均匀采样点
    for (int i = 0; i < points; ++i) {
        double xi    = i * (xMax / (points - 1));
        double phase = fmod(xi - offset, period);
        if (phase < 0) phase += period;
        double dist  = qMin(phase, period - phase);

        double val = (dist < peakWidth)
                         ? baseline + (peakVal - baseline) * (1.0 - dist / peakWidth)
                         : baseline;

        double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * noiseAmp;
        pts[xi] = qBound(0.0, val + jitter, 8.0);
    }

    // 强制插入每个脉冲的峰顶点，保证峰值准确
    double firstPeak = offset;
    while (firstPeak > period) firstPeak -= period;
    for (double pos = firstPeak; pos <= xMax; pos += period) {
        pts[pos] = peakVal; // 峰顶强制赋值，不加噪声
    }

    x = pts.keys().toVector();
    y = pts.values().toVector();
}
// ===== DeblurTS1Waveform（s2）=====
// 周期200µs，第一个脉冲在约180µs
void DeblurTS1Waveform::generate(QVector<double> &x, QVector<double> &y)
{
    generatePulseTrain(x, y, 4.0, 6.0, 200.0, 180.0, 2200.0);
}

// ===== DeblurRS1Waveform（s3）=====
// 上轨：和s2一样
void DeblurRS1Waveform::generate(QVector<double> &x, QVector<double> &y)
{
    generatePulseTrain(x, y, 4.0, 6.0, 200.0, 180.0, 2200.0);
}
// 下轨：基线0.1V，尖峰2.3V，延迟33µs（脉冲在213µs）
void DeblurRS1Waveform::generateExtra(QVector<double> &x, QVector<double> &y)
{
    generatePulseTrain(x, y, 0.1, 2.3, 200.0, 213.0, 2200.0, 0.01);
}

// ===== DeblurTS2Waveform（s4）=====
// 周期250µs，第一个脉冲在约230µs
void DeblurTS2Waveform::generate(QVector<double> &x, QVector<double> &y)
{
    generatePulseTrain(x, y, 4.0, 6.0, 250.0, 230.0, 2750.0);
}

// ===== DeblurRS2Waveform（s5）=====
// 上轨：和s4一样
void DeblurRS2Waveform::generate(QVector<double> &x, QVector<double> &y)
{
    generatePulseTrain(x, y, 4.0, 6.0, 250.0, 230.0, 2750.0);
}
// 下轨：基线0.1V，尖峰2.3V，延迟133µs（脉冲在363µs）
void DeblurRS2Waveform::generateExtra(QVector<double> &x, QVector<double> &y)
{
    generatePulseTrain(x, y, 0.1, 2.3, 250.0, 363.0, 2750.0, 0.01);
}

// ===== CoincidentTransmittedWaveform（s6）=====
// 基线4V，仅在1000µs处有单个尖峰
void CoincidentTransmittedWaveform::generate(QVector<double> &x, QVector<double> &y)
{
    const int    points    = 800;
    const double peakWidth = 3.0;
    const QVector<double> peaks = {0.0, 1000.0}; // 两个峰

    QMap<double, double> pts;

    for (int i = 0; i < points; ++i) {
        double xi  = i * (2200.0 / (points - 1));
        double val = 4.0;
        for (double pos : peaks) {
            double dist = qAbs(xi - pos);
            if (dist < peakWidth)
                val = qMax(val, 4.0 + (6.0 - 4.0) * (1.0 - dist / peakWidth));
        }
        double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * 0.02;
        pts[xi] = qBound(0.0, val + jitter, 8.0);
    }
    // 强制插入峰顶
    for (double pos : peaks) pts[pos] = 6.0;

    x = pts.keys().toVector();
    y = pts.values().toVector();
}

// ===== CoincidentReceivedWaveform（s7）=====
// 上轨：基线4V，1000µs处单峰6V
void CoincidentReceivedWaveform::generate(QVector<double> &x, QVector<double> &y)
{
    // 上轨和s6一样，两个峰在0和1000µs
    const int    points    = 800;
    const double peakWidth = 3.0;
    const QVector<double> peaks = {0.0, 1000.0};

    QMap<double, double> pts;

    for (int i = 0; i < points; ++i) {
        double xi  = i * (2000.0 / (points - 1));
        double val = 4.0;
        for (double pos : peaks) {
            double dist = qAbs(xi - pos);
            if (dist < peakWidth)
                val = qMax(val, 4.0 + (6.0 - 4.0) * (1.0 - dist / peakWidth));
        }
        double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * 0.02;
        pts[xi] = qBound(0.0, val + jitter, 8.0);
    }
    for (double pos : peaks) pts[pos] = 6.0;

    x = pts.keys().toVector();
    y = pts.values().toVector();
}

// 下轨：630µs和1640µs各有一个2.3V脉冲，中间（630~1000µs）低电平约0.1V，其余基线0.1V
void CoincidentReceivedWaveform::generateExtra(QVector<double> &x, QVector<double> &y)
{
    const int    points    = 800;
    const double peakWidth = 3.0;

    // 两个脉冲位置
    const QVector<double> peakPositions = {630.0, 1640.0};

    x.resize(points);
    y.resize(points);

    for (int i = 0; i < points; ++i) {
        x[i] = i * (2000.0 / (points - 1));

        // 检查是否在脉冲附近
        double peakVal = 0.0;
        for (double pos : peakPositions) {
            double dist = qAbs(x[i] - pos);
            if (dist < peakWidth) {
                peakVal = qMax(peakVal, 2.3 * (1.0 - dist / peakWidth));
            }
        }

        double val;
        if (peakVal > 0.0) {
            val = peakVal;
        } else {
            val = 0.1; // 基线接近0
        }

        double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * 0.01;
        y[i] = qBound(0.0, val + jitter, 8.0);
    }
}

void DeblurResultWaveform::generate(QVector<double> &x, QVector<double> &y)
{
    generatePulseTrain(x, y, 4.0, 6.0, m_period, m_period * 0.9, m_period * 11);
}

void DeblurResultWaveform::generateExtra(QVector<double> &x, QVector<double> &y)
{
    generatePulseTrain(x, y, 0.1, 2.3, m_period, m_period * 0.9 + m_delay, m_period * 11, 0.01);
}
