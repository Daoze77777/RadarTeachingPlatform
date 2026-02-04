#ifndef RADARDATAMODEL_H
#define RADARDATAMODEL_H

#include <QString>
#include <QList>
#include <QMap>

// 1. 单个组件的数据（按钮+内容）
struct RadarComponentItem {
    QString id;           // 唯一标识，如 "modulator"
    QString title;        // 按钮显示的文字，如 "脉冲调制器"
    QString imagePath;    // 右侧要显示的图片路径
    QString description;  // 右侧要显示的文字内容
};

// 2. 一个实验的完整配置
struct ExperimentRadarConfig {
    bool isVisible = true;          // 该实验是否显示雷达组件组
    QList<RadarComponentItem> items; // 组件列表
};

#endif // RADARDATAMODEL_H
