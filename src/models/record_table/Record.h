#ifndef __RECORD_H__
#define __RECORD_H__

#include <memory>
#include <QString>
#include <QMap>
#include <QByteArray>
#include <QDomElement>

#include "utility/delegate.h"

//#include "views/browser/browser.h"

#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>

// Класс одной записи в таблице записей
// Class entries in the table of records

// В полях записей с crypt=0 хранятся незашифрованные данные
// В полях записей с crypt=1 хранятся зашифрованные данные
// Это необходимо, чтобы дерево знаний генерировалось в/из XML быстро и без шифрации

// In the fields of records crypt = 0 are stored unencrypted data
// In the fields of records crypt = 1 are stored encrypted data
// It is necessary that the tree of knowledge was generated in / from XML quickly and without encryption

// class Attach;
// class AttachTableData;

namespace browser {
    class Browser;
    class WebView;
    class WebPage;
}

class TreeItem;
class AttachTableData;




class Record :
    public boost::intrusive_ref_counter<Record, boost::thread_safe_counter>
//        std::enable_shared_from_this<Record>
{

    // К закрытым функциям может иметь доступ объекты приаттаченного файла
    friend class Attach;
    friend class AttachTableData;

public:
    Record();
    Record(QMap<QString, QString> _field_data);

#ifdef _with_record_table
    Record(boost::intrusive_ptr<Record> obj);
    Record &operator =(const Record &obj);
#endif

    virtual ~Record();

    //    browser::WebPage *unique_page();   // const; // {return _page;}

    void dom_to_record(const QDomElement &_dom_element);
    //    QDomElement export_to_dom(QDomDocument *doc) const;
    QDomElement dom_from_record() const;
    QDomElement dom_from_record(std::shared_ptr<QDomDocument> doc) const;


    QString text_from_fat() const;
    QString text_from_lite_direct() const;
    void text_to_fat(QString _text_source);

    QString field(QString _name) const;
    void field(QString _name, QString value);
    bool is_natural_field_exists(QString name) const;


    // Setting and reading data without transformation. Used to generate / XML readers    // Установка и чтение данных без преобразований. Используется при генерации/чтении XML
    QString natural_field_source(QString name) const;
    void natural_field_source(QString name, QString value);

    QMap<QString, QString> natural_field_list() const;

    QMap<QString, QByteArray> picture_files() const;
    void picture_files(QMap<QString, QByteArray> picture_files);

    std::shared_ptr<AttachTableData> attach_table() const;
    std::shared_ptr<AttachTableData> attach_table();
    void attach_table(std::shared_ptr<AttachTableData> _attach_table_data);

    bool is_empty() const;
    bool is_lite() const;
    void to_lite();
    void to_fat();

    // Шифрация
    void to_encrypt_and_save_lite(void);
    void to_encrypt_and_save_fat(void);

    // Расшифровка
    void to_decrypt_and_save_lite(void);
    void to_decrypt_and_save_fat(void);

    void push_lite_attributes();
    void push_fat_attributes();
    //    bool is_holder();
    //    void active_request(int pos, int openLinkIn);
    //    Record *active_immediately(bool ai) {_active_immediately = ai; return this;}
    //    bool active_immediately() {return _active_immediately;}

    //    bool is_registered_to_record_controller_and_tabmanager() {return _is_registered_to_shadow_list;}
    //    void is_registered_to_record_controller_and_tabmanager(bool reg) {_is_registered_to_shadow_list = reg;}

    //    void binder(std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<Record>>> g) {_binder = g;}
    //    std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<Record>>> binder() const {return _binder;}

    //    void activator(std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<Record>>> a) {_activator = a;}
    //    std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<Record>>> activator() const {return _activator;}

    //    browser::WebView *bind();
    //    browser::WebView *active();
    bool dir_exists();
    bool file_exists();

protected:

    //    browser::WebPage            *_page;
    // ---------------------------------------------------------------------
    // Свойства класса (не забыть перечислить все в конструкторе копривания)
    // Class properties (do not forget to list all the constructor koprivaniya)
    // ---------------------------------------------------------------------

    bool                        _lite_flag;
    //    bool                        _is_registered_to_shadow_list = false;
    //    bool                        _active_request = false;
    //    int                         _position = -1;
    //    int                         _open_link_in_new_window = 0;
    //    bool                        _active_immediately = false;

    // Установка содержимого свойств происходит в вышестоящем коде
    // Set the properties of the contents occurs in the upstream code

    // Light properties // Легкие свойства
    QMap<QString, QString>      _field_data;    // // A list of the properties of records (attributes) ImyaAtributa - Meaning // Перечень свойств записи (атрибутов) ИмяАтрибута - Значение

    // Полновесные свойства
    QByteArray                  _text;          // Содержимое файла с текстом записи
    QMap<QString, QByteArray>   _picture_files; // Содержимое картинок, используемых в тексте записи (используется при переносе через буфер обмена, при DragAndDrop)

    // Таблица прикрепляемых файлов
    std::shared_ptr<AttachTableData>    _attach_table_data;

    //    std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<Record>>> _binder;
    //    std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<Record>>> _activator;

    // -----------------
    // Защищенные методы
    // -----------------

    void text_to_direct(QString _text_source);
    void create_file_and_save_text();

    QString id_and_name() const; // Внутренний метод для облегчения печати отладочной информации

    QString full_dir() const;
    QString short_dir() const;

    QString full_text_file_name() const;
    QString full_file_name(QString fileName) const;

    void to_encrypt_fields(void);
    void to_decrypt_fields(void);

    void check_and_fill_file_dir(QString &nameDirFull, QString &i_file_name);
    void check_and_create_text_file();

    QString natural_field(QString name) const;
    QString calculable_field(QString name) const;

#ifdef _with_record_table
    explicit Record(const Record &obj); // = delete;
#endif

private:




    //    friend class browser::WebPage;
    friend class ItemsFlat;
};

#endif // __RECORD_H__
