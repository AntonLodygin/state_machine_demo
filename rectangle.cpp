#include "rectangle.h"
#include <QPainter>
#include <QDebug>
#include <QCursor>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsRectItem>
#include <math.h>
#include "grabber.h"
#include <editabletextitem.h>
#include <iostream>


Rectangle::Rectangle(QObject *parent) :
    QObject(parent),
    m_cornerFlags(0)
{
    setAcceptHoverEvents(true);
    setFlags(ItemIsSelectable | ItemSendsGeometryChanges);

    for (int i = 0; i < 8; i++){
        cornerGrabber[i] = new Grabber(this);
    }
    setPositionGrabbers();

    title = new EditableTextItem("title", this, true);
    title->setFont(QFont("Monospace", 18));

    text = new EditableTextItem("code", this);
    text->setFont(QFont("Monospace", 18));
    setPositionText();
}

Rectangle::~Rectangle()
{
    for(int i = 0; i < 8; i++){
        delete cornerGrabber[i];
    }
}

QPointF Rectangle::previousPosition() const
{
    return m_previousPosition;
}

void Rectangle::setPreviousPosition(const QPointF previousPosition)
{
    if (m_previousPosition == previousPosition)
        return;

    m_previousPosition = previousPosition;
    emit previousPositionChanged();
}

void Rectangle::setRect(qreal x, qreal y, qreal w, qreal h)
{
    setRect(QRectF(x, y, w, h));
}

void Rectangle::setRect(const QRectF &rect)
{
    if (rect.width() >= 320 && rect.height() >= 180){
        QGraphicsRectItem::setRect(rect);
        if(brush().gradient() != 0){
            const QGradient * grad = brush().gradient();
            if(grad->type() == QGradient::LinearGradient){
                auto tmpRect = this->rect();
                const QLinearGradient *lGradient = static_cast<const QLinearGradient *>(grad);
                QLinearGradient g = *const_cast<QLinearGradient*>(lGradient);
                g.setStart(tmpRect.left() + tmpRect.width()/2,tmpRect.top());
                g.setFinalStop(tmpRect.left() + tmpRect.width()/2,tmpRect.bottom());
                setBrush(g);
            }
        }
    }
}

void Rectangle::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() & Qt::LeftButton) {
        m_leftMouseButtonPressed = true;
        setPreviousPosition(event->scenePos());
        emit clicked(this);
    }
    QGraphicsItem::mousePressEvent(event);
}

void Rectangle::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF pt = event->pos();
    switch (m_cornerFlags) {
    case Top:
        resizeTop(pt);
        break;
    case Bottom:
        resizeBottom(pt);
        break;
    case Left:
        resizeLeft(pt);
        break;
    case Right:
        resizeRight(pt);
        break;
    case TopLeft:
        resizeTop(pt);
        resizeLeft(pt);
        break;
    case TopRight:
        resizeTop(pt);
        resizeRight(pt);
        break;
    case BottomLeft:
        resizeBottom(pt);
        resizeLeft(pt);
        break;
    case BottomRight:
        resizeBottom(pt);
        resizeRight(pt);
        break;
    default:
        if (m_leftMouseButtonPressed) {
            setCursor(Qt::ClosedHandCursor);
            setFlag(ItemIsMovable);
        }
        break;
    }

    QGraphicsItem::mouseMoveEvent(event);
}

void Rectangle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() & Qt::LeftButton) {
        m_leftMouseButtonPressed = false;
        setFlag(ItemIsMovable, false);
    }
    QGraphicsItem::mouseReleaseEvent(event);
}


void Rectangle::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    setPositionGrabbers();
    showGrabbers();
    QGraphicsItem::hoverEnterEvent(event);
}

void Rectangle::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    m_cornerFlags = 0;
    hideGrabbers();
    setCursor(Qt::ArrowCursor);
    QGraphicsItem::hoverLeaveEvent( event );
}

void Rectangle::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QPointF pt = event->pos();              // The current position of the mouse
    qreal drx = pt.x() - rect().right();    // Distance between the mouse and the right
    qreal dlx = pt.x() - rect().left();     // Distance between the mouse and the left

    qreal dby = pt.y() - rect().top();      // Distance between the mouse and the top
    qreal dty = pt.y() - rect().bottom();   // Distance between the mouse and the bottom

    // If the mouse position is within a radius of 7
    // to a certain side( top, left, bottom or right)
    // we set the Flag in the Corner Flags Register

    m_cornerFlags = 0;
    if( dby < 7 && dby > -7 ) m_cornerFlags |= Top;       // Top side
    if( dty < 7 && dty > -7 ) m_cornerFlags |= Bottom;    // Bottom side
    if( drx < 7 && drx > -7 ) m_cornerFlags |= Right;     // Right side
    if( dlx < 7 && dlx > -7 ) m_cornerFlags |= Left;      // Left side

    switch (m_cornerFlags) {
    case Top:
    case Bottom:
        setCursor(QCursor(Qt::SizeVerCursor));
        break;

    case Left:
    case Right:
        setCursor(QCursor(Qt::SizeHorCursor));
        break;

    case TopRight:
    case BottomLeft:
        setCursor(QCursor(Qt::SizeBDiagCursor));
        break;

    case TopLeft:
    case BottomRight:
        setCursor(QCursor(Qt::SizeFDiagCursor));
        break;

    default:
        setCursor(Qt::OpenHandCursor);
        break;
    }
    QGraphicsItem::hoverMoveEvent(event);
}

void Rectangle::resizeLeft(const QPointF &pt)
{
    QRectF tmpRect = rect();
    // if the mouse is on the right side we return
    if( pt.x() > tmpRect.right() )
        return;
    qreal widthOffset =  ( pt.x() - tmpRect.right() );
    // limit the minimum width
    if( widthOffset > -10 )
        return;
    // if it's negative we set it to a positive width value
    if( widthOffset < 0 )
        tmpRect.setWidth( -widthOffset );
    else
        tmpRect.setWidth( widthOffset );
    // Since it's a left side , the rectange will increase in size
    // but keeps the topLeft as it was
    tmpRect.translate( rect().width() - tmpRect.width() , 0 );
    prepareGeometryChange();
    // Set the ne geometry
    setRect( tmpRect );
    // Update to see the result
    update();
    setPositionGrabbers();
    setPositionText();
}

void Rectangle::resizeRight(const QPointF &pt)
{
    QRectF tmpRect = rect();
    if( pt.x() < tmpRect.left() )
        return;
    qreal widthOffset =  ( pt.x() - tmpRect.left() );
    if( widthOffset < 10 ) /// limit
        return;
    if( widthOffset < 10)
        tmpRect.setWidth( -widthOffset );
    else
        tmpRect.setWidth( widthOffset );
    prepareGeometryChange();
    setRect( tmpRect );
    update();
    setPositionGrabbers();
    setPositionText();
}

void Rectangle::resizeBottom(const QPointF &pt)
{
    QRectF tmpRect = rect();
    if( pt.y() < tmpRect.top() )
        return;
    qreal heightOffset =  ( pt.y() - tmpRect.top() );
    if( heightOffset < 11 ) /// limit
        return;
    if( heightOffset < 0)
        tmpRect.setHeight( -heightOffset );
    else
        tmpRect.setHeight( heightOffset );
    prepareGeometryChange();
    setRect( tmpRect );
    update();
    setPositionGrabbers();
    setPositionText();
}

void Rectangle::resizeTop(const QPointF &pt)
{
    QRectF tmpRect = rect();
    if( pt.y() > tmpRect.bottom() )
        return;
    qreal heightOffset =  ( pt.y() - tmpRect.bottom() );
    if( heightOffset > -11 ) /// limit
        return;
    if( heightOffset < 0)
        tmpRect.setHeight( -heightOffset );
    else
        tmpRect.setHeight( heightOffset );
    tmpRect.translate( 0 , rect().height() - tmpRect.height() );
    prepareGeometryChange();
    setRect( tmpRect );
    update();
    setPositionGrabbers();
    setPositionText();
}

void Rectangle::setPositionText()
{
    std::cout << rect().width() << rect().height() << "\n";
    QRectF oldRect = rect();
    qreal titleHeight = title->boundingRect().height();
    title->setPos(oldRect.x() + 10, oldRect.y());
    text->setPos(oldRect.x() + 10, oldRect.y() + titleHeight);
    qreal textHeight = text->boundingRect().height();

    setRect(QRectF(oldRect.x(), oldRect.y(), oldRect.width(), titleHeight + textHeight));
    setPositionGrabbers();
}

void Rectangle::setPositionGrabbers()
{
    QRectF tmpRect = rect();
    cornerGrabber[GrabberTop]->setPos(tmpRect.left() + tmpRect.width()/2, tmpRect.top());
    cornerGrabber[GrabberBottom]->setPos(tmpRect.left() + tmpRect.width()/2, tmpRect.bottom());
    cornerGrabber[GrabberLeft]->setPos(tmpRect.left(), tmpRect.top() + tmpRect.height()/2);
    cornerGrabber[GrabberRight]->setPos(tmpRect.right(), tmpRect.top() + tmpRect.height()/2);
    cornerGrabber[GrabberTopLeft]->setPos(tmpRect.topLeft().x(), tmpRect.topLeft().y());
    cornerGrabber[GrabberTopRight]->setPos(tmpRect.topRight().x(), tmpRect.topRight().y());
    cornerGrabber[GrabberBottomLeft]->setPos(tmpRect.bottomLeft().x(), tmpRect.bottomLeft().y());
    cornerGrabber[GrabberBottomRight]->setPos(tmpRect.bottomRight().x(), tmpRect.bottomRight().y());
}

void Rectangle::showGrabbers()
{
    for(int i = 0; i < 8; i++){
        cornerGrabber[i]->setVisible(true);
    }
}

void Rectangle::hideGrabbers()
{
    for(int i = 0; i < 8; i++){
        cornerGrabber[i]->setVisible(false);
    }
}

void Rectangle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    QPainterPath path;
    path.addRoundedRect(rect(), 15, 15);
    QRectF tmpRect = rect();
    painter->drawLine(QPointF(tmpRect.x(), tmpRect.y() + 37), QPointF(tmpRect.right(), tmpRect.y() + 37)); //37 - boudingRect() шрифта размером 18

    painter->drawPath(path);
}
