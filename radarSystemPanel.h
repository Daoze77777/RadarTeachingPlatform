#ifndef RADARSYSTEMPANEL_H
#define RADARSYSTEMPANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QButtonGroup>
#include "RadarDataModel.h" // 包含之前定义的结构体
#include "collapsibleGroup.h"

// 假设 CheckboxButton 是你自定义的类
// class CheckboxButton;

class RadarSystemPanel : public QWidget {
    Q_OBJECT
public:
    explicit RadarSystemPanel(QWidget *parent = nullptr);

    // 根据从 XML 解析的数据更新面板按钮
    void updatePanel(const ExperimentRadarConfig& config);

signals:
    // 当某个按钮被选中时，发送该组件的完整数据（包含图片路径和描述）
    void itemSelected(const RadarComponentItem& itemData);

private slots:
    void onBtnClicked(QAbstractButton* btn);

private:
    QScrollArea* m_scrollArea;
    QWidget* m_container;
    QVBoxLayout* m_containerLayout;
    QButtonGroup* m_btnGroup;
    QList<RadarComponentItem> m_currentItems; // 存储当前实验的所有组件数据
};

#endif
