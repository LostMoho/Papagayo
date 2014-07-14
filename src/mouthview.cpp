#include <QPainter>

#include "mouthview.h"

MouthView::MouthView(QWidget *parent) :
	QWidget(parent)
{
	fDoc = NULL;
	fMouthID = 0;
	fFrame = 0;

	LipsyncDoc::LoadDictionaries();

	for (int32 mouth = 0; mouth < 4; mouth++)
	{
		QString		basePath;

		switch (mouth)
		{
			case 0:
				basePath = ":/mouths/mouths/1_Mouth_1/";
				break;
			case 1:
				basePath = ":/mouths/mouths/2_Mouth_2/";
				break;
			case 2:
				basePath = ":/mouths/mouths/3_Gary_C_Martin/";
				break;
			case 3:
				basePath = ":/mouths/mouths/4_Preston_Blair/";
				break;
		}

		for (int32 i = 0; i < LipsyncDoc::Phonemes.size(); i++)
		{
			fMouths[mouth].insert(LipsyncDoc::Phonemes[i], new QImage(basePath + LipsyncDoc::Phonemes[i] + ".jpg"));
		}
	}
}

MouthView::~MouthView()
{
}

void MouthView::SetDocument(LipsyncDoc *doc)
{
	fDoc = doc;
	update();
}

void MouthView::SetMouth(int32 id)
{
	fMouthID = PG_CLAMP(id, 0, 3);
}

void MouthView::onMouthChanged(int id)
{
	SetMouth(id);
	update();
}

void MouthView::onFrameChanged(int frame)
{
	fFrame = frame;
	update();
}

void MouthView::paintEvent(QPaintEvent *event)
{
	PG_UNUSED(event);

	QString		phoneme;
	QImage		*img = NULL;
	QPainter	dc(this);

	if (fDoc && fDoc->fCurrentVoice)
		phoneme = fDoc->fCurrentVoice->GetPhonemeAtFrame(fFrame);
	else
		phoneme = "etc";
	if (phoneme.isEmpty() && fDoc)
		phoneme = fDoc->GetVolumePhonemeAtFrame(fFrame);

	img = fMouths[fMouthID].value(phoneme);
	if (img)
	{
		int32	x = 0, y = 0;
		int32	w = width();
		int32	h = height();
		QColor	backCol(255, 255, 255);
		if (w > h)
		{
			dc.fillRect(QRect(x, y, w, h), backCol);
			x = (w - h) / 2;
			w = h;
		}
		else if (h > w)
		{
			dc.fillRect(QRect(x, y, w, h), backCol);
			y = (h - w) / 2;
			h = w;
		}
		dc.drawImage(QRect(x, y, w, h), *img);
	}
	else
		dc.eraseRect(0, 0, width(), height());
}
