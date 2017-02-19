#ifndef __DATABASECONFIG_H__
#define __DATABASECONFIG_H__

#include <QSettings>
#include <QString>
#include <memory>

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

#define CRYPT_CHECK_ROUNDS 1000
#define CRYPT_CHECK_HASH_LEN 160

class gl_para;
class AppConfig;

class DataBaseConfig : public QObject {
#if QT_VERSION == 0x050600
    W_OBJECT(DataBaseConfig)
#else
    Q_OBJECT
#endif

public:
    DataBaseConfig(std::shared_ptr<gl_para> gl_paras_, std::shared_ptr<AppConfig> appconfig_, QObject* pobj = 0);
    ~DataBaseConfig();

    //	void init(const gl_para &gl_paras_, const AppConfig &appconfig_);
    bool is_init(void);

    // Версия формата конфигфайла
    int get_config_version(void) const;
    void set_config_version(int i);

    int get_crypt_mode(void) const;
    void set_crypt_mode(int mode);

    QString get_crypt_check_salt(void) const;
    void set_crypt_check_salt(QString salt);

    QString get_crypt_check_hash(void) const;
    void set_crypt_check_hash(QString hash);

    QString get_middle_hash_check_data(void) const;
    void set_middle_hash_check_data(QString hash);

private:
    QSettings* conf;
    QString get_parameter(QString name) const;

    bool is_init_flag = false;
};

#endif // __DATABASECONFIG_H__
