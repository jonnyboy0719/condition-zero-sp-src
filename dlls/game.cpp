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
#include "extdll.h"
#include "eiface.h"
#include "util.h"
#include "game.h"
#include "filesystem_utils.h"

// Register skill cvars in 1 single define instead of doing it 3 times in a row each time.
#define CVAR_REGISTER_SKILL(_VAR) \
	CVAR_REGISTER(&_VAR##1);      \
	CVAR_REGISTER(&_VAR##2);      \
	CVAR_REGISTER(&_VAR##3)
#define CVAR_REGISTER_SKILLVAR(_VAR, _DEF) \
cvar_t _VAR##1 = {#_VAR##"1", #_DEF}; \
cvar_t _VAR##2 = {#_VAR##"2", #_DEF}; \
cvar_t _VAR##3 = {#_VAR##"3", #_DEF};
#define CVAR_REGISTER_SKILLVAR_SPECIFIC(_VAR, _EASY, _MEDIUM, _HARD) \
cvar_t _VAR##1 = {#_VAR##"1", #_EASY}; \
cvar_t _VAR##2 = {#_VAR##"2", #_MEDIUM}; \
cvar_t _VAR##3 = {#_VAR##"3", #_HARD};


cvar_t displaysoundlist = {"displaysoundlist", "0"};

// multiplayer server rules
cvar_t fragsleft = {"mp_fragsleft", "0", FCVAR_SERVER | FCVAR_UNLOGGED}; // Don't spam console/log files/users with this changing
cvar_t timeleft = {"mp_timeleft", "0", FCVAR_SERVER | FCVAR_UNLOGGED};	 // "      "

// multiplayer server rules
cvar_t teamplay = {"mp_teamplay", "0", FCVAR_SERVER};
cvar_t fraglimit = {"mp_fraglimit", "0", FCVAR_SERVER};
cvar_t timelimit = {"mp_timelimit", "0", FCVAR_SERVER};
cvar_t friendlyfire = {"mp_friendlyfire", "0", FCVAR_SERVER};
cvar_t falldamage = {"mp_falldamage", "0", FCVAR_SERVER};
cvar_t weaponstay = {"mp_weaponstay", "0", FCVAR_SERVER};
cvar_t forcerespawn = {"mp_forcerespawn", "1", FCVAR_SERVER};
cvar_t flashlight = {"mp_flashlight", "0", FCVAR_SERVER};
cvar_t aimcrosshair = {"mp_autocrosshair", "1", FCVAR_SERVER};
cvar_t decalfrequency = {"decalfrequency", "30", FCVAR_SERVER};
cvar_t teamlist = {"mp_teamlist", "hgrunt;scientist", FCVAR_SERVER};
cvar_t teamoverride = {"mp_teamoverride", "1"};
cvar_t defaultteam = {"mp_defaultteam", "0"};
cvar_t allowmonsters = {"mp_allowmonsters", "0", FCVAR_SERVER};

cvar_t allow_spectators = {"allow_spectators", "0.0", FCVAR_SERVER}; // 0 prevents players from being spectators

cvar_t mp_chattime = {"mp_chattime", "10", FCVAR_SERVER};

//CVARS FOR SKILL LEVEL SETTINGS
// Agrunt
cvar_t sk_agrunt_health1 = {"sk_agrunt_health1", "0"};
cvar_t sk_agrunt_health2 = {"sk_agrunt_health2", "0"};
cvar_t sk_agrunt_health3 = {"sk_agrunt_health3", "0"};

cvar_t sk_agrunt_dmg_punch1 = {"sk_agrunt_dmg_punch1", "0"};
cvar_t sk_agrunt_dmg_punch2 = {"sk_agrunt_dmg_punch2", "0"};
cvar_t sk_agrunt_dmg_punch3 = {"sk_agrunt_dmg_punch3", "0"};

// Apache
cvar_t sk_apache_health1 = {"sk_apache_health1", "0"};
cvar_t sk_apache_health2 = {"sk_apache_health2", "0"};
cvar_t sk_apache_health3 = {"sk_apache_health3", "0"};

// Barney
cvar_t sk_barney_health1 = {"sk_barney_health1", "0"};
cvar_t sk_barney_health2 = {"sk_barney_health2", "0"};
cvar_t sk_barney_health3 = {"sk_barney_health3", "0"};

// Barney
cvar_t sk_rebecca_health1 = {"sk_rebecca_health1", "0"};
cvar_t sk_rebecca_health2 = {"sk_rebecca_health2", "0"};
cvar_t sk_rebecca_health3 = {"sk_rebecca_health3", "0"};

// Bullsquid
cvar_t sk_bullsquid_health1 = {"sk_bullsquid_health1", "0"};
cvar_t sk_bullsquid_health2 = {"sk_bullsquid_health2", "0"};
cvar_t sk_bullsquid_health3 = {"sk_bullsquid_health3", "0"};

cvar_t sk_bullsquid_dmg_bite1 = {"sk_bullsquid_dmg_bite1", "0"};
cvar_t sk_bullsquid_dmg_bite2 = {"sk_bullsquid_dmg_bite2", "0"};
cvar_t sk_bullsquid_dmg_bite3 = {"sk_bullsquid_dmg_bite3", "0"};

cvar_t sk_bullsquid_dmg_whip1 = {"sk_bullsquid_dmg_whip1", "0"};
cvar_t sk_bullsquid_dmg_whip2 = {"sk_bullsquid_dmg_whip2", "0"};
cvar_t sk_bullsquid_dmg_whip3 = {"sk_bullsquid_dmg_whip3", "0"};

cvar_t sk_bullsquid_dmg_spit1 = {"sk_bullsquid_dmg_spit1", "0"};
cvar_t sk_bullsquid_dmg_spit2 = {"sk_bullsquid_dmg_spit2", "0"};
cvar_t sk_bullsquid_dmg_spit3 = {"sk_bullsquid_dmg_spit3", "0"};


// Big Momma
cvar_t sk_bigmomma_health_factor1 = {"sk_bigmomma_health_factor1", "1.0"};
cvar_t sk_bigmomma_health_factor2 = {"sk_bigmomma_health_factor2", "1.0"};
cvar_t sk_bigmomma_health_factor3 = {"sk_bigmomma_health_factor3", "1.0"};

cvar_t sk_bigmomma_dmg_slash1 = {"sk_bigmomma_dmg_slash1", "50"};
cvar_t sk_bigmomma_dmg_slash2 = {"sk_bigmomma_dmg_slash2", "50"};
cvar_t sk_bigmomma_dmg_slash3 = {"sk_bigmomma_dmg_slash3", "50"};

cvar_t sk_bigmomma_dmg_blast1 = {"sk_bigmomma_dmg_blast1", "100"};
cvar_t sk_bigmomma_dmg_blast2 = {"sk_bigmomma_dmg_blast2", "100"};
cvar_t sk_bigmomma_dmg_blast3 = {"sk_bigmomma_dmg_blast3", "100"};

cvar_t sk_bigmomma_radius_blast1 = {"sk_bigmomma_radius_blast1", "250"};
cvar_t sk_bigmomma_radius_blast2 = {"sk_bigmomma_radius_blast2", "250"};
cvar_t sk_bigmomma_radius_blast3 = {"sk_bigmomma_radius_blast3", "250"};

// Gargantua
cvar_t sk_gargantua_health1 = {"sk_gargantua_health1", "0"};
cvar_t sk_gargantua_health2 = {"sk_gargantua_health2", "0"};
cvar_t sk_gargantua_health3 = {"sk_gargantua_health3", "0"};

cvar_t sk_gargantua_dmg_slash1 = {"sk_gargantua_dmg_slash1", "0"};
cvar_t sk_gargantua_dmg_slash2 = {"sk_gargantua_dmg_slash2", "0"};
cvar_t sk_gargantua_dmg_slash3 = {"sk_gargantua_dmg_slash3", "0"};

cvar_t sk_gargantua_dmg_fire1 = {"sk_gargantua_dmg_fire1", "0"};
cvar_t sk_gargantua_dmg_fire2 = {"sk_gargantua_dmg_fire2", "0"};
cvar_t sk_gargantua_dmg_fire3 = {"sk_gargantua_dmg_fire3", "0"};

cvar_t sk_gargantua_dmg_stomp1 = {"sk_gargantua_dmg_stomp1", "0"};
cvar_t sk_gargantua_dmg_stomp2 = {"sk_gargantua_dmg_stomp2", "0"};
cvar_t sk_gargantua_dmg_stomp3 = {"sk_gargantua_dmg_stomp3", "0"};


// Hassassin
cvar_t sk_hassassin_health1 = {"sk_hassassin_health1", "0"};
cvar_t sk_hassassin_health2 = {"sk_hassassin_health2", "0"};
cvar_t sk_hassassin_health3 = {"sk_hassassin_health3", "0"};


// Headcrab
cvar_t sk_headcrab_health1 = {"sk_headcrab_health1", "0"};
cvar_t sk_headcrab_health2 = {"sk_headcrab_health2", "0"};
cvar_t sk_headcrab_health3 = {"sk_headcrab_health3", "0"};

cvar_t sk_headcrab_dmg_bite1 = {"sk_headcrab_dmg_bite1", "0"};
cvar_t sk_headcrab_dmg_bite2 = {"sk_headcrab_dmg_bite2", "0"};
cvar_t sk_headcrab_dmg_bite3 = {"sk_headcrab_dmg_bite3", "0"};


// Hgrunt
cvar_t sk_hgrunt_health1 = {"sk_hgrunt_health1", "0"};
cvar_t sk_hgrunt_health2 = {"sk_hgrunt_health2", "0"};
cvar_t sk_hgrunt_health3 = {"sk_hgrunt_health3", "0"};

cvar_t sk_hgrunt_kick1 = {"sk_hgrunt_kick1", "0"};
cvar_t sk_hgrunt_kick2 = {"sk_hgrunt_kick2", "0"};
cvar_t sk_hgrunt_kick3 = {"sk_hgrunt_kick3", "0"};

cvar_t sk_hgrunt_pellets1 = {"sk_hgrunt_pellets1", "0"};
cvar_t sk_hgrunt_pellets2 = {"sk_hgrunt_pellets2", "0"};
cvar_t sk_hgrunt_pellets3 = {"sk_hgrunt_pellets3", "0"};

cvar_t sk_hgrunt_gspeed1 = {"sk_hgrunt_gspeed1", "0"};
cvar_t sk_hgrunt_gspeed2 = {"sk_hgrunt_gspeed2", "0"};
cvar_t sk_hgrunt_gspeed3 = {"sk_hgrunt_gspeed3", "0"};

// Houndeye
cvar_t sk_houndeye_health1 = {"sk_houndeye_health1", "0"};
cvar_t sk_houndeye_health2 = {"sk_houndeye_health2", "0"};
cvar_t sk_houndeye_health3 = {"sk_houndeye_health3", "0"};

cvar_t sk_houndeye_dmg_blast1 = {"sk_houndeye_dmg_blast1", "0"};
cvar_t sk_houndeye_dmg_blast2 = {"sk_houndeye_dmg_blast2", "0"};
cvar_t sk_houndeye_dmg_blast3 = {"sk_houndeye_dmg_blast3", "0"};


// ISlave
cvar_t sk_islave_health1 = {"sk_islave_health1", "0"};
cvar_t sk_islave_health2 = {"sk_islave_health2", "0"};
cvar_t sk_islave_health3 = {"sk_islave_health3", "0"};

cvar_t sk_islave_dmg_claw1 = {"sk_islave_dmg_claw1", "0"};
cvar_t sk_islave_dmg_claw2 = {"sk_islave_dmg_claw2", "0"};
cvar_t sk_islave_dmg_claw3 = {"sk_islave_dmg_claw3", "0"};

cvar_t sk_islave_dmg_clawrake1 = {"sk_islave_dmg_clawrake1", "0"};
cvar_t sk_islave_dmg_clawrake2 = {"sk_islave_dmg_clawrake2", "0"};
cvar_t sk_islave_dmg_clawrake3 = {"sk_islave_dmg_clawrake3", "0"};

cvar_t sk_islave_dmg_zap1 = {"sk_islave_dmg_zap1", "0"};
cvar_t sk_islave_dmg_zap2 = {"sk_islave_dmg_zap2", "0"};
cvar_t sk_islave_dmg_zap3 = {"sk_islave_dmg_zap3", "0"};


// Icthyosaur
cvar_t sk_ichthyosaur_health1 = {"sk_ichthyosaur_health1", "0"};
cvar_t sk_ichthyosaur_health2 = {"sk_ichthyosaur_health2", "0"};
cvar_t sk_ichthyosaur_health3 = {"sk_ichthyosaur_health3", "0"};

cvar_t sk_ichthyosaur_shake1 = {"sk_ichthyosaur_shake1", "0"};
cvar_t sk_ichthyosaur_shake2 = {"sk_ichthyosaur_shake2", "0"};
cvar_t sk_ichthyosaur_shake3 = {"sk_ichthyosaur_shake3", "0"};


// Leech
cvar_t sk_leech_health1 = {"sk_leech_health1", "0"};
cvar_t sk_leech_health2 = {"sk_leech_health2", "0"};
cvar_t sk_leech_health3 = {"sk_leech_health3", "0"};

cvar_t sk_leech_dmg_bite1 = {"sk_leech_dmg_bite1", "0"};
cvar_t sk_leech_dmg_bite2 = {"sk_leech_dmg_bite2", "0"};
cvar_t sk_leech_dmg_bite3 = {"sk_leech_dmg_bite3", "0"};

// Controller
cvar_t sk_controller_health1 = {"sk_controller_health1", "0"};
cvar_t sk_controller_health2 = {"sk_controller_health2", "0"};
cvar_t sk_controller_health3 = {"sk_controller_health3", "0"};

cvar_t sk_controller_dmgzap1 = {"sk_controller_dmgzap1", "0"};
cvar_t sk_controller_dmgzap2 = {"sk_controller_dmgzap2", "0"};
cvar_t sk_controller_dmgzap3 = {"sk_controller_dmgzap3", "0"};

cvar_t sk_controller_speedball1 = {"sk_controller_speedball1", "0"};
cvar_t sk_controller_speedball2 = {"sk_controller_speedball2", "0"};
cvar_t sk_controller_speedball3 = {"sk_controller_speedball3", "0"};

cvar_t sk_controller_dmgball1 = {"sk_controller_dmgball1", "0"};
cvar_t sk_controller_dmgball2 = {"sk_controller_dmgball2", "0"};
cvar_t sk_controller_dmgball3 = {"sk_controller_dmgball3", "0"};

// Nihilanth
cvar_t sk_nihilanth_health1 = {"sk_nihilanth_health1", "0"};
cvar_t sk_nihilanth_health2 = {"sk_nihilanth_health2", "0"};
cvar_t sk_nihilanth_health3 = {"sk_nihilanth_health3", "0"};

cvar_t sk_nihilanth_zap1 = {"sk_nihilanth_zap1", "0"};
cvar_t sk_nihilanth_zap2 = {"sk_nihilanth_zap2", "0"};
cvar_t sk_nihilanth_zap3 = {"sk_nihilanth_zap3", "0"};

// Scientist
cvar_t sk_scientist_health1 = {"sk_scientist_health1", "0"};
cvar_t sk_scientist_health2 = {"sk_scientist_health2", "0"};
cvar_t sk_scientist_health3 = {"sk_scientist_health3", "0"};



// Zombie
cvar_t sk_zombie_health1 = {"sk_zombie_health1", "0"};
cvar_t sk_zombie_health2 = {"sk_zombie_health2", "0"};
cvar_t sk_zombie_health3 = {"sk_zombie_health3", "0"};

cvar_t sk_zombie_dmg_one_slash1 = {"sk_zombie_dmg_one_slash1", "0"};
cvar_t sk_zombie_dmg_one_slash2 = {"sk_zombie_dmg_one_slash2", "0"};
cvar_t sk_zombie_dmg_one_slash3 = {"sk_zombie_dmg_one_slash3", "0"};

cvar_t sk_zombie_dmg_both_slash1 = {"sk_zombie_dmg_both_slash1", "0"};
cvar_t sk_zombie_dmg_both_slash2 = {"sk_zombie_dmg_both_slash2", "0"};
cvar_t sk_zombie_dmg_both_slash3 = {"sk_zombie_dmg_both_slash3", "0"};


//Turret
cvar_t sk_turret_health1 = {"sk_turret_health1", "0"};
cvar_t sk_turret_health2 = {"sk_turret_health2", "0"};
cvar_t sk_turret_health3 = {"sk_turret_health3", "0"};


// MiniTurret
cvar_t sk_miniturret_health1 = {"sk_miniturret_health1", "0"};
cvar_t sk_miniturret_health2 = {"sk_miniturret_health2", "0"};
cvar_t sk_miniturret_health3 = {"sk_miniturret_health3", "0"};


// Sentry Turret
cvar_t sk_sentry_health1 = {"sk_sentry_health1", "0"};
cvar_t sk_sentry_health2 = {"sk_sentry_health2", "0"};
cvar_t sk_sentry_health3 = {"sk_sentry_health3", "0"};


// PLAYER WEAPONS

CVAR_REGISTER_SKILLVAR( sk_plr_knife, 10 );
CVAR_REGISTER_SKILLVAR( sk_plr_knifestab, 30 );
CVAR_REGISTER_SKILLVAR( sk_plr_machete, 30 );
CVAR_REGISTER_SKILLVAR( sk_plr_machetestab, 50 );
CVAR_REGISTER_SKILLVAR( sk_plr_9mm_bullet, 8 );
CVAR_REGISTER_SKILLVAR( sk_plr_45acp_bullet, 12 );
CVAR_REGISTER_SKILLVAR( sk_plr_357_bullet, 40 );
CVAR_REGISTER_SKILLVAR( sk_plr_9mmAR_bullet, 10 );
CVAR_REGISTER_SKILLVAR( sk_plr_556nato_bullet, 25 );
CVAR_REGISTER_SKILLVAR( sk_plr_762nato_bullet, 30 );
CVAR_REGISTER_SKILLVAR( sk_plr_338magnum_bullet, 60 );
CVAR_REGISTER_SKILLVAR( sk_plr_9mmAR_grenade, 100 );
CVAR_REGISTER_SKILLVAR( sk_plr_buckshot, 12 );
CVAR_REGISTER_SKILLVAR( sk_plr_rpg, 100 );
CVAR_REGISTER_SKILLVAR( sk_plr_hand_grenade, 100 );
CVAR_REGISTER_SKILLVAR( sk_plr_tripmine, 150 );


CVAR_REGISTER_SKILLVAR_SPECIFIC( sk_12mm_bullet, 8, 10, 11 );
CVAR_REGISTER_SKILLVAR_SPECIFIC( sk_9mmAR_bullet, 3, 4, 5 );
CVAR_REGISTER_SKILLVAR_SPECIFIC( sk_9mm_bullet, 4, 5, 8 );

CVAR_REGISTER_SKILLVAR_SPECIFIC( sk_suitcharger, 75, 50, 35 );
CVAR_REGISTER_SKILLVAR_SPECIFIC( sk_healthcharger, 50, 40, 25 );
CVAR_REGISTER_SKILLVAR_SPECIFIC( sk_battery, 35, 25, 20 );
CVAR_REGISTER_SKILLVAR_SPECIFIC( sk_healthkit, 20, 15, 10 );
CVAR_REGISTER_SKILLVAR_SPECIFIC( sk_scientist_heal, 35, 30, 25 );


// monster damage adjusters
cvar_t sk_monster_head1 = {"sk_monster_head1", "2"};
cvar_t sk_monster_head2 = {"sk_monster_head2", "2"};
cvar_t sk_monster_head3 = {"sk_monster_head3", "2"};

cvar_t sk_monster_chest1 = {"sk_monster_chest1", "1"};
cvar_t sk_monster_chest2 = {"sk_monster_chest2", "1"};
cvar_t sk_monster_chest3 = {"sk_monster_chest3", "1"};

cvar_t sk_monster_stomach1 = {"sk_monster_stomach1", "1"};
cvar_t sk_monster_stomach2 = {"sk_monster_stomach2", "1"};
cvar_t sk_monster_stomach3 = {"sk_monster_stomach3", "1"};

cvar_t sk_monster_arm1 = {"sk_monster_arm1", "1"};
cvar_t sk_monster_arm2 = {"sk_monster_arm2", "1"};
cvar_t sk_monster_arm3 = {"sk_monster_arm3", "1"};

cvar_t sk_monster_leg1 = {"sk_monster_leg1", "1"};
cvar_t sk_monster_leg2 = {"sk_monster_leg2", "1"};
cvar_t sk_monster_leg3 = {"sk_monster_leg3", "1"};

// player damage adjusters
cvar_t sk_player_head1 = {"sk_player_head1", "2"};
cvar_t sk_player_head2 = {"sk_player_head2", "2"};
cvar_t sk_player_head3 = {"sk_player_head3", "2"};

cvar_t sk_player_chest1 = {"sk_player_chest1", "1"};
cvar_t sk_player_chest2 = {"sk_player_chest2", "1"};
cvar_t sk_player_chest3 = {"sk_player_chest3", "1"};

cvar_t sk_player_stomach1 = {"sk_player_stomach1", "1"};
cvar_t sk_player_stomach2 = {"sk_player_stomach2", "1"};
cvar_t sk_player_stomach3 = {"sk_player_stomach3", "1"};

cvar_t sk_player_arm1 = {"sk_player_arm1", "1"};
cvar_t sk_player_arm2 = {"sk_player_arm2", "1"};
cvar_t sk_player_arm3 = {"sk_player_arm3", "1"};

cvar_t sk_player_leg1 = {"sk_player_leg1", "1"};
cvar_t sk_player_leg2 = {"sk_player_leg2", "1"};
cvar_t sk_player_leg3 = {"sk_player_leg3", "1"};

// END Cvars for Skill Level settings

// Register your console variables here
// This gets called one time when the game is initialied

void GameDLLInit()
{
	// Register cvars here:

	g_psv_gravity = CVAR_GET_POINTER("sv_gravity");
	g_psv_aim = CVAR_GET_POINTER("sv_aim");
	g_footsteps = CVAR_GET_POINTER("mp_footsteps");
	g_psv_cheats = CVAR_GET_POINTER("sv_cheats");

	if (!FileSystem_LoadFileSystem())
	{
		//Shut the game down as soon as possible.
		SERVER_COMMAND("quit\n");
		return;
	}

	CVAR_REGISTER(&displaysoundlist);
	CVAR_REGISTER(&allow_spectators);

	CVAR_REGISTER(&teamplay);
	CVAR_REGISTER(&fraglimit);
	CVAR_REGISTER(&timelimit);

	CVAR_REGISTER(&fragsleft);
	CVAR_REGISTER(&timeleft);

	CVAR_REGISTER(&friendlyfire);
	CVAR_REGISTER(&falldamage);
	CVAR_REGISTER(&weaponstay);
	CVAR_REGISTER(&forcerespawn);
	CVAR_REGISTER(&flashlight);
	CVAR_REGISTER(&aimcrosshair);
	CVAR_REGISTER(&decalfrequency);
	CVAR_REGISTER(&teamlist);
	CVAR_REGISTER(&teamoverride);
	CVAR_REGISTER(&defaultteam);
	CVAR_REGISTER(&allowmonsters);

	CVAR_REGISTER(&mp_chattime);

	// REGISTER CVARS FOR SKILL LEVEL STUFF
	// Alien Grunt
	CVAR_REGISTER_SKILL(sk_agrunt_health);
	CVAR_REGISTER_SKILL(sk_agrunt_dmg_punch);


	// Apache
	CVAR_REGISTER_SKILL(sk_apache_health);


	// Barney
	CVAR_REGISTER_SKILL(sk_barney_health);
	CVAR_REGISTER_SKILL(sk_rebecca_health);


	// Bullsquid
	CVAR_REGISTER_SKILL(sk_bullsquid_health);
	CVAR_REGISTER_SKILL(sk_bullsquid_dmg_bite);
	CVAR_REGISTER_SKILL(sk_bullsquid_dmg_whip);
	CVAR_REGISTER_SKILL(sk_bullsquid_dmg_spit);


	// Big Momma
	CVAR_REGISTER_SKILL(sk_bigmomma_health_factor);
	CVAR_REGISTER_SKILL(sk_bigmomma_dmg_slash);
	CVAR_REGISTER_SKILL(sk_bigmomma_dmg_blast);
	CVAR_REGISTER_SKILL(sk_bigmomma_radius_blast);


	// Gargantua
	CVAR_REGISTER_SKILL(sk_gargantua_health);
	CVAR_REGISTER_SKILL(sk_gargantua_dmg_slash);
	CVAR_REGISTER_SKILL(sk_gargantua_dmg_fire);
	CVAR_REGISTER_SKILL(sk_gargantua_dmg_stomp);


	// Hassassin
	CVAR_REGISTER_SKILL(sk_hassassin_health);


	// Headcrab
	CVAR_REGISTER_SKILL(sk_headcrab_health);
	CVAR_REGISTER_SKILL(sk_headcrab_dmg_bite);


	// Hgrunt
	CVAR_REGISTER_SKILL(sk_hgrunt_health);
	CVAR_REGISTER_SKILL(sk_hgrunt_kick);
	CVAR_REGISTER_SKILL(sk_hgrunt_pellets);
	CVAR_REGISTER_SKILL(sk_hgrunt_gspeed);


	// Houndeye
	CVAR_REGISTER_SKILL(sk_houndeye_health);
	CVAR_REGISTER_SKILL(sk_houndeye_dmg_blast);


	// ISlave
	CVAR_REGISTER_SKILL(sk_islave_health);
	CVAR_REGISTER_SKILL(sk_islave_dmg_claw);
	CVAR_REGISTER_SKILL(sk_islave_dmg_clawrake);
	CVAR_REGISTER_SKILL(sk_islave_dmg_zap);


	// Icthyosaur
	CVAR_REGISTER_SKILL(sk_ichthyosaur_health);
	CVAR_REGISTER_SKILL(sk_ichthyosaur_shake);


	// Leech
	CVAR_REGISTER_SKILL(sk_leech_health);
	CVAR_REGISTER_SKILL(sk_leech_dmg_bite);


	// Controller
	CVAR_REGISTER_SKILL(sk_controller_health);
	CVAR_REGISTER_SKILL(sk_controller_dmgzap);
	CVAR_REGISTER_SKILL(sk_controller_speedball);
	CVAR_REGISTER_SKILL(sk_controller_dmgball);


	// Nihilanth
	CVAR_REGISTER_SKILL(sk_nihilanth_health);
	CVAR_REGISTER_SKILL(sk_nihilanth_zap);


	// Scientist
	CVAR_REGISTER_SKILL(sk_scientist_health);


	// Zombie
	CVAR_REGISTER_SKILL(sk_zombie_health);
	CVAR_REGISTER_SKILL(sk_zombie_dmg_one_slash);
	CVAR_REGISTER_SKILL(sk_zombie_dmg_both_slash);


	//Turret
	CVAR_REGISTER_SKILL(sk_turret_health);


	// MiniTurret
	CVAR_REGISTER_SKILL(sk_miniturret_health);


	// Sentry Turret
	CVAR_REGISTER_SKILL(sk_sentry_health);


	// PLAYER WEAPONS

	CVAR_REGISTER_SKILL(sk_plr_knife);
	CVAR_REGISTER_SKILL(sk_plr_knifestab);
	CVAR_REGISTER_SKILL(sk_plr_machete);
	CVAR_REGISTER_SKILL(sk_plr_machetestab);

	CVAR_REGISTER_SKILL(sk_plr_9mm_bullet);
	CVAR_REGISTER_SKILL(sk_plr_45acp_bullet);
	CVAR_REGISTER_SKILL(sk_plr_357_bullet);
	CVAR_REGISTER_SKILL(sk_plr_9mmAR_bullet);
	CVAR_REGISTER_SKILL(sk_plr_556nato_bullet);
	CVAR_REGISTER_SKILL(sk_plr_762nato_bullet);
	CVAR_REGISTER_SKILL(sk_plr_338magnum_bullet);

	// M203 grenade
	CVAR_REGISTER_SKILL(sk_plr_9mmAR_grenade);


	// Shotgun buckshot
	CVAR_REGISTER_SKILL(sk_plr_buckshot);


	// RPG
	CVAR_REGISTER_SKILL(sk_plr_rpg);


	// Hand Grendade
	CVAR_REGISTER_SKILL(sk_plr_hand_grenade);


	// WORLD WEAPONS
	CVAR_REGISTER_SKILL(sk_12mm_bullet);
	CVAR_REGISTER_SKILL(sk_9mmAR_bullet);
	CVAR_REGISTER_SKILL(sk_9mm_bullet);

	// HEALTH/SUIT CHARGE DISTRIBUTION
	CVAR_REGISTER_SKILL(sk_suitcharger);

	CVAR_REGISTER_SKILL(sk_battery);

	CVAR_REGISTER_SKILL(sk_healthcharger);

	CVAR_REGISTER_SKILL(sk_healthkit);

	CVAR_REGISTER_SKILL(sk_scientist_heal);

	// monster damage adjusters
	CVAR_REGISTER_SKILL(sk_monster_head);

	CVAR_REGISTER_SKILL(sk_monster_chest);

	CVAR_REGISTER_SKILL(sk_monster_stomach);

	CVAR_REGISTER_SKILL(sk_monster_arm);

	CVAR_REGISTER_SKILL(sk_monster_leg);

	// player damage adjusters
	CVAR_REGISTER_SKILL(sk_player_head);

	CVAR_REGISTER_SKILL(sk_player_chest);

	CVAR_REGISTER_SKILL(sk_player_stomach);

	CVAR_REGISTER_SKILL(sk_player_arm);

	CVAR_REGISTER_SKILL(sk_player_leg);
	// END REGISTER CVARS FOR SKILL LEVEL STUFF

	SERVER_COMMAND("exec skill.cfg\n");
}

void GameDLLShutdown()
{
	FileSystem_FreeFileSystem();
}
