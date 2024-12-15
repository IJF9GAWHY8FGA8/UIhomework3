#ifndef COMMENT_MANAGER_H
#define COMMENT_MANAGER_H

#include "Comment.h"
#include <QList>

class CommentManager {
public:
    void addComment(const Comment &comment);
    QList<Comment> getComments() const;

private:
    QList<Comment> comments;
};

#endif // COMMENT_MANAGER_H
