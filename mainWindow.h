#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "customMainWindow.h"
#include "teachingMainWindow.h"
#include "radarDataModel.h"
#include "experimentSidebar.h"
#include "OscilloScopeWidget.h"
#include "InstrumentPanel.h"
#include <QTextBrowser>

class MainWindow : public CustomMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(int expId, QWidget *parent = nullptr);
    ~MainWindow();

public:
    void onMenuExperimentSelected(int expId);

private:
    void initUI(); //主入口
    void setupWindowBase();
    void setupTitleBar();    // 初始化顶部标题栏
    void setupLeftSidebar(); // 初始化左侧导航树
    void setupRightPanel();  // 初始化右侧仪器面板
    void setupCenterArea();  // 初始化中间区域
    QWidget* createWelcomePage();   //默认欢迎页面
    QWidget* createExperimentWorkPage();    //工作区页面
    QWidget* setupComponentDetailWidget(); // 根据ID初始化雷达组件、实验原理页面的函数
    QWidget* setupStepDetailWidget();       // 根据ID初始化实验步骤页面的函数
    void setupExperimentContext(int expId);     // 统一的数据加载与界面刷新函数

private:
    // 左中右核心容器
    QWidget *m_headerWidget;
    QWidget *m_leftSidebarContainer;
    QVBoxLayout *m_leftSidebarContentLayout;
    QWidget * m_CenterContainer;
    QWidget *m_rightPanelContainer;
    ExperimentSidebar* m_sidebar;
    QStackedWidget *m_centerStack;      //内层Stack
    QStackedWidget *m_mainCenterStack;  //外层Stack
    int m_currentExpId = 101;        //记录当前实验expId,熔断机制使用

    //成员变量
    QWidget *m_rootContainer;  //顶层容器
    QVBoxLayout *m_rootLayout; //顶层垂直布局
    TeachingMainWindow *m_teachingWindow;
    QLabel *m_experimentTitle;   //实验标题
    QWidget* m_componentDetailWidget; // 通用的组件展示页
    QWidget* m_stepDetailWidget;        //实验步骤专用页面
    QLabel* m_stepPromptLabel;      //实验步骤操作提示
    QLabel* m_stepActionImage;      //实验步骤操作图
    OscilloscopeWidget* m_oscilloscope;     //实验步骤右侧示波器
    QLabel* m_stepBottomImage;      //实验步骤底部图片
    QLabel* m_detailTitleLabel;       // 组件、原理通用标题
    QLabel* m_detailDescLabel;        // 组件、原理通用描述文本
    QLabel* m_detailTopImage;         // 组件、原理通用右上图片
    QLabel* m_detailBottomImage;      // 组件、原理通用底部固定图片
    QWidget* m_experimentPage;

    InstrumentPanel* m_instrumentPanel;

private slots:
    void onComponentSelected(const ExperimentContentItem& item, const QString& bottomImgPath);

signals:
    void returnToHome(); // 告诉外部：请求返回主页
};
#endif // MAINWINDOW_H
