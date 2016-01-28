#include <QString>
#include <QStringList>
#include <QMutableMapIterator>

#include "FixedParameters.h"


FixedParameters::FixedParameters(QObject *parent) : QObject(parent)
{
    Q_UNUSED(parent);

    _record_field = (QStringList()   \
                     << "id"         \
                     << "pin"        \
                     << "name"       \
                     << "author"     \
                     << "home"       \
                     << "url"        \
                     << "tags"       \
                     << "ctime"      \
                     << "dir"        \
                     << "file"       \
                     << "crypt"      \
                     << "hasAttach"  \
                     << "attachCount");

    _record_natural_field = (QStringList()\
                             << "id"      \
                             << "pin"     \
                             << "name"    \
                             << "author"  \
                             << "home"    \
                             << "url"     \
                             << "tags"    \
                             << "ctime"   \
                             << "dir"     \
                             << "file"    \
                             << "crypt");

    _record_calculable_field = (QStringList()       \
                                << "hasAttach"      \
                                << "attachCount");

    _record_field_crypted = (QStringList() \
                             << "pin"      \
                             << "name"     \
                             << "author"   \
                             << "home"     \
                             << "url"      \
                             << "tags");
}


FixedParameters::~FixedParameters()
{

}


/*

//// Перечень всех допустимых полей - натуральных и вычислимых
//QStringList FixedParameters::record_field_availableList(void) const
//{
//    // Для скорости задаются напрямую, не вызывая функции формирования из натурального и вычислимого списка полей

//    return (QStringList() << "id" \
//            << "name" \
//            << "author" \
//            << "url" \
//            << "tags" \
//            << "ctime" \
//            << "dir" \
//            << "file" \
//            << "crypt" \
//            \
//            << "hasAttach" \
//            << "attachCount");
//}


//// Перечень всех натуральных полей - то есть тех, которые напрямую хранятся в XML тегах
//QStringList FixedParameters::record_natural_field_available_list(void) const
//{
//    return (QStringList() << "id" \
//            << "name" \
//            << "author" \
//            << "url" \
//            << "tags" \
//            << "ctime" \
//            << "dir" \
//            << "file" \
//            << "crypt");
//}


//// Перечень всех вычислимых полей - такие поля нигде не сохраняются
//QStringList FixedParameters::record_calculable_field_available_list(void) const
//{
//    return (QStringList() << "hasAttach" \
//            << "attachCount");
//}

*/

bool FixedParameters::is_record_field_available(QString name) const
{
    //    if(_record_field.contains(name))
    //        return true;
    //    else
    //        return false;
    return _record_field.contains(name);
}


bool FixedParameters::is_record_field_natural(QString name) const
{
    //    if(_record_natural_field.contains(name))
    //        return true;
    //    else
    //        return false;

    return _record_natural_field.contains(name);
}



bool FixedParameters::is_record_field_calculable(QString name) const
{
    //    if(_record_calculable_field.contains(name))
    //        return true;
    //    else
    //        return false;

    return _record_calculable_field.contains(name);
}


// Получение описаний набора полей
QMap<QString, QString> FixedParameters::record_field_description(QStringList list) const
{
    QMap<QString, QString> names;

    names["id"]             = tr("ID");
    names["pin"]            = tr("Pin");
    names["name"]           = tr("Title");
    names["author"]         = tr("Author");
    names["home"]           = tr("Home");
    names["url"]            = tr("Url");
    names["tags"]           = tr("Tags");
    names["ctime"]          = tr("Create time");
    names["dir"]            = tr("Directory name");
    names["file"]           = tr("File name");
    names["crypt"]          = tr("Is crypt");
    names["hasAttach"]      = tr("Has attachs");
    names["attachCount"]    = tr("Attachs count");


    // Удаляются строчки, которых нет в переданном списке
    QMutableMapIterator<QString, QString> iterator(names);

    while(iterator.hasNext()) {
        iterator.next();

        if(list.contains(iterator.key()) == false)
            iterator.remove();
    }

    return names;
}



//QStringList FixedParameters::record_field_crypted_list(void) const
//{
//    QStringList names;

//    names << "name";
//    names << "author";
//    names << "url";
//    names << "tags";

//    return names;
//}

