#include "PhaseRangingWidget.h"

PhaseRangingWidget::PhaseRangingWidget(QWidget *parent)
    : QWidget(parent)
{
    m_Rmax1 = C_KM_US * T1_US / 2.0; // 97.5 km
    m_Rmax2 = C_KM_US * T2_US / 2.0; // 105.0 km
    setupUI();
}

void PhaseRangingWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(12);

    // 顶部控制栏
    QHBoxLayout *ctrlLayout = new QHBoxLayout;
    QLabel *lblSet = new QLabel("设置距离:", this);
    lblSet->setStyleSheet("font-size: 14px; font-weight: bold; color: #333;");

    m_spinDistance = new QDoubleSpinBox(this);
    m_spinDistance->setRange(1.0, 200.0);
    m_spinDistance->setValue(50.0);
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
    grid->addWidget(new QLabel("T₁ = 650 µs", gridWidget), 0, 0, Qt::AlignCenter);
    grid->addWidget(new QLabel("T₂ = 700 µs", gridWidget), 0, 1, Qt::AlignCenter);
    grid->addWidget(new QLabel("c = 0.3 km/µs", gridWidget), 0, 2, Qt::AlignCenter);

    // 行1：Rmax
    grid->addWidget(new QLabel("Rmax₁:", gridWidget), 1, 0, Qt::AlignRight);
    m_lblRmax1 = makeValLabel();
    m_lblRmax1->setText(QString("%1 km").arg(m_Rmax1, 0, 'f', 1));
    grid->addWidget(m_lblRmax1, 1, 1);

    grid->addWidget(new QLabel("Rmax₂:", gridWidget), 1, 2, Qt::AlignRight);
    m_lblRmax2 = makeValLabel();
    m_lblRmax2->setText(QString("%1 km").arg(m_Rmax2, 0, 'f', 1));
    grid->addWidget(m_lblRmax2, 1, 3);

    // 行2：输入距离 + 解算结果
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

    // 行3：相位差
    grid->addWidget(new QLabel("相位差 φ₁:", gridWidget), 3, 0, Qt::AlignRight);
    m_lblPhi1 = makeValLabel();
    grid->addWidget(m_lblPhi1, 3, 1);

    grid->addWidget(new QLabel("相位差 φ₂:", gridWidget), 3, 2, Qt::AlignRight);
    m_lblPhi2 = makeValLabel();
    grid->addWidget(m_lblPhi2, 3, 3);

    // 行4：Δφ
    grid->addWidget(new QLabel("Δφ:", gridWidget), 4, 0, Qt::AlignRight);
    m_lblDeltaPhi = makeValLabel();
    grid->addWidget(m_lblDeltaPhi, 4, 1);

    mainLayout->addLayout(ctrlLayout);
    mainLayout->addWidget(gridWidget);
    mainLayout->addStretch();

    connect(m_btnStart, &QPushButton::clicked, this, &PhaseRangingWidget::onStartClicked);
    connect(m_btnReset, &QPushButton::clicked, this, &PhaseRangingWidget::onResetClicked);
}

void PhaseRangingWidget::calculate(double R_km)
{
    double delay = 2.0 * R_km / C_KM_US; // 往返延迟 µs

    // 相位差（弧度）
    double phi1 = fmod(delay, T1_US) / T1_US * 2.0 * M_PI;
    double phi2 = fmod(delay, T2_US) / T2_US * 2.0 * M_PI;
    double deltaPhi = phi1 - phi2;
    if (deltaPhi < 0) deltaPhi += 2.0 * M_PI;

    // 解算距离：中国余数定理暴力搜索
    double R1_km = fmod(delay, T1_US) * C_KM_US / 2.0;
    double R2_km = fmod(delay, T2_US) * C_KM_US / 2.0;
    double Rsolved = -1.0;
    for (int n1 = 0; n1 * m_Rmax1 + R1_km <= 201.0; ++n1) {
        double candidate = n1 * m_Rmax1 + R1_km;
        double rem2 = fmod(2.0 * candidate / C_KM_US, T2_US) * C_KM_US / 2.0;
        if (qAbs(rem2 - R2_km) < 0.01) {
            Rsolved = candidate;
            break;
        }
    }

    // 更新标签
    m_lblR->setText(QString("%1 km").arg(R_km, 0, 'f', 2));
    m_lblPhi1->setText(QString("%1°").arg(qRadiansToDegrees(phi1), 0, 'f', 1));
    m_lblPhi2->setText(QString("%1°").arg(qRadiansToDegrees(phi2), 0, 'f', 1));
    m_lblDeltaPhi->setText(QString("%1°").arg(qRadiansToDegrees(deltaPhi), 0, 'f', 1));
    m_lblRsolved->setText(Rsolved >= 0
                              ? QString("%1 km").arg(Rsolved, 0, 'f', 2)
                              : "解算失败");

    // 通知示波器：用相位偏移驱动波形
    double wavePhase1 = -phi1;
    double wavePhase2 = -phi2;
    emit waveformRequested(wavePhase1, wavePhase2);
}

void PhaseRangingWidget::onStartClicked()
{
    calculate(m_spinDistance->value());
}

void PhaseRangingWidget::onResetClicked()
{
    reset();
}

void PhaseRangingWidget::feedDistance(int rawDistance)
{
    if (rawDistance <= 0 || rawDistance == 9999) return;
    double R_km = rawDistance * 0.1 * (200.0 / 2000.0);
    m_spinDistance->setValue(R_km);
    calculate(R_km);
}

void PhaseRangingWidget::reset()
{
    m_lblR->setText("--");
    m_lblPhi1->setText("--");
    m_lblPhi2->setText("--");
    m_lblDeltaPhi->setText("--");
    m_lblRsolved->setText("--");
    emit waveformRequested(0, 0);
}
