#ifndef RANGEDEBLURANIMATION_H
#define RANGEDEBLURANIMATION_H

#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "qcustomplot.h"

class RangeDeblurAnimation : public QWidget
{
    Q_OBJECT

public:
    explicit RangeDeblurAnimation(QWidget *parent = nullptr);
    void reset();

private slots:
    void onNextPhase();
    void onPauseClicked();
    void onResetClicked();

private:
    void setupUI();
    void setupPlot();
    void updatePhase();         // 根据当前阶段更新图形和文字
    void drawStaticWaveforms(); // 画静态底图：发射脉冲串、回波、噪声
    void clearHighlights();     // 清除所有高亮框

    // 阶段枚举
    enum Phase {
        Phase1_Transmitting = 0,  // 发射中
        Phase2_SwitchRX,          // 收发切换
        Phase3_NormalRange,       // 正常测距
        Phase4_SecondTX,          // 第二次发射
        Phase5_Ambiguity,         // 距离模糊产生
        Phase6_Summary,           // 总结
        PhaseCount
    };

    QCustomPlot  *m_plot;
    QLabel       *m_phaseTitle;   // 阶段标题
    QLabel       *m_phaseDesc;    // 阶段描述
    QPushButton  *m_btnPause;
    QPushButton  *m_btnReset;
    QPushButton *m_btnStart;        //开始按钮

    QTimer       *m_timer;
    int           m_currentPhase = 0;
    bool          m_paused       = false;

    // 静态波形 graph 索引
    // graph(0): 发射脉冲串（黑色）
    // graph(1): 噪声基线（灰色）
    // graph(2): 绿色正常回波
    // graph(3): 红色跨周期回波
    // graph(4): 远处绿色回波
    // graph(5~): 高亮框用 QCPItemRect

    QCPItemRect  *m_highlightRect = nullptr; // 当前高亮蓝框
    QCPItemEllipse *m_echoCircle  = nullptr; // 圆圈标记

    // 坐标参数（归一化到0~10的时间轴，单位：任意）
    // Tr=3，τ=0.5，t0=0.3
    // 发射脉冲位置：0, 3, 6, 9
    // 正常回波位置：1.5
    // 跨周期回波位置：4.2
    // 远处回波：7.5
};

#endif // RANGEDEBLURANIMATION_H
