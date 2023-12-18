#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "player.h"
#include "explode.h"
#include "effects.h"
#include "gamerules.h"
#include "soundent.h"

#ifndef CLIENT_DLL

class CRadioControlledBombTarget : public CBaseMonster
{
public:
	void Spawn() override;
	void Precache() override;
	void SetYawSpeed() override;
	int Classify() override;
	void StartMonster() override;
	void HandleAnimEvent(MonsterEvent_t* pEvent) override {}
	int ISoundMask() override { return 0; }
	bool TakeHealth(float flHealth, int bitsDamageType) override { return false; }
	bool TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType) override { return false; }
	void OnExplode();
	void OnBombPlaced(CBasePlayer* player);
	bool HasPlaced() const { return m_bPlaced; }
	bool KeyValue(KeyValueData* pkvd) override;
	void UpdateBombPlacement();

	bool Save(CSave& save) override;
	bool Restore(CRestore& restore) override;
	static TYPEDESCRIPTION m_SaveData[];

private:
	bool m_bPlaced = false;
	bool m_canplacebomb = true;
	string_t m_iszPlantTarget;
	Vector m_oldOrigin;
};
LINK_ENTITY_TO_CLASS(monster_bombtarget, CRadioControlledBombTarget);

TYPEDESCRIPTION CRadioControlledBombTarget::m_SaveData[] =
{
	DEFINE_FIELD(CRadioControlledBombTarget, m_bPlaced, FIELD_BOOLEAN),
	DEFINE_FIELD(CRadioControlledBombTarget, m_canplacebomb, FIELD_BOOLEAN),
	DEFINE_FIELD(CRadioControlledBombTarget, m_iszPlantTarget, FIELD_STRING),
	DEFINE_FIELD(CRadioControlledBombTarget, m_oldOrigin, FIELD_VECTOR),
};
IMPLEMENT_SAVERESTORE(CRadioControlledBombTarget, CBaseMonster);

int CRadioControlledBombTarget::Classify()
{
	return CLASS_NONE;
}

void CRadioControlledBombTarget::StartMonster()
{
	CBaseMonster::StartMonster();
	UTIL_SetOrigin(pev, m_oldOrigin);
	pev->solid = SOLID_BBOX;
	UpdateBombPlacement();
}


void CRadioControlledBombTarget::OnExplode()
{
	if ( !m_bPlaced ) return;
	FireTargets(STRING(m_iszTriggerTarget), m_hActivator, this, USE_TOGGLE, 0);
	ExplosionCreate(Center(), pev->angles, edict(), 150, true);
	UTIL_Remove(this);
}


void CRadioControlledBombTarget::OnBombPlaced(CBasePlayer* player)
{
	if ( m_bPlaced ) return;
	pev->renderamt = 255;
	pev->rendermode = kRenderNormal;
	pev->renderfx = kRenderFxNone;
	SET_MODEL(ENT(pev), "models/w_rcontrolbomb_bomb.mdl");
	m_bPlaced = true;
	pev->owner = player->edict();
	FireTargets(STRING(m_iszPlantTarget), m_hActivator, this, USE_TOGGLE, 0);
}

bool CRadioControlledBombTarget::KeyValue(KeyValueData* pkvd)
{
	if (FStrEq(pkvd->szKeyName, "canplacebomb"))
	{
		m_canplacebomb = atoi(pkvd->szValue) >= 1 ? true : false;
		UpdateBombPlacement();
		return true;
	}
	else if (FStrEq(pkvd->szKeyName, "PlantTarget"))
	{
		m_iszPlantTarget = ALLOC_STRING(pkvd->szValue);
		return true;
	}

	return CBaseMonster::KeyValue(pkvd);
}

void CRadioControlledBombTarget::UpdateBombPlacement()
{
	// Already placed? Ignore.
	if (m_bPlaced) return;
	if (m_canplacebomb)
	{
		ALERT(at_console, "DRAW BOMB PLACEMENT\n");
		UTIL_SetOrigin(pev, m_oldOrigin);
		pev->solid = SOLID_BBOX;
		pev->renderamt = 70;
		pev->rendermode = kRenderTransAdd;
		pev->renderfx = kRenderFxPulseFastWide;
	}
	else
	{
		ALERT(at_console, "HIDE BOMB PLACEMENT\n");
		pev->solid = SOLID_NOT;
		pev->renderamt = 0;
		pev->rendermode = kRenderTransAdd;
		pev->renderfx = kRenderFxNone;
	}
}


void CRadioControlledBombTarget::SetYawSpeed()
{
	pev->yaw_speed = 0;
}


void CRadioControlledBombTarget::Spawn()
{
	Precache();

	SET_MODEL(ENT(pev), "models/w_rcontrolbomb_indicator.mdl");

	UTIL_SetSize(pev, Vector(-8, -8, -8), Vector(8, 8, 8));
	UTIL_SetOrigin(pev, pev->origin);
	m_oldOrigin = pev->origin;

	pev->solid = SOLID_NOT;
	pev->takedamage = DAMAGE_NO;
	pev->movetype = MOVETYPE_FLY;
	pev->renderamt = 70;
	pev->rendermode = kRenderTransAdd;
	pev->renderfx = kRenderFxPulseFastWide;
	pev->health = -1;
	m_bloodColor = DONT_BLEED;
	m_flFieldOfView = 0;
	m_MonsterState = MONSTERSTATE_NONE;
	m_bPlaced = false;

	MonsterInit();
}


void CRadioControlledBombTarget::Precache()
{
	PRECACHE_MODEL("models/w_rcontrolbomb_bomb.mdl");
	PRECACHE_MODEL("models/w_rcontrolbomb_indicator.mdl");
}
#endif


void DoRadioControllerWarn(CBasePlayer* player)
{
#ifndef CLIENT_DLL
	edict_t* pFind;

	pFind = FIND_ENTITY_BY_CLASSNAME(NULL, "monster_bombtarget");

	while (!FNullEnt(pFind))
	{
		CBaseEntity* pEnt = CBaseEntity::Instance(pFind);
		CRadioControlledBombTarget* pRadioController = (CRadioControlledBombTarget*)pEnt;

		if (pRadioController)
		{
			if (pRadioController->pev->owner == player->edict() && pRadioController->HasPlaced())
				CSoundEnt::InsertSound(bits_SOUND_DANGER, pRadioController->pev->origin, 400, 0.5);
		}

		pFind = FIND_ENTITY_BY_CLASSNAME(pFind, "monster_bombtarget");
	}
#endif
}


bool DoRadioControllerCall(CBasePlayer* player, bool bExplode)
{
#ifndef CLIENT_DLL
	// Explode, or do line tracer.
	if ( bExplode )
	{
		edict_t* pFind;

		pFind = FIND_ENTITY_BY_CLASSNAME(NULL, "monster_bombtarget");

		while (!FNullEnt(pFind))
		{
			CBaseEntity* pEnt = CBaseEntity::Instance(pFind);
			CRadioControlledBombTarget* pRadioController = (CRadioControlledBombTarget*)pEnt;

			if (pRadioController)
			{
				if (pRadioController->pev->owner == player->edict() && pRadioController->HasPlaced())
					pRadioController->OnExplode();
			}

			pFind = FIND_ENTITY_BY_CLASSNAME(pFind, "monster_bombtarget");
		}
		return true;
	}
#endif

	UTIL_MakeVectors( player->pev->v_angle + player->pev->punchangle );
	Vector vecSrc = player->GetGunPosition();
	Vector vecAiming = gpGlobals->v_forward;

	TraceResult tr;
	UTIL_TraceLine( vecSrc, vecSrc + vecAiming * 128, dont_ignore_monsters, ENT(player->pev), &tr );

	if ( tr.flFraction < 1.0 )
	{
		CBaseEntity* pEntity = CBaseEntity::Instance(tr.pHit);
		if ( pEntity && FClassnameIs(pEntity->pev, "monster_bombtarget") )
		{
#ifdef CLIENT_DLL
			return true;
#else
			CRadioControlledBombTarget* pRadioController = (CRadioControlledBombTarget*)pEntity;
			if (pRadioController)
			{
				if (!pRadioController->HasPlaced())
				{
					pRadioController->OnBombPlaced( player );
					return true;
				}
			}
#endif
		}
	}
	return false;
}

LINK_ENTITY_TO_CLASS(weapon_radiocontrolledbomb, CRadioBombController);


//=========================================================
//=========================================================
void CRadioBombController::AddToPlayer(CBasePlayer* pPlayer)
{
	m_chargeReady = 0; // this satchel charge weapon now forgets that any satchels are deployed by it.
	CBasePlayerWeapon::AddToPlayer(pPlayer);
}

void CRadioBombController::Spawn()
{
	Precache();
	m_iId = WEAPON_RADIOCONTROLLER;
	SET_MODEL(ENT(pev), "models/w_rcontrolbomb.mdl");

	m_iDefaultAmmo = 1;	// Make sure we always spawn with one remote bomb.
	m_chargeReady = 0;

	FallInit(); // get ready to fall down.
}


void CRadioBombController::Precache()
{
	PRECACHE_MODEL("models/v_rcontrolbomb.mdl");
	PRECACHE_MODEL("models/w_rcontrolbomb.mdl");
	PRECACHE_MODEL("models/w_rcontrolbomb_bomb.mdl");
	PRECACHE_MODEL("models/w_rcontrolbomb_detonator.mdl");
	PRECACHE_MODEL("models/w_rcontrolbomb_indicator.mdl");
	PRECACHE_MODEL("models/p_rcontrolbomb.mdl");
	PRECACHE_MODEL("models/p_rcontrolbomb_bomb.mdl");
	PRECACHE_MODEL("models/p_rcontrolbomb_detonator.mdl");

	PRECACHE_SOUND("weapons/rc_detonate.wav");
	PRECACHE_SOUND("weapons/rc_draw_detonator.wav");
	PRECACHE_SOUND("weapons/rc_holster_detonator.wav");
	PRECACHE_SOUND("weapons/rc_plant.wav");

	UTIL_PrecacheOther("monster_bombtarget");
}


bool CRadioBombController::GetItemInfo(ItemInfo* p)
{
	WeaponSlots slot = GetWeaponSlotInfo(WEAPON_RADIOCONTROLLER);
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "Satchel Charge";
	p->iMaxAmmo1 = 1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = slot.slot;
	p->iPosition = slot.position;
	p->iFlags = ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE;
	p->iId = m_iId = slot.id;
	p->iWeight = SATCHEL_WEIGHT;

	return true;
}


bool CRadioBombController::Deploy()
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;

	bool result;
	if (m_chargeReady > 0)
		result = DefaultDeploy("models/v_rcontrolbomb.mdl", "models/p_rcontrolbomb_detonator.mdl", RADIOCONTROLLER_DRAW_DETONATOR, "hive");
	else
		result = DefaultDeploy("models/v_rcontrolbomb.mdl", "models/p_rcontrolbomb_bomb.mdl", RADIOCONTROLLER_DRAW_BOMB, "trip");
	if (result)
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.36;
	return result;
}


void CRadioBombController::Holster()
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;

	if (m_chargeReady > 0)
		SendWeaponAnim(RADIOCONTROLLER_HOLSTER_DETONATOR);
	else
		SendWeaponAnim(RADIOCONTROLLER_HOLSTER_BOMB);

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		// no more grenades!
		m_pPlayer->ClearWeaponBit(m_iId);
		SetThink(&CRadioBombController::DestroyItem);
		pev->nextthink = gpGlobals->time + 0.1;
	}

	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "common/null.wav", 1.0, ATTN_NORM);
}


bool CRadioBombController::CanPlant()
{
	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );
	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = gpGlobals->v_forward;

	TraceResult tr;
	UTIL_TraceLine( vecSrc, vecSrc + vecAiming * 128, dont_ignore_monsters, ENT(m_pPlayer->pev), &tr );

	if ( tr.flFraction < 1.0 )
	{
		CBaseEntity* pEntity = CBaseEntity::Instance(tr.pHit);
		if ( pEntity && FClassnameIs(pEntity->pev, "monster_bombtarget") )
		{
#ifndef CLIENT_DLL
			CRadioControlledBombTarget* pRadioController = (CRadioControlledBombTarget*)pEntity;
			if (pRadioController)
			{
				if (!pRadioController->HasPlaced())
					return true;
			}
#endif
		}
	}
	return false;
}



void CRadioBombController::PrimaryAttack()
{
	switch (m_chargeReady)
	{
		case 0:
		{
			if (!CanPlant()) return;
			SendWeaponAnim(RADIOCONTROLLER_PLANT);
			m_chargeReady = 4;
			m_flNextPrimaryAttack = GetNextAttackDelay(1.09);
			m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.09;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.09;
		}
		break;
		case 1:
		{
			SendWeaponAnim(RADIOCONTROLLER_DETONATE);
			DoRadioControllerWarn(m_pPlayer);
			m_chargeReady = 5;
			m_flNextPrimaryAttack = GetNextAttackDelay(1.53);
			m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.53;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.53;
			break;
		}

		case 2:
		case 3:
		case 4:
		case 5:
		{
		}
		break;
	}
}


void CRadioBombController::WeaponIdle()
{
	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	float flWait = UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10, 15);
	switch (m_chargeReady)
	{
		case 0:
		{
			SendWeaponAnim(RADIOCONTROLLER_IDLE_BOMB);
			// use tripmine animations
			strcpy(m_pPlayer->m_szAnimExtention, "trip");
		}
		break;

		case 1:
		{
			SendWeaponAnim(RADIOCONTROLLER_IDLE_CONTROLLER);
			// use hivehand animations
			strcpy(m_pPlayer->m_szAnimExtention, "hive");
		}
		break;

		case 2:
		{
#ifndef CLIENT_DLL
			m_pPlayer->pev->viewmodel = MAKE_STRING("models/v_rcontrolbomb.mdl");
			m_pPlayer->pev->weaponmodel = MAKE_STRING("models/p_rcontrolbomb_detonator.mdl");
#else
			LoadVModel("models/v_rcontrolbomb.mdl", m_pPlayer);
#endif
			SendWeaponAnim(RADIOCONTROLLER_DRAW_DETONATOR);

			// use tripmine animations
			strcpy(m_pPlayer->m_szAnimExtention, "hive");

			m_flNextPrimaryAttack = GetNextAttackDelay(1.36);
			m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.36;
			m_chargeReady = 1;
		}
		break;

		case 3:
		{
#ifndef CLIENT_DLL
			m_pPlayer->pev->viewmodel = MAKE_STRING("models/v_rcontrolbomb.mdl");
			m_pPlayer->pev->weaponmodel = MAKE_STRING("models/p_rcontrolbomb_bomb.mdl");
#else
			LoadVModel("models/v_rcontrolbomb.mdl", m_pPlayer);
#endif
			SendWeaponAnim(RADIOCONTROLLER_DRAW_BOMB);

			// use tripmine animations
			strcpy(m_pPlayer->m_szAnimExtention, "trip");

			m_flNextPrimaryAttack = GetNextAttackDelay(1.03);
			m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.03;
			m_chargeReady = 0;
		}
		break;

		case 4:
		{
			if (DoRadioControllerCall(m_pPlayer, false))
				m_chargeReady = 2;
			else
				m_chargeReady = 3;
			flWait = 0.0f;
		}
		break;

		case 5:
		{
			DoRadioControllerCall(m_pPlayer, true);
			m_chargeReady = 3;
			//flWait = 0.0f;
			m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
			RetireWeapon();
			return;
		}
		break;
	}
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + flWait; // how long till we do this again.
}
