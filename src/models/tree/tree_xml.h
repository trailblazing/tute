#ifndef __XMLTREE_H__
#define __XMLTREE_H__

#include <QTreeWidgetItem>
#include <QtXml>

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

class DiskHelper;
class TrashMonitoring;

class XmlTree : public QObject {
#if QT_VERSION == 0x050600
	W_OBJECT(XmlTree)
#else
	Q_OBJECT
#endif

	public:
	XmlTree(void);
	~XmlTree(void);
	bool load(QString _file_path);
	QDomDocument *dom_model(void);

	protected:
	QDomDocument *_dom_model;  // DOM-представление документа
};

#endif  // __XMLTREE_H__
