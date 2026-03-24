#include "RadarDistanceWidget.h"
#include <QRandomGenerator>
#include <QtMath>

RadarDistanceWidget::RadarDistanceWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    //setupPlot();
}

void RadarDistanceWidget::setupUI()
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);
    layout->setSpacing(15);

    QLabel *lblDistTitle = new QLabel("测量距离:", this);
    lblDistTitle->setStyleSheet("font-size: 14px; font-weight: bold; color: #333;");

    m_lblDistanceValue = new QLabel("-- mm", this);
    //m_lblDistanceValue->setMinimumWidth(100);
    m_lblDistanceValue->setFixedSize(100,50);
    m_lblDistanceValue->setAlignment(Qt::AlignCenter);
    m_lblDistanceValue->setStyleSheet(
        "background-color: #f0f0f0; color: #000; border: 1px solid #999;"
        "border-radius: 4px; padding: 4px 8px; font-size: 14px;");

    QLabel *lblTimeTitle = new QLabel("往返时间:", this);
    lblTimeTitle->setStyleSheet("font-size: 14px; font-weight: bold; color: #333;");

    m_lblTimeValue = new QLabel("-- ns", this);
    //m_lblTimeValue->setMinimumWidth(100);
    m_lblTimeValue->setFixedSize(100,50);
    m_lblTimeValue->setAlignment(Qt::AlignCenter);
    m_lblTimeValue->setStyleSheet(
        "background-color: #f0f0f0; color: #000; border: 1px solid #999;"
        "border-radius: 4px; padding: 4px 8px; font-size: 14px;");

    m_btnCapture = new QPushButton("取得距离", this);
    m_btnStart   = new QPushButton("开始", this);
    m_btnReset   = new QPushButton("重置", this);

    m_btnCapture->setStyleSheet("QPushButton { background-color: #27ae60; color: white; border-radius: 4px; padding: 5px 15px; }");
    m_btnStart->setStyleSheet("QPushButton { background-color: #2980b9; color: white; border-radius: 4px; padding: 5px 15px; }");
    m_btnReset->setStyleSheet("QPushButton { background-color: #7f8c8d; color: white; border-radius: 4px; padding: 5px 15px; }");

    m_btnStart->setEnabled(false);

    layout->addWidget(lblDistTitle);
    layout->addWidget(m_lblDistanceValue);
    layout->addSpacing(10);
    layout->addWidget(lblTimeTitle);
    layout->addWidget(m_lblTimeValue);
    layout->addStretch();
    layout->addWidget(m_btnCapture);
    layout->addWidget(m_btnStart);
    layout->addWidget(m_btnReset);

    connect(m_btnCapture, &QPushButton::clicked, this, &RadarDistanceWidget::onCaptureClicked);
    connect(m_btnStart,   &QPushButton::clicked, this, &RadarDistanceWidget::onStartClicked);
    connect(m_btnReset,   &QPushButton::clicked, this, &RadarDistanceWidget::onResetClicked);
}


void RadarDistanceWidget::feedDistance(int rawDistance)
{
    m_rawDistance = rawDistance;
}

void RadarDistanceWidget::onCaptureClicked()
{
    qDebug() << "[取得距离] m_rawDistance =" << m_rawDistance;

    if (m_rawDistance <= 0 || m_rawDistance == 9999) {
        m_lblDistanceValue->setText("无效数据");
        m_lblTimeValue->setText("-- ns");
        m_btnStart->setEnabled(false);
        return;
    }

    m_capturedMm     = m_rawDistance * 0.1;
    m_capturedTimeNs = m_capturedMm * 2.0 / 300.0;

    m_lblDistanceValue->setText(QString("%1 mm").arg(m_capturedMm, 0, 'f', 1));
    m_lblTimeValue->setText(QString("%1 ns").arg(m_capturedTimeNs, 0, 'f', 3));

    m_hasCaptured = true;
    m_btnStart->setEnabled(true);
}
void RadarDistanceWidget::onStartClicked()
{
    if (!m_hasCaptured) return;
    emit waveformRequested(m_capturedTimeNs); // 通知示波器画波形
}

void RadarDistanceWidget::onResetClicked()
{
    reset();
}

void RadarDistanceWidget::reset()
{
    m_rawDistance    = 0;
    m_capturedMm     = 0.0;
    m_capturedTimeNs = 0.0;
    m_hasCaptured    = false;
    m_lblDistanceValue->setText("-- mm");
    m_lblTimeValue->setText("-- ns");
    m_btnStart->setEnabled(false);
    emit waveformRequested(-1); // 通知示波器清空，-1表示重置
}

