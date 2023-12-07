/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
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

#pragma once

#define MAX_WEAPON_NAME 128


#define WEAPON_FLAGS_SELECTONEMPTY 1

#define WEAPON_IS_ONTARGET 0x40

struct WEAPON
{
	char szName[MAX_WEAPON_NAME];
	int iAmmoType;
	int iAmmo2Type;
	int iMax1;
	int iMax2;
	int iSlot;
	int iSlotPos;
	int iFlags;
	int iId;
	int iClip;

	int iCount; // # of itesm in plist

	V_HSPRITE hActive;
	Rect rcActive;
	V_HSPRITE hInactive;
	Rect rcInactive;
	V_HSPRITE hAmmo;
	Rect rcAmmo;
	V_HSPRITE hAmmo2;
	Rect rcAmmo2;
	V_HSPRITE hCrosshair;
	Rect rcCrosshair;
	V_HSPRITE hAutoaim;
	Rect rcAutoaim;
	V_HSPRITE hZoomedCrosshair;
	Rect rcZoomedCrosshair;
	V_HSPRITE hZoomedAutoaim;
	Rect rcZoomedAutoaim;
};
