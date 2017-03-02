
#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QListWidget>
#include <QMessageBox>
#include <QScrollArea>
#include <QStackedWidget>

#include "config_dialog.h"
#include "config_page.h"

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(ConfigDialog)
#endif

ConfigDialog::ConfigDialog(QWidget* parent)
    : QDialog(parent)
{
    setup_ui();
    setup_signals();
    assembly();
}

void ConfigDialog::setup_ui(void)
{
    // Список конфигурирующих виджетов
    contentsWidget = new QListWidget;
    contentsWidget->setViewMode(QListView::ListMode);
    contentsWidget->setMovement(QListView::Static);
    // contentsWidget->setMinimumWidth(100); // contentsWidget->setMaximumWidth(150);
    contentsWidget->setCurrentRow(0);
    contentsWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    // contentsWidget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents); // Не компилируется в Qt4

    // В scrollArea будут помещаться конфигурирующие виджеты, чтобы они были работоспособны на небольших экранах
    // scrollArea=new QScrollArea;
    // scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Область переключения конфигурирующих виджетов
    pagesWidget = new QStackedWidget;
    pagesWidget->setMinimumWidth(250);
    pagesWidget->setMinimumHeight(250);
    pagesWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    // Кнопки закрытия диалога
    confirmButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
}

void ConfigDialog::setup_signals(void)
{
    connect(confirmButtons, &QDialogButtonBox::accepted, this, &ConfigDialog::apply_changes);

    connect(confirmButtons, &QDialogButtonBox::rejected, this, &ConfigDialog::reject);

    connect(contentsWidget, &QListWidget::currentItemChanged, this, &ConfigDialog::change_page);
}

void ConfigDialog::assembly(void)
{
    QHBoxLayout* horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(contentsWidget);
    horizontalLayout->addWidget(pagesWidget, 1); // horizontalLayout->addWidget(pagesWidget, 1);

    // scrollArea->setWidget(pagesWidget);
    // scrollArea->adjustSize();

    QHBoxLayout* buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(confirmButtons);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addLayout(horizontalLayout);
    mainLayout->addStretch(1);
    mainLayout->addSpacing(12);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);

    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

void ConfigDialog::set_window_title(QString title)
{
    // Строка в заголовке окна
    setWindowTitle(title);
}

QListWidgetItem* ConfigDialog::add_widget(QWidget* inswidget, QString name)
{
    pagesWidget->addWidget(inswidget);

    return create_items(name);
}

// Создаются пункты для вызова нужных конфигурирующих виджетов
QListWidgetItem* ConfigDialog::create_items(QString name)
{
    QListWidgetItem* item = new QListWidgetItem(contentsWidget);
    item->setText(name);
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    return item;
}

// Приватный слот, переключение виджета настройки при клике по списку настроечных виджетов
void ConfigDialog::change_page(QListWidgetItem* current, QListWidgetItem* previous)
{
    if (!current)
        current = previous;
    pagesWidget->setCurrentIndex(contentsWidget->row(current));

    // scrollArea->adjustSize();
}

void ConfigDialog::externalChangePage(QListWidgetItem* item)
{
    contentsWidget->setCurrentItem(item);

    // scrollArea->adjustSize();
}

void ConfigDialog::updateListWidth(void)
{
    contentsWidget->updateGeometry();
    contentsWidget->update();

    // scrollArea->adjustSize();
}

void ConfigDialog::apply_changes(void)
{
    int difficult_flag = 0;
    // Перебираются виджеты настройки
    for (int i = 0; i < pagesWidget->count(); i++) {
        // Выясняется указатель на виджет
        ConfigPage* cnpg = qobject_cast<ConfigPage*>(pagesWidget->widget(i));
        // Вызывается метод apply_changes() для текущего перебираемого виджета
        if (cnpg->apply_changes() == 1)
            difficult_flag = 1;
    }
    // Если требуется перезапустить программу для принятия изменений
    if (difficult_flag == 1) {
        QMessageBox::warning(this, tr("Warning"), tr("The program will have to be restarted for changes to take effect."), QMessageBox::Ok);
        exit(0);
    }
    // Диалог настройки закрывается
    close();
}
