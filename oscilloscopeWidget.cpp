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
    else m_currentMode = None;
}

void OscilloscopeWidget::onRefreshTick()
{
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
        m_graph->setBrush(Qt::NoBrush);
        m_graph->data()->clear(); // 清空数据，不画任何波形

        m_plot->xAxis->setLabel("时间 (µs)");
        m_plot->yAxis->setLabel("电压 (V)");
        m_plot->xAxis->setRange(0, 3000);
        m_plot->yAxis->setRange(-0.1, 1.2);
        m_plot->yAxis->setNumberFormat("f");
        m_plot->yAxis->setNumberPrecision(1);

        QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
        fixedTicker->setTickStep(0.1);
        m_plot->yAxis->setTicker(fixedTicker);

        m_plot->xAxis->setTicker(QSharedPointer<QCPAxisTicker>(new QCPAxisTicker));

        m_plot->replot();
        return; // 不执行后面的 setData 和 replot
    }
    case TriggerPulse:
    {
        m_plot->xAxis->setLabel("时间(µs)");
        m_plot->yAxis->setLabel("电压(V)");
        m_plot->xAxis->setRange(0, 3100);
        m_plot->yAxis->setRange(-0.1, 1.3);
        m_plot->yAxis->setNumberFormat("f");
        m_plot->yAxis->setNumberPrecision(1);

        QVector<double> pulsePos = {250, 1200, 2250}; // 三个脉冲中心位置

        for (int i = 0; i < points; ++i) {
            x[i] = i * (3000.0 / (points - 1));

            double baseVal = 0.0;
            for (double pos : pulsePos) {
                double dt = x[i] - pos;
                if (dt >= 0 && dt < 200) {
                    // 快速上升 + 指数衰减
                    double pulse = 0.95 * qExp(-dt / 20.0);
                    baseVal = qMax(baseVal, pulse);
                }
            }

            double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * 0.02;
            y[i] = baseVal + jitter;
        }
        break;
    }
    case PulseModulation:
    {
        m_plot->xAxis->setLabel("时间(µs)");
        m_plot->yAxis->setLabel("电压(V)");
        m_plot->xAxis->setRange(0, 3100);
        m_plot->yAxis->setRange(-0.1, 1.3);
        m_plot->yAxis->setNumberFormat("f");
        m_plot->yAxis->setNumberPrecision(1);

        // 脉冲区间 [起始, 结束]
        QVector<QPair<double,double>> pulses = {{0, 100}, {1000, 1200}, {1900, 2100}};

        for (int i = 0; i < points; ++i) {
            x[i] = i * (3000.0 / (points - 1));

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
        m_plot->xAxis->setRange(0, 100);
        m_plot->yAxis->setRange(-0.1, 1.2);
        m_plot->yAxis->setNumberFormat("f");
        m_plot->yAxis->setNumberPrecision(1);

        for (int i = 0; i < points; ++i) {
            x[i] = i * (100.0 / (points - 1));

            // 周期22µs，中心0.50，幅度0.44
            double baseVal = 0.50 + 0.44 * qSin(2 * M_PI / 22.0 * x[i]);

            // 峰顶附近加小过冲
            double sinVal = qSin(2 * M_PI / 22.0 * x[i]);
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
        m_plot->xAxis->setRange(9.3, 9.385); // 两端加余量确保9.30和9.38都显示

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
            x[i] = 9.30 + i * (0.08 / (points - 1));

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
            double signal = noiseFloor + (peakPower - noiseFloor)
                                             * qExp(-qPow(dist / 0.005, 2));

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
        // 恢复默认均匀刻度（避免上个case的TextTicker残留）
        //m_plot->yAxis->setTicker(QSharedPointer<QCPAxisTicker>(new QCPAxisTicker));
        //m_plot->yAxis->setTickStep(0.1); // 每隔0.1一个刻度
        QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
        fixedTicker->setTickStep(0.1);
        m_plot->yAxis->setTicker(fixedTicker);

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
        m_plot->xAxis->setRange(0, 100);
        m_plot->yAxis->setRange(0, 1.0);
        m_plot->yAxis->setNumberFormat("f");
        m_plot->yAxis->setNumberPrecision(1);

        QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
        fixedTicker->setTickStep(0.1);
        m_plot->yAxis->setTicker(fixedTicker);

        for (int i = 0; i < points; ++i) {
            x[i] = i * (100.0 / (points - 1));
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
        m_graph->setBrush(Qt::NoBrush);

        m_plot->xAxis->setLabel("时间 (µs)");
        m_plot->yAxis->setLabel("电压 (V)");
        m_plot->xAxis->setRange(0, 80);
        m_plot->yAxis->setRange(-0.4, 0.4);
        m_plot->yAxis->setNumberFormat("f");
        m_plot->yAxis->setNumberPrecision(1);

        QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
        fixedTicker->setTickStep(0.1);
        m_plot->yAxis->setTicker(fixedTicker);

        double center = 40.0; // 高斯中心
        double sigma  = 6.0;  // 宽度

        for (int i = 0; i < points; ++i) {
            x[i] = i * (80.0 / (points - 1));

            // 高斯一阶导数：-((t-u)/sigma^2) * exp(-((t-u)^2)/(2*sigma^2))
            double t = x[i] - center;
            double gaussian_diff = -(t / (sigma * sigma))
                                   * qExp(-qPow(t, 2) / (2 * sigma * sigma));

            // 缩放到目标幅度：正峰~0.14，负峰~-0.12
            double baseVal = gaussian_diff * 2.0;

            double noiseAmp = (qAbs(baseVal) > 0.05) ? 0.008 : 0.004;
            double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * noiseAmp;

            y[i] = qBound(-0.2, baseVal + jitter, 0.2);
        }
        break;
    }
    case Tributary:
    {
        m_graph->setBrush(Qt::NoBrush);

        m_plot->xAxis->setLabel("时间 (µs)");
        m_plot->yAxis->setLabel("电压 (V)");
        m_plot->xAxis->setRange(0, 80);
        m_plot->yAxis->setRange(0, 0.8);
        m_plot->yAxis->setNumberFormat("f");
        m_plot->yAxis->setNumberPrecision(1);

        QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
        fixedTicker->setTickStep(0.1);
        m_plot->yAxis->setTicker(fixedTicker);

        for (int i = 0; i < points; ++i) {
            x[i] = i * (80.0 / (points - 1));

            // 高斯包络，中心40µs，sigma≈6，峰值0.42
            double baseVal = 0.42 * qExp(-qPow((x[i] - 40.0) / 6.0, 2));

            double noiseAmp = (baseVal > 0.05) ? 0.008 : 0.005;
            double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * noiseAmp;

            y[i] = qBound(0.0, baseVal + jitter, 0.4);
        }
        break;
    }
    default:
        return;
    }

    m_graph->setData(x, y);
    m_plot->replot();
}
