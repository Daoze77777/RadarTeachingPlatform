#include "RadarRangingDisplay.h"

RadarRangingDisplay::RadarRangingDisplay(QWidget *parent)
    : QWidget(parent),
    m_targetDistance(10.0),
    m_currentPos(0.0),
    m_isReturning(false),
    m_isRunning(false)
{
    setupUI();
    setupPlot();
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &RadarRangingDisplay::updateAnimationTick);
    onDistanceChanged(m_targetDistance);
}
void RadarRangingDisplay::setupUI() {
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    // --- 顶部控制栏优化 ---
    auto *topLayout = new QHBoxLayout();

    // 距离设置框样式优化
    QLabel *lblSetDist = new QLabel("设置距离:", this);
    lblSetDist->setStyleSheet("color: #333; font-weight: bold;");

    m_spinDistance = new QDoubleSpinBox(this);
    m_spinDistance->setRange(1.0, 100.0);
    m_spinDistance->setValue(m_targetDistance);
    m_spinDistance->setSuffix(" km");
    m_spinDistance->setMinimumWidth(100);
    // 解决白色看不见的问题：设置深色文字和浅灰色背景
    m_spinDistance->setStyleSheet(" background-color: #f0f0f0; color: #000000;");


    // 时间显示框（右侧框框）
    m_lblTime = new QLabel("脉冲时间: 0.0 µs", this);
    m_lblTime->setMinimumWidth(150);
    m_lblTime->setAlignment(Qt::AlignCenter);
    m_lblTime->setStyleSheet("background-color: #2c3e50; color: #ecf0f1; border-radius: 4px; padding: 5px; font-family: 'Consolas'; font-size: 14px;");

    m_btnStart = new QPushButton("开始", this);
    m_btnReset = new QPushButton("重置", this);
    m_btnStart->setStyleSheet("QPushButton { background-color: #2980b9; color: white; border-radius: 4px; padding: 5px 15px; } QPushButton:hover { background-color: #3498db; }");
    m_btnReset->setStyleSheet("QPushButton { background-color: #7f8c8d; color: white; border-radius: 4px; padding: 5px 15px; }");

    topLayout->addWidget(lblSetDist);
    topLayout->addWidget(m_spinDistance);
    topLayout->addStretch(); // 弹簧将时间推向右边
    topLayout->addWidget(m_lblTime);
    topLayout->addSpacing(20);
    topLayout->addWidget(m_btnStart);
    topLayout->addWidget(m_btnReset);

    m_plot = new QCustomPlot(this);
    m_plot->setMinimumHeight(200);

    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(m_plot);

    connect(m_spinDistance, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &RadarRangingDisplay::onDistanceChanged);
    connect(m_btnStart, &QPushButton::clicked, this, &RadarRangingDisplay::onStartClicked);
    connect(m_btnReset, &QPushButton::clicked, this, &RadarRangingDisplay::onResetClicked);
}
void RadarRangingDisplay::setupPlot() {
    m_plot->setBackground(QBrush(QColor(245, 250, 255)));
    m_plot->xAxis->setLabel("探测距离 (km)");
    m_plot->yAxis->setVisible(false);

    // ===== 圆弧完全写死，固定参考最大距离 =====
    const int arcCount = 8;
    const double maxDist = 100.0; // 固定，不跟随距离变化
    const int arcPoints = 80;

    // Y轴也写死，保证圆弧不变形
    m_plot->yAxis->setRange(-maxDist, maxDist);

    for (int i = 0; i < arcCount; ++i) {
        double r = (i + 1) * (maxDist / arcCount);
        QCPCurve *arc = new QCPCurve(m_plot->xAxis, m_plot->yAxis);
        arc->setPen(QPen(QColor(100, 160, 240, 60), 1));
        arc->setBrush(Qt::NoBrush);
        for (int j = 0; j < arcPoints; ++j) {
            double angle = -M_PI / 2.0 + j * M_PI / (arcPoints - 1);
            arc->addData(j, r * qCos(angle), r * qSin(angle));
        }
        m_bgArcs.append(arc);
    }

    // 目标线
    m_targetLine = new QCPItemLine(m_plot);
    m_targetLine->setPen(QPen(QColor(46, 204, 113), 2));

    // 波束
    m_pulseCurve = new QCPCurve(m_plot->xAxis, m_plot->yAxis);

    // ===== 红色圆形标记（初始隐藏）=====
    m_hitMarker = new QCPItemEllipse(m_plot);
    m_hitMarker->setPen(QPen(QColor(220, 50, 50), 2));
    m_hitMarker->setBrush(QBrush(QColor(220, 50, 50, 80)));
    m_hitMarker->setVisible(false);
}
void RadarRangingDisplay::onDistanceChanged(double distance) {
    if (m_isRunning) return;
    m_targetDistance = distance;

    // 只更新X轴和目标线，Y轴和圆弧不动
    m_plot->xAxis->setRange(0, m_targetDistance * 1.15);
    m_targetLine->start->setCoords(m_targetDistance, -100);
    m_targetLine->end->setCoords(m_targetDistance, 100);

    // 隐藏红色标记
    m_hitMarker->setVisible(false);
    m_plot->replot();
}
void RadarRangingDisplay::updateAnimationTick() {
    double speedFactor = m_targetDistance / 80.0;

    if (!m_isReturning) {
        m_currentPos += speedFactor;
        if (m_currentPos >= m_targetDistance) {
            m_currentPos = m_targetDistance;
            m_isReturning = true;

            // ===== 到达目标：显示红色圆形标记 =====
            double r = 1.5; // 固定标记半径
            m_hitMarker->topLeft->setCoords(m_targetDistance - r, r);
            m_hitMarker->bottomRight->setCoords(m_targetDistance + r, -r);
            m_hitMarker->setVisible(true);
        }
    } else {
        m_currentPos -= speedFactor;
        if (m_currentPos <= 0) {
            m_currentPos = 0;
            m_timer->stop();
            m_isRunning = false;
            m_spinDistance->setEnabled(true);
        }
    }

    double totalTraveled = m_isReturning
                               ? (m_targetDistance + (m_targetDistance - m_currentPos))
                               : m_currentPos;
    m_lblTime->setText(QString("脉冲时间: %1 µs").arg(totalTraveled / C_KM_US, 0, 'f', 1));

    updatePulseVisuals();
    m_plot->replot();
}
void RadarRangingDisplay::updatePulseVisuals() {
    QSharedPointer<QCPCurveDataContainer> data = m_pulseCurve->data();
    data->clear();

    // 固定波束大小：用Y轴的固定比例，不随距离变化
    const double fixedSize = 8.0; // 固定8km大小，视觉上固定

    if (!m_isReturning) {
        data->add(QCPCurveData(0, m_currentPos, 0));
        data->add(QCPCurveData(1, m_currentPos - fixedSize, fixedSize * 0.6));
        data->add(QCPCurveData(2, m_currentPos - fixedSize, -fixedSize * 0.6));
        data->add(QCPCurveData(3, m_currentPos, 0));
        m_pulseCurve->setBrush(QBrush(QColor(52, 152, 219, 150)));
        m_pulseCurve->setPen(QPen(QColor(41, 128, 185), 2));
    } else {
        data->add(QCPCurveData(0, m_currentPos, 0));
        data->add(QCPCurveData(1, m_currentPos + fixedSize, fixedSize * 0.5));
        data->add(QCPCurveData(2, m_currentPos + fixedSize, -fixedSize * 0.5));
        data->add(QCPCurveData(3, m_currentPos, 0));
        m_pulseCurve->setBrush(QBrush(QColor(231, 76, 60, 150)));
        m_pulseCurve->setPen(QPen(QColor(192, 57, 43), 2));
    }
}
void RadarRangingDisplay::onStartClicked() {
    if (m_isRunning) return;
    m_isRunning = true;
    m_currentPos = 0.0;
    m_isReturning = false;
    m_spinDistance->setEnabled(false);
    m_timer->start(16);
}
void RadarRangingDisplay::onResetClicked() {
    m_timer->stop();
    m_isRunning = false;
    m_spinDistance->setEnabled(true);
    m_currentPos = 0.0;
    m_isReturning = false;
    m_lblTime->setText("脉冲时间: 0.0 µs");
    m_pulseCurve->data()->clear();
    m_hitMarker->setVisible(false); // 清除标记
    m_plot->replot();
}
