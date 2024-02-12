
#include "vphysics_interface.h"
#include "datamap.h"
#include "baseplayer_shared.h"
#include "model_types.h"
#include "physobj.h"

#pragma once

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
// derive from this so we can add save/load data to it
struct VANCEgame_shadowcontrol_params_t : public hlshadowcontrol_params_t
{
//	Vector targetPosition;
	//QAngle targetRotation;
	//float maxAngular;
	//float maxDampAngular;
	//float maxSpeed;
	//float maxDampSpeed;
	//float dampFactor;
	//float teleportDistance;
	DECLARE_SIMPLE_DATADESC();
};



class CGrabController : public IMotionEvent
{
	DECLARE_SIMPLE_DATADESC();

public:

	CGrabController(void);
	~CGrabController(void);
	void AttachEntity(CBasePlayer *pPlayer, CBaseEntity *pEntity, IPhysicsObject *pPhys, bool bIsMegaPhysCannon, const Vector &vGrabPosition, bool bUseGrabPosition);
	void DetachEntity(bool bClearVelocity);
	void OnRestore();

	bool UpdateObject(CBasePlayer *pPlayer, float flError);

	void SetTargetPosition(const Vector &target, const QAngle &targetOrientation);
	float ComputeError();
	float GetLoadWeight(void) const { return m_flLoadWeight; }
	void SetAngleAlignment(float alignAngleCosine) { m_angleAlignment = alignAngleCosine; }
	void SetIgnorePitch(bool bIgnore) { m_bIgnoreRelativePitch = bIgnore; }
	QAngle TransformAnglesToPlayerSpace(const QAngle &anglesIn, CBasePlayer *pPlayer);
	QAngle TransformAnglesFromPlayerSpace(const QAngle &anglesIn, CBasePlayer *pPlayer);

	CBaseEntity *GetAttached() { return (CBaseEntity *)m_attachedEntity; }

	IMotionEvent::simresult_e Simulate(IPhysicsMotionController *pController, IPhysicsObject *pObject, float deltaTime, Vector &linear, AngularImpulse &angular);
	float GetSavedMass(IPhysicsObject *pObject);

	bool IsObjectAllowedOverhead(CBaseEntity *pEntity);

private:
	// Compute the max speed for an attached object
	void ComputeMaxSpeed(CBaseEntity *pEntity, IPhysicsObject *pPhysics);
	VANCEgame_shadowcontrol_params_t	m_shadow;
	float			m_timeToArrive;
	float			m_errorTime;
	float			m_error;
	float			m_contactAmount;
	float			m_angleAlignment;
	bool			m_bCarriedEntityBlocksLOS;
	bool			m_bIgnoreRelativePitch;

	float			m_flLoadWeight;
	float			m_savedRotDamping[VPHYSICS_MAX_OBJECT_LIST_COUNT];
	float			m_savedMass[VPHYSICS_MAX_OBJECT_LIST_COUNT];
	EHANDLE			m_attachedEntity;
	QAngle			m_vecPreferredCarryAngles;
	bool			m_bHasPreferredCarryAngles;
	float			m_flDistanceOffset;

	QAngle			m_attachedAnglesPlayerSpace;
	Vector			m_attachedPositionObjectSpace;

	IPhysicsMotionController *m_controller;

	bool			m_bAllowObjectOverhead; // Can the player hold this object directly overhead? (Default is NO)

	// NVNT player controlling this grab controller
	CBasePlayer*	m_pControllingPlayer;

	friend class CWeaponPhysCannon;
};


class VANCECTraceFilterNoOwnerTest : public CTraceFilterSimple
{
public:
	DECLARE_CLASS(VANCECTraceFilterNoOwnerTest, CTraceFilterSimple);

	VANCECTraceFilterNoOwnerTest(const IHandleEntity *passentity, int collisionGroup)
		: CTraceFilterSimple(NULL, collisionGroup), m_pPassNotOwner(passentity)
	{
	}

	virtual bool ShouldHitEntity(IHandleEntity *pHandleEntity, int contentsMask)
	{
		if (pHandleEntity != m_pPassNotOwner)
			return BaseClass::ShouldHitEntity(pHandleEntity, contentsMask);

		return false;
	}

protected:
	const IHandleEntity *m_pPassNotOwner;
};


class VANCECTraceFilterPhyscannon : public CTraceFilterSimple
{
public:
	DECLARE_CLASS(VANCECTraceFilterPhyscannon, CTraceFilterSimple);

	VANCECTraceFilterPhyscannon(const IHandleEntity *passentity, int collisionGroup)
		: CTraceFilterSimple(NULL, collisionGroup), m_pTraceOwner(passentity) {	}

	// For this test, we only test against entities (then world brushes afterwards)
	virtual TraceType_t	GetTraceType() const { return TRACE_ENTITIES_ONLY; }

	bool HasContentsGrate(CBaseEntity *pEntity)
	{
		// FIXME: Move this into the GetModelContents() function in base entity

		// Find the contents based on the model type
		int nModelType = modelinfo->GetModelType(pEntity->GetModel());
		if (nModelType == mod_studio)
		{
			CBaseAnimating *pAnim = dynamic_cast<CBaseAnimating *>(pEntity);
			if (pAnim != NULL)
			{
				CStudioHdr *pStudioHdr = pAnim->GetModelPtr();
				if (pStudioHdr != NULL && (pStudioHdr->contents() & CONTENTS_GRATE))
					return true;
			}
		}
		else if (nModelType == mod_brush)
		{
			// Brushes poll their contents differently
			int contents = modelinfo->GetModelContents(pEntity->GetModelIndex());
			if (contents & CONTENTS_GRATE)
				return true;
		}

		return false;
	}

	virtual bool ShouldHitEntity(IHandleEntity *pHandleEntity, int contentsMask)
	{
		// Only skip ourselves (not things we own)
		if (pHandleEntity == m_pTraceOwner)
			return false;

		// Get the entity referenced by this handle
		CBaseEntity *pEntity = EntityFromEntityHandle(pHandleEntity);
		if (pEntity == NULL)
			return false;

		// Handle grate entities differently
		if (HasContentsGrate(pEntity))
		{
			// See if it's a grabbable physics prop
			CPhysicsProp *pPhysProp = dynamic_cast<CPhysicsProp *>(pEntity);
			if (pPhysProp != NULL)
				return pPhysProp->CanBePickedUpByPhyscannon();

			// See if it's a grabbable physics prop
			if (FClassnameIs(pEntity, "prop_physics"))
			{
				CPhysicsProp *pPhysProp = dynamic_cast<CPhysicsProp *>(pEntity);
				if (pPhysProp != NULL)
					return pPhysProp->CanBePickedUpByPhyscannon();

				// Somehow had a classname that didn't match the class!
				Assert(0);
			}
			else if (FClassnameIs(pEntity, "func_physbox"))
			{
				// Must be a moveable physbox
				CPhysBox *pPhysBox = dynamic_cast<CPhysBox *>(pEntity);
				if (pPhysBox)
					return pPhysBox->CanBePickedUpByPhyscannon();

				// Somehow had a classname that didn't match the class!
				Assert(0);
			}

			// Don't bother with any other sort of grated entity
			return false;
		}

		// Use the default rules
		return BaseClass::ShouldHitEntity(pHandleEntity, contentsMask);
	}

protected:
	const IHandleEntity *m_pTraceOwner;
};

// We want to test against brushes alone
class VANCECTraceFilterOnlyBrushes : public CTraceFilterSimple
{
public:
	DECLARE_CLASS(VANCECTraceFilterOnlyBrushes, CTraceFilterSimple);
	VANCECTraceFilterOnlyBrushes(int collisionGroup) : CTraceFilterSimple(NULL, collisionGroup) {}
	virtual TraceType_t	GetTraceType() const { return TRACE_WORLD_ONLY; }
};