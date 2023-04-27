#include <iostream>

#include <QApplication>
#include <QIODevice>
#include <QTextStream>

#include "abstractframe.h"
#include "frameshandler.h"
#include "gatesframe.h"
#include "icon.h"
#include <QStyle>

#include <QProcess>
#include "contextmenu.h"
#include <QStyleOptionViewItem>
#include <QPainter>
#include <QPaintEngine>

using namespace std;
//using namespace Gates;


class TestButton: public QWidget
{

public:
    TestButton(QWidget * parent = nullptr): QWidget(parent, Qt::Window)
    {
        this->setMouseTracking(true);
        this->setWindowOpacity(0.7);
    }

    void testPaint()
    {
        QPainter paint(this);
        paint.setPen(Qt::blue);
        paint.drawText(rect(), Qt::AlignCenter, "The Text");
    }

    void paint(QPainter *painter,
                                const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const
    {
        QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
        QSize iconsize = icon.actualSize(option.decorationSize);
        QRect item = option.rect;
        QRect iconRect(item.left()+(item.width()/2)-(iconsize.width()/2),
                       item.top()+4+4, iconsize.width(), iconsize.height());
        QRect txtRect(item.left()+4, item.top()+iconsize.height()+4+4+4,
                      item.width()-8, item.height()-iconsize.height()-4);
        QBrush txtBrush = qvariant_cast<QBrush>(index.data(Qt::ForegroundRole));
        bool isSelected = option.state & QStyle::State_Selected;
        bool isEditing = false;//_isEditing && index==_index;

        /*QStyleOptionViewItem opt = option;
            initStyleOption(&opt,index);
            opt.decorationAlignment |= Qt::AlignCenter;
            opt.displayAlignment    |= Qt::AlignCenter;
            opt.decorationPosition   = QStyleOptionViewItem::Top;
            opt.features |= QStyleOptionViewItem::WrapText;
            const QWidget *widget = opt.widget;
            QStyle *style = widget ? widget->style() : QApplication::style();
            style->drawControl(QStyle::CE_ItemViewItem,&opt,painter);*/

        painter->setRenderHint(QPainter::Antialiasing);
        painter->setRenderHint(QPainter::HighQualityAntialiasing);

        if (isSelected && !isEditing) {
            QPainterPath path;
            QRect frame(item.left(),item.top()+4, item.width(), item.height()-4);
            path.addRoundRect(frame, 15, 15);
            //  path.addRect(frame);
            painter->setOpacity(0.7);
            painter->fillPath(path, option.palette.highlight());
            painter->setOpacity(1.0);
        }

        painter->drawPixmap(iconRect, icon.pixmap(iconsize.width(),iconsize.height()));

        if (isEditing) { return; }
        if (isSelected) { painter->setPen(option.palette.highlightedText().color()); }
        else { painter->setPen(txtBrush.color()); }

        painter->drawText(txtRect,
                          Qt::AlignTop|Qt::AlignHCenter|Qt::TextWordWrap|Qt::TextWrapAnywhere,
                          index.data().toString());
    }

public:

};


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextStream qout(stdout);


    Gates::IconItem icon("D:\\Games\\Noita\\noita.exe");
    icon.show();

//    BasicFrame test;

//    test.add_icon("C:\\Users\\Nexie\\Desktop\\Discord.lnk");
//    test.add_icon("C:\\Users\\Nexie\\Desktop\\libs");
//    test.add_icon("C:\\Program Files\\Google\\Chrome\\Application\\chrome.exe");
//    test.show();

//    QDir dir("C:\\Program Files\\Google\\Chrome\\Application\\chrome.exe");
//    QStringList filenames = dir.entryList();

//    for(int i = 0; i < filenames.size(); i++)
//        qout << filenames[i] << endl;

//    Gates::AbstractFrame frame(QString("C:\\Users\\Public\\Desktop\\"));
//    frame.show();

//    IconItem ic(QString("C:\\Users\\Nexie\\Desktop\\"));

//    LaunchWithShell(&ic);

//    IconItem t("C:\\Program Files\\Google\\Chrome\\Application\\chrome.exe");
//    qout << t.fileFullPath();


//    QProcess pr;
//    pr.setProgram("C:\\Program Files\\Google\\Chrome\\Application\\chrome.exe");
//    pr.start();

//    Gates::Frame frame("C:\\Users\\Public\\Desktop", "DeskTop");
//    frame.show();



    return a.exec();
//    return 0;
}


/*
 * Размышления:
 *      1) Стандартная библеотека для отрисовки графического интерфейса: MFC. Отображение иконок, меню.
 *          Использовать бессмысленно, разве что для доступа к пунктам меню и иконкам приложений и ярлыков. (медленне, чем Qt, несовместима с Qt)
 *      2) Программа пишется для Windows, с использованием таких понятий как WinsowsPowerShell, реестр. Кросс-платформинг не подрузамевается, поэтому грешим, джентельмены
 *
 * План:
 *      1) Класс ContexMenu. Доступ к элементам стандартного контекстного меню (реестр), вызов нужных приложений.
 *          Пункты copy, paste, detele и тд - воплощать вручную
 *      2) Класс Icon. Отображение иконки на экране. Взаимодействие с пользователем:
 *              - Запуск приложения (открытие файла в нужной программе). Можно использовать ShellExecute.
 *              - Вызов контекстного меню.
 *              - Выделение, перетаскивание, удаление
 *      3) Класс GatesFrame. Взаимодействие с пользователем:
 *              - Изменение геометрии окна
 *              - Привязка элементов Icon к окну
 *              - Анимации окна
 *      4) Класс GatesMainFrame. Может быть предком GatesFrame. Прозрачное окно, расстянутое на весь экран. Выполняет функцию основного рабочего стола.
 *
 *
 * Идеи:
 *      - Нужен способ контролировать все отображаемые GatesFrame. То есть нужен класс GatesFramesHandler, который бы контролировал все окна (изменял настройки).
 *      - Для возможности выделения нескольких иконок нужно реализовать IconGroup - объект, содержащий указатели на несколько объектов Icon и действия с ними.
 *
*/
