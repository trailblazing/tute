#ifndef _INDENTSLIDER_H_
#define _INDENTSLIDER_H_

#include <QMouseEvent>
#include <QWidget>

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

class IndentSlider : public QWidget {
#if QT_VERSION == 0x050600
    W_OBJECT(IndentSlider)
#else
    Q_OBJECT
#endif

public:
    IndentSlider(int width, int height, QWidget* parent = 0);

    // Управление размерами виджета
    void set_widget_geometry(int width, int height);
    void set_widget_width(int width);
    void set_widget_height(int height);

    // Управление размерами прямоугольника, по которому двигаются движки
    void set_indentline_left_pos(int i);
    void set_indentline_right_pos(int i);
    void set_indentline_height(int i);

    // Получение текущих отступов движков в абсолютных координатах виджета
    int get_geometry_textindent_pos(void);
    int get_geometry_leftindent_pos(void);
    int get_geometry_rightindent_pos(void);

    // Получение позиции отступа для отрисовки вертикальной линии отступа
    // в абсолютных координатах виджета.
    // Результат зависит от того, какой движок в данный момент выбран
    int get_current_indentedge_pos(void);

signals:
    void change_textindent_pos(int i)
#if QT_VERSION == 0x050600
        W_SIGNAL(change_textindent_pos, (int), i) //
#else
        ;
#endif
        void change_leftindent_pos(int i)
#if QT_VERSION == 0x050600
            W_SIGNAL(change_leftindent_pos, (int), i) //
#else
        ;
#endif
        void change_rightindent_pos(int i)
#if QT_VERSION == 0x050600
            W_SIGNAL(change_rightindent_pos, (int), i) //
#else
        ;
#endif

        void mouse_release(void)
#if QT_VERSION == 0x050600
            W_SIGNAL(mouse_release)
#else
        ;
#endif

                public slots : void set_textindent_pos(int i);
    void set_leftindent_pos(int i);
    void set_rightindent_pos(int i);

protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual void resizeEvent(QResizeEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);

    void draw_textindent(QPainter* painter);
    void draw_leftindent(QPainter* painter);
    void draw_rightindent(QPainter* painter);

    // Размеры виджета
    int widget_width;
    int widget_height;

    // Геометрия прямоугольника по которому двигаются слайдеры
    int indentline_left_pos = 0;
    int indentline_right_pos = 0;
    int indentline_height = 10;

    int indenttriangle_width = 8;
    int indentline_step_draw_marker;
    int indentline_step_marker;

    // Позиции (X-координаты) слайдеров.
    // Позиции указываются относительно прямоугольника
    // по которому двигаются слайдеры
    int textindent_pos = 0;
    int leftindent_pos = 0;
    int rightindent_pos = 0;

    // Какой слайдер выбран в данный момент
    int current_sel_indent = SEL_NONE;

    enum {
        SEL_NONE,
        SEL_TEXT_INDENT,
        SEL_LEFT_INDENT,
        SEL_RIGHT_INDENT
    };
};

#endif // _INDENTSLIDER_H_
