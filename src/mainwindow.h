#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "lipsyncdoc.h"

class QListWidgetItem;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

public:
	void OpenFile(QString filePath);
	bool IsOKToCloseDocument();
	void RestoreSettings();
	void SaveSettings();

protected:
	void closeEvent(QCloseEvent *event);
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);

public slots:
	void updateActions();

private slots:
	void onHelpAboutPapagayo();
	void onFileOpen();
	void onFileSave();
	void onFileSaveAs();
	void onPlay();
	void onStop();
	void onFpsChange(QString text);
	void onNewVoice();
	void onDeleteVoice();
	void onVoiceSelected(QListWidgetItem *item, QListWidgetItem* previous);
	void onVoiceItemChanged(QListWidgetItem *item);
	void onVoiceNameChanged();
	void onVoiceTextChanged();
	void onBreakdown();
	void onExport();

private:
	void buildVoiceList();

	LipsyncDoc			*fDoc;
	bool				fEnableAutoBreakdown;
	int					fDefaultFps;

	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
