#ifndef WAVEFORMVIEW_H
#define WAVEFORMVIEW_H

#include <QWidget>

#include "lipsyncdoc.h"

class QScrollArea;

class WaveformView : public QWidget
{
	Q_OBJECT
public:
	explicit WaveformView(QWidget *parent = 0);
	~WaveformView();
	QSize sizeHint() const;

	void SetScrollArea(QScrollArea *scrollArea);
	void SetDocument(LipsyncDoc *doc);

signals:
	void frameChanged(int);

public slots:
	void onZoomIn();
	void onZoomOut();
	void onAutoZoom();
	void positionChanged(qint64 milliseconds);

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void paintEvent(QPaintEvent *event);

private:
	QScrollArea	*fScrollArea;
	LipsyncDoc	*fDoc;
	int32		fNumSamples;
	real		*fAmp;
	bool		fDragging, fDoubleClick;
	int32		fDraggingEnd;
	int32		fCurFrame;
	int32		fOldFrame;
	int32		fScrubFrame;
	int32		fAudioStopFrame;
	int32		fSampleWidth;
	int32		fSamplesPerFrame;
	int32		fSamplesPerSec;
	int32		fFrameWidth;
	int32		fPhraseBottom;
	int32		fWordBottom;
	int32		fPhonemeTop;

	LipsyncPhrase	*fSelectedPhrase, *fParentPhrase;
	LipsyncWord		*fSelectedWord, *fParentWord;
	LipsyncPhoneme	*fSelectedPhoneme;
};

#endif // WAVEFORMVIEW_H
