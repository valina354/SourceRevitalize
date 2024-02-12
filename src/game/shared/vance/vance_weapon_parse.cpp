//============================ AdV Software, 2019 ============================//
//
//	Vance Weapon script parsing
//
//============================================================================//

#include "cbase.h"
#include "vance_weapon_parse.h"

FileWeaponInfo_t* CreateWeaponInfo()
{
	return new VanceWeaponInfo_t;
}

VanceWeaponInfo_t::VanceWeaponInfo_t() : FileWeaponInfo_t()
{
	flCameraMovementScale = 1.0f;
	*szCameraAttachment = 0;
	bHaveCamera = false;
	bHasFirstDrawAnim = false;
	szMuzzleFlashTexture[0] = 0; // default muzzle flash
	iWeaponLength = 0;
	roundsPerMinute = 0.0f;
}

void VanceWeaponInfo_t::Parse(KeyValues *pKeyValuesData, const char *szWeaponName)
{
	FileWeaponInfo_t::Parse(pKeyValuesData, szWeaponName);

	roundsPerMinute = 1 / (pKeyValuesData->GetFloat("rpm", 0.0f) / 60);

	bHasFirstDrawAnim = pKeyValuesData->GetBool("HasFirstDrawAnim");
	V_strncpy(szMuzzleFlashTexture, pKeyValuesData->GetString("MuzzleflashTexture", "effects/muzzleflash_light"), MAX_WEAPON_STRING);

	if (KeyValues *kvCameraAttachment = pKeyValuesData->FindKey("CameraAttachment"))
	{
		const char *soundname = kvCameraAttachment->GetString("Attachment");
		Q_strncpy(szCameraAttachment, soundname, MAX_WEAPON_STRING);
		bHaveCamera = true;
		flCameraMovementScale = kvCameraAttachment->GetFloat("Amount");
	}

	// im still not sure if "Collision" is the right name for it
	if (KeyValues* kvCollision = pKeyValuesData->FindKey("Collision"))
	{
		UTIL_StringToVector(vCollisionOffset.Base(), kvCollision->GetString("Offset", "0 0 0"));
		UTIL_StringToVector(angCollisionRotation.Base(), kvCollision->GetString("Rotation", "0 0 0"));
		iWeaponLength = kvCollision->GetInt("Length", 0);
	}

	UTIL_StringToVector(vIronsightOffset.Base(), pKeyValuesData->GetString("IronsightOffset", "0 0 0"));
	UTIL_StringToVector(recoilMin.Base(), pKeyValuesData->GetString("RecoilMin", pKeyValuesData->GetString("Recoil", "0 0 0")));
	UTIL_StringToVector(recoilMax.Base(), pKeyValuesData->GetString("RecoilMax", "0 0 0"));

	//melee shit
	fSwingSpeed = pKeyValuesData->GetFloat("melee_swingspeed", 0.25f);
	fSwingWidth = pKeyValuesData->GetFloat("melee_swingwidth", 30.0f);
	fSwingHeightLeft = pKeyValuesData->GetFloat("melee_swingheight_left", 30.0f);
	fSwingHeightRight = pKeyValuesData->GetFloat("melee_swingheight_right", 30.0f);

	fThrowVelocity = pKeyValuesData->GetFloat("melee_throw_velocity", 5000.0f);
	UTIL_StringToVector(iThrowRotation.Base(), pKeyValuesData->GetString("melee_throw_angvelocity", "0 1000 0"));
	fThrowDamage = pKeyValuesData->GetFloat("melee_throw_damage", 15.0f);

	//find the mag_model from the weapon_script
	if ( V_strlen( pKeyValuesData->GetString( "mag_model" ) ) != 0 )
	{
		const char *MagModelStr = pKeyValuesData->GetString( "mag_model" );
		V_strncpy( szWeaponMag, MagModelStr, MAX_PATH );
	}
	else
	{
		V_memset( szWeaponMag, '\0', MAX_PATH );
	}
}