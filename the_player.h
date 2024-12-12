#ifndef CW2_THE_PLAYER_H
#define CW2_THE_PLAYER_H

#include <QApplication>
#include <QMediaPlayer>
#include "the_button.h"
#include <vector>
#include <QTimer>
#include <QPushButton>

class ThePlayer : public QMediaPlayer {
    Q_OBJECT

private:
    std::vector<TheButtonInfo>* infos;  // A pointer to a vector of button info
    std::vector<TheButton*>* buttons;  // A pointer to a vector of buttons
    QTimer* mTimer;  // Timer for periodic updates
    long updateCount = 0;  // A counter to track updates
    int currentIndex = 0;  // Index to track the current video

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
};

#endif // CW2_THE_PLAYER_H
