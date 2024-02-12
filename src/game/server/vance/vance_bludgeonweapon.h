//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		The class from which all bludgeon melee
//				weapons are derived. 
//
// $Workfile:     $
// $Date:         $
// $NoKeywords: $
//=============================================================================//

#include "vance_baseweapon_shared.h"
#include "props.h"

#ifndef VANCEBLUDGEONWEAPON_H
#define VANCEBLUDGEONWEAPON_H

//=========================================================
// CBaseHLBludgeonWeapon 
//=========================================================
class CVanceBludgeonWeapon : public CBaseVanceWeapon
{
	DECLARE_CLASS(CVanceBludgeonWeapon, CBaseVanceWeapon);
public:
	CVanceBludgeonWeapon();

	DECLARE_SERVERCLASS();

	virtual	void	Spawn(void);
	virtual	void	Precache(void);

	//Attack functions
	virtual	void	PrimaryAttack(void);
	virtual	void	SecondaryAttack(void) { return;	};

	virtual void	ItemPostFrame(void);

	//Functions to select animation sequences 
	virtual Activity	GetPrimaryAttackActivity(void) { return	m_bSwingSwitch ? ACT_VM_SWING_LEFT : ACT_VM_SWING_RIGHT; }
	virtual Activity	GetSecondaryAttackActivity(void) { return	ACT_VM_HITCENTER2; }

	virtual	float	GetFireRate(void) { return	GetViewModelSequenceDuration(); }
	virtual float	GetRange(void) { return	32.0f; }
	virtual	float	GetDamageForActivity(Activity hitActivity) { return	1.0f; }

	virtual int		CapabilitiesGet(void);
	virtual	int		WeaponMeleeAttack1Condition(float flDot, float flDist);

	virtual void	Operator_HandleAnimEvent(animevent_t *pEvent, CBaseCombatCharacter *pOperator);

	virtual void	DefaultTouch(CBaseEntity *pOther);
	virtual void	OnPickedUp(CBaseCombatCharacter *pNewOwner);
	virtual bool	DefaultDeploy(char *szViewModel, char *szWeaponModel, int iActivity, char *szAnimExt);

	virtual bool	SendWeaponAnim(int iActivity);
	virtual void	ItemHolsterFrame();

protected:
	virtual	void	ImpactEffect(trace_t& trace);

private:
	bool			ImpactWater(const Vector& start, const Vector& end);
	void			Swing();
	void			SwingThink();
	void			Hit(trace_t& traceHit, Activity nHitActivity);
	void			ChooseIntersectionPointAndActivity(trace_t& hitTrace, const Vector& mins, const Vector& maxs, CBasePlayer* pOwner);

	float			m_flSwing;
	bool			m_bSwingSwitch;
	EHANDLE			m_hHitEnt;

	bool			m_bReadyToThrow = false;
	bool			m_bWaitingForDeathSetup = false;
	bool			m_bWaitingForDeath = false;

	float m_flNextGruntSound = 0.0f;
	float m_flNextGrunt2Sound = 0.0f;
};

#endif

// custom phsyics prop class used for thrown weapons, has a minimum damage amount when hitting enemies and will revert back to the weapon on the first hit
class CThrownBludgeonProp : public CPhysicsProp
{
	DECLARE_CLASS(CThrownBludgeonProp, CPhysicsProp);

public:
	void VPhysicsCollision(int index, gamevcollisionevent_t *pEvent) override;
	const char *m_sClassname;
	float m_fDamage;

	virtual void Think();
private:
	bool m_bReadyToSwap = false;
	Vector m_vPositionLast;
	bool m_bPositionLast = false;
};