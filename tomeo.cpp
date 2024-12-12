#include <QApplication>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QMediaPlaylist>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFileInfo>
#include <QDirIterator>
#include <QImageReader>
#include <QIcon>
#include "the_player.h"
#include "the_button.h"

// Get video and thumbnail information
std::vector<TheButtonInfo> getInfoIn(std::string loc) {
    std::vector<TheButtonInfo> out;
    QDir dir(QString::fromStdString(loc));
    QDirIterator it(dir);

    while (it.hasNext()) {
        QString f = it.next();
        qDebug() << "Found file: " << f;

        if (f.contains(".")) {
#if defined(_WIN32)
            if (f.contains(".wmv")) {
#else
            if (f.contains(".mp4") || f.contains("MOV")) {
#endif
                QString thumb = f.left(f.length() - 4) + ".png"; // Assume the thumbnail is in .png format
                if (QFile(thumb).exists()) {
                    QImageReader *imageReader = new QImageReader(thumb);
                    QImage sprite = imageReader->read(); // Read the thumbnail
                    if (!sprite.isNull()) {
                        QIcon* ico = new QIcon(QPixmap::fromImage(sprite)); // Create the button icon
                        QUrl* url = new QUrl(QUrl::fromLocalFile(f)); // Convert the file path to a QUrl
                        out.push_back(TheButtonInfo(url, ico)); // Add to the output list
                    } else {
                        qDebug() << "Warning: Skipping video due to failed thumbnail processing " << thumb;
                    }
                } else {
                    qDebug() << "Warning: Skipping video due to missing thumbnail " << thumb;
                }
            }
        }
    }
    return out;
}

// Get the video directory path
QString getVideoPath() {
    QString currentDir = QDir::currentPath();
    qDebug() << "Current working directory: " << currentDir;
    QDir dir(currentDir);
    dir.cdUp();
    dir.cd("2811_cw3-master-release-lowres/videos");  // Video storage directory
    return dir.absolutePath();
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Get the video files and thumbnails
    std::vector<TheButtonInfo> videos = getInfoIn(getVideoPath().toStdString());
    if (videos.empty()) {
        QMessageBox::warning(nullptr, "No Videos", "No videos found in the './videos' folder!");
        return -1;
    }

    // Create the video display widget
    QVideoWidget *videoWidget = new QVideoWidget;
    ThePlayer *player = new ThePlayer;
    player->setVideoOutput(videoWidget);

    // Set up button layout
    QWidget *buttonWidget = new QWidget();
    QHBoxLayout *buttonLayout = new QHBoxLayout();  // Use one layout
    buttonWidget->setLayout(buttonLayout);

    // Create and initialize buttons
    std::vector<TheButton*> buttons; // Declare the button container
    for (int i = 0; i < 4 && i < videos.size(); i++) {
        TheButton *button = new TheButton(buttonWidget);
        button->connect(button, SIGNAL(jumpTo(TheButtonInfo*)), player, SLOT(jumpTo(TheButtonInfo*)));
        buttonLayout->addWidget(button);  // Use buttonLayout instead of layout
        button->init(&videos.at(i));
        buttons.push_back(button); // Add the button to the container
    }

    // Set player content
    player->setContent(&buttons, &videos);

    // Create the main window
    QWidget window;
    QVBoxLayout *topLayout = new QVBoxLayout();
    window.setLayout(topLayout);
    window.setWindowTitle("Tomeo Video Player");
    window.setMinimumSize(800, 600);

    // Create the pause button
    QPushButton* pauseButton = new QPushButton("||");

    // Create the previous button (double triangle)
    QPushButton *prevButton = new QPushButton();
    prevButton->setText("◁◁");  // Double triangle represents "Previous"
    prevButton->setStyleSheet(
        "QPushButton {"
        "  border: none;"
        "  background-color: transparent;"
        "  font-size: 30px;"
        "  font-weight: bold;"
        "  color: black;"
        "}"
    );

    // Create the next button (double triangle)
    QPushButton *nextButton = new QPushButton();
    nextButton->setText("▷▷");  // Double triangle represents "Next"
    nextButton->setStyleSheet(
        "QPushButton {"
        "  border: none;"
        "  background-color: transparent;"
        "  font-size: 30px;"
        "  font-weight: bold;"
        "  color: black;"
        "}"
    );

    buttonLayout->addWidget(prevButton);
    buttonLayout->addWidget(pauseButton);
    buttonLayout->addWidget(nextButton);

    // Update the pause button style
    pauseButton->setStyleSheet(
        "QPushButton {"
        "font-size: 30px;"              // Set font size
        "font-weight: bold;"            // Set font weight to bold
        "border: none;"                 // Remove border
        "background-color: transparent;"  // Transparent background
        "color: black;"                 // Set text color
        "}"
    );

    // Pause button functionality
    QObject::connect(pauseButton, &QPushButton::clicked, [=]() {
        if (pauseButton->text() == "||") {
            pauseButton->setText("▶");
            player->pauseVideo();
        } else {
            pauseButton->setText("||");
            player->pauseVideo();
        }
    });

    int currentIndex = 0;

    // Previous button functionality
    QObject::connect(prevButton, &QPushButton::clicked, [&]() {
        if (currentIndex > 0) {
            currentIndex--;
        } else {
            currentIndex = videos.size() - 1;  // If it is the first video, jump to the last one
        }
        player->setVideoByIndex(currentIndex, &videos);
    });

    // Next button functionality
    QObject::connect(nextButton, &QPushButton::clicked, [&]() {
        if (currentIndex < videos.size() - 1) {
            currentIndex++;
        } else {
            currentIndex = 0;  // If it is the last video, jump to the first one
        }
        player->setVideoByIndex(currentIndex, &videos);
    });

    // Add video widget and button widget to the main window layout
    topLayout->addWidget(videoWidget);
    topLayout->addWidget(buttonWidget);

    // Show the window
    window.show();

    return app.exec();
}
