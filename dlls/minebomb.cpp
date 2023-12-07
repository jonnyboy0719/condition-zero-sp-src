#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "player.h"
#include "explode.h"
#include "effects.h"
#include "gamerules.h"

class CMineBomb : public CBaseMonster
{
public:
	void Spawn() override;
	void Precache() override;
	void SetYawSpeed() override;
	int Classify() override;
	void StartMonster() override;
	void HandleAnimEvent(MonsterEvent_t* pEvent) override {}
	int ISoundMask() override { return 0; }
	// Explode on damage, doesn't matter what it is
	bool TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType) override;
	void Killed(entvars_t* pevAttacker, int iGib) override;
	void OnExplode();
	void ExplodeOnTouch(CBaseEntity* pOther);
	void Beeping();
	void StartUp();
	void BeginDeath();
	void KillSprite();

private:
	bool m_ShowSprite;
	CSprite *m_pSprite;
	Vector m_oldOrigin;
};
LINK_ENTITY_TO_CLASS(monster_minebomb, CMineBomb);


int CMineBomb::Classify()
{
	return CLASS_NONE;
}


void CMineBomb::StartMonster()
{
	CBaseMonster::StartMonster();
	UTIL_SetOrigin(pev, m_oldOrigin);
	SetTouch(&CMineBomb::ExplodeOnTouch);
	SetThink(&CMineBomb::StartUp);

	m_pSprite = CSprite::SpriteCreate("sprites/flare1.spr", pev->origin + Vector(0, 0, 2), true);
	m_pSprite->SetTransparency(kRenderGlow, 255, 255, 255, 0, kRenderFxNoDissipation);
	m_pSprite->SetScale(0.15);
	m_pSprite->SetColor(241, 73, 0);
	m_pSprite->SetBrightness(34);
	m_pSprite->pev->renderamt = 0;

	pev->nextthink = gpGlobals->time + RANDOM_FLOAT(0.5, 1.0);
}


void CMineBomb::ExplodeOnTouch(CBaseEntity* pOther)
{
	// Don't trigger if it's a brush
	if ( pOther->pev->solid == SOLID_BSP ) return;
	SetThink(&CMineBomb::BeginDeath);
	pev->nextthink = gpGlobals->time + 0.1;
	KillSprite();
}


void CMineBomb::Beeping()
{
	if ( m_pSprite )
	{
		m_ShowSprite = !m_ShowSprite;
		m_pSprite->pev->renderamt = m_ShowSprite ? 255 : 0;
		if ( m_ShowSprite )
		{
			EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "weapons/mine_beep.wav", 0.5, ATTN_NORM, 1.0, 75);
			pev->nextthink = gpGlobals->time + 0.5f;
			return;
		}
	}
	pev->nextthink = gpGlobals->time + 1.0f;
}

void CMineBomb::StartUp()
{
	pev->solid = SOLID_BBOX;
	pev->takedamage = DAMAGE_YES;
	SetThink(&CMineBomb::Beeping);
	pev->nextthink = gpGlobals->time + 1.5f;
}


void CMineBomb::BeginDeath()
{
	SetTouch(NULL);
	SetThink(NULL);
	OnExplode();
}


void CMineBomb::KillSprite()
{
	if (m_pSprite)
	{
		UTIL_Remove(m_pSprite);
		m_pSprite = NULL;
	}
}


void CMineBomb::Killed(entvars_t* pevAttacker, int iGib)
{
	if ( pev->takedamage == DAMAGE_NO ) return;
	SetThink(&CMineBomb::BeginDeath);
	pev->nextthink = gpGlobals->time + 0.1;
	KillSprite();
}


void CMineBomb::OnExplode()
{
	ExplosionCreate(Center(), pev->angles, edict(), 150, true);
	SetThink(&CMineBomb::SUB_Remove);
	pev->nextthink = gpGlobals->time + 0.1;
}


void CMineBomb::SetYawSpeed()
{
	pev->yaw_speed = 0;
}


void CMineBomb::Spawn()
{
	Precache();

	SET_MODEL(ENT(pev), "models/w_rcontrolbomb_bomb.mdl");

	UTIL_SetSize(pev, Vector(-8, -8, -2), Vector(8, 8, 2));
	UTIL_SetOrigin(pev, pev->origin);
	m_oldOrigin = pev->origin;

	pev->solid = SOLID_NOT;
	pev->takedamage = DAMAGE_NO;
	pev->movetype = MOVETYPE_FLY;
	pev->health = 1;	// We actually don't care about our HP. The moment someone steps on us, or shoot at us, we go boom.
	m_bloodColor = DONT_BLEED;
	m_flFieldOfView = 0;
	m_MonsterState = MONSTERSTATE_NONE;

	// This entity cannot be harmed by the nervgas grenade.
	m_gasmask = true;

	MonsterInit();
}


bool CMineBomb::TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType)
{
	if (pev->takedamage == DAMAGE_YES)
	{
		SetThink(&CMineBomb::BeginDeath);
		pev->nextthink = gpGlobals->time + 0.1;
		KillSprite();
		return false;
	}
	return true;
}


void CMineBomb::Precache()
{
	PRECACHE_MODEL("models/w_rcontrolbomb_bomb.mdl");
	PRECACHE_MODEL("sprites/flare1.spr");
	PRECACHE_SOUND("weapons/mine_beep.wav");
}
