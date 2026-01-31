#include "loginwindow.h"
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QMouseEvent>
#include "teachingMainWindow.h"

loginWindow::loginWindow(QWidget *parent)
    : QWidget(parent)
    , m_moving(false)
{
    setupUI();
    setStyles();
    setupConnections();
}

loginWindow::~loginWindow() {}

void loginWindow::setupUI()
{
    //设置窗口图标
    setWindowIcon(QIcon(":/icons/resources/icons/title.ico"));

    //加载背景图
    m_backgroundPixmap = QPixmap(":/login/resources/login/login_background.png");

    //设置窗口背景透明
    setAttribute(Qt::WA_TranslucentBackground);

    //设置无边框窗口和显示最小化按钮
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);

    //设置窗口固定大小，匹配背景图片 820x600
    setFixedSize(820, 600);


    // 创建自定义标题栏按钮（右上角）
    const int buttonSize = 20;
    const int buttonTop = 52;
    const int buttonSpacing = 10;

    //关闭按钮
    closeButton = new QPushButton(this);
    closeButton->setObjectName("closeButton");
    closeButton->setGeometry(685, buttonTop, buttonSize, buttonSize);
    closeButton->setCursor(Qt::PointingHandCursor);
    closeButton->setIcon(QIcon(":/login/resources/login/close.png"));
    closeButton->setIconSize(QSize(buttonSize, buttonSize));
    closeButton->setFlat(true);

    // 最小化按钮
    minimizeButton = new QPushButton(this);
    minimizeButton->setObjectName("minimizeButton");
    minimizeButton->setGeometry(685 - buttonSize - buttonSpacing, buttonTop, buttonSize, buttonSize);
    minimizeButton->setCursor(Qt::PointingHandCursor);
    minimizeButton->setIcon(QIcon(":/login/resources/login/minimize.png"));
    minimizeButton->setIconSize(QSize(buttonSize, buttonSize));
    minimizeButton->setFlat(true);

    // 姓名标签 (中文) - 右侧表单区域
    nameLabel = new QLabel(QString::fromUtf8("姓 名"), this);
    nameLabel->setGeometry(475, 180, 50, 22);
    nameLabel->setObjectName("nameLabel");

    // Name标签 (英文) - 下边与姓名对齐
    nameLabelEn = new QLabel("Name", this);
    nameLabelEn->setGeometry(530, 188, 45, 14);
    nameLabelEn->setObjectName("nameLabelEn");

    // 姓名输入框
    usernameEdit = new QLineEdit(this);
    usernameEdit->setGeometry(475, 210, 210, 38);
    usernameEdit->setPlaceholderText(QString::fromUtf8("请输入您的姓名"));
    usernameEdit->setObjectName("usernameEdit");

    //学号标签 (中文)
    studentIdLabel = new QLabel(QString::fromUtf8("学 号"), this);
    studentIdLabel->setGeometry(475, 265, 50, 22);
    studentIdLabel->setObjectName("studentIdLabel");

    // 学号标签 (英文) - 下边与学号对齐
    studentIdLabelEn = new QLabel("Student ID", this);
    studentIdLabelEn->setGeometry(530, 273, 80, 14);
    studentIdLabelEn->setObjectName("studentIdLabelEn");

    // 学号输入框
    studentIdEdit = new QLineEdit(this);
    studentIdEdit->setGeometry(475, 295, 210, 38);
    studentIdEdit->setPlaceholderText(QString::fromUtf8("请输入您的学号"));
    studentIdEdit->setEchoMode(QLineEdit::Password);
    studentIdEdit->setObjectName("studentIdEdit");

    // 登录按钮
    loginButton = new QPushButton(this);
    loginButton->setText(QString::fromUtf8("登  录"));
    loginButton->setGeometry(460, 360, 230, 40);
    loginButton->setObjectName("loginButton");
    loginButton->setCursor(Qt::PointingHandCursor);
    loginButton->setFlat(true);

    // 注册
    registerLabel = new QLabel(QString::fromUtf8("<a href=\"#\" style=\"color: #3E3D70; text-decoration: none;\">立即注册</a>"), this);
    registerLabel->setGeometry(465, 430, 70, 18);
    registerLabel->setOpenExternalLinks(false);
    registerLabel->setObjectName("registerLabel");
    registerLabel->setCursor(Qt::PointingHandCursor);

    // 忘记密码
    forgotPasswordLabel = new QLabel(QString::fromUtf8("<a href=\"#\" style=\"color: #3E3D70; text-decoration: none;\">忘记密码？</a>"), this);
    forgotPasswordLabel->setGeometry(620, 430, 85, 18);
    forgotPasswordLabel->setOpenExternalLinks(false);
    forgotPasswordLabel->setObjectName("forgotPasswordLabel");
    forgotPasswordLabel->setCursor(Qt::PointingHandCursor);

}
void loginWindow::setStyles()
{

    QString style =
        // 姓名、学号标签样式
        "#nameLabel, #studentIdLabel {"
        "   color: #3E3D70;"
        "   font-size: 20px;"
        "   font-family: 'PingFang SC', 'Microsoft YaHei', sans-serif;"
        "   font-weight: 450;"
        "   background: transparent;"
        "   border: none;"
        "}"
        // Name、StudentID英文标签样式
        "#nameLabelEn, #studentIdLabelEn {"
        "   color: #3E3D70;"
        "   font-size: 15px;"
        "   font-family: 'PingFang SC', 'Microsoft YaHei', sans-serif;"
        "   font-weight: bold;"
        "   font-style: italic;"
        "   background: transparent;"
        "   border: none;"
        "   padding: 0px;"
        "}"
        // 输入框样式 - 只有下边框线
        "#usernameEdit, #studentIdEdit {"
        "   background: transparent;"
        "   border: none;"
        "   border-bottom: 1px solid rgba(210, 215, 230, 0.6);"
        "   padding: 8px 4px;"
        "   color: #333333;"
        "}"
        "#usernameEdit:focus, #studentIdEdit:focus {"
        "   background: transparent;"
        "   border: none;"
        "   border-bottom: 2px solid rgba(90, 140, 255, 0.7);"
        "}"
        // 注册和忘记密码链接样式
        "#registerLabel, #forgotPasswordLabel {"
        "   color: #3E3D70;"
        "   font-size: 14px;"
        "   font-family: 'PingFang SC', 'Microsoft YaHei', sans-serif;"
        "   background: transparent;"
        "   border: none;"
        "}"
        "#registerLabel a, #forgotPasswordLabel a {"
        "   color: #3E3D70;"
        "   text-decoration: none;"
        "}"
        "#registerLabel a:hover, #forgotPasswordLabel a:hover {"
        "   color: #2E2D60;"
        "   text-decoration: underline;"
        "}"
        // 标题栏按钮样式
        "#closeButton, #minimizeButton {"
        "   background: transparent;"
        "   border: none;"
        "}"
        "#closeButton:hover, #minimizeButton:hover {"
        "   background: rgba(255, 255, 255, 0.1);"
        "   border-radius: 3px;"
        "}"
        //登录按钮样式
        "#loginButton {"
        "   background: qlineargradient(spread:pad, x1:0, y1:0.5, x2:1, y2:0.5,"
        "       stop:0 #66aaff, stop:1 #3388ff);"
        "   border: none;"
        "   border-radius: 20px;"
        "   color: white;"
        "   font-size: 18px;"
        "   padding: 10px;"
        "   text-align: center;"
        "   font-weight: bold;"
        "}"
        "#loginButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #88ccff, stop:1 #55aaee);"
        "}"
        "#loginButton:pressed {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #4488dd, stop:1 #2266bb);"
        "}"
        ;
    setStyleSheet(style);
}
void loginWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);  //标记参数未使用，避免编译器警告
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); //抗锯齿
    painter.setRenderHint(QPainter::SmoothPixmapTransform); //光滑变换

    //设置圆角裁剪，减小圆角避免模糊
    QPainterPath path;
    path.addRoundedRect(rect(), 4, 4);
    painter.setClipPath(path);

    //绘制背景
    if (!m_backgroundPixmap.isNull()) {
        painter.drawPixmap(rect(), m_backgroundPixmap);
    }
}
void loginWindow::mousePressEvent(QMouseEvent *event)
{
    m_moving = true;  //开始移动
    mouse_pos = event->globalPosition().toPoint();
    window_pos = this->pos();
    diff_pos = mouse_pos - window_pos;
}
void loginWindow::mouseMoveEvent(QMouseEvent *event)
{
    QPoint pos = event->globalPosition().toPoint();
    this->move(pos - diff_pos);

}
void loginWindow::mouseReleaseEvent(QMouseEvent *event)
{
     m_moving = false;  // 结束移动
}
void loginWindow::setupConnections()
{
    connect(closeButton,&QPushButton::clicked,this, &loginWindow::close);
    connect(minimizeButton,&QPushButton::clicked,this, &loginWindow::showMinimized);
    connect(loginButton,&QPushButton::clicked,this, &loginWindow::onLoginButtonClicked);
}
void loginWindow::onLoginButtonClicked()
{
    QString username = usernameEdit->text();
    QString studentId = studentIdEdit->text();
    // 用户验证逻辑
    if (!username.isEmpty() && !studentId.isEmpty()) {
        qDebug()<<"hello";
        m_teachingWindow = new TeachingMainWindow();
        m_teachingWindow->show();

        this->hide();     // 立即隐藏窗口
        //this->deleteLater();  // 稍后删除对象

    } else {
        qDebug()<<"error";
    }

}
