#include "RadarSystemPanel.h"
#include <QPushButton>
#include <QDebug>
#include "collapsibleGroup.h"

RadarSystemPanel::RadarSystemPanel(QWidget *parent)
    :QWidget(parent)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // 1. 初始化滚动区域
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);     // 屏蔽横向滚动条
    m_scrollArea->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);    //必须设置为Fixed，防止自动扩张
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);    // 让滚动条在不滚动时自动隐藏，且不占用布局空间
    m_scrollArea->setStyleSheet(R"(
    QScrollArea {
        background: transparent;
    }
    QScrollBar:vertical {
        width: 6px;                 /* 极简细长风格 */
        background: transparent;    /* 背景透明 */
        margin: 0px;
    }
    QScrollBar::handle:vertical {
        background: rgba(255, 255, 255, 0.3); /* 半透明白色，适配深色背景 */
        min-height: 30px;
        border-radius: 3px;         /* 圆角滑块 */
    }
    QScrollBar::handle:vertical:hover {
        background: rgba(255, 255, 255, 0.5); /* 悬停时稍微亮一点 */
    }
    QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
        height: 0px;                /* 隐藏上下箭头 */
    }
    QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
        background: none;           /* 隐藏轨道背景 */
    }
)");

    m_container = new QWidget();
    //垂直布局
    m_containerLayout = new QVBoxLayout(m_container);
    m_containerLayout->setContentsMargins(0, 0, 8, 0);
    m_containerLayout->setSpacing(15); // 按钮间距
    m_containerLayout->setAlignment(Qt::AlignTop);

    m_scrollArea->setWidget(m_container);
    mainLayout->addWidget(m_scrollArea);

    // 2. 初始化按钮组（实现单选互斥）
    m_btnGroup = new QButtonGroup(this);
    m_btnGroup->setExclusive(true);

    connect(m_btnGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),this, &RadarSystemPanel::onBtnClicked);
}

void RadarSystemPanel::updatePanel(const ExperimentRadarConfig& config) {
    // 清理：删除旧按钮
    QLayoutItem *child;

    while ((child = m_containerLayout->takeAt(0)) != nullptr)
    {
        if (child->widget()) child->widget()->deleteLater();
        delete child;
    }

    for (auto btn : m_btnGroup->buttons())
    {
        m_btnGroup->removeButton(btn);
    }

    // 同步数据
    m_currentItems = config.items;

    // 动态创建新按钮
    for (int i = 0; i < m_currentItems.size(); ++i) {
        // 使用你现有的 CheckboxButton 类
        CheckboxButton* btn = new CheckboxButton(m_currentItems[i].title, m_container);
        m_containerLayout->addWidget(btn);
        m_btnGroup->addButton(btn, i); //将索引作为 ID 绑定
    }

    // 4. 布局与高度刷新 强制要求容器重新计算布局
    m_container->adjustSize();

    // 获取布局算出来的【真实总高度】（包含所有按钮高度、间距、边距）
    int totalHeight = m_containerLayout->sizeHint().height();
    qDebug()<<totalHeight;

    // 限制一个最大高度（例如 450px），超过则显示滚动条
    int displayHeight = qMin(totalHeight, 450);

    m_scrollArea->setFixedHeight(displayHeight);
    this->setFixedHeight(displayHeight);      // 视图层固定高度

    // 7. 通知上层 UI 刷新
    this->updateGeometry();

    // 默认选中第一个并触发显示
    // if (!m_btnGroup->buttons().isEmpty()) {
    // m_btnGroup->buttons().first()->setChecked(true);
    // emit itemSelected(m_currentItems.first());
    // }
}

void RadarSystemPanel::onBtnClicked(QAbstractButton* btn) {
    int index = m_btnGroup->id(btn);
    if (index >= 0 && index < m_currentItems.size()) {
        // 将 XML 中配置的图片和文字数据传出去
        emit itemSelected(m_currentItems[index]);
    }
}

QSize RadarSystemPanel::sizeHint() const {
    // 返回我们 setFixedHeight 设置的高度，或者 layout 的推荐高度
    return QSize(QWidget::sizeHint().width(), this->height());
}
