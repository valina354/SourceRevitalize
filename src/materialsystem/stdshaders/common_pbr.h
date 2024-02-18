#define PI 3.1415926
float luminance(float3 rgb)
{
    const float3 W = float3(0.5125, 0.7154, 0.7121);
    return dot(rgb, W);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float num   = NdotV;
    float denom = NdotV * (1.0 - roughness) + roughness;
	
    return num / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float r = roughness + 1.0f;
    r = (r * r) / 8.0f;
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, r);
    float ggx1  = GeometrySchlickGGX(NdotL, r);
	
    return ggx1 * ggx2;
} 

float DistributionGGX(float3 N, float3 H, float distL, float roughness)
{
    float alphaPrime = saturate(16.0f / (distL * 2.0) + roughness);
    float a      = roughness * alphaPrime;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float DistributionTrowbridgeReitz(float HN, float roughness, float aP) 
{

    float a2 = roughness * roughness;

    float ap2 = aP * aP;

    return (a2 * ap2) / pow(HN * HN * (a2 - 1.0) + 1.0, 2.0);

}

float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0f.xxx - F0) * pow(1.0f - cosTheta, 5.0);
}  

float3 Diffuse_OrenNayar(float3 DiffuseColor, float Roughness, float NoV, float NoL, float VoH)
{
    float a = Roughness * Roughness;
    float s = a;// / ( 1.29 + 0.5 * a );
    float s2 = s * s;
    float VoL = 2 * VoH * VoH - 1;		// double angle identity
    float Cosri = VoL - NoV * NoL;
    float C1 = 1 - 0.5 * s2 / (s2 + 0.33);
    float C2 = 0.45 * s2 / (s2 + 0.09) * Cosri * (Cosri >= 0 ? 1 / (max(NoL, NoV)) : 1);
    return DiffuseColor / PI * (C1 + C2) * (1 + Roughness * 0.5);
}

float3 DoPBRLight(float3 vWorldPos, float3 vWorldNormal, float3 albedo, float3 vPosition, float3 vColor, float3 vEye, float atten_radius, float3 metallness, float3 rough)
{
	float3 Li = (vPosition - vWorldPos );
    //float3 L = normalize(vPosition - vWorldPos);
	float3 V = normalize( vEye - vWorldPos );
	float3 N = normalize( vWorldNormal );

    float3 r = reflect(-V, N);
    float3 L = Li;
    float3 centerToRay = (dot(L, r) * r) - L;
    float3 closestPoint = L + centerToRay * saturate(4.0f / length(centerToRay));
    L = normalize(closestPoint);

    float3 metallic = clamp(metallness, 0.0f, 0.9f);
    float3 roughness = clamp(rough, 0.015f, 1.0f);
	
    float distance    = length(closestPoint);
	float attenuation = atten_radius;
	float3 radiance     = vColor * attenuation;

    float3 H = normalize(V + L);

    if(luminance(radiance) < 0.01f)
    {
        return 0.0f;
    }

    float HV = max(0.0, dot(H, V));
    float HL = max(0.0, dot(H, L));
    float HN = max(0.0, dot(H, N));
    float LN = max(0.0, dot(L, N));
    float NV = max(0.0, dot(N, V));

    float3 F0 = 0.04f.xxx; 
    F0      = lerp(F0, albedo, metallic);
    float3 F = fresnelSchlick(HL, F0);
    float3 F2 = fresnelSchlick(HV, F0);
    //float3 F = Diffuse_OrenNayar(F0, roughness, NV, LN, HV);

	// D - Calculate normal distribution for specular BRDF.
	float D = DistributionGGX(N, H, length(Li), roughness);
    //float alpha = roughness * roughness;
    //float alphaPrime = clamp(lightRadius / (lenL * 2.0) + alpha, 0.0, 1.0);
    //float D = DistributionTrowbridgeReitz(HN, alpha, alphaPrime);
	
    // Calculate geometric attenuation for specular BRDF.
	float G = GeometrySmith(N, V, L, roughness);

	// Diffuse scattering happens due to light being refracted multiple times by a dielectric medium.
	// Metals on the other hand either reflect or absorb energy so diffuse contribution is always, zero.
	// To be energy conserving we must scale diffuse BRDF contribution based on Fresnel factor & metalness.
    //float3 kd = lerp((1.0f.xxx - F), 0.0f.xxx, metallic.x);
	float3 kd = (1.0f.xxx - F) * (1.0f.xxx - F2) * (1.0f.xxx - metallic);

	float3 diffuseBRDF = (kd * albedo.rgb) / PI;

	// Cook-Torrance specular microfacet BRDF.
	float3 specularBRDF = (F * D * G) / max(0.001, 4.0 * LN * NV);

	return (diffuseBRDF + specularBRDF) * LN * radiance;
}

float random (float2 uv) {
    return frac(sin(dot(uv.xy,
                         float2(12.9898,78.233)))*
        43758.5453123);
}

float3 SampleAmbientReflection(float3 normal, in float3 Ambient, in float3 Ground)
{
    float NU = max(0.0, dot(normal, float3(0.0f, 0.0f, 1.0f)) * 0.5f +0.5f);
    float reflectionTransition = step(NU, 0.5f);
	float3 reflection = lerp(Ground, Ambient, reflectionTransition);
    return reflection;
}

float3 DoAmbient(float2 UV, float3 vWorldPos, float3 vWorldNormal, float3 vEye, in float roughness, in float3 albedo, in float3 Ambient, in float3 Ground)
{
	float3 V = normalize( vEye - vWorldPos );
    float NV = max(0.0, dot(vWorldNormal, V) * 0.5f +0.5f);
    float NU = max(0.0, dot(vWorldNormal, float3(0.0f, 0.0f, 1.0f)) * 0.5f +0.5f);

	float diffuseTransition = NU;
	float3 diffuse = lerp(Ground, Ambient, diffuseTransition);

	HALF3 reflectVect = 2.0 * NV * vWorldNormal - V;
	float3 reflection = 0.0f.xxx;
    for (unsigned int isample = 0; isample < 32; isample++)
	{
		float3 randomvec = float3(random(UV + isample), random(UV + isample + 1), random(UV + isample + 2));
		randomvec = randomvec * 2.0f - 1.0f;
		reflection += SampleAmbientReflection(float4((reflectVect + (randomvec * roughness * roughness * 1.75f)), roughness * 4.0), Ground, Ambient).rgb / 32.0f;
	}


	return albedo.rgb * lerp(reflection, diffuse, roughness);
}

// Compute the matrix used to transform tangent space normals to world space
// This expects DirectX normal maps in Mikk Tangent Space http://www.mikktspace.com
float3x3 compute_tangent_frame(float3 N, float3 P, float2 uv, out float3 T, out float3 B, out float sign_det)
{
    float3 dp1 = ddx(P);
    float3 dp2 = ddy(P);
    float2 duv1 = ddx(uv);
    float2 duv2 = ddy(uv);

    sign_det = dot(dp2, cross(N, dp1)) > 0.0 ? -1 : 1;

    float3x3 M = float3x3(dp1, dp2, cross(dp1, dp2));
    float2x3 inverseM = float2x3(cross(M[1], M[2]), cross(M[2], M[0]));
    T = normalize(mul(float2(duv1.x, duv2.x), inverseM));
    B = normalize(mul(float2(duv1.y, duv2.y), inverseM));
    return float3x3(T, B, N);
}

#if PARALLAXOCCLUSION
float2 parallaxCorrect(float2 texCoord, float3 viewRelativeDir, sampler BumpmapSampler, float parallaxDepth, float parallaxCenter)
{
    float fLength = length( viewRelativeDir );
    float fParallaxLength = sqrt( fLength * fLength - viewRelativeDir.z * viewRelativeDir.z ) / viewRelativeDir.z; 
    float2 vParallaxDirection = normalize(  viewRelativeDir.xy );
    float2 vParallaxOffsetTS = vParallaxDirection * fParallaxLength;
    vParallaxOffsetTS *= parallaxDepth;

     // Compute all the derivatives:
    float2 dx = ddx( texCoord );
    float2 dy = ddy( texCoord );

    int nNumSteps = 20;

    float fCurrHeight = 0.0;
    float fStepSize   = 1.0 / (float) nNumSteps;
    float fPrevHeight = 1.0;
    float fNextHeight = 0.0;

    int    nStepIndex = 0;
    bool   bCondition = true;

    float2 vTexOffsetPerStep = fStepSize * vParallaxOffsetTS;
    float2 vTexCurrentOffset = texCoord;
    float  fCurrentBound     = 1.0;
    float  fParallaxAmount   = 0.0;

    float2 pt1 = 0;
    float2 pt2 = 0;

    float2 texOffset2 = 0;

    [unroll]
    while ( nStepIndex < nNumSteps ) 
    {
        vTexCurrentOffset -= vTexOffsetPerStep;

        // Sample height map which in this case is stored in the alpha channel of the normal map:
        fCurrHeight = parallaxCenter + tex2Dgrad( BumpmapSampler, vTexCurrentOffset, dx, dy ).a;

        fCurrentBound -= fStepSize;

        if ( fCurrHeight > fCurrentBound ) 
        {     
            pt1 = float2( fCurrentBound, fCurrHeight );
            pt2 = float2( fCurrentBound + fStepSize, fPrevHeight );

            texOffset2 = vTexCurrentOffset - vTexOffsetPerStep;

            nStepIndex = nNumSteps + 1;
        }
        else
        {
            nStepIndex++;
            fPrevHeight = fCurrHeight;
        }
    }   // End of while ( nStepIndex < nNumSteps )

    float fDelta2 = pt2.x - pt2.y;
    float fDelta1 = pt1.x - pt1.y;
    fParallaxAmount = (pt1.x * fDelta2 - pt2.x * fDelta1 ) / ( fDelta2 - fDelta1 );
    float2 vParallaxOffset = vParallaxOffsetTS * (1 - fParallaxAmount);
    // The computed texture offset for the displaced point on the pseudo-extruded surface:
    float2 texSample = texCoord - vParallaxOffset;
    return texSample;
}
#else
float2 parallaxCorrect(float2 texCoord, float3 viewRelativeDir, sampler BumpmapSampler, sampler depthMap2, float blend, float parallaxDepth, float parallaxCenter)
{
    float fLength = length( viewRelativeDir );
    float fParallaxLength = sqrt( fLength * fLength - viewRelativeDir.z * viewRelativeDir.z ) / viewRelativeDir.z; 
    float2 vParallaxDirection = normalize(  viewRelativeDir.xy );
    float2 vParallaxOffsetTS = vParallaxDirection * fParallaxLength;
    vParallaxOffsetTS *= parallaxDepth;

     // Compute all the derivatives:
    float2 dx = ddx( texCoord );
    float2 dy = ddy( texCoord );

    int nNumSteps = 20;

    float fCurrHeight = 0.0;
    float fStepSize   = 1.0 / (float) nNumSteps;
    float fPrevHeight = 1.0;
    float fNextHeight = 0.0;

    int    nStepIndex = 0;
    bool   bCondition = true;

    float2 vTexOffsetPerStep = fStepSize * vParallaxOffsetTS;
    float2 vTexCurrentOffset = texCoord;
    float  fCurrentBound     = 1.0;
    float  fParallaxAmount   = 0.0;

    float2 pt1 = 0;
    float2 pt2 = 0;

    float2 texOffset2 = 0;

    [unroll]
    while ( nStepIndex < nNumSteps ) 
    {
        vTexCurrentOffset -= vTexOffsetPerStep;

        // Sample height map which in this case is stored in the alpha channel of the normal map:
        fCurrHeight = parallaxCenter + lerp(tex2Dgrad( BumpmapSampler, vTexCurrentOffset, dx, dy ).a, tex2Dgrad( depthMap2, vTexCurrentOffset, dx, dy ).a, blend);

        fCurrentBound -= fStepSize;

        if ( fCurrHeight > fCurrentBound ) 
        {     
            pt1 = float2( fCurrentBound, fCurrHeight );
            pt2 = float2( fCurrentBound + fStepSize, fPrevHeight );

            texOffset2 = vTexCurrentOffset - vTexOffsetPerStep;

            nStepIndex = nNumSteps + 1;
        }
        else
        {
            nStepIndex++;
            fPrevHeight = fCurrHeight;
        }
    }   // End of while ( nStepIndex < nNumSteps )

    float fDelta2 = pt2.x - pt2.y;
    float fDelta1 = pt1.x - pt1.y;
    fParallaxAmount = (pt1.x * fDelta2 - pt2.x * fDelta1 ) / ( fDelta2 - fDelta1 );
    float2 vParallaxOffset = vParallaxOffsetTS * (1 - fParallaxAmount);
    // The computed texture offset for the displaced point on the pseudo-extruded surface:
    float2 texSample = texCoord - vParallaxOffset;
    return texSample;
}
#endif

float3 worldToRelative(float3 worldVector, float3 surfTangent, float3 surfBasis, float3 surfNormal)
{
   return float3(
       dot(worldVector, surfTangent),
       dot(worldVector, surfBasis),
       dot(worldVector, surfNormal)
   );
}