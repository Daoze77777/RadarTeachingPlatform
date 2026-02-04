
#include "experimentSidebar.h"
#include <QVBoxLayout>

ExperimentSidebar::ExperimentSidebar(QWidget *parent)
    : QWidget(parent)
{
    loadRadarData();
    initUI();
    // 3. 【核心步骤】分别为不同的组填充内容
    // 这种写法清晰地将“容器创建”和“内容填充”分离开来
    initRadarContent();     // 填充雷达组件
    //initPrincipleContent(); // 填充测量原理 (占位)
    //initStepContent();      // 填充实验步骤 (占位)
}

void ExperimentSidebar::initUI()
{
    // 1. 设置主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0); // 紧密排列

    // 2. 【关键优化】定义配置数据 将变化的参数（标题、图标）提取出来，而不是硬编码在 new 语句里
    struct GroupConfig {
        QString title;
        QString iconPath;
    };

    // 配置列表：如果你想改名字或换图标，只用改这里
    GroupConfig configs[4] = {
        {"雷达组件", ":/mainicons/resources/mainIcons/leidazujian.png"},  // 对应 GroupType::RadarComponents (0)
        {"测量原理", ":/mainicons/resources/mainIcons/celiangyuanli.png"},  // 对应 GroupType::Principles (1)
        {"实验步骤", ":/mainicons/resources/mainIcons/shiyanbuzouICon.png"},  // 对应 GroupType::Steps (2)
        {"相关课程", ":/mainicons/resources/mainIcons/xaingguankechengICon.png"}   // 对应 GroupType::Courses (3)
    };

    // 3. 循环创建对象（只写一次 new）
    for (int i = 0; i < 4; ++i) {
        // 调用构造函数,this 指针自动作为 parent 传入
        m_groups[i] = new CollapsibleGroup(configs[i].title, QIcon(configs[i].iconPath), this);

        // 记得设置 ObjectName，方便调试或特殊样式
        m_groups[i]->setObjectName(QString("sidebarGroup_%1").arg(i));

        // 加入布局
        mainLayout->addWidget(m_groups[i]);

        // 4. 绑定互斥逻辑（手风琴效果）
        connect(m_groups[i], &CollapsibleGroup::expandedChanged, [this, i](bool expanded) {
            if (expanded) {
                for (int j = 0; j < 4; ++j) {
                    if (i != j) m_groups[j]->setExpanded(false);
                }
            }
        });
    }
    // 5. 底部弹簧
    mainLayout->addStretch();
}
void ExperimentSidebar::loadRadarData()
{
    // 1. 先加载数据 (避免后续卡顿)
    m_radarConfigs = RadarConfigLoader::loadAllConfigs(":/xml/resources/xml/mcfjlcl.xml");
    // 调试一下，看看读到了没有
    qDebug() << "已加载雷达配置，共" << m_radarConfigs.size() << "个实验配置";
}
// 【关键实现】如何添加子选项
void ExperimentSidebar::initRadarContent()
{
    // 1. 获取第0个组（雷达组件组）
    CollapsibleGroup* group = m_groups[RadarComponents];
    if (!group) return;

    // 2. 创建我们在上一步封装好的 RadarSystemPanel
    // 这个 Panel 会自动管理里面的复选框、布局和滚动条
    m_radarPanel = new RadarSystemPanel(this);

    // 3. 将 Panel 添加到折叠组的内容区
    // 假设 CollapsibleGroup 有 addWidget 方法，或者你可以获得它的 layout
    // 如果 CollapsibleGroup 继承自 QWidget，通常我们需要把它加到内部的 contentLayout 中
    group->addWidget(m_radarPanel);

    // 4. 连接信号：当 Panel 里的按钮被点击，转发出去
    //connect(m_radarPanel, &RadarSystemPanel::itemSelected,this, &ExperimentSidebar::radarComponentSelected);
}
// 实现获取组的接口
CollapsibleGroup* ExperimentSidebar::getGroup(GroupType type)
{
    if (type >= 0 && type < 4)
    {
        return m_groups[type];
    }
    return nullptr;
}
// 实现重置接口
void ExperimentSidebar::resetAllGroups()
{
    for (int i = 0; i < 4; ++i)
    {
        // 这里调用你之前添加的 clearContent()
        if (m_groups[i]) m_groups[i]->clearContent();
    }
}
// 切换实验的接口
void ExperimentSidebar::switchExperiment(int experimentType)
{
    // --- 1. 处理雷达组件组 ---
    if (m_radarConfigs.contains(experimentType)) {
        ExperimentRadarConfig config = m_radarConfigs[experimentType];

        // 控制组的显隐
        m_groups[RadarComponents]->setVisible(config.isVisible);

        // 如果显示，让 Panel 刷新按钮
        if (config.isVisible && m_radarPanel) {
            m_radarPanel->updatePanel(config);

            // 自动展开雷达组（可选）
            m_groups[RadarComponents]->setExpanded(true);
        }
    } else {
        m_groups[RadarComponents]->setVisible(false);
    }

    // --- 2. 处理测量原理组 (如果有对应逻辑) ---
    // updatePrinciples(experimentType);

    // --- 3. 处理实验步骤组 ---
    // updateSteps(experimentType);
}
