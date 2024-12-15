#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class PlaylistWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private:
    PlaylistWidget *playlistWidget;
};

#endif // MAINWINDOW_H

