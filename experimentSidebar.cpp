
#include "experimentSidebar.h"
#include <QVBoxLayout>

ExperimentSidebar::ExperimentSidebar(QWidget *parent) : QWidget(parent)
{
    // 1. 设置主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0); // 紧密排列

    // 2. 【关键优化】定义配置数据
    // 将变化的参数（标题、图标）提取出来，而不是硬编码在 new 语句里
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
        // 这里是你调用 3 个参数构造函数的地方
        // this 指针自动作为 parent 传入
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
// 实现获取组的接口
CollapsibleGroup* ExperimentSidebar::getGroup(GroupType type)
{
    if (type >= 0 && type < 4) {
        return m_groups[type];
    }
    return nullptr;
}
// 实现重置接口
void ExperimentSidebar::resetAllGroups()
{
    for (int i = 0; i < 4; ++i) {
        // 这里调用你之前添加的 clearContent()
        if (m_groups[i]) m_groups[i]->clearContent();
    }
}
