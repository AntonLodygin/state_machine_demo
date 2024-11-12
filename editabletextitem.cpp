#include "editabletextitem.h"
#include <QGraphicsSceneMouseEvent>
#include <QTextCursor>
#include <QFocusEvent>
#include <iostream>
#include <QCursor>
#include <QPainter>
#include <QTextDocument>
#include <QTextBlockFormat>
#include <rectangle.h>


EditableTextItem::EditableTextItem(const QString &text, QGraphicsRectItem *parent, bool align)
    : QGraphicsTextItem(text, parent), align(align) {
    setFlags(QGraphicsItem::ItemIsSelectable);
    setTextInteractionFlags(Qt::NoTextInteraction);
    setAlign();
}

void EditableTextItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton and !hasFocus()) {
        event->accept();
        return;
    }
    QGraphicsTextItem::mousePressEvent(event);
}

void EditableTextItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    setTextInteractionFlags(Qt::TextEditorInteraction);
    setFocus();
    isEdit = true;
    QGraphicsTextItem::mouseDoubleClickEvent(event);
}

void EditableTextItem::keyPressEvent(QKeyEvent *event){
    if (isEdit) {
        Rectangle *parentRectangle = dynamic_cast<Rectangle*>(parentItem());
        QGraphicsTextItem::keyPressEvent(event);
        parentRectangle->setPositionText();
    }
}

void EditableTextItem::focusOutEvent(QFocusEvent *event) {
    setTextInteractionFlags(Qt::NoTextInteraction);
    isEdit = false;
    setPlainText(toPlainText().trimmed());
    QGraphicsTextItem::focusOutEvent(event);
}

void EditableTextItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    if (hasFocus()) {
        setCursor(QCursor(Qt::IBeamCursor));
    } else {
        setCursor(QCursor(Qt::ArrowCursor));
    }

    QGraphicsTextItem::hoverMoveEvent(event);
}

void EditableTextItem::setAlign(){
    QGraphicsRectItem *rectParent = dynamic_cast<QGraphicsRectItem*>(parentItem());
    setTextWidth(rectParent->rect().width() - 20);
    QTextBlockFormat blockFormat;
    if (align) {
        blockFormat.setAlignment(Qt::AlignCenter);
    } else {
        blockFormat.setAlignment(Qt::AlignLeft);
    }
    textCursor().mergeBlockFormat(blockFormat);
}

void EditableTextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    setAlign();
    painter->setFont(QFont(font()));

    QGraphicsTextItem::paint(painter, option, widget);
}










// #include "editabletextitem.h"
// #include <QGraphicsSceneMouseEvent>
// #include <QTextCursor>
// #include <QFocusEvent>
// #include <QObject>


// EditableTextItem::EditableTextItem(const QString &text, QObject *parent)
//     : QObject(parent) {
//     setPlainText(text);
//     setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);
//     setTextInteractionFlags(Qt::NoTextInteraction);
// }

// void EditableTextItem::focusOutEvent(QFocusEvent *event) {
//     setTextInteractionFlags(Qt::NoTextInteraction);
//     isEdit = false;

//     QTextCursor cursor = textCursor();
//     cursor.clearSelection();
//     setTextCursor(cursor);

//     QGraphicsTextItem::focusOutEvent(event);
// }

// void EditableTextItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
//     setTextInteractionFlags(Qt::TextEditorInteraction);
//     setFocus();
//     isEdit = true;
//     QGraphicsTextItem::mouseDoubleClickEvent(event);
// }

// void EditableTextItem::mousePressEvent(QGraphicsSceneMouseEvent *event){
//     if (event->button() == Qt::LeftButton) {
//         event->accept();
//         return;
//     }
//     QGraphicsTextItem::mousePressEvent(event);
// }


// void EditableTextItem::keyPressEvent(QKeyEvent *event){
//     if (isEdit) {
//         emit textEdit();
//         QGraphicsTextItem::keyPressEvent(event);
//     }

// }
