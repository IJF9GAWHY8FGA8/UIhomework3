//
//
//

#ifndef CW2_THE_PLAYER_H
#define CW2_THE_PLAYER_H

#include <QMediaPlayer>
#include "the_button.h"
#include <vector>
#include <QTimer>

class ThePlayer : public QMediaPlayer {
    Q_OBJECT

private:
    std::vector<TheButtonInfo>* infos;
    std::vector<TheButton*>* buttons;
    QTimer* mTimer;
    long updateCount = 0;

public:
    ThePlayer(QObject *parent = nullptr);
    ~ThePlayer();

    void setContent(std::vector<TheButton*>* b, std::vector<TheButtonInfo>* i);
    void setPlaybackSpeed(double speed);

private slots:
    void shuffle();
    void playStateChanged(QMediaPlayer::State ms);

public slots:
    void jumpTo(TheButtonInfo* button);
};

#endif // CW2_THE_PLAYER_H
