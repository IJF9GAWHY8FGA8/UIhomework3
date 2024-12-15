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
#include <QComboBox>
#include <QLabel>
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
    // 创建并添加自定义视频控件
    CustomVideoWidget* videoWidget = new CustomVideoWidget;  // 自定义视频控件
    videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);  // 设置为可扩展，填充父控件
    mainLayout->addWidget(videoWidget, 4);  // 将视频控件添加到布局中，权重为4

    // 创建视频进度条（替换 QSlider）
    VideoSlider* progressSlider = new VideoSlider(parent);
    progressSlider->setRange(0, 100);  // 设置进度条范围为 0-100%
    progressSlider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);  // 宽度可扩展，高度固定
    mainLayout->addWidget(progressSlider, 1);  // 将进度条添加到布局中，权重为1

    // 将进度条连接到播放器
    player->setProgressSlider(progressSlider);

    // 创建一个定时器，每隔100毫秒更新进度
    QTimer* timer = new QTimer(parent);
    QObject::connect(timer, &QTimer::timeout, [=]() {
        if (player->duration() > 0) {
            int position = (int)((float)player->position() / player->duration() * 100);  // 计算进度
            progressSlider->setValue(position);  // 更新进度条的值
        }
    });
    timer->start(100);  // 每100毫秒更新一次

    // 将进度条的移动与视频位置连接
    QObject::connect(progressSlider, &VideoSlider::sliderMoved, [=](int value) {
        int newPosition = (value / 100.0) * player->duration();  // 将进度条的值转换为视频位置
        player->setPosition(newPosition);  // 设置播放器的位置
    });

    // 连接视频控件的大小变化与进度条宽度
    QObject::connect(videoWidget, &CustomVideoWidget::resized, [=](int newWidth) {
        progressSlider->setFixedWidth(newWidth);  // 更新进度条的宽度，确保与视频控件一致
    });

    // 设置视频播放器的输出为自定义视频控件
    player->setVideoOutput(videoWidget);
}

// Function to setup control buttons: pause, previous, and next
void setupControls(QVBoxLayout* mainLayout, QWidget* parent, ThePlayer* player, std::vector<TheButtonInfo>& videos, int& currentIndex) {

    // 创建暂停按钮
    QPushButton* pauseButton = new QPushButton("||");

    // 创建上一个视频按钮
    QPushButton* prevButton = new QPushButton();
    prevButton->setText("◁◁");

    // 创建下一个视频按钮
    QPushButton* nextButton = new QPushButton();
    nextButton->setText("▷▷");

    // 创建快进按钮
    QPushButton* fastForwardButton = new QPushButton("⏩");  // 使用 ⏩ 表示快进

    // 创建快退按钮
    QPushButton* rewindButton = new QPushButton("⏪");  // 使用 ⏪ 表示快退

    // 设置按钮最小和最大尺寸
    QSize minSize(50, 50);  // 设置按钮最小尺寸为 50x50
    QSize maxSize(150, 50); // 设置按钮最大尺寸为 150x50

    // 设置按钮尺寸
    pauseButton->setMinimumSize(minSize);
    pauseButton->setMaximumSize(maxSize);

    prevButton->setMinimumSize(minSize);
    prevButton->setMaximumSize(maxSize);

    nextButton->setMinimumSize(minSize);
    nextButton->setMaximumSize(maxSize);

    fastForwardButton->setMinimumSize(minSize);
    fastForwardButton->setMaximumSize(maxSize);

    rewindButton->setMinimumSize(minSize);
    rewindButton->setMaximumSize(maxSize);

    // 创建倍速选择器
    QComboBox* speedComboBox = new QComboBox();
    speedComboBox->addItem("1x");
    speedComboBox->addItem("1.25x");
    speedComboBox->addItem("1.5x");
    speedComboBox->addItem("2x");
    speedComboBox->setCurrentIndex(0);  // 默认选择 1x

    // 设置倍速选择器的最大尺寸
    speedComboBox->setMinimumSize(minSize);
    speedComboBox->setMaximumSize(maxSize);

    // 创建音量滑块
    QSlider* volumeSlider = new QSlider(Qt::Horizontal);
    volumeSlider->setMinimum(0);  // 最小音量
    volumeSlider->setMaximum(100);  // 最大音量
    volumeSlider->setValue(50);  // 默认音量 50%
    volumeSlider->setTickPosition(QSlider::TicksBelow);
    volumeSlider->setTickInterval(10);

    // 设置音量滑块的最大尺寸
    volumeSlider->setMinimumSize(minSize);
    volumeSlider->setMaximumSize(maxSize);

    // 如果视频列表为空，则返回
    if (videos.empty()) {
        qDebug() << "No videos found!";
        return;
    }

    // 调试：输出视频列表数量
    qDebug() << "Number of videos: " << videos.size();

    // 暂停按钮功能
    QObject::connect(pauseButton, &QPushButton::clicked, [=]() {
        if (pauseButton->text() == "||") {
            pauseButton->setText("▶");
            player->pauseVideo();
        } else {
            pauseButton->setText("||");
            player->pauseVideo();
        }
    });

    // 上一个视频按钮功能
    QObject::connect(prevButton, &QPushButton::clicked, [&]() {
        qDebug() << "Prev button clicked! Current index: " << currentIndex;
        if (currentIndex > 0) {
            currentIndex--;
        } else {
            currentIndex = videos.size() - 1;
        }
        qDebug() << "New index after prev: " << currentIndex;
        player->setVideoByIndex(currentIndex, &videos);
    });

    // 下一个视频按钮功能
    QObject::connect(nextButton, &QPushButton::clicked, [&]() {
        qDebug() << "Next button clicked! Current index: " << currentIndex;
        if (currentIndex < videos.size() - 1) {
            currentIndex++;
        } else {
            currentIndex = 0;
        }
        qDebug() << "New index after next: " << currentIndex;
        player->setVideoByIndex(currentIndex, &videos);
    });

    // 快进按钮功能
    QObject::connect(fastForwardButton, &QPushButton::clicked, [&]() {
        int currentPos = player->position();  // 获取当前播放位置
        int newPos = currentPos + 1000;  // 快进10秒 (单位：毫秒)
        if (newPos > player->duration()) {
            newPos = player->duration();  // 如果超过视频时长，设置为最大时长
        }
        player->setPosition(newPos);  // 设置新的播放位置
    });

    // 快退按钮功能
    QObject::connect(rewindButton, &QPushButton::clicked, [&]() {
        int currentPos = player->position();  // 获取当前播放位置
        int newPos = currentPos - 1000;  // 快退10秒 (单位：毫秒)
        if (newPos < 0) {
            newPos = 0;  // 如果小于0，设置为0
        }
        player->setPosition(newPos);  // 设置新的播放位置
    });

    // 连接倍速选择器
    QObject::connect(speedComboBox, SIGNAL(currentIndexChanged(int)), player, SLOT(updatePlaybackSpeed(int)));

    // 连接音量滑块
    QObject::connect(volumeSlider, &QSlider::valueChanged, [=](int value) {
        player->setVolume(value);  // 调整音量
    });

    // 控制按钮布局
    QHBoxLayout* controlLayout = new QHBoxLayout();
    controlLayout->addWidget(rewindButton);
    controlLayout->addWidget(prevButton);
    controlLayout->addWidget(pauseButton);
    controlLayout->addWidget(nextButton);
    controlLayout->addWidget(fastForwardButton);

    // 将控制按钮布局添加到主布局
    mainLayout->addLayout(controlLayout);

    // 将倍速选择器添加到控制布局
    controlLayout->addWidget(speedComboBox);

    // 将音量滑块添加到控制布局
    controlLayout->addWidget(volumeSlider);

    // 设置按钮和倍速选择器、音量滑块居中
    controlLayout->setAlignment(Qt::AlignCenter);
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

    // Initialize currentIndex to 0 (first video in the list)
    int currentIndex = 0;

    // Set up the control buttons
    setupControls(mainLayout, &window, player, videos, currentIndex);

    // Show the window
    window.show();

    return app.exec();  // Start the event loop
}

