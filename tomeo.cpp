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
#include <QSlider>
#include <QTimer>
#include "the_player.h"
#include "the_button.h"
#include "the_slider.h"

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

// Define setupUI function outside of main()
void setupUI(QWidget* parent, ThePlayer* player, QVBoxLayout* mainLayout) {
    // Create and add the custom video widget
    CustomVideoWidget* videoWidget = new CustomVideoWidget;  // Custom video widget
    mainLayout->addWidget(videoWidget);

    // Create the video progress slider (replace QSlider)
    VideoSlider* progressSlider = new VideoSlider(parent);
    progressSlider->setRange(0, 100);  // Set the range of the progress bar to 0-100%
    progressSlider->setFixedWidth(videoWidget->width());  // Set progress slider width to match video widget
    mainLayout->addWidget(progressSlider);  // Add progress slider below the video widget

    // Connect the progress slider to the player
    player->setProgressSlider(progressSlider);

    // Create a timer to periodically update the progress
    QTimer* timer = new QTimer(parent);
    QObject::connect(timer, &QTimer::timeout, [=]() {
        if (player->duration() > 0) {
            int position = (int)((float)player->position() / player->duration() * 100);  // Calculate progress
            progressSlider->setValue(position);  // Update the progress slider
        }
    });
    timer->start(100);  // Update every 100 ms

    // Connect the progress slider movement to the video position
    QObject::connect(progressSlider, &VideoSlider::sliderMoved, [=](int value) {
        int newPosition = (value / 100.0) * player->duration();  // Convert progress value to video position
        player->setPosition(newPosition);  // Set the player's position
    });

    // Connect the video widget resizing to the progress slider width
    QObject::connect(videoWidget, &CustomVideoWidget::resized, [=](int newWidth) {
        progressSlider->setFixedWidth(newWidth);  // Update the progress slider width
    });
}

// Function to setup control buttons: pause, previous, and next
void setupControls(QVBoxLayout* mainLayout, QWidget* parent, ThePlayer* player, std::vector<TheButtonInfo>& videos) {
    // Create the pause button
    QPushButton* pauseButton = new QPushButton("||");

    // Create the previous button (double triangle)
    QPushButton *prevButton = new QPushButton();
    prevButton->setText("◁◁");  // Double triangle represents "Previous"

    // Create the next button (double triangle)
    QPushButton *nextButton = new QPushButton();
    nextButton->setText("▷▷");  // Double triangle represents "Next"

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

    // Previous button functionality
    int currentIndex = 0;
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

    // Layout for controls
    QHBoxLayout *controlLayout = new QHBoxLayout();
    controlLayout->addWidget(pauseButton);
    controlLayout->addWidget(prevButton);
    controlLayout->addWidget(nextButton);

    // Add control layout to the main layout
    mainLayout->addLayout(controlLayout);
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Get video files and thumbnails
    std::vector<TheButtonInfo> videos = getInfoIn(getVideoPath().toStdString());
    if (videos.empty()) {
        QMessageBox::warning(nullptr, "No Videos", "No videos found in the './videos' folder!");
        return -1;
    }

    // Create the video widget
    QVideoWidget *videoWidget = new QVideoWidget;
    ThePlayer *player = new ThePlayer;
    player->setVideoOutput(videoWidget);  // Connect the player to the video widget

    // Setup the button layout
    QWidget *buttonWidget = new QWidget();
    QVBoxLayout *buttonLayout = new QVBoxLayout(buttonWidget);
    buttonWidget->setLayout(buttonLayout);

    // Create a button for each video and add them to the layout
    std::vector<TheButton*> buttons;
    for (int i = 0; i < 4 && i < videos.size(); i++) {
        TheButton *button = new TheButton(buttonWidget);
        button->connect(button, SIGNAL(jumpTo(TheButtonInfo*)), player, SLOT(jumpTo(TheButtonInfo*)));
        buttonLayout->addWidget(button);
        button->init(&videos.at(i));
        buttons.push_back(button);
    }
    player->setContent(&buttons, &videos);

    // Create the main window
    QWidget window;
    QVBoxLayout *mainLayout = new QVBoxLayout(&window);
    window.setWindowTitle("Tomeo Video Player");
    window.setMinimumSize(800, 600);

    // Set up UI components
    setupUI(&window, player, mainLayout);

    // Add the video widget to the layout
    mainLayout->addWidget(videoWidget);

    // Set up the control buttons
    setupControls(mainLayout, &window, player, videos);

    // Show the window
    window.show();

    return app.exec();  // Start the event loop
}
