#ifndef __RECORDTABLEDATA_H__
#define __RECORDTABLEDATA_H__

#include <QAbstractListModel>
#include <QModelIndex>
#include <QVariant>
#include <QObject>
#include <QtXml>
#include <QMap>
#include <QString>
#include <QByteArray>
#include <memory>

class TreeItem;
class Record;
class MetaEditor;

class RecordTableData {

public:
    RecordTableData(void);
    virtual ~RecordTableData();

    // Получение текста указанной записи
    QString getText(int pos);

    // Установка текста и картинок указанной записи
    void setTextAndPictures(int pos,
                            const QString &text,
                            const QMap<QString, QByteArray> &picturesFiles = (QMap<QString, QByteArray>()));

    // Получение значения указанного поля для указанного элемента
    QString getField(QString name, int pos) const;

    // Установка значения указанного поля для указанного элемента
    void setField(QString name, QString value, int pos);

    // Получение образа записи
    std::shared_ptr<Record> getRecordLite(int pos);
    std::shared_ptr<Record> getRecordFat(int pos);
    std::shared_ptr<Record> getRecord(int pos);
    std::shared_ptr<Record> find(const QUrl &url);
    std::shared_ptr<Record> find(const std::shared_ptr<Record> r);

    // Первичное заполнение таблицы конечных записей
    void init(TreeItem *item, QDomElement domModel);

    // Удаление всех элементов таблицы конечных записей
    void deleteAllRecords(void);

    // Количество записей в таблице данных
    int size(void) const;

    // Функция создания DOM-документа из данных таблицы конечных записей
    QDomElement exportDataToDom(QDomDocument *doc) const;

    // Получение ссылки на объект ветки, которой принадлежит таблица
    TreeItem *getItem(void);

    int insertNewRecord(int mode
                        , int pos
                        , std::shared_ptr<Record> record);

    void editRecordFields(int pos
                          , QMap<QString, QString> editFields);

    void deleteRecord(int i);
    void deleteRecordById(QString id);

    bool isRecordExists(const QString &id);
    bool isRecordExists(const QUrl &url);

    int getPosById(QString id);

    void empty(void);

    void moveUp(int pos);
    void moveDn(int pos);

    void switchToEncrypt(void);
    void switchToDecrypt(void);

    static void editorLoadCallback(QObject *editor,
                                   QString &loadText);

    static void editorSaveCallback(QObject *editor,
                                   QString saveText);

    int  getWorkPos(void);
    void setWorkPos(int pos);

    void checkAndCreateTextFile(int pos, QString fullFileName);


private:

    // Функция заполнения таблицы из DOM-документа
    // Function to populate a table of DOM-document
    void setupDataFromDom(QDomElement *domModel);

    // Таблица записей (в нормальном виде содержит только "легкие" объекты записей)
    // Table entries (in the normal form contains only "light" objects records)
    QList< std::shared_ptr<Record> > tableData;

    // Ссылка на ветку, которой принадлежит данная таблица
    // Link to the branch that owns the table
    TreeItem *treeItem;

    // Номер записи, с которой работал пользователь
    // Number of entries with which the user works
    int workPos;
    friend class MetaEditor;
};


#endif /* __RECORDTABLEDATA_H__ */
