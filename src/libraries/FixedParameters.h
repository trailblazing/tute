#ifndef __FIXEDPARAMETERS_H__
#define __FIXEDPARAMETERS_H__

#include <QObject>
#include <QMap>
#include <QStringList>

// Неизменяемые параметры, то есть параметры, которые заданы жестко в текущей версии MyTetra
// Immutable parameters, ie parameters that are hard-coded in the current version MyTetra

class FixedParameters : public QObject {
    Q_OBJECT

public:
    FixedParameters(QObject *parent=0);
    virtual ~FixedParameters();

    QStringList _record_field;
    QStringList _record_natural_field;
    QStringList _record_calculable_field;
    QStringList _record_field_crypted;

    bool is_record_field_available(QString name) const;
    bool is_record_field_natural(QString name) const;
    bool is_record_field_calculable(QString name) const;

    QMap<QString, QString> record_field_description(QStringList list) const;

signals:

public slots:

};

#endif // __FIXEDPARAMETERS_H__
