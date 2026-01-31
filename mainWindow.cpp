#include "mainWindow.h"
#include <QScrollArea>
#include <QStatusBar>
#include "teachingMainWindow.h"
MainWindow::MainWindow(TeachingMainWindow *teachingWindow, ExperimentType expType, QWidget *parent)
    : m_teachingWindow(teachingWindow)
{
    initUI();
}
MainWindow::~MainWindow() {}

void MainWindow::initUI()
{
    //基础窗口设置
    setupTitleBar();
    setupWindowBase();
    // 设置自定义窗口标题和图标
    setCustomMainWindowTitle(":/icons/resources/icons/title.png", "气象雷达测量原理实验平台");
    //初始化各板块
    setupLeftSidebar();
    setupCenterArea();
    setupRightPanel();

    //Body部分采用水平布局,experimentPage
    QWidget *bodyContainer = new QWidget;
    bodyContainer->setStyleSheet("background-color:purple;");

    QHBoxLayout *bodyLayout = new QHBoxLayout(bodyContainer);
    bodyLayout->setContentsMargins(0, 0, 0, 0);
    bodyLayout->setSpacing(15);
    bodyLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop); // 👈 关键！

    // 按顺序添加：左 -> 中 -> 右
    bodyLayout->addWidget(m_leftSidebarContainer); // 固定宽度
    bodyLayout->addWidget(m_centerStack);
    bodyLayout->addWidget(m_rightPanelContainer);
    //bodyLayout->addWidget(m_rightPanelContainer);  // 固定宽度
    //statusBar()->hide();

    // 5. 将 Header 和 Body 加入主布局
    //m_rootLayout->addWidget(m_leftSidebarContainer);
    m_rootLayout->addWidget(bodyContainer);
    resize(1920, 1033);
}

void MainWindow::setupWindowBase()
{
    //获取CentralWidget(父类构造函数中通常已经setCentralWidget了),如果父类没做，就自己new一个
    QWidget *mainContainer = this->centralWidget;
    if (!mainContainer) {
        mainContainer = new QWidget(this);
        setCentralWidget(mainContainer);
    }
    mainContainer->setObjectName("mainContainer");
    mainContainer->setStyleSheet(
        "#mainContainer { background-image: none; background-color: #E9EEF8; }");
    m_rootContainer = mainContainer;

    //核心布局：顶层垂直布局
    QVBoxLayout *mainLayout = new QVBoxLayout(mainContainer);
    mainLayout->setContentsMargins(0, 0, 0, 0); // 彻底清空父类的巨大边距
    mainLayout->setSpacing(0);
    m_rootLayout = mainLayout;

    // 添加标题栏 (复用父类的成果)
    if (this->titleBar) {
        mainLayout->addWidget(this->titleBar);
        setupWindowStyle(); //设置样式表
    }

    // 创建实验内容专属的 StackedWidget (这就是你之后切换不同实验的舞台)
    //     if (!this->contentWidget)
    // {
    //     this->contentWidget = new QStackedWidget(this);
    // }
    //mainLayout->addWidget(this->contentWidget);
}
void MainWindow::setupTitleBar()
{
    setupCustomTitleBar();
    // 1. 安全检查
    //if (!m_teachingWindow || !titleBar) return;
    // 2. 让教学窗口初始化它的菜单对象(fileMenu和helpMenu)
    // 这样 fileMenu 里面就会装满 setupExperimentMenu()定义的内容
    //m_teachingWindow->setupMenu();
}
void MainWindow::setupLeftSidebar()
{
    m_leftSidebarContainer = new QWidget(this);
    //m_leftSidebarContainer->setFixedWidth(280);  //设置固定宽度
    m_leftSidebarContainer->setFixedSize(280, 963);
    m_leftSidebarContainer->setObjectName("leftSidebarContainer");
    m_leftSidebarContainer->setStyleSheet("background-color:blue;");

    //为外层容器设置布局
    QVBoxLayout *leftSidebarLayout = new QVBoxLayout(m_leftSidebarContainer);
    leftSidebarLayout->setContentsMargins(0, 0, 0, 0);
    leftSidebarLayout->setSpacing(0);

    // 创建 QScrollArea
    QScrollArea *scrollArea = new QScrollArea(m_leftSidebarContainer);
    //scrollArea->setFixedWidth(270);  // 固定宽度270px
    scrollArea->setWidgetResizable(true);                             //内部部件随滚动区缩放
    scrollArea->setFrameShape(QFrame::NoFrame);                       // 无边框
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 屏蔽横向滚动条
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 禁用纵向滚动条，避免展开时闪烁
    //scrollArea->setStyleSheet("background-color:yellow;");
    leftSidebarLayout->addWidget(scrollArea);
}

void MainWindow::setupCenterArea()
{
    // 1. 创建中间总容器
    QWidget *centerContainer = new QWidget(this);
    centerContainer->setObjectName("centerContainer");
    centerContainer->setFixedSize(1419, 963);
    centerContainer->setStyleSheet("QWidget#centerContainer { background-color: cyan; }");

    // 总垂直布局：负责切分“上显示、下操作”
    QVBoxLayout *mainVLayout = new QVBoxLayout(centerContainer);
    mainVLayout->setContentsMargins(15, 15, 15, 15);
    mainVLayout->setSpacing(15);

    // 上方显示区：放置雷达和示波器
    QWidget *topArea = new QWidget();
    QHBoxLayout *topLayout = new QHBoxLayout(topArea);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setSpacing(15);

    // 预留给雷达和示波器
    // m_radarDisplay = new RadarRangingDisplayWidget();
    // m_oscilloscope = new OscilloscopeWidget();

    // 暂时用占位符，以后直接 addWidget(m_radarDisplay) 即可
    QFrame *radarBox = new QFrame();
    radarBox->setStyleSheet(
        "background-color: #001529; border-radius: 10px; border: 2px solid #3E4F5F;");
    QFrame *oscilloBox = new QFrame();
    oscilloBox->setStyleSheet(
        "background-color: #000; border-radius: 10px; border: 2px solid #3E4F5F;");

    topLayout->addWidget(radarBox, 1);   // 比例 1
    topLayout->addWidget(oscilloBox, 1); // 比例 1

    // 下方操作区：这就是你的 m_centerStack
    // 创建最外层的堆叠容器
    m_centerStack = new QStackedWidget;
    m_centerStack->setObjectName("centerStack");
    m_centerStack->setStyleSheet("QWidget#centerStack { background-color: #F7F9FF;}");

    // 将两个大块放入主垂直布局，并精准分配比例
    mainVLayout->addWidget(topArea, 6);       // 上方占 6 份高度
    mainVLayout->addWidget(m_centerStack, 4); // 下方占 4 份高度
}
void MainWindow::setupRightPanel()
{
    // 1. 创建右侧固定宽度的容器
    m_rightPanelContainer = new QWidget(this);
    m_rightPanelContainer->setFixedSize(200, 963); // 根据你之前的 UI 设定宽度
    m_rightPanelContainer->setObjectName("rightPanelContainer");
    // 设置背景色，通常是稍微深一点的蓝色或灰色，以区分中央区域
    m_rightPanelContainer->setStyleSheet("QWidget#rightPanelContainer { background-color: pink; }");

    // 2. 创建垂直布局
    QVBoxLayout *rightPanelLayout = new QVBoxLayout(m_rightPanelContainer);
    rightPanelLayout->setContentsMargins(5, 10, 5, 10);
    rightPanelLayout->setSpacing(15);
    rightPanelLayout->setAlignment(Qt::AlignTop);
}
