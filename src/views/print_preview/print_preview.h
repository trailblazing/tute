#ifndef _PRINTPREVIEW_H_
#define _PRINTPREVIEW_H_

#include <wobjectdefs.h>
#include <QObject>

#include <QMainWindow>
#include <QPrinter>
#include <QVBoxLayout>
#include <QDialog>

class PreviewView;
class QTextDocument;
class FlatToolButton;

class PrintPreview : public QDialog {
    W_OBJECT(PrintPreview)
    public:
	PrintPreview(const QTextDocument *document, QWidget *parent);
	virtual ~PrintPreview();

    private slots:
	void	print();
	void	page_setup();

    private:
	void setup();

	QTextDocument	*doc;
	PreviewView	*view;
	QPrinter	printer;

	QVBoxLayout *centralLayout;
};

#endif	// _PRINTPREVIEW_H_