#pragma once

#include <QQuickWindow>

class ResizableFramelessWindow : public QQuickWindow
{
    Q_OBJECT
    Q_PROPERTY(bool blurBehind READ blurBehind WRITE setBlurBehind)
    Q_PROPERTY(int edgeOffset READ edgeOffset WRITE setEdgeOffset)

    Q_PROPERTY(QSize sizeIncrement READ sizeIncrement WRITE setSizeIncrement)
    Q_PROPERTY(QSize baseSize READ baseSize WRITE setBaseSize)
    Q_PROPERTY(bool  resizeSnapToGrid READ resizeSnapToGrid WRITE setResizeSnapToGrid)

    Q_PROPERTY(Qt::Edges currentEdges READ currentEdges)
    Q_PROPERTY(int minimisedHeight READ minimisedHeight WRITE setMinimisedHeight)
    Q_PROPERTY(int maximisedHeight READ maximisedHeight WRITE setMaximisedHeight)


public:
    explicit ResizableFramelessWindow(QWindow * parent = nullptr);
    bool blurBehind() const;
    void setBlurBehind(bool newBlurBehind);

    int edgeOffset() const;
    void setEdgeOffset(int newEdgeOffset);

    Qt::Edges currentEdges() const;

    int minimisedHeight() const;
    void setMinimisedHeight(int newMinimisedHeight);

    int maximisedHeight() const;
    void setMaximisedHeight(int newMaximisedHeight);

    bool resizeSnapToGrid() const;
    void setResizeSnapToGrid(bool newResizeSnapToGrid);

protected:
    void setEdges (QPoint pos);
    Qt::CursorShape getCursorShape(QPoint pos);

    void mouseMoveEvent(QMouseEvent * ev) override;
    void mousePressEvent(QMouseEvent * ev) override;
    void resizeEvent(QResizeEvent * ev) override;

private:
    bool _blurBehind { false };
    int _edgeOffset { 5 };
    Qt::Edges _currentEdges;
    int m_minimisedHeight { 40 };
    int m_maximisedHeight { 100 };
    bool m_resizeSnapToGrid { false };
};
