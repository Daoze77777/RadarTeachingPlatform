
#include "collapsibleGroup.h"
#include <QPainter>
#include <QScrollArea>
#include <QTimer>

// CollapsibleButton 实现
CollapsibleButton::CollapsibleButton(const QString &text, const QIcon &leftIcon, QWidget *parent)
    : QPushButton(text, parent)
    , m_leftIcon(leftIcon)
    , m_expanded(false)
{
    // 加载箭头图标（使用新的图片资源）
    m_rightExpandIcon = QIcon(":/mainicons/resources/mainIcons/fangxiangup.png");  // 展开状态
    m_rightCollapseIcon = QIcon(":/mainicons/resources/mainIcons/fangxiangdown.png");  // 折叠状态

    setCheckable(true);  // 允许选中状态
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // 设置样式，参考图二设计 - 导航栏270px，左右各留20px空隙
    setStyleSheet(R"(
        QPushButton {
            background-color: #525D94;
            color: #FFFFFF;
            padding: 5px 16px 5px 46px;
            border: none;
            border-radius: 8px;
            text-align: left;
            font-size: 18px;
            font-weight: 500;
            font-family: 'PingFang SC', 'Microsoft YaHei', sans-serif;
        }
        QPushButton:hover {
            background-color: #525D94;
            color: #FFFFFF;
            border-radius: 8px;
        }
        QPushButton:pressed {
            background-color: #525D94;
            color: #FFFFFF;
            border-radius: 8px;
        }
        QPushButton:checked {
            background-color: #525D94;
            color: #FFFFFF;
            border: none;
        }
        QPushButton:checked:hover {
            background-color: #525D94;
        }
    )");
}
void CollapsibleButton::setExpanded(bool expanded)
{
    m_expanded = expanded;
    update();  // 触发重绘
}
void CollapsibleButton::paintEvent(QPaintEvent *event)
{
    QPushButton::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制左侧图标
    if (!m_leftIcon.isNull()) {
        int iconSize = 20;
        int leftPadding = 20; // 距离左边框的距离
        // 自动计算垂直居中坐标
        int yPos = (height() - iconSize) / 2;
        QRect leftIconRect(leftPadding, yPos, iconSize, iconSize);
        m_leftIcon.paint(&painter, leftIconRect);
    }

    // 绘制右侧箭头
    QIcon rightIcon = m_expanded ? m_rightExpandIcon : m_rightCollapseIcon;
    if (!rightIcon.isNull()) {
        int arrowSize = 20;
        int rightPadding = 20; // 距离右边框的距离
        // 自动计算右对齐和垂直居中
        int xPos = width() - rightPadding - arrowSize;
        int yPos = (height() - arrowSize) / 2;
        QRect rightIconRect(xPos,yPos, arrowSize, arrowSize);
        rightIcon.paint(&painter, rightIconRect);
    }
}

// CheckboxButton实现
CheckboxButton::CheckboxButton(const QString &text, QWidget *parent)
    : QPushButton(text, parent)
{
    // 1. 基础配置
    setCheckable(true);
    setAutoExclusive(true);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setFixedHeight(40);
    setMinimumWidth(80);

    // 2. 事件处理
    setAttribute(Qt::WA_Hover, true);
    setMouseTracking(true);
    setStyleSheet(R"(
        QPushButton {
            background: #F8F9FA;
            color: #0A1424;
            padding: 6px 12px 6px 16px;
            border: 1px solid #E9ECEF;
            border-radius: 8px;
            text-align: left;
        }
        QPushButton:hover,
        QPushButton:pressed,
        QPushButton:checked {
            background: #C4F4DC;
            color: #0A1424;
            border: 1px solid #C0E6DC;
        }
    )");
}

// NumberedButton实现
StepButton::StepButton(int index, const QString &text, QWidget *parent)
    : QPushButton(parent)
{
    setupUi(index, text);

    // 连接信号：当按钮选中状态改变时，刷新样式
    connect(this, &StepButton::toggled, this, &StepButton::onToggled);

    // 初始化为未选中状态
    updateStyle(false);
}
void StepButton::setupUi(int index, const QString &text)
{
    // 1. 按钮整体属性
    this->setCheckable(true);
    this->setMinimumHeight(40);

    // 2. 创建布局
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(15,0,0,0); // 设置内边距
    layout->setSpacing(9);  // 数字和文字的间距

    // 3. 创建左侧圆形数字 Label
    m_numLabel = new QLabel(QString::number(index), this);
    m_numLabel->setFixedSize(26, 26);
    m_numLabel->setAlignment(Qt::AlignCenter);
    // 设置 Label 的对象名，方便在 QSS 中特异性定位（可选）
    m_numLabel->setObjectName("NumLabel");
    m_numLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

    // 4. 创建右侧文字 Label
    m_textLabel = new QLabel(text, this);
    m_textLabel->setObjectName("TextLabel");

    // 【关键】让 Label 不拦截鼠标事件，点击文字也能触发按钮点击
    m_textLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

    // 5. 添加到布局
    layout->addWidget(m_numLabel);
    layout->addWidget(m_textLabel);
    layout->addStretch(); // 弹簧占位，让内容靠左
}
void StepButton::onToggled(bool checked)
{
    updateStyle(checked);
}
void StepButton::updateStyle(bool checked)
{
    if (checked) {
        // 1. 按钮整体样式
        this->setStyleSheet(R"(
            StepButton {
                background-color: #D1F2EB;
                border: 1px solid #B5EAD7;
                border-radius: 8px;
            }
            StepButton:hover {
                background-color: #C6F3DE;
            }
        )");

        // 2. 圆圈数字样式 (深绿底 + 白字)
        m_numLabel->setStyleSheet(R"(
            QLabel {
                background-color: #00C853;
                color: #FFFFFF;
                border-radius: 13px;
                font-weight: bold;
            }
        )");

        // 3. 文字样式 (深绿色加粗)
        m_textLabel->setStyleSheet("color: #0A1424;  background:transparent;");

    } else {
        // 1. 按钮整体样式
        this->setStyleSheet(R"(
            StepButton {
                background-color: #F8F9FA;
                border: 1px solid #DEE2EA;
                border-radius: 8px;
            }
            StepButton:hover {
                background-color: #C4F4DC;;
            }
        )");

        // 2. 圆圈数字样式 (浅紫底 + 蓝紫字)
        m_numLabel->setStyleSheet(R"(
            QLabel {
                background-color: #D7DDF9;
                color: #2240E1;
                border-radius: 13px;
                font-weight: bold;
            }
        )");

        // 3. 文字样式 (深灰正常)
        m_textLabel->setStyleSheet("color: #0A1424;  background:transparent;");
    }
}

// CollapsibleGroup 实现
CollapsibleGroup::CollapsibleGroup(const QString &title, const QIcon &icon, QWidget *parent)
    : QWidget(parent)
    , m_expanded(false)
{
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 0);
    mainLayout->setSpacing(0);

    // 创建头部按钮
    m_headerButton = new CollapsibleButton(title, icon, this);
    m_headerButton->setFixedSize(230,40);

    // 创建内容容器
    m_contentWidget = new QWidget(this);
    m_contentWidget->setMinimumHeight(0);  // 确保最小高度为0
    m_contentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);  // 横向扩展
    //m_contentWidget->setStyleSheet("QWidget { background-color: purple; }");

    //内容容器垂直布局
    m_contentLayout = new QVBoxLayout(m_contentWidget);
    m_contentLayout->setAlignment(Qt::AlignTop);

    // 创建动画 - 进一步优化动画设置
    m_animation = new QPropertyAnimation(m_contentWidget, "maximumHeight", this);
    m_animation->setDuration(100);  // 更短的动画时间减少界面卡顿
    m_animation->setEasingCurve(QEasingCurve::OutCubic);

    // 添加到主布局
    mainLayout->addWidget(m_headerButton);
    mainLayout->addWidget(m_contentWidget);

    // 初始状态为折叠 - 先隐藏内容区域
    m_contentWidget->setMaximumHeight(0);
    m_contentWidget->hide();
    setExpanded(false);

    // 连接信号
    connect(m_headerButton, &QPushButton::clicked, this, &CollapsibleGroup::onHeaderClicked);
    connect(m_animation, &QPropertyAnimation::finished, this, &CollapsibleGroup::onAnimationFinished);
}
void CollapsibleGroup::addWidget(QWidget *widget)
{
    m_contentLayout->addWidget(widget);
    // 如果当前是折叠状态，确保内容区域仍然隐藏
    if (!m_expanded)
    {
        m_contentWidget->setMaximumHeight(0);
        m_contentWidget->hide();
    }
}
void CollapsibleGroup::clearContent()
{
     // 1. 如果动画正在运行，先停止，防止计算错误
     if (m_animation->state() == QPropertyAnimation::Running) {
         m_animation->stop();
     }
     // 2. 遍历并删除所有子控件
     QLayoutItem *item;
     while ((item = m_contentLayout->takeAt(0)) != nullptr) {
         if (QWidget *widget = item->widget()) {
             widget->deleteLater(); // 彻底释放内存
         }
         delete item;
     }
     // 3. 重置状态
     m_contentWidget->setMaximumHeight(0); // 高度归零
     m_expanded = false;                   // 状态设为折叠
     m_headerButton->setExpanded(false);   // 按钮图标变回折叠态
}
void CollapsibleGroup::setExpanded(bool expanded)
{
    if (m_expanded == expanded) return;

    // 1. 停止旧动画
    if (m_animation->state() == QPropertyAnimation::Running)
    {
        m_animation->stop();
    }
    m_expanded = expanded;
    m_headerButton->setExpanded(expanded);

    if (expanded) {
        // --- 展开逻辑 ---
        m_contentWidget->show();

        // 关键：激活布局，让它去问子控件(RadarSystemPanel)要 sizeHint
        m_contentLayout->activate();

        // 获取真实高度：此时 RadarSystemPanel 的 sizeHint 会起作用
        int targetHeight = m_contentLayout->sizeHint().height();

        // 保底机制：如果算出来太小（比如还没布局好），给个最小值
        if (targetHeight < 10) targetHeight = 100;

        qDebug() << "折叠组展开，目标高度：" << targetHeight;

        m_animation->setStartValue(0);
        m_animation->setEndValue(targetHeight);
        m_animation->start();

    } else {
        // --- 折叠逻辑 ---
        m_animation->setStartValue(m_contentWidget->height());
        m_animation->setEndValue(0);
        m_animation->start();
    }

    emit expandedChanged(expanded);
}
// 无动画设置展开状态（用于初始化首帧就展开，避免可见动画/闪烁）
// void CollapsibleGroup::setInitialExpanded(bool expanded)
// {
//     // 直接停止可能存在的动画
//     if (m_animation && m_animation->state() == QPropertyAnimation::Running) {
//         m_animation->stop();
//     }
//     m_expanded = expanded;
//     if (m_headerButton) {
//         m_headerButton->setExpanded(expanded);
//     }
//     if (expanded) {
//         // 直接展开：移除限制并显示内容区域
//         if (m_contentLayout) m_contentLayout->setEnabled(true);
//         if (m_contentWidget) {
//             m_contentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
//             m_contentWidget->setMinimumHeight(0);
//             m_contentWidget->setMaximumHeight(QWIDGETSIZE_MAX);
//             m_contentWidget->setMinimumWidth(0);
//             m_contentWidget->setMaximumWidth(QWIDGETSIZE_MAX);
//             m_contentWidget->show();
//             m_contentLayout->invalidate();
//             m_contentWidget->adjustSize();
//             m_contentWidget->updateGeometry();
//         }
//     } else {
//         // 直接折叠：限制高度并隐藏
//         if (m_contentLayout) m_contentLayout->setEnabled(false);
//         if (m_contentWidget) {
//             m_contentWidget->setMinimumHeight(0);
//             m_contentWidget->setMaximumHeight(0);
//             m_contentWidget->hide();
//         }
//     }
//     // 注意：初始化阶段不发射 expandedChanged，避免触发互斥折叠逻辑
// }
bool CollapsibleGroup::isExpanded() const
{
    return m_expanded;
}
// void CollapsibleGroup::setSelected(bool selected)
// {
//     m_headerButton->setChecked(selected);
// }
// bool CollapsibleGroup::isSelected() const
// {
//     return m_headerButton->isChecked();
// }
// CollapsibleButton* CollapsibleGroup::getHeaderButton() const
// {
//     return m_headerButton;
// }
// bool CollapsibleGroup::containsPoint(const QPoint& point) const
// {
//     // 检查点击是否在整个组件区域内（包括头部和内容区域）
//     return geometry().contains(point);
// }
void CollapsibleGroup::resetStateForSwitch()
{
    // 1. 暴力打断可能正在运行的动画
    if (m_animation->state() == QPropertyAnimation::Running) {
        m_animation->stop();
    }

    // 2. 状态标识置为折叠
    m_expanded = false;
    m_headerButton->setExpanded(false);
    //->setExpanded(false);

    // 3. ★ 最关键的一步 ★
    // 必须隐藏，视觉上它是折叠的
    m_contentWidget->hide();

    // 但是！【绝对不能】把 maximumHeight 设为 0，也不能禁用 layout
    // 必须彻底放开限制，否则里面新塞入的 SidebarListPanel 算不出 sizeHint
    m_contentWidget->setMaximumHeight(QWIDGETSIZE_MAX);

    if (m_contentLayout) {
        m_contentLayout->setEnabled(true); // 确保布局引擎是活的
    }
}
QWidget* CollapsibleGroup::getContentWidget()
{
    return m_contentWidget;
}
QVBoxLayout* CollapsibleGroup::getContentLayout() const
{
    return m_contentLayout;
}
void CollapsibleGroup::onHeaderClicked()
{
    // 只需要这一行！所有的动画和高度计算都交给 setExpanded 去处理
    setExpanded(!m_expanded);
    emit clicked();
}
void CollapsibleGroup::onAnimationFinished()
{
     emit animationFinished();
     if (m_expanded) {
         // 展开完了，设为最大值，这样内部如果用滚轮或者内容变化，不会被卡住
         m_contentWidget->setMaximumHeight(QWIDGETSIZE_MAX);
     } else {
         m_contentWidget->hide();
     }
}
