#ifndef __XMLTREE_H__
#define __XMLTREE_H__



#include <QtXml>
#include <QTreeWidgetItem>


#if QT_VERSION == 0x050600
#include <wobjectdefs.h>
#include <QObject>
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
	bool		load(QString _file_name);
	QDomDocument	*dom_model(void);

    protected:
	QDomDocument *_dom_model;	// DOM-представление документа
};


#endif	// __XMLTREE_H__
