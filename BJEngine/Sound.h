#pragma once 
#include "pch.h"

namespace BJAudio 
{


	class Sound
	{
		friend class Sound3D;
	public:

		Sound(std::wstring fileName);
		~Sound() = default;

		void Close();
		bool Init();
		bool Play();
	
	private:
		IXAudio2* pXAudio2 = nullptr;
		IXAudio2MasteringVoice* pMasterVoice = nullptr;
		WAVEFORMATEXTENSIBLE wfx = { 0 };
		XAUDIO2_BUFFER buffer = { 0 };
		IXAudio2SourceVoice* pSourceVoice;

		bool isStarted;
		std::wstring fileName;

		HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition);
		HRESULT ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset);

	};


}