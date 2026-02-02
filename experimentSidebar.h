// experimentsidebar.h
#ifndef EXPERIMENTSIDEBAR_H
#define EXPERIMENTSIDEBAR_H

#include <QWidget>
#include "collapsiblegroup.h" // 引用你现有的类

class ExperimentSidebar : public QWidget
{
    Q_OBJECT
public:
    // 使用枚举，让调用者更清晰
    enum GroupType {
        RadarComponents = 0, // 0 雷达组件
        Principles,          // 1 测量原理
        Steps,               // 2 实验步骤
        Courses              // 3 相关课程
    };

    explicit ExperimentSidebar(QWidget *parent = nullptr);

    void resetAllGroups();
    CollapsibleGroup* getGroup(GroupType type);

private:
    CollapsibleGroup* m_groups[4]; // 固定存储4个组
};

#endif // EXPERIMENTSIDEBAR_H
