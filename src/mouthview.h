#ifndef MOUTHVIEW_H
#define MOUTHVIEW_H

#include <QWidget>

#include "lipsyncdoc.h"

class MouthView : public QWidget
{
	Q_OBJECT
public:
	explicit MouthView(QWidget *parent = 0);
	~MouthView();

	void SetDocument(LipsyncDoc *doc);
	void SetMouth(int32 id);

signals:

public slots:
	void onMouthChanged(int id);
	void onFrameChanged(int frame);

protected:
	void paintEvent(QPaintEvent *event);

private:
	LipsyncDoc					*fDoc;
	int32						fMouthID;
	int32						fFrame;
	QHash<QString, QImage *>	fMouths[4];
};

#endif // MOUTHVIEW_H
