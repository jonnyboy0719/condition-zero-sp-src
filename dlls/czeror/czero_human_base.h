#ifndef SERVER_CZERO_HUMAN_BASE_H
#define SERVER_CZERO_HUMAN_BASE_H

#include "squadmonster.h"
#include "talkmonster.h"
#include "barney.h"
#include "hgrunt.h"
#include <string>

extern int g_fGruntQuestion;
extern Vector _tinyhull;

enum CSDSLanguage
{
	LANG_DEF = 0,
	LANG_AM,
	LANG_BR,
	LANG_FR,
	LANG_GE,
	LANG_JA,
	LANG_RU,
	LANG_AR,
	LANG_SP
};

enum CSDSTeamVoice
{
	VOICE_CT = 0,
	VOICE_T
};

enum CSDSTeams
{
	TEAM_CT = 0,		// Player ally (or player)
	TEAM_CIV,			// Civillian, ignored
	TEAM_TERROR_RUSSIAN,	// Terror Team 1 (terror_russian)
	TEAM_TERROR_ARTIC,		// Terror Team 2 (terror_artic)
};

namespace CZero
{
	const char *GetVoiceLang( const char *szSentence, CSDSLanguage lang, CSDSTeamVoice team );
	CSDSTeams GetEnemyTeam(CBaseEntity* pEnemy);
	int GetTeamRelationShip(CBaseEntity *me, CBaseEntity *target);
	void OnHit( edict_t *ent, bool headshot );
}

class CZeroHumanBase_Pistol : public CBarney
{
public:
	bool Save(CSave& save) override;
	bool Restore(CRestore& restore) override;
	static TYPEDESCRIPTION m_SaveData[];
	void TalkInit();
	int IRelationship(CBaseEntity* pTarget) override;
	void OnSentenceSay(BarneySay say) override;
	CSDSLanguage m_VoiceLang;
	CSDSTeamVoice m_TeamVoice;
};

class CZeroHumanBase : public CHGrunt
{
public:
	bool Save(CSave& save) override;
	bool Restore(CRestore& restore) override;
	static TYPEDESCRIPTION m_SaveData[];
	int IRelationship(CBaseEntity* pTarget) override;
	void DeathSound();
	void PainSound();
	void IdleSound();
	void ThrowSentence();
	void SpeakSentence();
	void HandleAnimEvent(MonsterEvent_t* pEvent) override;
	static const char* CZeroGruntSentences[];
	CSDSLanguage m_VoiceLang;
	CSDSTeamVoice m_TeamVoice;
};

#endif