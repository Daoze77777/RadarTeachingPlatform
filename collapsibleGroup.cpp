
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

    // 设置样式，参考图二设计 - 导航栏270px，左右各留20px空隙  #525D94
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
    setFixedHeight(28);
    setMinimumWidth(80);

    // 2. 事件处理
    setAttribute(Qt::WA_Hover, true);
    setMouseTracking(true);

    // 3. 字体设置（通过 QFont API）
    // QFont btnFont;
    // btnFont.setFamily("PingFang SC");
    // btnFont.setPixelSize(14);
    // btnFont.setWeight(QFont::Normal);
    // setFont(btnFont);

    // 4. 样式表（移除 margin，由父布局控制）
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

// #ifdef QT_DEBUG
//     // 调试信息（仅在 Debug 模式下）
//     QFontInfo fontInfo(font());
//     qDebug() << "CheckboxButton created:"
//              << "Font:" << fontInfo.family()
//              << "Size:" << fontInfo.pixelSize()
//              << "Weight:" << font().weight();
// #endif
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
    // 设置内容区域透明背景，允许子组件的滚动条显示
    m_contentWidget->setStyleSheet("QWidget { background-color: transparent`; }");
    m_contentLayout = new QVBoxLayout(m_contentWidget);
    m_contentLayout->setContentsMargins(0, 8, 0, 8);  // 默认值，雷达系统按钮不使用这个
    m_contentLayout->setSpacing(10);

    // 创建动画 - 进一步优化动画设置
    m_animation = new QPropertyAnimation(m_contentWidget, "maximumHeight", this);
    m_animation->setDuration(120);  // 更短的动画时间减少界面卡顿
    //m_animation->setEasingCurve(QEasingCurve::InOutQuad);  // 快速启动和结束，减少感知延迟
    //m_animation->setEasingCurve(QEasingCurve::OutQuad);
    m_animation->setEasingCurve(QEasingCurve::OutCubic);

    // 连接信号
    connect(m_headerButton, &QPushButton::clicked, this, &CollapsibleGroup::onHeaderClicked);
    connect(m_animation, &QPropertyAnimation::finished, this, &CollapsibleGroup::onAnimationFinished);

    // 添加到主布局
    mainLayout->addWidget(m_headerButton);
    mainLayout->addWidget(m_contentWidget);
    //mainLayout->addStretch();

    // 初始状态为折叠 - 先隐藏内容区域
    m_contentWidget->setMaximumHeight(0);
    m_contentWidget->hide();
    setExpanded(false);
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
     //m_headerButton->setExpanded(false);   // 按钮图标变回折叠态
}
void CollapsibleGroup::setExpanded(bool expanded)
{
    if (m_expanded == expanded)
        return;
    // 🔧 关键修复：立即停止任何正在进行的动画，防止多个动画冲突导致布局异常
    if (m_animation->state() == QPropertyAnimation::Running) {
        m_animation->stop();
        // 强制完成当前状态
        if (m_expanded && m_contentWidget->maximumHeight() > 0) {
            // 如果正在展开，直接跳到展开状态
            m_contentWidget->setMaximumHeight(QWIDGETSIZE_MAX);
        } else if (!m_expanded && m_contentWidget->maximumHeight() < QWIDGETSIZE_MAX) {
            // 如果正在折叠，直接跳到折叠状态
            m_contentWidget->setMaximumHeight(0);
            m_contentWidget->hide();
        }
    }
    m_expanded = expanded;
    m_headerButton->setExpanded(expanded);
    // 判断是否是雷达系统组，需要特殊处理
    bool isRadarGroup = (objectName() == "radarSystemGroup");
    // 如果是雷达系统组且正在折叠，使用更直接的处理方式
    if (isRadarGroup && !expanded) {
        // 雷达系统组直接折叠，只限制高度，不影响宽度
        m_contentWidget->setMinimumHeight(0);
        m_contentWidget->setMaximumHeight(0);
        m_contentWidget->hide();
        emit expandedChanged(expanded);
        return;
    }
    if (expanded) {
        // 🔧 关键修复：展开前完全重置所有尺寸相关的属性
        // 1. 重置尺寸策略（横向扩展，纵向自适应）
        m_contentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        // 2. 移除所有高度限制
        m_contentWidget->setMinimumHeight(0);
        m_contentWidget->setMaximumHeight(QWIDGETSIZE_MAX);
        // 3. 移除所有宽度限制（确保横向完全扩展）
        m_contentWidget->setMinimumWidth(0);
        m_contentWidget->setMaximumWidth(QWIDGETSIZE_MAX);
        // 4. 重新启用布局
        if (m_contentLayout) {
            m_contentLayout->setEnabled(true);
        }
        // 5. 强制更新几何信息
        m_contentWidget->updateGeometry();
        // 🔧 关键修复：先计算高度，再显示内容，避免抖动
        // 使用布局的sizeHint计算高度（不需要show()）
        m_contentLayout->invalidate(); // 强制重新计算布局
        m_contentLayout->activate(); // 激活布局
        // 更精确地计算高度，使用多个指标取最大值
        int contentHeight = qMax(m_contentWidget->sizeHint().height(),
                                 m_contentLayout->sizeHint().height() + 4);
        // 确保高度足够容纳所有子组件
        contentHeight = qMax(contentHeight, m_contentLayout->minimumSize().height());
        // 🔧 遍历子组件，如果有QScrollArea，使用其固定高度
        for (int i = 0; i < m_contentLayout->count(); ++i) {
            QWidget *child = m_contentLayout->itemAt(i)->widget();
            if (child) {
                QScrollArea *scrollArea = qobject_cast<QScrollArea*>(child);
                if (scrollArea) {
                    // 使用滚动区域的固定高度或最小高度
                    int scrollHeight = scrollArea->minimumHeight();
                    if (scrollArea->maximumHeight() == scrollArea->minimumHeight()) {
                        scrollHeight = scrollArea->maximumHeight();  // 固定高度
                    }
                    contentHeight = qMax(contentHeight, scrollHeight + m_contentLayout->contentsMargins().top() + m_contentLayout->contentsMargins().bottom());
                }
            }
        }
        // 如果是雷达系统组，额外添加一些高度以确保足够空间
        if (isRadarGroup) {
            contentHeight += 2; // 额外添加一些缓冲
        }
        qDebug() << "🔍 折叠组" << (isRadarGroup ? "(雷达系统)" : "") << "展开，计算高度：" << contentHeight;
        // 重置为动画初始状态（只限制高度）
        m_contentWidget->setMaximumHeight(0);
        m_contentWidget->show();  // 高度为0时显示，不会造成抖动
        // 开始动画
        m_animation->setStartValue(0);
        m_animation->setEndValue(contentHeight);
        m_animation->start();
    } else {
        // 🔧 优化折叠逻辑：使用更短的动画时间，减少多组同时折叠/展开时的抖动
        int contentHeight = m_contentWidget->height();
        // 避免微小高度值可能导致的视觉跳跃
        if (contentHeight < 5) {
            contentHeight = 0;
            m_contentWidget->setMinimumHeight(0);
            m_contentWidget->setMaximumHeight(0);
            m_contentWidget->hide();
            emit expandedChanged(expanded);
            return; // 高度已经很小，直接隐藏避免动画
        }
        // 🔧 关键优化：缩短折叠动画时间（80ms），减少多个折叠组互斥切换时的布局抖动
        int originalDuration = m_animation->duration();
        m_animation->setDuration(80);  // 临时设置更短的折叠动画
        // 正常折叠动画
        m_animation->setStartValue(contentHeight);
        m_animation->setEndValue(0);
        m_animation->start();
        // 动画结束后恢复原始时长
        QTimer::singleShot(80, this, [this, originalDuration]() {
            m_animation->setDuration(originalDuration);
        });
    }
    emit expandedChanged(expanded);
}
// 无动画设置展开状态（用于初始化首帧就展开，避免可见动画/闪烁）
void CollapsibleGroup::setInitialExpanded(bool expanded)
{
    // 直接停止可能存在的动画
    if (m_animation && m_animation->state() == QPropertyAnimation::Running) {
        m_animation->stop();
    }
    m_expanded = expanded;
    if (m_headerButton) {
        m_headerButton->setExpanded(expanded);
    }
    if (expanded) {
        // 直接展开：移除限制并显示内容区域
        if (m_contentLayout) m_contentLayout->setEnabled(true);
        if (m_contentWidget) {
            m_contentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
            m_contentWidget->setMinimumHeight(0);
            m_contentWidget->setMaximumHeight(QWIDGETSIZE_MAX);
            m_contentWidget->setMinimumWidth(0);
            m_contentWidget->setMaximumWidth(QWIDGETSIZE_MAX);
            m_contentWidget->show();
            m_contentLayout->invalidate();
            m_contentWidget->adjustSize();
            m_contentWidget->updateGeometry();
        }
    } else {
        // 直接折叠：限制高度并隐藏
        if (m_contentLayout) m_contentLayout->setEnabled(false);
        if (m_contentWidget) {
            m_contentWidget->setMinimumHeight(0);
            m_contentWidget->setMaximumHeight(0);
            m_contentWidget->hide();
        }
    }
    // 注意：初始化阶段不发射 expandedChanged，避免触发互斥折叠逻辑
}
bool CollapsibleGroup::isExpanded() const
{
    return m_expanded;
}
void CollapsibleGroup::setSelected(bool selected)
{
    m_headerButton->setChecked(selected);
}
bool CollapsibleGroup::isSelected() const
{
    return m_headerButton->isChecked();
}
CollapsibleButton* CollapsibleGroup::getHeaderButton() const
{
    return m_headerButton;
}
bool CollapsibleGroup::containsPoint(const QPoint& point) const
{
    // 检查点击是否在整个组件区域内（包括头部和内容区域）
    return geometry().contains(point);
}
QVBoxLayout* CollapsibleGroup::getContentLayout() const
{
    return m_contentLayout;
}
void CollapsibleGroup::onHeaderClicked()
{
    emit animationStarted();  // 通知动画开始，触发UI交互模式
    setExpanded(!m_expanded);
    emit clicked();  // 发出点击信号
}
void CollapsibleGroup::onAnimationFinished()
{
     // 发送信号
     emit animationFinished();
     // 只需要这一步来通知父布局重新计算高度
     // setAttribute(Qt::WA_LayoutUsesWidgetRect); // 在构造函数里设置这个可能有用
     if (m_expanded) {
         // 展开结束时，解开高度限制，防止内容动态增加时被截断
         m_contentWidget->setMaximumHeight(QWIDGETSIZE_MAX);
     } else {
         m_contentWidget->hide(); // 折叠结束隐藏，提升性能
     }
     // 触发一次父布局调整即可
     if (parentWidget() && parentWidget()->layout()) {
         parentWidget()->layout()->activate();
     }
}
