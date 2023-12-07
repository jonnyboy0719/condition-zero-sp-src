
#pragma once

//
//-----------------------------------------------------
//
class CHudObjectives : public CHudBase
{
public:
	bool Init() override;
	bool VidInit() override;
	bool Draw(float fTime) override;
	void ColorBlend(color24& rgb, color24 rgb_org);
	bool MsgFunc_OBJ(const char* pszName, int iSize, void* pbuf);

	void Show();
	void Hide();
	float m_fFade;
	bool m_bDoFade;
};
