#include "oscilloScopeWidget.h"
#include "pulseWaveforms.h"
#include "FMWaveforms.h"
#include "PhaseWaveforms.h"
#include "DeblurWaveforms.h"
#include "TrackingWaveforms.h"
#include <QVBoxLayout>

OscilloscopeWidget::OscilloscopeWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    m_plot = new QCustomPlot(this);
    layout->addWidget(m_plot);
    layout->setContentsMargins(0, 0, 0, 0);

    setupPlot();
    registerWaveforms();

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &OscilloscopeWidget::onRefreshTick);
    m_timer->start(30);
}

void OscilloscopeWidget::setupPlot()
{
    m_graph = m_plot->addGraph();
    m_graph->setPen(QPen(QColor(0, 150, 80), 2));
    m_plot->setBackground(QBrush(Qt::white));

    QPen pen(Qt::lightGray);
    m_plot->xAxis->setBasePen(pen);
    m_plot->yAxis->setBasePen(pen);
    m_plot->xAxis->setTickPen(pen);
    m_plot->yAxis->setTickPen(pen);

    m_plot->xAxis->setSubTicks(false);
    m_plot->yAxis->setSubTicks(false);
    m_plot->xAxis2->setVisible(true);
    m_plot->yAxis2->setVisible(true);
    m_plot->xAxis2->setTickLabels(false);
    m_plot->yAxis2->setTickLabels(false);
    m_plot->xAxis2->setTicks(false);
    m_plot->yAxis2->setTicks(false);
    m_plot->xAxis2->setBasePen(pen);
    m_plot->yAxis2->setBasePen(pen);

    m_plot->yAxis->grid()->setZeroLinePen(Qt::NoPen);//y轴零线非实线
}

void OscilloscopeWidget::registerWaveforms()
{
    // 脉冲法波形
    m_waveforms["TriggerPulse"]         = std::make_shared<TriggerPulseWaveform>();
    m_waveforms["PulseModulation"]      = std::make_shared<PulseModulationWaveform>();
    m_waveforms["IntermediateFrequency"]= std::make_shared<IntermediateFrequencyWaveform>();
    m_waveforms["LocalOscillator"]      = std::make_shared<LocalOscillatorWaveform>();
    m_waveforms["RFEmit"]               = std::make_shared<RFEmitWaveform>();
    m_waveforms["RFReceive"]            = std::make_shared<RFReceiveWaveform>();
    m_waveforms["Baseband"]             = std::make_shared<BasebandWaveform>();
    m_waveforms["Detector"]             = std::make_shared<DetectorWaveform>();
    m_waveforms["Threshold"]            = std::make_shared<ThresholdWaveform>();
    m_waveforms["Branch"]               = std::make_shared<BranchWaveform>();
    m_waveforms["Differential"]         = std::make_shared<DifferentialWaveform>();
    m_waveforms["Tributary"]            = std::make_shared<TributaryWaveform>();

    // Distance 系列单独持有，方便后续更新 peakTime
    m_distancePulseWaveform = std::make_shared<DistancePulseWaveform>();
    m_distanceUsWaveform    = std::make_shared<DistanceUsWaveform>();
    m_waveforms["Distance"]   = m_distancePulseWaveform;
    m_waveforms["DistanceUs"] = m_distanceUsWaveform;

    // 调频法波形
    m_waveforms["FMEmpty"]         = std::make_shared<FMEmptyWaveform>();
    m_waveforms["FMTransmission"]  = std::make_shared<FMTransmissionWaveform>();
    m_waveforms["FMReception"]     = std::make_shared<FMReceptionWaveform>();
    m_waveforms["MixedFrequency"]  = std::make_shared<MixedFrequencyWaveform>();
    m_waveforms["FrequencyMeter"]  = std::make_shared<FrequencyMeterWaveform>();
    m_fmResultWaveform = std::make_shared<FMResultWaveform>();
    m_waveforms["FMResult"] = m_fmResultWaveform;

    //相位法波形
    m_waveforms["PhaseEmpty"]        = std::make_shared<PhaseEmptyWaveform>();
    m_waveforms["TransmitSignal1"]   = std::make_shared<TransmitSignal1Waveform>();
    m_waveforms["TransmitSignal2"]   = std::make_shared<TransmitSignal2Waveform>();
    m_waveforms["ReceiveSignal1"]    = std::make_shared<ReceiveSignal1Waveform>();
    m_waveforms["ReceiveSignal2"]    = std::make_shared<ReceiveSignal2Waveform>();
    m_waveforms["Oscillation"]       = std::make_shared<OscillationWaveform>();
    m_waveforms["ReceiverMixer"]     = std::make_shared<ReceiverMixerWaveform>();
    m_waveforms["PhaseShifterZero"]  = std::make_shared<PhaseShifterZeroWaveform>();
    m_phaseResultWaveform = std::make_shared<PhaseResultWaveform>();
    m_waveforms["PhaseResult"] = m_phaseResultWaveform;

    //距离退模糊波形
    m_waveforms["DeblurEmpty"]             = std::make_shared<DeblurEmptyWaveform>();
    m_waveforms["DeblurTS1"]               = std::make_shared<DeblurTS1Waveform>();
    m_waveforms["DeblurRS1"]               = std::make_shared<DeblurRS1Waveform>();
    m_waveforms["DeblurTS2"]               = std::make_shared<DeblurTS2Waveform>();
    m_waveforms["DeblurRS2"]               = std::make_shared<DeblurRS2Waveform>();
    m_waveforms["CoincidentTransmitted"]   = std::make_shared<CoincidentTransmittedWaveform>();
    m_waveforms["CoincidentReceived"]      = std::make_shared<CoincidentReceivedWaveform>();
    m_deblurResultWaveform = std::make_shared<DeblurResultWaveform>();
    m_waveforms["DeblurResult"] = m_deblurResultWaveform;

    //距离跟踪波形
    m_waveforms["TrackingEmpty"]    = std::make_shared<TrackingEmptyWaveform>();
    m_waveforms["TrackingTrigger"]  = std::make_shared<TrackingTriggerWaveform>();
    m_waveforms["Echo"]             = std::make_shared<EchoWaveform>();
    m_waveforms["TrackingSawtooth"] = std::make_shared<TrackingSawtoothWaveform>();
    m_waveforms["Potentiometer"]    = std::make_shared<PotentiometerWaveform>();
    m_waveforms["ManuallyTrack"]    = std::make_shared<ManuallyTrackWaveform>();
    m_waveforms["ManualCapture"] = std::make_shared<ManualCaptureWaveform>();
    m_waveforms["FrontGate"] = std::make_shared<FrontGateWaveform>();
    m_waveforms["RearGate"]  = std::make_shared<RearGateWaveform>();
    m_waveforms["AutoTrack"] = std::make_shared<AutoTrackWaveform>();
}

void OscilloscopeWidget::setData(const QString &waveform)
{
    // DistanceUs 模式由 onDistanceWaveformRequested 专门设置，
    // 其他 setData 调用不应打断它
    if (m_isDistanceUsMode && !waveform.isEmpty()) return;

    m_isDistanceUsMode = false;

    if (waveform.isEmpty()) {
        m_currentWaveform = nullptr;
    } else {
        m_currentWaveform = m_waveforms.value(waveform, nullptr);
        qDebug() << "[示波器] setData:" << waveform << "找到波形:" << (m_currentWaveform != nullptr);
    }
}

void OscilloscopeWidget::onRefreshTick()
{
    // DistanceUs 模式静态显示，不走 timer 刷新
    if (m_isDistanceUsMode) return;

    // 清除 Distance 添加的额外 graph，只保留 graph(0)
    while (m_plot->graphCount() > 1)
        m_plot->removeGraph(m_plot->graphCount() - 1);

    // 重置 ticker，防止不同波形之间互相污染
    m_plot->xAxis->setTicker(QSharedPointer<QCPAxisTicker>(new QCPAxisTicker));
    m_plot->yAxis->setTicker(QSharedPointer<QCPAxisTicker>(new QCPAxisTicker));
    m_graph->setBrush(Qt::NoBrush);

    // 空白模式
    if (!m_currentWaveform) {
        m_graph->data()->clear();
        m_plot->xAxis->setLabel("时间 (µs)");
        m_plot->yAxis->setLabel("电压 (V)");
        m_plot->xAxis->setRange(0, 3050);
        m_plot->yAxis->setRange(-0.1, 1.3);
        m_plot->yAxis->setNumberFormat("f");
        m_plot->yAxis->setNumberPrecision(1);

        QSharedPointer<QCPAxisTickerText> yTicker(new QCPAxisTickerText);
        for (double v : {-0.1, 0.1, 0.3, 0.5, 0.7, 0.9, 1.1, 1.3})
            yTicker->addTick(v, QString::number(v, 'f', 1));
        m_plot->yAxis->setTicker(yTicker);

        m_plot->replot();
        return;
    }

    // 生成波形数据
    QVector<double> x, y;
    m_currentWaveform->generate(x, y);

    // 配置坐标轴
    applyAxisConfig();

    m_graph->setData(x, y);

    // Distance 波形补充辅助线
    if (m_currentWaveform == m_distancePulseWaveform) {
        double peakTime = m_distancePulseWaveform->getPeakTime();

        m_plot->axisRect()->setMinimumMargins(QMargins(0, 0, 20, 0));

        QSharedPointer<QCPAxisTickerFixed> xTicker(new QCPAxisTickerFixed);
        xTicker->setTickStep(100);
        xTicker->setScaleStrategy(QCPAxisTickerFixed::ssNone);
        m_plot->xAxis->setTicker(xTicker);

        // 黄色垂直虚线
        if (m_plot->graphCount() < 2) m_plot->addGraph();
        m_plot->graph(1)->setPen(QPen(QColor(255, 200, 0), 1, Qt::DashLine));
        m_plot->graph(1)->setBrush(Qt::NoBrush);
        m_plot->graph(1)->setData({peakTime, peakTime}, {0.0, 1.0});

        // 黄色斜线
        if (m_plot->graphCount() < 3) m_plot->addGraph();
        m_plot->graph(2)->setPen(QPen(QColor(255, 200, 0), 2));
        m_plot->graph(2)->setBrush(Qt::NoBrush);
        m_plot->graph(2)->setData({0.0, 100.0}, {0.58, 0.0});
    }

    if (m_currentWaveform->needsExtraGraph()) {
        QVector<double> ex, ey;
        m_currentWaveform->generateExtra(ex, ey);
        if (m_plot->graphCount() < 2) m_plot->addGraph();
        m_plot->graph(1)->setPen(QPen(QColor(255, 200, 0), 2));
        m_plot->graph(1)->setBrush(Qt::NoBrush);
        m_plot->graph(1)->setData(ex, ey);
    }

    // Distance 波形需要额外的辅助 graph
    applyExtraGraphs();

    m_plot->replot();
}

void OscilloscopeWidget::applyAxisConfig()
{
    if (!m_currentWaveform) return;

    m_plot->xAxis->setLabel(m_currentWaveform->xLabel());
    m_plot->yAxis->setLabel(m_currentWaveform->yLabel());
    m_plot->xAxis->setRange(m_currentWaveform->xMin(), m_currentWaveform->xMax());
    m_plot->yAxis->setRange(m_currentWaveform->yMin(), m_currentWaveform->yMax());
    m_plot->yAxis->setNumberFormat("f");
    m_plot->yAxis->setNumberPrecision(1);

    // Y轴步长
    double yStep = m_currentWaveform->yStep();
    if (yStep > 0) {
        QSharedPointer<QCPAxisTickerFixed> t(new QCPAxisTickerFixed);
        t->setTickStep(yStep);
        t->setScaleStrategy(QCPAxisTickerFixed::ssNone);
        m_plot->yAxis->setTicker(t);
    }

    // X轴步长
    double xStep = m_currentWaveform->xStep();
    if (xStep > 0) {
        QSharedPointer<QCPAxisTickerFixed> t(new QCPAxisTickerFixed);
        t->setTickStep(xStep);
        t->setScaleStrategy(QCPAxisTickerFixed::ssNone);
        m_plot->xAxis->setTicker(t);
    }
}

void OscilloscopeWidget::applyExtraGraphs()
{
    // Distance 波形：黄色虚线 + 黄色斜线
    if (m_currentWaveform == m_distancePulseWaveform) {
        double peakTime = m_distancePulseWaveform->xMax() > 0
                              ? m_distancePulseWaveform->xMax() * 0.0  // peakTime 由外部设置
                              : 0;
        // peakTime 通过 DistancePulseWaveform 内部存储，这里直接读 xMax 无意义
        // 实际峰值位置需要从 generate 后的数据里取，简化处理：
        // 由于 Distance 波形比较特殊，直接在这里重新取 peakTime
        double pt = m_distancePulseWaveform->xMax(); // 占位，实际见下
        Q_UNUSED(pt);

        // 添加辅助 graph
        m_plot->axisRect()->setMinimumMargins(QMargins(0, 0, 20, 0));

        QSharedPointer<QCPAxisTickerFixed> xTicker(new QCPAxisTickerFixed);
        xTicker->setTickStep(100);
        xTicker->setScaleStrategy(QCPAxisTickerFixed::ssNone);
        m_plot->xAxis->setTicker(xTicker);
    }
}

void OscilloscopeWidget::onRadarAnimationFinished(double pulseTimeUs)
{
    if (m_isDistanceUsMode) return;

    m_distancePulseWaveform->setPeakTime(pulseTimeUs);
    m_currentWaveform = m_distancePulseWaveform;

    // 立即绘制一次
    QVector<double> x, y;
    m_currentWaveform->generate(x, y);

    while (m_plot->graphCount() > 1)
        m_plot->removeGraph(m_plot->graphCount() - 1);
    m_graph->setBrush(Qt::NoBrush);

    m_plot->xAxis->setLabel("时间 (µs)");
    m_plot->yAxis->setLabel("电压 (V)");
    m_plot->xAxis->setRange(0, 1000);
    m_plot->yAxis->setRange(0, 1.0);
    m_plot->yAxis->setNumberFormat("f");
    m_plot->yAxis->setNumberPrecision(1);

    QSharedPointer<QCPAxisTickerFixed> xTicker(new QCPAxisTickerFixed);
    xTicker->setTickStep(100);
    xTicker->setScaleStrategy(QCPAxisTickerFixed::ssNone);
    m_plot->xAxis->setTicker(xTicker);
    m_plot->axisRect()->setMinimumMargins(QMargins(0, 0, 20, 0));

    m_graph->setData(x, y);

    // 黄色垂直虚线
    if (m_plot->graphCount() < 2) m_plot->addGraph();
    m_plot->graph(1)->setPen(QPen(QColor(255, 200, 0), 1, Qt::DashLine));
    m_plot->graph(1)->setBrush(Qt::NoBrush);
    m_plot->graph(1)->setData({pulseTimeUs, pulseTimeUs}, {0.0, 1.0});

    // 黄色斜线
    if (m_plot->graphCount() < 3) m_plot->addGraph();
    m_plot->graph(2)->setPen(QPen(QColor(255, 200, 0), 2));
    m_plot->graph(2)->setBrush(Qt::NoBrush);
    m_plot->graph(2)->setData({0.0, 100.0}, {0.58, 0.0});

    m_plot->replot();
}

void OscilloscopeWidget::onDistanceWaveformRequested(double timeUs)
{
    m_distanceUsWaveform->setPeakTime(timeUs);
    m_currentWaveform  = m_distanceUsWaveform;
    m_isDistanceUsMode = true;

    // 静态绘制，不走 timer
    while (m_plot->graphCount() > 1)
        m_plot->removeGraph(m_plot->graphCount() - 1);
    m_graph->setBrush(Qt::NoBrush);

    QVector<double> x, y;
    m_currentWaveform->generate(x, y);

    double peakTime = timeUs;
    double xMax     = qMax(peakTime * 1.5, 4.0);

    m_plot->xAxis->setLabel("时间 (µs)");
    m_plot->yAxis->setLabel("电压 (V)");
    m_plot->xAxis->setRange(0, xMax);
    m_plot->yAxis->setRange(0, 1.0);
    m_plot->yAxis->setNumberFormat("f");
    m_plot->yAxis->setNumberPrecision(1);

    QSharedPointer<QCPAxisTickerFixed> yTicker(new QCPAxisTickerFixed);
    yTicker->setTickStep(0.1);
    yTicker->setScaleStrategy(QCPAxisTickerFixed::ssNone);
    m_plot->yAxis->setTicker(yTicker);

    double xStep;
    if      (xMax <= 10.0)   xStep = 1.0;
    else if (xMax <= 50.0)   xStep = 5.0;
    else if (xMax <= 100.0)  xStep = 10.0;
    else if (xMax <= 500.0)  xStep = 50.0;
    else if (xMax <= 1000.0) xStep = 100.0;
    else                     xStep = 200.0;

    QSharedPointer<QCPAxisTickerFixed> xTicker(new QCPAxisTickerFixed);
    xTicker->setTickStep(xStep);
    xTicker->setScaleStrategy(QCPAxisTickerFixed::ssNone);
    m_plot->xAxis->setTicker(xTicker);
    m_plot->axisRect()->setMinimumMargins(QMargins(0, 0, 20, 0));

    m_graph->setData(x, y);

    // 黄色垂直虚线
    if (m_plot->graphCount() < 2) m_plot->addGraph();
    m_plot->graph(1)->setPen(QPen(QColor(255, 200, 0), 1, Qt::DashLine));
    m_plot->graph(1)->setBrush(Qt::NoBrush);
    m_plot->graph(1)->setData({peakTime, peakTime}, {0.0, 1.0});

    // 黄色斜线
    if (m_plot->graphCount() < 3) m_plot->addGraph();
    m_plot->graph(2)->setPen(QPen(QColor(255, 200, 0), 2));
    m_plot->graph(2)->setBrush(Qt::NoBrush);
    double slopeEnd = xMax * 0.12;
    m_plot->graph(2)->setData({0.0, slopeEnd}, {0.58, 0.0});

    m_plot->replot();
}

void OscilloscopeWidget::onDeblurWaveformRequested(int track, double period, double delay)
{
    if (track == 0) { setData(""); return; }

    m_deblurResultWaveform->setParams(period, delay);
    m_currentWaveform  = m_deblurResultWaveform;
    m_isDistanceUsMode = false; // 走 timer 刷新
}

void OscilloscopeWidget::onPhaseWaveformRequested(double phase1, double phase2)
{
    m_phaseResultWaveform->setParams(phase1, phase2);
    m_currentWaveform  = m_phaseResultWaveform;
    m_isDistanceUsMode = false;
}

void OscilloscopeWidget::onFMWaveformRequested(double deltaF_MHz)
{
    if (deltaF_MHz <= 0) { setData(""); return; }
    m_fmResultWaveform->setDeltaF(deltaF_MHz);
    m_currentWaveform  = m_fmResultWaveform;
    m_isDistanceUsMode = false;
}
