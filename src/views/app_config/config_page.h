#ifndef _CONFIGPAGE_H_
#define _CONFIGPAGE_H_

#include <QWidget>

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

class ConfigPage : public QWidget {
#if QT_VERSION == 0x050600
	W_OBJECT(ConfigPage)
#else
	Q_OBJECT
#endif

	public:
	ConfigPage(QWidget *parent = 0);
	virtual ~ConfigPage();

	virtual int apply_changes(void);
};

#endif  // _CONFIGPAGE_H_
