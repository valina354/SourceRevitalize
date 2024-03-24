//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose:
//
// $NoKeywords: $
//===========================================================================//

#include "BaseVSShader.h"

#include "SDK_screenspaceeffect_vs30.inc"
#include "heathaze_ps30.inc"
#include <chrono>

ConVar r_post_heathaze_distortionamm( "r_post_heathaze_distortionamm", "0", FCVAR_CHEAT );
ConVar r_post_heathaze_windspeed( "r_post_heathaze_windspeed", "0", FCVAR_CHEAT );
ConVar r_post_heathaze_blurstrength( "r_post_heathaze_blurstrength", "0", FCVAR_CHEAT );

BEGIN_VS_SHADER_FLAGS( HeatHaze, "Help for Bloom", SHADER_NOT_EDITABLE )
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
			DECLARE_STATIC_PIXEL_SHADER( heathaze_ps30 );
			SET_STATIC_PIXEL_SHADER( heathaze_ps30 );
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
		float fDistamm[4];
		fDistamm[0] = r_post_heathaze_distortionamm.GetFloat();
		pShaderAPI->SetPixelShaderConstant( 1, fDistamm, 1 );
		float fWindspe[4];
		fWindspe[0] = r_post_heathaze_windspeed.GetFloat();
		pShaderAPI->SetPixelShaderConstant( 2, fWindspe, 1 );
		float fBlurStrength[4];
		fBlurStrength[0] = r_post_heathaze_blurstrength.GetFloat();
		pShaderAPI->SetPixelShaderConstant( 3, fBlurStrength, 1 );

		//if( g_pHardwareConfig->SupportsPixelShaders_2_b() )
		{
			DECLARE_DYNAMIC_PIXEL_SHADER( heathaze_ps30 );
			SET_DYNAMIC_PIXEL_SHADER( heathaze_ps30 );
		}
	}
	Draw();
}
END_SHADER
