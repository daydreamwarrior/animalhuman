#include "stdafx.h"
#include "Sound.h"


CSound::CSound()
{
	FMOD::System_Create(&pSystem);
	pSystem->init(
		SOUND_NUM
		, FMOD_INIT_NORMAL
		, nullptr
	);
	for (auto& p : pSound) p = nullptr;
}


CSound::~CSound()
{
	for (int i = 0; i < SOUND_NUM; ++i)
	{
		pSound[i]->release();
	}

	pSystem->release();
	pSystem->close();
}

void CSound::Add_sound()
{
	pSystem->createStream(
		"Sound/Electrodoodle.mp3"
		, FMOD_2D | FMOD_LOOP_NORMAL
		, nullptr
		, &pSound[0]
	); // 배경음 사운드

	pSystem->createSound(
		"Sound/Skill1.wav"
		, FMOD_DEFAULT | FMOD_LOOP_OFF
		, nullptr
		, &pSound[1]
	); //이펙트

	pSystem->createSound(
		"Sound/punch_cut.mp3"
		, FMOD_DEFAULT | FMOD_LOOP_OFF
		, nullptr
		, &pSound[2]
	); //이펙트(맞을 때)

	pSystem->createSound(
		"Sound/tiger_cut.mp3"
		, FMOD_DEFAULT | FMOD_LOOP_OFF
		, nullptr
		, &pSound[3]
	); //이펙트, 공격 성공할 때

}

void CSound::Play(int n)
{
	pSystem->update();

	pSystem->playSound(pSound[n]
		, nullptr, false, &pChannel[n]);

}
