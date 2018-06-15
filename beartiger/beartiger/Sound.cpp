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
		"Sound/handclap_ingame_vol.mp3"
		, FMOD_2D | FMOD_LOOP_NORMAL
		, nullptr
		, &pSound[0]
	); // ����� ����

	pSystem->createSound(
		"Sound/jppark_opning.wav"
		, FMOD_DEFAULT | FMOD_LOOP_OFF
		, nullptr
		, &pSound[1]
	); //������ ���

	pSystem->createSound(
		"Sound/punch_cut.mp3"
		, FMOD_DEFAULT | FMOD_LOOP_OFF
		, nullptr
		, &pSound[2]
	); //����Ʈ(���� ��)

	pSystem->createSound(
		"Sound/tiger_cut.mp3"
		, FMOD_DEFAULT | FMOD_LOOP_OFF
		, nullptr
		, &pSound[3]
	); //����Ʈ,���� ������ ��

	pSystem->createSound(
		"Sound/ending_cut.mp3"
		, FMOD_DEFAULT | FMOD_LOOP_OFF
		, nullptr
		, &pSound[4]
	); //����ȭ�� ���

	pSystem->createSound(
		"Sound/garlic_save.wav"
		, FMOD_DEFAULT | FMOD_LOOP_OFF
		, nullptr
		, &pSound[5]
	); //���� ���� �� ȿ����

	pSystem->createSound(
		"Sound/badending_cut.mp3"
		, FMOD_DEFAULT | FMOD_LOOP_OFF
		, nullptr
		, &pSound[6]
	); //���� ���� �� ȿ����


}

void CSound::Play(int n)
{
	pSystem->update();

	pSystem->playSound(pSound[n]
		, nullptr, false, &pChannel[n]);

}
