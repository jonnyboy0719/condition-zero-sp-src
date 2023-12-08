
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "squadmonster.h"
#include "talkmonster.h"
#include "soundent.h"
#include "weapons.h"
#include "czero_human_base.h"

/// <summary>
/// Pistol enemy
/// </summary>
class CTerrorArtic_Pistol : public CZeroHumanBase_Pistol
{
	DEFINE_BASE( CZeroHumanBase_Pistol );
public:
	void StartMonster() override;
	void Spawn() override;
	void Precache() override;
	void BarneyFirePistol() override;
	CBaseEntity* FindNearestFriend(bool fPlayer) override { return nullptr; }
	bool TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType) override;
	void Killed(entvars_t* pevAttacker, int iGib);
};
LINK_ENTITY_TO_CLASS(monster_terrorist_arctic_pistol, CTerrorArtic_Pistol);

void CTerrorArtic_Pistol::StartMonster()
{
	CBase::StartMonster();
	// They have their weapons drawn at start
	SetBodygroup(GUN_GROUP, BARNEY_BODY_GUNDRAWN);
	m_fGunDrawn = true;
	UTIL_SetSize(pev, VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX);
	SetupDeathAnimation();
}

void CTerrorArtic_Pistol::Spawn()
{
	pev->team = CSDSTeams::TEAM_TERROR2;
	m_VoiceLang = CSDSLanguage::LANG_RU;
	m_TeamVoice = CSDSTeamVoice::VOICE_T;
	UTIL_SetSize(pev, _tinyhull, _tinyhull);
	CBase::Spawn();
	SET_MODEL(ENT(pev), "models/czero/t_arctic_p.mdl");
	SetUse(NULL);
}

void CTerrorArtic_Pistol::Precache()
{
	CBase::Precache();
	PRECACHE_MODEL("models/czero/t_arctic_p.mdl");
	PRECACHE_SOUND("weapons/glock18-2.wav"); // Single
}

void CTerrorArtic_Pistol::BarneyFirePistol()
{
	Vector vecShootOrigin;

	UTIL_MakeVectors(pev->angles);
	vecShootOrigin = pev->origin + Vector(0, 0, 55);
	Vector vecShootDir = ShootAtEnemy(vecShootOrigin);

	Vector angDir = UTIL_VecToAngles(vecShootDir);
	SetBlending(0, angDir.x);
	pev->effects = EF_MUZZLEFLASH;

	FireBullets(1, vecShootOrigin, vecShootDir, VECTOR_CONE_2DEGREES, 1024, BULLET_MONSTER_9MM);

	int pitchShift = RANDOM_LONG(0, 20);

	// Only shift about half the time
	if (pitchShift > 10)
		pitchShift = 0;
	else
		pitchShift -= 5;
	EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "weapons/glock18-2.wav", 1, ATTN_NORM, 0, 100 + pitchShift);

	CSoundEnt::InsertSound(bits_SOUND_COMBAT, pev->origin, 384, 0.3);

	m_cAmmoLoaded--; // take away a bullet!
}

bool CTerrorArtic_Pistol::TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType)
{
	// make sure friends talk about it if player hurts talkmonsters...
	bool ret = CTalkMonster::TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
	if (!IsAlive() || pev->deadflag == DEAD_DYING)
		return ret;

	if (m_MonsterState != MONSTERSTATE_PRONE && (pevAttacker->flags & FL_CLIENT) != 0)
		m_flPlayerDamage += flDamage;

	return ret;
}

void CTerrorArtic_Pistol::Killed(entvars_t* pevAttacker, int iGib)
{
	if (GetBodygroup(GUN_GROUP) < BARNEY_BODY_GUNGONE)
	{ // drop the gun!
		Vector vecGunPos;
		Vector vecGunAngles;

		SetBodygroup(GUN_GROUP, BARNEY_BODY_GUNGONE);

		GetAttachment(0, vecGunPos, vecGunAngles);

		CBaseEntity* pGun = DropItem("weapon_glock", vecGunPos, vecGunAngles);
	}
	m_hTargetEnt = NULL;
	// Don't finish that sentence
	StopTalking();
	CBase::Killed(pevAttacker, iGib);
}

/// <summary>
/// MP5 with Grenades
/// </summary>
class CTerrorArtic_Grunt : public CZeroHumanBase
{
	DEFINE_BASE( CZeroHumanBase );
public:
	void StartMonster() override;
	void Spawn() override;
	void Precache() override;
	void HandleAnimEvent(MonsterEvent_t* pEvent) override;
	virtual void SetWeapons();
};
LINK_ENTITY_TO_CLASS(monster_terrorist_arctic_grenader, CTerrorArtic_Grunt);

void CTerrorArtic_Grunt::StartMonster()
{
	CBase::StartMonster();
	UTIL_SetSize(pev, VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX);
	SetupDeathAnimation();
}

void CTerrorArtic_Grunt::Spawn()
{
	pev->team = CSDSTeams::TEAM_TERROR2;
	m_VoiceLang = CSDSLanguage::LANG_RU;
	m_TeamVoice = CSDSTeamVoice::VOICE_T;
	UTIL_SetSize(pev, _tinyhull, _tinyhull);
	SetWeapons();
	CBase::Spawn();
	SET_MODEL(ENT(pev), "models/czero/t_arctic.mdl");
}

void CTerrorArtic_Grunt::Precache()
{
	CBase::Precache();
	PRECACHE_MODEL("models/czero/t_arctic.mdl");
	PRECACHE_SOUND("weapons/ak47-npc1.wav");
	PRECACHE_SOUND("weapons/ak47-npc2.wav");
}

void CTerrorArtic_Grunt::HandleAnimEvent(MonsterEvent_t* pEvent)
{
	switch (pEvent->event)
	{
		case HGRUNT_AE_DROP_GUN:
		{
			if (GetBodygroup(GUN_GROUP) != GUN_NONE)
			{
				Vector vecGunPos;
				Vector vecGunAngles;

				GetAttachment(0, vecGunPos, vecGunAngles);

				// switch to body group with no gun.
				SetBodygroup(GUN_GROUP, GUN_NONE);

				// now spawn a gun.
				if (FBitSet(pev->weapons, HGRUNT_SHOTGUN))
					DropItem("weapon_autoshotgun", vecGunPos, vecGunAngles);
				else if (FBitSet(pev->weapons, HGRUNT_WEPM4))
					DropItem("weapon_ak47", vecGunPos, vecGunAngles);
				else
					DropItem("weapon_mp5", vecGunPos, vecGunAngles);
			}
		}
		break;

		case HGRUNT_AE_BURST1:
		{
			if (FBitSet(pev->weapons, HGRUNT_9MMAR))
			{
				Shoot();

				// the first round of the three round burst plays the sound and puts a sound in the world sound list.
				if (RANDOM_LONG(0, 1))
				{
					EMIT_SOUND(ENT(pev), CHAN_WEAPON, "hgrunt/gr_mgun1.wav", 1, ATTN_NORM);
				}
				else
				{
					EMIT_SOUND(ENT(pev), CHAN_WEAPON, "hgrunt/gr_mgun2.wav", 1, ATTN_NORM);
				}
			}
			else if (FBitSet(pev->weapons, HGRUNT_WEPM4))
			{
				Shoot();

				// the first round of the three round burst plays the sound and puts a sound in the world sound list.
				if (RANDOM_LONG(0, 1))
				{
					EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/ak47-npc1.wav", 1, ATTN_NORM);
				}
				else
				{
					EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/ak47-npc2.wav", 1, ATTN_NORM);
				}
			}
			else
			{
				Shotgun();

				EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/sbarrel1.wav", 1, ATTN_NORM);
			}

			CSoundEnt::InsertSound(bits_SOUND_COMBAT, pev->origin, 384, 0.3);
		}
		break;

		default:
			CBase::HandleAnimEvent(pEvent);
		break;
	}
}

void CTerrorArtic_Grunt::SetWeapons()
{
	// MP5 with Hand grenade
	pev->weapons = 0;
}


/// <summary>
/// Shotgunner
/// </summary>
class CTerrorArtic_Shotgunner : public CTerrorArtic_Grunt
{
public:
	void SetWeapons() override;
};
LINK_ENTITY_TO_CLASS(monster_terrorist_arctic_shotgun, CTerrorArtic_Grunt);

void CTerrorArtic_Shotgunner::SetWeapons()
{
	pev->weapons = HGRUNT_SHOTGUN;
}


/// <summary>
/// TODO: We don't have a melee yet, so just use MP5 w/ no grenade
/// </summary>
class CTerrorArtic_Melee : public CTerrorArtic_Grunt
{
public:
	void SetWeapons() override;
};
LINK_ENTITY_TO_CLASS(monster_terrorist_arctic_melee, CTerrorArtic_Melee);

void CTerrorArtic_Melee::SetWeapons()
{
	pev->weapons = HGRUNT_9MMAR;
}


/// <summary>
/// TODO: We don't have a sniper yet, so just use M4
/// </summary>
class CTerrorArtic_SniperRifle : public CTerrorArtic_Grunt
{
public:
	void SetWeapons() override;
};
LINK_ENTITY_TO_CLASS(monster_terrorist_arctic_sniperrifle, CTerrorArtic_SniperRifle);

void CTerrorArtic_SniperRifle::SetWeapons()
{
	pev->weapons = HGRUNT_WEPM4;
}


/// <summary>
/// Uses AK47
/// </summary>
class CTerrorArtic_AssaultRifle : public CTerrorArtic_Grunt
{
public:
	void SetWeapons() override;
};
LINK_ENTITY_TO_CLASS(monster_terrorist_arctic_assaultrifle, CTerrorArtic_AssaultRifle);

void CTerrorArtic_AssaultRifle::SetWeapons()
{
	pev->weapons = HGRUNT_WEPM4;
}


/// <summary>
/// Uses MP5
/// </summary>
class CTerrorArtic_MP5 : public CTerrorArtic_Grunt
{
public:
	void SetWeapons() override;
};
LINK_ENTITY_TO_CLASS(monster_terrorist_arctic_mp5, CTerrorArtic_MP5);

void CTerrorArtic_MP5::SetWeapons()
{
	pev->weapons = HGRUNT_9MMAR;
}
