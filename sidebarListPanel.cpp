#include "sidebarListPanel.h"
#include <QPushButton>
#include <QDebug>
#include "collapsibleGroup.h"
#include "experimentSidebar.h"

SidebarListPanel::SidebarListPanel(PanelStyle style, QWidget *parent)
    : QWidget(parent)
    , m_style(style)
{
    initUI();
}

void SidebarListPanel::initUI()
{
    //创建主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 1. 初始化滚动区域
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);     // 屏蔽横向滚动条
    m_scrollArea->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);    //必须设置为Fixed，防止自动扩张
    m_scrollArea->setStyleSheet(R"(
    QScrollBar:vertical {
        width: 6px;                 /* 极简细长风格 */
        background: #D5E2FF;    /* 背景透明 */
        margin: 0px;
    }
    QScrollBar::handle:vertical {
        background: rgba(255, 255, 255, 0.7); /* 半透明白色，适配深色背景 */
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
    //m_container->setStyleSheet("background-color:pink;");

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

    //connect(m_btnGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),this, &SidebarListPanel::onBtnClicked);
}

void SidebarListPanel::updateList(const QList<ExperimentContentItem>& items) {
    // 1. 同步清理旧按钮（防止 deleteLater 的延迟干扰）
    while (QLayoutItem* child = m_containerLayout->takeAt(0)) {
        if (QWidget* w = child->widget()) {
            w->setParent(nullptr); // 彻底脱离布局
            delete w;              // 同步删除，不给第二次渲染留机会
            qDebug()<<"按钮已清理";
        }
        delete child;
    }

    // 2. 重置按钮组
    for (auto btn : m_btnGroup->buttons()) {
        m_btnGroup->removeButton(btn);
        qDebug()<<"按钮已重置";
    }

    // 2. 清除固定高度限制，让布局重置
    this->setMinimumHeight(0);
    this->setFixedHeight(QWIDGETSIZE_MAX);

    // 同步数据
    m_currentItems = items;

    // 动态创建新按钮
    for (int i = 0; i < m_currentItems.size(); ++i) {
        ExperimentContentItem item = m_currentItems[i];
        // 使用你现有的 CheckboxButton 类
        QAbstractButton* btn = nullptr;
        if (m_style == PanelStyle::Standard) {
            // 创建之前的 CheckboxButton
            btn = new CheckboxButton(m_currentItems[i].title, m_container);
        } else {
            // 创建新的带序号按钮，传入序号 i+1
            btn = new StepButton(i + 1, m_currentItems[i].title, m_container);
        }
        m_containerLayout->addWidget(btn);
        m_btnGroup->addButton(btn, i);  //将索引作为 ID 绑定

        // 注意中括号 [=]：表示“按值捕获”当前循环里的 item 和 bottomImgPath，绝对不能用 [&] 按引用捕获，否则所有按钮最后都会指向最后一个 item
        connect(btn, &QPushButton::clicked, this, [this, item]() {
            emit itemClicked(item);     //当按钮被点击时，发射侧边栏的专属信号，把刚才捕获的数据传出去
        });
        btn->show(); // 确保显式显示
    }

    // 3. 高度刷新修正
    // 确保子项已经添加完毕后再强制计算
    QCoreApplication::sendPostedEvents(this, QEvent::LayoutRequest);

    // 获取布局算出来的真实总高度（包含所有按钮高度、间距、边距）
    int totalHeight = m_containerLayout->sizeHint().height();
    qDebug()<<totalHeight;

    // 限制一个最大高度（例如 450px），超过则显示滚动条
    int displayHeight = qMin(totalHeight, 450);

    m_scrollArea->setFixedHeight(displayHeight);
    this->setFixedHeight(displayHeight);      // 视图层固定高度

    // 8. 刷新父容器（关键：必须逐级通知）
    QWidget* parent = this->parentWidget();
    while (parent) {
        parent->updateGeometry();
        // 如果父对象是 CollapsibleGroup，可能需要显式触发它的布局刷新
        if (parent->layout()) {
            parent->layout()->activate();
        }
        parent = parent->parentWidget();
    }
    // 布局与高度刷新 强制要求容器重新计算布局
    m_container->adjustSize();

    QCoreApplication::processEvents(); // 给 OS 窗口系统一个喘息机会来同步位图

    // 通知上层 UI 刷新
    //this->updateGeometry();
}

QSize SidebarListPanel::sizeHint() const {
    // 返回我们 setFixedHeight 设置的高度，或者 layout 的推荐高度
    return QSize(QWidget::sizeHint().width(), this->height());
}
