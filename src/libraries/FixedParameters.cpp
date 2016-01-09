#include <QString>
#include <QStringList>
#include <QMutableMapIterator>

#include "FixedParameters.h"


FixedParameters::FixedParameters(QObject *parent) : QObject(parent)
{
    Q_UNUSED(parent);

    _record_field = (QStringList()              \
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

    _record_natural_field = (QStringList()          \
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

    _record_calculable_field = (QStringList()               \
                                          << "hasAttach"    \
                                          << "attachCount");

    _record_field_crypted = (QStringList()  \
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
// Перечень всех допустимых полей - натуральных и вычислимых
QStringList FixedParameters::recordFieldAvailableList(void) const
{
  // Для скорости задаются напрямую, не вызывая функции формирования из натурального и вычислимого списка полей

  return (QStringList() << "id" \
                        << "name" \
                        << "author" \
                        << "url" \
                        << "tags" \
                        << "ctime" \
                        << "dir" \
                        << "file" \
                        << "crypt" \
                        \
                        << "hasAttach" \
                        << "attachCount");
}


// Перечень всех натуральных полей - то есть тех, которые напрямую хранятся в XML тегах
QStringList FixedParameters::recordNaturalFieldAvailableList(void) const
{
  return (QStringList() << "id" \
                        << "name" \
                        << "author" \
                        << "url" \
                        << "tags" \
                        << "ctime" \
                        << "dir" \
                        << "file" \
                        << "crypt" );
}


// Перечень всех вычислимых полей - такие поля нигде не сохраняются
QStringList FixedParameters::recordCalculableFieldAvailableList(void) const
{
  return (QStringList() << "hasAttach" \
                        << "attachCount");
}
*/


bool FixedParameters::isRecordFieldAvailable(QString name) const
{
    if(_record_field.contains(name))
        return true;
    else
        return false;
}


bool FixedParameters::isRecordFieldNatural(QString name) const
{
    if(_record_natural_field.contains(name))
        return true;
    else
        return false;
}



bool FixedParameters::isRecordFieldCalculable(QString name) const
{
    if(_record_calculable_field.contains(name))
        return true;
    else
        return false;
}


// Получение описаний набора полей
QMap<QString, QString> FixedParameters::recordFieldDescription(QStringList list) const
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


/*
QStringList FixedParameters::recordFieldCryptedList(void) const
{
 QStringList names;

 names << "name";
 names << "author";
 names << "url";
 names << "tags";

 return names;
}
*/
