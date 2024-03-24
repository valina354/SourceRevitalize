//===================== Copyright (c) AdV Software And Source Revitalize For Improving It. All Rights Reserved. ======================
//
// The .cpp File for the model PBR shader
//
//==================================================================================================

#include "BaseVSShader.h"
#include "convar.h"
#include "vertexlitpbr_dx9_helper.h"
#include "lightpass_helper.h"



#ifdef STDSHADER
BEGIN_VS_SHADER(VertexLitPBR,
	"Help for LightmappedPBR")
#else
BEGIN_VS_SHADER(VertexLitPBR,
	"Help for LightmappedPBR")
#endif

BEGIN_SHADER_PARAMS
SHADER_PARAM(ALPHATESTREFERENCE, SHADER_PARAM_TYPE_FLOAT, "0.0", "")
SHADER_PARAM(ENVMAP, SHADER_PARAM_TYPE_TEXTURE, "shadertest/shadertest_env", "envmap")
SHADER_PARAM(NORMALMAP, SHADER_PARAM_TYPE_TEXTURE, "models/shadertest/shader1_normal", "bump map")

SHADER_PARAM(BRDF, SHADER_PARAM_TYPE_TEXTURE, "models/PBRTest/BRDF", "")
SHADER_PARAM(NOISE, SHADER_PARAM_TYPE_TEXTURE, "shaders/bluenoise", "")
SHADER_PARAM(ROUGHNESS, SHADER_PARAM_TYPE_TEXTURE, "", "")
SHADER_PARAM(METALLIC, SHADER_PARAM_TYPE_TEXTURE, "", "")
SHADER_PARAM(AO, SHADER_PARAM_TYPE_TEXTURE, "", "")
SHADER_PARAM(EMISSIVE, SHADER_PARAM_TYPE_TEXTURE, "", "")
SHADER_PARAM( DETAIL, SHADER_PARAM_TYPE_TEXTURE, "", "" )
SHADER_PARAM( DETAILSTRENGTH, SHADER_PARAM_TYPE_TEXTURE, "", "" )
SHADER_PARAM(LIGHTMAP, SHADER_PARAM_TYPE_TEXTURE, "shadertest/BaseTexture", "lightmap texture--will be bound by the engine")

SHADER_PARAM(USESMOOTHNESS, SHADER_PARAM_TYPE_BOOL, "0", "Invert roughness")
SHADER_PARAM( NORMALMAPALPHASMOOTHNESS, SHADER_PARAM_TYPE_BOOL, "0", "Use the alpha channel of bumpmap as inverted roughness" )
END_SHADER_PARAMS

void SetupVars(VertexLitPBR_DX9_Vars_t& info)
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
	info.m_nDetailStrength = DETAILSTRENGTH;
	info.m_nEnvmap = ENVMAP;
	info.m_nBumpmap = NORMALMAP;
	info.m_nFlashlightTexture = FLASHLIGHTTEXTURE;
	info.m_nFlashlightTextureFrame = FLASHLIGHTTEXTUREFRAME;
	info.m_nBRDF = BRDF;
	info.m_nUseSmoothness = USESMOOTHNESS;
	info.m_nLightmap = LIGHTMAP;
	info.m_nBumpAlphaSmoothness = NORMALMAPALPHASMOOTHNESS;
}

void SetupVars(DrawLightPass_Vars_t& info)
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
	info.bModel = true;
	info.m_nUseSmoothness = USESMOOTHNESS;
}

SHADER_INIT_PARAMS()
{
	// PBR relies heavily on envmaps
	if ( !params[ENVMAP]->IsDefined() )
		params[ENVMAP]->SetStringValue( "env_cubemap" );

	if ( params[NORMALMAP]->IsDefined() && ( !LIGHTMAP ) )
	{
		params[NORMALMAP]->SetUndefined();
	}
	VertexLitPBR_DX9_Vars_t info;
	SetupVars(info);
	InitParamsVertexLitPBR_DX9(this, params, pMaterialName, info);
}

SHADER_FALLBACK
{
	return 0;
}

SHADER_INIT
{
	VertexLitPBR_DX9_Vars_t info;
	SetupVars(info);
	InitVertexLitPBR_DX9(this, params, info);
}

SHADER_DRAW
{
	bool bDrawStandardPass = true;
	bool hasFlashlight = UsingFlashlight(params);

	if (bDrawStandardPass)
	{
		VertexLitPBR_DX9_Vars_t info;
		SetupVars(info);
		DrawVertexLitPBR_DX9(this, params, pShaderAPI, pShaderShadow, hasFlashlight, info, vertexCompression, pContextDataPtr);
	}
	else
	{
		// Skip this pass!
		Draw(false);
	}

}

END_SHADER

// Compatability
BEGIN_SHADER_FLAGS( PBR, "", SHADER_NOT_EDITABLE )
BEGIN_SHADER_PARAMS
END_SHADER_PARAMS
SHADER_FALLBACK
{
	return IS_FLAG_SET( MATERIAL_VAR_MODEL ) ? "VertexLitPBR" : "LightmappedPBR";
}
SHADER_INIT
{
}
SHADER_DRAW
{
}
END_SHADER