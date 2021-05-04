/**
  * A dialog for adjusting spine export settings
  *@author Jakob Wilson - www.jakobwesley.com
  * */

#ifndef SPINEDIALOG_H
#define SPINEDIALOG_H

#include <QDialog>
#include "lipsyncdoc.h"
#include "spinedoc.h"
#include <QTreeWidgetItem>
#include <QtWidgets>

namespace Ui {
class SpineDialog;
}

class SpineDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SpineDialog(LipsyncDoc* _doc, QWidget *parent = 0);
    ~SpineDialog();

private:
    Ui::SpineDialog *ui;
    SpineDoc* spineDoc;
    LipsyncDoc* doc;


public slots:
    void LoadSpineFile();
    void ExportSpine();
};

#endif // SPINEDIALOG_H
