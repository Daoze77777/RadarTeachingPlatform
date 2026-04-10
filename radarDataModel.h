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
    Courses,             // 3 相关课程
    GroupCount           // 4: 计数用
};

// 按钮类型(按钮是否显示序号)
enum class PanelStyle {
    Standard,  // 雷达组件、测量原理使用的复选样式
    Numbered   // 实验步骤使用的带序号样式
};

// 单个按钮组件的数据
struct ExperimentContentItem {
    QString id;          // 唯一标识（步骤内唯一即可，不同实验可重名）
    QString title;       // 按钮显示的文字
    QString imagePath;   // 右侧要显示的图片路径
    QString description; // 右侧要显示的文字内容
    QString moduleType;  // "Component" / "Step" / "Course"
    QString txBit;       // 格式 "s1:1"/"s2:16"，空字符串表示无对应灯

    // ===== 新增字段 =====
    // 对应示波器波形名，空字符串表示该步骤无波形
    // 例如："TriggerPulse" / "PulseModulation" / "FMTransmit" 等
    QString waveform;

    // 特殊行为标记，替代 mainWindow 里硬编码的 id 判断
    // "noLight"          — 不需要灯就直接显示（如 s1 设置目标位置）
    // "distanceMeasure"  — 显示测距控件（如 s14）
    // 空字符串           — 普通步骤，走灯亮/灭逻辑
    QString special;
};

// 单个折叠组的数据包
struct GroupData {
    bool isVisible = true;
    QList<ExperimentContentItem> items;
};

// 一个实验的完整配置
struct ExperimentConfig {
    int id = 0;
    QString name;
    QString fixedBottomImage;
    QString expType;  // "demo" 或 "test"
    int txCount = 0;

    GroupData radarGroup;
    GroupData principleGroup;
    GroupData stepGroup;
    GroupData courseGroup;
};

#endif // RADARDATAMODEL_H
