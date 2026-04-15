#include "AutoTrackTestWidget.h"

AutoTrackTestWidget::AutoTrackTestWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    m_trackTimer = new QTimer(this);
    m_trackTimer->setInterval(30);
    connect(m_trackTimer, &QTimer::timeout,
            this, &AutoTrackTestWidget::onTrackTick);
}

void AutoTrackTestWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(12);

    // 顶部控制栏
    QHBoxLayout *ctrlLayout = new QHBoxLayout;
    QLabel *lblHint = new QLabel("点击开始跟踪，系统将自动锁定目标：", this);
    lblHint->setStyleSheet("font-size: 14px; color: #333;");

    m_btnStart = new QPushButton("开始跟踪", this);
    m_btnStart->setStyleSheet(
        "QPushButton { background-color: #27ae60; color: white; "
        "border-radius: 4px; padding: 5px 15px; }");

    m_btnReset = new QPushButton("重置", this);
    m_btnReset->setStyleSheet(
        "QPushButton { background-color: #7f8c8d; color: white; "
        "border-radius: 4px; padding: 5px 15px; }");

    ctrlLayout->addWidget(lblHint, 1);
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

    grid->addWidget(new QLabel("目标距离:", gridWidget), 0, 0, Qt::AlignRight);
    m_lblTargetR = makeValLabel();
    grid->addWidget(m_lblTargetR, 0, 1);

    grid->addWidget(new QLabel("回波位置:", gridWidget), 0, 2, Qt::AlignRight);
    m_lblEchoPos = makeValLabel();
    grid->addWidget(m_lblEchoPos, 0, 3);

    grid->addWidget(new QLabel("门位置:", gridWidget), 1, 0, Qt::AlignRight);
    m_lblGatePos = makeValLabel();
    grid->addWidget(m_lblGatePos, 1, 1);

    grid->addWidget(new QLabel("Δt:", gridWidget), 1, 2, Qt::AlignRight);
    m_lblDeltaT = makeValLabel();
    grid->addWidget(m_lblDeltaT, 1, 3);

    grid->addWidget(new QLabel("ΔR:", gridWidget), 2, 0, Qt::AlignRight);
    m_lblDeltaR = makeValLabel();
    grid->addWidget(m_lblDeltaR, 2, 1);

    m_lblStatus = new QLabel("等待开始...", gridWidget);
    m_lblStatus->setAlignment(Qt::AlignCenter);
    m_lblStatus->setFixedHeight(36);
    m_lblStatus->setStyleSheet(
        "QLabel { background-color: #f0f0f0; border-radius: 4px; "
        "font-size: 15px; font-weight: bold; color: #555; }");
    grid->addWidget(m_lblStatus, 3, 0, 1, 4);

    mainLayout->addLayout(ctrlLayout);
    mainLayout->addWidget(gridWidget);
    mainLayout->addStretch();

    connect(m_btnStart, &QPushButton::clicked,
            this, &AutoTrackTestWidget::onStartClicked);
    connect(m_btnReset, &QPushButton::clicked,
            this, &AutoTrackTestWidget::onResetClicked);
}

void AutoTrackTestWidget::feedDistance(int rawDistance)
{
    if (m_locked) return;
    if (rawDistance <= 0 || rawDistance == 9999) return;

    double R_km   = rawDistance * 0.1 * (200.0 / 2000.0);
    m_targetR     = R_km;
    m_echoPos     = fmod(2.0 * R_km / C_KM_US, X_MAX_US);

    m_lblTargetR->setText(QString("%1 km").arg(R_km,      0, 'f', 2));
    m_lblEchoPos->setText(QString("%1 µs").arg(m_echoPos, 0, 'f', 1));
}

void AutoTrackTestWidget::onStartClicked()
{
    if (m_tracking) return;
    m_locked   = true;
    m_tracking = true;
    m_gatePos  = 0.0;
    m_btnStart->setEnabled(false);

    m_lblStatus->setText("跟踪中...");
    m_lblStatus->setStyleSheet(
        "QLabel { background-color: #fef9e7; border-radius: 4px; "
        "font-size: 15px; font-weight: bold; color: #b7950b; }");

    emit trackingStarted();
    m_trackTimer->start();
}

void AutoTrackTestWidget::onTrackTick()
{
    if (qAbs(m_gatePos - m_echoPos) > 0.1)
        m_gatePos += (m_echoPos - m_gatePos) * 0.05;
    else {
        m_gatePos  = m_echoPos;
        m_tracking = false;
        m_trackTimer->stop();
        m_btnStart->setEnabled(true);

        m_lblStatus->setText("✓ 跟踪锁定");
        m_lblStatus->setStyleSheet(
            "QLabel { background-color: #d5f5e3; border-radius: 4px; "
            "font-size: 15px; font-weight: bold; color: #1e8449; }");
    }

    updateDisplay(m_gatePos);
}

void AutoTrackTestWidget::updateDisplay(double gatePos)
{
    double R_km = gatePos * C_KM_US / 2.0;
    double R_m  = R_km * 1000.0;

    m_lblGatePos->setText(QString("%1 µs").arg(gatePos, 0, 'f', 1));
    m_lblDeltaT->setText(QString("%1 µs").arg(gatePos,  0, 'f', 1));
    m_lblDeltaR->setText(QString("%1 m").arg(R_m,       0, 'f', 0));

    emit gatePosChanged(gatePos);
}

void AutoTrackTestWidget::onResetClicked()
{
    reset();
}

void AutoTrackTestWidget::reset()
{
    m_trackTimer->stop();
    m_tracking = false;
    m_locked   = false;
    m_gatePos  = 0.0;
    m_echoPos  = 133.0;
    m_targetR  = 0.0;
    m_btnStart->setEnabled(true);

    m_lblTargetR->setText("--");
    m_lblEchoPos->setText("--");
    m_lblGatePos->setText("--");
    m_lblDeltaT->setText("--");
    m_lblDeltaR->setText("--");
    m_lblStatus->setText("等待开始...");
    m_lblStatus->setStyleSheet(
        "QLabel { background-color: #f0f0f0; border-radius: 4px; "
        "font-size: 15px; font-weight: bold; color: #555; }");

    emit trackingReset();

    emit gatePosChanged(0.0);
}
