/**
  * A dialog for adjusting spine export settings
  *@author Jakob Wilson - www.jakobwesley.com
  * */

#include "spinedialog.h"
#include "ui_spinedialog.h"

SpineDialog::SpineDialog(LipsyncDoc* _doc, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SpineDialog)
{
    ui->setupUi(this);

    //set up listeners
    QObject::connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(ExportSpine()));
    QObject::connect(ui->btnBrowse, SIGNAL(clicked(bool)), this, SLOT(LoadSpineFile()));
    this->doc = _doc;
    this->spineDoc = NULL;
}

SpineDialog::~SpineDialog()
{
    if(this->spineDoc != NULL)
        delete this->spineDoc;
    delete ui;
}


/**
 * @brief SpineDialog::LoadSpineFile loads a spine file and adds the bone structure to the tree view
 * @param fileName
 */
void SpineDialog::LoadSpineFile()
{

    QSettings settings;
    QString name = doc->fCurrentVoice->fName + tr(".json");
    QDir dir(settings.value("default_dir", "").toString());
    name = dir.absoluteFilePath(name);
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    tr("Export"), name,
                                                    tr("DAT files (*.json)"));
    if (filePath.isEmpty())
        return;

    if(this->spineDoc)
    {
        delete this->spineDoc;
        this->spineDoc = NULL;
    }
    if(ui->boneTree->model() != NULL)
        delete ui->boneTree->model();

    this->spineDoc = new SpineDoc(filePath);
    QStandardItemModel* model = this->spineDoc->GetModel();
    ui->boneTree->setModel(model);



}


/**
 * @brief SpineDialog::ExportSpine exports the active voice to the selected json file and bone
 * @param voice - the lipsyncvoice that we want to export
 */
void SpineDialog::ExportSpine()
{

    QSettings settings;
    QString name = doc->fCurrentVoice->fName + tr(".json");
    QDir dir(settings.value("default_dir", "").toString());
    name = dir.absoluteFilePath(name);
    QString filePath = QFileDialog::getSaveFileName(this,
                                                    tr("Export"), name,
                                                    tr("DAT files (*.json)"));
    if (filePath.isEmpty())
        return;

    QFileInfo info(filePath);
    settings.setValue("default_dir", info.dir().absolutePath());

    if(!(this->spineDoc))
        this->spineDoc = new SpineDoc(ui->xDimensions->value(), ui->yDimensions->value() );

    this->spineDoc->exportWords = ui->cbExportWords->isChecked();
    this->spineDoc->AddVoice(this->doc->fCurrentVoice, ui->boneTree->currentIndex().data().toString());
    this->spineDoc->Export(filePath);




}
