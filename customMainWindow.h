#ifndef CUSTOMMAINWINDOW_H
#define CUSTOMMAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QMenu>
#include <QStackedWidget>

class CustomMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CustomMainWindow(QWidget *parent = nullptr);
    virtual ~CustomMainWindow();

    void setCustomMainWindowTitle(const QString& iconPath, const QString& title);

protected:
    //子类可以访问成员
    //标题栏
    QWidget* titleBar;
    //图标
    QLabel* iconLabel;
    //标题文字
    QLabel* titleLabel;
    //最小化按钮
    QPushButton* minButton;
    //关闭按钮
    QPushButton* closeButton;
    //菜单栏
    QPushButton* fileButton;
    QPushButton* helpButton;
    QMenu *fileMenu;
    QMenu *helpMenu;

    //首页面内容区域
    QWidget* centralWidget;
    //首页面内容窗口，可以多窗口切换，包括目录页面和各个实验科目页面
    QStackedWidget* contentWidget;

    // 基本设置函数，子类可以调用
    void setupWindowStyle();
    void setupCustomTitleBar();
    void setupBasicLayout();


    // 鼠标事件处理
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

public slots:
    // 公共槽函数，供子类和外部调用
    virtual void about();

private:

    // 拖拽相关变量
    bool isDragging;
    QPoint dragPosition;

};
#endif // CUSTOMMAINWINDOW_H
