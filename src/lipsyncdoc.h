#ifndef LIPSYNCDOC_H
#define LIPSYNCDOC_H

#include <QMediaPlayer>

#include "audioextractor.h"

class QFile;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class LipsyncPhoneme
{
public:
	LipsyncPhoneme();
	~LipsyncPhoneme();

	QString		fText;
	int32		fFrame;
	int32		fTop, fBottom;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class LipsyncWord
{
public:
	LipsyncWord();
	~LipsyncWord();

	void RunBreakdown(QString language);
	void RepositionPhoneme(LipsyncPhoneme *phoneme);

	QString					fText;
	int32					fStartFrame, fEndFrame;
	int32					fTop, fBottom;
	QList<LipsyncPhoneme *>	fPhonemes;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class LipsyncPhrase
{
public:
	LipsyncPhrase();
	~LipsyncPhrase();

	void RunBreakdown(QString language);
	void RepositionWord(LipsyncWord *word);

	QString					fText;
	int32					fStartFrame, fEndFrame;
	int32					fTop, fBottom;
	QList<LipsyncWord *>	fWords;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class LipsyncVoice
{
public:
	LipsyncVoice(const QString &name);
	~LipsyncVoice();

	void Open(QTextStream &in);
	void Save(QTextStream &out);
	void Export(QString path);
	void RunBreakdown(QString language, int32 audioDuration);
	void RepositionPhrase(LipsyncPhrase *phrase, int32 audioDuration);
	QString GetPhonemeAtFrame(int32 frame);

	QString					fName;
	QString					fText;
	QList<LipsyncPhrase *>	fPhrases;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class LipsyncDoc : public QObject
{
	Q_OBJECT
public:
	LipsyncDoc();
	~LipsyncDoc();

	static void LoadDictionaries();

	void Open(const QString &path);
	void OpenAudio(const QString &path);
	void Save();
	void RebuildAudioSamples();

	int32 Fps() { return fFps; }
	void SetFps(int32 fps);
	QMediaPlayer *GetAudioPlayer();
	AudioExtractor *GetAudioExtractor();
	int32 Duration() { return fAudioDuration; }
	QString GetVolumePhonemeAtFrame(int32 frame);

private slots:

private:
	static void LoadDictionary(QFile *f);

	int32					fFps, fAudioDuration;
	QString					fAudioPath;
	QMediaPlayer			*fAudioPlayer;
	AudioExtractor			*fAudioExtractor;
	real					fMaxAmplitude;

public:
	QString					fPath;
	bool					fDirty;
	QList<LipsyncVoice *>	fVoices;
	LipsyncVoice			*fCurrentVoice;

	static QList<QString>				Phonemes;
	static QHash<QString, QString>		DictionaryToPhonemeMap;
	static QHash<QString, QStringList>	PhonemeDictionary;

	/*
	 * I would have preferred to use a QAudioDecoder object, but it doesn't seem to actually be implemented (at least on Mac).
	*/
};

#endif // LIPSYNCDOC_H
