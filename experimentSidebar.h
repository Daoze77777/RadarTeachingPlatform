
#ifndef EXPERIMENTSIDEBAR_H
#define EXPERIMENTSIDEBAR_H

#include <QWidget>
#include "collapsiblegroup.h"
#include "sidebarListPanel.h"

class ExperimentSidebar : public QWidget
{
    Q_OBJECT
public:
    explicit ExperimentSidebar(QWidget *parent = nullptr);

    //重置所有组
    void resetAllGroups();

    // 获取特定组（如果有外部需要控制展开/折叠）
    CollapsibleGroup* getGroup(GroupType type);

    // 切换实验的接口
    void switchExperiment(int expId);

    void initConnections();

    QList<ExperimentContentItem> getStepItems() const {
        return m_config.stepGroup.items;
    }

private:
    void initUI();                  // 创建4个组的外壳

    void updateGroupContent(int groupIndex, const GroupData& data);       // 辅助函数：更新单个组的数据和可见性

private:
    CollapsibleGroup* m_groups[GroupCount];    //4个折叠组

    SidebarListPanel* m_panels[GroupCount];   // 4个内容面板

    ExperimentConfig m_config;

    QMap<int, ExperimentConfig> m_configCache;  // 保存所有加载的配置数据

signals:
    // 定义一个向外发送数据的信号
    void componentSelected(const ExperimentContentItem& item, const QString& bottomImgPath);
};

#endif // EXPERIMENTSIDEBAR_H
