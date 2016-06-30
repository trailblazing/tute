#ifndef __XMLTREE_H__
#define __XMLTREE_H__

#include <wobjectdefs.h>
#include <QObject>

#include <QtXml>
#include <QTreeWidgetItem>

class DiskHelper;
class TrashMonitoring;

class XmlTree : public QObject {
    W_OBJECT(XmlTree)

    public:
	XmlTree(void);
	~XmlTree(void);
	bool		load(QString _file_name);
	QDomDocument	*dom_model(void);

    protected:
	QDomDocument *_dom_model;	// DOM-представление документа
};


#endif	// __XMLTREE_H__
