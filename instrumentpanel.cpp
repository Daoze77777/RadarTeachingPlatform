#include "InstrumentPanel.h"

InstrumentPanel::InstrumentPanel(QWidget *parent) : QWidget(parent)
{
    // 设置自身的固定宽度和背景色
    this->setFixedWidth(200);
    //this->setStyleSheet("InstrumentPanel { background-color: #F0F4F8; border-left: 1px solid #D0D0D0; }");
    //this->setStyleSheet("background-color:pink;");

    // 主垂直布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    //mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->setAlignment(Qt::AlignTop);

    // --- 1. 仪器面板信息 模块 ---
    QLabel* title1 = new QLabel("仪器面板信息", this);
    title1->setAlignment(Qt::AlignCenter);
    title1->setStyleSheet("background-color: #E6EFFF; padding: 6px; font-weight: bold; color:#345196; border-radius: 3px;");
    mainLayout->addWidget(title1);
    mainLayout->addSpacing(10); // 模块间距

    // 添加指示灯条目（文本，内部ID，初始颜色）
    mainLayout->addWidget(createPanelItem("电源", "power", "gray"));
    mainLayout->addWidget(createPanelItem("通信", "comm", "red")); // 截图里通信是红的
    mainLayout->addWidget(createPanelItem("传感", "sensor", "green"));
    mainLayout->addWidget(createPanelItem("端口", "port", "gray"));
    mainLayout->addSpacing(10); // 模块间距

    // --- 2. 目标位置 模块 ---
    QLabel* title2 = new QLabel("目标", this);
    title2->setAlignment(Qt::AlignCenter);
    title2->setStyleSheet("background-color: #E6EFFF; padding: 6px; color: #345196; font-weight: bold; border-radius: 3px;");
    mainLayout->addWidget(title2);
    mainLayout->addSpacing(10);

    QLabel* posLabel = new QLabel("位置:", this);
    posLabel->setStyleSheet("color: #345196; font-weight: bold; padding-left: 15px;");
    mainLayout->addWidget(posLabel);

    // 批量生成 T1 到 T12
    for (int i = 1; i <= 12; ++i) {
        QString tName = QString("T%1").arg(i);
        mainLayout->addWidget(createPanelItem(tName, tName, "gray"));
    }
}

QWidget* InstrumentPanel::createPanelItem(const QString& text, const QString& id, const QString& initialColor)
{
    QWidget* container = new QWidget(this);
    QVBoxLayout* vLayout = new QVBoxLayout(container);
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(0);

    QWidget* itemWidget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(itemWidget);
    layout->setContentsMargins(10, 10, 10, 10); // 紧密贴合
    layout->setSpacing(10);

    // 左侧：凸起标签
    QLabel* label = new QLabel(text, itemWidget);
    label->setFixedSize(58, 24);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet(
        "QLabel {"
        "  background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #F5F7FA, stop:1 #E4E8F0);"
        "  border: 1px solid #C0C8D0;"
        "  border-radius: 3px;"
        "  color: #333333;"
        "}"
        );

    // 右侧：指示灯
    QLabel* lightLabel = new QLabel(itemWidget);
    lightLabel->setFixedSize(16, 16);

    // 初始化颜色并保存到 Map
    setLightColor(id, initialColor); // 借用现成的逻辑设置初始颜色
    // 由于 setLightColor 会去 map 里找，所以这里要先存进去
    m_indicatorLights.insert(id, lightLabel);

    // 重新调用一次以应用样式
    setLightColor(id, initialColor);

    layout->addWidget(label);
    layout->addStretch(); // 将标签和灯分别推向两边
    layout->addWidget(lightLabel);

    // 2. 分割线 [按照要求添加]
    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFixedHeight(1);
    //line->setStyleSheet("background-color: #E0E0E0; margin-left: 15px; margin-right: 15px;");

    vLayout->addWidget(itemWidget);
    //vLayout->addWidget(line);

    return container;
}

void InstrumentPanel::setLightColor(const QString& id, const QString& color)
{
    if (!m_indicatorLights.contains(id)) return;

    QString colorHex = "#A0A0A0"; // 默认灰色
    if (color == "red")   colorHex = "#FF3333";
    else if (color == "green") colorHex = "#33FF33";
    // 以后还可以加 yellow, blue 等

    m_indicatorLights[id]->setStyleSheet(getLightStyleSheet(colorHex));
}

QString InstrumentPanel::getLightStyleSheet(const QString& colorHex)
{
    // 径向渐变，实现中间亮、边缘暗的 3D 发光质感
    return QString(
               "QLabel {"
               "  background-color: qradialgradient(cx:0.5, cy:0.5, radius:0.5, fx:0.3, fy:0.3, stop:0 white, stop:1 %1);"
               "  border: 2px solid #888888;"
               "  border-radius: 8px;"
               "}"
               ).arg(colorHex);
}
