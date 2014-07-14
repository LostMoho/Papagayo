#include <QPainter>
#include <QMouseEvent>
#include <QScrollArea>
#include <QScrollBar>

#include "waveformview.h"
#include "breakdowndialog.h"

#define DEFAULT_SAMPLE_WIDTH		4
#define DEFAULT_SAMPLES_PER_FRAME	2

WaveformView::WaveformView(QWidget *parent) :
	QWidget(parent)
{
//	setAttribute(Qt::WA_StaticContents);
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);// QSizePolicy::Fixed ?

	fScrollArea = NULL;
	fDoc = NULL;
	fNumSamples = 0;
	fAmp = NULL;
	fDragging = false;
	fDoubleClick = false;
	fCurFrame = 0;
	fOldFrame = 0;
	fScrubFrame = 0;
	fAudioStopFrame = -1;
	fSampleWidth = DEFAULT_SAMPLE_WIDTH;
	fSamplesPerFrame = DEFAULT_SAMPLES_PER_FRAME;
	fSamplesPerSec = 24 * fSamplesPerFrame;
	fFrameWidth = fSampleWidth * fSamplesPerFrame;
	fPhraseBottom = 16;
	fWordBottom = 32;
	fPhonemeTop = 128;
	fSelectedPhrase = NULL;
	fSelectedWord = NULL;
	fSelectedPhoneme = NULL;
}

WaveformView::~WaveformView()
{
	if (fAmp)
	{
		delete [] fAmp;
		fAmp = NULL;
	}
}

QSize WaveformView::sizeHint() const
{
	if (fDoc && fNumSamples > 0)
		return QSize(fNumSamples * fSampleWidth, 100);
	else
		return QSize(400, 100);
}

void WaveformView::SetScrollArea(QScrollArea *scrollArea)
{
	fScrollArea = scrollArea;
}

void WaveformView::SetDocument(LipsyncDoc *doc)
{
	if (fDoc == NULL && doc)
	{
		fSampleWidth = DEFAULT_SAMPLE_WIDTH;
		fSamplesPerFrame = DEFAULT_SAMPLES_PER_FRAME;
		fSamplesPerSec = doc->Fps() * fSamplesPerFrame;
		fFrameWidth = fSampleWidth * fSamplesPerFrame;
	}
	fDoc = doc;
	fNumSamples = 0;
	if (fAmp)
	{
		delete [] fAmp;
		fAmp = NULL;
	}
	if (fDoc && fDoc->GetAudioExtractor())
	{
		AudioExtractor *extractor = fDoc->GetAudioExtractor();

		fFrameWidth = fSampleWidth * fSamplesPerFrame;
		real duration = extractor->Duration();
		real time = 0.0f;
		real sampleDur = 1.0f / fSamplesPerSec;
		real maxAmp = 0.0f;
		while (time < duration)
		{
			fNumSamples++;
			time += sampleDur;
		}
		if (fNumSamples < 1)
			fNumSamples = 1;
		fAmp = new real[fNumSamples];
		time = 0.0f;
		int32 i = 0;
		while (time < duration)
		{
			fAmp[i] = extractor->GetRMSAmplitude(time, sampleDur);
			if (fAmp[i] > maxAmp)
				maxAmp = fAmp[i];
			time += sampleDur;
			i++;
		}
		// normalize amplitudes
		maxAmp = 0.95f / maxAmp;
		for (i = 0; i < fNumSamples; i++)
			fAmp[i] *= maxAmp;
	}

	updateGeometry();
	update();
}

void WaveformView::onZoomIn()
{
	if (fDoc && fSamplesPerFrame < 16)
	{
		if (fSampleWidth < 2)
		{
			fSampleWidth = 2;
			fSamplesPerFrame = 1;
			fSamplesPerSec = fDoc->Fps() * fSamplesPerFrame;
			fFrameWidth = fSampleWidth * fSamplesPerFrame;
			SetDocument(fDoc);
		}
		else
		{
			fSamplesPerFrame *= 2;
			fSamplesPerSec = fDoc->Fps() * fSamplesPerFrame;
			fFrameWidth = fSampleWidth * fSamplesPerFrame;
			SetDocument(fDoc);
		}
	}
}

void WaveformView::onZoomOut()
{
	if (fDoc)
	{
		if (fSamplesPerFrame > 1)
		{
			fSamplesPerFrame /= 2;
			if (fSamplesPerFrame < 1)
				fSamplesPerFrame = 1;
			fSamplesPerSec = fDoc->Fps() * fSamplesPerFrame;
			fFrameWidth = fSampleWidth * fSamplesPerFrame;
			SetDocument(fDoc);
		}
		else if (fSampleWidth > 2)
		{
			fSampleWidth /= 2;
			if (fSampleWidth < 1)
				fSampleWidth = 1;
			fSamplesPerSec = fDoc->Fps() * fSamplesPerFrame;
			fFrameWidth = fSampleWidth * fSamplesPerFrame;
			SetDocument(fDoc);
		}
	}
}

void WaveformView::onAutoZoom()
{
	if (fDoc)
	{
		fSampleWidth = DEFAULT_SAMPLE_WIDTH;
		fSamplesPerFrame = DEFAULT_SAMPLES_PER_FRAME;
		fSamplesPerSec = fDoc->Fps() * fSamplesPerFrame;
		fFrameWidth = fSampleWidth * fSamplesPerFrame;
		SetDocument(fDoc);
	}
}

void WaveformView::positionChanged(qint64 milliseconds)
{
	if (fDoc)
	{
		real f = ((real)milliseconds / 1000.0f) * fDoc->Fps();
		int32 frame = PG_FLOOR(f);
		if (frame != fCurFrame)
		{
			if (fAudioStopFrame >= 0)
			{
				if (frame > fAudioStopFrame)
				{
					if (fDoc->GetAudioPlayer())
						fDoc->GetAudioPlayer()->stop();
					fAudioStopFrame = -1;
				}
				else
				{
					fCurFrame = frame;
					emit(frameChanged(fCurFrame));
				}
				update();
			}
			else if (fDragging)
			{
				if (frame > fCurFrame + 1)
				{
					if (fDoc->GetAudioPlayer())
						fDoc->GetAudioPlayer()->stop();
				}
			}
			else
			{
				fCurFrame = frame;
				emit(frameChanged(fCurFrame));
				update();
			}


			QMediaPlayer *audioPlayer = fDoc->GetAudioPlayer();
			if (!fDragging && audioPlayer && audioPlayer->state() == QMediaPlayer::PlayingState)
			{
				if (fScrollArea)
				{
					QScrollBar *scrollBar = fScrollArea->horizontalScrollBar();
					if (scrollBar)
					{
						int frameX = fCurFrame * fFrameWidth;
						int scrollX = scrollBar->value();
						int scrollW = fScrollArea->width();
						if (frameX - scrollX > scrollW)
						{
							scrollBar->setValue(frameX - scrollW / 6);
						}
						else if (frameX - scrollX < 0)
							scrollBar->setValue(frameX);
					}
				}
			}
		}
	}
}

void WaveformView::mousePressEvent(QMouseEvent *event)
{
	int32	mouseY = event->y();
	real	f = (real)event->x() / (real)fFrameWidth;
	int32	frame = PG_FLOOR(f), frameDist;

	fScrubFrame = -1;
	fCurFrame = -1;
	fOldFrame = frame;
	fDragging = false;
	fDraggingEnd = -1;
	fSelectedPhrase = fParentPhrase = NULL;
	fSelectedWord = fParentWord = NULL;
	fSelectedPhoneme = NULL;

	if (fDoc && fDoc->GetAudioPlayer())
	{
		fDragging = true;

		if (fDoc->fCurrentVoice)
		{ // test to see if the user clicked on a phrase, word, or phoneme
			// first, find the phrase that was clicked on
			for (int32 i = 0; i < fDoc->fCurrentVoice->fPhrases.size(); i++)
			{
				if (frame >= fDoc->fCurrentVoice->fPhrases[i]->fStartFrame && frame <= fDoc->fCurrentVoice->fPhrases[i]->fEndFrame)
				{
					fSelectedPhrase = fDoc->fCurrentVoice->fPhrases[i];
					break;
				}
			}
			// next, find the word that was clicked on
			if (fSelectedPhrase)
			{
				for (int32 i = 0; i < fSelectedPhrase->fWords.size(); i++)
				{
					if (frame >= fSelectedPhrase->fWords[i]->fStartFrame && frame <= fSelectedPhrase->fWords[i]->fEndFrame)
					{
						fSelectedWord = fSelectedPhrase->fWords[i];
						break;
					}
				}
			}
			// finally, find the phoneme that was clicked on
			if (fSelectedWord)
			{
				for (int32 i = 0; i < fSelectedWord->fPhonemes.size(); i++)
				{
					if (frame == fSelectedWord->fPhonemes[i]->fFrame)
					{
						fSelectedPhoneme = fSelectedWord->fPhonemes[i];
						break;
					}
				}
			}

			fParentPhrase = fSelectedPhrase;
			fParentWord = fSelectedWord;

			// now, test if the click was within the vertical range of one of these objects
			if (fSelectedPhrase && mouseY >= fSelectedPhrase->fTop && mouseY <= fSelectedPhrase->fBottom)
			{
				fSelectedWord = NULL;
				fSelectedPhoneme = NULL;
				fDraggingEnd = 0; // beginning of phrase
				frameDist = frame - fSelectedPhrase->fStartFrame;
				if ((fSelectedPhrase->fEndFrame - frame) < frameDist)
				{
					fDraggingEnd = 1; // end of phrase
					frameDist = fSelectedPhrase->fEndFrame - frame;
				}
				if ((fSelectedPhrase->fEndFrame - fSelectedPhrase->fStartFrame > 1) &&
						(PG_FABS((fSelectedPhrase->fEndFrame + fSelectedPhrase->fStartFrame) / 2 - frame) < frameDist))
				{
					fDraggingEnd = 2; // middle of phrase
				}
			}
			else if (fSelectedWord && mouseY >= fSelectedWord->fTop && mouseY <= fSelectedWord->fBottom)
			{
				fSelectedPhrase = NULL;
				fSelectedPhoneme = NULL;
				fDraggingEnd = 0; // beginning of word
				frameDist = frame - fSelectedWord->fStartFrame;
				if ((fSelectedWord->fEndFrame - frame) < frameDist)
				{
					fDraggingEnd = 1; // end of word
					frameDist = fSelectedWord->fEndFrame - frame;
				}
				if ((fSelectedWord->fEndFrame - fSelectedWord->fStartFrame > 1) &&
						(PG_FABS((fSelectedWord->fEndFrame + fSelectedWord->fStartFrame) / 2 - frame) < frameDist))
				{
					fDraggingEnd = 2; // middle of phrase
				}
			}
			else if (fSelectedPhoneme && mouseY >= fSelectedPhoneme->fTop && mouseY <= fSelectedPhoneme->fBottom)
			{
				fSelectedPhrase = NULL;
				fSelectedWord = NULL;
				fDraggingEnd = 0;
			}
			else
			{
				fSelectedPhrase = fParentPhrase = NULL;
				fSelectedWord = fParentWord = NULL;
				fSelectedPhoneme = NULL;
			}

			if (fSelectedPhrase == NULL && fSelectedWord == NULL && fSelectedPhoneme == NULL)
			{
				mouseMoveEvent(event);
			}
			if (event->button() == Qt::RightButton && fSelectedWord)
			{
				fDragging = false;
			}
			else if (fDoubleClick)
			{
				bool playSegment = false;
				QMediaPlayer *audioPlayer = fDoc->GetAudioPlayer();
				int32 startFrame;
				fAudioStopFrame = -1;
				if (audioPlayer)
				{
					if (fSelectedPhrase)
					{
						playSegment = true;
						startFrame = fSelectedPhrase->fStartFrame;
						fAudioStopFrame = fSelectedPhrase->fEndFrame + 1;
					}
					else if (fSelectedWord)
					{
						playSegment = true;
						startFrame = fSelectedWord->fStartFrame;
						fAudioStopFrame = fSelectedWord->fEndFrame + 1;
					}
					else if (fSelectedPhoneme)
					{
						playSegment = true;
						startFrame = fSelectedPhoneme->fFrame;
						fAudioStopFrame = startFrame + 1;
					}
					if (playSegment)
					{
						float f = ((real)startFrame / (real)fDoc->Fps()) * 1000.0f;
						audioPlayer->setPosition(PG_ROUND(f));
						audioPlayer->play();
						emit(frameChanged(fScrubFrame));
					}
				}
				fDragging = false;
				fDraggingEnd = -1;
				fSelectedPhrase = NULL;
				fSelectedWord = NULL;
				fSelectedPhoneme = NULL;
			}
		}
	}
}

void WaveformView::mouseDoubleClickEvent(QMouseEvent *event)
{
	fDoubleClick = true;
	mousePressEvent(event);
	fDoubleClick = false;
}

void WaveformView::mouseMoveEvent(QMouseEvent *event)
{
	if (!fDragging || !fDoc)
		return;

	bool	needUpdate = false;
	real	f = (real)event->x() / (real)fFrameWidth;
	int32	frame = PG_FLOOR(f);

	if (fSelectedPhrase)
	{
		if (fDraggingEnd == 0)
		{
			if (frame != fSelectedPhrase->fStartFrame)
			{
				fDoc->fDirty = true;
				fSelectedPhrase->fStartFrame = frame;
				if (fSelectedPhrase->fStartFrame > fSelectedPhrase->fEndFrame - 1)
					fSelectedPhrase->fStartFrame = fSelectedPhrase->fEndFrame - 1;
				fDoc->fCurrentVoice->RepositionPhrase(fSelectedPhrase, fDoc->Duration());
				needUpdate = true;
			}
		}
		else if (fDraggingEnd == 1)
		{
			if (frame != fSelectedPhrase->fEndFrame)
			{
				fDoc->fDirty = true;
				fSelectedPhrase->fEndFrame = frame;
				if (fSelectedPhrase->fEndFrame < fSelectedPhrase->fStartFrame + 1)
					fSelectedPhrase->fEndFrame = fSelectedPhrase->fStartFrame + 1;
				fDoc->fCurrentVoice->RepositionPhrase(fSelectedPhrase, fDoc->Duration());
				needUpdate = true;
			}
		}
		else if (fDraggingEnd == 2)
		{
			if (frame != fOldFrame)
			{
				fDoc->fDirty = true;
				fSelectedPhrase->fStartFrame += frame - fOldFrame;
				fSelectedPhrase->fEndFrame += frame - fOldFrame;
				if (fSelectedPhrase->fEndFrame < fSelectedPhrase->fStartFrame + 1)
					fSelectedPhrase->fEndFrame = fSelectedPhrase->fStartFrame + 1;
				fDoc->fCurrentVoice->RepositionPhrase(fSelectedPhrase, fDoc->Duration());
				needUpdate = true;
			}
		}
	}
	else if (fSelectedWord)
	{
		if (fDraggingEnd == 0)
		{
			if (frame != fSelectedWord->fStartFrame)
			{
				fDoc->fDirty = true;
				fSelectedWord->fStartFrame = frame;
				if (fSelectedWord->fStartFrame > fSelectedWord->fEndFrame - 1)
					fSelectedWord->fStartFrame = fSelectedWord->fEndFrame - 1;
				fParentPhrase->RepositionWord(fSelectedWord);
				needUpdate = true;
			}
		}
		else if (fDraggingEnd == 1)
		{
			if (frame != fSelectedWord->fEndFrame)
			{
				fDoc->fDirty = true;
				fSelectedWord->fEndFrame = frame;
				if (fSelectedWord->fEndFrame < fSelectedWord->fStartFrame + 1)
					fSelectedWord->fEndFrame = fSelectedWord->fStartFrame + 1;
				fParentPhrase->RepositionWord(fSelectedWord);
				needUpdate = true;
			}
		}
		else if (fDraggingEnd == 2)
		{
			if (frame != fOldFrame)
			{
				fDoc->fDirty = true;
				fSelectedWord->fStartFrame += frame - fOldFrame;
				fSelectedWord->fEndFrame += frame - fOldFrame;
				if (fSelectedWord->fEndFrame < fSelectedWord->fStartFrame + 1)
					fSelectedWord->fEndFrame = fSelectedWord->fStartFrame + 1;
				fParentPhrase->RepositionWord(fSelectedWord);
				needUpdate = true;
			}
		}
	}
	else if (fSelectedPhoneme)
	{
		if (fDraggingEnd == 0)
		{
			if (frame != fSelectedPhoneme->fFrame)
			{
				fDoc->fDirty = true;
				fSelectedPhoneme->fFrame = frame;
				fParentWord->RepositionPhoneme(fSelectedPhoneme);
				needUpdate = true;
			}
		}
	}

	fOldFrame = frame;

	QMediaPlayer *audioPlayer = fDoc->GetAudioPlayer();
	if (frame != fScrubFrame)
	{
		fScrubFrame = frame;
		fCurFrame = fScrubFrame;
		f = ((real)fScrubFrame / (real)fDoc->Fps()) * 1000.0f;
		audioPlayer->setPosition(PG_FLOOR(f));
		audioPlayer->play();
		emit(frameChanged(fScrubFrame));
		needUpdate = true;
	}
	if (needUpdate)
		update();
}

void WaveformView::mouseReleaseEvent(QMouseEvent *event)
{
	if (fDoc && fDoc->GetAudioPlayer() && fAudioStopFrame < 0)
		fDoc->GetAudioPlayer()->stop();
	if (event->button() == Qt::RightButton && fSelectedWord)
	{
		// manually enter the pronunciation for this word
		BreakdownDialog *dlog = new BreakdownDialog(fSelectedWord, this);
		if (dlog->exec() == QDialog::Accepted)
		{
			fDoc->fDirty = true;
			while (!fSelectedWord->fPhonemes.isEmpty())
				delete fSelectedWord->fPhonemes.takeFirst();
			QStringList phList = dlog->PhonemeString().split(' ', QString::SkipEmptyParts);
			for (int i = 0; i < phList.size(); i++)
			{
				QString phStr = phList.at(i);
				if (phStr.isEmpty())
					continue;
				LipsyncPhoneme *phoneme = new LipsyncPhoneme;
				phoneme->fText = phStr;
				fSelectedWord->fPhonemes << phoneme;
			}
			if (fParentPhrase)
				fParentPhrase->RepositionWord(fSelectedWord);
			update();
		}
		delete dlog;
	}
	fScrubFrame = -1;
	fCurFrame = -1;
	fDragging = false;
	fDraggingEnd = -1;
	fSelectedPhrase = NULL;
	fSelectedWord = NULL;
	fSelectedPhoneme = NULL;
	emit(frameChanged(0));
	update();
}

void WaveformView::paintEvent(QPaintEvent *event)
{
	PG_UNUSED(event);

	QPainter	dc(this);

	int32		clientWidth = width();
	int32		clientHeight = height();

	if (fDoc == NULL)
	{
		dc.drawText(QRect(0, 0, clientWidth, clientHeight), Qt::AlignHCenter | Qt::AlignVCenter, tr("Drop WAV audio file here"));
		return;
	}

	int32		topBorder = 16; // should be the height of frame label text
	int32		halfClientHeight;
	int32		sampleHeight, halfSampleHeight, textHeight;
	int32		fps = fDoc->Fps();
	int32		x = 0;
	int32		frameX;
	int32		sample = 0;
	int32		frame = 0;
	bool		drawPlayMarker = false;
	QRect		r;
	QColor		textCol(64, 64, 64);
	QColor		sampleFillCol(162, 205, 242);
	QColor		sampleOutlineCol(30, 121, 198);
	QColor		playBackCol(255, 127, 127);
	QColor		playForeCol(209, 102, 121, 128);
	QColor		playOutlineCol(128, 0, 0);
	QColor		frameCol(192, 192, 192);
	QColor		phraseFillCol(205, 242, 162);
	QColor		phraseOutlineCol(121, 198, 30);
	QColor		wordFillCol(242, 205, 162);
	QColor		wordOutlineCol(198, 121, 30);
	QColor		wordMissingFillCol(255, 127, 127);
	QColor		wordMissingOutlineCol(255, 0, 0);
	QColor		phonemeFillCol(231, 185, 210);
	QColor		phonemeOutlineCol(173, 114, 146);
	QMediaPlayer	*audioPlayer = fDoc->GetAudioPlayer();

	textHeight = dc.fontMetrics().height() + 4;
	topBorder = textHeight;
	halfClientHeight = (clientHeight - textHeight) / 2;

	if (audioPlayer && audioPlayer->state() == QMediaPlayer::PlayingState)
	{
		drawPlayMarker = true;
		x = fCurFrame * fFrameWidth;
		dc.fillRect(QRect(x, 0, fFrameWidth, clientHeight), playBackCol);
	}

	x = 0;
	for (int32 i = 0; i < fNumSamples; i++)
	{
		if (((sample + 1) % fSamplesPerFrame) == 0)
		{
			dc.setPen(frameCol);
			// draw frame marker
			frameX = (frame + 1) * fFrameWidth;
			if (fSampleWidth >= 2 && ((fFrameWidth > 2) || ((frame + 2) % fps == 0)))
			{
				dc.drawLine(frameX, topBorder, frameX, clientHeight);
			}
			// draw frame label
			if ((fFrameWidth > 30) || ((frame + 2) % fps == 0))
			{
				dc.drawLine(frameX, 0, frameX, topBorder);
				dc.drawText(frameX + 2, textHeight - 4, QString::number(frame + 2));
			}
		}

		sampleHeight = PG_ROUND(fAmp[i] * (real)(clientHeight - topBorder));
		halfSampleHeight = sampleHeight / 2;
		r.setRect(x, topBorder + halfClientHeight - halfSampleHeight, fSampleWidth + 1, sampleHeight);
		dc.fillRect(r, sampleFillCol);
		dc.setPen(sampleOutlineCol);
		dc.drawLine(r.topLeft(), r.topRight());
		dc.drawLine(r.bottomLeft(), r.bottomRight());
		dc.drawLine(r.topRight(), r.bottomRight());
		if (i == 0)
		{
			dc.drawLine(r.topLeft(), r.bottomLeft());
		}
		else if (fAmp[i] > fAmp[i - 1])
		{
			sampleHeight = PG_ROUND(fAmp[i - 1] * (real)(clientHeight - topBorder));
			halfSampleHeight = sampleHeight / 2;
			dc.drawLine(r.topLeft(), QPoint(r.left(), topBorder + halfClientHeight - halfSampleHeight));
			dc.drawLine(r.bottomLeft(), QPoint(r.left(), topBorder + halfClientHeight - halfSampleHeight + sampleHeight - 1));
		}

		x += fSampleWidth;
		sample++;
		if ((sample % fSamplesPerFrame) == 0)
			frame++;
	}

	if (fDoc->fCurrentVoice)
	{
		topBorder += 4;
		for (int32 p = 0; p < fDoc->fCurrentVoice->fPhrases.size(); p++)
		{
			LipsyncPhrase *phrase = fDoc->fCurrentVoice->fPhrases[p];
			r = QRect(phrase->fStartFrame * fFrameWidth, topBorder, (phrase->fEndFrame - phrase->fStartFrame + 1) * fFrameWidth, textHeight);
			phrase->fTop = r.top();
			phrase->fBottom = r.bottom();
			dc.fillRect(r, phraseFillCol);
			dc.setPen(phraseOutlineCol);
			dc.drawRect(r);
			dc.setClipRect(r);
			dc.setPen(textCol);
			r = r.marginsRemoved(QMargins(2, 2, 2, 2));
			dc.drawText(QPoint(r.left(), r.bottom() - 2), phrase->fText);
			dc.setClipping(false);

			for (int32 w = 0; w < phrase->fWords.size(); w++)
			{
				LipsyncWord *word = phrase->fWords[w];
				r = QRect(word->fStartFrame * fFrameWidth, topBorder + 4 + textHeight, (word->fEndFrame - word->fStartFrame + 1) * fFrameWidth, textHeight);
				if (w & 1)
					r.translate(0, textHeight - textHeight / 4);
				word->fTop = r.top();
				word->fBottom = r.bottom();
				if (word->fPhonemes.size() == 0)
				{
					dc.fillRect(r, wordMissingFillCol);
					dc.setPen(wordMissingOutlineCol);
				}
				else
				{
					dc.fillRect(r, wordFillCol);
					dc.setPen(wordOutlineCol);
				}
				dc.drawRect(r);
				dc.setClipRect(r);
				dc.setPen(textCol);
				r = r.marginsRemoved(QMargins(2, 2, 2, 2));
				dc.drawText(QPoint(r.left(), r.bottom() - 2), word->fText);
				dc.setClipping(false);

				for (int32 i = 0; i < word->fPhonemes.size(); i++)
				{
					LipsyncPhoneme *phoneme = word->fPhonemes[i];
					r = QRect(phoneme->fFrame * fFrameWidth, clientHeight - 4 - textHeight, fFrameWidth, textHeight);
					if (i & 1)
						r.translate(0, -(textHeight - textHeight / 4));
					phoneme->fTop = r.top();
					phoneme->fBottom = r.bottom();
					dc.fillRect(r, phonemeFillCol);
					dc.setPen(phonemeOutlineCol);
					dc.drawRect(r);
					dc.setPen(textCol);
					r = r.marginsRemoved(QMargins(2, 2, 2, 2));
					dc.drawText(QPoint(r.left(), r.bottom() - 2), phoneme->fText);
				} // for i
			} // for w
		} // for p
	}

	if (drawPlayMarker)
	{
		x = fCurFrame * fFrameWidth;
		dc.fillRect(QRect(x, 0, fFrameWidth, clientHeight), playForeCol);
		dc.setPen(playOutlineCol);
		dc.drawRect(QRect(x, 0, fFrameWidth, clientHeight));
	}
}
