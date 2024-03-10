//===== Copyright � 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose:
//
// $NoKeywords: $
//===========================================================================//

#include "BaseVSShader.h"

#include "screenspace_simple_vs30.inc"
#include "ssgi0_vs30.inc"
#include "ssgi0_ps30.inc"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

BEGIN_VS_SHADER( SSGI0, "Help for SSGI0" )
BEGIN_SHADER_PARAMS
SHADER_PARAM( FBTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "_rt_FullFrameFB", "" )
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
		int fmt = VERTEX_POSITION;
		pShaderShadow->VertexShaderVertexFormat( fmt, 1, 0, 0 );

		// Pre-cache shaders
		DECLARE_STATIC_VERTEX_SHADER( ssgi0_vs30 );
		SET_STATIC_VERTEX_SHADER( ssgi0_vs30 );

		if ( g_pHardwareConfig->SupportsPixelShaders_2_b() )
		{
			DECLARE_STATIC_PIXEL_SHADER( ssgi0_ps30 );
			SET_STATIC_PIXEL_SHADER( ssgi0_ps30 );
		}
		else
		{
			DECLARE_STATIC_PIXEL_SHADER( ssgi0_ps30 );
			SET_STATIC_PIXEL_SHADER( ssgi0_ps30 );
		}
	}

	DYNAMIC_STATE
	{
		BindTexture( SHADER_SAMPLER0, FBTEXTURE, -1 );
		DECLARE_DYNAMIC_VERTEX_SHADER( ssgi0_vs30 );
		SET_DYNAMIC_VERTEX_SHADER( ssgi0_vs30 );

		if ( g_pHardwareConfig->SupportsPixelShaders_2_b() )
		{
			DECLARE_DYNAMIC_PIXEL_SHADER( ssgi0_ps30 );
			SET_DYNAMIC_PIXEL_SHADER( ssgi0_ps30 );
		}
		else
		{
			DECLARE_DYNAMIC_PIXEL_SHADER( ssgi0_ps30 );
			SET_DYNAMIC_PIXEL_SHADER( ssgi0_ps30 );
		}
	}
	Draw();
}
END_SHADER