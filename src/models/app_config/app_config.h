#ifndef __APPCONFIG_H__
#define __APPCONFIG_H__

#include <memory>

#include <QDebug>
#include <QDir>
#include <QSettings>

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

class QRect;
class QStringList;
class gl_para;

class AppConfig : public QObject {
#if QT_VERSION == 0x050600
	W_OBJECT(AppConfig)
#else
	Q_OBJECT
#endif

    public:
	AppConfig(std::shared_ptr<gl_para> gl_paras_, QObject* pobj = 0);
	~AppConfig();

	// void init(const gl_para &gl_paras_);
	bool is_init(void);

	void sync(void);

	// Версия формата конфигфайла
	int config_version(void) const;
	void config_version(int i);

	// QString root_dir(void) const;
	// bool root_dir(QString dirName);

	// абочая директория
	QString data_dir(void) const;
	bool data_dir(QString dirName);

	// Директория с корзиной
	QString trash_dir(void) const;
	bool trash_dir(QString dirName);

	// Максимальный размер директории корзины в мегабайтах
	unsigned int trash_size(void) const;
	bool trash_size(unsigned int mbSize);

	// Максимально допустимое число файлов в корзине
	int trash_max_file_count(void) const;
	bool trash_max_file_count(int count);

	// Нужно ли показывать подтверждение при выполнении действия "cut на ветке
	bool cut_branch_confirm(void) const;
	bool cut_branch_confirm(bool confirm);

	// Нужно ли печатать отладочные сообщения в консоль
	bool print_debug_messages(void) const;
	bool print_debug_messages(bool isPrint);

	// Язык интерфейса
	QString interface_language(void) const;
	bool interface_language(QString language);

	// Номер последнего префикса в виде строки с ведущими нулями
	QString add_new_record_expand_info(void) const;
	void add_new_record_expand_info(QString);

	// Геометрия основного окна
	QRect mainwin_geometry(void) const;
	void mainwin_geometry(int x, int y, int w, int h);

	// QSize treescreensize(void)const;
	// void treescreensize(const QSize&);

	void h_tree_splitter_sizelist(QList<int> list);
	QList<int> h_tree_splitter_sizelist();

	void h_record_splitter_sizelist(QList<int> list);
	QList<int> h_record_splitter_sizelist();

	QList<int> v_right_splitter_sizelist(void) const;
	void v_right_splitter_sizelist(QList<int> list);

	// QList<int> hspl_sizelist(void)const;
	// void hspl_sizelist(QList<int> list);

	QList<int> findsplitter_sizelist(void) const;
	void findsplitter_sizelist(QList<int> list);

	QList<int> splitter_sizelist(QString name) const;
	void splitter_sizelist(QString name, QList<int> list);

	std::pair<QString, QStringList> tree_position(void) const;
	void tree_position(QString view_root_id, QStringList current_item_absolute_path);

	// ID записи в таблице конечных записей, которая выделена на экране
	QString recordtable_selected_record_id(void) const;
	void recordtable_selected_record_id(QString id);

	// Настройки виджета поиска по базе
	int findscreen_wordregard(void) const;
	void findscreen_wordregard(int pos);

	int findscreen_howextract(void) const;
	void findscreen_howextract(int pos);

	int find_screen_tree_search_area(void) const;
	void find_screen_tree_search_area(int pos);

	bool findscreen_find_in_field(QString fieldName) const;
	void findscreen_find_in_field(QString fieldName, bool isChecked);

	bool editor_show(void) const;
	void editor_show(bool isShow);

	bool findscreen_show(void) const;
	void findscreen_show(bool isShow);

	QString howpassrequest(void) const;
	void howpassrequest(QString mode);

	bool run_in_minimized_window(void) const;
	void run_in_minimized_window(bool flag);

	bool auto_close_password_enable(void) const;
	void auto_close_password_enable(bool flag);

	int auto_close_password_delay(void) const;
	void auto_close_password_delay(int delay);

	QString synchro_command(void) const;
	void synchro_command(QString command);

	bool synchro_on_startup(void) const;
	void synchro_on_startup(bool flag);

	bool synchro_on_exit(void) const;
	void synchro_on_exit(bool flag);

	bool synchro_console_details(void) const;
	void synchro_console_details(bool flag);

	int editor_cursor_position(void) const;
	void editor_cursor_position(int n);

	int editor_scroll_bar_position(void) const;
	void editor_scroll_bar_position(int n);

	QString password_middle_hash(void) const;
	void password_middle_hash(QString hash);

	bool password_save_flag(void) const;
	void password_save_flag(bool flag);

	bool remember_cursor_at_history_navigation(void) const;
	void remember_cursor_at_history_navigation(bool flag);

	bool remember_cursor_at_ordinary_selection(void) const;
	void remember_cursor_at_ordinary_selection(bool flag);

	int ugly_qss_replace_height_for_table_view(void) const;
	void ugly_qss_replace_height_for_table_view(int n);

	// Перечень полей, отображаемых в таблице конечных записей
	QStringList record_table_show_fields(void) const;
	void record_table_show_fields(QStringList fields);

	// Нужно ли отображать горизонтальные заголовки в таблице конечных записей
	bool record_table_show_horizontal_headers(void) const;
	void record_table_show_horizontal_headers(bool flag);

	// Whether to show vertical headers in the final table of records   // Нужно
	// ли отображать вертикальные заголовки в таблице конечных записей
	bool record_table_show_vertical_headers(void) const;
	void record_table_show_vertical_headers(bool flag);

	// Ширина полей, отображаемых в таблице конечных записей
	QStringList record_table_fields_width(void) const;
	void record_table_fields_width(QStringList fieldsWidth);

	// Показывать ли сплешскрин при старте программы
	bool show_splash_screen(void) const;
	void show_splash_screen(bool isShow);

	// ежим интерфейса, возможные значения "desktop" и "mobile"
	QString interface_mode(void) const;
	void interface_mode(QString mode);

	QString focus_widget(void) const;
	void focus_widget(QString widgetName);

	QStringList hide_editor_tools(void) const;
	void hide_editor_tools(QStringList toolsNames);

	// Флаг, показывающий распахнуты ли все инструметы управления в окне поиска по
	// базе
	bool find_in_base_expand(void) const;
	void find_in_base_expand(bool state);

	// азрешено ли использовать собственный формат вывода даты и времени
	bool enable_custom_datetime_format(void) const;
	void enable_custom_datetime_format(bool state);

	// Строка собственного формата вывода даты и времени
	QString custom_datetime_format(void) const;
	void custom_datetime_format(QString format);

	// Путь на диске по которому пользователь открывал файлы чтобы приаттачить их
	// к записи
	QString attach_append_dir(void) const;
	void attach_append_dir(QString dir);

	// Путь на диске по которому пользователь сохранял (Сохранить как...)
	// приаттаченные файлы
	QString attach_save_as_dir(void) const;
	void attach_save_as_dir(QString dir);

	// азрешать ли для просмотра расшифровывать зашифрованные файлы в директорию
	// корзины
	bool enable_decrypt_file_to_trash_directory(void) const;
	void enable_decrypt_file_to_trash_directory(bool state);

	int topic_number() const;
	void topic_number(int num);

    private:
	QSettings* _app_conf;
	QString _config_file_full_name = ""; // было static
	QString get_parameter(QString name) const;

	QStringList remove_parameter_from_table(QString removeName, QStringList table);
	QString parameter_type_from_table(QString parameterName, QStringList table) const;
	QString parameter_value_from_table(QString parameterName, QStringList table) const;
	QStringList replace_parameter_in_table(QString replaceName, QString replaceType, QString replaceValue, QStringList table);

	void update_version_process(void);

	template <int version>
	QStringList parameter_table(bool withEndSignature = true);

	// QStringList	get_parameter_table_1(bool withEndSignature = true)
	// const;
	// QStringList	get_parameter_table_2(bool withEndSignature = true)
	// const;
	// QStringList	get_parameter_table_3(bool withEndSignature = true)
	// const;
	// QStringList	get_parameter_table_4(bool withEndSignature = true)
	// const;
	// QStringList	get_parameter_table_5(bool withEndSignature = true);
	// QStringList	get_parameter_table_6(bool withEndSignature = true);
	// QStringList	get_parameter_table_7(bool withEndSignature = true);
	// QStringList	get_parameter_table_8(bool withEndSignature = true);
	// QStringList	get_parameter_table_9(bool withEndSignature = true);
	// QStringList	get_parameter_table_10(bool withEndSignature = true);
	// QStringList	get_parameter_table_11(bool withEndSignature = true);
	// QStringList	get_parameter_table_12(bool withEndSignature = true);
	// QStringList	get_parameter_table_13(bool withEndSignature = true);
	// QStringList	get_parameter_table_14(bool withEndSignature = true);
	// QStringList	get_parameter_table_15(bool withEndSignature = true);
	// QStringList	get_parameter_table_16(bool withEndSignature = true);
	// QStringList	get_parameter_table_17(bool withEndSignature = true);
	// QStringList	get_parameter_table_18(bool withEndSignature = true);
	// QStringList	get_parameter_table_19(bool withEndSignature = true);
	// QStringList	get_parameter_table_20(bool withEndSignature = true);
	// QStringList	get_parameter_table_21(bool withEndSignature = true);
	// QStringList	get_parameter_table_22(bool withEndSignature = true);
	// QStringList	get_parameter_table_23(bool withEndSignature = true);
	// QStringList	get_parameter_table_24(bool withEndSignature = true);
	// QStringList	get_parameter_table_25(bool withEndSignature = true);
	// QStringList	get_parameter_table_26(bool withEndSignature = true);
	// QStringList	get_parameter_table_27(bool withEndSignature = true);
	bool is_init_flag = false;
};

template <>
QStringList AppConfig::parameter_table<0>(bool withEndSignature);
template <>
QStringList AppConfig::parameter_table<1>(bool withEndSignature);
template <>
QStringList AppConfig::parameter_table<2>(bool withEndSignature);
template <>
QStringList AppConfig::parameter_table<3>(bool withEndSignature);
template <>
QStringList AppConfig::parameter_table<4>(bool withEndSignature);
template <>
QStringList AppConfig::parameter_table<5>(bool withEndSignature);
template <>
QStringList AppConfig::parameter_table<6>(bool withEndSignature);
template <>
QStringList AppConfig::parameter_table<7>(bool withEndSignature);
template <>
QStringList AppConfig::parameter_table<8>(bool withEndSignature);
template <>
QStringList AppConfig::parameter_table<9>(bool withEndSignature);
template <>
QStringList AppConfig::parameter_table<10>(bool withEndSignature);
template <>
QStringList AppConfig::parameter_table<11>(bool withEndSignature);
template <>
QStringList AppConfig::parameter_table<12>(bool withEndSignature);
template <>
QStringList AppConfig::parameter_table<13>(bool withEndSignature);
template <>
QStringList AppConfig::parameter_table<14>(bool withEndSignature);
template <>
QStringList AppConfig::parameter_table<15>(bool withEndSignature);
template <>
QStringList AppConfig::parameter_table<16>(bool withEndSignature);
template <>
QStringList AppConfig::parameter_table<17>(bool withEndSignature);
template <>
QStringList AppConfig::parameter_table<18>(bool withEndSignature);
template <>
QStringList AppConfig::parameter_table<19>(bool withEndSignature);
template <>
QStringList AppConfig::parameter_table<20>(bool withEndSignature);
template <>
QStringList AppConfig::parameter_table<21>(bool withEndSignature);
template <>
QStringList AppConfig::parameter_table<22>(bool withEndSignature);
template <>
QStringList AppConfig::parameter_table<23>(bool withEndSignature);
template <>
QStringList AppConfig::parameter_table<24>(bool withEndSignature);
template <>
QStringList AppConfig::parameter_table<25>(bool withEndSignature);
template <>
QStringList AppConfig::parameter_table<26>(bool withEndSignature);

#endif // __APPCONFIG_H__
