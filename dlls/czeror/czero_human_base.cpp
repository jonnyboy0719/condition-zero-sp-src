
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "czero_human_base.h"

// We don't want the yellow particles flying around us.
Vector _tinyhull(0.5f, 0.5f, 0.5f);

TYPEDESCRIPTION CZeroHumanBase_Pistol::m_SaveData[] =
{
	DEFINE_FIELD(CZeroHumanBase_Pistol, m_VoiceLang, FIELD_INTEGER),
	DEFINE_FIELD(CZeroHumanBase_Pistol, m_TeamVoice, FIELD_INTEGER),
};

IMPLEMENT_SAVERESTORE(CZeroHumanBase_Pistol, CBarney);


TYPEDESCRIPTION CZeroHumanBase::m_SaveData[] =
{
	DEFINE_FIELD(CZeroHumanBase, m_VoiceLang, FIELD_INTEGER),
	DEFINE_FIELD(CZeroHumanBase, m_TeamVoice, FIELD_INTEGER),
};

IMPLEMENT_SAVERESTORE(CZeroHumanBase, CHGrunt);

int CZeroHumanBase_Pistol::IRelationship(CBaseEntity* pTarget)
{
	return CZero::GetTeamRelationShip(this, pTarget);
}

void CZeroHumanBase_Pistol::TalkInit()
{
	CTalkMonster::TalkInit();

	// If not TEAM_CT, then go "grr, im angy" at the sight of the player
	if ( pev->team != TEAM_CT )
		Remember(bits_MEMORY_PROVOKED | bits_MEMORY_SUSPICIOUS);

	m_szGrp[TLK_ANSWER] = CZero::GetVoiceLang("ANSWER", m_VoiceLang, m_TeamVoice);
	m_szGrp[TLK_QUESTION] = CZero::GetVoiceLang("QUEST", m_VoiceLang, m_TeamVoice);
	m_szGrp[TLK_IDLE] = CZero::GetVoiceLang("IDLE", m_VoiceLang, m_TeamVoice);
	m_szGrp[TLK_STARE] = NULL;
	m_szGrp[TLK_USE] = NULL;
	m_szGrp[TLK_UNUSE] = NULL;
	m_szGrp[TLK_STOP] = NULL;

	m_szGrp[TLK_NOSHOOT] = NULL;
	m_szGrp[TLK_HELLO] = NULL;

	m_szGrp[TLK_PLHURT1] = NULL;
	m_szGrp[TLK_PLHURT2] = NULL;
	m_szGrp[TLK_PLHURT3] = NULL;

	m_szGrp[TLK_PHELLO] = NULL;
	m_szGrp[TLK_PIDLE] = NULL;
	m_szGrp[TLK_PQUESTION] = NULL;

	m_szGrp[TLK_SMELL] = NULL;

	m_szGrp[TLK_WOUND] = CZero::GetVoiceLang("WOUND", m_VoiceLang, m_TeamVoice);
	m_szGrp[TLK_MORTAL] = CZero::GetVoiceLang("WOUND", m_VoiceLang, m_TeamVoice);
}

void CZeroHumanBase_Pistol::OnSentenceSay( BarneySay say )
{
	switch ( say )
	{
	case BARNSAY_ATTACK:
		PlaySentence(CZero::GetVoiceLang("ALERT", m_VoiceLang, m_TeamVoice), RANDOM_FLOAT(2.8, 3.2), VOL_NORM, ATTN_IDLE);
		break;
	case BARNSAY_GRENADE:
		PlaySentence(CZero::GetVoiceLang("GREN", m_VoiceLang, m_TeamVoice), 4, VOL_NORM, ATTN_NORM);
		break;
	case BARNSAY_CANTSEE:
		PlaySentence(CZero::GetVoiceLang("FLASHED", m_VoiceLang, m_TeamVoice), 4, VOL_NORM, ATTN_NORM);
		break;
	case BARNSAY_SMOKED:
		PlaySentence(CZero::GetVoiceLang("INSMOKE", m_VoiceLang, m_TeamVoice), 4, VOL_NORM, ATTN_NORM);
		break;
	case BARNSAY_PAIN:
		PlaySentence(CZero::GetVoiceLang("WOUND", m_VoiceLang, m_TeamVoice), 4, VOL_NORM, ATTN_NORM); CZero::OnHit( ENT(pev), (m_LastHitGroup == HITGROUP_HEAD) );
		break;
	case BARNSAY_DIE:
		PlaySentence(CZero::GetVoiceLang("DEATH", m_VoiceLang, m_TeamVoice), 4, VOL_NORM, ATTN_NORM); CZero::OnHit( ENT(pev), (m_LastHitGroup == HITGROUP_HEAD) );
		break;
	}
}

int CZeroHumanBase::IRelationship(CBaseEntity* pTarget)
{
	return CZero::GetTeamRelationShip(this, pTarget);
}

void CZeroHumanBase::DeathSound()
{
	SENTENCEG_PlayRndSz(ENT(pev), CZero::GetVoiceLang("DEATH", m_VoiceLang, m_TeamVoice), HGRUNT_SENTENCE_VOLUME, ATTN_NORM, 0, m_voicePitch);
	CZero::OnHit( ENT(pev), (m_LastHitGroup == HITGROUP_HEAD) );
}

void CZeroHumanBase::PainSound()
{
	if (gpGlobals->time > m_flNextPainTime)
	{
		SENTENCEG_PlayRndSz(ENT(pev), CZero::GetVoiceLang("WOUND", m_VoiceLang, m_TeamVoice), HGRUNT_SENTENCE_VOLUME, ATTN_NORM, 0, m_voicePitch);
		m_flNextPainTime = gpGlobals->time + 1;
	}
	CZero::OnHit( ENT(pev), (m_LastHitGroup == HITGROUP_HEAD) );
}

void CZeroHumanBase::IdleSound()
{
	if (FOkToSpeak() && (0 != g_fGruntQuestion || RANDOM_LONG(0, 1)))
	{
		if (0 == g_fGruntQuestion)
		{
				// ask question or make statement
				switch (RANDOM_LONG(0, 2))
				{
				case 0: // check in
					SENTENCEG_PlayRndSz(ENT(pev), CZero::GetVoiceLang("CHECK", m_VoiceLang, m_TeamVoice), HGRUNT_SENTENCE_VOLUME, ATTN_NORM, 0, m_voicePitch);
					g_fGruntQuestion = 1;
					break;
				case 1: // question
					SENTENCEG_PlayRndSz(ENT(pev), CZero::GetVoiceLang("QUEST", m_VoiceLang, m_TeamVoice), HGRUNT_SENTENCE_VOLUME, ATTN_NORM, 0, m_voicePitch);
					g_fGruntQuestion = 2;
					break;
				case 2: // statement
					SENTENCEG_PlayRndSz(ENT(pev), CZero::GetVoiceLang("IDLE", m_VoiceLang, m_TeamVoice), HGRUNT_SENTENCE_VOLUME, ATTN_NORM, 0, m_voicePitch);
					break;
				}
		}
		else
		{
				switch (g_fGruntQuestion)
				{
				case 1: // check in
					SENTENCEG_PlayRndSz(ENT(pev), CZero::GetVoiceLang("CLEAR", m_VoiceLang, m_TeamVoice), HGRUNT_SENTENCE_VOLUME, ATTN_NORM, 0, m_voicePitch);
					break;
				case 2: // question
					SENTENCEG_PlayRndSz(ENT(pev), CZero::GetVoiceLang("ANSWER", m_VoiceLang, m_TeamVoice), HGRUNT_SENTENCE_VOLUME, ATTN_NORM, 0, m_voicePitch);
					break;
				}
				g_fGruntQuestion = 0;
		}
		JustSpoke();
	}
}

void CZeroHumanBase::ThrowSentence()
{
	SENTENCEG_PlayRndSz(ENT(pev), CZero::GetVoiceLang("THROW", m_VoiceLang, m_TeamVoice), HGRUNT_SENTENCE_VOLUME, GRUNT_ATTN, 0, m_voicePitch);
}

void CZeroHumanBase::SpeakSentence()
{
	if (m_iSentence == HGRUNT_SENT_NONE)
	{
		// no sentence cued up.
		return;
	}

	if (FOkToSpeak())
	{
		SENTENCEG_PlayRndSz(ENT(pev), CZero::GetVoiceLang(CZeroGruntSentences[m_iSentence], m_VoiceLang, m_TeamVoice), HGRUNT_SENTENCE_VOLUME, GRUNT_ATTN, 0, m_voicePitch);
		JustSpoke();
	}
}

void CZeroHumanBase::HandleAnimEvent( MonsterEvent_t* pEvent )
{
	switch (pEvent->event)
	{
		case HGRUNT_AE_CAUGHT_ENEMY:
		{
			if (FOkToSpeak())
			{
				SENTENCEG_PlayRndSz(ENT(pev), CZero::GetVoiceLang("ALERT", m_VoiceLang, m_TeamVoice), HGRUNT_SENTENCE_VOLUME, GRUNT_ATTN, 0, m_voicePitch);
				JustSpoke();
			}
		}
		break;

		default:
			CHGrunt::HandleAnimEvent(pEvent);
		break;
	}
}

const char* CZeroHumanBase::CZeroGruntSentences[] =
{
	"GREN",		// grenade scared grunt
	"ALERT",	// sees player
	"ENEMY",	// new enemy
	"ENEMY",	// running to cover
	"THROW",	// about to throw grenade
	"CHARGE",	// running out to get the enemy
	"TAUNT",	// say rude things
	"FLASHED",	// got flashbanged
	"INSMOKE",	// got caught in smoke
};

const char* CZero::GetVoiceLang(const char* szSentence, CSDSLanguage lang, CSDSTeamVoice team)
{
	static std::string value;
	value.clear();
	switch (team)
	{
		case VOICE_CT: value = "CT_"; break;
		case VOICE_T: value = "T_"; break;
	}
	value += szSentence;
	switch (lang)
	{
		case LANG_AM: value += "_AM"; break;
		case LANG_BR: value += "_BR"; break;
		case LANG_FR: value += "_FR"; break;
		case LANG_GE: value += "_GE"; break;
		case LANG_JA: value += "_JA"; break;
		case LANG_RU: value += "_RU"; break;
		case LANG_AR: value += "_AR"; break;
		case LANG_SP: value += "_SP"; break;
	}
	return value.c_str();
}

CSDSTeams CZero::GetEnemyTeam(CBaseEntity* pEnemy)
{
	if (FClassnameIs(pEnemy->pev, "player"))
			return TEAM_CT;
	return (CSDSTeams)pEnemy->pev->team;
}

int CZero::GetTeamRelationShip(CBaseEntity* me, CBaseEntity* target)
{
	CSDSTeams target_team = CZero::GetEnemyTeam(target);
	switch ( me->pev->team )
	{
		case TEAM_CIV:
		{
			if ( target_team == TEAM_TERROR_RUSSIAN || target_team == TEAM_TERROR_ARTIC )
				return R_FR;
			else if ( target_team == TEAM_CIV || target_team == TEAM_CT )
				return R_AL;
		}
		break;
		case TEAM_CT:
		{
			if ( target_team == TEAM_TERROR_RUSSIAN || target_team == TEAM_TERROR_ARTIC )
				return R_NM;
			else if ( target_team == TEAM_CIV || target_team == TEAM_CT )
				return R_AL;
		}
		break;
		case TEAM_TERROR_RUSSIAN:
		{
			if ( target_team == TEAM_TERROR_ARTIC || target_team == TEAM_CT )
				return R_NM;
			else if ( target_team == TEAM_TERROR_RUSSIAN )
				return R_AL;
			else if ( target_team == TEAM_CIV )
				return R_NO;
		}
		break;
		case TEAM_TERROR_ARTIC:
		{
			if ( target_team == TEAM_TERROR_RUSSIAN || target_team == TEAM_CT )
				return R_NM;
			else if ( target_team == TEAM_TERROR_ARTIC )
				return R_AL;
			else if ( target_team == TEAM_CIV )
				return R_NO;
		}
		break;
	}
	return R_DL;
}

void CZero::OnHit( edict_t *ent, bool headshot )
{
	if ( !ent ) return;
	if ( !headshot )
	{
		switch ( RANDOM_LONG(0, 1) )
		{
		case 0:
			EMIT_SOUND(ent, CHAN_AUTO, "weapons/hits/bullet_hit1.wav", 1, ATTN_NORM);
			break;
		case 1:
			EMIT_SOUND(ent, CHAN_AUTO, "weapons/hits/bullet_hit2.wav", 1, ATTN_NORM);
			break;
		}
	}
	else
		EMIT_SOUND(ent, CHAN_AUTO, "weapons/hits/headshot.wav", 1, ATTN_NORM);
}
