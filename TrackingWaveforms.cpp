#include "TrackingWaveforms.h"
#include <QtMath>
#include <QRandomGenerator>
#include <QMap>

// ===== TrackingTriggerWaveform（s2）=====
// 三个尖峰在 250、1200、2200µs，峰值0.97V，基线-0.05V
void TrackingTriggerWaveform::generate(QVector<double> &x, QVector<double> &y)
{
    const QVector<double> peakPos = {250.0, 1200.0, 2200.0};
    const double baseline  = -0.05;
    const double peakVal   = 0.97;
    const double sigma     = 12.0; // 尖峰宽度
    const int    points    = 600;

    QMap<double, double> pts;

    // 均匀采样
    for (int i = 0; i < points; ++i) {
        double xi  = i * (3000.0 / (points - 1));
        double val = baseline;
        for (double pos : peakPos) {
            double dt = xi - pos;
            if (qAbs(dt) < sigma * 4)
                val = qMax(val, baseline + (peakVal - baseline)
                               * qExp(-dt * dt / (2.0 * sigma * sigma)));
        }
        double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * 0.012;
        pts[xi] = qBound(-0.1, val + jitter, 1.2);
    }

    // 强制插入峰顶
    for (double pos : peakPos)
        pts[pos] = peakVal;

    x = pts.keys().toVector();
    y = pts.values().toVector();
}

// ===== EchoWaveform（s3/s10）=====
// 单高斯峰，中心25µs，峰值0.93V，基线0.04V
void EchoWaveform::generate(QVector<double> &x, QVector<double> &y)
{
    const int    points   = 300;
    const double center   = 25.0;
    const double sigma    = 6.0;
    const double baseline = 0.04;
    const double peakVal  = 0.93;

    x.resize(points);
    y.resize(points);

    for (int i = 0; i < points; ++i) {
        x[i] = i * (50.0 / (points - 1));
        double dt  = x[i] - center;
        double val = baseline + (peakVal - baseline)
                     * qExp(-dt * dt / (2.0 * sigma * sigma));
        double noiseAmp = (qAbs(dt) < sigma * 2) ? 0.008 : 0.012;
        double jitter   = (QRandomGenerator::global()->generateDouble() * 2 - 1) * noiseAmp;
        y[i] = qBound(0.0, val + jitter, 1.2);
    }
}

// ===== TrackingSawtoothWaveform（s4）=====
// 锯齿波，周期20µs，0→5V线性上升，瞬间归零
void TrackingSawtoothWaveform::generate(QVector<double> &x, QVector<double> &y)
{
    const double period  = 20.0;
    const double vMax    = 5.0;
    const int    points  = 800;

    QMap<double, double> pts;

    for (int i = 0; i < points; ++i) {
        double xi    = i * (80.0 / (points - 1));
        double phase = fmod(xi, period) / period; // 0~1
        double val   = phase * vMax;
        double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * 0.01;
        pts[xi] = qBound(0.0, val + jitter, 8.0);
    }

    // 强制插入每个周期的峰顶和归零点，保证锯齿形状
    for (double pos = period; pos <= 80.0; pos += period) {
        pts[pos - 0.01] = vMax; // 归零前峰顶
        pts[pos]        = 0.0;  // 瞬间归零
    }

    x = pts.keys().toVector();
    y = pts.values().toVector();
}

// ===== PotentiometerWaveform（s5）=====
// 直流4.35V + 细噪声
void PotentiometerWaveform::generate(QVector<double> &x, QVector<double> &y)
{
    const int    points   = 500;
    const double baseline = 4.35;
    const double noise    = 0.02;

    x.resize(points);
    y.resize(points);

    for (int i = 0; i < points; ++i) {
        x[i] = i * (1300.0 / (points - 1));
        double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * noise;
        y[i] = qBound(0.0, baseline + jitter, 8.0);
    }
}

// ===== ManuallyTrackWaveform（s6）=====
// 基线0，88µs处单峰5V
void ManuallyTrackWaveform::generate(QVector<double> &x, QVector<double> &y)
{
    const double pulseStart = 85.0;  // 脉冲起始
    const double pulseEnd   = 92.0;  // 脉冲结束
    const double peakVal    = 5.0;
    const int    points     = 400;

    QMap<double, double> pts;

    for (int i = 0; i < points; ++i) {
        double xi  = i * (100.0 / (points - 1));
        double val = (xi >= pulseStart && xi <= pulseEnd) ? peakVal : 0.0;
        double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * 0.01;
        pts[xi] = qBound(0.0, val + jitter, 8.0);
    }

    // 强制插入上升沿和下降沿的关键点
    pts[pulseStart - 0.01] = 0.0;
    pts[pulseStart]        = peakVal;
    pts[pulseEnd]          = peakVal;
    pts[pulseEnd + 0.01]   = 0.0;

    x = pts.keys().toVector();
    y = pts.values().toVector();
}

// 黄色：锯齿形跟踪门，0~33µs
void ManualCaptureWaveform::generate(QVector<double> &x, QVector<double> &y)
{
    QMap<double, double> pts;
    // 上升沿
    pts[0.0]  = 0.0;
    pts[0.5]  = 3.0;  // 快速上升到约3V
    // 下降到0
    pts[33.0] = 0.0;
    // 填充中间点
    int points = 200;
    for (int i = 0; i < points; ++i) {
        double xi = i * (267.0 / (points - 1));
        if (xi < 0.5) {
            pts[xi] = xi / 0.5 * 3.0;
        } else if (xi <= 33.0) {
            pts[xi] = 3.0 * (1.0 - (xi - 0.5) / (33.0 - 0.5));
        } else {
            pts[xi] = 0.0;
        }
    }
    x = pts.keys().toVector();
    y = pts.values().toVector();
}

// 绿色：回波高斯峰，中心133µs，峰值3V
void ManualCaptureWaveform::generateExtra(QVector<double> &x, QVector<double> &y)
{
    const int    points  = 300;
    const double center  = 133.0;
    const double sigma   = 20.0;
    const double peak    = 3.0;

    x.resize(points);
    y.resize(points);
    for (int i = 0; i < points; ++i) {
        x[i] = i * (267.0 / (points - 1));
        double dt = x[i] - center;
        double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * 0.015;
        y[i] = qBound(0.0, peak * qExp(-dt*dt/(2.0*sigma*sigma)) + jitter, 5.0);
    }
}

static void generateGateWaveform(QVector<double> &x, QVector<double> &y, double pulseStart, double pulseEnd, double peakVal = 2.0)
{
    const int points = 500;
    QMap<double, double> pts;

    for (int i = 0; i < points; ++i) {
        double xi  = i * (1000.0 / (points - 1));
        double val = (xi >= pulseStart && xi <= pulseEnd) ? peakVal : 0.0;
        double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * 0.012;
        pts[xi] = qBound(0.0, val + jitter, 5.0);
    }
    // 强制上升沿下降沿
    pts[pulseStart - 0.1] = 0.0;
    pts[pulseStart]       = peakVal;
    pts[pulseEnd]         = peakVal;
    pts[pulseEnd + 0.1]   = 0.0;

    x = pts.keys().toVector();
    y = pts.values().toVector();
}

void FrontGateWaveform::generate(QVector<double> &x, QVector<double> &y)
{
    generateGateWaveform(x, y, 580.0, 640.0);
}

void RearGateWaveform::generate(QVector<double> &x, QVector<double> &y)
{
    generateGateWaveform(x, y, 640.0, 700.0);
}

void AutoTrackWaveform::generate(QVector<double> &x, QVector<double> &y)
{
    stepForward();
    const int    points = 300;
    const double center = 133.0;
    const double sigma  = 20.0;
    x.resize(points); y.resize(points);
    for (int i = 0; i < points; ++i) {
        x[i] = i * (267.0 / (points - 1));
        double dt = x[i] - center;
        double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * 0.015;
        y[i] = qBound(0.0, 3.0 * qExp(-dt*dt/(2.0*sigma*sigma)) + jitter, 5.0);
    }
}

void AutoTrackWaveform::generateExtra(QVector<double> &x, QVector<double> &y)
{
    QMap<double, double> pts;
    double center = m_gatePos;
    double width  = 33.0;
    int points = 300;
    for (int i = 0; i < points; ++i) {
        double xi = i * (267.0 / (points - 1));
        double dt = xi - center;
        double val = (qAbs(dt) < width) ? 3.0 * (1.0 - qAbs(dt) / width) : 0.0;
        pts[xi] = qBound(0.0, val, 5.0);
    }
    pts[center] = 3.0;
    x = pts.keys().toVector();
    y = pts.values().toVector();
}

// generate 改成画回波（原来generateExtra的内容）
void ManualTrackDynamicWaveform::generate(QVector<double> &x, QVector<double> &y)
{
    const int    points = 300;
    const double center = m_echoPos;
    const double sigma  = 20.0;
    x.resize(points); y.resize(points);
    for (int i = 0; i < points; ++i) {
        x[i] = i * (267.0 / (points - 1));
        double dt = x[i] - center;
        double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * 0.015;
        y[i] = qBound(0.0, 3.0 * qExp(-dt*dt/(2.0*sigma*sigma)) + jitter, 5.0);
    }
}

// generateExtra 改成画跟踪门（原来generate的内容）
void ManualTrackDynamicWaveform::generateExtra(QVector<double> &x, QVector<double> &y)
{
    QMap<double, double> pts;
    double center = m_gatePos;
    double width  = 33.0;
    int points = 300;
    for (int i = 0; i < points; ++i) {
        double xi = i * (267.0 / (points - 1));
        double dt = xi - center;
        double val = (qAbs(dt) < width) ? 3.0 * (1.0 - qAbs(dt) / width) : 0.0;
        pts[xi] = qBound(0.0, val, 5.0);
    }
    pts[center] = 3.0;
    x = pts.keys().toVector();
    y = pts.values().toVector();
}
