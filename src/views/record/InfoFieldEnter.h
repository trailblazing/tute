#ifndef INFOFIELDENTER_H_
#define INFOFIELDENTER_H_

#include <QDialog>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QGridLayout>

#include <QCheckBox>


class FlatToolButton;


// Виджет ввода инфополей Title, Author, Url, Tags...

//namespace browser {

//    class TabBar;
//}

extern QMap<Qt::CheckState, QString> _string_from_check_state;
extern QMap<QString, Qt::CheckState> _state_check_from_string;

class InfoFieldEnter : public QWidget {
    Q_OBJECT

public:
    InfoFieldEnter(QWidget *parent = 0);
    ~InfoFieldEnter();

    void setFocusToStart(void);

    bool checkFieldName(QString name);
    QString getField(QString name);
    void setField(QString name, QString value);

    QCheckBox *tabbar() {return _recordpin;}

public slots:

    void expandInfoClick(void);

private:
    // current key for work
    QLabel    *_recordpinlabel;
    //    browser::TabBar *_tabbar;
    QCheckBox *_recordpin;

    // Ввод названия записи
    QLabel    *recordNameLabel;
    QLineEdit *recordName;

    // Ввод автора
    QLabel    *recordAuthorLabel;
    QLineEdit *recordAuthor;

    // home Url
    QLabel    *recordHomeLabel;
    QLineEdit *recordHome;


    // Ввод Url
    QLabel    *recordUrlLabel;
    QLineEdit *recordUrl;

    // Ввод текстовых меток
    QLabel    *recordTagsLabel;
    QLineEdit *recordTags;

    // Кнопка, раскрывающая и скрывающая поля author, url, tags
    FlatToolButton *expandInfo;

    // Размещалка элементов
    QGridLayout *infoFieldLayout;

    void setup_ui(void);
    void setup_signals(void);
    void assembly(void);

    void expandInfoOnDisplay(QString expand);

};

#endif /* INFOFIELDENTER_H_ */
