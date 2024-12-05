#include <iostream>
#include <QApplication>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QMediaPlaylist>
#include <string>
#include <vector>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QFileInfo>
#include <QtWidgets/QFileIconProvider>
#include <QDesktopServices>
#include <QImageReader>
#include <QMessageBox>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include "the_player.h"
#include "the_button.h"

// read in videos and thumbnails to this directory
std::vector<TheButtonInfo> getInfoIn (std::string loc) {

    std::vector<TheButtonInfo> out =  std::vector<TheButtonInfo>();
    QDir dir(QString::fromStdString(loc));
    QDirIterator it(dir);

    while (it.hasNext()) { // for all files
        QString f = it.next();
        qDebug() << "Found file: " << f;

        if (f.contains("."))
#if defined(_WIN32)
            if (f.contains(".wmv")) { // windows
#else
            if (f.contains(".mp4") || f.contains("MOV")) { // mac/linux
#endif
                QString thumb = f.left(f.length() - 4) + ".png";
                if (QFile(thumb).exists()) { // if a png thumbnail exists
                    QImageReader *imageReader = new QImageReader(thumb);
                    QImage sprite = imageReader->read(); // read the thumbnail
                    if (!sprite.isNull()) {
                        QIcon* ico = new QIcon(QPixmap::fromImage(sprite)); // create an icon for the button
                        QUrl* url = new QUrl(QUrl::fromLocalFile(f)); // convert the file location to a generic url
                        out.push_back(TheButtonInfo(url, ico)); // add to the output list
                    } else {
                        qDebug() << "Warning: skipping video because I couldn't process thumbnail " << thumb;
                    }
                } else {
                    qDebug() << "Warning: skipping video because I couldn't find thumbnail " << thumb;
                }
            }
    }

    return out;
}

QString getVideoPath() { //write by HU
    // Gets the current working directory
    QString currentDir = QDir::currentPath();
    qDebug() << "Current working directory: " << currentDir;

    // Concatenate out the path to the destination video folder
    QDir dir(currentDir);  // Current directory
    dir.cdUp();  // Return to the previous directory
    dir.cd("2811_cw3-master-release-lowres/videos");  // Go to the video folder

    qDebug() << "Video directory path: " << dir.absolutePath();  //Output the final directory path
    return dir.absolutePath();  // Return an absolute path
}

int main(int argc, char *argv[]) {
    qDebug() << "Current working directory: " << QDir::currentPath();
    // Let's check if Qt is operational first
    qDebug() << "Qt version: " << QT_VERSION_STR;

    // Create the Qt Application
    QApplication app(argc, argv);

    // Collect all the videos in the folder
    std::vector<TheButtonInfo> videos;

    // Get the video path and load the video information
    videos = getInfoIn(getVideoPath().toStdString());

    if (videos.size() == 0) {
        const int result = QMessageBox::information(
                    NULL,
                    QString("Tomeo"),
                    QString("No videos found in './videos'!"));
        exit(-1);
    }

    // The widget that will show the video
    QVideoWidget *videoWidget = new QVideoWidget;

    // The QMediaPlayer which controls the playback
    ThePlayer *player = new ThePlayer;
    player->setVideoOutput(videoWidget);

    // A row of buttons
    QWidget *buttonWidget = new QWidget();
    std::vector<TheButton*> buttons;
    QHBoxLayout *layout = new QHBoxLayout();
    buttonWidget->setLayout(layout);

    // Create the four buttons
    for (int i = 0; i < 4 && i < videos.size(); i++) {
        TheButton *button = new TheButton(buttonWidget);
        button->connect(button, SIGNAL(jumpTo(TheButtonInfo*)), player, SLOT(jumpTo(TheButtonInfo*))); // When clicked, tell the player to play
        buttons.push_back(button);
        layout->addWidget(button);
        button->init(&videos.at(i));
    }

    // Tell the player what buttons and videos are available
    player->setContent(&buttons, &videos);

    // Create the main window and layout
    QWidget window;
    QVBoxLayout *top = new QVBoxLayout();
    window.setLayout(top);
    window.setWindowTitle("Tomeo");
    window.setMinimumSize(800, 680);

    // Add the video and the buttons to the top-level widget
    top->addWidget(videoWidget);
    top->addWidget(buttonWidget);

    // Show the window
    window.show();

    // Wait for the app to terminate
    return app.exec();
}
