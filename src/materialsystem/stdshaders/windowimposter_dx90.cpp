//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "BaseVSShader.h"
#include "cpp_shader_constant_register_map.h"

#include "SDK_windowimposter_vs30.inc"
#include "SDK_windowimposter_ps30.inc"



// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

DEFINE_FALLBACK_SHADER( sdk_windowimposter, sdk_windowimposter_DX90 )

BEGIN_VS_SHADER( sdk_windowimposter_DX90,
			  "Help for WindowImposter_DX90" )

	BEGIN_SHADER_PARAMS
		SHADER_PARAM( ENVMAP, SHADER_PARAM_TYPE_TEXTURE, "shadertest/shadertest_env", "envmap" )
		

		// Parallax cubemaps
		SHADER_PARAM( ENVMAPPARALLAX, SHADER_PARAM_TYPE_BOOL, "0", "Enables parallax correction code for env_cubemaps" )
		SHADER_PARAM( ENVMAPPARALLAXOBB1, SHADER_PARAM_TYPE_VEC4, "[1 0 0 0]", "The first line of the parallax correction OBB matrix" )
		SHADER_PARAM( ENVMAPPARALLAXOBB2, SHADER_PARAM_TYPE_VEC4, "[0 1 0 0]", "The second line of the parallax correction OBB matrix" )
		SHADER_PARAM( ENVMAPPARALLAXOBB3, SHADER_PARAM_TYPE_VEC4, "[0 0 1 0]", "The third line of the parallax correction OBB matrix" )
		SHADER_PARAM( ENVMAPORIGIN, SHADER_PARAM_TYPE_VEC3, "[0 0 0]", "The world space position of the env_cubemap being corrected" )
	END_SHADER_PARAMS

	// Set up anything that is necessary to make decisions in SHADER_FALLBACK.
	SHADER_INIT_PARAMS()
	{
	}

	SHADER_FALLBACK
	{
		return NULL;
	}

	SHADER_INIT
	{
		LoadCubeMap( ENVMAP );
	}

	SHADER_DRAW
	{

		// Parallax cubemaps
		bool hasParallaxCorrection = params[ENVMAPPARALLAX]->GetIntValue() > 0;

		SHADOW_STATE
		{
			if( g_pHardwareConfig->GetHDRType() != HDR_TYPE_NONE )
				pShaderShadow->EnableSRGBWrite( true );

			pShaderShadow->EnableTexture( SHADER_SAMPLER0, true );

			DECLARE_STATIC_VERTEX_SHADER( sdk_windowimposter_vs30 );
				SET_STATIC_VERTEX_SHADER_COMBO( PARALLAXCORRECT, hasParallaxCorrection ); // Parallax cubemaps enabled for 2_0b and onwards
			SET_STATIC_VERTEX_SHADER( sdk_windowimposter_vs30 );

			if( g_pHardwareConfig->SupportsPixelShaders_2_b() )
			{
				DECLARE_STATIC_PIXEL_SHADER( sdk_windowimposter_ps30 );
					SET_STATIC_PIXEL_SHADER_COMBO( PARALLAXCORRECT, hasParallaxCorrection ); // Parallax cubemaps enabled for 2_0b and onwards
				SET_STATIC_PIXEL_SHADER( sdk_windowimposter_ps30 );
			}
			else
			{
				DECLARE_STATIC_PIXEL_SHADER( sdk_windowimposter_ps30 );
					SET_STATIC_PIXEL_SHADER_COMBO( PARALLAXCORRECT, 0 ); // No parallax cubemaps with ps_2_0 :(
				SET_STATIC_PIXEL_SHADER( sdk_windowimposter_ps30 );
			}

			unsigned int flags = VERTEX_POSITION;
			int nTexCoordCount = 2;

			if (hasParallaxCorrection)
			{
				flags |= VERTEX_NORMAL;
				nTexCoordCount++;
			}

			pShaderShadow->VertexShaderVertexFormat( flags, nTexCoordCount, 0, 0 );
			pShaderShadow->EnableBlending( true );
			pShaderShadow->BlendFunc( SHADER_BLEND_SRC_ALPHA, SHADER_BLEND_ONE_MINUS_SRC_ALPHA );
			pShaderShadow->EnableDepthWrites( false );
			FogToFogColor();
		}
		DYNAMIC_STATE
		{
			DECLARE_DYNAMIC_VERTEX_SHADER( sdk_windowimposter_vs30 );
				SET_DYNAMIC_VERTEX_SHADER_COMBO( DOWATERFOG, pShaderAPI->GetSceneFogMode() == MATERIAL_FOG_LINEAR_BELOW_FOG_Z );
			SET_DYNAMIC_VERTEX_SHADER( sdk_windowimposter_vs30 );

			if( g_pHardwareConfig->SupportsPixelShaders_2_b() )
			{
				DECLARE_DYNAMIC_PIXEL_SHADER( sdk_windowimposter_ps30 );
					SET_DYNAMIC_PIXEL_SHADER_COMBO( PIXELFOGTYPE, pShaderAPI->GetPixelFogCombo() );
				SET_DYNAMIC_PIXEL_SHADER( sdk_windowimposter_ps30 );
			}
			else
			{
				DECLARE_DYNAMIC_PIXEL_SHADER( sdk_windowimposter_ps30 );
					SET_DYNAMIC_PIXEL_SHADER_COMBO( PIXELFOGTYPE, pShaderAPI->GetPixelFogCombo() );
				SET_DYNAMIC_PIXEL_SHADER( sdk_windowimposter_ps30 );
			}

			SetModulationVertexShaderDynamicState();

			pShaderAPI->SetPixelShaderFogParams( PSREG_FOG_PARAMS );

			float vEyePos_SpecExponent[4];
			pShaderAPI->GetWorldSpaceCameraPosition( vEyePos_SpecExponent );
			vEyePos_SpecExponent[3] = 0.0f;
			pShaderAPI->SetPixelShaderConstant( PSREG_EYEPOS_SPEC_EXPONENT, vEyePos_SpecExponent, 1 );
			

			// Parallax cubemaps
			if (hasParallaxCorrection)
			{
				pShaderAPI->SetPixelShaderConstant( 0, params[ENVMAPORIGIN]->GetVecValue() );

				float* vecs[3];
				vecs[0] = const_cast<float*>(params[ENVMAPPARALLAXOBB1]->GetVecValue());
				vecs[1] = const_cast<float*>(params[ENVMAPPARALLAXOBB2]->GetVecValue());
				vecs[2] = const_cast<float*>(params[ENVMAPPARALLAXOBB3]->GetVecValue());
				float matrix[4][4];
				for (int i = 0; i < 3; i++)
				{
					for (int j = 0; j < 4; j++)
					{
						matrix[i][j] = vecs[i][j];
					}
				}
				matrix[3][0] = matrix[3][1] = matrix[3][2] = 0;
				matrix[3][3] = 1;
				pShaderAPI->SetPixelShaderConstant( 1, &matrix[0][0], 4 );
			}
			BindTexture( SHADER_SAMPLER0, ENVMAP, -1 );
		}
		Draw();
	}

END_SHADER