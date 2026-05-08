#include "ResizableFramelessWindow.h"
#include <QDebug>
#include "cpp/utilities/BlurBehindHelper/BlurBehindHelper.h"
#include <QResizeEvent>

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

ResizableFramelessWindow::ResizableFramelessWindow(QWindow *parent) :
    QQuickWindow(parent)
{
    setFlag(Qt::FramelessWindowHint, true);
    setEdgeOffset(5);
    setSizeIncrement(QSize(50, 50));
}

bool ResizableFramelessWindow::blurBehind() const
{
    return _blurBehind;
}

void ResizableFramelessWindow::setBlurBehind(bool newBlurBehind)
{
    if(newBlurBehind)
        _blurBehind = BlurBehindHelper::applyBlur(this);
    else
        _blurBehind = BlurBehindHelper::disableBlur(this);
}

int ResizableFramelessWindow::edgeOffset() const
{
    return _edgeOffset;
}

void ResizableFramelessWindow::setEdgeOffset(int newEdgeOffset)
{
    _edgeOffset = newEdgeOffset;
}

Qt::Edges ResizableFramelessWindow::currentEdges() const
{
    return _currentEdges;
}

void ResizableFramelessWindow::setEdges(QPoint pos)
{
    _currentEdges &= 0;
    if(pos.x() < _edgeOffset) _currentEdges |= Qt::LeftEdge;
    if(pos.x() > (width() - _edgeOffset)) _currentEdges |= Qt::RightEdge;
    if(pos.y() < _edgeOffset) _currentEdges |= Qt::TopEdge;
    if(pos.y() > (height() - _edgeOffset)) _currentEdges |= Qt::BottomEdge;
}

Qt::CursorShape ResizableFramelessWindow::getCursorShape(QPoint pos)
{
    // if(!geometry().contains(pos)) return Qt::ArrowCursor;

    switch(_currentEdges)
    {
    case Qt::LeftEdge | Qt::TopEdge :
    case Qt::RightEdge | Qt::BottomEdge :
        return Qt::SizeFDiagCursor;
    case Qt::LeftEdge | Qt::BottomEdge :
    case Qt::RightEdge | Qt::TopEdge :
        return Qt::SizeBDiagCursor;
    case Qt::TopEdge:
    case Qt::BottomEdge:
        return Qt::SizeVerCursor;
    case Qt::LeftEdge:
    case Qt::RightEdge:
        return Qt::SizeHorCursor;
    default:
        return Qt::ArrowCursor;
    }
}

void ResizableFramelessWindow::mouseMoveEvent(QMouseEvent *ev)
{
    // qDebug() << "[ResizableFramelessWindow][mouseMoveEvent] : registered";

    setEdges(ev->pos());
    setCursor(getCursorShape(ev->pos()));
}

void ResizableFramelessWindow::mousePressEvent(QMouseEvent *ev)
{
    // qDebug() << "[ResizableFramelessWindow][mousePressEvent] : registered";

    setEdges(ev->pos());
    if(geometry().contains(ev->globalPosition().toPoint()) && _currentEdges)
        startSystemResize(_currentEdges);
}

void ResizableFramelessWindow::resizeEvent(QResizeEvent *ev)
{
    if(!m_resizeSnapToGrid)
        return QQuickWindow::resizeEvent(ev);
    if(sizeIncrement() == QSize(0, 0))
        return QQuickWindow::resizeEvent(ev);

    // int newHeight = ev->size().height() < baseSize().height()
    //               ? baseSize().height()
    //               : sizeIncrement().height() == 0
    //               ? ev->size().height()
    //               : ev->size().height() - ((ev->size().height() - baseSize().height()) % sizeIncrement().height() + baseSize().height());

    // int newWidth  = ev->size().width() < baseSize().width()
    //               ? baseSize().width()
    //               : sizeIncrement().width() == 0
    //               ? ev->size().width()
    //               : ev->size().width() - ((ev->size().width() - baseSize().width()) % sizeIncrement().width() + baseSize().width());

    // QSize newSize = QSize(newWidth, newHeight);
    // QSize oldSize = ev->oldSize();
    // delete ev;

    return QQuickWindow::resizeEvent(ev);
}

int ResizableFramelessWindow::minimisedHeight() const
{
    return m_minimisedHeight;
}

void ResizableFramelessWindow::setMinimisedHeight(int newMinimisedHeight)
{
    m_minimisedHeight = newMinimisedHeight;
}

int ResizableFramelessWindow::maximisedHeight() const
{
    return m_maximisedHeight;
}

void ResizableFramelessWindow::setMaximisedHeight(int newMaximisedHeight)
{
    m_maximisedHeight = newMaximisedHeight;
}

bool ResizableFramelessWindow::nativeEvent(const QByteArray & eventType,
                                           void * message, qintptr * result)
{
    if (m_resizeSnapToGrid && eventType == "windows_generic_MSG") {
        const auto * msg = static_cast<MSG *>(message);
        if (msg->message == WM_SIZING) {
            auto * rc = reinterpret_cast<RECT *>(msg->lParam);
            const QSize inc  = sizeIncrement();
            const QSize base = baseSize();

            if (inc.width() > 0 && base.width() > 0) {
                const int w    = rc->right - rc->left;
                const int cols = qMax(1, qRound(float(w - base.width()) / inc.width()));
                const int snapped = base.width() + cols * inc.width();

                const WPARAM edge = msg->wParam;
                if (edge == WMSZ_LEFT || edge == WMSZ_TOPLEFT || edge == WMSZ_BOTTOMLEFT)
                    rc->left = rc->right - snapped;
                else
                    rc->right = rc->left + snapped;
            }

            *result = TRUE;
            return true;
        }
    }
    return QQuickWindow::nativeEvent(eventType, message, result);
}

bool ResizableFramelessWindow::resizeSnapToGrid() const
{
    return m_resizeSnapToGrid;
}

void ResizableFramelessWindow::setResizeSnapToGrid(bool newResizeSnapToGrid)
{
    m_resizeSnapToGrid = newResizeSnapToGrid;
}
