//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose:
//
// $NoKeywords: $
//===========================================================================//

#include "BaseVSShader.h"

#include "SDK_screenspaceeffect_vs30.inc"
#include "blindness_ps30.inc"

ConVar r_post_radialblur_strength( "r_post_radialblur_strength", "0", FCVAR_ARCHIVE );
ConVar r_post_radialblur_amount( "r_post_radialblur_amount", "0", FCVAR_ARCHIVE );

BEGIN_VS_SHADER_FLAGS( Radialblur, "Help for Bloom", SHADER_NOT_EDITABLE )
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
			DECLARE_STATIC_PIXEL_SHADER( blindness_ps30 );
			SET_STATIC_PIXEL_SHADER( blindness_ps30 );
		}
	}

	DYNAMIC_STATE
	{
		BindTexture( SHADER_SAMPLER0, FBTEXTURE, -1 );

		float fStrength[4];
		fStrength[0] = r_post_radialblur_strength.GetFloat();
		pShaderAPI->SetPixelShaderConstant( 0, fStrength, 1 );
		float fAmount[4];
		fAmount[0] = r_post_radialblur_amount.GetFloat();
		pShaderAPI->SetPixelShaderConstant( 1, fAmount, 1 );

		//if( g_pHardwareConfig->SupportsPixelShaders_2_b() )
		{
			DECLARE_DYNAMIC_PIXEL_SHADER( blindness_ps30 );
			SET_DYNAMIC_PIXEL_SHADER( blindness_ps30 );
		}
	}
	Draw();
}
END_SHADER
