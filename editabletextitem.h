#ifndef EDITABLETEXTITEM_H
#define EDITABLETEXTITEM_H

#include <QGraphicsTextItem>


class EditableTextItem : public QGraphicsTextItem {
public:
    explicit EditableTextItem(const QString &text, QGraphicsRectItem *parent = nullptr, bool align = false);

protected:
    void focusOutEvent(QFocusEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    // void hover
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

signals:
    // void textEdit();

protected:
    void setAlign();
    bool isEdit;
    bool align;
};


#endif // EDITABLETEXTITEM_H


// #ifndef EDITABLETEXTITEM_H
// #define EDITABLETEXTITEM_H

// #include <QGraphicsTextItem>
// #include <QObject>


// class EditableTextItem : public QObject, public QGraphicsTextItem
// {
//     Q_OBJECT

// public:
//     explicit EditableTextItem(const QString &text, QObject *parent = nullptr);
//     ~EditableTextItem();

// protected:
//     void focusOutEvent(QFocusEvent *event) override;
//     void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
//     void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
//     void keyPressEvent(QKeyEvent *event) override;

// signals:
//     void textEdit();

// private:
//     bool isEdit;
// };


// #endif // EDITABLETEXTITEM_H
