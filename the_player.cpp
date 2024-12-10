//
//
//

#include "the_player.h"

ThePlayer::ThePlayer(QObject *parent) : QMediaPlayer(parent) {
    setVolume(0); // be slightly less annoying
    connect(this, &QMediaPlayer::stateChanged, this, &ThePlayer::playStateChanged);
    mTimer = new QTimer(this);
    mTimer->setInterval(1000); // 1000ms is one second between ...
    mTimer->start();
    connect(mTimer, &QTimer::timeout, this, &ThePlayer::shuffle);
}

ThePlayer::~ThePlayer() {
    mTimer->stop();
    delete mTimer;
}

void ThePlayer::setContent(std::vector<TheButton*>* b, std::vector<TheButtonInfo>* i) {
    buttons = b;
    infos = i;
    jumpTo(buttons->at(0)->info);
}

void ThePlayer::shuffle() {
    TheButtonInfo* i = &infos->at(rand() % infos->size());
    buttons->at(updateCount++ % buttons->size())->init(i);
}

void ThePlayer::playStateChanged(QMediaPlayer::State ms) {
    switch (ms) {
    case QMediaPlayer::StoppedState:
        play(); // starting playing again...
        break;
    default:
        break;
    }
}

void ThePlayer::jumpTo(TheButtonInfo* button) {
    setMedia(*button->url);
    play();
}

void ThePlayer::setPlaybackSpeed(double speed) {
    setPlaybackRate(speed);
}
