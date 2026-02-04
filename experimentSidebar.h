// experimentsidebar.h
#ifndef EXPERIMENTSIDEBAR_H
#define EXPERIMENTSIDEBAR_H

#include <QWidget>
#include "collapsiblegroup.h" // 引用你现有的类
#include "radarSystemPanel.h"   // 引入刚才写的雷达面板
#include "radarConfigLoader.h"  // 引入配置加载器

// 使用枚举，让调用者更清晰
enum GroupType {
    RadarComponents = 0, // 0 雷达组件
    Principles,          // 1 测量原理
    Steps,               // 2 实验步骤
    Courses              // 3 相关课程
};

class ExperimentSidebar : public QWidget
{
    Q_OBJECT
public:
    explicit ExperimentSidebar(QWidget *parent = nullptr);
    void resetAllGroups();
    CollapsibleGroup* getGroup(GroupType type);
    // 切换实验的接口
    void switchExperiment(int experimentType);

private:
    CollapsibleGroup* m_groups[4]; // 固定存储4个组

    // 【新增】保存雷达面板指针，因为我们需要频繁调用它的 updatePanel
    RadarSystemPanel* m_radarPanel = nullptr;
    // 【新增】保存所有加载的配置数据
    QMap<int, ExperimentRadarConfig> m_radarConfigs;
    // 内部初始化函数
    void initUI();         // 创建4个组的外壳（你现在的代码）
    void loadRadarData();    // 读取XML数据的函数
    void initRadarContent();     // 填充第0组：雷达组件
    //void initPrincipleContent(); // 填充第1组：测量原理
    //void initStepContent();      // 填充第2组：实验步骤
};

#endif // EXPERIMENTSIDEBAR_H
