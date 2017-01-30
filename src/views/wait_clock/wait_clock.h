#ifndef _WAITCLOCK_H_
#define _WAITCLOCK_H_




#include <QWidget>
#include <QHBoxLayout>
#include <QIcon>
#include <QTimer>
#include <QLabel>
#include <QPixmap>












#if QT_VERSION == 0x050600
#include <wobjectdefs.h>
#include <QObject>
#endif













class WaitClock : public QWidget {
#if QT_VERSION == 0x050600
    W_OBJECT(WaitClock)
#else
    Q_OBJECT
#endif

public:
    WaitClock(QWidget *parent=0);
    ~WaitClock();

private slots:

    void iconUpdate();

private:

    QHBoxLayout *centralLayout;

    QLabel iconLabel;

    QPixmap kadr_00;
    QPixmap kadr_01;
    QPixmap kadr_02;

    int currentIconNum;

    QTimer timer;

};

#endif /* _WAITCLOCK_H_ */
