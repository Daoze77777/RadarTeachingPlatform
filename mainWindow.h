#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "customMainWindow.h"
#include "teachingMainWindow.h"
#include "radarDataModel.h"
#include "experimentSidebar.h"
#include "OscilloScopeWidget.h"
#include "InstrumentPanel.h"
#include "radarRangingDisplay.h"
#include "stepController.h"
#include <QTextBrowser>
#include <QSerialPort>
#include <QMessageBox>
#include "radarDataParser.h"
#include "radarDistanceWidget.h"
#include "RangeDeblurAnimation.h"

class MainWindow : public CustomMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(int expId, QWidget *parent = nullptr);
    ~MainWindow();

public:
    void onMenuExperimentSelected(int expId);

private:
    void initUI();
    void setupWindowBase();
    void setupTitleBar();
    void setupLeftSidebar();
    void setupRightPanel();
    void setupCenterArea();
    QWidget* createWelcomePage();
    QWidget* createExperimentWorkPage();
    QWidget* setupComponentDetailWidget();
    QWidget* setupStepDetailWidget();
    void setupExperimentContext(int expId);
    void initSerial();
    void closeSerial();
    void autoRefreshStep(const ExperimentContentItem &item);
    void resetStepDisplay(); // 灯灭时还原默认图片和波形

private:
    // 左中右核心容器
    QWidget *m_headerWidget;
    QWidget *m_leftSidebarContainer;
    QVBoxLayout *m_leftSidebarContentLayout;
    QWidget *m_CenterContainer;
    QWidget *m_rightPanelContainer;
    ExperimentSidebar *m_sidebar;
    QStackedWidget *m_centerStack;
    QStackedWidget *m_mainCenterStack;
    RangeDeblurAnimation *m_rangeDeblurAnim = nullptr;  //距离退模糊原理演示
    RangeDeblurAnimation *m_rangeDeblurAnimStep = nullptr; // 步骤页面用
    int m_currentExpId = 101;

    // 成员变量
    QWidget *m_rootContainer;
    QVBoxLayout *m_rootLayout;
    TeachingMainWindow *m_teachingWindow;
    QLabel *m_experimentTitle;
    QWidget *m_componentDetailWidget;
    QWidget *m_stepDetailWidget;
    QLabel *m_stepPromptLabel;
    QLabel *m_stepActionImage;
    RadarRangingDisplay *m_radarRangingDisply;
    RadarDistanceWidget *m_radarDistanceWidget;
    OscilloscopeWidget *m_oscilloscope;
    QLabel *m_stepBottomImage;
    QLabel *m_detailTitleLabel;
    QLabel *m_detailDescLabel;
    QLabel *m_detailTopImage;
    QLabel *m_detailBottomImage;
    QWidget *m_experimentPage;

    InstrumentPanel *m_instrumentPanel;

    // 串口相关
    QSerialPort    *m_serial  = nullptr;
    RadarDataParser *m_parser = nullptr;

    // 测试验证状态
    bool m_isTestMode = false;
    int  m_unlockedStepIndex = -1;
    QList<ExperimentContentItem> m_stepItems;
    QString m_currentStepId;
    RadarData m_lastRadarData;

    // 跳步检测（统一由 StepController 管理）
    StepController *m_stepCtrl = nullptr;

private slots:
    void onComponentSelected(const ExperimentContentItem &item, const QString &bottomImgPath);
    void onRadarDataReceived(const RadarData &data);

signals:
    void returnToHome();
};

#endif // MAINWINDOW_H
