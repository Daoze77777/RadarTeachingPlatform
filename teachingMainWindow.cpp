#include "teachingMainWindow.h"
#include <QApplication>
#include <QScreen>
#include <qwidget.h>

#include <QGridLayout>
#include <QStyle>
#include <QVBoxLayout>
#include "mainWindow.h"
#include "ExperimentManager.h"

TeachingMainWindow::TeachingMainWindow(QWidget *parent)
{
    //加载窗口样式
    setupWindowStyle();
    //设置自定义标题栏
    setupCustomTitleBar();
    //设置基本布局
    setupBasicLayout();
    //设置窗口标题和图标
    setCustomMainWindowTitle(":/icons/resources/icons/title.png", "气象雷达测量原理实验平台");
    //设置菜单
    setupMenu();
    initUI();
    //设置窗口大小
    resize(1920, 1033);
    //this->showFullScreen();



}
TeachingMainWindow::~TeachingMainWindow() {}
void TeachingMainWindow::initUI()
{
    // 将主页面添加到contentWidget中
    m_homePage = createHomePage();
    contentWidget->addWidget(m_homePage);
    contentWidget->setCurrentWidget(m_homePage);
}
QWidget *TeachingMainWindow::createHomePage()
{
    // 创建主页容器
    QWidget *homePage = new QWidget(this);
    homePage->setObjectName("homePage");
    homePage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(homePage);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // 创建内容容器
    QWidget *contentWidget = new QWidget;
    //contentWidget->setObjectName("contentWidget");
    mainLayout->addWidget(contentWidget, /*stretch=*/1);

    // 内容容器的布局
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setSpacing(0);
    contentLayout->setContentsMargins(0, 0, 0, 0);

    // 主容器的布局
    QVBoxLayout *containerLayout = contentLayout;
    containerLayout->setSpacing(60);

    // 标题区域 中+英
    QWidget *titleArea = new QWidget();
    titleArea->setObjectName("titleWidget");
    titleArea->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

    QVBoxLayout *titleLayout = new QVBoxLayout(titleArea);
    titleLayout->setSpacing(8);                              // 中文与英文间距
    titleLayout->setContentsMargins(0, 0, 0, 0);             // 无内边距
    titleLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft); // 左上对齐

    m_cnTitleLabel = new QLabel("测距原理实验平台", titleArea);
    m_cnTitleLabel->setObjectName("mainTitle");

    m_enTitleLabel = new QLabel("Distance measurement experimental platform", titleArea);
    m_enTitleLabel->setObjectName("subTitle");

    titleLayout->addWidget(m_cnTitleLabel);
    titleLayout->addWidget(m_enTitleLabel);
    containerLayout->addWidget(titleArea);

    // 设置主页背景透明，显示父窗口的背景图片，并添加标题样式
    homePage->setStyleSheet(R"(
    QWidget#homePage {
        background-color: transparent;
        background-repeat: no-repeat;
    }
    QWidget#contentWidget {
        background-color: transparent;
    }
    QWidget#titleWidget {
        background-color: transparent;
    }
    QLabel#mainTitle {
        font-size: 75px;
        font-family: 'PingFang SC', 'Microsoft YaHei', sans-serif;
        font-weight: 600;
        font-style: italic;
        color: #2D2F4F;
    }
    QLabel#subTitle {
        padding-left: 63px;
        font-size: 50px;
        font-family: 'PingFang SC', 'Microsoft YaHei', sans-serif;
        font-weight: 600;
        font-style: italic;
        color: #5E85E8;
    }
)");

    // 中间链接区域
    QWidget *centerWidget = new QWidget;
    centerWidget->setObjectName("centerWidget");

    QVBoxLayout *centerLayout = new QVBoxLayout(centerWidget);
    centerLayout->setSpacing(0);
    centerLayout->setContentsMargins(0, 0, 0, 0);

    containerLayout->addWidget(centerWidget);

    m_linksWidget = new QWidget;
    m_linksWidget->setObjectName("linksWidget");
    m_linksWidget->setMinimumHeight(480);

    m_linksLayout = new QGridLayout(m_linksWidget);
    m_linksLayout->setContentsMargins(120, 40, 120, 45);

    QWidget *linksWidget = m_linksWidget;
    QGridLayout *linksLayout = m_linksLayout;
    centerLayout->addWidget(linksWidget);
    // 背景与按钮样式
    centerWidget->setStyleSheet(R"(
    QWidget#centerWidget {
        background-color: transparent;
    }
    QWidget#linksWidget {
        background-color: transparent;
        border-image: url(:/icons/resources/icons/links-widget.png) 60 50 60 50 stretch stretch;
    }
    QPushButton {
        background-image: url(:/icons/resources/icons/link-btn-circle.png);
        background-repeat: no-repeat;

        border: none;
        color: #2D2F4F;
        text-align:  left;
        padding-left: 80px;
        padding-right:30px;
        font-family: PingFangSC, 'Microsoft YaHei';
        font-weight: 400;
        font-size: 28px;
    }
    QPushButton:hover {
        background-image: url(:/icons/resources/icons/link-btn-hover.png);
        background-repeat: no-repeat;
        background-position: left center;
        color: #2D2F4F;
    }
    QPushButton:pressed {
        background-image: url(:/icons/resources/icons/link-btn-pressed.png);
        background-repeat: no-repeat;
        background-position: left center;
        color: #FFFFFF;
    }
)");

    // 1. 从管理类获取【当前激活平台】的所有实验数据
    SystemPlatformInfo currentPlatform = ExperimentManager::instance().getCurrentPlatform();
    QList<ExperimentInfo> expList = currentPlatform.experiments;

    // 2. 清理旧按钮（保留你原来的防内存泄漏逻辑）
    qDeleteAll(m_experimentButtons);
    m_experimentButtons.clear();

    //创建并布局按钮（两列，每列最多5行）
    for (int i = 0; i < expList.size(); ++i) {
        const ExperimentInfo& exp = expList[i]; // 取出当前实验的结构体数据
        QPushButton *btn = new QPushButton(exp.name, linksWidget);

        // 设置最小尺寸（建议考虑使用 setMinimumSize 配合布局拉伸）
        btn->setMinimumSize(680, 70);
        // 存储指针
        m_experimentButtons.append(btn);

        // 计算行列：i % 5 得到行 (0-4)，i / 5 得到列 (0-1)
        int row = i % 5;
        int col = i / 5;
        linksLayout->addWidget(btn, row, col);

        int expId = exp.expId; // 实验ID，比如 101, 102
        connect(btn, &QPushButton::clicked, this, [this, expId]() {
            MainWindow *mainWin = new MainWindow(expId);    //根据expId创建 MainWindow 实例
            mainWin->setAttribute(Qt::WA_DeleteOnClose);    //资源管理：关闭主窗口时自动释放内存
            // 接收返回信号
            connect(mainWin, &MainWindow::returnToHome, this, [this, mainWin]() {
                this->show();
                mainWin->close();
            });
            mainWin->show();
            this->hide();
        });
    }
    return homePage;
}
void TeachingMainWindow::updateSwitchButtonPosition()
{
    if (m_switchPlatformBtn && isVisible()) { // 👈 关键：检查指针和可见性
        int x = width() - 250;
        int y = 85;
        // 防止负坐标
        if (x < 0)
            x = 0;
        if (y < 0)
            y = 0;
        m_switchPlatformBtn->move(x, y);
    }
}
void TeachingMainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    updateSwitchButtonPosition();
}
void TeachingMainWindow::onMenuExperimentSelected(int expId) {
    // 逻辑：直接触发窗口跳转
    MainWindow *mainWin = new MainWindow(expId);
    mainWin->setAttribute(Qt::WA_DeleteOnClose);
    // 核心联动：接收返回信号
     connect(mainWin, &MainWindow::returnToHome, this, [this, mainWin]() {
         this->showNormal();   // 重新显示实验选择界面 (如果之前是全屏，可以调 showMaximized())
         mainWin->close();     // 关闭当前的实验主窗口 (触发 WA_DeleteOnClose 自动清理内存)
     });
     mainWin->show();
    this->hide(); // 隐藏欢迎页
}
