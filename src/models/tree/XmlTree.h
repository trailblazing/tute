#ifndef __XMLTREE_H__
#define __XMLTREE_H__

#include <QObject>
#include <QtXml>
#include <QTreeWidgetItem>

class DiskHelper;
class TrashMonitoring;

class XmlTree : public QObject {
    Q_OBJECT

public:
    XmlTree(void);
    ~XmlTree(void);
    bool load(QString _file_name);
    QDomDocument* dom_model(void);

protected:
    QDomDocument *_dom_model; // DOM-представление документа
};


#endif // __XMLTREE_H__
