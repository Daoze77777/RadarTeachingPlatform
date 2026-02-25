#ifndef SIDEBARLISTPANEL_H
#define SIDEBARLISTPANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QButtonGroup>
#include "RadarDataModel.h"

class SidebarListPanel : public QWidget {

    Q_OBJECT

public:
    explicit SidebarListPanel(PanelStyle style, QWidget *parent = nullptr);

    // 根据从 XML 解析的数据更新面板按钮
    void updateList(const QList<ExperimentContentItem>& items);

    // 重写 sizeHint，这是 Qt 布局系统沟通尺寸的标准方式
    QSize sizeHint() const override;

private:
    void initUI();

signals:
    // 当某个按钮被选中时，发送该组件的完整数据（包含图片路径和描述）
    void itemClicked(const ExperimentContentItem& item); // 定义内部信号


private:
    PanelStyle m_style;     // 存储当前面板的样式
    QScrollArea* m_scrollArea;
    QWidget* m_container;
    QVBoxLayout* m_containerLayout;
    QButtonGroup* m_btnGroup;
    QList<ExperimentContentItem> m_currentItems; // 存储当前实验的所有组件数据
};

#endif
