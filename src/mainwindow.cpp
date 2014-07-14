#include <QtWidgets>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	fDoc = NULL;
	fEnableAutoBreakdown = true;
	fDefaultFps = 24;

	ui->setupUi(this);

	setAcceptDrops(true);

	ui->waveformView->SetScrollArea(ui->scrollArea);
	ui->fpsEdit->setValidator(new QIntValidator(1, 120));

	connect(ui->actionZoomIn, SIGNAL(triggered()), ui->waveformView, SLOT(onZoomIn()));
	connect(ui->actionZoomOut, SIGNAL(triggered()), ui->waveformView, SLOT(onZoomOut()));
	connect(ui->actionAutoZoom, SIGNAL(triggered()), ui->waveformView, SLOT(onAutoZoom()));
	connect(ui->fpsEdit, SIGNAL(textChanged(QString)), this, SLOT(onFpsChange(QString)));
	connect(ui->waveformView, SIGNAL(frameChanged(int)), ui->mouthView, SLOT(onFrameChanged(int)));

	RestoreSettings();
	updateActions();
}

MainWindow::~MainWindow()
{
	if (fDoc)
		delete fDoc;
	delete ui;
}

void MainWindow::OpenFile(QString filePath)
{
	if (fDoc)
	{
		delete fDoc;
		fDoc = NULL;
	}

	fDoc = new LipsyncDoc;
	QFileInfo info(filePath);
	if (info.suffix().toLower() == "pgo")
	{
		fDoc->Open(filePath);
	}
	else
	{
		fDoc->OpenAudio(filePath);
		fDoc->SetFps(fDefaultFps);
	}

	if (fDoc->GetAudioPlayer() == NULL)
	{
		delete fDoc;
		fDoc = NULL;
		QMessageBox::warning(this, tr("Papagayo"),
							 tr("Error opening audio file."),
							 QMessageBox::Ok);
		setWindowTitle(tr("Papagayo"));
	}
	else
	{
		ui->waveformView->SetDocument(fDoc);
		ui->mouthView->SetDocument(fDoc);
		fDoc->GetAudioPlayer()->setNotifyInterval(17); // 60 fps
		connect(fDoc->GetAudioPlayer(), SIGNAL(positionChanged(qint64)), ui->waveformView, SLOT(positionChanged(qint64)));

		RebuildVoiceList();
		if (fDoc->fCurrentVoice)
		{
			ui->voiceName->setText(fDoc->fCurrentVoice->fName);
			fEnableAutoBreakdown = false;
			ui->voiceText->setPlainText(fDoc->fCurrentVoice->fText);
			fEnableAutoBreakdown = true;
		}
		setWindowTitle(tr("Papagayo") + " - " + info.fileName());
	}

	ui->fpsEdit->setText(QString::number(fDoc->Fps()));
	updateActions();
}

bool MainWindow::IsOKToCloseDocument()
{
	if (fDoc && fDoc->fDirty)
	{
		int res = QMessageBox::warning(this, tr("Papagayo"),
				tr("The document has been modified.\n"
				"Do you want to save your changes?"),
				QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

		if (res == QMessageBox::Yes)
		{
			onFileSave();
			if (fDoc->fDirty)
				return false;
			else
				return true;
		}
		else if (res == QMessageBox::No)
		{
			return true;
		}
		else if (res == QMessageBox::Cancel)
		{
			return false;
		}
	}

	return true;
}

void MainWindow::RestoreSettings()
{
	QSettings settings;

	restoreGeometry(settings.value("MainWindow/geometry").toByteArray());
	restoreState(settings.value("MainWindow/windowState").toByteArray());

	fDefaultFps = settings.value("defaultFps").toInt();
	if (fDefaultFps < 1)
		fDefaultFps = 24;
	fDefaultFps = PG_CLAMP(fDefaultFps, 1, 120);
}

void MainWindow::SaveSettings()
{
	QSettings settings;

	settings.setValue("MainWindow/geometry", saveGeometry());
	settings.setValue("MainWindow/windowState", saveState());
	settings.setValue("defaultFps", fDefaultFps);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	if (IsOKToCloseDocument())
	{
		if (fDoc)
		{
			delete fDoc;
			fDoc = NULL;
		}
		SaveSettings();
		event->accept();
	}
	else
	{
		event->ignore();
	}
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
	QList<QUrl>	urls = event->mimeData()->urls();
	if (urls.isEmpty())
		return;

	QString filePath = urls.first().toLocalFile();

	if (filePath.isEmpty())
		return;

	QFileInfo info(filePath);
	QString extn = info.suffix().toLower();
	if (extn == "wav" || extn == "pgo" || extn == "aif" || extn == "aiff")
		event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
	QList<QUrl>	urls = event->mimeData()->urls();
	if (urls.isEmpty())
		return;

	QString filePath = urls.first().toLocalFile();

	if (filePath.isEmpty())
		return;

	QFileInfo info(filePath);
	QString extn = info.suffix().toLower();
	if (extn == "wav" || extn == "pgo" || extn == "aif" || extn == "aiff")
	{
		event->acceptProposedAction();
		if (IsOKToCloseDocument())
			OpenFile(filePath);
	}
}

void MainWindow::updateActions()
{
	if (fDoc)
	{
		ui->actionSave->setEnabled(true);
		ui->actionSave_As->setEnabled(true);
		ui->actionPlay->setEnabled(true);
		ui->actionStop->setEnabled(true);
		ui->actionZoomIn->setEnabled(true);
		ui->actionZoomOut->setEnabled(true);
		ui->actionAutoZoom->setEnabled(true);

		ui->voiceName->setEnabled(true);
		ui->voiceText->setEnabled(true);
		ui->languageChoice->setEnabled(false);
		ui->breakdownButton->setEnabled(fDoc->fCurrentVoice && !fDoc->fCurrentVoice->fText.isEmpty());
		ui->exportChoice->setEnabled(true);
		ui->exportButton->setEnabled(fDoc->fCurrentVoice && !fDoc->fCurrentVoice->fText.isEmpty());
		ui->fpsEdit->setEnabled(true);
		ui->voiceList->setEnabled(true);
		ui->newVoiceButton->setEnabled(true);
		ui->deleteVoiceButton->setEnabled(fDoc->fCurrentVoice && fDoc->fVoices.size() > 1);
	}
	else
	{
		ui->actionSave->setEnabled(false);
		ui->actionSave_As->setEnabled(false);
		ui->actionPlay->setEnabled(false);
		ui->actionStop->setEnabled(false);
		ui->actionZoomIn->setEnabled(false);
		ui->actionZoomOut->setEnabled(false);
		ui->actionAutoZoom->setEnabled(false);

		ui->voiceName->setEnabled(false);
		ui->voiceText->setEnabled(false);
		ui->languageChoice->setEnabled(false);
		ui->breakdownButton->setEnabled(false);
		ui->exportChoice->setEnabled(false);
		ui->exportButton->setEnabled(false);
		ui->fpsEdit->setEnabled(false);
		ui->voiceList->setEnabled(false);
		ui->newVoiceButton->setEnabled(false);
		ui->deleteVoiceButton->setEnabled(false);
	}
}

void MainWindow::onHelpAboutPapagayo()
{
	QString msg = "\
<center>\
<font size=+3><b>Papagayo</b></font>\
<br><font size=-1><i>Version: 2.0b1</i></font>\
<br><font size=-1>&copy; 2005-2014 Mike Clifton</font>\
<br><font size=-1><a href=\"http://www.gnu.org/licenses/gpl.html\">License: GPLv3</a></font>\
<br><br><img src=\":/images/images/papagayo.png\"><br>\
<br>A lip-sync tool for use with <a href=\"http://anime.smithmicro.com/\">Anime Studio Pro</a>\
<br>(formerly <a href=\"http://www.lostmarble.com\">Lost Marble's Moho</a>)\
<br><font size=-1>This software uses the Carnegie Mellon <a href=\"http://www.speech.cs.cmu.edu/cgi-bin/cmudict\">Pronouncing Dictionary</a></font>\
<br><font size=-1>Spanish pronunciation code provided by <a href=\"http://www-personal.monash.edu.au/~myless/catnap/index.html\">Myles Strous</a></font>\
<br><font size=-1>3D mouth shapes provided by <a href=\"http://www.garycmartin.com\">Gary C. Martin</a></font>\
<br><font size=-1>Some icons provided by <a href=\"http://glyphicons.com\">GLYPHICONS.com</a></font>\
</center>\
";

	QMessageBox::about(this, tr("About Papagayo"), msg);
}

void MainWindow::onFileOpen()
{
	if (!IsOKToCloseDocument())
		return;

	QSettings settings;
	QString filePath = QFileDialog::getOpenFileName(this,
													tr("Open"), settings.value("default_dir", "").toString(),
													tr("Papgayo and Audio files (*.pgo;*.wav;*.aif;*.aiff)"));
	if (filePath.isEmpty())
		return;

	QFileInfo info(filePath);
	settings.setValue("default_dir", info.dir().absolutePath());

	OpenFile(filePath);
}

void MainWindow::onFileSave()
{
	if (!fDoc)
		return;

	if (fDoc->fPath.isEmpty())
	{
		onFileSaveAs();
		return;
	}

	fDoc->Save();
	QFileInfo info(fDoc->fPath);
	setWindowTitle(tr("Papagayo") + " - " + info.fileName());
}

void MainWindow::onFileSaveAs()
{
	if (!fDoc)
		return;

	QSettings settings;
	QString name = tr("Untitled.pgo");
	if (!fDoc->fPath.isEmpty())
	{
		name = fDoc->fPath;
	}
	else
	{
		QDir dir(settings.value("default_dir", "").toString());
		name = dir.absoluteFilePath(name);
	}
	QString filePath = QFileDialog::getSaveFileName(this,
													tr("Save"), name,
													tr("Papgayo files (*.pgo)"));
	if (filePath.isEmpty())
		return;

	QFileInfo info(filePath);
	settings.setValue("default_dir", info.dir().absolutePath());

	fDoc->fPath = filePath;
	onFileSave();
}

void MainWindow::onPlay()
{
	if (fDoc && fDoc->GetAudioPlayer())
		fDoc->GetAudioPlayer()->play();
}

void MainWindow::onStop()
{
	if (fDoc && fDoc->GetAudioPlayer())
		fDoc->GetAudioPlayer()->stop();
}

void MainWindow::onFpsChange(QString text)
{
	if (!fDoc)
		return;

	int32 fps = text.toInt();
	fps = PG_CLAMP(fps, 1, 120);

	if (fps == fDoc->Fps())
		return;

	fDefaultFps = fps;
	fDoc->SetFps(fps);
	ui->waveformView->SetDocument(NULL);
	ui->waveformView->SetDocument(fDoc);
}

void MainWindow::onNewVoice()
{
	if (!fDoc)
		return;

	QString		newVoiceName = tr("Voice");
	newVoiceName += " ";
	newVoiceName += QString::number(fDoc->fVoices.size() + 1);
	fDoc->fCurrentVoice = new LipsyncVoice(newVoiceName);
	fDoc->fVoices << fDoc->fCurrentVoice;
	RebuildVoiceList();
	if (fDoc->fCurrentVoice)
	{
		ui->voiceName->setText(fDoc->fCurrentVoice->fName);
		ui->voiceText->setPlainText(fDoc->fCurrentVoice->fText);
	}
}

void MainWindow::onDeleteVoice()
{
	if (!fDoc || fDoc->fVoices.size() < 2 || fDoc->fCurrentVoice == NULL)
		return;

	int id = fDoc->fVoices.indexOf(fDoc->fCurrentVoice);
	fDoc->fVoices.removeAt(id);
	delete fDoc->fCurrentVoice;
	if (id > 0)
		id--;
	fDoc->fCurrentVoice = fDoc->fVoices[id];
	RebuildVoiceList();
	if (fDoc->fCurrentVoice)
	{
		ui->voiceName->setText(fDoc->fCurrentVoice->fName);
		fEnableAutoBreakdown = false;
		ui->voiceText->setPlainText(fDoc->fCurrentVoice->fText);
		fEnableAutoBreakdown = true;
	}
	updateActions();
}

void MainWindow::onVoiceSelected(QListWidgetItem *item)
{
	if (fRebuildingList || !fDoc)
		return;

	int id = ui->voiceList->row(item);
	if (id >= 0 && id < fDoc->fVoices.size())
	{
		fDoc->fCurrentVoice = fDoc->fVoices[id];
		if (fDoc->fCurrentVoice)
		{
			ui->voiceName->setText(fDoc->fCurrentVoice->fName);
			fEnableAutoBreakdown = false;
			ui->voiceText->setPlainText(fDoc->fCurrentVoice->fText);
			fEnableAutoBreakdown = true;
		}
	}
	ui->waveformView->update();
	updateActions();
}

void MainWindow::onVoiceItemChanged(QListWidgetItem *item)
{
	if (fRebuildingList || !fDoc)
		return;

	int id = ui->voiceList->row(item);
	if (id >= 0 && id < fDoc->fVoices.size())
	{
		fDoc->fCurrentVoice = fDoc->fVoices[id];
		if (fDoc->fCurrentVoice)
		{
			fDoc->fCurrentVoice->fName = item->text();
			ui->voiceName->setText(fDoc->fCurrentVoice->fName);
		}
	}
}

void MainWindow::onVoiceNameChanged()
{
	if (!fDoc || !fDoc->fCurrentVoice)
		return;

	fDoc->fCurrentVoice->fName = ui->voiceName->text();
	RebuildVoiceList();
}

void MainWindow::onVoiceTextChanged()
{
	if (!fDoc || !fDoc->fCurrentVoice)
		return;

	fDoc->fCurrentVoice->fText = ui->voiceText->toPlainText();
	if (fEnableAutoBreakdown)
		onBreakdown(); // this is cool, but it could slow things down by doing constant breakdowns
	updateActions();
}

void MainWindow::onBreakdown()
{
	if (!fDoc || !fDoc->fCurrentVoice)
		return;

	LipsyncDoc::LoadDictionaries();
	fDoc->fDirty = true;
	int32 duration = fDoc->Fps() * 10;
	if (fDoc->GetAudioExtractor())
	{
		real f = fDoc->GetAudioExtractor()->Duration();
		f *= fDoc->Fps();
		duration = PG_ROUND(f);
	}
	fDoc->fCurrentVoice->RunBreakdown("EN", duration);
	ui->waveformView->update();
}

void MainWindow::onExport()
{
	if (!fDoc || !fDoc->fCurrentVoice)
		return;

	QSettings settings;
	QString name = fDoc->fCurrentVoice->fName + tr(".dat");
	QDir dir(settings.value("default_dir", "").toString());
	name = dir.absoluteFilePath(name);
	QString filePath = QFileDialog::getSaveFileName(this,
													tr("Export"), name,
													tr("DAT files (*.dat)"));
	if (filePath.isEmpty())
		return;

	QFileInfo info(filePath);
	settings.setValue("default_dir", info.dir().absolutePath());

	fDoc->fCurrentVoice->Export(filePath);
}

void MainWindow::RebuildVoiceList()
{
	if (fRebuildingList)
		return;

	fRebuildingList = true;
	ui->voiceList->clear();
	if (fDoc)
	{
		for (int i = 0; i < fDoc->fVoices.size(); i++)
		{
			ui->voiceList->addItem(fDoc->fVoices[i]->fName);
			QListWidgetItem *item = ui->voiceList->item(i);
			item->setFlags(item->flags() | Qt::ItemIsEditable);
		}
		if (fDoc->fCurrentVoice)
		{
			ui->voiceList->setCurrentItem(ui->voiceList->item(fDoc->fVoices.indexOf(fDoc->fCurrentVoice)));
		}
	}
	fRebuildingList = false;
}
