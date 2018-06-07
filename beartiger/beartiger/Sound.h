#pragma once
#include "FMOD\include\fmod.hpp"

#define SOUND_NUM 3

#pragma comment(lib, "FMOD/lib/fmod64_vc.lib")

using namespace FMOD;


class CSound
{
public:
	CSound();
	virtual ~CSound();
	System * pSystem;
	Sound * pSound[SOUND_NUM];
	Channel * pChannel[SOUND_NUM];

	void Add_sound();
	void Play(int n);
};

