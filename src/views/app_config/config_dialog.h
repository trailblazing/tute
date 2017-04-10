#ifndef _CONFIGDIALOG_H_
#define _CONFIGDIALOG_H_

#include <QDialog>

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

class QDialogButtonBox;
class QListWidget;
class QListWidgetItem;
class QStackedWidget;
class QListWidgetItem;
class QScrollArea;
class AppConfigDialog;

class ConfigDialog : public QDialog {
#if QT_VERSION == 0x050600
	W_OBJECT(ConfigDialog)
#else
	Q_OBJECT
#endif

	public:
	ConfigDialog(QWidget *parent);
	QListWidgetItem *add_widget(QWidget *inswidget, QString name);
	void set_window_title(QString title);

	void updateListWidth(void);
	void externalChangePage(QListWidgetItem *item);

	private slots:
	void apply_changes(void);
	void change_page(QListWidgetItem *current, QListWidgetItem *previous);

	private:
	void setup_ui(void);
	void setup_signals(void);
	void assembly(void);

	QListWidgetItem *create_items(QString name);

	QListWidget *_contentsWidget;  // QScrollArea *scrollArea;
	QStackedWidget *_pagesWidget;
	QDialogButtonBox *_confirmButtons;
};

#endif  // _CONFIGDIALOG_H_
