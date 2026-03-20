#include "oscilloscopewidget.h"
#include <QVBoxLayout>

OscilloscopeWidget::OscilloscopeWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    m_plot = new QCustomPlot(this);
    layout->addWidget(m_plot);
    layout->setContentsMargins(0,0,0,0);

    setupPlot();

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &OscilloscopeWidget::onRefreshTick);
    m_timer->start(30); // 30ms 刷新实现平滑抖动
}

void OscilloscopeWidget::setupPlot() {
    m_graph = m_plot->addGraph();
    // 波形颜色建议使用深一点的绿色或蓝色，在白底上更有质感
    m_graph->setPen(QPen(QColor(0, 150, 80), 2));
    // 1. 设置背景颜色为白色
    m_plot->setBackground(QBrush(Qt::white));
    // 2. 将坐标轴线条改为灰色
    QPen pen(Qt::lightGray);
    m_plot->xAxis->setBasePen(pen);
    m_plot->yAxis->setBasePen(pen);
    m_plot->xAxis->setTickPen(pen);
    m_plot->yAxis->setTickPen(pen);
    m_plot->xAxis->setSubTicks(false);
    m_plot->yAxis->setSubTicks(false);
    m_plot->xAxis2->setVisible(true);  // 显示上侧X轴
    m_plot->yAxis2->setVisible(true);  // 显示右侧Y轴
    m_plot->xAxis2->setTickLabels(false);  // 上轴不显示文字（避免重复）
    m_plot->yAxis2->setTickLabels(false);  // 右轴不显示文字（避免重复）
    m_plot->xAxis2->setTicks(false);       // 上轴不显示刻度线（可选，更干净）
    m_plot->yAxis2->setTicks(false);       // 右轴不显示刻度线（可选，更干净）
    m_plot->xAxis2->setBasePen(pen);   // 上轴线和主X轴一样
    m_plot->yAxis2->setBasePen(pen);   // 右轴线和主Y轴一样
}

void OscilloscopeWidget::setData(const QString& stepId) {
    if (stepId == "s2") m_currentMode = TriggerPulse;
    else if (stepId == "s3") m_currentMode = PulseModulation;
    else if (stepId == "s4") m_currentMode = IntermediateFrequency;
    else if (stepId == "s5") m_currentMode = LocalOscillator;
    else if (stepId == "s6") m_currentMode = RFEmit;
    else if (stepId == "s7") m_currentMode = RFReceive;
    else if (stepId == "s8") m_currentMode = Baseband;
    else if (stepId == "s9") m_currentMode = Detector;
    else if (stepId == "s10") m_currentMode = Threshold;
    else if (stepId == "s11") m_currentMode = Branch;
    else if (stepId == "s12") m_currentMode = Differential;
    else if (stepId == "s13") m_currentMode = Tributary;
    else if (stepId == "s14") m_currentMode = Distance;
    else m_currentMode = None;
}

void OscilloscopeWidget::onRefreshTick()
{
    // 清除Distance添加的额外graph，只保留graph(0)
    while (m_plot->graphCount() > 1)
        m_plot->removeGraph(m_plot->graphCount() - 1);
    // 每次刷新前重置ticker为默认，防止case之间互相污染
    m_plot->xAxis->setTicker(QSharedPointer<QCPAxisTicker>(new QCPAxisTicker));
    m_plot->yAxis->setTicker(QSharedPointer<QCPAxisTicker>(new QCPAxisTicker));
    m_graph->setBrush(Qt::NoBrush); // 加这一行
    int points = 200;
    QVector<double> x(points), y(points);

    switch (m_currentMode)
    {
    case None:
    {
        m_graph->data()->clear(); // 清空数据
        m_plot->xAxis->setLabel("时间 (µs)");
        m_plot->yAxis->setLabel("电压 (V)");
        m_plot->xAxis->setRange(0, 3050);
        m_plot->yAxis->setRange(-0.1, 1.3);
        m_plot->yAxis->setNumberFormat("f");
        m_plot->yAxis->setNumberPrecision(1);
        QSharedPointer<QCPAxisTickerText> yTicker(new QCPAxisTickerText);
        QVector<double> yTicks = {-0.1, 0.1, 0.3, 0.5, 0.7, 0.9, 1.1, 1.3};
        for (double v : yTicks)
            yTicker->addTick(v, QString::number(v, 'f', 1));
        m_plot->yAxis->setTicker(yTicker);

        m_plot->replot();
        return; // 跳过后面的 setData 和 replot
    }
    case TriggerPulse:
    {
        m_plot->xAxis->setLabel("时间(µs)");
        m_plot->yAxis->setLabel("电压(V)");
        m_plot->xAxis->setRange(0, 3050);
        m_plot->yAxis->setRange(-0.1, 1.3);
        m_plot->yAxis->setNumberFormat("f");
        m_plot->yAxis->setNumberPrecision(1);
        QSharedPointer<QCPAxisTickerText> yTicker(new QCPAxisTickerText);
        QVector<double> yTicks = {-0.1, 0.1, 0.3, 0.5, 0.7, 0.9, 1.1, 1.3};
        for (double v : yTicks)
            yTicker->addTick(v, QString::number(v, 'f', 1));
        m_plot->yAxis->setTicker(yTicker);
        QVector<double> pulsePos = {250, 1200, 2250};
        // 先生成均匀点，再插入峰顶点
        int n = 200;
        QVector<double> xAll, yAll;
        xAll.reserve(n + pulsePos.size());
        yAll.reserve(n + pulsePos.size());
        // 强制插入峰顶点
        for (double pos : pulsePos) {
            xAll.append(pos);
            yAll.append(0.97); // 峰顶强制赋值
        }
        // 均匀采样点
        for (int i = 0; i < n; ++i) {
            double xi = i * (3050.0 / (n - 1));
            // 跳过已经插入的峰顶附近（避免重复）
            bool nearPeak = false;
            for (double pos : pulsePos) {
                if (qAbs(xi - pos) < 1.5) { nearPeak = true; break; }
            }
            if (nearPeak) continue;
            double baseVal = 0.0;
            for (double pos : pulsePos) {
                double dt = xi - pos;
                if (dt >= 0 && dt < 300) {
                    baseVal = qMax(baseVal, 0.97 * qExp(-dt / 30.0));
                }
            }
            double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * 0.02;
            xAll.append(xi);
            yAll.append(qBound(-0.1, baseVal + jitter, 1.3));
        }
        // 按X排序后设置数据
        // 用 QMap 自动排序
        QMap<double, double> sorted;
        for (int i = 0; i < xAll.size(); ++i)
            sorted[xAll[i]] = yAll[i];

        x = sorted.keys().toVector();
        y = sorted.values().toVector();

        break;
    }
    case PulseModulation:
    {
        m_plot->xAxis->setLabel("时间(µs)");
        m_plot->yAxis->setLabel("电压(V)");
        m_plot->xAxis->setRange(0, 3050);
        m_plot->yAxis->setRange(-0.1, 1.3);
        m_plot->yAxis->setNumberFormat("f");
        m_plot->yAxis->setNumberPrecision(1);

        // 手动指定Y轴刻度，强制显示-0.1
        QSharedPointer<QCPAxisTickerText> yTicker(new QCPAxisTickerText);
        QVector<double> yTicks = {-0.1, 0.1, 0.3, 0.5, 0.7, 0.9, 1.1, 1.3};
        for (double v : yTicks)
            yTicker->addTick(v, QString::number(v, 'f', 1));
        m_plot->yAxis->setTicker(yTicker);


        // 脉冲区间 [起始, 结束]
        QVector<QPair<double,double>> pulses = {{0, 100}, {1000, 1200}, {1900, 2100}};

        for (int i = 0; i < points; ++i) {
            x[i] = i * (3050.0 / (points - 1));

            bool inPulse = false;
            bool isRisingEdge = false;
            for (auto& p : pulses) {
                if (x[i] >= p.first && x[i] <= p.second) {
                    inPulse = true;
                    // 上升沿过冲区域
                    if (x[i] - p.first < 30.0) isRisingEdge = true;
                    break;
                }
            }

            double baseVal;
            if (isRisingEdge) {
                // 上升沿尖峰：快速冲到0.93再回落到0.90
                double dt = x[i] - [&](){
                    for (auto& p : pulses) if (x[i] >= p.first && x[i] <= p.second) return p.first;
                    return 0.0;
                }();
                baseVal = 0.90 + 0.03 * qExp(-dt / 8.0);
            } else if (inPulse) {
                baseVal = 0.90;
            } else {
                baseVal = 0.06;
            }

            double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * 0.015;
            y[i] = qBound(-0.1, baseVal + jitter, 1.3);
        }
        break;
    }
    case IntermediateFrequency:
    {
        m_plot->xAxis->setLabel("时间(µs)");
        m_plot->yAxis->setLabel("电压(V)");
        m_plot->xAxis->setRange(0, 102);
        m_plot->yAxis->setRange(-0.1, 1.3);
        m_plot->yAxis->setNumberFormat("f");
        m_plot->yAxis->setNumberPrecision(1);

        // 手动指定Y轴刻度
        QSharedPointer<QCPAxisTickerText> yTicker(new QCPAxisTickerText);
        QVector<double> yTicks = {-0.1, 0.1, 0.3, 0.5, 0.7, 0.9, 1.1, 1.3};
        for (double v : yTicks)
            yTicker->addTick(v, QString::number(v, 'f', 1));
        m_plot->yAxis->setTicker(yTicker);

        // 4个峰顶位置约在 7, 35, 63, 92µs，周期约28µs
        // 从相位-π/2开始，让第一个峰在x≈7附近
        double T = 28.0;  // 周期28µs
        double phase = -M_PI / 2.0 + (2 * M_PI / T) * 7.0; // 相位偏移让峰顶对齐

        for (int i = 0; i < points; ++i) {
            x[i] = i * (102.0 / (points - 1));

            double baseVal = 0.50 + 0.44 * qSin(2 * M_PI / T * x[i] - phase);

            double sinVal = qSin(2 * M_PI / T * x[i] - phase);
            double noiseAmp = (sinVal > 0.85) ? 0.025 : 0.015;
            double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * noiseAmp;

            y[i] = qBound(-0.1, baseVal + jitter, 1.2);
        }
        break;
    }
    case LocalOscillator:
    {
        m_graph->setBrush(Qt::NoBrush);

        m_plot->xAxis->setLabel("频率 (GHz)");
        m_plot->yAxis->setLabel("功率 (dBm)");
        m_plot->xAxis->setRange(9.3, 9.381); // 两端加余量确保9.30和9.38都显示

        QSharedPointer<QCPAxisTickerFixed> xTicker(new QCPAxisTickerFixed);
        xTicker->setTickStep(0.01);
        xTicker->setScaleStrategy(QCPAxisTickerFixed::ssNone);
        m_plot->xAxis->setTicker(xTicker);

        m_plot->yAxis->setRange(-140, 15);
        QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
        QVector<double> yTicks = {15, -4, -24, -43, -63, -82, -101, -121, -140};
        for (double v : yTicks)
            textTicker->addTick(v, QString::number(v));
        m_plot->yAxis->setTicker(textTicker);

        double centerFreq = 9.34;
        double noiseFloor = -135.0;
        double peakPower  = -43.0;

        for (int i = 0; i < points; ++i) {
            x[i] = 9.30 + i * (0.081 / (points - 1));

            double dist = x[i] - centerFreq;
            double signal = noiseFloor + (peakPower - noiseFloor)
                                             * qExp(-qPow(dist / 0.003, 2));

            double noise = (QRandomGenerator::global()->generateDouble() * 2 - 1) * 3.0;
            if (qAbs(dist) < 0.005)
                noise *= 0.1;

            y[i] = qBound(-140.0, signal + noise, 15.0);
        }
        break;
    }
    case RFEmit:
    {
        m_graph->setBrush(Qt::NoBrush);

        m_plot->xAxis->setLabel("频率 (GHz)");
        m_plot->yAxis->setLabel("功率 (dBm)");
        m_plot->xAxis->setRange(9.30, 9.50);
        m_plot->yAxis->setRange(-100, 45);

        // 自定义Y轴非均匀刻度
        QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
        QVector<double> yTicks = {45, 27, 9, -9, -28, -46, -64, -82, -100};
        for (double v : yTicks)
            textTicker->addTick(v, QString::number(v));
        m_plot->yAxis->setTicker(textTicker);

        double centerFreq = 9.40;
        double noiseFloor = -93.0;
        double peakPower  = 9.0;

        for (int i = 0; i < points; ++i) {
            x[i] = 9.30 + i * (0.20 / (points - 1));

            double dist = x[i] - centerFreq;
            // 峰比LocalOscillator稍宽，sigma大一点
            double signal = noiseFloor + (peakPower - noiseFloor) * qExp(-qPow(dist / 0.005, 2));
            double noise = (QRandomGenerator::global()->generateDouble() * 2 - 1) * 2.5;

            if (qAbs(dist) < 0.008)
                noise *= 0.1;

            y[i] = qBound(-100.0, signal + noise, 45.0);
        }
        break;
    }
    case RFReceive:
    {
        m_graph->setBrush(Qt::NoBrush);

        m_plot->xAxis->setLabel("频率 (GHz)");
        m_plot->yAxis->setLabel("功率 (dBm)");
        m_plot->xAxis->setRange(9.30, 9.50);
        m_plot->yAxis->setRange(-100, 45);

        // 自定义Y轴非均匀刻度
        QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
        QVector<double> yTicks = {45, 27, 9, -9, -28, -46, -64, -82, -100};
        for (double v : yTicks)
            textTicker->addTick(v, QString::number(v));
        m_plot->yAxis->setTicker(textTicker);

        double centerFreq = 9.40;
        double noiseFloor = -93.0;
        double peakPower  = 9.0;

        for (int i = 0; i < points; ++i) {
            x[i] = 9.30 + i * (0.20 / (points - 1));

            double dist = x[i] - centerFreq;
            // 峰比LocalOscillator稍宽，sigma大一点
            double signal = noiseFloor + (peakPower - noiseFloor)
                                             * qExp(-qPow(dist / 0.005, 2));

            double noise = (QRandomGenerator::global()->generateDouble() * 2 - 1) * 2.5;

            if (qAbs(dist) < 0.008)
                noise *= 0.1;

            y[i] = qBound(-100.0, signal + noise, 45.0);
        }
        break;
    }
    case Baseband:
    {
        m_graph->setBrush(Qt::NoBrush);

        m_plot->xAxis->setLabel("时间 (µs)");
        m_plot->yAxis->setLabel("电压 (V)");
        m_plot->xAxis->setRange(0, 8);
        m_plot->yAxis->setRange(0, 1.0);
        m_plot->yAxis->setNumberFormat("f");
        m_plot->yAxis->setNumberPrecision(1);

        QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
        fixedTicker->setTickStep(0.1);
        m_plot->yAxis->setTicker(fixedTicker);
        QSharedPointer<QCPAxisTickerFixed> fixedTickerx(new QCPAxisTickerFixed);
        fixedTickerx->setTickStep(1);
        m_plot->xAxis->setTicker(fixedTickerx);

        for (int i = 0; i < points; ++i) {
            x[i] = i * (8.0 / (points - 1));
            // 周期2us，占空比50%：0~1高，1~2低，2~3高...
            double t = fmod(x[i], 2.0);
            double baseVal = (t < 1.0) ? 0.95 : 0.05;
            double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * 0.02;
            y[i] = baseVal + jitter;
        }
        break;
    }
    case Detector:
    {
        m_graph->setBrush(Qt::NoBrush);

        m_plot->xAxis->setLabel("时间 (µs)");
        m_plot->yAxis->setLabel("电压 (V)");
        m_plot->xAxis->setRange(0, 50);
        m_plot->yAxis->setRange(0, 1.0);
        m_plot->yAxis->setNumberFormat("f");
        m_plot->yAxis->setNumberPrecision(1);

        QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
        fixedTicker->setTickStep(0.1);
        m_plot->yAxis->setTicker(fixedTicker);

        for (int i = 0; i < points; ++i) {
            x[i] = i * (50.0 / (points - 1));

            // 高斯包络，中心25µs，sigma≈5
            double baseVal = 0.04 + 0.91 * qExp(-qPow((x[i] - 25.0) / 5.0, 2));

            // 峰值附近噪声小，基线噪声稍大
            double noiseAmp = (baseVal > 0.3) ? 0.008 : 0.018;
            double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * noiseAmp;

            y[i] = qBound(0.0, baseVal + jitter, 1.0);
        }
        break;
    }
    case Threshold:
    {
        m_graph->setBrush(Qt::NoBrush);

        m_plot->xAxis->setLabel("时间 (µs)");
        m_plot->yAxis->setLabel("电压 (V)");
        m_plot->xAxis->setRange(0, 101);
        m_plot->yAxis->setRange(0, 1.0);
        m_plot->yAxis->setNumberFormat("f");
        m_plot->yAxis->setNumberPrecision(1);

        QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
        fixedTicker->setTickStep(0.1);
        m_plot->yAxis->setTicker(fixedTicker);

        for (int i = 0; i < points; ++i) {
            x[i] = i * (101.0 / (points - 1));
            double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * 0.012;
            y[i] = 0.50 + jitter;
        }
        break;
    }
    case Branch:
    {
        m_graph->setBrush(Qt::NoBrush);

        m_plot->xAxis->setLabel("时间 (µs)");
        m_plot->yAxis->setLabel("电压 (V)");
        m_plot->xAxis->setRange(0, 80);
        m_plot->yAxis->setRange(0, 1.0);
        m_plot->yAxis->setNumberFormat("f");
        m_plot->yAxis->setNumberPrecision(1);

        QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
        fixedTicker->setTickStep(0.1);
        m_plot->yAxis->setTicker(fixedTicker);

        for (int i = 0; i < points; ++i) {
            x[i] = i * (80.0 / (points - 1));

            // 高斯包络，基线0.30，峰值0.92，中心40µs，sigma≈7
            double baseVal = 0.30 + 0.62 * qExp(-qPow((x[i] - 40.0) / 7.0, 2));

            double noiseAmp = (baseVal > 0.5) ? 0.010 : 0.018;
            double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * noiseAmp;

            y[i] = qBound(0.0, baseVal + jitter, 1.0);
        }
        break;
    }
    case Differential:
    {
        m_plot->xAxis->setLabel("时间 (µs)");
        m_plot->yAxis->setLabel("电压 (V)");
        m_plot->xAxis->setRange(0, 80);
        m_plot->yAxis->setRange(-0.4, 0.4);
        m_plot->yAxis->setNumberFormat("f");
        m_plot->yAxis->setNumberPrecision(1);

        QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
        fixedTicker->setTickStep(0.1);
        m_plot->yAxis->setTicker(fixedTicker);

        // 正峰34µs，负峰48µs，两峰间距14µs
        // 高斯导数正负峰间距 = sigma*sqrt(2)，所以 sigma = 14/sqrt(2) ≈ 7.5
        // 高斯中心在两峰中间：(34+48)/2 = 41µs
        double center = 41.0;
        double sigma  = 7.5;

        for (int i = 0; i < points; ++i) {
            x[i] = i * (80.0 / (points - 1));

            double t = x[i] - center;
            double gaussian_diff = -(t / (sigma * sigma))
                                   * qExp(-qPow(t, 2) / (2.0 * sigma * sigma));

            // 峰值目标±0.14，gaussian_diff峰值=1/(sigma*sqrt(e))≈0.049
            // 缩放系数 = 0.14 / 0.049 ≈ 2.85
            double baseVal = gaussian_diff * 3.0;

            double noiseAmp = (qAbs(baseVal) > 0.03) ? 0.006 : 0.003;
            double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * noiseAmp;

            y[i] = qBound(-0.4, baseVal + jitter, 0.4);
        }
        break;
    }
    case Tributary:
    {
        m_graph->setBrush(Qt::NoBrush);

        m_plot->xAxis->setLabel("时间 (µs)");
        m_plot->yAxis->setLabel("电压 (V)");
        m_plot->xAxis->setRange(0, 80);
        m_plot->yAxis->setRange(0, 0.8);  // 范围改大
        m_plot->yAxis->setNumberFormat("f");
        m_plot->yAxis->setNumberPrecision(1);

        QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
        fixedTicker->setTickStep(0.1);
        m_plot->yAxis->setTicker(fixedTicker);

        for (int i = 0; i < points; ++i) {
            x[i] = i * (80.0 / (points - 1));

            double baseVal = 0.72 * qExp(-qPow((x[i] - 40.0) / 6.0, 2)) + 0.008; // 峰值改为0.72

            double noiseAmp = (baseVal > 0.05) ? 0.008 : 0.012;
            double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * noiseAmp;

            y[i] = qBound(0.0, baseVal + jitter, 0.8);
        }
        break;
    }
    // case Distance:
    // {
    //     m_graph->setBrush(Qt::NoBrush);

    //     m_plot->xAxis->setLabel("时间 (µs)");
    //     m_plot->yAxis->setLabel("电压 (V)");
    //     m_plot->xAxis->setRange(0, 1050);
    //     m_plot->yAxis->setRange(0, 1.0);
    //     m_plot->yAxis->setNumberFormat("f");
    //     m_plot->yAxis->setNumberPrecision(1);

    //     QSharedPointer<QCPAxisTickerFixed> xTicker(new QCPAxisTickerFixed);
    //     xTicker->setTickStep(100);
    //     xTicker->setScaleStrategy(QCPAxisTickerFixed::ssNone);
    //     m_plot->xAxis->setTicker(xTicker);
    //     m_plot->xAxis->setRange(0, 1050);

    //     QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
    //     fixedTicker->setTickStep(0.1);
    //     m_plot->yAxis->setTicker(fixedTicker);

    //     // ===== graph(0)：绿色尖峰脉冲 =====
    //     m_graph->setPen(QPen(QColor(0, 150, 80), 2)); // 保持setupPlot的绿色

    //     int n = 2000;
    //     x.resize(n);
    //     y.resize(n);

    //     // 强制插入峰顶点
    //     QVector<QPair<double,double>> pts;
    //     pts.append({400.0, 0.58}); // 峰顶强制插入

    //     for (int i = 0; i < n; ++i) {
    //         double xi = i * (1050.0 / (n - 1));
    //         if (qAbs(xi - 400.0) < 0.5) continue; // 跳过峰顶附近避免重复

    //         double baseVal = 0.0;
    //         double dt = xi - 400.0;
    //         if (dt >= 0 && dt < 200)
    //             baseVal = 0.58 * qExp(-dt / 15.0);
    //         else if (dt < 0 && dt > -200)
    //             baseVal = 0.58 * qExp(dt / 15.0); // 上升沿对称

    //         double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * 0.008;
    //         pts.append({xi, qBound(0.0, baseVal + jitter, 1.0)});
    //     }

    //     std::sort(pts.begin(), pts.end(), [](const QPair<double,double>& a, const QPair<double,double>& b){
    //         return a.first < b.first;
    //     });

    //     x.resize(pts.size());
    //     y.resize(pts.size());
    //     for (int i = 0; i < pts.size(); ++i) {
    //         x[i] = pts[i].first;
    //         y[i] = pts[i].second;
    //     }
    //     m_graph->setData(x, y);

    //     // ===== graph(1)：黄色垂直触发线 =====
    //     if (m_plot->graphCount() < 2)
    //         m_plot->addGraph();
    //     m_plot->graph(1)->setPen(QPen(QColor(255, 200, 0), 1, Qt::DashLine));
    //     m_plot->graph(1)->setBrush(Qt::NoBrush);
    //     QVector<double> lx = {400.0, 400.0};
    //     QVector<double> ly = {0.0, 1.0};
    //     m_plot->graph(1)->setData(lx, ly);

    //     // ===== graph(2)：黄色斜线（距离扫描） =====
    //     if (m_plot->graphCount() < 3)
    //         m_plot->addGraph();
    //     m_plot->graph(2)->setPen(QPen(QColor(255, 200, 0), 2));
    //     m_plot->graph(2)->setBrush(Qt::NoBrush);
    //     QVector<double> sx = {0.0, 100.0};
    //     QVector<double> sy = {0.58, 0.0}; // 从(0,0.58)线性降到(100,0)
    //     m_plot->graph(2)->setData(sx, sy);

    //     m_plot->replot();
    //     return; // 已经手动replot，跳过外层
    // }
    case Distance:
    {
        m_graph->setBrush(Qt::NoBrush);

        m_plot->xAxis->setLabel("时间 (µs)");
        m_plot->yAxis->setLabel("电压 (V)");
        m_plot->xAxis->setRange(0, 1000);
        m_plot->yAxis->setRange(0, 1.0);
        m_plot->yAxis->setNumberFormat("f");
        m_plot->yAxis->setNumberPrecision(1);

        QSharedPointer<QCPAxisTickerFixed> xTicker(new QCPAxisTickerFixed);
        xTicker->setTickStep(100);
        xTicker->setScaleStrategy(QCPAxisTickerFixed::ssNone);
        m_plot->xAxis->setTicker(xTicker);
        m_plot->axisRect()->setMinimumMargins(QMargins(0, 0, 20, 0));

        double peakTime = m_distancePulseTime; // 目标时间点(µs)

        // ===== graph(0)：绿色尖峰脉冲 =====
        QVector<QPair<double,double>> pts;
        pts.append({peakTime, 0.58}); // 强制插入峰顶

        int n = 2000;
        for (int i = 0; i < n; ++i) {
            double xi = i * (1000.0 / (n - 1));
            if (qAbs(xi - peakTime) < 0.5) continue;

            double dt = xi - peakTime;
            double baseVal = 0.0;
            if (qAbs(dt) < 300)
                baseVal = 0.58 * qExp(-dt * dt / (2.0 * 15.0 * 15.0));

            double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * 0.008;
            pts.append({xi, qBound(0.0, baseVal + jitter, 1.0)});
        }

        std::sort(pts.begin(), pts.end(), [](const QPair<double,double>& a, const QPair<double,double>& b){
            return a.first < b.first;
        });

        x.resize(pts.size());
        y.resize(pts.size());
        for (int i = 0; i < pts.size(); ++i) {
            x[i] = pts[i].first;
            y[i] = pts[i].second;
        }
        m_graph->setData(x, y);

        // ===== graph(1)：黄色垂直虚线 =====
        if (m_plot->graphCount() < 2) m_plot->addGraph();
        m_plot->graph(1)->setPen(QPen(QColor(255, 200, 0), 1, Qt::DashLine));
        m_plot->graph(1)->setBrush(Qt::NoBrush);
        m_plot->graph(1)->setData({peakTime, peakTime}, {0.0, 1.0});

        // ===== graph(2)：黄色斜线 =====
        if (m_plot->graphCount() < 3) m_plot->addGraph();
        m_plot->graph(2)->setPen(QPen(QColor(255, 200, 0), 2));
        m_plot->graph(2)->setBrush(Qt::NoBrush);
        m_plot->graph(2)->setData({0.0, 100.0}, {0.58, 0.0}); // 固定终点1000µs

        m_plot->replot();
        return;
    }
    default:
        return;
    }

    m_graph->setData(x, y);
    m_plot->replot();
}

void OscilloscopeWidget::onRadarAnimationFinished(double pulseTimeUs)
{
    m_distancePulseTime = pulseTimeUs;
    m_currentMode = Distance;
    onRefreshTick(); // 立即触发一次绘制
}
