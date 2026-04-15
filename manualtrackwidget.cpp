#include "ManualTrackWidget.h"

ManualTrackWidget::ManualTrackWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void ManualTrackWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(12);

    // 顶部控制栏
    QHBoxLayout *ctrlLayout = new QHBoxLayout;

    QLabel *lblSlider = new QLabel("跟踪门位置:", this);
    lblSlider->setStyleSheet("font-size: 14px; font-weight: bold; color: #333;");

    m_slider = new QSlider(Qt::Horizontal, this);
    m_slider->setRange(0, 267);
    m_slider->setValue(0);
    m_slider->setMinimumWidth(200);

    m_btnStart = new QPushButton("开始", this);
    m_btnStart->setStyleSheet(
        "QPushButton { background-color: #27ae60; color: white; "
        "border-radius: 4px; padding: 5px 15px; }");

    m_btnReset = new QPushButton("重置", this);
    m_btnReset->setStyleSheet(
        "QPushButton { background-color: #7f8c8d; color: white; "
        "border-radius: 4px; padding: 5px 15px; }");

    ctrlLayout->addWidget(lblSlider);
    ctrlLayout->addWidget(m_slider, 1);
    ctrlLayout->addWidget(m_btnStart);
    ctrlLayout->addWidget(m_btnReset);

    // 指标网格
    QWidget     *gridWidget = new QWidget(this);
    QGridLayout *grid       = new QGridLayout(gridWidget);
    grid->setSpacing(10);
    gridWidget->setStyleSheet(
        "QWidget { background-color: #f7f9fc; border-radius: 6px; }"
        "QLabel  { font-size: 14px; color: #2D2F4F; }");

    auto makeValLabel = [&]() -> QLabel* {
        QLabel *l = new QLabel("--", gridWidget);
        l->setAlignment(Qt::AlignCenter);
        l->setFixedHeight(32);
        l->setStyleSheet(
            "QLabel { background-color: #e8eef8; border: 1px solid #b0bcd4; "
            "border-radius: 4px; font-weight: bold; color: #1a3a6e; padding: 0 8px; }");
        return l;
    };

    grid->addWidget(new QLabel("门位置:", gridWidget),  0, 0, Qt::AlignRight);
    m_lblGatePos = makeValLabel();
    grid->addWidget(m_lblGatePos, 0, 1);

    grid->addWidget(new QLabel("旋钮电压:", gridWidget), 0, 2, Qt::AlignRight);
    m_lblVoltage = makeValLabel();
    grid->addWidget(m_lblVoltage, 0, 3);

    grid->addWidget(new QLabel("Δt:", gridWidget), 1, 0, Qt::AlignRight);
    m_lblDeltaT = makeValLabel();
    grid->addWidget(m_lblDeltaT, 1, 1);

    grid->addWidget(new QLabel("ΔR:", gridWidget), 1, 2, Qt::AlignRight);
    m_lblDeltaR = makeValLabel();
    grid->addWidget(m_lblDeltaR, 1, 3);

    m_lblStatus = new QLabel("等待操作...", gridWidget);
    m_lblStatus->setAlignment(Qt::AlignCenter);
    m_lblStatus->setFixedHeight(36);
    m_lblStatus->setStyleSheet(
        "QLabel { background-color: #f0f0f0; border-radius: 4px; "
        "font-size: 15px; font-weight: bold; color: #555; }");
    grid->addWidget(m_lblStatus, 2, 0, 1, 4);

    mainLayout->addLayout(ctrlLayout);
    mainLayout->addWidget(gridWidget);
    mainLayout->addStretch();

    connect(m_slider,   &QSlider::valueChanged,
            this, &ManualTrackWidget::onSliderChanged);
    connect(m_btnStart, &QPushButton::clicked,
            this, &ManualTrackWidget::onStartClicked);
    connect(m_btnReset, &QPushButton::clicked,
            this, &ManualTrackWidget::onResetClicked);
}

void ManualTrackWidget::onStartClicked()
{
    m_echoLocked = true;
    m_started    = true;
    m_btnStart->setEnabled(false);
    m_lblStatus->setText("跟踪中...");
    m_lblStatus->setStyleSheet(
        "QLabel { background-color: #fef9e7; border-radius: 4px; "
        "font-size: 15px; font-weight: bold; color: #b7950b; }");
}

void ManualTrackWidget::onSliderChanged(int value)
{
    updateDisplay(static_cast<double>(value));
}

void ManualTrackWidget::feedKnobVoltage(double voltage)
{
    double R_km    = voltage / 5.0 * 200.0;
    double delay   = 2.0 * R_km / C_KM_US;
    double gatePos = fmod(delay, X_MAX_US);

    m_lblVoltage->setText(QString("%1 V").arg(voltage, 0, 'f', 3));
    m_slider->blockSignals(true);
    m_slider->setValue(static_cast<int>(gatePos));
    m_slider->blockSignals(false);
    updateDisplay(gatePos);
}

void ManualTrackWidget::feedEchoPos(double echoPos_us)
{
    if (m_echoLocked) return;  // 点开始后锁定回波
    m_echoPos = echoPos_us;
}

void ManualTrackWidget::updateDisplay(double gatePos)
{
    double R_km = gatePos * C_KM_US / 2.0;
    double R_m  = R_km * 1000.0;

    m_lblGatePos->setText(QString("%1 µs").arg(gatePos, 0, 'f', 1));
    m_lblDeltaT->setText(QString("%1 µs").arg(gatePos,  0, 'f', 1));
    m_lblDeltaR->setText(QString("%1 m").arg(R_m,       0, 'f', 0));

    if (qAbs(gatePos - m_echoPos) < 5.0) {
        m_lblStatus->setText("✓ 捕获成功");
        m_lblStatus->setStyleSheet(
            "QLabel { background-color: #d5f5e3; border-radius: 4px; "
            "font-size: 15px; font-weight: bold; color: #1e8449; }");
    } else if (m_echoLocked) {  // m_locked → m_echoLocked
        m_lblStatus->setText("跟踪中...");
        m_lblStatus->setStyleSheet(
            "QLabel { background-color: #fef9e7; border-radius: 4px; "
            "font-size: 15px; font-weight: bold; color: #b7950b; }");
    }

    emit gatePosChanged(gatePos);
}

void ManualTrackWidget::onResetClicked()
{
    reset();
}

void ManualTrackWidget::reset()
{
    m_echoLocked = false;
    m_started    = false;
    m_btnStart->setEnabled(true);

    m_slider->blockSignals(true);
    m_slider->setValue(0);
    m_slider->blockSignals(false);

    m_echoPos = 133.0;
    m_lblGatePos->setText("--");
    m_lblVoltage->setText("--");
    m_lblDeltaT->setText("--");
    m_lblDeltaR->setText("--");
    m_lblStatus->setText("等待操作...");
    m_lblStatus->setStyleSheet(
        "QLabel { background-color: #f0f0f0; border-radius: 4px; "
        "font-size: 15px; font-weight: bold; color: #555; }");

    emit gatePosChanged(0.0);
    emit resetted();
}
