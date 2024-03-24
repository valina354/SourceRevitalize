//===== Copyright � 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose:
//
// $NoKeywords: $
//===========================================================================//

#include "BaseVSShader.h"

#include "SDK_screenspaceeffect_vs30.inc"
#include "ascii_ps30.inc"

BEGIN_VS_SHADER_FLAGS( ASCII, "Help for Bloom", SHADER_NOT_EDITABLE )
BEGIN_SHADER_PARAMS
SHADER_PARAM( FBTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "_rt_VanceHDR", "" )
SHADER_PARAM( ASCIITEXTURE, SHADER_PARAM_TYPE_TEXTURE, "", "" )
END_SHADER_PARAMS

SHADER_INIT
{
	if ( params[FBTEXTURE]->IsDefined() )
	{
		LoadTexture( FBTEXTURE );
	}
	if ( params[ASCIITEXTURE]->IsDefined() )
	{
		LoadTexture( ASCIITEXTURE );
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
			DECLARE_STATIC_PIXEL_SHADER( ascii_ps30 );
			SET_STATIC_PIXEL_SHADER( ascii_ps30 );
		}
	}

	DYNAMIC_STATE
	{
		BindTexture( SHADER_SAMPLER0, FBTEXTURE, -1 );
		BindTexture( SHADER_SAMPLER1, ASCIITEXTURE, -1 );

		//if( g_pHardwareConfig->SupportsPixelShaders_2_b() )
		{
			DECLARE_DYNAMIC_PIXEL_SHADER( ascii_ps30 );
			SET_DYNAMIC_PIXEL_SHADER( ascii_ps30 );
		}
	}
	Draw();
}
END_SHADER
