#ifndef THE_BUTTON_H
#define THE_BUTTON_H

#include <QPushButton>
#include <QUrl>
#include <QIcon>

class TheButtonInfo {
public:
    QUrl* url;  //  URL
    QIcon* icon; // icon
    TheButtonInfo(QUrl* u, QIcon* i) : url(u), icon(i) {}
};

class TheButton : public QPushButton {
    Q_OBJECT

public:
    TheButtonInfo* info;

    TheButton(QWidget* parent) : QPushButton(parent) {
        setIconSize(QSize(200, 110));
        connect(this, SIGNAL(released()), this, SLOT(clicked()));
    }

    void init(TheButtonInfo* i);

private slots:
    void clicked();

signals:
    void jumpTo(TheButtonInfo*);
};

#endif // THE_BUTTON_H
