#include "PulseWaveforms.h"
#include <QtMath>
#include <QRandomGenerator>
#include <QMap>
#include <algorithm>

// ===== TriggerPulseWaveform =====
void TriggerPulseWaveform::generate(QVector<double> &x, QVector<double> &y)
{
    QVector<double> pulsePos = {250, 1200, 2250};
    int n = 200;
    QVector<double> xAll, yAll;
    xAll.reserve(n + pulsePos.size());
    yAll.reserve(n + pulsePos.size());

    for (double pos : pulsePos) {
        xAll.append(pos);
        yAll.append(0.97);
    }

    for (int i = 0; i < n; ++i) {
        double xi = i * (3050.0 / (n - 1));
        bool nearPeak = false;
        for (double pos : pulsePos) {
            if (qAbs(xi - pos) < 1.5) { nearPeak = true; break; }
        }
        if (nearPeak) continue;

        double baseVal = 0.0;
        for (double pos : pulsePos) {
            double dt = xi - pos;
            if (dt >= 0 && dt < 300)
                baseVal = qMax(baseVal, 0.97 * qExp(-dt / 30.0));
        }
        double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * 0.02;
        xAll.append(xi);
        yAll.append(qBound(-0.1, baseVal + jitter, 1.3));
    }

    QMap<double, double> sorted;
    for (int i = 0; i < xAll.size(); ++i)
        sorted[xAll[i]] = yAll[i];

    x = sorted.keys().toVector();
    y = sorted.values().toVector();
}

// ===== PulseModulationWaveform =====
void PulseModulationWaveform::generate(QVector<double> &x, QVector<double> &y)
{
    int points = 200;
    x.resize(points);
    y.resize(points);

    QVector<QPair<double,double>> pulses = {{0, 100}, {1000, 1200}, {1900, 2100}};

    for (int i = 0; i < points; ++i) {
        x[i] = i * (3050.0 / (points - 1));

        bool inPulse = false;
        bool isRisingEdge = false;
        double pulseStart = 0.0;
        for (auto &p : pulses) {
            if (x[i] >= p.first && x[i] <= p.second) {
                inPulse = true;
                pulseStart = p.first;
                if (x[i] - p.first < 30.0) isRisingEdge = true;
                break;
            }
        }

        double baseVal;
        if (isRisingEdge) {
            double dt = x[i] - pulseStart;
            baseVal = 0.90 + 0.03 * qExp(-dt / 8.0);
        } else if (inPulse) {
            baseVal = 0.90;
        } else {
            baseVal = 0.06;
        }

        double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * 0.015;
        y[i] = qBound(-0.1, baseVal + jitter, 1.3);
    }
}

// ===== IntermediateFrequencyWaveform =====
void IntermediateFrequencyWaveform::generate(QVector<double> &x, QVector<double> &y)
{
    int points = 200;
    x.resize(points);
    y.resize(points);

    double T = 28.0;
    double phase = -M_PI / 2.0 + (2 * M_PI / T) * 7.0;

    for (int i = 0; i < points; ++i) {
        x[i] = i * (102.0 / (points - 1));
        double baseVal = 0.50 + 0.44 * qSin(2 * M_PI / T * x[i] - phase);
        double sinVal  = qSin(2 * M_PI / T * x[i] - phase);
        double noiseAmp = (sinVal > 0.85) ? 0.025 : 0.015;
        double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * noiseAmp;
        y[i] = qBound(-0.1, baseVal + jitter, 1.2);
    }
}

// ===== LocalOscillatorWaveform =====
void LocalOscillatorWaveform::generate(QVector<double> &x, QVector<double> &y)
{
    int points = 200;
    x.resize(points);
    y.resize(points);

    double centerFreq = 9.34;
    double noiseFloor = -135.0;
    double peakPower  = -43.0;

    for (int i = 0; i < points; ++i) {
        x[i] = 9.30 + i * (0.081 / (points - 1));
        double dist   = x[i] - centerFreq;
        double signal = noiseFloor + (peakPower - noiseFloor) * qExp(-qPow(dist / 0.003, 2));
        double noise  = (QRandomGenerator::global()->generateDouble() * 2 - 1) * 3.0;
        if (qAbs(dist) < 0.005) noise *= 0.1;
        y[i] = qBound(-140.0, signal + noise, 15.0);
    }
}

// ===== RFEmitWaveform =====
void RFEmitWaveform::generate(QVector<double> &x, QVector<double> &y)
{
    int points = 200;
    x.resize(points);
    y.resize(points);

    double centerFreq = 9.40;
    double noiseFloor = -93.0;
    double peakPower  = 9.0;

    for (int i = 0; i < points; ++i) {
        x[i] = 9.30 + i * (0.20 / (points - 1));
        double dist   = x[i] - centerFreq;
        double signal = noiseFloor + (peakPower - noiseFloor) * qExp(-qPow(dist / 0.005, 2));
        double noise  = (QRandomGenerator::global()->generateDouble() * 2 - 1) * 2.5;
        if (qAbs(dist) < 0.008) noise *= 0.1;
        y[i] = qBound(-100.0, signal + noise, 45.0);
    }
}

// ===== RFReceiveWaveform =====
void RFReceiveWaveform::generate(QVector<double> &x, QVector<double> &y)
{
    int points = 200;
    x.resize(points);
    y.resize(points);

    double centerFreq = 9.40;
    double noiseFloor = -93.0;
    double peakPower  = 9.0;

    for (int i = 0; i < points; ++i) {
        x[i] = 9.30 + i * (0.20 / (points - 1));
        double dist   = x[i] - centerFreq;
        double signal = noiseFloor + (peakPower - noiseFloor) * qExp(-qPow(dist / 0.005, 2));
        double noise  = (QRandomGenerator::global()->generateDouble() * 2 - 1) * 2.5;
        if (qAbs(dist) < 0.008) noise *= 0.1;
        y[i] = qBound(-100.0, signal + noise, 45.0);
    }
}

// ===== BasebandWaveform =====
void BasebandWaveform::generate(QVector<double> &x, QVector<double> &y)
{
    int points = 200;
    x.resize(points);
    y.resize(points);

    for (int i = 0; i < points; ++i) {
        x[i] = i * (8.0 / (points - 1));
        double t       = fmod(x[i], 2.0);
        double baseVal = (t < 1.0) ? 0.95 : 0.05;
        double jitter  = (QRandomGenerator::global()->generateDouble() * 2 - 1) * 0.02;
        y[i] = baseVal + jitter;
    }
}

// ===== DetectorWaveform =====
void DetectorWaveform::generate(QVector<double> &x, QVector<double> &y)
{
    int points = 200;
    x.resize(points);
    y.resize(points);

    for (int i = 0; i < points; ++i) {
        x[i] = i * (50.0 / (points - 1));
        double baseVal  = 0.04 + 0.91 * qExp(-qPow((x[i] - 25.0) / 5.0, 2));
        double noiseAmp = (baseVal > 0.3) ? 0.008 : 0.018;
        double jitter   = (QRandomGenerator::global()->generateDouble() * 2 - 1) * noiseAmp;
        y[i] = qBound(0.0, baseVal + jitter, 1.0);
    }
}

// ===== ThresholdWaveform =====
void ThresholdWaveform::generate(QVector<double> &x, QVector<double> &y)
{
    int points = 200;
    x.resize(points);
    y.resize(points);

    for (int i = 0; i < points; ++i) {
        x[i] = i * (101.0 / (points - 1));
        double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * 0.012;
        y[i] = 0.50 + jitter;
    }
}

// ===== BranchWaveform =====
void BranchWaveform::generate(QVector<double> &x, QVector<double> &y)
{
    int points = 200;
    x.resize(points);
    y.resize(points);

    for (int i = 0; i < points; ++i) {
        x[i] = i * (80.0 / (points - 1));
        double baseVal  = 0.30 + 0.62 * qExp(-qPow((x[i] - 40.0) / 7.0, 2));
        double noiseAmp = (baseVal > 0.5) ? 0.010 : 0.018;
        double jitter   = (QRandomGenerator::global()->generateDouble() * 2 - 1) * noiseAmp;
        y[i] = qBound(0.0, baseVal + jitter, 1.0);
    }
}

// ===== DifferentialWaveform =====
void DifferentialWaveform::generate(QVector<double> &x, QVector<double> &y)
{
    int points = 200;
    x.resize(points);
    y.resize(points);

    double center = 41.0;
    double sigma  = 7.5;

    for (int i = 0; i < points; ++i) {
        x[i] = i * (80.0 / (points - 1));
        double t             = x[i] - center;
        double gaussian_diff = -(t / (sigma * sigma))
                               * qExp(-qPow(t, 2) / (2.0 * sigma * sigma));
        double baseVal  = gaussian_diff * 3.0;
        double noiseAmp = (qAbs(baseVal) > 0.03) ? 0.006 : 0.003;
        double jitter   = (QRandomGenerator::global()->generateDouble() * 2 - 1) * noiseAmp;
        y[i] = qBound(-0.4, baseVal + jitter, 0.4);
    }
}

// ===== TributaryWaveform =====
void TributaryWaveform::generate(QVector<double> &x, QVector<double> &y)
{
    int points = 200;
    x.resize(points);
    y.resize(points);

    for (int i = 0; i < points; ++i) {
        x[i] = i * (80.0 / (points - 1));
        double baseVal  = 0.72 * qExp(-qPow((x[i] - 40.0) / 6.0, 2)) + 0.008;
        double noiseAmp = (baseVal > 0.05) ? 0.008 : 0.012;
        double jitter   = (QRandomGenerator::global()->generateDouble() * 2 - 1) * noiseAmp;
        y[i] = qBound(0.0, baseVal + jitter, 0.8);
    }
}

// ===== DistancePulseWaveform（原理演示动画完成后）=====
void DistancePulseWaveform::generate(QVector<double> &x, QVector<double> &y)
{
    double peakTime = m_peakTimeUs;
    int n = 2000;

    QVector<QPair<double,double>> pts;
    pts.append({peakTime, 0.58});

    for (int i = 0; i < n; ++i) {
        double xi = i * (1000.0 / (n - 1));
        if (qAbs(xi - peakTime) < 0.5) continue;
        double dt      = xi - peakTime;
        double baseVal = 0.0;
        if (qAbs(dt) < 300)
            baseVal = 0.58 * qExp(-dt * dt / (2.0 * 15.0 * 15.0));
        double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * 0.008;
        pts.append({xi, qBound(0.0, baseVal + jitter, 1.0)});
    }

    std::sort(pts.begin(), pts.end(),
              [](const QPair<double,double> &a, const QPair<double,double> &b){
                  return a.first < b.first;
              });

    x.resize(pts.size());
    y.resize(pts.size());
    for (int i = 0; i < pts.size(); ++i) {
        x[i] = pts[i].first;
        y[i] = pts[i].second;
    }
}

// ===== DistanceUsWaveform（测试验证用户取得距离后）=====
void DistanceUsWaveform::generate(QVector<double> &x, QVector<double> &y)
{
    double peakTime = m_peakTimeUs;
    double xMax     = qMax(peakTime * 1.5, 4.0);
    int n = 2000;

    QVector<QPair<double,double>> pts;
    pts.append({peakTime, 0.58});

    for (int i = 0; i < n; ++i) {
        double xi = i * (xMax / (n - 1));
        if (qAbs(xi - peakTime) < xMax * 0.0005) continue;
        double dt      = xi - peakTime;
        double sigma   = xMax * 0.008;
        double baseVal = 0.0;
        if (qAbs(dt) < xMax * 0.3)
            baseVal = 0.58 * qExp(-dt * dt / (2.0 * sigma * sigma));
        double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * 0.008;
        pts.append({xi, qBound(0.0, baseVal + jitter, 1.0)});
    }

    std::sort(pts.begin(), pts.end(),
              [](const QPair<double,double> &a, const QPair<double,double> &b){
                  return a.first < b.first;
              });

    x.resize(pts.size());
    y.resize(pts.size());
    for (int i = 0; i < pts.size(); ++i) {
        x[i] = pts[i].first;
        y[i] = pts[i].second;
    }
}
