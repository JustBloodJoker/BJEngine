#include "Sound.h"

namespace BJAudio
{

    Sound::Sound(std::wstring fileName)
    {
        isStarted = false;
        this->fileName = fileName;



    }

    void Sound::Close()
    {
        RELEASE(pXAudio2);
        pMasterVoice->DestroyVoice();
        pSourceVoice->DestroyVoice();
    }

    bool Sound::Init()
    {
        HRESULT hr;
        hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        if (FAILED(hr))
            return hr;

        if (FAILED(hr = XAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR)))
            return hr;

        if (FAILED(hr = pXAudio2->CreateMasteringVoice(&pMasterVoice)))
            return hr;

        HANDLE hFile = CreateFile(
            fileName.c_str(),
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            0,
            NULL);

        if (INVALID_HANDLE_VALUE == hFile)
            return HRESULT_FROM_WIN32(GetLastError());

        if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
            return HRESULT_FROM_WIN32(GetLastError());

        DWORD dwChunkSize;
        DWORD dwChunkPosition;
        FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition);
        DWORD filetype;
        ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);
        if (filetype != fourccWAVE)
            return S_FALSE;

        FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);
        ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);

        FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
        BYTE* pDataBuffer = new BYTE[dwChunkSize];
        ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);

        buffer.AudioBytes = dwChunkSize;
        buffer.pAudioData = pDataBuffer;
        buffer.Flags = XAUDIO2_END_OF_STREAM;
    }

    bool Sound::Play()
    {
       
        if (!isStarted)
        {
            HRESULT hr;

            hr = pXAudio2->CreateSourceVoice(&pSourceVoice, (WAVEFORMATEX*)&wfx);
            if (FAILED(hr)) return false;

            hr = pSourceVoice->SubmitSourceBuffer(&buffer);
            if (FAILED(hr))
                return false;

            hr = pSourceVoice->Start(0);
            if (FAILED(hr))
                return false;
            isStarted = true;
        }
        std::string temp(fileName.begin(), fileName.end());
        
        static float volume = 0.1f;
        ImGui::Begin(temp.c_str());
        ImGui::SliderFloat("Volume", &volume, 0.001f, 1.5f);
        ImGui::End();

        if (BJEngine::PackMananger::Get()->GetSavingStatus())
        {
            SoundType templd;
            templd.path = temp;
            templd.volume = volume;
            BJEngine::PackMananger::Get()->AddSound(templd);
        }

        pSourceVoice->SetVolume(volume);

        return true;
    }

    HRESULT Sound::FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition)
    {
        HRESULT hr = S_OK;
        if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
            return HRESULT_FROM_WIN32(GetLastError());
        DWORD dwChunkType;
        DWORD dwChunkDataSize;
        DWORD dwRIFFDataSize = 0;
        DWORD dwFileType;
        DWORD bytesRead = 0;
        DWORD dwOffset = 0;
        while (hr == S_OK)
        {
            DWORD dwRead;
            if (0 == ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
                hr = HRESULT_FROM_WIN32(GetLastError());
            if (0 == ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
                hr = HRESULT_FROM_WIN32(GetLastError());
            switch (dwChunkType)
            {
            case fourccRIFF:
                dwRIFFDataSize = dwChunkDataSize;
                dwChunkDataSize = 4;
                if (0 == ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL))
                    hr = HRESULT_FROM_WIN32(GetLastError());
                break;
            default:
                if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT))
                    return HRESULT_FROM_WIN32(GetLastError());
            }
            dwOffset += sizeof(DWORD) * 2;
            if (dwChunkType == fourcc)
            {
                dwChunkSize = dwChunkDataSize;
                dwChunkDataPosition = dwOffset;
                return S_OK;
            }
            dwOffset += dwChunkDataSize;
            if (bytesRead >= dwRIFFDataSize) return S_FALSE;
        }
        return S_OK;
    }

    HRESULT Sound::ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset)
    {
        HRESULT hr = S_OK;
        if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
            return HRESULT_FROM_WIN32(GetLastError());
        DWORD dwRead;
        if (0 == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
            hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;
    }

}