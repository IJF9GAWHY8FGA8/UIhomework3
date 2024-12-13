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

void ThePlayer::updateSliderPosition() {
    if (duration() > 0) {
        int positionValue = position();  // 获取当前播放位置
        int position = (int)((float)positionValue / duration() * 100);  // 计算当前进度为百分比
        progressSlider->setValue(position);  // 更新滑块位置
    }
}

void ThePlayer::seekVideo(int value) {
    if (duration() > 0) {
        int seekPosition = (value / 100.0) * duration();  // 将滑块的值转换为视频的时间位置
        setPosition(seekPosition);  // 设置视频播放位置
    }
}

