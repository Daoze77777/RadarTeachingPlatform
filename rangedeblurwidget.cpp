#include "RangeDeblurWidget.h"
#include <QtMath>

RangeDeblurWidget::RangeDeblurWidget(QWidget *parent)
    : QWidget(parent)
{
    m_Rmax1 = C_KM_US * T1_US / 2.0; // 30 km
    m_Rmax2 = C_KM_US * T2_US / 2.0; // 37.5 km
    setupUI();
}

void RangeDeblurWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(12);

    // ===== 顶部控制栏 =====
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

    // ===== 指标网格 =====
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
    grid->addWidget(new QLabel("T₁ = 200 µs", gridWidget),  0, 0, Qt::AlignCenter);
    grid->addWidget(new QLabel("T₂ = 250 µs", gridWidget),  0, 1, Qt::AlignCenter);
    grid->addWidget(new QLabel("c = 0.3 km/µs", gridWidget),0, 2, Qt::AlignCenter);

    // 行1：Rmax
    grid->addWidget(new QLabel("Rmax₁:", gridWidget), 1, 0, Qt::AlignRight);
    m_lblRmax1 = makeValLabel();
    m_lblRmax1->setText(QString("%1 km").arg(m_Rmax1, 0, 'f', 1));
    grid->addWidget(m_lblRmax1, 1, 1);

    grid->addWidget(new QLabel("Rmax₂:", gridWidget), 1, 2, Qt::AlignRight);
    m_lblRmax2 = makeValLabel();
    m_lblRmax2->setText(QString("%1 km").arg(m_Rmax2, 0, 'f', 1));
    grid->addWidget(m_lblRmax2, 1, 3);

    // 行2：输入距离 + 退模糊结果
    grid->addWidget(new QLabel("输入距离 R:", gridWidget), 2, 0, Qt::AlignRight);
    m_lblR = makeValLabel();
    grid->addWidget(m_lblR, 2, 1);

    grid->addWidget(new QLabel("退模糊 R:", gridWidget), 2, 2, Qt::AlignRight);
    m_lblRsolved = makeValLabel();
    m_lblRsolved->setStyleSheet(
        m_lblRsolved->styleSheet() +
        "QLabel { color: #c0392b; font-size: 15px; }");
    grid->addWidget(m_lblRsolved, 2, 3);

    // 行3：模糊距离
    grid->addWidget(new QLabel("模糊距离 R₁:", gridWidget), 3, 0, Qt::AlignRight);
    m_lblR1 = makeValLabel();
    grid->addWidget(m_lblR1, 3, 1);

    grid->addWidget(new QLabel("模糊距离 R₂:", gridWidget), 3, 2, Qt::AlignRight);
    m_lblR2 = makeValLabel();
    grid->addWidget(m_lblR2, 3, 3);

    // 行4：延迟时间
    grid->addWidget(new QLabel("延迟 t₁:", gridWidget), 4, 0, Qt::AlignRight);
    m_lblT1 = makeValLabel();
    grid->addWidget(m_lblT1, 4, 1);

    grid->addWidget(new QLabel("延迟 t₂:", gridWidget), 4, 2, Qt::AlignRight);
    m_lblT2 = makeValLabel();
    grid->addWidget(m_lblT2, 4, 3);

    mainLayout->addLayout(ctrlLayout);
    mainLayout->addWidget(gridWidget);
    mainLayout->addStretch();

    connect(m_btnStart, &QPushButton::clicked, this, &RangeDeblurWidget::onStartClicked);
    connect(m_btnReset, &QPushButton::clicked, this, &RangeDeblurWidget::onResetClicked);
}

void RangeDeblurWidget::calculate(double R_km)
{
    double R1 = fmod(R_km, m_Rmax1);
    double R2 = fmod(R_km, m_Rmax2);
    double t1 = 2.0 * R1 / C_KM_US;
    double t2 = 2.0 * R2 / C_KM_US;

    // 中国余数定理：暴力搜索（量程200km内足够快）
    double Rsolved = -1.0;
    for (int n1 = 0; n1 * m_Rmax1 + R1 <= 200.0; ++n1) {
        double candidate = n1 * m_Rmax1 + R1;
        double rem2 = fmod(candidate, m_Rmax2);
        if (qAbs(rem2 - R2) < 0.01) {
            Rsolved = candidate;
            break;
        }
    }

    // 更新标签
    m_lblR->setText(QString("%1 km").arg(R_km, 0, 'f', 2));
    m_lblR1->setText(QString("%1 km").arg(R1,   0, 'f', 2));
    m_lblR2->setText(QString("%1 km").arg(R2,   0, 'f', 2));
    m_lblT1->setText(QString("%1 µs").arg(t1,   0, 'f', 2));
    m_lblT2->setText(QString("%1 µs").arg(t2,   0, 'f', 2));
    m_lblRsolved->setText(Rsolved >= 0
                              ? QString("%1 km").arg(Rsolved, 0, 'f', 2)
                              : "解算失败");

    // 发射信号：通知示波器画两路波形
    emit waveformRequested(1, T1_US, t1); // 轨道1
    emit waveformRequested(2, T2_US, t2); // 轨道2
}

void RangeDeblurWidget::onStartClicked()
{
    m_capturedR_km = m_spinDistance->value();
    m_hasCaptured  = true;
    calculate(m_capturedR_km);
}

void RangeDeblurWidget::onResetClicked()
{
    reset();
}

void RangeDeblurWidget::feedDistance(int rawDistance)
{
    if (rawDistance <= 0 || rawDistance == 9999) return;
    // 0.1mm → km，比例系数：200km / 2000mm = 0.1
    double R_km = rawDistance * 0.1 * (200.0 / 2000.0);
    m_spinDistance->setValue(R_km);
    calculate(R_km);
}

void RangeDeblurWidget::reset()
{
    m_hasCaptured  = false;
    m_capturedR_km = 0.0;
    m_lblR->setText("--");
    m_lblR1->setText("--");
    m_lblR2->setText("--");
    m_lblT1->setText("--");
    m_lblT2->setText("--");
    m_lblRsolved->setText("--");
    emit waveformRequested(0, 0, 0); // 清空示波器
}
