#include "RangeDeblurAnimation.h"
#include <QtMath>
#include <QRandomGenerator>
#include <QFrame>

// ===== 坐标常量 =====
static const double X_MAX       = 10.0;
static const double TR           = 3.0;   // 脉冲重复周期
static const double TAU          = 0.5;   // 脉冲宽度 τ
static const double T0           = 0.3;   // 收发切换时间
static const double TX_POSITIONS[] = {0.0, 3.0, 6.0, 9.0}; // 发射脉冲起点
static const double ECHO_NORMAL  = 1.5;   // 正常回波位置
static const double ECHO_AMBIG   = 4.2;   // 跨周期回波位置（距离模糊）
static const double ECHO_FAR     = 7.5;   // 远处回波

// Y 轴：上半（发射）0.6~1.0，下半（回波）0.1~0.5，噪声基线 0.25
static const double TX_BASE     = 0.6;
static const double TX_TOP      = 1.0;
static const double ECHO_BASE   = 0.25;
static const double ECHO_HEIGHT = 0.25; // 回波峰值高度
static const double NOISE_Y     = 0.25;

RangeDeblurAnimation::RangeDeblurAnimation(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    setupPlot();
    drawStaticWaveforms();
    updatePhase();

    m_timer = new QTimer(this);
    m_timer->setInterval(3000);
    connect(m_timer, &QTimer::timeout, this, &RangeDeblurAnimation::onNextPhase);
    //m_timer->start();
}

void RangeDeblurAnimation::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(8);

    // 波形区
    m_plot = new QCustomPlot(this);
    m_plot->setMinimumHeight(180);
    m_plot->setMaximumHeight(220);
    mainLayout->addWidget(m_plot);

    // 说明框
    QFrame *descFrame = new QFrame(this);
    //descFrame->setStyleSheet("QFrame { background: #F0F4FF; border: none; }");
    descFrame->setStyleSheet("background: transparent; border: none;");
    descFrame->setFrameShape(QFrame::StyledPanel);
    descFrame->setStyleSheet("QFrame { background: #F0F4FF; border: 1px solid #A0B0D0; border-radius: 6px; }");
    QVBoxLayout *descLayout = new QVBoxLayout(descFrame);
    descLayout->setContentsMargins(12, 8, 12, 8);
    descLayout->setSpacing(4);

    m_phaseTitle = new QLabel(this);
    m_phaseTitle->setStyleSheet("font-size: 15px; font-weight: bold; color: #2244AA; border: none; background: transparent;");

    m_phaseDesc = new QLabel(this);
    m_phaseDesc->setWordWrap(true);
   m_phaseDesc->setStyleSheet("font-size: 13px; color: #333; border: none; background: transparent;");

    descLayout->addWidget(m_phaseTitle);
    descLayout->addWidget(m_phaseDesc);
    mainLayout->addWidget(descFrame);

    // 按钮区
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->setAlignment(Qt::AlignCenter);
    btnLayout->setSpacing(20);

    m_btnStart = new QPushButton("开始演示", this);
    m_btnPause = new QPushButton("暂停", this);
    m_btnReset = new QPushButton("重置", this);

    m_btnStart->setFixedSize(90, 36);
    m_btnPause->setFixedSize(90, 36);
    m_btnReset->setFixedSize(90, 36);

    m_btnPause->setStyleSheet("QPushButton { background:#345196; color:white; border-radius:4px; font-size:14px; }"
                              "QPushButton:hover { background:#4466BB; }");
    m_btnReset->setStyleSheet("QPushButton { background:#7F8C8D; color:white; border-radius:4px; font-size:14px; }"
                              "QPushButton:hover { background:#95A5A6; }");
    m_btnStart->setStyleSheet("QPushButton { background:#345196; color:white; border-radius:4px; font-size:14px; }"
                              "QPushButton:hover { background:#4466BB; }");

    // 初始状态：暂停按钮隐藏，timer不启动
    m_btnPause->setVisible(false);
    connect(m_btnStart, &QPushButton::clicked, this, [this]() {
        m_btnStart->setVisible(false);
        m_btnPause->setVisible(true);
        m_timer->start();
    });
    connect(m_btnPause, &QPushButton::clicked, this, &RangeDeblurAnimation::onPauseClicked);
    connect(m_btnReset, &QPushButton::clicked, this, &RangeDeblurAnimation::onResetClicked);

    btnLayout->addWidget(m_btnStart);
    btnLayout->addWidget(m_btnPause);
    btnLayout->addWidget(m_btnReset);
    mainLayout->addLayout(btnLayout);
}

void RangeDeblurAnimation::setupPlot()
{
    m_plot->setBackground(QBrush(Qt::white));
    m_plot->axisRect()->setMargins(QMargins(40, 25, 20, 20));

    // 隐藏坐标轴刻度，只保留箭头效果
    m_plot->xAxis->setRange(0, X_MAX + 0.3);
    m_plot->yAxis->setRange(0, 1.15);
    m_plot->xAxis->setTickLabels(false);
    m_plot->yAxis->setTickLabels(false);
    m_plot->xAxis->setTicks(false);
    m_plot->yAxis->setTicks(false);
    m_plot->xAxis->setBasePen(QPen(Qt::black, 1));
    m_plot->yAxis->setBasePen(Qt::NoPen);
    m_plot->xAxis2->setVisible(false);
    m_plot->yAxis2->setVisible(false);

    // X轴箭头用 item line 模拟
    // 上方时间轴标签
    // QCPItemText *txLabel = new QCPItemText(m_plot);
    // txLabel->setPositionAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    // txLabel->position->setCoords(X_MAX / 2.0, 1.12);
    // txLabel->setText("发射脉冲");
    // txLabel->setFont(QFont("", 10, QFont::Bold));
    // txLabel->setColor(Qt::black);

    QCPItemText *rxLabel = new QCPItemText(m_plot);
    rxLabel->setPositionAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    rxLabel->position->setCoords(X_MAX / 2.0, 0.55);
    rxLabel->setText("回波");
    rxLabel->setFont(QFont("", 10, QFont::Bold));
    rxLabel->setColor(Qt::black);

    QCPItemText *noiseLabel = new QCPItemText(m_plot);
    noiseLabel->setPositionAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    noiseLabel->position->setCoords(X_MAX + 0.05, NOISE_Y);
    noiseLabel->setText("噪声");
    noiseLabel->setFont(QFont("", 9));
    noiseLabel->setColor(Qt::gray);

    // t 箭头标签
    QCPItemText *tTxLabel = new QCPItemText(m_plot);
    tTxLabel->setPositionAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    tTxLabel->position->setCoords(X_MAX + 0.05, TX_BASE + (TX_TOP - TX_BASE) / 2.0);
    tTxLabel->setText("t");
    tTxLabel->setFont(QFont("", 9));
    tTxLabel->setColor(Qt::black);

    // τ 标注
    QCPItemText *tauLabel = new QCPItemText(m_plot);
    tauLabel->setPositionAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    tauLabel->position->setCoords(TAU / 2.0, TX_TOP + 0.03);
    tauLabel->setText("τ");
    tauLabel->setFont(QFont("", 9));
    tauLabel->setColor(QColor(0, 100, 200));

    // tr 标注（蓝色箭头文字）
    QCPItemText *trLabel = new QCPItemText(m_plot);
    trLabel->setPositionAlignment(Qt::AlignHCenter | Qt::AlignTop);
    trLabel->position->setCoords(ECHO_NORMAL / 2.0, ECHO_BASE + ECHO_HEIGHT + 0.12);
    trLabel->setText("tᵣ");
    trLabel->setFont(QFont("", 9));
    trLabel->setColor(QColor(0, 100, 200));

    // "来自上一发脉冲回波" 标注
    QCPItemText *ambigNote = new QCPItemText(m_plot);
    ambigNote->setPositionAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    ambigNote->position->setCoords(ECHO_AMBIG, ECHO_BASE + ECHO_HEIGHT + 0.14);
    ambigNote->setText("来自上一发脉冲\n回波");
    ambigNote->setFont(QFont("", 8));
    ambigNote->setColor(QColor(200, 0, 0));

    // tr(跨周期) 标注
    QCPItemText *ambigTimeLabel = new QCPItemText(m_plot);
    ambigTimeLabel->setPositionAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    ambigTimeLabel->position->setCoords(ECHO_AMBIG, ECHO_BASE + ECHO_HEIGHT + 0.04);
    ambigTimeLabel->setText("tᵣ(跨周期)");
    ambigTimeLabel->setFont(QFont("", 8));
    ambigTimeLabel->setColor(QColor(200, 0, 0));

    // 水平分隔线
    QCPItemLine *sepLine = new QCPItemLine(m_plot);
    sepLine->start->setCoords(0, 0.57);
    sepLine->end->setCoords(X_MAX + 0.2, 0.57);
    sepLine->setPen(QPen(Qt::lightGray, 1, Qt::DashLine));

    // 初始化高亮框（默认隐藏）
    m_highlightRect = new QCPItemRect(m_plot);
    m_highlightRect->setPen(QPen(QColor(0, 150, 220), 2));
    m_highlightRect->setBrush(QBrush(QColor(0, 150, 220, 30)));
    m_highlightRect->setVisible(false);

    // 初始化圆圈标记（默认隐藏）
    m_echoCircle = new QCPItemEllipse(m_plot);
    m_echoCircle->setPen(QPen(QColor(0, 150, 220), 2));
    m_echoCircle->setBrush(Qt::NoBrush);
    m_echoCircle->setVisible(false);
}

void RangeDeblurAnimation::drawStaticWaveforms()
{
    // graph(0): 发射脉冲串（黑色矩形波）
    QCPGraph *txGraph = m_plot->addGraph();
    txGraph->setPen(QPen(Qt::black, 1.5));
    txGraph->setBrush(Qt::NoBrush);

    QVector<double> txX, txY;
    for (double pos : TX_POSITIONS) {
        // 上升沿
        txX << pos;   txY << TX_BASE;
        txX << pos;   txY << TX_TOP;
        // 高电平
        txX << pos + TAU; txY << TX_TOP;
        // 下降沿
        txX << pos + TAU; txY << TX_BASE;
    }
    // 基线延伸到末尾
    txX << 0.0; txY << TX_BASE;
    // 重新排序：先画基线
    QVector<double> txXFinal, txYFinal;
    txXFinal << 0.0;
    txYFinal << TX_BASE;
    for (int i = 0; i < (int)(sizeof(TX_POSITIONS)/sizeof(TX_POSITIONS[0])); ++i) {
        double pos = TX_POSITIONS[i];
        txXFinal << pos << pos << pos+TAU << pos+TAU;
        txYFinal << TX_BASE << TX_TOP << TX_TOP << TX_BASE;
    }
    txXFinal << X_MAX + 0.2;
    txYFinal << TX_BASE;
    txGraph->setData(txXFinal, txYFinal);

    // graph(1): 噪声基线（灰色）
    QCPGraph *noiseGraph = m_plot->addGraph();
    noiseGraph->setPen(QPen(Qt::gray, 1));
    int nPoints = 300;
    QVector<double> nx, ny;
    nx.resize(nPoints); ny.resize(nPoints);
    for (int i = 0; i < nPoints; ++i) {
        nx[i] = i * (X_MAX / (nPoints - 1));
        ny[i] = NOISE_Y + (QRandomGenerator::global()->generateDouble() * 2 - 1) * 0.012;
    }
    noiseGraph->setData(nx, ny);

    // graph(2): 正常回波（绿色高斯峰）
    QCPGraph *echoNormal = m_plot->addGraph();
    echoNormal->setPen(QPen(QColor(0, 160, 60), 2));
    int ep = 80;
    QVector<double> ex, ey;
    ex.resize(ep); ey.resize(ep);
    double sigma = 0.12;
    for (int i = 0; i < ep; ++i) {
        ex[i] = ECHO_NORMAL - 0.5 + i * (1.0 / (ep-1));
        double dt = ex[i] - ECHO_NORMAL;
        ey[i] = NOISE_Y + ECHO_HEIGHT * qExp(-dt*dt / (2*sigma*sigma));
    }
    echoNormal->setData(ex, ey);

    // graph(3): 跨周期回波（红色高斯峰）
    QCPGraph *echoAmbig = m_plot->addGraph();
    echoAmbig->setPen(QPen(QColor(200, 0, 0), 2));
    QVector<double> ax, ay;
    ax.resize(ep); ay.resize(ep);
    for (int i = 0; i < ep; ++i) {
        ax[i] = ECHO_AMBIG - 0.5 + i * (1.0 / (ep-1));
        double dt = ax[i] - ECHO_AMBIG;
        ay[i] = NOISE_Y + ECHO_HEIGHT * qExp(-dt*dt / (2*sigma*sigma));
    }
    echoAmbig->setData(ax, ay);

    // graph(4): 远处绿色回波
    QCPGraph *echoFar = m_plot->addGraph();
    echoFar->setPen(QPen(QColor(0, 160, 60), 2));
    QVector<double> fx, fy;
    fx.resize(ep); fy.resize(ep);
    for (int i = 0; i < ep; ++i) {
        fx[i] = ECHO_FAR - 0.5 + i * (1.0 / (ep-1));
        double dt = fx[i] - ECHO_FAR;
        fy[i] = NOISE_Y + ECHO_HEIGHT * qExp(-dt*dt / (2*sigma*sigma));
    }
    echoFar->setData(fx, fy);

    m_plot->replot();
}

void RangeDeblurAnimation::clearHighlights()
{
    m_highlightRect->setVisible(false);
    m_echoCircle->setVisible(false);
}

void RangeDeblurAnimation::updatePhase()
{
    clearHighlights();

    struct PhaseInfo {
        QString title;
        QString desc;
        // 高亮框参数（x1,y1,x2,y2），0表示不显示
        double hx1, hy1, hx2, hy2;
        bool useCircle;
        double cx, cy, cr; // 圆圈中心和半径
        bool redCircle;
    };

    static const PhaseInfo phases[] = {
        // Phase1: 发射中 - 蓝框套第一个发射脉冲
        {
            "◆ 阶段：发射中",
            "雷达发射机工作，收发共用天线。接收机在 τ 时间内断开，无法接收回波。\n➡ 这一段距离内的目标不可测。",
            -0.05, TX_BASE - 0.03, TAU + 0.05, TX_TOP + 0.05,
            false, 0, 0, 0, false
        },
        // Phase2: 收发切换 - 蓝框套 τ 之后的切换时间
        {
            "◆ 阶段：收发切换",
            "发射结束后天线和接收机切换到接收状态，还需时间 t₀。\n➡ 最小可测距离：Rmin = c(τ + t₀)/2。",
            TAU - 0.05, TX_BASE - 0.03, TAU + T0 + 0.05, TX_TOP + 0.05,
            false, 0, 0, 0, false
        },
        // Phase3: 正常测距 - 蓝圆圈套绿色回波
        {
            "◆ 阶段：正常测距",
            "回波在同一发射周期内返回。测得时延 tᵣ → 距离 R = ctᵣ/2。\n➡ 单值测距，无模糊。",
            0, 0, 0, 0,
            true, ECHO_NORMAL, NOISE_Y + ECHO_HEIGHT/2.0, 0.45, false
        },
        // Phase4: 第二次发射 - 蓝框套第二个发射脉冲
        {
            "◆ 阶段：第二次发射",
            "雷达周期性发射。此时前一发回波（远目标）可能还未返回。",
            TR - 0.05, TX_BASE - 0.03, TR + TAU + 0.05, TX_TOP + 0.05,
            false, 0, 0, 0, false
        },
        // Phase5: 距离模糊 - 红圆圈套红色回波
        {
            "◆ 阶段：距离模糊产生",
            "远目标回波跨周期返回，落入下一发射周期。\n系统无法判断该回波属于哪一次发射。\n➡ 同一延迟 tᵣ 对应多个 R → 距离模糊。",
            0, 0, 0, 0,
            true, ECHO_AMBIG, NOISE_Y + ECHO_HEIGHT/2.0, 0.45, true
        },
        // Phase6: 总结 - 蓝框套噪声区域末端
        {
            "◆ 阶段：总结",
            "噪声只是干扰，不会导致模糊。\n模糊真正成因：Tᵣ 过短 → 远距回波跨周期。\n模糊的具体现象：雷达测得的延迟 tᵣ 与真实延迟相差一个周期，计算出的距离被折叠回近处。\n解决：双 PRF 退模糊。",
            X_MAX - 0.5, NOISE_Y - 0.05, X_MAX + 0.25, NOISE_Y + 0.05,
            false, 0, 0, 0, false
        },
    };

    const PhaseInfo &p = phases[m_currentPhase];
    m_phaseTitle->setText(p.title);
    m_phaseDesc->setText(p.desc);

    if (p.useCircle) {
        m_echoCircle->setVisible(true);
        m_echoCircle->topLeft->setCoords(p.cx - p.cr, p.cy + p.cr);
        m_echoCircle->bottomRight->setCoords(p.cx + p.cr, p.cy - p.cr);
        QPen circlePen = p.redCircle
                         ? QPen(QColor(220, 0, 0), 2)
                         : QPen(QColor(0, 150, 220), 2);
        m_echoCircle->setPen(circlePen);
    } else if (p.hx2 > 0) {
        m_highlightRect->setVisible(true);
        m_highlightRect->topLeft->setCoords(p.hx1, p.hy2);
        m_highlightRect->bottomRight->setCoords(p.hx2, p.hy1);
    }

    m_plot->replot();

    // 最后一个阶段自动停止
    if (m_currentPhase >= PhaseCount - 1) {
        m_timer->stop();
        m_btnPause->setEnabled(false);
    }
}

void RangeDeblurAnimation::onNextPhase()
{
    if (m_currentPhase < PhaseCount - 1) {
        m_currentPhase++;
        updatePhase();
    }
}

void RangeDeblurAnimation::onPauseClicked()
{
    if (m_paused) {
        m_timer->start();
        m_btnPause->setText("暂停");
    } else {
        m_timer->stop();
        m_btnPause->setText("继续");
    }
    m_paused = !m_paused;
}

void RangeDeblurAnimation::onResetClicked()
{
    m_timer->stop();
    m_currentPhase = 0;
    m_paused       = false;
    m_btnPause->setText("暂停");
    m_btnPause->setEnabled(true);
    updatePhase();
    m_btnStart->setVisible(true);
    m_btnPause->setVisible(false);
    m_btnPause->setText("暂停");
    m_btnPause->setEnabled(true);
    //m_timer->start();
}

void RangeDeblurAnimation::reset()
{
    onResetClicked();
}
