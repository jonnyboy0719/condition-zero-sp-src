
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "triggers.h"

class CSeqSoundPrecache : public CPointEntity
{
public:
	void Spawn() override;
	void Precache() override;
};
LINK_ENTITY_TO_CLASS(seq_snd, CSeqSoundPrecache);

void CSeqSoundPrecache::Precache()
{
	if (!m_szCustomSndPrecache.empty())
		PRECACHE_SOUND(m_szCustomSndPrecache.c_str());
}

void CSeqSoundPrecache::Spawn()
{
	Precache();
	pev->solid = SOLID_NOT;
}
