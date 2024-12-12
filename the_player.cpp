#include "the_player.h"

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
        case QMediaPlayer::State::StoppedState:
            play();
            break;
    default:
        break;
    }
}

void ThePlayer::jumpTo(TheButtonInfo* button) {
    setMedia(*button->url);
    play();
}

void ThePlayer::pauseVideo() {
    if (state() == QMediaPlayer::PlayingState) {
        pause();  // Pauses the current playing media
    } else {
        play();  // If not playing, it starts playing
    }
}
