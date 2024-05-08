#include "audio.h"
#include <iostream>
#include <cstring>

using namespace std;
using namespace irrklang;

static ISoundEngine* s_engine;

//----------------------------------------------------------------------
void StreamingMusic::InitAudio()
{
	// start the sound engine with default parameters
	s_engine = createIrrKlangDevice();

	if (!s_engine)
	{
		cout << " ERROR ! Could not init audio" << endl;
	}
}
//----------------------------------------------------------------------
void StreamingMusic::KillAudio()
{
    s_engine->drop(); // delete engine
}
//----------------------------------------------------------------------
StreamingMusic::StreamingMusic()
{
    m_music = nullptr;
    m_memorySoundData = nullptr;
    m_memorySoundDataSize = 0;
    m_memorySoundDataCreated = false;
}
//----------------------------------------------------------------------
StreamingMusic::~StreamingMusic()
{
}
//----------------------------------------------------------------------
void StreamingMusic::SetSongName(const char* songName)
{
    m_memorySongName = songName;
}
//----------------------------------------------------------------------
void StreamingMusic::StartFetchAudio(int musicSize)
{
    m_memorySoundDataCreated = false;
    m_memorySoundDataSize = musicSize;
    m_memorySoundData = std::make_unique<irrklang::ik_c8[]>(m_memorySoundDataSize);
    m_memorySoundCurrentPos = 0;
    cout<<" CLIENT : Fetched Audio"<<endl;
}
//----------------------------------------------------------------------
bool StreamingMusic::ReadyToPlayMusic() const
{
    return m_memorySoundCurrentPos > 500000; // 500kb
}
//----------------------------------------------------------------------
void StreamingMusic::FetchAudioToMemory(char* musicData, unsigned int musicDataSize)
{
    memcpy(m_memorySoundData.get() + m_memorySoundCurrentPos, musicData, musicDataSize);
    m_memorySoundCurrentPos += musicDataSize;

    if (!m_memorySoundDataCreated)
    {
        cout << " CLIENT : Adding sound source " << m_memorySongName.c_str() << endl;
        ISoundSource* source = s_engine->addSoundSourceFromMemory(
            m_memorySoundData.get(),
            m_memorySoundDataSize,
            m_memorySongName.c_str(),
            false
        );

        // Set stream mode and threshold
        if (source)
        {
            if (!m_music)
            {
                source->setStreamMode(ESM_STREAMING);
                source->setForcedStreamingThreshold(-1);
            }
            else
            {
                source->setStreamMode(ESM_NO_STREAMING);
            }
        }
        

        m_memorySoundDataCreated = true;
    }
}

//----------------------------------------------------------------------
void StreamingMusic::StartMusic()
{
    cout << " CLIENT : Playing " << m_memorySongName.c_str() << endl;            
    m_music = s_engine->play2D(m_memorySongName.c_str(), true, false, false, ESM_STREAMING, true);
    m_music->setVolume(0.5f); // Personal preference here, I turn the volume down to 50%
}
//----------------------------------------------------------------------


void StreamingMusic::StopMusic()
{
    if(m_music)
    {
        cout << " CLIENT : Stoping music" << endl;
        s_engine->removeSoundSource(m_memorySongName.c_str());        
        m_music->drop();
    }

}
void StreamingMusic::PauseMusic()
{
    if (m_music)
    {
        cout << " CLIENT : Pausing music" << endl;
        m_music->setIsPaused(true);
    }
}

void StreamingMusic::ResumeMusic()
{
    if (m_music)
    {
        cout << " CLIENT : Resuming music" << endl;
        m_music->setIsPaused(false);
    }
}


void StreamingMusic::ForwardMusic()
{
    if (m_music)
    {
        // Get the current playback position
        int currentPosition = m_music->getPlayPosition();

        // Forward by 10 seconds (10000 milliseconds)
        int newPosition = currentPosition + 10000;

        // Set the new playback position
        m_music->setPlayPosition(newPosition);
    }
}
void StreamingMusic::BackwardMusic()
{
    if (m_music)
    {
        // Get the current playback position
        int currentPosition = m_music->getPlayPosition();

        // Forward by 10 seconds (10000 milliseconds)
        int newPosition = currentPosition - 10000;

        // Set the new playback position
        m_music->setPlayPosition(newPosition);
    }
}
//----------------------------------------------------------------------
