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

    //创建Body内容容器
    QWidget *bodyContainer = new QWidget;
    //bodyContainer->setStyleSheet("background-color:purple;");
    //Body部分采用水平布局
    QHBoxLayout *bodyLayout = new QHBoxLayout(bodyContainer);
    bodyLayout->setContentsMargins(0, 0, 0, 0);
    bodyLayout->setSpacing(15);
    bodyLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop); // 左上对齐

    // 按顺序添加：左 -> 中 -> 右
    bodyLayout->addWidget(m_leftSidebarContainer); // 固定宽度
    bodyLayout->addWidget(m_CenterContainer);
    bodyLayout->addWidget(m_rightPanelContainer);

    // 将Body加入主布局
    m_rootLayout->addWidget(bodyContainer);
    resize(1920, 1033);
}

void MainWindow::setupWindowBase()
{
    //获取CentralWidget(父类构造函数中通常已经setCentralWidget了),如果父类没做，就自己new一个
    QWidget *mainContainer = this->centralWidget;
    if (!mainContainer)
    {
        mainContainer = new QWidget(this);
        setCentralWidget(mainContainer);
    }
    mainContainer->setObjectName("mainContainer");
    mainContainer->setStyleSheet("#mainContainer { background-image: none; background-color: #E9EEF8; }");
    m_rootContainer = mainContainer;

    //核心布局：顶层垂直布局
    QVBoxLayout *mainLayout = new QVBoxLayout(mainContainer);
    mainLayout->setContentsMargins(0, 0, 0, 0); // 彻底清空父类的巨大边距
    mainLayout->setSpacing(0);
    m_rootLayout = mainLayout;

    // 添加标题栏 (复用父类的成果)
    if (this->titleBar)
    {
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
    //创建左侧导航栏容器
    m_leftSidebarContainer = new QWidget(this);
    m_leftSidebarContainer->setFixedSize(270, 963);
    //m_leftSidebarContainer->setFixedWidth(270);
    m_leftSidebarContainer->setObjectName("leftSidebarContainer");
    m_leftSidebarContainer->setStyleSheet("background-color:blue;");

    //设置布局
    QVBoxLayout *leftSidebarContentLayout = new QVBoxLayout(m_leftSidebarContainer);
    leftSidebarContentLayout->setContentsMargins(0, 0, 0, 0);
    leftSidebarContentLayout->setSpacing(0);

    // 创建 QScrollArea
    QScrollArea *scrollArea = new QScrollArea(m_leftSidebarContainer);
    //scrollArea->setFixedWidth(270);  // 固定宽度270px
    scrollArea->setWidgetResizable(true);                             //内部部件随滚动区缩放
    //scrollArea->setFrameShape(QFrame::NoFrame);                       // 无边框
    //scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 屏蔽横向滚动条
    //scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 禁用纵向滚动条，避免展开时闪烁
    scrollArea->setStyleSheet("background-color:yellow;");
    leftSidebarContentLayout->addWidget(scrollArea);

    // 创建滚动区内部的容器部件
    QWidget *scrollContent = new QWidget();
    scrollContent->setObjectName("scrollContent");
    scrollContent->setStyleSheet("background-color: red;"); // 导航栏底色

    // 为内部容器设置垂直布局，用来放你的那些 CollapsibleGroup
    QVBoxLayout *contentLayout = new QVBoxLayout(scrollContent);
    contentLayout->setContentsMargins(10, 10, 10, 10);
    contentLayout->setSpacing(8);
    contentLayout->setAlignment(Qt::AlignTop); // 确保内容从顶部开始排列

}
void MainWindow::setupCenterArea()
{
    // 1. 创建中间总容器
    QWidget *centerContainer = new QWidget(this);
    centerContainer->setObjectName("centerContainer");
    centerContainer->setFixedSize(1419, 963);
    centerContainer->setStyleSheet("QWidget#centerContainer { background-color: orange; }");
    m_CenterContainer = centerContainer;

    // 总垂直布局：负责切分“上显示、下操作”
    QVBoxLayout *mainVLayout = new QVBoxLayout(centerContainer);
    mainVLayout->setContentsMargins(15, 15, 15, 15);
    mainVLayout->setSpacing(15);

    // 上方显示区：放置雷达和示波器
    QWidget *topArea = new QWidget;
    //topArea->setStyleSheet("background-color: cyan;");
    //上方显示区水平布局
    QHBoxLayout *topLayout = new QHBoxLayout(topArea);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setSpacing(15);

    // 预留给雷达和示波器
    //m_radarDisplay = new RadarRangingDisplayWidget();
    //m_oscilloscope = new OscilloscopeWidget();

    //暂时用占位符，以后直接 addWidget(m_radarDisplay) 即可
    QFrame *radarBox = new QFrame();
    radarBox->setStyleSheet("background-color: #FFFFFF; border-radius: 10px; border: 1px solid #FFFFFF;");
    QFrame *oscilloBox = new QFrame();
    oscilloBox->setStyleSheet("background-color: #FFFFFF; border-radius: 10px; border: 1px solid #FFFFFF;");

    topLayout->addWidget(radarBox, 1);   // 比例 1
    topLayout->addWidget(oscilloBox, 1); // 比例 1

    // 下方操作区，创建最外层的堆叠容器
    m_centerStack = new QStackedWidget;
    m_centerStack->setObjectName("centerStack");
    m_centerStack->setStyleSheet("QWidget#centerStack { background-color: #FFFFFF; border-radius: 10px; border: 1px solid #FFFFFF;}");

    // 将两个大块放入主垂直布局，并精准分配比例
    mainVLayout->addWidget(topArea, 5);       // 上方占 5 份高度
    mainVLayout->addWidget(m_centerStack, 5); // 下方占 5 份高度
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
