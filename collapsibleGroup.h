#ifndef COLLAPSIBLEGROUP_H
#define COLLAPSIBLEGROUP_H

#include <QPushButton>
#include <QPaintEvent>
#include <QPropertyAnimation>
#include <QVBoxLayout>
// 自定义按钮，支持右侧箭头图标
class CollapsibleButton : public QPushButton
{
    Q_OBJECT
public:
    explicit CollapsibleButton(const QString &text, const QIcon &leftIcon, QWidget *parent = nullptr);
    void setExpanded(bool expanded);
protected:
    void paintEvent(QPaintEvent *event) override;


private:
    QIcon m_leftIcon;
    QIcon m_rightExpandIcon;
    QIcon m_rightCollapseIcon;
    bool m_expanded;
};

// 可折叠组组件
class CollapsibleGroup : public QWidget
{
    Q_OBJECT
public:
    explicit CollapsibleGroup(const QString &title, const QIcon &icon, QWidget *parent = nullptr);
    void addWidget(QWidget *widget);
    void clearContent(); //清空布局
    void setExpanded(bool expanded);
    // 无动画设置展开状态（用于初始化首帧就展开，避免可见动画）
    void setInitialExpanded(bool expanded);
    bool isExpanded() const;
    void setSelected(bool selected);
    bool isSelected() const;
    CollapsibleButton* getHeaderButton() const;
    bool containsPoint(const QPoint& point) const;
    QVBoxLayout* getContentLayout() const;  // 获取内容布局


signals:
    void expandedChanged(bool expanded);
    void clicked();
    void animationStarted();  // 动画开始信号
    void animationFinished(); // 动画结束信号

private slots:
    void onHeaderClicked();
    void onAnimationFinished();

private:
    CollapsibleButton *m_headerButton;  //导航栏一级按钮
    QPropertyAnimation *m_animation;
    QVBoxLayout *m_contentLayout;
    QWidget *m_contentWidget;
    bool m_expanded;

};
#endif // COLLAPSIBLEGROUP_H
