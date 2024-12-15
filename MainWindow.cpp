#include "MainWindow.h"
#include "PlaylistWidget.h"
#include <QVBoxLayout>
#include <QTabWidget>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    auto *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    auto *mainLayout = new QVBoxLayout(centralWidget);

    auto *tabWidget = new QTabWidget(this);
    tabWidget->addTab(new QWidget(), "浏览器");

    playlistWidget = new PlaylistWidget(this);
    tabWidget->addTab(playlistWidget, "播放列表");

    mainLayout->addWidget(tabWidget);
}
