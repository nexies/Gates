#pragma once


#include <QWidget>
#include <QRubberBand>

class Resizable : public QObject {
    Q_OBJECT

public:
    enum Edge {
        None = 0x0,
        Left = 0x1,
        Top = 0x2,
        Right = 0x4,
        Bottom = 0x8,
        TopLeft = 0x10,
        TopRight = 0x20,
        BottomLeft = 0x40,
        BottomRight = 0x80,
    };
    Q_ENUM(Edge);
    Q_DECLARE_FLAGS(Edges, Edge);

    Resizable(QWidget *target);

    void setGripBorderWidth(int w) {
        _gripBorderWidth = w;
    }
    int gripBorderWidth() const {
        return _gripBorderWidth;
    }

protected:
    bool eventFilter(QObject *o, QEvent *e) override;
    void mouseHover(QHoverEvent*);
    void mouseLeave(QEvent*);
    void mousePress(QMouseEvent*);
    void mouseRealese(QMouseEvent*);
    void mouseMove(QMouseEvent*);
    void updateCursorShape(const QPoint &);
    void calculateCursorPosition(const QPoint &, const QRect &, Edges &);

private:
    QWidget *_target = nullptr;
    QRubberBand *_rubberband = nullptr;
    bool _cursorchanged;
    bool _leftButtonPressed;
    Edges _mousePress = Edge::None;
    Edges _mouseMove = Edge::None;
    int _gripBorderWidth;

    QPoint _dragPos;
    bool _dragStart = false;
signals:
    void geometryUpdated();
};

