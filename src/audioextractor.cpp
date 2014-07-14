#include "audioextractor.h"

#define MAX_AUDIO_FRAMES	53000000

AudioExtractor::AudioExtractor(const char *path, bool reverse)
{
	fNumFrames = 0;
	fNumSamples = 0;
	fSamples = NULL;
	fSourcePath = path;

	if (path == NULL)
		return;
	
	// The sound file might already be in the right format (AIFF or WAV), try and read it. 
    bool soundRead = ReadSoundFile(path);
    
	if (soundRead && reverse)
	{
		int32	revI;
		float	temp;

		for (int32 i = 0; i < fSndInfo.frames / 2; i++)
		{
			revI = (int32)fSndInfo.frames - 1 - i;
			for (int32 j = 0; j < fSndInfo.channels; j++)
			{
				temp = fSamples[i * fSndInfo.channels + j];
				fSamples[i * fSndInfo.channels + j] = fSamples[revI * fSndInfo.channels + j];
				fSamples[revI * fSndInfo.channels + j] = temp;
			}
		}
	}
}

AudioExtractor::~AudioExtractor()
{
	if (fSamples)
		delete [] fSamples;
}

bool AudioExtractor::IsValid() const
{
	if (fSamples == NULL)
		return false;
	return true;
}

real AudioExtractor::Duration() const
{
	if (fSamples == NULL)
		return 0.0f;

	return (real)fSndInfo.frames / (real)fSndInfo.samplerate;
}

real AudioExtractor::GetAmplitude(real startTime, real duration) const
{
	if (fSamples == NULL || duration < 0.0f)
		return 0.0f;

	uint32	i, start, end;
	real	sample, total = 0.0f;

	start = TimeToSample(startTime, true);
	end = TimeToSample(startTime + duration, true);
	if (end == start)
		return 0.0f;
	for (i = start; i < end; i++)
	{
		sample = PG_FABS(fSamples[i]);
		if (sample > 1.001f)
			continue;
		total += sample;
	}
	total = total / (real)(end - start);
	return total;
}

real AudioExtractor::GetRMSAmplitude(real startTime, real duration) const
{
	if (fSamples == NULL || duration < 0.0f)
		return 0.0f;

	uint32	i, start, end;
	real	sample, total = 0.0f;

	start = TimeToSample(startTime, true);
	end = TimeToSample(startTime + duration, true);
	if (end == start)
		return 0.0f;
	for (i = start; i < end; i++)
	{
		sample = PG_FABS(fSamples[i]);
		if (sample > 1.001f)
			continue;
		total += sample * sample;
	}
	total = total / (real)(end - start);
	return (real)sqrt(total);
}

real AudioExtractor::GetMaxAmplitude(real startTime, real duration) const
{
	if (fSamples == NULL || duration < 0.0f)
		return 0.0f;

	uint32	i, start, end;
	real	sample, max = -PG_HUGE;

	start = TimeToSample(startTime, true);
	end = TimeToSample(startTime + duration, true);
	if (end == start)
		return 0.0f;
	for (i = start; i < end; i++)
	{
		sample = PG_FABS(fSamples[i]);
		if (sample > 1.001f)
			continue;
		max = PG_MAX(max, sample);
	}
	return max;
}

uint32 AudioExtractor::NumSamples() const
{
	return fNumSamples;
}

int32 AudioExtractor::SampleRate() const
{
	return fSndInfo.samplerate;
}

real *AudioExtractor::Buffer() const
{
	return fSamples;
}

uint32 AudioExtractor::TimeToSample(real time, bool clamped) const
{
	if (fSamples == NULL)
		return 0;

	uint32	sample;

	time = time * (real)(fSndInfo.samplerate * fSndInfo.channels);
	sample = PG_ROUND(time);
	if (fSndInfo.channels)
	{
		while (sample % fSndInfo.channels)
			sample--;
	}
	if (clamped)
		return PG_CLAMP(sample, 0, fNumSamples - 1);
	else
		return sample;
}

bool AudioExtractor::ReadSoundFile(const char *soundFilePath)
{
    SNDFILE *sndFile = sf_open(soundFilePath, SFM_READ, &fSndInfo);
    if (!sndFile)
        return false;
    
    if (fSndInfo.frames > MAX_AUDIO_FRAMES)
        fSndInfo.frames = MAX_AUDIO_FRAMES;
    fNumSamples = (int32)(fSndInfo.frames * fSndInfo.channels);
    fSamples = new float[fNumSamples];
/*	if (sndFormat == (SF_FORMAT_OGG | SF_FORMAT_VORBIS))
	{
		float		*bufPtr = fSamples;
		sf_count_t	sampleCount = fNumSamples;
		sf_count_t	chunkSize = 65536;
		sf_count_t	readCount;
		
		while (sampleCount > chunkSize)
		{
			readCount = sf_read_float(sndFile, bufPtr, chunkSize);
			if (readCount == 0)
				break;
			bufPtr += readCount;
			sampleCount -= readCount;
		}
		if (sampleCount > 0)
			sf_read_float(sndFile, bufPtr, chunkSize);
		fNumFrames = fSndInfo.frames;
	}
	else*/
	{
		fNumFrames = sf_readf_float(sndFile, fSamples, fSndInfo.frames);
	}
    sf_close(sndFile);
    return true;
}
