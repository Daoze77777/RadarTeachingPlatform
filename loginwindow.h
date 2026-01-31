#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include "teachingMainWindow.h"
class loginWindow : public QWidget
{
    Q_OBJECT

public:
    explicit loginWindow(QWidget *parent = nullptr);
    ~loginWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void onLoginButtonClicked(); //登录按钮操作

private:
    void setupUI();
    void setupConnections();
    void setStyles();

private:
    //最小化、关闭按钮
    QPushButton* closeButton;
    QPushButton* minimizeButton;

    //姓名
    QLabel* nameLabel;
    QLabel* nameLabelEn;
    QLineEdit* usernameEdit;

    //学号
    QLabel* studentIdLabel;
    QLabel* studentIdLabelEn;
    QLineEdit* studentIdEdit;

    //登录按钮
    QPushButton* loginButton;

    //注册和忘记密码
    QLabel* registerLabel;
    QLabel* forgotPasswordLabel;
    TeachingMainWindow* m_teachingWindow;

    //窗口移动
    bool m_moving;
    QPoint mouse_pos;
    QPoint window_pos;
    QPoint diff_pos;

    //背景图片
    QPixmap m_backgroundPixmap;
};
#endif // LOGINWINDOW_H
