
#ifndef __PASSWORD_H__
#define __PASSWORD_H__

#include <QString>

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#else
#include <QObject>
#endif

// Терминология:
// Ключ шифрования - это набор байт, полученный путем преобразования пароля.
// Механизм преобразования можно посмотреть в методе
// setCryptKeyToMemory(QString password)
// Промежуточный хеш - это набор байт, полученный путем преобразования пароля,
// по тому же алгоритму что и ключ шифрования,
// только без завершающего преобразования в MD5

#define SAVED_PASSWORD_CHECKING_LINE \
	"This string is used for checking middle hash"

class Password : public QObject {
#if QT_VERSION == 0x050600
	W_OBJECT(Password)
#else
	Q_OBJECT
#endif

	public:
	Password();
	virtual ~Password();

	bool retrievePassword();
	bool replacePassword();
	void resetPassword(void);

	void setCryptKeyToMemory(QString password);

	private:
	bool enterExistsPassword(void);
	bool checkPasswordWithExists(QString password);
	void saveCheckPasswordKey(QString password);

	QByteArray calculateMiddleHash(QString password);

	void saveMiddleHashCheckData(QByteArray middleHash);
	QByteArray calculateMiddleHashCheckData(QByteArray middleHash);

	void smartSaveMiddleHash(QString password);

	bool checkMiddleHash(void);

	void setCryptKeyToMemoryFromMiddleHash(void);
};

#endif  // __PASSWORD_H__
