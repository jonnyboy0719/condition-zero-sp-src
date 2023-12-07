#ifndef SERVER_HUMAN_BARNEY_H
#define SERVER_HUMAN_BARNEY_H

//=========================================================
// Monster's Anim Events Go Here
//=========================================================
// first flag is barney dying for scripted sequences?
#define BARNEY_AE_DRAW (2)
#define BARNEY_AE_SHOOT (3)
#define BARNEY_AE_HOLSTER (4)
#define BARNEY_AE_RELOAD (5)
#define BARNEY_AE_RELOAD_FINISHED (6)

#define BARNEY_BODY_GUNHOLSTERED 0
#define BARNEY_BODY_GUNDRAWN 1
#define BARNEY_BODY_GUNGONE 2

#define REBECCA_AE_DRAW_DEAGLE (7)
#define REBECCA_BODY_GUN_DEAGLE 1
#define REBECCA_BODY_GUN_GLOCK 2
#define REBECCA_BODY_GUN_NONE 3

#define BODY_GROUP 0
#define HEAD_GROUP 1
#define GUN_GROUP 2

//=========================================================
// monster-specific schedule types
//=========================================================
enum
{
	SCHED_POLICE_RELOAD = LAST_COMMON_SCHEDULE + 1,
};

enum BarneySay
{
	BARNSAY_ATTACK,
	BARNSAY_KILL,
	BARNSAY_POK,
	BARNSAY_PAIN,
	BARNSAY_DIE,
	BARNSAY_SHOT,
	BARNSAY_MAD,
	BARNSAY_GRENADE,
	BARNSAY_CANTSEE,
	BARNSAY_SMOKED
};

class CBarney : public CTalkMonster
{
public:
	void Spawn() override;
	void Precache() override;
	void SetYawSpeed() override;
	int ISoundMask() override;
	virtual void BarneyFirePistol();
	void AlertSound();
	int Classify() override;
	void CheckAmmo() override;
	void HandleAnimEvent(MonsterEvent_t* pEvent) override;

	void RunTask(Task_t* pTask) override;
	void StartTask(Task_t* pTask) override;
	int ObjectCaps() override { return CTalkMonster::ObjectCaps() | FCAP_IMPULSE_USE; }
	bool TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType);
	bool CheckRangeAttack1(float flDot, float flDist) override;

	void DeclineFollowing() override;

	virtual void OnSentenceSay( BarneySay say );

	void OnNPCCoughing() override;
	void OnNPCFlashed() override;

	// Override these to set behavior
	Schedule_t* GetScheduleOfType(int Type) override;
	Schedule_t* GetSchedule() override;
	MONSTERSTATE GetIdealState() override;

	void DeathSound();
	void PainSound();

	void TalkInit();

	void TraceAttack(entvars_t* pevAttacker, float flDamage, Vector vecDir, TraceResult* ptr, int bitsDamageType) override;
	void Killed(entvars_t* pevAttacker, int iGib);

	bool Save(CSave& save) override;
	bool Restore(CRestore& restore) override;
	static TYPEDESCRIPTION m_SaveData[];

	bool m_fGunDrawn;
	float m_painTime;
	float m_checkAttackTime;
	bool m_lastAttackCheck;

	bool m_bFlashBanged;
	bool m_bCoughing;

	// UNDONE: What is this for?  It isn't used?
	float m_flPlayerDamage; // how much pain has the player inflicted on me?

	CUSTOM_SCHEDULES;
};

#endif