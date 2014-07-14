#include "breakdowndialog.h"
#include "ui_breakdowndialog.h"
#include "lipsyncdoc.h"

BreakdownDialog::BreakdownDialog(LipsyncWord *word, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::BreakdownDialog)
{
	fWord = word;

	ui->setupUi(this);
	ui->wordLabel->setText(tr("Break down the word:") + " " + fWord->fText);

	connect(ui->aiBut, SIGNAL(clicked()), this, SLOT(on_aiBut()));
	connect(ui->oBut, SIGNAL(clicked()), this, SLOT(on_oBut()));
	connect(ui->eBut, SIGNAL(clicked()), this, SLOT(on_eBut()));
	connect(ui->uBut, SIGNAL(clicked()), this, SLOT(on_uBut()));
	connect(ui->lBut, SIGNAL(clicked()), this, SLOT(on_lBut()));
	connect(ui->wqBut, SIGNAL(clicked()), this, SLOT(on_wqBut()));
	connect(ui->mbpBut, SIGNAL(clicked()), this, SLOT(on_mbpBut()));
	connect(ui->fvBut, SIGNAL(clicked()), this, SLOT(on_fvBut()));
	connect(ui->etcBut, SIGNAL(clicked()), this, SLOT(on_etcBut()));
	connect(ui->restBut, SIGNAL(clicked()), this, SLOT(on_restBut()));

	QString str;
	for (int i = 0; i < fWord->fPhonemes.size(); i++)
	{
		str += " ";
		str += fWord->fPhonemes[i]->fText;
	}
	ui->breakdownEdit->setText(str.trimmed());
}

BreakdownDialog::~BreakdownDialog()
{
	delete ui;
}

QString BreakdownDialog::PhonemeString()
{
	return ui->breakdownEdit->text().trimmed();
}

void BreakdownDialog::on_aiBut()
{
	AddPhoneme("AI");
}

void BreakdownDialog::on_oBut()
{
	AddPhoneme("O");
}

void BreakdownDialog::on_eBut()
{
	AddPhoneme("E");
}

void BreakdownDialog::on_uBut()
{
	AddPhoneme("U");
}

void BreakdownDialog::on_lBut()
{
	AddPhoneme("L");
}

void BreakdownDialog::on_wqBut()
{
	AddPhoneme("WQ");
}

void BreakdownDialog::on_mbpBut()
{
	AddPhoneme("MBP");
}

void BreakdownDialog::on_fvBut()
{
	AddPhoneme("FV");
}

void BreakdownDialog::on_etcBut()
{
	AddPhoneme("etc");
}

void BreakdownDialog::on_restBut()
{
	AddPhoneme("rest");
}

void BreakdownDialog::AddPhoneme(QString phoneme)
{
	QString str = ui->breakdownEdit->text().trimmed();
	str += " ";
	str += phoneme;
	ui->breakdownEdit->setText(str.trimmed());
}

