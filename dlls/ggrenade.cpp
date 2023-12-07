/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
/*

===== generic grenade.cpp ========================================================

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "soundent.h"
#include "decals.h"
#include "shake.h"
#include "UserMessages.h"


//===================grenade


LINK_ENTITY_TO_CLASS(grenade, CGrenade);

// Grenades flagged with this will be triggered when the owner calls detonateSatchelCharges
#define SF_DETONATE 0x0001

//
// Radius Flash
//
static unsigned short _FixedUnsigned16(float value, float scale)
{
	int output;

	output = value * scale;
	if (output < 0)
		output = 0;
	if (output > 0xFFFF)
		output = 0xFFFF;

	return (unsigned short)output;
}

float PercentageOfFlashForPlayer(CBaseEntity *player, Vector flashPos)
{
	if (!(player->IsPlayer()))
	{
		// if this entity isn't a player, it's a hostage or some other entity, then don't bother with the expensive checks
		// that come below.
		return 0.0f;
	}

	const float FLASH_FRACTION = 0.167f;
	const float SIDE_OFFSET = 75.0f;

	Vector pos = player->EyePosition();
	Vector vecRight, vecUp;

	Vector tempAngle;
	VectorAngles(player->EyePosition() - flashPos, tempAngle);
	AngleVectors(tempAngle, NULL, &vecRight, &vecUp);

	vecRight = vecRight.Normalize();
	vecUp = vecUp.Normalize();

	// Set up all the ray stuff.
	// We don't want to let other players block the flash bang so we use this custom filter.
	TraceResult tr;

	UTIL_TraceLine( flashPos, pos, dont_ignore_monsters, NULL, &tr );

	if ((tr.flFraction == 1.0f) || (tr.pHit == player->edict()))
	{
		return 1.0f;
	}

	float retval = 0.0f;

	// check the point straight up.
	pos = flashPos + vecUp*50.0f;
	UTIL_TraceLine( flashPos, pos, dont_ignore_monsters, NULL, &tr );
	// Now shoot it to the player's eye.
	pos = player->EyePosition();
	UTIL_TraceLine( tr.vecEndPos, pos, dont_ignore_monsters, NULL, &tr );

	if ((tr.flFraction == 1.0f) || (tr.pHit == player->edict()))
	{
		retval += FLASH_FRACTION;
	}

	// check the point up and right.
	pos = flashPos + vecRight*SIDE_OFFSET + vecUp*10.0f;
	UTIL_TraceLine( flashPos, pos, dont_ignore_monsters, NULL, &tr );
	// Now shoot it to the player's eye.
	pos = player->EyePosition();
	UTIL_TraceLine( tr.vecEndPos, pos, dont_ignore_monsters, NULL, &tr );

	if ((tr.flFraction == 1.0f) || (tr.pHit == player->edict()))
	{
		retval += FLASH_FRACTION;
	}

	// Check the point up and left.
	pos = flashPos - vecRight*SIDE_OFFSET + vecUp*10.0f;
	UTIL_TraceLine( flashPos, pos, dont_ignore_monsters, NULL, &tr );
	// Now shoot it to the player's eye.
	pos = player->EyePosition();
	UTIL_TraceLine( tr.vecEndPos, pos, dont_ignore_monsters, NULL, &tr );

	if ((tr.flFraction == 1.0f) || (tr.pHit == player->edict()))
	{
		retval += FLASH_FRACTION;
	}

	return retval;
}

void CBaseMonster::RadiusFlash(Vector vecSrc, entvars_t* pevInflictor, entvars_t* pevAttacker)
{
	CBaseEntity* pEntity = NULL;
	TraceResult tr;
	Vector vecSpot;
	Vector vecEyePos;
	Vector vForward;
	Vector vecLOS;
	float flDot, fadeTime, fadeHold, flAdjustedDamage;

	static float flRadius = 3000;
	const float flDamage = 3.5;
	float falloff = flDamage / flRadius;

	vecSrc.z += 1; // in case grenade is lying on the ground

	if (!pevAttacker)
		pevAttacker = pevInflictor;

	// iterate on all entities in the vicinity.
	while ((pEntity = UTIL_FindEntityInSphere(pEntity, vecSrc, flRadius)) != NULL)
	{
		vecSpot = pEntity->BodyTarget(vecSrc);
		vecEyePos = pEntity->EyePosition();

		UTIL_TraceLine(vecSrc, vecSpot, dont_ignore_monsters, ENT(pev), &tr);

		if ( tr.flFraction == 1.0 || tr.pHit == pEntity->edict() )
		{
			if ( FClassnameIs( pEntity->pev, "player" ) )
			{
				float percentageOfFlash = PercentageOfFlashForPlayer( pEntity, vecSrc );
				ALERT(at_console, "percentageOfFlash: %f\n", percentageOfFlash);
				if ( percentageOfFlash <= 0.0 ) continue;

				flAdjustedDamage = flDamage - (vecSrc - pEntity->EyePosition()).Length() * falloff;
				ALERT(at_console, "flAdjustedDamage: %f\n", flAdjustedDamage);
				if ( flAdjustedDamage <= 0 ) continue;

				// See if we were facing the flash
				AngleVectors( pEntity->pev->angles, &vForward, NULL, NULL );

				vecLOS = (vecSrc - vecEyePos);

				float flDistance = vecLOS.Length();

				// Normalize both vectors so the dotproduct is in the range -1.0 <= x <= 1.0
				vecLOS = vecLOS.Normalize();

				flDot = DotProduct( vecLOS, vForward );

				float startingAlpha = 255;

				// if target is facing the bomb, the effect lasts longer
				if ( flDot >= 0.6 )
				{
					// looking at the flashbang
					fadeTime = flAdjustedDamage * 2.5f;
					fadeHold = flAdjustedDamage * 1.25f;
				}
				else if ( flDot >= 0.3 )
				{
					// looking to the side
					fadeTime = flAdjustedDamage * 1.75f;
					fadeHold = flAdjustedDamage * 0.8f;
				}
				else if ( flDot >= -0.2 )
				{
					// looking to the side
					fadeTime = flAdjustedDamage * 1.00f;
					fadeHold = flAdjustedDamage * 0.5f;
				}
				else
				{
					// facing away
					fadeTime = flAdjustedDamage * 0.5f;
					fadeHold = flAdjustedDamage * 0.25f;
				//	startingAlpha = 200;
				}

				fadeTime *= percentageOfFlash;
				fadeHold *= percentageOfFlash;

				ALERT(at_console, "Fade Time: %f\n", fadeTime);
				ALERT(at_console, "Fade Hold: %f\n", fadeHold);

				ScreenFade fade;
				fade.duration = _FixedUnsigned16(fadeTime, 1 << 12); // 4.12 fixed
				fade.holdTime = _FixedUnsigned16(fadeHold, 1 << 12); // 4.12 fixed
				fade.r = 255;
				fade.g = 255;
				fade.b = 255;
				fade.a = startingAlpha;
				fade.fadeFlags = 0;
				MESSAGE_BEGIN(MSG_PVS, gmsgFade, pev->origin);
					WRITE_SHORT(fade.duration);	 // fade lasts this long
					WRITE_SHORT(fade.holdTime);	 // fade lasts this long
					WRITE_SHORT(fade.fadeFlags); // fade type (in / out)
					WRITE_BYTE(fade.r);			 // fade red
					WRITE_BYTE(fade.g);			 // fade green
					WRITE_BYTE(fade.b);			 // fade blue
					WRITE_BYTE(fade.a);			 // fade blue
				MESSAGE_END();
			}
			else
			{
				auto pNPC = pEntity->MyMonsterPointer();
				if ( pNPC )
					pNPC->OnNPCFlashed();
			}
		}
	}
}

//
// Grenade Explode
//
void CGrenade::Explode(Vector vecSrc, Vector vecAim)
{
	TraceResult tr;
	UTIL_TraceLine(pev->origin, pev->origin + Vector(0, 0, -32), ignore_monsters, ENT(pev), &tr);

	Explode(&tr, DMG_BLAST);
}

// UNDONE: temporary scorching for PreAlpha - find a less sleazy permenant solution.
void CGrenade::Explode(TraceResult* pTrace, int bitsDamageType)
{
	switch ( m_iGrenadeType )
	{
		default:
		case GRENADE_TYPE_EXPLOSIVE: OnType_Explode( pTrace, bitsDamageType ); break;
		case GRENADE_TYPE_FLASH: OnType_Flash( pTrace, bitsDamageType ); break;
		case GRENADE_TYPE_SMOKE:
		case GRENADE_TYPE_GAS: OnType_Smoke( pTrace, bitsDamageType ); break;
	}
}


void CGrenade::OnType_Explode(TraceResult* pTrace, int bitsDamageType)
{
	pev->model = iStringNull; //invisible
	pev->solid = SOLID_NOT;	  // intangible

	pev->takedamage = DAMAGE_NO;

	// Pull out of the wall a bit
	if (pTrace->flFraction != 1.0)
	{
		pev->origin = pTrace->vecEndPos + (pTrace->vecPlaneNormal * 0.6);
	}

	int iContents = UTIL_PointContents(pev->origin);

	MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pev->origin);
	WRITE_BYTE(TE_EXPLOSION);	// This makes a dynamic light and the explosion sprites/sound
	WRITE_COORD(pev->origin.x); // Send to PAS because of the sound
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z);
	if (iContents != CONTENTS_WATER)
	{
		WRITE_SHORT(g_sModelIndexFireball);
	}
	else
	{
		WRITE_SHORT(g_sModelIndexWExplosion);
	}
	WRITE_BYTE((pev->dmg - 50) * .60); // scale * 10
	WRITE_BYTE(15);					   // framerate
	WRITE_BYTE(TE_EXPLFLAG_NOSOUND);
	MESSAGE_END();

	CSoundEnt::InsertSound(bits_SOUND_COMBAT, pev->origin, NORMAL_EXPLOSION_VOLUME, 3.0);
	entvars_t* pevOwner;
	if (pev->owner)
		pevOwner = VARS(pev->owner);
	else
		pevOwner = NULL;

	pev->owner = NULL; // can't traceline attack owner if this is set

	// Counteract the + 1 in RadiusDamage.
	Vector origin = pev->origin;
	origin.z -= 1;

	RadiusDamage(origin, pev, pevOwner, pev->dmg, CLASS_NONE, bitsDamageType);

	if (RANDOM_FLOAT(0, 1) < 0.5)
	{
		UTIL_DecalTrace(pTrace, DECAL_SCORCH1);
	}
	else
	{
		UTIL_DecalTrace(pTrace, DECAL_SCORCH2);
	}

	switch (RANDOM_LONG(0, 1))
	{
	case 0:
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/hegrenade-1.wav", 1.0, ATTN_NORM);
		break;
	case 1:
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/hegrenade-2.wav", 1.0, ATTN_NORM);
		break;
	}

	pev->effects |= EF_NODRAW;
	SetThink(&CGrenade::Smoke);
	pev->velocity = g_vecZero;
	pev->nextthink = gpGlobals->time + 0.3;

	if (iContents != CONTENTS_WATER)
	{
		int sparkCount = RANDOM_LONG(0, 3);
		for (int i = 0; i < sparkCount; i++)
			Create("spark_shower", pev->origin, pTrace->vecPlaneNormal, NULL);
	}
}

void CGrenade::OnType_Flash(TraceResult* pTrace, int bitsDamageType)
{
	pev->model = iStringNull; //invisible
	pev->solid = SOLID_NOT;	  // intangible

	pev->takedamage = DAMAGE_NO;

	// Pull out of the wall a bit
	if (pTrace->flFraction != 1.0)
	{
		pev->origin = pTrace->vecEndPos + (pTrace->vecPlaneNormal * 0.6);
	}

	Vector origin = pev->origin;
	origin.z -= 1;

	// Schedule for "Is Flashed"
	RadiusFlash( origin, pev, VARS(pev->owner) );

	switch (RANDOM_LONG(0, 1))
	{
	case 0:
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/flashbang-1.wav", 1.0, ATTN_NORM);
		break;
	case 1:
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/flashbang-2.wav", 1.0, ATTN_NORM);
		break;
	}

	if (RANDOM_FLOAT(0, 1) < 0.5)
	{
		UTIL_DecalTrace(pTrace, DECAL_SCORCH1);
	}
	else
	{
		UTIL_DecalTrace(pTrace, DECAL_SCORCH2);
	}

	pev->effects |= EF_NODRAW;
	SetThink(&CGrenade::Smoke);
	pev->velocity = g_vecZero;
	pev->nextthink = gpGlobals->time + 0.3;
}

void CGrenade::OnType_Smoke(TraceResult* pTrace, int bitsDamageType)
{
	pev->solid = SOLID_NOT;
	pev->takedamage = DAMAGE_NO;
	pev->effects |= EF_NODRAW;

	EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/sg_explode.wav", 1.0, ATTN_NORM);
	m_flGasTime = gpGlobals->time + 5.0;

	SetThink(&CGrenade::DoSmoke);
	pev->velocity = g_vecZero;
	pev->nextthink = gpGlobals->time + 0.3;
}

void CGrenade::Smoke()
{
	if (UTIL_PointContents(pev->origin) == CONTENTS_WATER)
	{
		UTIL_Bubbles(pev->origin - Vector(64, 64, 64), pev->origin + Vector(64, 64, 64), 100);
	}
	else
	{
		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
		WRITE_BYTE(TE_SMOKE);
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z);
		WRITE_SHORT(g_sModelIndexSmoke);
		WRITE_BYTE((pev->dmg - 50) * 0.80); // scale * 10
		WRITE_BYTE(12);						// framerate
		MESSAGE_END();
	}
	UTIL_Remove(this);
}

void CGrenade::DoSmoke()
{
	if ( gpGlobals->time > m_flGasTime )
	{
		UTIL_Remove(this);
		return;
	}

	if ( UTIL_PointContents(pev->origin) == CONTENTS_WATER )
		UTIL_Bubbles( pev->origin - Vector(64, 64, 64), pev->origin + Vector(64, 64, 64), 100 );

	Vector origin = pev->origin;
	origin.z -= 1;
	RadiusGas( origin, 100, pev, VARS(pev->owner), (m_iGrenadeType == handgrenade_type_e::GRENADE_TYPE_GAS) ? true : false );

	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
		WRITE_BYTE(TE_SMOKE);
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z);
		WRITE_SHORT(g_sModelIndexSmoke);
		WRITE_BYTE(100);
		WRITE_BYTE(12);
	MESSAGE_END();

	pev->nextthink = gpGlobals->time + 0.5;
}

void CGrenade::Killed(entvars_t* pevAttacker, int iGib)
{
	Detonate();
}


// Timed grenade, this think is called when time runs out.
void CGrenade::DetonateUse(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
{
	SetThink(&CGrenade::Detonate);
	pev->nextthink = gpGlobals->time;
}

void CGrenade::PreDetonate()
{
	CSoundEnt::InsertSound(bits_SOUND_DANGER, pev->origin, 400, 0.3);

	SetThink(&CGrenade::Detonate);
	pev->nextthink = gpGlobals->time + 1;
}


void CGrenade::Detonate()
{
	TraceResult tr;
	Vector vecSpot; // trace starts here!

	vecSpot = pev->origin + Vector(0, 0, 8);
	UTIL_TraceLine(vecSpot, vecSpot + Vector(0, 0, -40), ignore_monsters, ENT(pev), &tr);

	Explode(&tr, DMG_BLAST);
}


//
// Contact grenade, explode when it touches something
//
void CGrenade::ExplodeTouch(CBaseEntity* pOther)
{
	TraceResult tr;
	Vector vecSpot; // trace starts here!

	pev->enemy = pOther->edict();

	vecSpot = pev->origin - pev->velocity.Normalize() * 32;
	UTIL_TraceLine(vecSpot, vecSpot + pev->velocity.Normalize() * 64, ignore_monsters, ENT(pev), &tr);

	Explode(&tr, DMG_BLAST);
}


void CGrenade::DangerSoundThink()
{
	if (!IsInWorld())
	{
		UTIL_Remove(this);
		return;
	}

	CSoundEnt::InsertSound(bits_SOUND_DANGER, pev->origin + pev->velocity * 0.5, pev->velocity.Length(), 0.2);
	pev->nextthink = gpGlobals->time + 0.2;

	if (pev->waterlevel != 0)
	{
		pev->velocity = pev->velocity * 0.5;
	}
}


void CGrenade::BounceTouch(CBaseEntity* pOther)
{
	// don't hit the guy that launched this grenade
	if (pOther->edict() == pev->owner)
		return;

	// only do damage if we're moving fairly fast
	if (m_flNextAttack < gpGlobals->time && pev->velocity.Length() > 100)
	{
		entvars_t* pevOwner = VARS(pev->owner);
		if (pevOwner)
		{
			TraceResult tr = UTIL_GetGlobalTrace();
			ClearMultiDamage();
			pOther->TraceAttack(pevOwner, 1, gpGlobals->v_forward, &tr, DMG_CLUB);
			ApplyMultiDamage(pev, pevOwner);
		}
		m_flNextAttack = gpGlobals->time + 1.0; // debounce
	}

	Vector vecTestVelocity;
	// pev->avelocity = Vector (300, 300, 300);

	// this is my heuristic for modulating the grenade velocity because grenades dropped purely vertical
	// or thrown very far tend to slow down too quickly for me to always catch just by testing velocity.
	// trimming the Z velocity a bit seems to help quite a bit.
	vecTestVelocity = pev->velocity;
	vecTestVelocity.z *= 0.45;

	if (!m_fRegisteredSound && vecTestVelocity.Length() <= 60)
	{
		//ALERT( at_console, "Grenade Registered!: %f\n", vecTestVelocity.Length() );

		// grenade is moving really slow. It's probably very close to where it will ultimately stop moving.
		// go ahead and emit the danger sound.

		// register a radius louder than the explosion, so we make sure everyone gets out of the way
		CSoundEnt::InsertSound(bits_SOUND_DANGER, pev->origin, pev->dmg / 0.4, 0.3);
		m_fRegisteredSound = true;
	}

	if ((pev->flags & FL_ONGROUND) != 0)
	{
		// add a bit of static friction
		pev->velocity = pev->velocity * 0.8;

		pev->sequence = RANDOM_LONG(1, 1);
	}
	else
	{
		// play bounce sound
		BounceSound();
	}
	pev->framerate = pev->velocity.Length() / 200.0;
	if (pev->framerate > 1.0)
		pev->framerate = 1;
	else if (pev->framerate < 0.5)
		pev->framerate = 0;
}



void CGrenade::SlideTouch(CBaseEntity* pOther)
{
	// don't hit the guy that launched this grenade
	if (pOther->edict() == pev->owner)
		return;

	// pev->avelocity = Vector (300, 300, 300);

	if ((pev->flags & FL_ONGROUND) != 0)
	{
		// add a bit of static friction
		pev->velocity = pev->velocity * 0.95;

		if (pev->velocity.x != 0 || pev->velocity.y != 0)
		{
			// maintain sliding sound
		}
	}
	else
	{
		BounceSound();
	}
}

void CGrenade::BounceSound()
{
	switch (RANDOM_LONG(0, 2))
	{
	case 0:
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/grenade_hit1.wav", 0.25, ATTN_NORM);
		break;
	case 1:
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/grenade_hit2.wav", 0.25, ATTN_NORM);
		break;
	case 2:
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/grenade_hit3.wav", 0.25, ATTN_NORM);
		break;
	}
}

void CGrenade::TumbleThink()
{
	if (!IsInWorld())
	{
		UTIL_Remove(this);
		return;
	}

	StudioFrameAdvance();
	pev->nextthink = gpGlobals->time + 0.1;

	if (pev->dmgtime - 1 < gpGlobals->time)
	{
		CSoundEnt::InsertSound(bits_SOUND_DANGER, pev->origin + pev->velocity * (pev->dmgtime - gpGlobals->time), 400, 0.1);
	}

	if (pev->dmgtime <= gpGlobals->time)
	{
		SetThink(&CGrenade::Detonate);
	}
	if (pev->waterlevel != 0)
	{
		pev->velocity = pev->velocity * 0.5;
		pev->framerate = 0.2;
	}
}


void CGrenade::Spawn()
{
	pev->movetype = MOVETYPE_BOUNCE;
	pev->classname = MAKE_STRING("grenade");

	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "models/w_hegrenade.mdl");
	UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0));

	pev->dmg = 100;
	m_fRegisteredSound = false;
}


CGrenade* CGrenade::ShootContact(entvars_t* pevOwner, Vector vecStart, Vector vecVelocity)
{
	CGrenade* pGrenade = GetClassPtr((CGrenade*)NULL);
	pGrenade->Spawn();
	// contact grenades arc lower
	pGrenade->pev->gravity = 0.5; // lower gravity since grenade is aerodynamic and engine doesn't know it.
	UTIL_SetOrigin(pGrenade->pev, vecStart);
	pGrenade->pev->velocity = vecVelocity;
	pGrenade->pev->angles = UTIL_VecToAngles(pGrenade->pev->velocity);
	pGrenade->pev->owner = ENT(pevOwner);

	// make monsters afaid of it while in the air
	pGrenade->SetThink(&CGrenade::DangerSoundThink);
	pGrenade->pev->nextthink = gpGlobals->time;

	// Tumble in air
	pGrenade->pev->avelocity.x = RANDOM_FLOAT(-100, -500);

	// Explode on contact
	pGrenade->SetTouch(&CGrenade::ExplodeTouch);

	pGrenade->pev->dmg = gSkillData.plrDmgM203Grenade;

	return pGrenade;
}


CGrenade* CGrenade::ShootTimed(entvars_t* pevOwner, Vector vecStart, Vector vecVelocity, float time, int type)
{
	CGrenade* pGrenade = GetClassPtr((CGrenade*)NULL);
	pGrenade->Spawn();
	UTIL_SetOrigin(pGrenade->pev, vecStart);
	pGrenade->pev->velocity = vecVelocity;
	pGrenade->pev->angles = UTIL_VecToAngles(pGrenade->pev->velocity);
	pGrenade->pev->owner = ENT(pevOwner);

	pGrenade->SetTouch(&CGrenade::BounceTouch); // Bounce if touched

	// Take one second off of the desired detonation time and set the think to PreDetonate. PreDetonate
	// will insert a DANGER sound into the world sound list and delay detonation for one second so that
	// the grenade explodes after the exact amount of time specified in the call to ShootTimed().

	pGrenade->pev->dmgtime = gpGlobals->time + time;
	pGrenade->SetThink(&CGrenade::TumbleThink);
	pGrenade->pev->nextthink = gpGlobals->time + 0.1;
	if (time < 0.1)
	{
		pGrenade->pev->nextthink = gpGlobals->time;
		pGrenade->pev->velocity = Vector(0, 0, 0);
	}

	pGrenade->pev->sequence = RANDOM_LONG(3, 6);
	pGrenade->pev->framerate = 1.0;

	// Tumble through the air
	// pGrenade->pev->avelocity.x = -400;

	pGrenade->pev->gravity = 0.5;
	pGrenade->pev->friction = 0.8;

	switch ( type )
	{
		default:
		case GRENADE_TYPE_EXPLOSIVE:
			SET_MODEL(ENT(pGrenade->pev), "models/w_hegrenade.mdl");
			break;
		case GRENADE_TYPE_FLASH:
			SET_MODEL(ENT(pGrenade->pev), "models/w_flashbang.mdl");
			break;
		case GRENADE_TYPE_SMOKE:
			SET_MODEL(ENT(pGrenade->pev), "models/w_smokegrenade.mdl");
			break;
		case GRENADE_TYPE_GAS:
			SET_MODEL(ENT(pGrenade->pev), "models/w_gasgrenade.mdl");
			break;
	}
	pGrenade->pev->dmg = 100;
	pGrenade->m_iGrenadeType = type;

	return pGrenade;
}


CGrenade* CGrenade::ShootSatchelCharge(entvars_t* pevOwner, Vector vecStart, Vector vecVelocity)
{
	CGrenade* pGrenade = GetClassPtr((CGrenade*)NULL);
	pGrenade->pev->movetype = MOVETYPE_BOUNCE;
	pGrenade->pev->classname = MAKE_STRING("grenade");

	pGrenade->pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pGrenade->pev), "models/w_hegrenade.mdl"); // Change this to satchel charge model

	UTIL_SetSize(pGrenade->pev, Vector(0, 0, 0), Vector(0, 0, 0));

	pGrenade->pev->dmg = 200;
	UTIL_SetOrigin(pGrenade->pev, vecStart);
	pGrenade->pev->velocity = vecVelocity;
	pGrenade->pev->angles = g_vecZero;
	pGrenade->pev->owner = ENT(pevOwner);

	// Detonate in "time" seconds
	pGrenade->SetThink(&CGrenade::SUB_DoNothing);
	pGrenade->SetUse(&CGrenade::DetonateUse);
	pGrenade->SetTouch(&CGrenade::SlideTouch);
	pGrenade->pev->spawnflags = SF_DETONATE;

	pGrenade->pev->friction = 0.9;

	return pGrenade;
}



void CGrenade::UseSatchelCharges(entvars_t* pevOwner, SATCHELCODE code)
{
	edict_t* pentFind;
	edict_t* pentOwner;

	if (!pevOwner)
		return;

	CBaseEntity* pOwner = CBaseEntity::Instance(pevOwner);

	pentOwner = pOwner->edict();

	pentFind = FIND_ENTITY_BY_CLASSNAME(NULL, "grenade");
	while (!FNullEnt(pentFind))
	{
		CBaseEntity* pEnt = Instance(pentFind);
		if (pEnt)
		{
			if (FBitSet(pEnt->pev->spawnflags, SF_DETONATE) && pEnt->pev->owner == pentOwner)
			{
				if (code == SATCHEL_DETONATE)
					pEnt->Use(pOwner, pOwner, USE_ON, 0);
				else // SATCHEL_RELEASE
					pEnt->pev->owner = NULL;
			}
		}
		pentFind = FIND_ENTITY_BY_CLASSNAME(pentFind, "grenade");
	}
}

//======================end grenade
