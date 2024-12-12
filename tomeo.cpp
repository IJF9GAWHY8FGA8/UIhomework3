#include <QApplication>
#include <QtMultimediaWidgets/QVideoWidget>
#include <string>
#include <vector>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QFileInfo>
#include <QtWidgets/QFileIconProvider>
#include <QDesktopServices>
#include <QImageReader>
#include <QObject>
#include <QMessageBox>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QSlider>
#include "the_player.h"
#include "the_button.h"

std::vector<TheButtonInfo> getInfoIn(std::string loc) {
    std::vector<TheButtonInfo> out;
    QDir dir(QString::fromStdString(loc));
    QDirIterator it(dir);

    while (it.hasNext()) {
        QString f = it.next();
        qDebug() << "Found file: " << f;

        if (f.contains("."))
#if defined(_WIN32)
            if (f.contains(".wmv")) {
#else
            if (f.contains(".mp4") || f.contains("MOV")) {
#endif
                QString thumb = f.left(f.length() - 4) + ".png";
                if (QFile(thumb).exists()) {
                    QImageReader *imageReader = new QImageReader(thumb);
                    QImage sprite = imageReader->read();
                    if (!sprite.isNull()) {
                        QIcon* ico = new QIcon(QPixmap::fromImage(sprite));
                        QUrl* url = new QUrl(QUrl::fromLocalFile(f));
                        out.push_back(TheButtonInfo(url, ico));
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

QString getVideoPath() {
    QString currentDir = QDir::currentPath();
    qDebug() << "Current working directory: " << currentDir;

    QDir dir(currentDir);
    dir.cdUp();
    dir.cd("2811_cw3-master-release-lowres/videos");
    qDebug() << "Video directory path: " << dir.absolutePath();
    return dir.absolutePath();
}

int main(int argc, char *argv[]) {
    qDebug() << "Current working directory: " << QDir::currentPath();
    qDebug() << "Qt version: " << QT_VERSION_STR;

    QApplication app(argc, argv);

    std::vector<TheButtonInfo> videos = getInfoIn(getVideoPath().toStdString());
    if (videos.size() == 0) {
        QMessageBox::information(NULL, QString("Tomeo"), QString("No videos found in './videos'!"));
        exit(-1);
    }

    QVideoWidget *videoWidget = new QVideoWidget;
    ThePlayer *player = new ThePlayer;
    player->setVideoOutput(videoWidget);

    QWidget *buttonWidget = new QWidget;
    std::vector<TheButton*> buttons;
    QHBoxLayout *layout = new QHBoxLayout(buttonWidget);

    for (int i = 0; i < 4 && i < videos.size(); i++) {
        TheButton *button = new TheButton(buttonWidget);
        button->connect(button, SIGNAL(jumpTo(TheButtonInfo*)), player, SLOT(jumpTo(TheButtonInfo*)));
        buttons.push_back(button);
        layout->addWidget(button);
        button->init(&videos.at(i));
    }

    player->setContent(&buttons, &videos);

    QSlider *speedSlider = new QSlider(Qt::Horizontal);
    speedSlider->setRange(5, 20);
    speedSlider->setValue(10);
    speedSlider->setTickInterval(1);
    speedSlider->setTickPosition(QSlider::TicksBelow);
    layout->addWidget(speedSlider);
    QObject::connect(speedSlider, &QSlider::valueChanged, [player](int value) {
        player->setPlaybackSpeed(value / 10.0);
    });

    QWidget window;
    QVBoxLayout *top = new QVBoxLayout(&window);
    window.setWindowTitle("Tomeo");
    window.setMinimumSize(800, 680);

    top->addWidget(videoWidget);
    top->addWidget(buttonWidget);

    window.show();

    return app.exec();
}
