#include "customMainWindow.h"
#include <QDialog>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QVBoxLayout>
#include "ExperimentManager.h"

CustomMainWindow::CustomMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , titleBar(nullptr)
    , iconLabel(nullptr)
    , titleLabel(nullptr)
    , fileButton(nullptr)
    , helpButton(nullptr)
    , minButton(nullptr)
    , closeButton(nullptr)
    , centralWidget(nullptr)
    , contentWidget(nullptr)
    , isDragging(false)
{
    // 设置窗口为无边框
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinMaxButtonsHint);
}
CustomMainWindow::~CustomMainWindow() {}
void CustomMainWindow::setCustomMainWindowTitle(const QString &iconPath, const QString &title)
{
    //设置窗口图标
    QMainWindow::setWindowIcon(QIcon(iconPath));
    if (iconLabel) {
        QPixmap titleIcon(iconPath);
        iconLabel->setPixmap(
            titleIcon.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    //设置标题
    QMainWindow::setWindowTitle(title);
    if (titleLabel) {
        titleLabel->setText(title);
    }
}
void CustomMainWindow::setupCustomTitleBar()
{
    //创建标题栏
    titleBar = new QWidget(this);
    titleBar->setObjectName("titleBar");
    titleBar->setFixedHeight(70);

    //创建标题栏布局
    QHBoxLayout *titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(30, 10, 30, 10);

    //创建图标
    iconLabel = new QLabel();
    iconLabel->setFixedSize(50, 50);
    iconLabel->setPixmap(QPixmap(":/icons/resources/icons/title.png").scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    iconLabel->setObjectName("iconLabel");
    titleLayout->addWidget(iconLabel);

    //创建标题
    titleLabel = new QLabel();
    titleLabel->setObjectName("titleLabel");
    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();

    //创建文件按钮
    fileButton = new QPushButton("文件(F)");
    fileButton->setObjectName("menuButton");
    fileButton->setFixedSize(160, 50);

    //创建文件菜单
    fileMenu = new QMenu(this);
    fileButton->setMenu(fileMenu);

    //创建帮助按钮
    helpButton = new QPushButton("帮助(H)");
    helpButton->setObjectName("menuButton");
    helpButton->setFixedSize(160, 50);

    //创建帮助菜单
    helpMenu = new QMenu(this);
    helpButton->setMenu(helpMenu);

    titleLayout->addWidget(fileButton);
    titleLayout->addWidget(helpButton);
    titleLayout->addSpacing(68);

    //创建窗口控制按钮
    minButton = new QPushButton();
    minButton->setObjectName("minButton");
    minButton->setFixedSize(38, 38); // 使用38x38以完整显示图片
    minButton->setFlat(true);        // 设置为扁平按钮，避免边框干扰

    closeButton = new QPushButton();
    closeButton->setObjectName("closeButton");
    closeButton->setFixedSize(38, 38); // 使用38x38以完整显示图片
    closeButton->setFlat(true);        // 设置为扁平按钮，避免边框干扰

    // 连接按钮信号
    connect(minButton, &QPushButton::clicked, this, &CustomMainWindow::showMinimized);
    connect(closeButton, &QPushButton::clicked, this, &CustomMainWindow::close);

    // 添加部件到标题栏布局
    titleLayout->addWidget(minButton);
    titleLayout->addWidget(closeButton);
}
void CustomMainWindow::setupBasicLayout()
{
    // 创建中央部件
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 添加标题栏到主布局
    mainLayout->addWidget(titleBar);

    // 创建内容区域
    QWidget *contentContainer = new QWidget;
    contentContainer->setObjectName("contentContainer");

    QVBoxLayout *contentLayout = new QVBoxLayout(contentContainer);
    contentLayout->setAlignment(Qt::AlignCenter);
    contentLayout->setContentsMargins(91, 123, 145, 120);
    contentLayout->setSpacing(0);

    //实验卡片栏
    contentWidget = new QStackedWidget();
    contentWidget->setObjectName("contentWidget");

    // 添加内容区域到主布局，并设置拉伸因子使其占据剩余空间
    contentLayout->addWidget(contentWidget);
    // mainLayout->addWidget(contentWidget, 1);
    mainLayout->addWidget(contentContainer);
}
void CustomMainWindow::setupWindowStyle()
{
    QString styleSheet = QString(R"(
        QMainWindow {
            background-image: url(:/icons/resources/icons/main-bg.png);
            background-repeat: no-repeat;
        }

        QWidget#contentContainer
        {
            background-color:transparent;
        }

        QWidget#titleBar {
            background: rgba(255,255,255,0.7);
            background-repeat: no-repeat;
            background-position: center;
        }

        QLabel#titleLabel {
            font-family: 'PingFang SC', 'Microsoft YaHei', sans-serif;
            font-weight: 400;
            color: #433E59;
            font-size: 32px;
            padding-left: 3px;
        }

        QPushButton#menuButton {
            background-color:#FFFFFF;
            border-radius: 25px;
            color: #2D2F4F;
            font-size: 28px;
            font-weight: 400;
            text-align: center;
            border: 1px solid #FFFFFF;
            outline: none; /* 关键：移除虚线 */
        }

        QPushButton#menuButton:hover {
            background-color: #D3D3D3;
            color: #2D2F4F;
        }

        QPushButton#menuButton:pressed {
            background-color: #3D73E9;
            color: #FFFFFF;
        }

        QPushButton#menuButton::menu-indicator {
            image: none; /* 隐藏默认箭头 */
        }

        QPushButton#minButton {
            border: none;
            background-color: transparent;
            background-image: url(:/icons/resources/icons/min-btn.png);
            background-repeat: no-repeat;
            background-position: center;
        }

        QPushButton#minButton:hover {
            background-image: url(:/icons/resources/icons/min-btn-hover.png);
            background-repeat: no-repeat;
            background-position: center;
        }

        QPushButton#minButton:pressed {
            background-image: url(:/icons/resources/icons/min-btn-clicked.png);
            background-repeat: no-repeat;
            background-position: center;
        }

        QPushButton#closeButton {
            border: none;
            background-color: transparent;
            background-image: url(:/icons/resources/icons/close-btn.png);
            background-repeat: no-repeat;
            background-position: center;
            border-radius: 50%;
        }

        QPushButton#closeButton:hover {
            background-image: url(:/icons/resources/icons/close-btn-hover.png);
            background-repeat: no-repeat;
            background-position: center;
        }

        QPushButton#closeButton:pressed {
            background-image: url(:/icons/resources/icons/close-btn-clicked.png);
            background-repeat: no-repeat;
            background-position: center;
        }
    )");
    setStyleSheet(styleSheet);
}
void CustomMainWindow::mousePressEvent(QMouseEvent *event)
{
    // 只有点击标题栏时才允许拖拽
    if (event->button() == Qt::LeftButton && titleBar
        && titleBar->geometry().contains(event->pos())) {
        isDragging = true;
        dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
}
void CustomMainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (isDragging && event->buttons() & Qt::LeftButton) {
        move(event->globalPosition().toPoint() - dragPosition);
        event->accept();
    }
}
void CustomMainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
        event->accept();
    }
}
void CustomMainWindow::about()
{
    // 创建自定义弹窗
    QDialog *dialog = new QDialog(this);
    dialog->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    dialog->setAttribute(Qt::WA_TranslucentBackground);
    dialog->setFixedSize(380, 200);

    // 主容器（带背景图片）
    QWidget *container = new QWidget(dialog);
    container->setFixedSize(380, 200);
    container->setStyleSheet(R"(
        QWidget {
            background-image: url(:/icons/resources/icons/tanchuang.png);
            background-repeat: no-repeat;
            background-position: center;
            border-radius: 12px;
        }
    )");

    QVBoxLayout *dialogLayout = new QVBoxLayout(dialog);
    dialogLayout->setContentsMargins(0, 0, 0, 0);
    dialogLayout->addWidget(container);

    // 容器内部布局
    QVBoxLayout *mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(60, 12, 20, 15);
    mainLayout->setSpacing(8);

    // 标题栏
    QHBoxLayout *titleLayout = new QHBoxLayout();
    titleLayout->setContentsMargins(0, 0, 0, 0);

    // 标题文字（居中）
    QLabel *titleLabel = new QLabel("关 于");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(R"(
        QLabel {
            font-family: 'PingFang SC', 'Microsoft YaHei', sans-serif;
            font-size: 18px;
            font-weight: 800;
            color: #5E81B5;
            background: transparent;
        }
    )");

    // 关闭按钮
    QPushButton *closeBtn = new QPushButton("×");
    closeBtn->setFixedSize(24, 24);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet(R"(
        QPushButton {
            background: transparent;
            border: none;
            font-size: 20px;
            font-weight: bold;
            color: #A0B3DC;
        }
        QPushButton:hover {
            color: #7A9BCC;
        }
    )");
    connect(closeBtn, &QPushButton::clicked, dialog, &QDialog::close);

    // 左侧占位（与关闭按钮对称）
    QWidget *leftSpacer = new QWidget();
    leftSpacer->setFixedWidth(12);
    leftSpacer->setStyleSheet("background: transparent;");

    titleLayout->addWidget(leftSpacer);
    titleLayout->addStretch();
    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();
    titleLayout->addWidget(closeBtn);
    titleLayout->addSpacing(6);

    // 分隔线
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("background-color: #E5E5E5; max-height: 1px;");

    // 内容区域
    QLabel *appNameLabel = new QLabel("气象雷达测量原理实验平台");
    appNameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    appNameLabel->setStyleSheet(R"(
        QLabel {
            font-family: 'PingFang SC', 'Microsoft YaHei', sans-serif;
            font-size: 18px;
            font-weight: bold;
            color: #252E5B;
            background: transparent;
        }
    )");

    QLabel *descLabel = new QLabel("这是一个用于气象雷达相关教学的应用程序");
    descLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    descLabel->setStyleSheet(R"(
        QLabel {
            font-family: 'PingFang SC', 'Microsoft YaHei', sans-serif;
            font-size: 15px;
            font-weight: 300;
            color: #252E5B;
            background: transparent;
        }
    )");

    QLabel *versionLabel = new QLabel("版本 1.0.0.0");
    versionLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    versionLabel->setStyleSheet(R"(
        QLabel {
            font-family: 'PingFang SC', 'Microsoft YaHei', sans-serif;
            font-size: 15px;
            font-weight: bold;
            color: #252E5B;
            background: transparent;
        }
    )");

    // OK按钮
    QPushButton *okBtn = new QPushButton("OK");
    okBtn->setFixedSize(90, 28);
    okBtn->setCursor(Qt::PointingHandCursor);
    okBtn->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1:1, y1:0, x2:0, y2:0, stop:0 #70A9FF, stop:1 #507CFF);
            border: none;
            border-radius: 14px;
            font-family: 'PingFang SC', 'Microsoft YaHei', sans-serif;
            font-size: 18px;
            font-weight: bold;
            color: #FFFFFF;
        }
        QPushButton:hover {
            background: qlineargradient(x1:1, y1:0, x2:0, y2:0, stop:0 #5A99EF, stop:1 #406CEF);
        }
        QPushButton:pressed {
            background: qlineargradient(x1:1, y1:0, x2:0, y2:0, stop:0 #4A89DF, stop:1 #305CDF);
        }
    )");
    connect(okBtn, &QPushButton::clicked, dialog, &QDialog::accept);

    mainLayout->addLayout(titleLayout);
    mainLayout->addWidget(line);
    mainLayout->addSpacing(0);
    mainLayout->addWidget(appNameLabel);
    mainLayout->addWidget(descLabel);
    mainLayout->addWidget(versionLabel);
    mainLayout->addStretch();

    // OK按钮使用绝对定位，固定在右下角
    okBtn->setParent(container);
    okBtn->move(380 - 90 - 40, 200 - 28 - 47); // 右下角

    dialog->exec();
    dialog->deleteLater();
}
void CustomMainWindow::setupMenu()
{
    //清空默认菜单
    fileMenu->clear();
    helpMenu->clear();

    // 设置菜单样式
    QString menuStyle = R"(
        QMenu {
            background-color:rgba(247, 249, 255, 0.5);
            border-radius: 10px;
            border: 2px solid #6796FE;
            padding: 0px;
        }

        QMenu::item {
            background-color: transparent ;
            border-radius: 8px;
            font-family: 'PingFang SC';
            font-weight: 400;
            font-size: 18px;
            color: #252E5B;
            padding: 5px 25px;
            margin: 2px 2px;
        }

        QMenu::item:selected {
            background-color: qlineargradient(x1:1, y1:0, x2:0, y2:0, stop:0 #70A9FF, stop:1 #507CFF);
            color: #FFFFFF;
        }

        QMenu::item:pressed {
            background-color: qlineargradient(x1:1, y1:0, x2:0, y2:0, stop:0 #5A99EF, stop:1 #406CEF);

        }
    )";
    // 设置文件菜单窗口属性以支持圆角和透明
    fileMenu->setWindowFlags(fileMenu->windowFlags() | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    fileMenu->setAttribute(Qt::WA_TranslucentBackground);
    fileMenu->setStyleSheet(menuStyle);

    // 添加文件菜单项
    setupExperimentMenu();

    // 设置帮助菜单窗口属性以支持圆角和透明
    helpMenu->setWindowFlags(helpMenu->windowFlags() | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    helpMenu->setAttribute(Qt::WA_TranslucentBackground);
    helpMenu->setMinimumWidth(helpButton->width());
    helpMenu->setStyleSheet(menuStyle);

    // 添加帮助菜单项
    QAction *aboutAction = new QAction("关于(A)", this);
    helpMenu->addAction(aboutAction);
    connect(aboutAction, &QAction::triggered, this, &CustomMainWindow::about);
}
void CustomMainWindow::setupExperimentMenu()
{
    // 只获取【当前平台】的实验列表
    SystemPlatformInfo currentPlatform = ExperimentManager::instance().getCurrentPlatform();

    for (const ExperimentInfo& exp : currentPlatform.experiments) {
        QAction* action = new QAction(exp.name, this);
        action->setData(exp.expId); // 存入全局唯一的 expId
        connect(action, &QAction::triggered, this, [this, action]() {
            this->onMenuExperimentSelected(action->data().toInt());
        });
        fileMenu->addAction(action);
    }
}

