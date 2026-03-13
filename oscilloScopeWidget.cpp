#include "oscilloscopewidget.h"
#include <QtMath>

OscilloscopeWidget::OscilloscopeWidget(QWidget *parent)
    : QChartView(parent)
    , m_currentMode(None)
    , m_amplitude(4.0)
    , m_frequency(1.0)
    , m_timeOffset(0.0)
{
    setupChart();

    // 启动动画定时器（30ms 刷新率确保抖动流畅）
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &OscilloscopeWidget::onUpdateTick);
    m_timer->start(30);
}

OscilloscopeWidget::~OscilloscopeWidget() {}

void OscilloscopeWidget::setupChart() {
    m_chart = new QChart();
    m_series = new QLineSeries();

    // 设置画笔：亮绿色，宽度 1.5，模拟示波器电子束
    QPen pen(QColor(0, 255, 128));
    pen.setWidthF(1.5);
    m_series->setPen(pen);

    m_chart->addSeries(m_series);

    m_axisX = new QValueAxis();
    m_axisX->setRange(0, 20);
    m_axisX->setTitleText("时间 (μs)");

    m_axisY = new QValueAxis();
    m_axisY->setRange(-0.5, 5.5);
    m_axisY->setTitleText("电压 (V)");

    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_chart->addAxis(m_axisY, Qt::AlignLeft);
    m_series->attachAxis(m_axisX);
    m_series->attachAxis(m_axisY);

    m_chart->legend()->hide();
    m_chart->setBackgroundVisible(false); // 配合深色UI

    this->setChart(m_chart);
    this->setRenderHint(QPainter::Antialiasing);
}

void OscilloscopeWidget::setData(const QString& stepId) {
    if (stepId == "s1") m_currentMode = TargetSetup;
    else if (stepId == "s2") m_currentMode = PulseTrigger;
    else if (stepId == "s3") m_currentMode = SineWave;
    else if (stepId == "s4") m_currentMode = RadarEcho;
    else m_currentMode = None;
}

void OscilloscopeWidget::onUpdateTick() {
    m_timeOffset += 0.3; // 更新偏移量，使波形向左滚动
    updateWaveform();
}

void OscilloscopeWidget::updateWaveform() {
    QList<QPointF> points;
    const int count = 250; // 采样点数
    const double xRange = 20.0;
    const double step = xRange / count;

    for (int i = 0; i <= count; ++i) {
        double x = i * step;
        double y = 0.5; // 基准电平
        double jitter = -0.15 + QRandomGenerator::global()->generateDouble() * 0.3;
        switch (m_currentMode) {
        case TargetSetup:
            y = calcTargetWave(x);
            jitter *= 2.0; // 信号区抖动更剧烈
            break;
        case PulseTrigger:
            y = calcPulseWave(x);
            break;
        case SineWave:
            y = calcSineWave(x);
            break;
        case RadarEcho:
            // 模拟远距离回波，带有随机幅度衰减
            if (x > 12.0 && x < 14.0)
                y = 3.0 + (-0.5 + QRandomGenerator::global()->generateDouble() * 1.0);
            else y = jitter;
            break;
        default:
            y = 0.5; // 只有微弱底噪
            break;
        }
        points.append(QPointF(x, y + jitter));
    }
    m_series->replace(points); // 高效替换数据
}

double OscilloscopeWidget::calcTargetWave(double x) {
    // 模拟图片中 s1 的包络线：组合正弦波
    return 1.5 * qSin(0.5 * x) + 1.0 * qCos(1.2 * x) + 2.5;
}

double OscilloscopeWidget::calcPulseWave(double x) {
    // s2: 5us 到 7us 之间的触发脉冲
    return (x > 5.0 && x < 7.0) ? 4.5 : 0.2;
}

double OscilloscopeWidget::calcSineWave(double x) {
    // 标准正弦波，受 timeOffset 驱动滚动
    return (m_amplitude/2.0) * qSin(m_frequency * x - m_timeOffset) + 2.5;
}
