#ifndef RADARDATAMODEL_H
#define RADARDATAMODEL_H

#include <QString>
#include <QList>
#include <QMap>

// 折叠组按钮
enum GroupType {
    RadarComponents = 0, // 0 雷达组件
    Principles,          // 1 测量原理
    Steps,               // 2 实验步骤
    Courses,              // 3 相关课程
    GroupCount           // 4: 计数用
};

//按钮类型(按钮是否显示序号)
enum class PanelStyle {
    Standard,  // 雷达组件、测量原理使用的复选样式
    Numbered     // 实验步骤使用的带序号样式
};

// 单个按钮组件的数据
struct ExperimentContentItem {
    QString id;           // 唯一标识，如 "modulator"
    QString title;        // 按钮显示的文字，如 "脉冲调制器"
    QString imagePath;    // 右侧要显示的图片路径
    QString description;  // 右侧要显示的文字内容
};

// 单个折叠组的数据包
struct GroupData {
    bool isVisible = true;      // 对应 XML 中的 visible="..."
    QList<ExperimentContentItem> items;   // 对应 XML 中的 <Item> 列表
};

// 一个实验的完整配置
struct ExperimentConfig {
    int id;
    QString name;
    QString fixedBottomImage; // 用于存储从 XML 读到的底部图路径

    // 四个组的数据
    GroupData radarGroup;     // 雷达组件
    GroupData principleGroup; // 测量原理
    GroupData stepGroup;      // 实验步骤
    GroupData courseGroup;    // 相关课程
};

#endif // RADARDATAMODEL_H
