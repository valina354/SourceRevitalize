//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//

#include "cbase.h"
#include "basehlcombatweapon.h"
#include "player.h"
#include "gamerules.h"
#include "ammodef.h"
#include "mathlib/mathlib.h"
#include "in_buttons.h"
#include "soundent.h"
#include "animation.h"
#include "ai_condition.h"
#include "vance_bludgeonweapon.h"
#include "ndebugoverlay.h"
#include "te_effect_dispatch.h"
#include "rumble_shared.h"
#include "gamestats.h"
#include "npcevent.h"
#include "ai_basenpc.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IMPLEMENT_SERVERCLASS_ST( CVanceBludgeonWeapon, DT_VanceBludgeonWeapon )
END_SEND_TABLE()

ConVar vance_melee_raycount("sk_plr_melee_raycount", "10");

ConVar vance_melee_debug("vance_melee_debug", "0");

//weapon script overrides
ConVar vance_melee_width("vance_melee_width_override", "0");
ConVar vance_melee_height_left("vance_melee_height_left_override", "0");
ConVar vance_melee_height_right("Vance_melee_height_right_override", "0");
ConVar vance_melee_swingspeed("vance_melee_swingspeed_override", "0");

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
CVanceBludgeonWeapon::CVanceBludgeonWeapon() : m_flSwing(0.0f)
{
	m_bFiresUnderwater = true;
}

//-----------------------------------------------------------------------------
// Purpose: Spawn the weapon
//-----------------------------------------------------------------------------
void CVanceBludgeonWeapon::Spawn( void )
{
	m_fMinRange1	= 0;
	m_fMinRange2	= 0;
	m_fMaxRange1	= 64;
	m_fMaxRange2	= 64;
	//Call base class first
	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: Precache the weapon
//-----------------------------------------------------------------------------
void CVanceBludgeonWeapon::Precache( void )
{
	//Call base class first
	BaseClass::Precache();

	PrecacheScriptSound("AlyxPlayer.Grunt");
}

void CVanceBludgeonWeapon::Operator_HandleAnimEvent(animevent_t *pEvent, CBaseCombatCharacter *pOperator) {
	if (pEvent->event == AE_SV_SWING)
	{
		trace_t traceHit;

		// Try a ray
		CBasePlayer *pOwner = ToBasePlayer(GetOwner());
		if (!pOwner)
			return;

		pOwner->RumbleEffect(RUMBLE_CROWBAR_SWING, 0, RUMBLE_FLAG_RESTART);

		Vector swingStart = pOwner->Weapon_ShootPosition();
		Vector forward;

		forward = pOwner->GetAutoaimVector(AUTOAIM_SCALE_DEFAULT, GetRange());

		Vector swingEnd = swingStart + forward * GetRange();
		UTIL_TraceLine(swingStart, swingEnd, MASK_SHOT_HULL, pOwner, COLLISION_GROUP_NONE, &traceHit);

		SetThink(&CVanceBludgeonWeapon::SwingThink);
		SetNextThink(gpGlobals->curtime);

		return;
	} else if (pEvent->event == AE_MELEE_THROW) {
		CBasePlayer* pOwner = ToBasePlayer(GetOwner());
		if (!pOwner)
			return;
		Vector dirForward;
		AngleVectors(pOwner->EyeAngles(), &dirForward);

		// make ourselves a prop_thrownbludgeonweapon
		CThrownBludgeonProp *pProp = dynamic_cast< CThrownBludgeonProp * >(CreateEntityByName("prop_thrownbludgeonweapon"));
		if (pProp)
		{
			char buf[512];
			// Pass in standard key values
			Q_snprintf(buf, sizeof(buf), "%.10f %.10f %.10f", pOwner->Weapon_ShootPosition().x, pOwner->Weapon_ShootPosition().y, pOwner->Weapon_ShootPosition().z);
			pProp->KeyValue("origin", buf);
			Q_snprintf(buf, sizeof(buf), "%.10f %.10f %.10f", pOwner->EyeAngles().x, pOwner->EyeAngles().y, pOwner->EyeAngles().z);
			pProp->KeyValue("angles", buf);
			pProp->KeyValue("model", GetWorldModel());
			pProp->KeyValue("fademindist", "-1");
			pProp->KeyValue("fademaxdist", "0");
			pProp->KeyValue("fadescale", "1");
			pProp->KeyValue("inertiaScale", "1.0");
			pProp->KeyValue("physdamagescale", "0.1");
			DispatchSpawn(pProp);
			pProp->Activate();

			pProp->m_sClassname = GetClassname();
			pProp->m_fDamage = ((VanceWeaponInfo_t &)GetWpnData()).fThrowDamage;

			pProp->ApplyAbsVelocityImpulse(((VanceWeaponInfo_t &)GetWpnData()).fThrowVelocity * dirForward);
			pProp->ApplyLocalAngularVelocityImpulse(((VanceWeaponInfo_t &)GetWpnData()).iThrowRotation);
		}
	} else {
		BaseClass::Operator_HandleAnimEvent(pEvent, pOperator);
	}
}

int CVanceBludgeonWeapon::CapabilitiesGet()
{ 
	return bits_CAP_WEAPON_MELEE_ATTACK1; 
}


int CVanceBludgeonWeapon::WeaponMeleeAttack1Condition( float flDot, float flDist )
{
	if (flDist > 64)
	{
		return COND_TOO_FAR_TO_ATTACK;
	}
	else if (flDot < 0.7)
	{
		return COND_NOT_FACING_ATTACK;
	}

	return COND_CAN_MELEE_ATTACK1;
}

//------------------------------------------------------------------------------
// Purpose : Update weapon
//------------------------------------------------------------------------------
void CVanceBludgeonWeapon::ItemPostFrame( void )
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );
	
	if ( pOwner == NULL )
		return;

	//we are midair or arent, switch anims if we need to
	if (!(pOwner->GetFlags() & FL_ONGROUND) && GetIdealActivity() == ACT_VM_SPRINT && m_fDoNotDisturb <= gpGlobals->curtime) {
		SetIdealActivity(ACT_VM_SPRINT2);
	}
	if (!(pOwner->GetFlags() & FL_ONGROUND) && GetIdealActivity() == ACT_VM_SPRINT_EXTENDED && m_fDoNotDisturb <= gpGlobals->curtime) {
		SetIdealActivity(ACT_VM_SPRINT2_EXTENDED);
	}
	if ((pOwner->GetFlags() & FL_ONGROUND) && GetIdealActivity() == ACT_VM_SPRINT2 && m_fDoNotDisturb <= gpGlobals->curtime) {
		SetIdealActivity(ACT_VM_SPRINT);
	}
	if ((pOwner->GetFlags() & FL_ONGROUND) && GetIdealActivity() == ACT_VM_SPRINT2_EXTENDED && m_fDoNotDisturb <= gpGlobals->curtime) {
		SetIdealActivity(ACT_VM_SPRINT_EXTENDED);
	}

	bool bThrowReadyMode = false;
	bool bThrowCanceled = false;

	if ( (pOwner->m_nButtons & IN_ATTACK) && (m_flNextPrimaryAttack <= gpGlobals->curtime) )
	{
		if (!m_bReadyToThrow && !m_bWaitingForDeathSetup && !m_bWaitingForDeath) {
			PrimaryAttack();
		}
		bThrowCanceled = true;
	} 
	else if ( (pOwner->m_nButtons & IN_ATTACK2) && (m_flNextSecondaryAttack <= gpGlobals->curtime) )
	{
		bThrowReadyMode = true;
	}
	else if (GetActivity() != ACT_VM_THROW_TO_IDLE) {
		WeaponIdle();
	}

	//if we are tryin to throw
	if (m_bWaitingForDeath) {
		m_bReadyToThrow = false;
	}
	if (m_bWaitingForDeathSetup && GetActivity() != ACT_VM_THROW) {
		//start switching to a different weapon, any weapon
		if (!pOwner->GetLastWeapon()) {
			if (!pOwner->SwitchToNextBestWeapon(this) && pOwner->HasNamedPlayerItem("weapon_unarmed")) {
				pOwner->Weapon_Switch(static_cast<CBaseCombatWeapon*>(pOwner->HasNamedPlayerItem("weapon_unarmed"))); //default to unarmed weapon
			}
		} else {
			pOwner->Weapon_Switch(pOwner->GetLastWeapon());
		}
		m_bWaitingForDeathSetup = false;
		m_bReadyToThrow = false;
		m_bWaitingForDeath = true;
	} else if (bThrowReadyMode) {
		if (!m_bReadyToThrow) {
			SendWeaponAnim(ACT_VM_IDLE_TO_THROW);
			m_bReadyToThrow = true;
		}
	} else if (m_bReadyToThrow) {
		if (bThrowCanceled) {
			SendWeaponAnim(ACT_VM_THROW_TO_IDLE);
			m_bReadyToThrow = false;
		} else if (GetActivity() != ACT_VM_IDLE_TO_THROW) {
			//throw the thing
			SendWeaponAnim(ACT_VM_THROW);
			if (m_flNextGrunt2Sound < gpGlobals->curtime)
			{
				EmitSound("AlyxPlayer.Grunt");
				m_flNextGrunt2Sound = gpGlobals->curtime + RandomFloat(5.0f, 13.0f);
			}
			m_bWaitingForDeathSetup = true;
			m_bReadyToThrow = false;
		}
		m_flNextSecondaryAttack = gpGlobals->curtime + SequenceDuration();
		m_flNextPrimaryAttack = gpGlobals->curtime + SequenceDuration();  //not that it matters since weve just thrown it anyway
	}
}

bool CVanceBludgeonWeapon::SendWeaponAnim(int iActivity) {
	if (iActivity == ACT_VM_HOLSTER && m_bWaitingForDeathSetup) {
		return false;
	}
	return BaseClass::SendWeaponAnim(iActivity);
}

void CVanceBludgeonWeapon::ItemHolsterFrame() {
	CBasePlayer *pOwner = ToBasePlayer(GetOwner());
	if (m_bWaitingForDeath && pOwner) {
		Vector vik = Vector(0, 0, 0);
		pOwner->Weapon_Drop(this, &vik, &vik);
		UTIL_Remove(this);
		return;
	}
	BaseClass::ItemHolsterFrame();
}

//------------------------------------------------------------------------------
// Purpose :
// Input   :
// Output  :
//------------------------------------------------------------------------------
void CVanceBludgeonWeapon::PrimaryAttack()
{
	m_bSwingSwitch = !m_bSwingSwitch;
	Swing();

	// Don't make grunt sounds all the time
	if (m_flNextGruntSound < gpGlobals->curtime)
	{
		EmitSound("AlyxPlayer.Grunt"); // Grunt sound (omg guys le funny sex noise! only in ohio)

		m_flNextGruntSound = gpGlobals->curtime + RandomFloat(8.0f, 15.0f);
	}
}

//------------------------------------------------------------------------------
// Purpose: Implement impact function
//------------------------------------------------------------------------------
void CVanceBludgeonWeapon::Hit( trace_t &traceHit, Activity nHitActivity)
{
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );
	
	//Do view kick
	AddViewKick();

	//Make sound for the AI
	CSoundEnt::InsertSound( SOUND_BULLET_IMPACT, traceHit.endpos, 400, 0.2f, pPlayer );

	// This isn't great, but it's something for when the crowbar hits.
	pPlayer->RumbleEffect( RUMBLE_AR2, 0, RUMBLE_FLAG_RESTART );

	CBaseEntity	*pHitEntity = traceHit.m_pEnt;

	//Apply damage to a hit target
	if ( pHitEntity != NULL )
	{
		Vector hitDirection;
		pPlayer->EyeVectors( NULL, &hitDirection, NULL );
		VectorNormalize( hitDirection );

		if (m_bSwingSwitch)
			hitDirection = -hitDirection;

		CTakeDamageInfo info( GetOwner(), GetOwner(), GetDamageForActivity( nHitActivity ), DMG_CLUB );

		if( pPlayer && pHitEntity->IsNPC() )
		{
			// If bonking an NPC, adjust damage.
			info.AdjustPlayerDamageInflictedForSkillLevel();
		}

		CalculateMeleeDamageForce( &info, hitDirection, traceHit.endpos );

		pHitEntity->DispatchTraceAttack( info, hitDirection, &traceHit ); 
		ApplyMultiDamage();

		// Now hit all triggers along the ray that... 
		TraceAttackToTriggers( info, traceHit.startpos, traceHit.endpos, hitDirection );

		if ( ToBaseCombatCharacter( pHitEntity ) )
		{
			gamestats->Event_WeaponHit( pPlayer, true, GetClassname(), info );
		}
	}

	// Apply an impact effect
	ImpactEffect( traceHit );
}

void CVanceBludgeonWeapon::ChooseIntersectionPointAndActivity( trace_t &hitTrace, const Vector &mins, const Vector &maxs, CBasePlayer *pOwner )
{
	int			i, j, k;
	float		distance;
	const float	*minmaxs[2] = {mins.Base(), maxs.Base()};
	trace_t		tmpTrace;
	Vector		vecHullEnd = hitTrace.endpos;
	Vector		vecEnd;

	distance = 1e6f;
	Vector vecSrc = hitTrace.startpos;

	vecHullEnd = vecSrc + ((vecHullEnd - vecSrc)*2);
	UTIL_TraceLine( vecSrc, vecHullEnd, MASK_SHOT_HULL, pOwner, COLLISION_GROUP_NONE, &tmpTrace );
	if ( tmpTrace.fraction == 1.0 )
	{
		for ( i = 0; i < 2; i++ )
		{
			for ( j = 0; j < 2; j++ )
			{
				for ( k = 0; k < 2; k++ )
				{
					vecEnd.x = vecHullEnd.x + minmaxs[i][0];
					vecEnd.y = vecHullEnd.y + minmaxs[j][1];
					vecEnd.z = vecHullEnd.z + minmaxs[k][2];

					UTIL_TraceLine( vecSrc, vecEnd, MASK_SHOT_HULL, pOwner, COLLISION_GROUP_NONE, &tmpTrace );
					if ( tmpTrace.fraction < 1.0 )
					{
						float thisDistance = (tmpTrace.endpos - vecSrc).Length();
						if ( thisDistance < distance )
						{
							hitTrace = tmpTrace;
							distance = thisDistance;
						}
					}
				}
			}
		}
	}
	else
	{
		hitTrace = tmpTrace;
	}


	return;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &traceHit - 
//-----------------------------------------------------------------------------
bool CVanceBludgeonWeapon::ImpactWater(const Vector& start, const Vector& end)
{
	//FIXME: This doesn't handle the case of trying to splash while being underwater, but that's not going to look good
	//		 right now anyway...

	// We must start outside the water
	if (UTIL_PointContents(start) & (CONTENTS_WATER | CONTENTS_SLIME))
		return false;

	// We must end inside of water
	if (!(UTIL_PointContents(end) & (CONTENTS_WATER | CONTENTS_SLIME)))
		return false;

	trace_t	waterTrace;

	UTIL_TraceLine(start, end, (CONTENTS_WATER | CONTENTS_SLIME), GetOwner(), COLLISION_GROUP_NONE, &waterTrace);

	if (waterTrace.fraction < 1.0f)
	{
		CEffectData	data;

		data.m_fFlags = 0;
		data.m_vOrigin = waterTrace.endpos;
		data.m_vNormal = waterTrace.plane.normal;
		data.m_flScale = 8.0f;

		// See if we hit slime
		if (waterTrace.contents & CONTENTS_SLIME)
		{
			data.m_fFlags |= FX_WATER_IN_SLIME;
		}

		DispatchEffect("watersplash", data);
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVanceBludgeonWeapon::ImpactEffect(trace_t& traceHit)
{
	// See if we hit water (we don't do the other impact effects in this case)
	if (ImpactWater(traceHit.startpos, traceHit.endpos))
		return;

	//FIXME: need new decals
	UTIL_ImpactTrace(&traceHit, DMG_CLUB);
}

void CVanceBludgeonWeapon::SwingThink()
{
	CBasePlayer* pOwner = ToBasePlayer(GetOwner());
	if (!pOwner)
		return;

	Vector swingStart = pOwner->Weapon_ShootPosition();
	Vector forward;

	QAngle AngForward = pOwner->GetAbsAngles();
	float sk_plr_melee_angle = ((VanceWeaponInfo_t &)GetWpnData()).fSwingWidth;
	if (vance_melee_width.GetFloat() != 0.0f) { sk_plr_melee_angle = vance_melee_width.GetFloat(); }
	if (m_bSwingSwitch)
	{
		AngForward[YAW] -= sk_plr_melee_angle;
		AngForward[YAW] += sk_plr_melee_angle * m_flSwing * 2.0f;

		float sk_plr_melee_height = ((VanceWeaponInfo_t &)GetWpnData()).fSwingHeightLeft;
		if (vance_melee_height_left.GetFloat() != 0.0f) { sk_plr_melee_height = vance_melee_height_left.GetFloat(); }
		AngForward[PITCH] -= sk_plr_melee_height;
		AngForward[PITCH] += sk_plr_melee_height * m_flSwing * 2.0f;
	}
	else
	{
		AngForward[YAW] += sk_plr_melee_angle;
		AngForward[YAW] -= sk_plr_melee_angle * m_flSwing * 2.0f;

		float sk_plr_melee_height = ((VanceWeaponInfo_t &)GetWpnData()).fSwingHeightRight;
		if (vance_melee_height_right.GetFloat() != 0.0f) { sk_plr_melee_height = vance_melee_height_right.GetFloat(); }
		AngForward[PITCH] -= sk_plr_melee_height;
		AngForward[PITCH] += sk_plr_melee_height * m_flSwing * 2.0f;
	}
	AngleVectors(AngForward, &forward);
	Vector swingEnd = swingStart + forward * GetRange();

	float swingspeed = ((VanceWeaponInfo_t &)GetWpnData()).fSwingSpeed;
	if (vance_melee_swingspeed.GetFloat() != 0.0f) { swingspeed = vance_melee_swingspeed.GetFloat(); }
	SetNextThink(gpGlobals->curtime + swingspeed / vance_melee_raycount.GetInt());

	m_flSwing += 1.0f / vance_melee_raycount.GetInt();

	trace_t traceHit;
	UTIL_TraceLine(swingStart, swingEnd, MASK_SHOT_HULL, pOwner, COLLISION_GROUP_NONE, &traceHit);
	Activity nHitActivity = GetPrimaryAttackActivity();

	CTakeDamageInfo triggerInfo(GetOwner(), GetOwner(), GetDamageForActivity(nHitActivity), DMG_CLUB);
	triggerInfo.SetDamagePosition(traceHit.startpos);
	triggerInfo.SetDamageForce(forward);
	TraceAttackToTriggers(triggerInfo, traceHit.startpos, traceHit.endpos, forward);
	
	// -------------------------
	//	Miss
	// -------------------------
	if (traceHit.fraction == 1.0f)
	{
		// We want to test the first swing again
		Vector testEnd = swingStart + forward * GetRange();

		// See if we happened to hit water
		ImpactWater(swingStart, testEnd);

		if(vance_melee_debug.GetBool())
			DebugDrawLine(swingStart, swingEnd, 255, 255, 0, false, 5.0f);
	}
	else
	{
		if(m_hHitEnt != traceHit.m_pEnt)
			Hit(traceHit, nHitActivity);
		m_hHitEnt = traceHit.m_pEnt;

		if(vance_melee_debug.GetBool())
			DebugDrawLine(swingStart, swingEnd, 255, 0, 0, false, 5.0f);
	}

	if (m_flSwing >= 1.0f)
	{
		m_hHitEnt = NULL;
		m_flSwing = 0.0f;
		SetThink(NULL);
	}
}

//------------------------------------------------------------------------------
// Purpose : Starts the swing of the weapon and determines the animation
// Input   : bIsSecondary - is this a secondary attack?
//------------------------------------------------------------------------------
void CVanceBludgeonWeapon::Swing()
{
	CBasePlayer *pOwner = ToBasePlayer(GetOwner());
	if (!pOwner)
		return;

	pOwner->RumbleEffect( RUMBLE_CROWBAR_SWING, 0, RUMBLE_FLAG_RESTART );

	m_iPrimaryAttacks++;

	gamestats->Event_WeaponFired(pOwner, true, GetClassname());

	// Send the anim
	SendWeaponAnim(GetPrimaryAttackActivity());

	//Setup our next attack times
	m_flNextPrimaryAttack = gpGlobals->curtime + GetFireRate();
	m_flNextSecondaryAttack = gpGlobals->curtime + SequenceDuration();

	//Play swing sound
	WeaponSound( SINGLE );
}

void CVanceBludgeonWeapon::DefaultTouch(CBaseEntity *pOther) {
	CBasePlayer *pPlayer = ToBasePlayer(pOther);
	if (!pPlayer)
		return;
	//if the player already has another vance_bludgeonweapon then dont let them pick it up automatically
	for (int i = 0; i < MAX_WEAPONS; i++) {
		CVanceBludgeonWeapon *wPlayerWeapon = static_cast<CVanceBludgeonWeapon *>(pPlayer->GetWeapon(i));
		if (wPlayerWeapon && wPlayerWeapon->GetWpnData().iSlot == 0) {
			return;
		}
	}
	BaseClass::DefaultTouch(pOther);
}
void CVanceBludgeonWeapon::OnPickedUp(CBaseCombatCharacter *pNewOwner) {
	BaseClass::OnPickedUp(pNewOwner);
}
bool CVanceBludgeonWeapon::DefaultDeploy(char *szViewModel, char *szWeaponModel, int iActivity, char *szAnimExt) {
	//vomit out any melee weapons the player happens to have
	CBasePlayer *pPlayer = ToBasePlayer(GetOwner());
	if (pPlayer) {
		Vector dirForward;
		AngleVectors(pPlayer->EyeAngles(), &dirForward);
		dirForward *= Vector(1, 1, 0);
		dirForward = dirForward.Normalized();
		for (int i = 0; i < MAX_WEAPONS; i++) {
			CVanceBludgeonWeapon *wPlayerWeapon = static_cast<CVanceBludgeonWeapon *>(pPlayer->GetWeapon(i));
			if (wPlayerWeapon && wPlayerWeapon != this && wPlayerWeapon->GetWpnData().iSlot == 0) {
				//Weapon_Drop sucks and is stupid and sometimes spawns weapons without gravity, so we spawn a new entity instead and kill ourselves after our child is born
				CVanceBludgeonWeapon *newent = static_cast<CVanceBludgeonWeapon *>(CBaseEntity::Create(wPlayerWeapon->GetClassname(), pPlayer->Weapon_ShootPosition() - Vector(0, 0, 12), pPlayer->GetAbsAngles(), nullptr));
				newent->ApplyAbsVelocityImpulse(dirForward * 300);
				pPlayer->Weapon_Detach(wPlayerWeapon);
				UTIL_Remove(wPlayerWeapon);
			}
		}
	}
	return BaseClass::DefaultDeploy(szViewModel, szWeaponModel, iActivity, szAnimExt);
}

//=============================================================================================================
// THROWN PHYSICS PROP VERSION OF THE WEAPON THAT INTERACTS WITH NPCS
//=============================================================================================================
LINK_ENTITY_TO_CLASS(prop_thrownbludgeonweapon, CThrownBludgeonProp)
void CThrownBludgeonProp::VPhysicsCollision(int index, gamevcollisionevent_t *pEvent) {
	CBaseEntity *pHitEntity = pEvent->pEntities[!index];
	if (!pHitEntity)
		pHitEntity = GetContainingEntity(INDEXENT(0));
	if (!ToBasePlayer(pHitEntity) && !m_bReadyToSwap) { //damaging the player like this crashes the game, so dont do that
		pHitEntity->TakeDamage(CTakeDamageInfo(GetBaseEntity(), UTIL_GetLocalPlayer()->GetBaseEntity(), pEvent->preVelocity[0].Normalized() * m_fDamage * 30, GetAbsOrigin(), m_fDamage, DMG_CLUB));
	}

	BaseClass::VPhysicsCollision(index, pEvent);

	//weve had our fun turn us back into a weapon
	if (!m_bReadyToSwap)
		SetNextThink(gpGlobals->curtime + 0.1f);
	m_bReadyToSwap = true;
}

void CThrownBludgeonProp::Think() {
	BaseClass::Think();

	if (m_bReadyToSwap && m_bPositionLast) {
		CVanceBludgeonWeapon *newent = static_cast<CVanceBludgeonWeapon *>(CBaseEntity::Create(m_sClassname, GetAbsOrigin(), GetAbsAngles(), nullptr));
		newent->ApplyAbsVelocityImpulse((GetAbsOrigin() - m_vPositionLast) / gpGlobals->frametime);
		UTIL_Remove(this);
	} else if (m_bReadyToSwap) {
		m_vPositionLast = GetAbsOrigin();
		m_bPositionLast = true;
		SetNextThink(gpGlobals->curtime);
	}
}