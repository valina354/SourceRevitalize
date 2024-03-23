//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose:
//
// $NoKeywords: $
//===========================================================================//

#include "BaseVSShader.h"

#include "SDK_screenspaceeffect_vs30.inc"
#include "dithering_ps30.inc"
#include <chrono>

ConVar r_post_dithering_drunkammount( "r_post_dithering_drunkammount", "0", FCVAR_CHEAT );
ConVar r_post_dithering_errorammount( "r_post_dithering_errorammount", "0.2", FCVAR_CHEAT );
ConVar r_post_dithering_grainammount( "r_post_dithering_grainammount", "0.2", FCVAR_CHEAT );

BEGIN_VS_SHADER_FLAGS( Dithering, "Help for Bloom", SHADER_NOT_EDITABLE )
BEGIN_SHADER_PARAMS
SHADER_PARAM( FBTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "_rt_VanceHDR", "" )
END_SHADER_PARAMS

SHADER_INIT
{
	if ( params[FBTEXTURE]->IsDefined() )
	{
		LoadTexture( FBTEXTURE );
	}
}

SHADER_FALLBACK
{
	// Requires DX9 + above
	if ( g_pHardwareConfig->GetDXSupportLevel() < 90 )
	{
		Assert( 0 );
		return "Wireframe";
	}
	return 0;
}

SHADER_DRAW
{
	SHADOW_STATE
	{
		pShaderShadow->EnableDepthWrites( false );

		pShaderShadow->EnableTexture( SHADER_SAMPLER0, true );
		pShaderShadow->EnableTexture( SHADER_SAMPLER1, true );
		int fmt = VERTEX_POSITION;
		pShaderShadow->VertexShaderVertexFormat( fmt, 1, 0, 0 );

		// Pre-cache shaders
		DECLARE_STATIC_VERTEX_SHADER( sdk_screenspaceeffect_vs30 );
		SET_STATIC_VERTEX_SHADER( sdk_screenspaceeffect_vs30 );

		//if( g_pHardwareConfig->SupportsPixelShaders_2_b() )
		{
			DECLARE_STATIC_PIXEL_SHADER( dithering_ps30 );
			SET_STATIC_PIXEL_SHADER( dithering_ps30 );
		}
	}

	DYNAMIC_STATE
	{
		BindTexture( SHADER_SAMPLER0, FBTEXTURE, -1 );
		// Get the current time point
		auto currentTimePoint = std::chrono::high_resolution_clock::now();

		// Convert the time point to a duration since the epoch
		auto duration = currentTimePoint.time_since_epoch();

		// Convert the duration to seconds (or any other unit you prefer)
		float currentTime = std::chrono::duration<float>( duration ).count();

		// Set the current time as a shader constant
		pShaderAPI->SetPixelShaderConstant( 0, &currentTime, 1 );
		float fDrunkam[4];
		fDrunkam[0] = r_post_dithering_drunkammount.GetFloat();
		pShaderAPI->SetPixelShaderConstant( 1, fDrunkam, 1 );
		float fErroramm[4];
		fErroramm[0] = r_post_dithering_errorammount.GetFloat();
		pShaderAPI->SetPixelShaderConstant( 2, fErroramm, 1 );
		float fGrainamm[4];
		fGrainamm[0] = r_post_dithering_grainammount.GetFloat();
		pShaderAPI->SetPixelShaderConstant( 3, fGrainamm, 1 );

		//if( g_pHardwareConfig->SupportsPixelShaders_2_b() )
		{
			DECLARE_DYNAMIC_PIXEL_SHADER( dithering_ps30 );
			SET_DYNAMIC_PIXEL_SHADER( dithering_ps30 );
		}
	}
	Draw();
}
END_SHADER
