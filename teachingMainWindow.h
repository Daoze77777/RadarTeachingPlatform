#ifndef TEACHINGMAINWINDOW_H
#define TEACHINGMAINWINDOW_H
#include <QGridLayout>
#include "customMainWindow.h"

class TeachingMainWindow : public CustomMainWindow
{
    Q_OBJECT
public:
    explicit TeachingMainWindow(QWidget *parent = nullptr);
    ~TeachingMainWindow();
    void setupMenu();

protected:
    bool m_platformSwitchEnabled; // 切换平台按钮是否可用（true=可点击，false=禁用）
    QLabel *m_cnTitleLabel;       // 中文标题标签
    QLabel *m_enTitleLabel;       // 英文标题标签

    // 预创建的实验页面（避免首次点击卡顿）
    QWidget *m_homePage;                      // 主页（实验选择界面）
    QList<QPushButton *> m_experimentButtons; // 当前实验按钮列表
    QWidget *m_linksWidget;                   // 链接容器
    QGridLayout *m_linksLayout;               // 链接布局

    enum PlatformType {
        Platform_Distance, // 测距
        Platform_Angle,    // 测角
        Platform_Speed     // 测速
    };

private:
    QStackedWidget *m_stackedWidget;
    QPushButton *m_switchPlatformBtn;
    int m_currentPlatformIndex = 0; // 当前平台索引
    QStringList m_platformNames = {"测距平台", "测角平台", "测速平台"};
    QList<QWidget *> m_platformPages; // 存储三个页面

private:
    void ensureSwitchPlatformButton();
    void switchPlatform(int index);
    void updateSwitchButtonPosition();
    void resizeEvent(QResizeEvent *event);
    void initUI();
    QWidget *createPlatformPage(PlatformType type);
    void setupExperimentMenu(); //设置实验菜单

    QWidget *createHomePage(); // 创建主页

private slots:
    //实验按钮槽函数
    void onPulseMethodClicked();
    void onPulseMethodTestClicked();
    void onFrequencyModulationClicked();
    void onFrequencyModulationTestClicked();
    void onPhaseMethodClicked();
    void onPhaseMethodTestClicked();
    void onRangeAmbiguityClicked();
    void onRangeAmbiguityTestClicked();
    void onRangeTrackingClicked();
    void onRangeTrackingTestClicked();
};

#endif // TEACHINGMAINWINDOW_H
