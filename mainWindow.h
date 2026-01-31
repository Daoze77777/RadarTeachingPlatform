#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "customMainWindow.h"
#include "experimentTypes.h"
#include "teachingMainWindow.h"

class MainWindow : public CustomMainWindow
{
    Q_OBJECT

public:
   explicit MainWindow(TeachingMainWindow *teachingWindow = nullptr, ExperimentType expType = EXP_PULSE_METHOD,QWidget *parent = nullptr);
    ~MainWindow();

protected:

private:
    void initUI();               //主入口
    void setupWindowBase();
    void setupTitleBar();         // 初始化顶部标题栏
    void setupLeftSidebar();    // 初始化左侧导航树
    void setupRightPanel();     // 初始化右侧仪器面板
    void setupCenterArea();     // 初始化中间区域

    // 左中右核心容器
    QWidget *m_headerWidget;
    QWidget *m_leftSidebarContainer;
    QStackedWidget *m_centerStack;
    QWidget *m_rightPanelContainer;

    //成员变量
    QWidget* m_rootContainer;  //顶层容器
    QVBoxLayout* m_rootLayout;  //顶层垂直布局
    TeachingMainWindow* m_teachingWindow;


};
#endif // MAINWINDOW_H
