#include <QWidget>
#include <QRegion>
#include <QLayout>
#include <QString>
#include <QWidget>
#include <QVariant>
#include <QTableWidget>
#include <QtDebug>
#include <QHeaderView>
#include <QPaintEvent>
#include <QTableView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QStyledItemDelegate>

#include "FindTableWidget.h"
#include "main.h"
#include "views/main_window/MainWindow.h"
#include "models/app_config/AppConfig.h"

#define USER_ROLE_PIN       Qt::UserRole
#define USER_ROLE_PATH      Qt::UserRole+1
#define USER_ROLE_RECORD_ID Qt::UserRole+2


extern AppConfig appconfig;


FindTableWidget::FindTableWidget(QWidget *parent)
    : QWidget(parent)
    , _findtableview(new QTableView(this))
    , _findtablemodel(new QStandardItemModel(this))
{
    setupUI();
    setupModels();
    setupSignals();
    assembly();

    re_initialize();
}


FindTableWidget::~FindTableWidget(void)
{

}


void FindTableWidget::setupUI(void)
{
    //findTableView = new QTableView(this);
    _findtableview->setObjectName("findTableView");
    _findtableview->setMinimumSize(1, 1);
    _findtableview->horizontalHeader()->hide();

    // Установка высоты строки с принудительной стилизацией (если это необходимо),
    // так как стилизация через QSS для элементов QTableView полноценно не работает
    // У таблицы есть вертикальные заголовки, для каждой строки, в которых отображается номер строки.
    // При задании высоты вертикального заголовка, высота применяется и для всех ячеек в строке.
    _findtableview->verticalHeader()->setDefaultSectionSize(_findtableview->verticalHeader()->minimumSectionSize());
    int height = appconfig.getUglyQssReplaceHeightForTableView();

    if(height != 0)
        _findtableview->verticalHeader()->setDefaultSectionSize(height);

    if(appconfig.getInterfaceMode() == "mobile")
        _findtableview->verticalHeader()->setDefaultSectionSize(calculate_iconsize_px());

    // Устанавливается режим что могут выделяться только строки
    // а не отдельный item таблицы
    _findtableview->setSelectionBehavior(QAbstractItemView::SelectRows);

    // Устанавливается режим что редактирование невозможно
    _findtableview->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Настройка области виджета для кинетической прокрутки
    set_kinetic_scrollarea(qobject_cast<QAbstractItemView *>(_findtableview));
}


void FindTableWidget::setupModels(void)
{
    // Создается модель табличных данных
    //_findtablemodel = new QStandardItemModel(this);

    // Модель привязывается к виду
    _findtableview->setModel(_findtablemodel);
}


void FindTableWidget::setupSignals(void)
{
    connect(_findtableview, SIGNAL(activated(const QModelIndex &)), this, SLOT(selectCell(const QModelIndex &)));
}


void FindTableWidget::assembly(void)
{
    QHBoxLayout *central_layout = new QHBoxLayout();

    central_layout->addWidget(_findtableview);
    central_layout->setContentsMargins(0, 0, 0, 0); // Границы убираются

    this->setLayout(central_layout);
}


void FindTableWidget::updateColumnsWidth(void)
{
    _findtableview->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    _findtableview->hide();
    _findtableview->show();
}


void FindTableWidget::re_initialize(void)
{
    // Модель таблицы очищается
    _findtablemodel->setRowCount(0);
    _findtablemodel->setColumnCount(0);

    // В модели таблицы устанавливаются две колонки Path и Title
    _findtablemodel->setColumnCount(2);

    // В модели устанавливаются заголовки колонок
    QStringList list;
    list << tr("Title") << tr("Details");
    _findtablemodel->setHorizontalHeaderLabels(list);

    _findtableview->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    _findtableview->horizontalHeader()->setStretchLastSection(true);
}


void FindTableWidget::addRow(QString title, QString pin, QString branchName, QString tags, QStringList path, QString recordId)
{
    int i = _findtablemodel->rowCount();

    _findtablemodel->insertRow(i);

    // Принудительная стилизация, так как стилизация через QSS для элементов QTableView полноценно не работает
    // int height=mytetraconfig.getUglyQssReplaceHeightForTableView();
    // if(height!=0)
    //  findTableView->setRowHeight(i, height);

    // Заголовок (название) записи
    QStandardItem *item_title = new QStandardItem();
    item_title->setText(title);

    // В ячейке заголовка также хранится информация о пути к ветке
    // и номере записи в таблице конечных записей
    qDebug() << "Path to record" << path;

    item_title->setData(QVariant(pin), USER_ROLE_PIN);
    item_title->setData(QVariant(path), USER_ROLE_PATH);
    item_title->setData(QVariant(recordId), USER_ROLE_RECORD_ID);


    //    QStandardItem *item_pin = new QStandardItem();
    //    item_pin->setText(pin);
    // Информация о записи
    QStandardItem *item_info = new QStandardItem();
    //    QString info;

    tags = tags.trimmed();

    if(tags.length() > 0)
        item_info->setText(branchName + " (" + tags + ")");
    else
        item_info->setText(branchName);

    //_findtablemodel->setItem(i, 0, item_pin);
    _findtablemodel->setItem(i, 0, item_title);
    _findtablemodel->setItem(i, 1, item_info);


    qDebug() << "In findtablewidget add_row() row count " << _findtablemodel->rowCount();
}


bool FindTableWidget::eventFilter(QObject *o, QEvent *e)
{
    if(o == _findtableview->viewport() && e->type() == QEvent::Paint) {
        QPaintEvent *pe = static_cast<QPaintEvent *>(e);
        qDebug() << "Rect to paint:" << pe->rect();
        qDebug() << "Visible rect:" << (_findtableview->viewport()->visibleRegion()).boundingRect();
        qDebug() << " ";
        return false;
    }

    return false;
}


// void FindTableWidget::selectCell(int row, int column)
void FindTableWidget::selectCell(const QModelIndex &index)
{
    QStandardItem *clickItem = _findtablemodel->itemFromIndex(index);
    QStandardItem *item = _findtablemodel->item(clickItem->row(), 0); // // These are located in the left column at index 0 // Данные находятся в самом левом столбце с индексом 0

    // Выясняется путь к ветке и номер в таблице конечных записей
    QStringList path = item->data(USER_ROLE_PATH).toStringList();
    QString recordId = item->data(USER_ROLE_RECORD_ID).toString();

    qDebug() << "Get path to record:" << path;

    find_object<MainWindow>("mainwindow")->tree_position(path);
    find_object<MainWindow>("mainwindow")->select_id(recordId);
}

void FindTableWidget::remove_id(const QString &id)
{
    QList<QStandardItem *> items = _findtablemodel->findItems(QString(USER_ROLE_RECORD_ID));
    //QStandardItem *id_item = _findtablemodel->item(items->row(), 0);
    QStandardItem *checked = nullptr;
    //QString recordId = id_item->data(USER_ROLE_RECORD_ID).toString();

    for(auto item : items) {
        QString recordId = item->data(USER_ROLE_RECORD_ID).toString();

        if(recordId == id) {checked = item; break;}
    }

    if(checked) {
        _findtablemodel->removeRow(checked->row(), QModelIndex());
        _findtableview->reset();
        _findtableview->setModel(_findtablemodel);
    }
}

void FindTableWidget::remove_row(const int row)
{
    Q_UNUSED(row);
}






