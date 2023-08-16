#pragma once

#include "pch.h"

namespace BJAudio {

	/// 
	/// RECREATE
	/// 
	class Sound
	{
		friend class Sound3D;
	public:
		Sound();
		~Sound() = default;

		void Close();

		bool InitAndPlay(const TCHAR* name);
		void Repeat(bool isRep);
	private:
		bool isStarted;

		HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition);
		HRESULT ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset);
		
	};



}