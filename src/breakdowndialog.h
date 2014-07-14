#ifndef BREAKDOWNDIALOG_H
#define BREAKDOWNDIALOG_H

#include <QDialog>

namespace Ui {
class BreakdownDialog;
}

class LipsyncWord;

class BreakdownDialog : public QDialog
{
	Q_OBJECT

public:
	explicit BreakdownDialog(LipsyncWord *word, QWidget *parent = 0);
	~BreakdownDialog();

	QString PhonemeString();

private slots:
	void on_aiBut();
	void on_oBut();
	void on_eBut();
	void on_uBut();
	void on_lBut();
	void on_wqBut();
	void on_mbpBut();
	void on_fvBut();
	void on_etcBut();
	void on_restBut();

private:
	void AddPhoneme(QString phoneme);

	Ui::BreakdownDialog *ui;

	LipsyncWord		*fWord;
};

#endif // BREAKDOWNDIALOG_H
