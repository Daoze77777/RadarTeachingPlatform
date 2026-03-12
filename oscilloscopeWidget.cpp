#include "oscilloscopewidget.h"
#include <QVBoxLayout>

OscilloscopeWidget::OscilloscopeWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    m_plot = new QCustomPlot(this);
    layout->addWidget(m_plot);
    layout->setContentsMargins(20,0,0,20);

    setupPlot();

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &OscilloscopeWidget::onRefreshTick);
    m_timer->start(30); // 30ms 刷新实现平滑抖动
}

void OscilloscopeWidget::setupPlot() {
    m_graph = m_plot->addGraph();

    // 线条：亮绿色
    m_graph->setPen(QPen(QColor(0, 255, 128), 2));

    // 填充：设置渐变阴影
    QLinearGradient gradient(0, 0, 0, 400);
    gradient.setColorAt(0, QColor(0, 255, 128, 150)); // 顶部发光
    gradient.setColorAt(1, QColor(0, 255, 128, 0));   // 向下渐隐
    m_graph->setBrush(QBrush(gradient));
    // 1. 设置背景颜色为白色
    m_plot->setBackground(QBrush(Qt::white));
    // 2. 将坐标轴线条改为黑色，否则在白色背景下看不见
    QPen darkPen(Qt::lightGray);
    m_plot->xAxis->setBasePen(darkPen);
    m_plot->yAxis->setBasePen(darkPen);
    m_plot->xAxis->setTickPen(darkPen);
    m_plot->yAxis->setTickPen(darkPen);
    m_plot->xAxis->setTickLabelColor(Qt::lightGray);
    m_plot->yAxis->setTickLabelColor(Qt::lightGray);

    m_plot->xAxis->setRange(0, 20);
    m_plot->yAxis->setRange(-0.5, 5.5);
    // 波形颜色建议使用深一点的绿色或蓝色，在白底上更有质感
    m_graph->setPen(QPen(QColor(0, 150, 80), 2));
}

void OscilloscopeWidget::setData(const QString& stepId) {
    if (stepId == "s2") m_currentMode = TargetSetup;
    else if (stepId == "s3") m_currentMode = PulseTrigger;
    else if (stepId == "s4") m_currentMode = RadarEcho;
    else m_currentMode = None;
}

void OscilloscopeWidget::onRefreshTick() {
    int points = 200;
    QVector<double> x(points), y(points);

    for (int i = 0; i < points; ++i) {
        x[i] = i * (20.0 / (points - 1));
        double baseVal = 0.2;

        // 根据模式计算基础形状
        if (m_currentMode == TargetSetup) {
            baseVal = 3.0 * qExp(-qPow(x[i] - 10, 2) / 6.0); // 模拟山峰状信号
        } else if (m_currentMode == PulseTrigger) {
            baseVal = (x[i] > 4 && x[i] < 6) ? 4.5 : 0.2;
        }

        // 添加实时“毛刺”噪声
        double jitterRange = (baseVal > 0.5) ? 0.25 : 0.08;
        double jitter = (QRandomGenerator::global()->generateDouble() * 2 - 1) * jitterRange;
        y[i] = baseVal + jitter;
    }

    m_graph->setData(x, y);
    m_plot->replot();
}
