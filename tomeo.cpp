/*
 *
 *    ______
 *   /_  __/___  ____ ___  ___  ____
 *    / / / __ \/ __ __ \/ _ \/ __ \
 *   / / / /_/ / / / / / /  __/ /_/ /
 *  /_/  \____/_/ /_/ /_/\___/\____/
 *              video for sports enthusiasts...
 *
 *  2811 cw3 : twak
 */
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
#include <QString>
#include <QTime>
#include <QTextEdit>
#include <QScrollArea>
#include <QScrollBar>
#include <QSplitter>
#include <QSlider>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QDir>
#include <QListWidget>
#include "the_player.h"
#include "the_button.h"
#include "the_slider.h"

void updateTimeAndProgress(ThePlayer* player, VideoSlider* progressSlider, QLabel* timeLabel);
void setupCommentSection(QWidget* parent, QVBoxLayout* mainLayout);
// Get video and thumbnail information
std::vector<TheButtonInfo> getInfoIn(std::string loc) {
    std::vector<TheButtonInfo> out;
    QDir dir(QString::fromStdString(loc));
    QDirIterator it(dir);

    while (it.hasNext()) {
        QString f = it.next();
        qDebug() << "Found file: " << f;

        // 处理视频文件
        if (f.contains(".")) {
#if defined(_WIN32)
            // 如果是视频文件（.wmv）
            if (f.contains(".wmv")) {
#else
            // 处理 MP4 和 MOV 文件
            if (f.contains(".mp4") || f.contains("MOV")) {
#endif
                QString thumb = f.left(f.length() - 4) + ".png"; // 假设视频缩略图为 .png 格式
                if (QFile(thumb).exists()) {
                    QImageReader *imageReader = new QImageReader(thumb);
                    QImage sprite = imageReader->read(); // 读取缩略图
                    if (!sprite.isNull()) {
                        QIcon* ico = new QIcon(QPixmap::fromImage(sprite)); // 创建按钮图标
                        QUrl* url = new QUrl(QUrl::fromLocalFile(f)); // 转换文件路径为 QUrl
                        out.push_back(TheButtonInfo(url, ico)); // 添加到输出列表
                    } else {
                        qDebug() << "Warning: Skipping video due to failed thumbnail processing " << thumb;
                    }
                } else {
                    qDebug() << "Warning: Skipping video due to missing thumbnail " << thumb;
                }
            }
        }

        // 处理图片文件
        else if (f.contains(".png")) {
            QImageReader *imageReader = new QImageReader(f);
            QImage sprite = imageReader->read(); // 读取图片
            if (!sprite.isNull()) {
                QIcon* ico = new QIcon(QPixmap::fromImage(sprite)); // 创建按钮图标
                QUrl* url = new QUrl(QUrl::fromLocalFile(f)); // 转换文件路径为 QUrl
                out.push_back(TheButtonInfo(url, ico)); // 添加到输出列表
            } else {
                qDebug() << "Warning: Skipping image due to failed loading " << f;
            }
        }
    }
    return out;
}

// Get the video directory path
QString getVideoPath() {
    // 显示一个消息框，询问用户是否选择固定路径
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(nullptr, "Choose Path", "Do you want to use a fixed path?",
                                  QMessageBox::Yes | QMessageBox::No);

    // 如果用户选择了固定路径
    if (reply == QMessageBox::Yes) {
        QString currentDir = QDir::currentPath();
        qDebug() << "Current working directory: " << currentDir;

        QDir dir(currentDir);
        if (!dir.cdUp()) {
            qWarning() << "Failed to move up one level in directory. Current directory: " << currentDir;
            return "";  // 如果无法向上移动目录，返回空字符串
        }

        // 切换到视频存储目录
        if (!dir.cd("2811_cw3-master-release-lowres/videos")) {
            qWarning() << "Failed to change directory to 'videos' in: " << dir.absolutePath();
            return "";  // 如果找不到该目录，返回空字符串
        }

        return dir.absolutePath();  // 返回绝对路径
    }
    // 如果用户选择了自定义路径
    else {
        QString dirPath = QFileDialog::getExistingDirectory(nullptr, "Select Video Folder", QDir::currentPath());
        if (!dirPath.isEmpty()) {
            qDebug() << "User selected directory: " << dirPath;
            return dirPath;  // 返回用户选择的路径
        } else {
            qWarning() << "No folder selected.";
            return "";  // 用户没有选择任何文件夹
        }
    }
}

// Define setupUI function outside of main()
void setupUI(QWidget* parent, ThePlayer* player, QVBoxLayout* mainLayout) {
    // 创建并添加自定义视频控件
    CustomVideoWidget* videoWidget = new CustomVideoWidget;
    videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(videoWidget, 4);

    // 创建视频进度条（替换 QSlider）
    VideoSlider* progressSlider = new VideoSlider(parent);
    progressSlider->setRange(0, 100);
    progressSlider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // 创建时间显示标签
    QLabel* timeLabel = new QLabel("00:00:00 / 00:00:00", parent);
    timeLabel->setAlignment(Qt::AlignCenter);

    // 创建水平布局
    QHBoxLayout* timeLayout = new QHBoxLayout;
    timeLayout->setContentsMargins(0, 0, 0, 0);
    timeLayout->setSpacing(10);
    timeLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    timeLabel->adjustSize();
    timeLayout->addWidget(progressSlider, 1);
    timeLayout->addWidget(timeLabel, 0);
    mainLayout->addLayout(timeLayout, 1);

    // 将进度条连接到播放器
    player->setProgressSlider(progressSlider);

    // 创建定时器更新进度
    QTimer* timer = new QTimer(parent);
    QObject::connect(timer, &QTimer::timeout, [=]() {
        updateTimeAndProgress(player, progressSlider, timeLabel);
    });
    timer->start(200);  // 更新间隔改为200ms

    // 连接进度条移动与视频位置
    QObject::connect(progressSlider, &VideoSlider::sliderMoved, [=](int value) {
        int newPosition = (value / 100.0) * player->duration();
        player->setPosition(newPosition);
    });

    // 设置视频播放器输出
    player->setVideoOutput(videoWidget);
}

// 提取更新时间标签和进度条更新的函数
void updateTimeAndProgress(ThePlayer* player, VideoSlider* progressSlider, QLabel* timeLabel) {
    if (player->duration() > 0) {
        int position = (int)((float)player->position() / player->duration() * 100);
        progressSlider->setValue(position);

        int currentTime = player->position() / 1000;
        int totalTime = player->duration() / 1000;
        int currentHours = currentTime / 3600;
        int currentMinutes = (currentTime % 3600) / 60;
        int currentSeconds = currentTime % 60;
        int totalHours = totalTime / 3600;
        int totalMinutes = (totalTime % 3600) / 60;
        int totalSeconds = totalTime % 60;

        QString timeString = QString("%1:%2:%3 / %4:%5:%6")
            .arg(currentHours, 2, 10, QChar('0'))
            .arg(currentMinutes, 2, 10, QChar('0'))
            .arg(currentSeconds, 2, 10, QChar('0'))
            .arg(totalHours, 2, 10, QChar('0'))
            .arg(totalMinutes, 2, 10, QChar('0'))
            .arg(totalSeconds, 2, 10, QChar('0'));
        timeLabel->setText(timeString);
        timeLabel->adjustSize();
    }
}

// Function to setup control buttons: pause, previous, and next
void setupControls(QVBoxLayout* mainLayout, [[maybe_unused]] QWidget* parent, ThePlayer* player, std::vector<TheButtonInfo>& videos, int& currentIndex) {
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
    QSize minSize(25, 25);  // 设置按钮最小尺寸为 25x25
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
    volumeSlider->setValue(0);  // 默认音量 0
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
        if (static_cast<size_t>(currentIndex) < videos.size() - 1) {
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
        int newPos = currentPos + 5000;  // 快进5秒 (单位：毫秒)
        if (newPos > player->duration()) {
            newPos = player->duration();  // 如果超过视频时长，设置为最大时长
        }
        player->setPosition(newPos);  // 设置新的播放位置
    });

    // 快退按钮功能
    QObject::connect(rewindButton, &QPushButton::clicked, [&]() {
        int currentPos = player->position();  // 获取当前播放位置
        int newPos = currentPos - 5000;  // 快退5秒 (单位：毫秒)
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

    // 设置按钮和倍速选择器、音量滑块居中
    controlLayout->setAlignment(Qt::AlignCenter);

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
}

void setupPlaylistAndCommentSection(QWidget* window, QVBoxLayout* mainLayout,
                                    ThePlayer* player, const std::vector<TheButtonInfo>& videos) {
    // 创建播放列表
    QListWidget* playlistWidget = new QListWidget();
    playlistWidget->setFlow(QListWidget::LeftToRight);  // 让播放列表横向排列
    playlistWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);  // 始终显示水平滚动条
    playlistWidget->setIconSize(QSize(200, 200));  // 设置缩略图大小

    // 设置每一项的固定大小
    for (const auto& video : videos) {
        QFileInfo videoInfo(video.url->toLocalFile());

        // 创建列表项，设置为空文本
        QListWidgetItem* item = new QListWidgetItem();

        // 增大缩略图，设置为 200x140 大小
        QPixmap pixmap = video.icon->pixmap(200, 140);  // 设置为 200x140 或其他大小
        item->setIcon(QIcon(pixmap));  // 设置缩略图为图标

        // 不设置文本，这样列表项只显示图标
        item->setText("");  // 清空文本内容

        playlistWidget->addItem(item);  // 添加到播放列表
    }

    // 设置每一项的宽度
    for (int i = 0; i < playlistWidget->count(); ++i) {
        QListWidgetItem* item = playlistWidget->item(i);
        item->setSizeHint(QSize(200, 140));  // 设置每一项的大小
    }

    // 默认隐藏播放列表
    playlistWidget->setVisible(false);

    // 创建按钮控制播放列表的显示和隐藏
    QPushButton* togglePlaylistButton = new QPushButton("Toggle Playlist");
    togglePlaylistButton->setFixedSize(250, 30);  // 设置按钮大小为 250x30
    QObject::connect(togglePlaylistButton, &QPushButton::clicked, [=]() {
        bool isVisible = playlistWidget->isVisible();
        playlistWidget->setVisible(!isVisible);  // 切换播放列表的显示状态
    });

    // 创建评论区按钮
    QPushButton* commentButton = new QPushButton("Show Comments");
    commentButton->setFixedSize(250, 30);  // 设置评论按钮大小为 250x30

    // 创建评论区域（QScrollArea）
    QWidget* commentWidget = new QWidget(window);
    QVBoxLayout* commentLayout = new QVBoxLayout(commentWidget);

    // 创建文本框，供用户输入或查看评论
    QTextEdit* commentTextEdit = new QTextEdit(commentWidget);
    commentTextEdit->setPlaceholderText("Enter your comment...");
    commentTextEdit->setFixedHeight(100);  // 初始高度设置为100

    // 创建一个布局用于显示提交的评论
    QVBoxLayout* displayCommentLayout = new QVBoxLayout();

    // 创建提示标签，用于在没有评论时显示
    QLabel* noCommentLabel = new QLabel("No comments yet. Be the first to comment!");
    noCommentLabel->setAlignment(Qt::AlignCenter);
    noCommentLabel->setStyleSheet("color: gray; font-style: italic;");

    // 将提示标签添加到评论区域
    commentLayout->addWidget(noCommentLabel);

    // 将文本框和状态标签添加到评论区域
    commentLayout->addWidget(commentTextEdit);

    // 创建提交评论按钮
    QPushButton* submitButton = new QPushButton("Submit Comment");
    submitButton->setFixedSize(250, 30);  // 设置提交按钮大小为 250x30
    commentLayout->addWidget(submitButton);  // 添加提交按钮

    commentLayout->addLayout(displayCommentLayout); // 添加显示评论的布局

    // 创建滚动区域，将评论区域添加到 QScrollArea
    QScrollArea* commentScrollArea = new QScrollArea(window);
    commentScrollArea->setWidget(commentWidget);
    commentScrollArea->setWidgetResizable(true);
    commentScrollArea->setFixedHeight(window->height() * 0.4);  // 设置最大高度为 40% 的窗口高度

    // 隐藏评论区域
    commentScrollArea->setVisible(false);

    // 连接评论按钮的点击信号，切换评论区域的显示与隐藏
    QObject::connect(commentButton, &QPushButton::clicked, [=]() {
        if (commentScrollArea->isVisible()) {
            commentScrollArea->setVisible(false);  // 隐藏评论区域
            commentButton->setText("Show Comments");  // 更改按钮文本
        } else {
            commentScrollArea->setVisible(true);  // 显示评论区域
            commentButton->setText("Hide Comments");  // 更改按钮文本
        }
    });

    // 创建一个水平布局容器来放置按钮
    QHBoxLayout* buttonLayoutRow = new QHBoxLayout();
    buttonLayoutRow->addWidget(togglePlaylistButton);  // 播放列表按钮
    buttonLayoutRow->addWidget(commentButton);  // 评论区按钮
    buttonLayoutRow->addStretch();  // 留出空间，使按钮靠右

    // 添加顶部按钮布局到主布局
    mainLayout->addLayout(buttonLayoutRow);

    // 添加评论区滚动区域到布局
    mainLayout->addWidget(commentScrollArea);

    // 添加播放列表到布局
    mainLayout->addWidget(playlistWidget);

    // 当点击播放列表中的视频时，切换视频
    QObject::connect(playlistWidget, &QListWidget::itemClicked, [=](QListWidgetItem* item) {
        std::size_t index = playlistWidget->row(item);
            if (index < videos.size()) {
                player->setVideoByIndex(index, &videos);
            }
        });

    // 连接提交评论按钮的点击信号，处理评论提交
    QObject::connect(submitButton, &QPushButton::clicked, [=]() {
        QString comment = commentTextEdit->toPlainText();
        if (comment.isEmpty()) {
            // 如果评论为空，提醒用户输入内容
            commentTextEdit->setPlaceholderText("Please enter a comment before submitting.");
        } else {
            // 创建一个新的 QLabel 来显示提交的评论
            QString commentText = "YOU: " + comment;
            QLabel* newCommentLabel = new QLabel(commentText);
            newCommentLabel->setWordWrap(true);  // 自动换行显示评论
            newCommentLabel->setStyleSheet("border-bottom: 1px solid #ccc; padding: 5px 0;");  // 分界线样式

            displayCommentLayout->addWidget(newCommentLabel);  // 将新的评论添加到布局中

            // 隐藏提示标签（没有评论时）
            noCommentLabel->setVisible(false);

            // 清空文本框并恢复提示
            commentTextEdit->clear();
            commentTextEdit->setPlaceholderText("Enter your comment...");
        }
    });
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 获取视频文件和缩略图信息
    std::vector<TheButtonInfo> videos = getInfoIn(getVideoPath().toStdString());
    if (videos.empty()) {
        QMessageBox::warning(nullptr, "No Videos", "No videos found in the './videos' folder!");
        return -1;
    }

    // 创建视频控件
    QVideoWidget *videoWidget = new QVideoWidget;
    ThePlayer *player = new ThePlayer;
    player->setVideoOutput(videoWidget);  // 将播放器连接到视频控件

    // 创建主窗口
    QWidget window;
    QVBoxLayout *mainLayout = new QVBoxLayout(&window);
    window.setWindowTitle("Tomeo Video Player");
    window.setMinimumSize(800, 600);

    // 设置UI组件
    setupUI(&window, player, mainLayout);

    // 添加视频控件到布局中
    mainLayout->addWidget(videoWidget);

    // 设置按钮布局
    std::vector<TheButton*> buttons;
    for (std::size_t i = 0; i < 4 && i < videos.size(); i++) {
        TheButton *button = new TheButton(&window);
        button->connect(button, SIGNAL(jumpTo(TheButtonInfo*)), player, SLOT(jumpTo(TheButtonInfo*)));
        button->init(&videos.at(i));
        buttons.push_back(button);
        button->setVisible(false);
    }
    player->setContent(&buttons, &videos);

    int currentIndex = 0;
    setupPlaylistAndCommentSection(&window, mainLayout, player, videos);

    // 设置控制按钮
    setupControls(mainLayout, nullptr, player, videos, currentIndex);

    // 显示窗口
    window.show();

    return app.exec();  // 启动事件循环
}







