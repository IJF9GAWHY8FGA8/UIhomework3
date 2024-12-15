#ifndef CW2_THE_PLAYER_H
#define CW2_THE_PLAYER_H

#include <QApplication>
#include <QMediaPlayer>
#include "the_button.h"
#include <vector>
#include <QTimer>
#include <QPushButton>
#include <QSlider>
#include <QVideoWidget>
#include <QResizeEvent>

class ThePlayer : public QMediaPlayer {
    Q_OBJECT

private:
    std::vector<TheButtonInfo>* infos;  // A pointer to a vector of button info
    std::vector<TheButton*>* buttons;  // A pointer to a vector of buttons
    QTimer* mTimer;  // Timer for periodic updates
    long updateCount = 0;  // A counter to track updates
    int currentIndex = 0;  // Index to track the current video
    QSlider* progressSlider;

public:
    ThePlayer() : QMediaPlayer(nullptr) {
        setVolume(0);  // Set volume to 0 to avoid loud playback
        connect(this, &QMediaPlayer::stateChanged, this, &ThePlayer::playStateChanged);

        // Set up the timer to update every second
        mTimer = new QTimer(this);
        mTimer->setInterval(1000); // 1000ms equals 1 second
        mTimer->start();
        connect(mTimer, &QTimer::timeout, this, &ThePlayer::shuffle);  // Call shuffle method every second
    }

    // Set the content for the buttons and videos
    void setContent(std::vector<TheButton*>* b, std::vector<TheButtonInfo>* i);
    QSlider* getProgressSlider() const { return progressSlider; }

    void setProgressSlider(QSlider* slider) {
        progressSlider = slider;
    }

    // Method to set the playback speed
    void setPlaybackSpeed(qreal speed) {
        // Set the playback rate for the media player
        setPlaybackRate(speed);
    }

private slots:
    // Shuffle the button icons and video every second
    void shuffle();

    // Call when the media player state changes
    void playStateChanged(QMediaPlayer::State ms);

public slots:
    // Jump to the specified video
    void jumpTo(TheButtonInfo* button);

    // Pause or play the video
    void pauseVideo();

    // Set the video content and switch based on the index
    void setVideoByIndex(int index, const std::vector<TheButtonInfo>* videos) {
        if (index >= 0 && index < videos->size()) {
            // Use const TheButtonInfo* to match the constant reference returned
            const TheButtonInfo* buttonInfo = &videos->at(index);
            setMedia(*buttonInfo->url);  // Set the video file
            play();  // Play the video
        }
    }
    void updateSliderPosition();  // 更新进度条位置
    void seekVideo(int value);    // 根据进度条值调整播放进度
    void updatePlaybackSpeed(int index) {
        qreal speed = 1.0;  // 默认倍速为1x
        switch (index) {
            case 0: speed = 1.0; break;   // 1x
            case 1: speed = 1.25; break;   // 1.25x
            case 2: speed = 1.5; break;   // 1.5x
            case 3: speed = 2.0; break;   // 2x
            default: break;
        }
        setPlaybackSpeed(speed);  // 设置播放器的倍速
    }
};

class CustomVideoWidget : public QVideoWidget {
    Q_OBJECT
public:
    explicit CustomVideoWidget(QWidget *parent = nullptr) : QVideoWidget(parent) {}

protected:
    void resizeEvent(QResizeEvent *event) override {
        QVideoWidget::resizeEvent(event);  // 调用父类的 resizeEvent
        emit resized(width());  // 触发一个自定义信号，传递新宽度
    }

signals:
    void resized(int newWidth);  // 自定义信号，传递新宽度
};

#endif // CW2_THE_PLAYER_H
