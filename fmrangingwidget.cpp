#include "FMRangingWidget.h"

FMRangingWidget::FMRangingWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void FMRangingWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(12);

    // 顶部控制栏
    QHBoxLayout *ctrlLayout = new QHBoxLayout;
    QLabel *lblSet = new QLabel("设置距离:", this);
    lblSet->setStyleSheet("font-size: 14px; font-weight: bold; color: #333;");

    m_spinDistance = new QDoubleSpinBox(this);
    m_spinDistance->setRange(1.0, 75.0);
    m_spinDistance->setValue(20.0);
    m_spinDistance->setSuffix(" km");
    m_spinDistance->setMinimumWidth(110);
    m_spinDistance->setStyleSheet("background-color: #f0f0f0; color: #000;");

    m_btnStart = new QPushButton("开始", this);
    m_btnReset = new QPushButton("重置", this);
    m_btnStart->setStyleSheet(
        "QPushButton { background-color: #2980b9; color: white; "
        "border-radius: 4px; padding: 5px 15px; }");
    m_btnReset->setStyleSheet(
        "QPushButton { background-color: #7f8c8d; color: white; "
        "border-radius: 4px; padding: 5px 15px; }");

    ctrlLayout->addWidget(lblSet);
    ctrlLayout->addWidget(m_spinDistance);
    ctrlLayout->addStretch();
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

    // 行0：固定参数
    grid->addWidget(new QLabel("ΔF = 1000 MHz", gridWidget), 0, 0, Qt::AlignCenter);
    grid->addWidget(new QLabel("T/2 = 500 µs",  gridWidget), 0, 1, Qt::AlignCenter);
    grid->addWidget(new QLabel("c = 0.3 km/µs", gridWidget), 0, 2, Qt::AlignCenter);

    // 行1：Rmax
    grid->addWidget(new QLabel("最大不模糊距离 Rmax:", gridWidget), 1, 0, Qt::AlignRight);
    m_lblRmax = makeValLabel();
    m_lblRmax->setText(QString("%1 km").arg(RMAX_KM, 0, 'f', 1));
    grid->addWidget(m_lblRmax, 1, 1);

    // 行2：输入距离 + 解算距离
    grid->addWidget(new QLabel("输入距离 R:", gridWidget), 2, 0, Qt::AlignRight);
    m_lblR = makeValLabel();
    grid->addWidget(m_lblR, 2, 1);

    grid->addWidget(new QLabel("解算距离 R:", gridWidget), 2, 2, Qt::AlignRight);
    m_lblRsolved = makeValLabel();
    m_lblRsolved->setStyleSheet(
        "QLabel { background-color: #e8eef8; border: 1px solid #b0bcd4; "
        "border-radius: 4px; font-weight: bold; color: #c0392b; "
        "font-size: 15px; padding: 0 8px; }");
    grid->addWidget(m_lblRsolved, 2, 3);

    // 行3：差频 + 延迟
    grid->addWidget(new QLabel("差频 Δf:", gridWidget), 3, 0, Qt::AlignRight);
    m_lblDeltaF = makeValLabel();
    grid->addWidget(m_lblDeltaF, 3, 1);

    grid->addWidget(new QLabel("往返延迟 t:", gridWidget), 3, 2, Qt::AlignRight);
    m_lblDelay = makeValLabel();
    grid->addWidget(m_lblDelay, 3, 3);

    mainLayout->addLayout(ctrlLayout);
    mainLayout->addWidget(gridWidget);
    mainLayout->addStretch();

    connect(m_btnStart, &QPushButton::clicked, this, &FMRangingWidget::onStartClicked);
    connect(m_btnReset, &QPushButton::clicked, this, &FMRangingWidget::onResetClicked);
}

void FMRangingWidget::calculate(double R_km)
{
    // 差频公式：Δf = 2·ΔF·R / (c·T/2)
    double deltaF = 2.0 * DELTA_F_MHZ * R_km / (C_KM_US * T_HALF_US);
    double delay  = 2.0 * R_km / C_KM_US; // 往返延迟 µs
    // 反解距离验证
    double Rsolved = deltaF * C_KM_US * T_HALF_US / (2.0 * DELTA_F_MHZ);

    m_lblR->setText(QString("%1 km").arg(R_km,     0, 'f', 2));
    m_lblDeltaF->setText(QString("%1 MHz").arg(deltaF,  0, 'f', 2));
    m_lblDelay->setText(QString("%1 µs").arg(delay,   0, 'f', 2));
    m_lblRsolved->setText(QString("%1 km").arg(Rsolved, 0, 'f', 2));

    emit waveformRequested(deltaF);
}

void FMRangingWidget::onStartClicked()
{
    calculate(m_spinDistance->value());
}

void FMRangingWidget::onResetClicked()
{
    reset();
}

void FMRangingWidget::feedDistance(int rawDistance)
{
    if (rawDistance <= 0 || rawDistance == 9999) return;
    // 0.1mm → km，量程75km对应实际0.75m
    double R_km = rawDistance * 0.1 * (75.0 / 2000.0);
    m_spinDistance->setValue(R_km);
    calculate(R_km);
}

void FMRangingWidget::reset()
{
    m_lblR->setText("--");
    m_lblDeltaF->setText("--");
    m_lblDelay->setText("--");
    m_lblRsolved->setText("--");
    emit waveformRequested(0);
}
