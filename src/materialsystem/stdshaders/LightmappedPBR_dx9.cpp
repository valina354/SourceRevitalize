//===================== Copyright (c) AdV Software And Source Revitalize For Improving It. All Rights Reserved. ======================
//
// The .cpp File for the brush PBR shader
//
//==================================================================================================

#include "BaseVSShader.h"
#include "convar.h"
#include "LightmappedPBR_dx9_helper.h"
#include "lightpass_helper.h"

#ifdef STDSHADER
BEGIN_VS_SHADER( LightmappedPBR, "Help for LightmappedPBR" )
#else
BEGIN_VS_SHADER( LightmappedPBR, "Help for LightmappedPBR" )
#endif

BEGIN_SHADER_PARAMS
SHADER_PARAM( ALPHATESTREFERENCE, SHADER_PARAM_TYPE_FLOAT, "0.0", "" )
SHADER_PARAM( ENVMAP, SHADER_PARAM_TYPE_TEXTURE, "shadertest/shadertest_env", "envmap" )
SHADER_PARAM( NORMALMAP, SHADER_PARAM_TYPE_TEXTURE, "models/shadertest/shader1_normal", "bump map" )
SHADER_PARAM( SEAMLESS_SCALE, SHADER_PARAM_TYPE_FLOAT, "0", "Scale factor for 'seamless' texture mapping. 0 means to use ordinary mapping" )

SHADER_PARAM( BRDF, SHADER_PARAM_TYPE_TEXTURE, "models/PBRTest/BRDF", "" )
SHADER_PARAM( NOISE, SHADER_PARAM_TYPE_TEXTURE, "shaders/bluenoise", "" )
SHADER_PARAM( ROUGHNESS, SHADER_PARAM_TYPE_TEXTURE, "", "" )
SHADER_PARAM( METALLIC, SHADER_PARAM_TYPE_TEXTURE, "", "" )
SHADER_PARAM( AO, SHADER_PARAM_TYPE_TEXTURE, "", "" )
SHADER_PARAM( EMISSIVE, SHADER_PARAM_TYPE_TEXTURE, "", "" )
SHADER_PARAM( DETAIL, SHADER_PARAM_TYPE_TEXTURE, "", "" )
SHADER_PARAM( DETAILSCALE, SHADER_PARAM_TYPE_TEXTURE, "", "" )
SHADER_PARAM( USESMOOTHNESS, SHADER_PARAM_TYPE_BOOL, "0", "Invert roughness" )


SHADER_PARAM( PARALLAXDEPTH, SHADER_PARAM_TYPE_FLOAT, "0", "Depth of the Parallax Map" );
SHADER_PARAM( PARALLAXSCALING, SHADER_PARAM_TYPE_FLOAT, "0", "Scaling of the Parallax Map" );
SHADER_PARAM( PARALLAXSTEP, SHADER_PARAM_TYPE_FLOAT, "0", "Step Scalling of the Parallax Map" );

// Parallax cubemaps
SHADER_PARAM( ENVMAPPARALLAX, SHADER_PARAM_TYPE_BOOL, "0", "Enables parallax correction code for env_cubemaps" )
SHADER_PARAM( ENVMAPPARALLAXOBB1, SHADER_PARAM_TYPE_VEC4, "[1 0 0 0]", "The first line of the parallax correction OBB matrix" )
SHADER_PARAM( ENVMAPPARALLAXOBB2, SHADER_PARAM_TYPE_VEC4, "[0 1 0 0]", "The second line of the parallax correction OBB matrix" )
SHADER_PARAM( ENVMAPPARALLAXOBB3, SHADER_PARAM_TYPE_VEC4, "[0 0 1 0]", "The third line of the parallax correction OBB matrix" )
SHADER_PARAM( ENVMAPORIGIN, SHADER_PARAM_TYPE_VEC3, "[0 0 0]", "The world space position of the env_cubemap being corrected" )
END_SHADER_PARAMS

void SetupVars( LightmappedPBR_DX9_Vars_t &info )
{
	info.m_nBaseTexture = BASETEXTURE;
	info.m_nBaseTextureFrame = FRAME;
	info.m_nBaseTextureTransform = BASETEXTURETRANSFORM;
	info.m_nAlphaTestReference = ALPHATESTREFERENCE;
	info.m_nRoughness = ROUGHNESS;
	info.m_nMetallic = METALLIC;
	info.m_nAO = AO;
	info.m_nEmissive = EMISSIVE;
	info.m_nDetail = DETAIL;
	info.m_nDetailStrength = DETAILSCALE;
	info.m_nEnvmap = ENVMAP;
	info.m_nBumpmap = NORMALMAP;
	info.m_nFlashlightTexture = FLASHLIGHTTEXTURE;
	info.m_nFlashlightTextureFrame = FLASHLIGHTTEXTUREFRAME;
	info.m_nBRDF = BRDF;
	info.m_nUseSmoothness = USESMOOTHNESS;
	info.m_nSeamlessMappingScale = SEAMLESS_SCALE;
	info.ParallaxDepth = PARALLAXDEPTH;
	info.ParallaxScaling = PARALLAXSCALING;
	info.ParallaxStep = PARALLAXSTEP;

	// Parallax cubemaps
	info.m_nEnvmapParallax = ENVMAPPARALLAX;
	info.m_nEnvmapParallaxObb1 = ENVMAPPARALLAXOBB1;
	info.m_nEnvmapParallaxObb2 = ENVMAPPARALLAXOBB2;
	info.m_nEnvmapParallaxObb3 = ENVMAPPARALLAXOBB3;
	info.m_nEnvmapOrigin = ENVMAPORIGIN;
}

void SetupVars( DrawLightPass_Vars_t &info )
{
	info.m_nBaseTexture = BASETEXTURE;
	info.m_nBaseTextureFrame = FRAME;
	info.m_nNoise = NOISE;
	info.m_nBumpmap = NORMALMAP;
	info.m_nRoughness = ROUGHNESS;
	info.m_nMetallic = METALLIC;
	info.m_nBumpmap2 = -1;
	info.m_nBumpFrame2 = -1;
	info.m_nBumpTransform2 = -1;
	info.m_nBaseTexture2 = -1;
	info.m_nBaseTexture2Frame = -1;
	info.m_nSeamlessMappingScale = -1;
	info.bModel = false;
	info.m_nUseSmoothness = USESMOOTHNESS;
}

SHADER_INIT_PARAMS()
{
	LightmappedPBR_DX9_Vars_t info;
	SetupVars( info );
	InitParamsLightmappedPBR_DX9( this, params, pMaterialName, info );
}

SHADER_FALLBACK
{
	return 0;
}

SHADER_INIT
{
	// PBR relies heavily on envmaps
	if ( !params[ENVMAP]->IsDefined() )
		params[ENVMAP]->SetStringValue( "env_cubemap" );
	LightmappedPBR_DX9_Vars_t info;
	SetupVars( info );
	InitLightmappedPBR_DX9( this, params, info );
}

SHADER_DRAW
{
	bool hasFlashlight = UsingFlashlight( params );
	bool bDrawStandardPass = true;

	if ( bDrawStandardPass )
	{
		LightmappedPBR_DX9_Vars_t info;
		SetupVars( info );
		DrawLightmappedPBR_DX9( this, params, pShaderAPI, pShaderShadow, hasFlashlight, info, vertexCompression, pContextDataPtr );
	}
	else
	{
		// Skip this pass!
		Draw( false );
	}
}

END_SHADER