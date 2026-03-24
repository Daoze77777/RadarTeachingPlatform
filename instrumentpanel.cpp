#include "InstrumentPanel.h"

InstrumentPanel::InstrumentPanel(QWidget *parent) : QWidget(parent)
{
    this->setFixedWidth(200);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->setAlignment(Qt::AlignTop);

    // ===== 仪器面板信息模块 =====
    QLabel* title1 = new QLabel("仪器面板信息", this);
    title1->setAlignment(Qt::AlignCenter);
    title1->setStyleSheet(
        "background-color: #E6EFFF; padding: 6px;"
        "font-weight: bold; color:#345196; border-radius: 3px;");
    mainLayout->addWidget(title1);
    mainLayout->addSpacing(10);

    mainLayout->addWidget(createPanelItem("电源", "power",  "green"));
    mainLayout->addWidget(createPanelItem("通信", "comm",   "green"));
    mainLayout->addWidget(createPanelItem("传感", "sensor", "gray"));
    mainLayout->addWidget(createPanelItem("端口", "port",   "gray"));
    mainLayout->addSpacing(10);

    // ===== 目标模块 =====
    QLabel* title2 = new QLabel("目标", this);
    title2->setAlignment(Qt::AlignCenter);
    title2->setStyleSheet(
        "background-color: #E6EFFF; padding: 6px;"
        "color: #345196; font-weight: bold; border-radius: 3px;");
    mainLayout->addWidget(title2);
    mainLayout->addSpacing(10);

    QLabel* posLabel = new QLabel("位置:", this);
    posLabel->setStyleSheet("color: #345196; font-weight: bold; padding-left: 15px;");
    mainLayout->addWidget(posLabel);

    // T灯区域：单独容器，方便动态增删
    QWidget* targetContainer = new QWidget(this);
    m_targetLayout = new QVBoxLayout(targetContainer);
    m_targetLayout->setContentsMargins(0, 0, 0, 0);
    m_targetLayout->setSpacing(0);
    mainLayout->addWidget(targetContainer);
}

void InstrumentPanel::setTargetLightCount(int count)
{
    // 1. 清除旧 T 灯：从 map 里移除，从布局里删除 widget
    for (int i = 1; i <= m_txCount; ++i)
        m_indicatorLights.remove(QString("T%1").arg(i));

    while (QLayoutItem* layoutItem = m_targetLayout->takeAt(0)) {
        if (QWidget* w = layoutItem->widget())
            w->deleteLater();
        delete layoutItem;
    }

    // 2. 记录新数量，创建新 T 灯（初始全灰）
    m_txCount = count;
    for (int i = 1; i <= m_txCount; ++i) {
        QString tName = QString("T%1").arg(i);
        m_targetLayout->addWidget(createPanelItem(tName, tName, "gray"));
    }
}

void InstrumentPanel::resetAllTargetLights()
{
    for (int i = 1; i <= m_txCount; ++i)
        setLightColor(QString("T%1").arg(i), "gray");
}

void InstrumentPanel::setLightColor(const QString& id, const QString& color)
{
    if (!m_indicatorLights.contains(id)) return;

    QString colorHex = "#A0A0A0"; // 灰色
    if      (color == "green") colorHex = "#33FF33";
    else if (color == "red")   colorHex = "#FF3333";

    m_indicatorLights[id]->setStyleSheet(getLightStyleSheet(colorHex));
}

QString InstrumentPanel::getLightStyleSheet(const QString& colorHex)
{
    return QString(
        "QLabel {"
        "  background-color: qradialgradient("
        "    cx:0.5, cy:0.5, radius:0.5, fx:0.3, fy:0.3,"
        "    stop:0 white, stop:1 %1);"
        "  border: 2px solid #888888;"
        "  border-radius: 8px;"
        "}"
    ).arg(colorHex);
}

QWidget* InstrumentPanel::createPanelItem(const QString& text, const QString& id, const QString& initialColor)
{
    QWidget* container = new QWidget(this);
    QVBoxLayout* vLayout = new QVBoxLayout(container);
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(0);

    QWidget* itemWidget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(itemWidget);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(10);

    // 左侧标签
    QLabel* label = new QLabel(text, itemWidget);
    label->setFixedSize(58, 24);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet(
        "QLabel {"
        "  background-color: qlineargradient("
        "    x1:0, y1:0, x2:0, y2:1,"
        "    stop:0 #F5F7FA, stop:1 #E4E8F0);"
        "  border: 1px solid #C0C8D0;"
        "  border-radius: 3px;"
        "  color: #333333;"
        "}");

    // 右侧指示灯
    QLabel* lightLabel = new QLabel(itemWidget);
    lightLabel->setFixedSize(16, 16);

    // 先存入 map，再设置初始颜色
    m_indicatorLights.insert(id, lightLabel);
    setLightColor(id, initialColor);

    layout->addWidget(label);
    layout->addStretch();
    layout->addWidget(lightLabel);

    vLayout->addWidget(itemWidget);
    return container;
}
