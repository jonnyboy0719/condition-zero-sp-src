
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "triggers.h"
#include "weapons.h"

struct Particle_t
{
	int texture;
	float time;
	float blendtime;
	float scale;
	Vector origin;
};

const int MAX_PARTICLES = (1<<5);

// Note: This entity does not even work in Deleted Scenes.
class CEnvParticleEmitter : public CPointEntity
{
public:
	bool KeyValue(KeyValueData* pkvd) override;
	void Spawn() override;
	void Precache() override;
	void OnEmitterThink();
	void EXPORT ManagerUse(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value);

	Vector BlendTo( const Vector &blend_from, const Vector &blend_to, float &flLastTimeCheck, float blend_time );
	void BlendMA( const Vector &start, float scale, const Vector &direction, Vector &dest );
	void BlendScale( const Vector& in, float scale, Vector& result );

	bool m_bActive = true;
	bool m_bHasTarget = false;

	int m_iSprite;
	int m_particle_noise;
	int m_scale_speed;
	int m_fade_speed;
	int m_particle_scale;
	int m_particle_count;
	int m_particle_speed;

	float m_particle_gravity;
	float m_frequency;

	Vector m_particle_avelocity;
	Vector m_TargetLOC;

	//string_t m_vis_point;
	//string_t m_target_direction;
	string_t m_particle;
	string_t m_target_origin;

	Particle_t particles[MAX_PARTICLES];

	bool Save(CSave& save) override;
	bool Restore(CRestore& restore) override;
	static TYPEDESCRIPTION m_SaveData[];
};
//LINK_ENTITY_TO_CLASS(env_particle_emitter, CEnvParticleEmitter);


TYPEDESCRIPTION CEnvParticleEmitter::m_SaveData[] =
{
	DEFINE_FIELD(CEnvParticleEmitter, m_bActive, FIELD_BOOLEAN),
	DEFINE_FIELD(CEnvParticleEmitter, m_bHasTarget, FIELD_BOOLEAN),
	DEFINE_FIELD(CEnvParticleEmitter, m_iSprite, FIELD_INTEGER),
	DEFINE_FIELD(CEnvParticleEmitter, m_scale_speed, FIELD_INTEGER),
	DEFINE_FIELD(CEnvParticleEmitter, m_particle_noise, FIELD_INTEGER),
	DEFINE_FIELD(CEnvParticleEmitter, m_fade_speed, FIELD_INTEGER),
	DEFINE_FIELD(CEnvParticleEmitter, m_frequency, FIELD_FLOAT),
	DEFINE_FIELD(CEnvParticleEmitter, m_particle_scale, FIELD_INTEGER),
	DEFINE_FIELD(CEnvParticleEmitter, m_particle_count, FIELD_INTEGER),
	DEFINE_FIELD(CEnvParticleEmitter, m_particle_speed, FIELD_INTEGER),
	DEFINE_FIELD(CEnvParticleEmitter, m_particle_gravity, FIELD_FLOAT),
	DEFINE_FIELD(CEnvParticleEmitter, m_particle, FIELD_STRING),
	//DEFINE_FIELD(CEnvParticleEmitter, m_target_direction, FIELD_STRING),
	//DEFINE_FIELD(CEnvParticleEmitter, m_vis_point, FIELD_STRING),
	DEFINE_FIELD(CEnvParticleEmitter, m_target_origin, FIELD_STRING),
	DEFINE_FIELD(CEnvParticleEmitter, m_particle_avelocity, FIELD_VECTOR),
	DEFINE_FIELD(CEnvParticleEmitter, m_TargetLOC, FIELD_VECTOR),
};
IMPLEMENT_SAVERESTORE(CEnvParticleEmitter, CPointEntity);


bool CEnvParticleEmitter::KeyValue(KeyValueData* pkvd)
{
	if (FStrEq(pkvd->szKeyName, "enabled"))
	{
		m_bActive = atoi(pkvd->szValue) >= 1 ? true : false;
		return true;
	}
	else if (FStrEq(pkvd->szKeyName, "particle_texture"))
	{
		m_particle = ALLOC_STRING(pkvd->szValue);
		return true;
	}
	else if (FStrEq(pkvd->szKeyName, "vis_point"))
	{
		// NOT USED
		//m_vis_point = ALLOC_STRING(pkvd->szValue);
		return true;
	}
	else if (FStrEq(pkvd->szKeyName, "target_direction"))
	{
		// NOT USED
		//m_target_direction = ALLOC_STRING(pkvd->szValue);
		return true;
	}
	else if (FStrEq(pkvd->szKeyName, "scale_speed"))
	{
		m_scale_speed = atoi(pkvd->szValue);
		return true;
	}
	else if (FStrEq(pkvd->szKeyName, "fade_speed"))
	{
		m_fade_speed = atoi(pkvd->szValue);
		return true;
	}
	else if (FStrEq(pkvd->szKeyName, "frequency"))
	{
		m_frequency = atof(pkvd->szValue);
		return true;
	}
	else if (FStrEq(pkvd->szKeyName, "particle_avelocity"))
	{
		// Convert "0 0 0" > Vector( 0, 0, 0 )
		UTIL_StringToVector(m_particle_avelocity, pkvd->szValue);
		return true;
	}
	else if (FStrEq(pkvd->szKeyName, "particle_life"))
	{
		// NOT USED BY TE_SMOKE
		// particle_life(integer) : "Particle Life" : 0
		return true;
	}
	else if (FStrEq(pkvd->szKeyName, "particle_scale"))
	{
		m_particle_scale = atoi(pkvd->szValue);
		// particle_scale(integer) : "Particle Scale" : 0
		return true;
	}
	else if (FStrEq(pkvd->szKeyName, "particle_gravity"))
	{
		float var = atof(pkvd->szValue);
		if ( var < 0.0 ) var = 0.0f;
		else if ( var > 1.0 ) var = 1.0;
		m_particle_gravity = var;
		// particle_gravity(integer) : "Particle Gravity 1.0 - 0.0" : 0
		return true;
	}
	else if (FStrEq(pkvd->szKeyName, "particle_count"))
	{
		m_particle_count = atoi(pkvd->szValue);
		return true;
	}
	else if (FStrEq(pkvd->szKeyName, "particle_noise"))
	{
		m_particle_noise = atoi(pkvd->szValue);
		return true;
	}
	else if (FStrEq(pkvd->szKeyName, "particle_speed"))
	{
		m_particle_speed = atoi(pkvd->szValue);
		return true;
	}
	else if (FStrEq(pkvd->szKeyName, "target_origin"))
	{
		m_target_origin = ALLOC_STRING(pkvd->szValue);
		return true;
	}
	return false;
}


void CEnvParticleEmitter::Spawn()
{
	Precache();

	m_bHasTarget = false;

	// Clear the memory
	memset(particles, 0, sizeof(Particle_t));

	Vector vecSpot = pev->origin;
	for ( int i = 0; i < m_particle_count; i++ )
	{
		Particle_t &particle = particles[i];
		particle.origin = vecSpot;
		particle.scale = 0;
		particle.time = gpGlobals->time + m_frequency;
		particle.texture = m_iSprite;
	}

	// If we got a target, use that as our "destination" for our particles
	if (!FStringNull(m_target_origin))
	{
		auto pFind = FIND_ENTITY_BY_TARGETNAME( NULL, STRING(m_target_origin) );
		if ( !FNullEnt( pFind ) )
		{
			m_TargetLOC = CBaseEntity::Instance( pFind )->pev->origin;
			m_bHasTarget = true;
		}
	}

	pev->solid = SOLID_NOT;
	SetUse(&CEnvParticleEmitter::ManagerUse);
#if 0
	SetThink(&CEnvParticleEmitter::OnEmitterThink);
	pev->nextthink = gpGlobals->time + RANDOM_FLOAT(0.5, 1.0);
#endif
}


void CEnvParticleEmitter::Precache()
{
	// Use our particle, but if it's not valid, then use default smoke
	if (!FStringNull(m_particle))
		m_iSprite = PRECACHE_MODEL(STRING(m_particle));
	else
		m_iSprite = g_sModelIndexSmoke;
}


void CEnvParticleEmitter::OnEmitterThink()
{
	pev->nextthink = gpGlobals->time + 0.1;
	if ( !m_bActive ) return;

	for ( int i = 0; i < m_particle_count; i++ )
	{
		Particle_t &particle = particles[i];
		if ( particle.time > gpGlobals->time )
			continue;

		if ( particle.scale < m_particle_scale )
		{
			// Set our previous time here, so we can properly blend the origin points.
			if ( particle.scale == 0 )
				particle.blendtime = gpGlobals->time;
			particle.scale += m_scale_speed;
		}
		else
		{
			// Reset particle
			particle.time = gpGlobals->time + m_frequency;
			particle.scale = 0;
			particle.origin = pev->origin;
			continue;
		}

		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, particle.origin );
			WRITE_BYTE( TE_SPRITE );
		//	WRITE_BYTE( TE_SMOKE );
			// Cordinates
			WRITE_COORD( particle.origin.x );
			WRITE_COORD( particle.origin.y );
			WRITE_COORD( particle.origin.z );
			// Sprite model index
			WRITE_SHORT( particle.texture );
			// Sprite scale
			WRITE_BYTE( particle.scale );
			// Sprite framerate
			WRITE_BYTE( 255 );
			//WRITE_BYTE( m_fade_speed );
		MESSAGE_END();

		Vector dest = particle.origin;

		// If we got a target, head there instead.
		if ( m_bHasTarget )
			dest = m_TargetLOC;

		// Change their origin due to vel and grav
		dest.x + m_particle_avelocity.x + m_particle_speed + RANDOM_FLOAT(-m_particle_noise, m_particle_noise);
		dest.y + m_particle_avelocity.y + m_particle_speed + RANDOM_FLOAT(-m_particle_noise, m_particle_noise);
		dest.z + m_particle_avelocity.z + m_particle_speed + RANDOM_FLOAT(-m_particle_noise, m_particle_noise);

		// Now, reduce because of gravity
		dest.z -= m_particle_gravity;

		// Make sure we blend our vectors.
		particle.origin = BlendTo( particle.origin, dest, particle.blendtime, 1.0f );
	}
}


void CEnvParticleEmitter::ManagerUse(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
{
	// Toggle it
	m_bActive = !m_bActive;
}

Vector CEnvParticleEmitter::BlendTo( const Vector& blend_from, const Vector& blend_to, float& flLastTimeCheck, float blend_time )
{
	// Don't update if this is invalid
	if ( !flLastTimeCheck && !blend_time ) return blend_to;

	// Clear if blend is over
	if ( (flLastTimeCheck + blend_time) < gpGlobals->time )
	{
		flLastTimeCheck = 0;
		return blend_to;
	}

	// Smooth it out
	float interp = (gpGlobals->time - flLastTimeCheck) / blend_time;
	Vector vOut;
	BlendScale( blend_from, (1.0 - interp), vOut );
	BlendMA( vOut, interp, blend_to, vOut );
	return vOut;
}

void CEnvParticleEmitter::BlendMA( const Vector& start, float scale, const Vector& direction, Vector& dest )
{
	dest.x = start.x + direction.x * scale;
	dest.y = start.y + direction.y * scale;
	dest.z = start.z + direction.z * scale;
}

void CEnvParticleEmitter::BlendScale( const Vector& in, float scale, Vector& result )
{
	result.x = in.x * scale;
	result.y = in.y * scale;
	result.z = in.z * scale;
}
