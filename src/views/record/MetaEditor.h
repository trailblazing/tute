#ifndef _METAEDITOR_H_
#define _METAEDITOR_H_

#include <memory>
#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QTextCodec>

#include "libraries/wyedit/Editor.h"


extern QMap<Qt::CheckState, QString> _check_state;
extern QMap<QString, Qt::CheckState> _state_check;


class QHBoxLayout;
class QGridLayout;
class QScrollArea;
class QSplitter;
class AttachTableScreen;
class Record;
class FindScreen;


class ClickableLabel: public QLabel {
    Q_OBJECT
public:
    ClickableLabel(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags()): QLabel(parent, f) {}
    ClickableLabel(const QString &text, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags()): QLabel(text, parent, f) {}
Q_SIGNALS:
    void mousePressEvent(QMouseEvent *ev);

};

class MetaEditor: public Editor {
    Q_OBJECT

public slots:

    void setField(QString n, QString v);
    void clearAll(void);
    void onClickToTag(const QString &text);


signals:
    // Сигнал вырабатывается при клике на текстовой метке
    void setFindTextSignal(QString text);

public:
    MetaEditor(QString object_name, FindScreen *_find_screen);
    ~MetaEditor(void);

    void setTreePath(QString path);
    void setPin(QString pin);
    void switch_pin();
    void setName(QString name);
    void setAuthor(QString author);
    void setHome(QString url);
    void setUrl(QString url);
    void setTags(QString tags);

    static void toAttachCallback(void);

    void switchToEditorLayout(void);
    void switchToAttachLayout(void);
    void bind(std::shared_ptr<Record> r);   // {_record = r;}
    std::shared_ptr<Record> record() {return _record;}

private:
    void setupLabels(void);
    void setupUI(void);
    void metaAssembly(void);
    void setupSignals(FindScreen *_find_screen);

    QLabel          *_tree_path;       // Надпись Path (только для мобильного интерфейса)
    QCheckBox       *_record_pin;

    QLabel          *_record_name;     // Надпись Title
    QLabel          *_record_author;   // Надпись Author(s)

    QLabel          *_label_home;       // Inscription    // Надпись "Url"
    ClickableLabel  *_record_home;

    QLabel          *_label_url;       // Inscription    // Надпись "Url"
    ClickableLabel  *_record_url;

    QLabel          *_label_tags;      // Надпись "Tags"
    QWidget         *_record_tags_container;
    QHBoxLayout     *_record_tags_layout;
    QScrollArea     *_record_tags_scrollarea;
    QString         _record_tags_text;
    QStringList     _record_tags_text_list;
    QList<QLabel *> _record_tags_labels;

    QSplitter       *_editor_and_filetable_splitter;

    // Виджет слоя прикрепляемых файлов
    AttachTableScreen   *_attachtable_screen;

    // Виджет слоя редактирования текста
    QGridLayout     *_editor_main_layer;
    QWidget         *_editor_main_screen;

    // Группировалка виджетов всех слоев (слоя редактирования и слоя прикрепляемых файлов)
    QVBoxLayout     *_meta_editor_join_layer;
    QMetaObject::Connection _home_connection;   // for disconnect
    QMetaObject::Connection _url_connection;    // for disconnect
    std::shared_ptr<Record> _record;
};

#endif /* _METAEDITOR_H_ */
