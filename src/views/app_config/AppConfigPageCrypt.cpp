#include <QWidget>
#include <QBoxLayout>
#include <QDir>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>

#include "AppConfigPageCrypt.h"
#include "models/app_config/AppConfig.h"
#include "libraries/GlobalParameters.h"
#include "views/enter_password/EnterPassword.h"
#include "models/database_config/DataBaseConfig.h"
#include "libraries/crypt/Password.h"


extern AppConfig appconfig;
extern GlobalParameters globalparameters;
extern DataBaseConfig databaseconfig;


AppConfigPageCrypt::AppConfigPageCrypt(QWidget *parent) : ConfigPage(parent)
{
    qDebug() << "Create crypt config page";

    setup_ui();
    update_ui();
    setup_signals();
    assembly();
}


AppConfigPageCrypt::~AppConfigPageCrypt()
{

}


void AppConfigPageCrypt::setup_ui(void)
{
    // Группировщик виджетов работы с паролем
    passRetrieveBox = new QGroupBox(this);
    passRetrieveBox->setTitle(tr("Password settings"));

    // Строка состояния пароля
    passRetrieveStatus = new QLabel(this);
    passRetrieveStatus->setWordWrap(true);

    // Кнопка запроса введения пароля
    passRetrieveButton = new QPushButton(this);
    passRetrieveButton->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed, QSizePolicy::ToolButton));

    // Строка с аннотацией
    passRetrieveAnnotation = new QLabel(this);
    passRetrieveAnnotation->setWordWrap(true);

    // Виджеты вставляются в группировщик
    QVBoxLayout *passRetrieveLayout = new QVBoxLayout;
    passRetrieveLayout->addWidget(passRetrieveStatus);
    passRetrieveLayout->addWidget(passRetrieveButton);
    passRetrieveLayout->addWidget(passRetrieveAnnotation);
    passRetrieveBox->setLayout(passRetrieveLayout);


    // Группировщик виджетов выбора, как запрашивать пароль
    howPassRequestBox = new QGroupBox(this);
    howPassRequestBox->setTitle(tr("Access to encrypted data"));

    howPassRequestRadio1 = new QRadioButton(tr("Ask the password when you click on an encrypted item"));
    howPassRequestRadio2 = new QRadioButton(tr("Ask the password at MyTetra startup"));

    // Точка устанавливается возле того пункта, который настроен в конфиге
    if(appconfig.get_howpassrequest() == "atClickOnCryptBranch")
        howPassRequestRadio1->setChecked(true);
    else
        howPassRequestRadio2->setChecked(true);


    storePassRequestBox = new QGroupBox(this);
    storePassRequestBox->setTitle(tr("Store password"));

    passwordSaveEnable = new QCheckBox(tr("Store password locally"));

    passwordSaveAnnotation = new QLabel(tr("Password will be saved at first next entered.<br>Stored password will be cleared if uncheck this checkbox."));

    if(appconfig.getPasswordSaveFlag()) {
        howPassRequestRadio1->setEnabled(false);
        howPassRequestRadio2->setEnabled(false);
        passwordSaveEnable->setChecked(true);
    } else {
        howPassRequestRadio1->setEnabled(true);
        howPassRequestRadio2->setEnabled(true);
        passwordSaveEnable->setChecked(false);
    }


    // Виджеты вставляются в группировщики
    QVBoxLayout *howPassRequestLayout = new QVBoxLayout;
    howPassRequestLayout->addWidget(howPassRequestRadio1);
    howPassRequestLayout->addWidget(howPassRequestRadio2);
    howPassRequestLayout->addWidget(passwordSaveEnable);
    howPassRequestBox->setLayout(howPassRequestLayout);


    QVBoxLayout *storePassRequestLayout = new QVBoxLayout;
    storePassRequestLayout->addWidget(passwordSaveEnable);
    storePassRequestLayout->addWidget(passwordSaveAnnotation);
    storePassRequestBox->setLayout(storePassRequestLayout);



    // Группировщик настойки автозакрытия окна пароля
    autoClosePasswordBox = new QGroupBox(this);
    autoClosePasswordBox->setTitle(tr("Auto closing password window"));

    autoClosePasswordEnable = new QCheckBox(tr("Enable auto closing password window, sec"), this);

    autoClosePasswordDelay = new QSpinBox(this);
    autoClosePasswordDelay->setValue(appconfig.get_autoClosePasswordDelay());
    autoClosePasswordDelay->setRange(1, 999);

    // Устанавливается галка и активность виджета выбора задержки
    onAutoClosePasswordEnableToggle(appconfig.get_autoClosePasswordEnable());

    // Виджеты вставляются в группировщик
    QHBoxLayout *autoClosePasswordLayout = new QHBoxLayout;
    autoClosePasswordLayout->addWidget(autoClosePasswordEnable);
    autoClosePasswordLayout->addWidget(autoClosePasswordDelay);
    autoClosePasswordLayout->addStretch();
    autoClosePasswordBox->setLayout(autoClosePasswordLayout);


    // Разрешать ли расшифровывать зашифрованные файлы аттачей в каталог корзины при предпросмотре
    decryptFileToTrashDirectoryBox = new QGroupBox(this);
    decryptFileToTrashDirectoryBox->setTitle(tr("File decrypt method for attach preview"));

    decryptFileToTrashDirectoryEnable = new QCheckBox(tr("Enable temporary decrypt attach file to trash directory"));

    // Устанавливается галка согласно настройке из файла конфигурации
    decryptFileToTrashDirectoryEnable->setChecked(appconfig.getEnableDecryptFileToTrashDirectory());

    // Виджеты вставляются в группировщик
    QHBoxLayout *decryptFileToTrashDirectoryLayout = new QHBoxLayout;
    decryptFileToTrashDirectoryLayout->addWidget(decryptFileToTrashDirectoryEnable);
    decryptFileToTrashDirectoryBox->setLayout(decryptFileToTrashDirectoryLayout);
}


void AppConfigPageCrypt::update_ui(void)
{
    // Строка состояния пароля
    passRetrieveStatus->setText(getRetrieveStatusText());

    // Кнопка запроса введения пароля
    passRetrieveButton->setText(getRetrieveButtonText());

    // Строка с аннотацией
    passRetrieveAnnotation->setText(getRetrieveAnnotationText());

}


void AppConfigPageCrypt::setup_signals(void)
{
    // При нажатии кнопки работы с паролем
    connect(passRetrieveButton, &QPushButton::clicked, this, &AppConfigPageCrypt::onPassRetrieveButtonClicked);

    // При клике на галку разрешения автозакрытия пароля
    connect(autoClosePasswordEnable, &QCheckBox::toggled, this, &AppConfigPageCrypt::onAutoClosePasswordEnableToggle);

    connect(passwordSaveEnable, &QCheckBox::toggled, this, &AppConfigPageCrypt::onPasswordSaveEnableToggle);
}


void AppConfigPageCrypt::assembly(void)
{
    // Собирается основной слой
    QVBoxLayout *central_layout = new QVBoxLayout();
    central_layout->addWidget(passRetrieveBox);
    central_layout->addWidget(howPassRequestBox);
    central_layout->addWidget(storePassRequestBox);
    central_layout->addWidget(autoClosePasswordBox);
    central_layout->addWidget(decryptFileToTrashDirectoryBox);
    central_layout->addStretch();

    // Основной слой устанавливается
    setLayout(central_layout);
}


QString AppConfigPageCrypt::getRetrieveStatusText(void)
{
    QString status = tr("<b>Status:</b> ");

    // Если в хранилище данных вообще не задан пароль
    if(databaseconfig.get_crypt_mode() == 0)
        status = status + tr("No password is set. ");
    else
        status = status + tr("Password is set. ");

    // Если пароль (точнее хеш пароля) хранится локально
    if(appconfig.getPasswordSaveFlag() &&
       appconfig.getPasswordMiddleHash().length() > 0)
        status = status + tr("Password is saved locally. ");

    return status;
}


QString AppConfigPageCrypt::getRetrieveButtonText(void)
{
    // Если в хранилище данных вообще не задан пароль
    if(databaseconfig.get_crypt_mode() == 0)
        return tr("Set a password");
    else
        return tr("Change password");
}


QString AppConfigPageCrypt::getRetrieveAnnotationText(void)
{
    // Если в хранилище данных вообще не задан пароль
    if(databaseconfig.get_crypt_mode() == 0)
        return tr("A password will be used to encrypt the item that you selected. Use \"Encrypt item\" or \"Decrypt item\" in context menu.");
    else
        return tr("If you change your password all encrypted item will be re-encrypted with a new password.");
}


// Действия при нажатии кнопки работы с паролем
void AppConfigPageCrypt::onPassRetrieveButtonClicked(void)
{
    // Если в хранилище данных вообще не задан пароль
    if(databaseconfig.get_crypt_mode() == 0) {
        // Включается диалог запроса пароля "с нуля"

        Password password;

        if(password.retrievePassword() == false) return;

        update_ui();
        return;
    } else {
        // Включается диалог изменения пароля
        Password password;

        password.replacePassword();
    }

}


void AppConfigPageCrypt::onPasswordSaveEnableToggle(bool checked)
{
    if(checked) {
        howPassRequestRadio1->setEnabled(false);
        howPassRequestRadio2->setEnabled(false);
    } else {
        howPassRequestRadio1->setEnabled(true);
        howPassRequestRadio2->setEnabled(true);
    }
}


// Действие при клике на галку настройки автоматического закрытия окна пароля
void AppConfigPageCrypt::onAutoClosePasswordEnableToggle(bool checked)
{
    // Устанавливается галка и активность виджета выбора задержки
    if(checked) {
        autoClosePasswordEnable->setChecked(true);
        autoClosePasswordDelay->setEnabled(true);
    } else {
        autoClosePasswordEnable->setChecked(false);
        autoClosePasswordDelay->setEnabled(false);
    }
}


// Метод должен возвращать уровень сложности сделанных изменений
// 0 - изменения не требуют перезапуска программы
// 1 - изменения требуют перезапуска программы
int AppConfigPageCrypt::apply_changes(void)
{
    qDebug() << "Apply changes crypt";

    if(howPassRequestRadio1->isChecked() &&
       appconfig.get_howpassrequest() == "atStartProgram")
        appconfig.set_howpassrequest("atClickOnCryptBranch");

    if(howPassRequestRadio2->isChecked() &&
       appconfig.get_howpassrequest() == "atClickOnCryptBranch")
        appconfig.set_howpassrequest("atStartProgram");

    if(autoClosePasswordEnable->isChecked() != appconfig.get_autoClosePasswordEnable())
        appconfig.set_autoClosePasswordEnable(autoClosePasswordEnable->isChecked());

    if(autoClosePasswordDelay->value() != appconfig.get_autoClosePasswordDelay())
        appconfig.set_autoClosePasswordDelay(autoClosePasswordDelay->value());

    if(passwordSaveEnable->isChecked() != appconfig.getPasswordSaveFlag()) {
        appconfig.setPasswordSaveFlag(passwordSaveEnable->isChecked());

        // Если галка установлена что хранить локально пароль ненужно
        if(!passwordSaveEnable->isChecked()) {
            // Промежуточный хеш пароля удаляется
            appconfig.setPasswordMiddleHash("");
        }
    }

    if(decryptFileToTrashDirectoryEnable->isChecked() != appconfig.getEnableDecryptFileToTrashDirectory())
        appconfig.setEnableDecryptFileToTrashDirectory(decryptFileToTrashDirectoryEnable->isChecked());

    return 0;
}
