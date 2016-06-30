#ifndef _CONFIGPAGE_H_
#define _CONFIGPAGE_H_

#include <wobjectdefs.h>
#include <QObject>

#include <QWidget>


class ConfigPage : public QWidget {
    W_OBJECT(ConfigPage)

    public:
	ConfigPage(QWidget *parent = 0);
	virtual ~ConfigPage();

	virtual int apply_changes(void);
};


#endif	// _CONFIGPAGE_H_
