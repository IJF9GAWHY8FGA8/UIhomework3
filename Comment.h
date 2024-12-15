#ifndef COMMENT_H
#define COMMENT_H

#include <QString>

class Comment {
public:
    QString text;

    Comment(const QString &content) : text(content) {}
};

#endif // COMMENT_H
