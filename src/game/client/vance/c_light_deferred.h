#ifndef C_LIGHT_DEFERRED_H
#define C_LIGHT_DEFERRED_H

class C_LightDeferred : public CBaseEntity
{
	DECLARE_CLASS(C_LightDeferred, CBaseEntity);
public:
	DECLARE_CLIENTCLASS();

	C_LightDeferred();
	~C_LightDeferred();

	virtual int ObjectCaps() {
		return FCAP_DONT_SAVE;
	};


	Vector m_vColor;
	Vector m_vPos;
	float m_flBrightness;
	float m_flRadius;

	virtual void OnDataChanged(DataUpdateType_t type);

	virtual void ApplyDataToLight();
	virtual void ClientThink();

	virtual bool ShouldDraw() { return false; }
	virtual int DrawModel(int flags) { return 0; }
	virtual void FireEvent(const Vector& origin, const QAngle& angles, int event, const char* options) {}
	def_light_t* m_light;
};

#endif