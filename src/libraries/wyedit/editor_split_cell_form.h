#ifndef _EDITORSPLITCELLFORM_H_
#define _EDITORSPLITCELLFORM_H_

#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QSpinBox>
#include <QWidget>

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

class EditorSplitCellForm : public QDialog {
#if QT_VERSION == 0x050600
    W_OBJECT(EditorSplitCellForm)
#else
    Q_OBJECT
#endif
public:
    EditorSplitCellForm(int fixHorisontalSplit, int fixVerticalSplit);
    ~EditorSplitCellForm();

    int get_num_split_h(void);
    int get_num_split_v(void);

private:
    int fixHSplit;
    int fixVSplit;

    QCheckBox* checkboxFixHSplit;
    QCheckBox* checkboxFixVSplit;

    QLabel* checkboxLabelFixHSplit;
    QLabel* checkboxLabelFixVSplit;

    QLabel* labelPictogramH;
    QLabel* labelPictogramV;

    QLabel* labelH;
    QLabel* labelV;

    QSpinBox* numSplitH;
    QSpinBox* numSplitV;

    QDialogButtonBox* buttonBox;
};

#endif // _EDITORSPLITCELLFORM_H_
