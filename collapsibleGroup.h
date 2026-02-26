#ifndef COLLAPSIBLEGROUP_H
#define COLLAPSIBLEGROUP_H

#include <QPushButton>
#include <QPaintEvent>
#include <QPropertyAnimation>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QLabel>

// 自定义折叠组按钮
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

// Standerd组件按钮
class CheckboxButton : public QPushButton {
    Q_OBJECT
public:
    explicit CheckboxButton(const QString &text, QWidget *parent = nullptr);
};

//Numbered步骤按钮
class StepButton : public QPushButton
{
    Q_OBJECT
public:
    explicit StepButton(int index, const QString &text, QWidget *parent = nullptr);

protected:

private slots:
    // 处理选中状态切换时的样式更新
    void onToggled(bool checked);

private:
    // 内部初始化函数
    void setupUi(int index, const QString &text);

    // 更新样式的核心逻辑
    void updateStyle(bool checked);

private:
    QLabel *m_numLabel;  // 左侧圆圈数字
    QLabel *m_textLabel; // 右侧文字描述
};

// 可折叠组
class CollapsibleGroup : public QWidget
{
    Q_OBJECT
public:
    explicit CollapsibleGroup(const QString &title, const QIcon &icon, QWidget *parent = nullptr);
    void addWidget(QWidget *widget);
    void clearContent(); //清空布局
    void setExpanded(bool expanded);
    // 无动画设置展开状态（用于初始化首帧就展开，避免可见动画）
    //void setInitialExpanded(bool expanded);
    bool isExpanded() const;
    //void setSelected(bool selected);
    //bool isSelected() const;
    //CollapsibleButton* getHeaderButton() const;
    //bool containsPoint(const QPoint& point) const;
    QVBoxLayout* getContentLayout() const;  // 获取内容布局
    QWidget* getContentWidget();
    void resetStateForSwitch();

private:

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
    QScrollArea* m_scrollArea;
    QVBoxLayout *m_contentLayout;
    QWidget *m_contentWidget;
    bool m_expanded;

};
#endif // COLLAPSIBLEGROUP_H
