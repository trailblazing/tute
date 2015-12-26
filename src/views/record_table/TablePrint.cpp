#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

#include "views/print_preview/PrintPreview.h"
#include "models/record_table/TableProxyModel.h"

#include "TablePrint.h"


TablePrint::TablePrint(QWidget *parent) : QDialog(parent)
{
    setup_ui();
    setup_signals();
    assembly();
}


TablePrint::~TablePrint()
{
    delete textArea;

    delete printButton;
    delete saveButton;
    delete cancelButton;

    delete buttonBox;
}


void TablePrint::setup_ui()
{
    setModal(true);

    QSizePolicy sizePolicy;
    sizePolicy.setHorizontalPolicy(QSizePolicy::Expanding);
    sizePolicy.setVerticalPolicy(QSizePolicy::Expanding);

    // Область отображения распечатываемой таблицы
    textArea = new QTextEdit(this);
    textArea->setAcceptRichText(true);
    textArea->setSizePolicy(sizePolicy);

    printButton = new QPushButton(tr("Print"));
    saveButton = new QPushButton(tr("Save"));
    cancelButton = new QPushButton(tr("Cancel"));

    // Линейка с кнопками управления
    buttonBox = new QDialogButtonBox(Qt::Horizontal);
    buttonBox->addButton(printButton,  QDialogButtonBox::AcceptRole);
    buttonBox->addButton(saveButton,   QDialogButtonBox::AcceptRole);
    buttonBox->addButton(cancelButton, QDialogButtonBox::RejectRole);

    // Устанавливается размер окна, равный виджету, из которого
    // этот виджет был вызван
    if(this->parent()->isWidgetType()) {
        QWidget *parentWidget = qobject_cast<QWidget *>(this->parent());
        QRect geom(parentWidget->pos(), parentWidget->size());

        qDebug() << "Parent window geometry " << geom.x() << geom.y() << geom.width() << geom.height();

        setMinimumSize(parentWidget->size());
    }

    // Устанавливается заголовок окна
    this->setWindowTitle(tr("Print notes table"));
}


void TablePrint::setup_signals()
{
    // connect(buttonBox, SIGNAL(accepted()), this, SLOT(print()));
    // connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    connect(printButton,  SIGNAL(clicked()), this, SLOT(print()));
    connect(saveButton,   SIGNAL(clicked()), this, SLOT(save()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}


void TablePrint::assembly()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Добавляется область текста
    mainLayout->addWidget(textArea);

    // Добавляется линейка кнопок Print и Cancel
    mainLayout->addWidget(buttonBox);
}


// Метод установки модели данных, из которой будет формироваться таблица
void TablePrint::setModel(TableProxyModel *iModel)
{
    model = iModel;
}


// Формирование HTML-кода таблицы
void TablePrint::generateHtmlTableFromModel(void)
{
    QString html = "<table border='1' style='border-style:solid; margin-top:0; margin-bottom:0; margin-left:0; margin-right:0;' cellspacing='0' cellpadding='0'>";

    // Заголовок таблицы
    html += "<tr>";

    for(int nColumn = 0; nColumn < model->columnCount(); nColumn++) {
        QVariant cellTextVariant = model->headerData(nColumn, Qt::Horizontal, Qt::DisplayRole);
        QString cellText = cellTextVariant.toString();

        html += "<td style='padding: 10px;'><b>" + cellText + "</b></td>";
    }

    html += "</tr>";


    // Данные из модели сразу преобразуются в HTML
    for(int nRow = 0; nRow < model->rowCount(); nRow++) {
        html += "<tr>";

        for(int nColumn = 0; nColumn < model->columnCount(); nColumn++)
            if(model->index(nRow, nColumn).isValid()) {
                QVariant cellTextVariant = model->index(nRow, nColumn).data(Qt::DisplayRole);
                QString cellText = cellTextVariant.toString();

                html += "<td style='padding: 10px;'>" + cellText + "</td>";
            }

        html += "</tr>";
    }

    html += "</table>";

    qDebug() << html;

    textArea->setHtml(html);
}


void TablePrint::setTitleToHtml(QString title)
{
    textArea->setHtml("<p>" + title + "</p>" + textArea->toHtml());
}


// Слот в котором происходит вызов служебного окна предпросмотра тачатаемой таблицы
void TablePrint::print(void)
{
    QTextDocument doc; // Документ, который будет отправлен на печать

    doc.setHtml(textArea->toHtml());

    PrintPreview *preview = new PrintPreview(&doc, this);

    preview->setModal(true);
    preview->setAttribute(Qt::WA_DeleteOnClose);
    preview->show();
}


void TablePrint::save(void)
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("HTML Files (*.html)"));

    if(fileName != "") {
        QFile file(fileName);

        if(!file.open(QIODevice::WriteOnly)) {
            QMessageBox msgBox;
            msgBox.setText(tr("The file has been write only."));
            msgBox.exec();
        } else {
            QTextStream stream(&file);
            stream.setCodec("UTF-8");
            stream << textArea->document()->toHtml("UTF-8"); // Команда stream << textArea->toHtml() не подходит, так как не выствляет в заголовках charset
            stream.flush();
            file.close();
        }
    }
}
