#ifndef AUDIO_H
#define AUDIO_H

#include <string>
#include <memory>
#include <irrKlang.h>

class StreamingMusic
{
    public:
    StreamingMusic();
    ~StreamingMusic();

    void SetSongName(const char* songName);
    void StartFetchAudio(int musicSize);
    void FetchAudioToMemory(char* musicData, unsigned int musicDataSize);
    bool ReadyToPlayMusic() const;
    void StartMusic();
    void StopMusic();
    void PauseMusic();
    void ResumeMusic();
    void ForwardMusic();
    void BackwardMusic();
    void InitAudio();
    void KillAudio();
    
    private:
    irrklang::ISound*                   m_music; // Handle to music resource
    std::unique_ptr<irrklang::ik_c8[]>  m_memorySoundData; // Memory area where we fill up with audio
    irrklang::ik_s32                    m_memorySoundDataSize; // Total size of the audio file we will stream
    irrklang::ik_s32                    m_memorySoundCurrentPos; // Current position (amount we have already downloaded)
    bool                                m_memorySoundDataCreated;
    std::string                         m_memorySongName; // irrklang needs to know what type of format we got
};



#endif