#include <wobjectimpl.h>
#include <QString>
#include <QStringList>
#include <QMutableMapIterator>

#include "fixed_parameters.h"

//const char *id_field = "id";
//const char *pin_field = "pin";
//const char *rating_field = "rating";
//const char *name_field = "name";
//const char *author_field = "author";
//const char *home_field = "home";
//const char *url_field = "url";
//const char *tags_field = "tags";
//const char *ctime_field = "ctime";
//const char *dir_field = "dir";
//const char *file_field = "file";
//const char *crypt_field = "crypt";
//const char *has_attach_field = "hstch";     // "hasAttach";
//const char *attach_count_field = "tchcnt";  // "attachCount";

W_OBJECT_IMPL(FixedParameters)

// Получение описаний набора полей
QMap<QString, QString> FixedParameters::record_field_description(QStringList list) const {
    QMap<QString, QString> names;

    names[boost::mpl::c_str < id_type > ::value] = tr("ID");
    names[boost::mpl::c_str < pin_type > ::value] = tr("Pin");
    names[boost::mpl::c_str < rating_type > ::value] = tr("Rating");
    names[boost::mpl::c_str < name_type > ::value] = tr("Title");
    names[boost::mpl::c_str < author_type > ::value] = tr("Author");
    names[boost::mpl::c_str < home_type > ::value] = tr("Home");
    names[boost::mpl::c_str < url_type > ::value] = tr("Url");
    names[boost::mpl::c_str < tags_type > ::value] = tr("Tags");
    names[boost::mpl::c_str < ctime_type > ::value] = tr("Create time");
    names[boost::mpl::c_str < dir_type > ::value] = tr("Directory name");
    names[boost::mpl::c_str < file_type > ::value] = tr("File name");
    names[boost::mpl::c_str < crypt_type > ::value] = tr("Is crypt");
    names[boost::mpl::c_str < has_attach_type > ::value] = tr("Has attachs");
    names[boost::mpl::c_str < attach_count_type > ::value] = tr("Attachs count");
    names[boost::mpl::c_str < dynamic_name_type > ::value] = tr("Dynamic name");

    // Удаляются строчки, которых нет в переданном списке
    QMutableMapIterator<QString, QString> iterator(names);
    while(iterator.hasNext()){
        iterator.next();
        if(list.contains(iterator.key()) == false)iterator.remove();
    }
    return names;
}

FixedParameters::FixedParameters(QObject *parent) : QObject(parent){
    Q_UNUSED(parent);

    _record_field = (QStringList()                     \
        << boost::mpl::c_str<id_type>::value           \
        << boost::mpl::c_str<pin_type>::value          \
        << boost::mpl::c_str<rating_type>::value       \
        << boost::mpl::c_str<name_type>::value         \
        << boost::mpl::c_str<author_type>::value       \
        << boost::mpl::c_str<home_type>::value         \
        << boost::mpl::c_str<url_type>::value          \
        << boost::mpl::c_str<tags_type>::value         \
        << boost::mpl::c_str<ctime_type>::value        \
        << boost::mpl::c_str<dir_type>::value          \
        << boost::mpl::c_str<file_type>::value         \
        << boost::mpl::c_str<crypt_type>::value        \
        << boost::mpl::c_str<has_attach_type>::value   \
        << boost::mpl::c_str<attach_count_type>::value \
        << boost::mpl::c_str<dynamic_name_type>::value \
        );

    _record_natural_field = (QStringList()       \
        << boost::mpl::c_str<id_type>::value     \
        << boost::mpl::c_str<pin_type>::value    \
        << boost::mpl::c_str<rating_type>::value \
        << boost::mpl::c_str<name_type>::value   \
        << boost::mpl::c_str<author_type>::value \
        << boost::mpl::c_str<home_type>::value   \
        << boost::mpl::c_str<url_type>::value    \
        << boost::mpl::c_str<tags_type>::value   \
        << boost::mpl::c_str<ctime_type>::value  \
        << boost::mpl::c_str<dir_type>::value    \
        << boost::mpl::c_str<file_type>::value   \
        << boost::mpl::c_str<crypt_type>::value  \
        );

    _record_calculable_field = (QStringList()          \
        << boost::mpl::c_str<has_attach_type>::value   \
        << boost::mpl::c_str<attach_count_type>::value \
        << boost::mpl::c_str<dynamic_name_type>::value \
        );

    _record_field_crypted = (QStringList()       \
        << boost::mpl::c_str<pin_type>::value    \
        << boost::mpl::c_str<name_type>::value   \
        << boost::mpl::c_str<author_type>::value \
        << boost::mpl::c_str<home_type>::value   \
        << boost::mpl::c_str<url_type>::value    \
        << boost::mpl::c_str<tags_type>::value   \
        );
}


FixedParameters::~FixedParameters()
{}


/*

//// Перечень всех допустимых полей - натуральных и вычислимых
//QStringList FixedParameters::record_field_availableList(void) const
//{
//    // Для скорости задаются напрямую, не вызывая функции формирования из натурального и вычислимого списка полей

//    return (QStringList() << "id" \
//            << name_field \
//            << author_field \
//            << url_field \
//            << tags_field \
//            << ctime_field \
//            << dir_field \
//            << file_field \
//            << crypt_field \
//            \
//            << has_attach_field \
//            << attach_count_field);
//}


//// Перечень всех натуральных полей - то есть тех, которые напрямую хранятся в XML тегах
//QStringList FixedParameters::record_natural_field_available_list(void) const
//{
//    return (QStringList() << "id" \
//            << name_field \
//            << author_field \
//            << url_field \
//            << tags_field \
//            << ctime_field \
//            << dir_field \
//            << file_field \
//            << crypt_field);
//}


//// Перечень всех вычислимых полей - такие поля нигде не сохраняются
//QStringList FixedParameters::record_calculable_field_available_list(void) const
//{
//    return (QStringList() << has_attach_field \
//            << attach_count_field);
//}

*/

bool FixedParameters::is_record_field_available(QString name) const {
    // if(_record_field.contains(name))
    // return true;
    // else
    // return false;
    return _record_field.contains(name);
}


bool FixedParameters::is_record_field_natural(QString name) const {
    // if(_record_natural_field.contains(name))
    // return true;
    // else
    // return false;

    return _record_natural_field.contains(name);
}



bool FixedParameters::is_record_field_calculable(QString name) const {
    // if(_record_calculable_field.contains(name))
    // return true;
    // else
    // return false;

    return _record_calculable_field.contains(name);
}






// QStringList FixedParameters::record_field_crypted_list(void) const
// {
// QStringList names;

// names << name_field;
// names << author_field;
// names << url_field;
// names << tags_field;

// return names;
// }

