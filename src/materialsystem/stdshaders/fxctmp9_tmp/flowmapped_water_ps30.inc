#include "shaderlib/cshader.h"
class flowmapped_water_ps30_Static_Index
{
private:
	int m_nBASETEXTURE;
#ifdef _DEBUG
	bool m_bBASETEXTURE;
#endif
public:
	void SetBASETEXTURE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBASETEXTURE = i;
#ifdef _DEBUG
		m_bBASETEXTURE = true;
#endif
	}
	void SetBASETEXTURE( bool i )
	{
		m_nBASETEXTURE = i ? 1 : 0;
#ifdef _DEBUG
		m_bBASETEXTURE = true;
#endif
	}
private:
	int m_nENVMAP;
#ifdef _DEBUG
	bool m_bENVMAP;
#endif
public:
	void SetENVMAP( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nENVMAP = i;
#ifdef _DEBUG
		m_bENVMAP = true;
#endif
	}
	void SetENVMAP( bool i )
	{
		m_nENVMAP = i ? 1 : 0;
#ifdef _DEBUG
		m_bENVMAP = true;
#endif
	}
private:
	int m_nREFRACT;
#ifdef _DEBUG
	bool m_bREFRACT;
#endif
public:
	void SetREFRACT( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nREFRACT = i;
#ifdef _DEBUG
		m_bREFRACT = true;
#endif
	}
	void SetREFRACT( bool i )
	{
		m_nREFRACT = i ? 1 : 0;
#ifdef _DEBUG
		m_bREFRACT = true;
#endif
	}
private:
	int m_nABOVEWATER;
#ifdef _DEBUG
	bool m_bABOVEWATER;
#endif
public:
	void SetABOVEWATER( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nABOVEWATER = i;
#ifdef _DEBUG
		m_bABOVEWATER = true;
#endif
	}
	void SetABOVEWATER( bool i )
	{
		m_nABOVEWATER = i ? 1 : 0;
#ifdef _DEBUG
		m_bABOVEWATER = true;
#endif
	}
private:
	int m_nFLOW_DEBUG;
#ifdef _DEBUG
	bool m_bFLOW_DEBUG;
#endif
public:
	void SetFLOW_DEBUG( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nFLOW_DEBUG = i;
#ifdef _DEBUG
		m_bFLOW_DEBUG = true;
#endif
	}
	void SetFLOW_DEBUG( bool i )
	{
		m_nFLOW_DEBUG = i ? 1 : 0;
#ifdef _DEBUG
		m_bFLOW_DEBUG = true;
#endif
	}
private:
	int m_nFLASHLIGHT;
#ifdef _DEBUG
	bool m_bFLASHLIGHT;
#endif
public:
	void SetFLASHLIGHT( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nFLASHLIGHT = i;
#ifdef _DEBUG
		m_bFLASHLIGHT = true;
#endif
	}
	void SetFLASHLIGHT( bool i )
	{
		m_nFLASHLIGHT = i ? 1 : 0;
#ifdef _DEBUG
		m_bFLASHLIGHT = true;
#endif
	}
private:
	int m_nLIGHTMAPWATERFOG;
#ifdef _DEBUG
	bool m_bLIGHTMAPWATERFOG;
#endif
public:
	void SetLIGHTMAPWATERFOG( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nLIGHTMAPWATERFOG = i;
#ifdef _DEBUG
		m_bLIGHTMAPWATERFOG = true;
#endif
	}
	void SetLIGHTMAPWATERFOG( bool i )
	{
		m_nLIGHTMAPWATERFOG = i ? 1 : 0;
#ifdef _DEBUG
		m_bLIGHTMAPWATERFOG = true;
#endif
	}
private:
	int m_nFORCEFRESNEL;
#ifdef _DEBUG
	bool m_bFORCEFRESNEL;
#endif
public:
	void SetFORCEFRESNEL( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nFORCEFRESNEL = i;
#ifdef _DEBUG
		m_bFORCEFRESNEL = true;
#endif
	}
	void SetFORCEFRESNEL( bool i )
	{
		m_nFORCEFRESNEL = i ? 1 : 0;
#ifdef _DEBUG
		m_bFORCEFRESNEL = true;
#endif
	}
public:
	flowmapped_water_ps30_Static_Index( )
	{
#ifdef _DEBUG
		m_bBASETEXTURE = false;
#endif // _DEBUG
		m_nBASETEXTURE = 0;
#ifdef _DEBUG
		m_bENVMAP = false;
#endif // _DEBUG
		m_nENVMAP = 0;
#ifdef _DEBUG
		m_bREFRACT = false;
#endif // _DEBUG
		m_nREFRACT = 0;
#ifdef _DEBUG
		m_bABOVEWATER = false;
#endif // _DEBUG
		m_nABOVEWATER = 0;
#ifdef _DEBUG
		m_bFLOW_DEBUG = false;
#endif // _DEBUG
		m_nFLOW_DEBUG = 0;
#ifdef _DEBUG
		m_bFLASHLIGHT = false;
#endif // _DEBUG
		m_nFLASHLIGHT = 0;
#ifdef _DEBUG
		m_bLIGHTMAPWATERFOG = false;
#endif // _DEBUG
		m_nLIGHTMAPWATERFOG = 0;
#ifdef _DEBUG
		m_bFORCEFRESNEL = false;
#endif // _DEBUG
		m_nFORCEFRESNEL = 0;
	}
	int GetIndex()
	{
		// Asserts to make sure that we aren't using any skipped combinations.
		// Asserts to make sure that we are setting all of the combination vars.
#ifdef _DEBUG
		bool bAllStaticVarsDefined = m_bBASETEXTURE && m_bENVMAP && m_bREFRACT && m_bABOVEWATER && m_bFLOW_DEBUG && m_bFLASHLIGHT && m_bLIGHTMAPWATERFOG && m_bFORCEFRESNEL;
		Assert( bAllStaticVarsDefined );
#endif // _DEBUG
		return ( 8 * m_nBASETEXTURE ) + ( 16 * m_nENVMAP ) + ( 32 * m_nREFRACT ) + ( 64 * m_nABOVEWATER ) + ( 128 * m_nFLOW_DEBUG ) + ( 384 * m_nFLASHLIGHT ) + ( 768 * m_nLIGHTMAPWATERFOG ) + ( 1536 * m_nFORCEFRESNEL ) + 0;
	}
};
#define shaderStaticTest_flowmapped_water_ps30 psh_forgot_to_set_static_BASETEXTURE + psh_forgot_to_set_static_ENVMAP + psh_forgot_to_set_static_REFRACT + psh_forgot_to_set_static_ABOVEWATER + psh_forgot_to_set_static_FLOW_DEBUG + psh_forgot_to_set_static_FLASHLIGHT + psh_forgot_to_set_static_LIGHTMAPWATERFOG + psh_forgot_to_set_static_FORCEFRESNEL + 0
class flowmapped_water_ps30_Dynamic_Index
{
private:
	int m_nFLASHLIGHTSHADOWS;
#ifdef _DEBUG
	bool m_bFLASHLIGHTSHADOWS;
#endif
public:
	void SetFLASHLIGHTSHADOWS( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nFLASHLIGHTSHADOWS = i;
#ifdef _DEBUG
		m_bFLASHLIGHTSHADOWS = true;
#endif
	}
	void SetFLASHLIGHTSHADOWS( bool i )
	{
		m_nFLASHLIGHTSHADOWS = i ? 1 : 0;
#ifdef _DEBUG
		m_bFLASHLIGHTSHADOWS = true;
#endif
	}
private:
	int m_nWRITE_DEPTH_TO_DESTALPHA;
#ifdef _DEBUG
	bool m_bWRITE_DEPTH_TO_DESTALPHA;
#endif
public:
	void SetWRITE_DEPTH_TO_DESTALPHA( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nWRITE_DEPTH_TO_DESTALPHA = i;
#ifdef _DEBUG
		m_bWRITE_DEPTH_TO_DESTALPHA = true;
#endif
	}
	void SetWRITE_DEPTH_TO_DESTALPHA( bool i )
	{
		m_nWRITE_DEPTH_TO_DESTALPHA = i ? 1 : 0;
#ifdef _DEBUG
		m_bWRITE_DEPTH_TO_DESTALPHA = true;
#endif
	}
private:
	int m_nPIXELFOGTYPE;
#ifdef _DEBUG
	bool m_bPIXELFOGTYPE;
#endif
public:
	void SetPIXELFOGTYPE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nPIXELFOGTYPE = i;
#ifdef _DEBUG
		m_bPIXELFOGTYPE = true;
#endif
	}
	void SetPIXELFOGTYPE( bool i )
	{
		m_nPIXELFOGTYPE = i ? 1 : 0;
#ifdef _DEBUG
		m_bPIXELFOGTYPE = true;
#endif
	}
public:
	flowmapped_water_ps30_Dynamic_Index()
	{
#ifdef _DEBUG
		m_bFLASHLIGHTSHADOWS = false;
#endif // _DEBUG
		m_nFLASHLIGHTSHADOWS = 0;
#ifdef _DEBUG
		m_bWRITE_DEPTH_TO_DESTALPHA = false;
#endif // _DEBUG
		m_nWRITE_DEPTH_TO_DESTALPHA = 0;
#ifdef _DEBUG
		m_bPIXELFOGTYPE = false;
#endif // _DEBUG
		m_nPIXELFOGTYPE = 0;
	}
	int GetIndex()
	{
		// Asserts to make sure that we aren't using any skipped combinations.
		// Asserts to make sure that we are setting all of the combination vars.
#ifdef _DEBUG
		bool bAllDynamicVarsDefined = m_bFLASHLIGHTSHADOWS && m_bWRITE_DEPTH_TO_DESTALPHA && m_bPIXELFOGTYPE;
		Assert( bAllDynamicVarsDefined );
#endif // _DEBUG
		return ( 1 * m_nFLASHLIGHTSHADOWS ) + ( 2 * m_nWRITE_DEPTH_TO_DESTALPHA ) + ( 4 * m_nPIXELFOGTYPE ) + 0;
	}
};
#define shaderDynamicTest_flowmapped_water_ps30 psh_forgot_to_set_dynamic_FLASHLIGHTSHADOWS + psh_forgot_to_set_dynamic_WRITE_DEPTH_TO_DESTALPHA + psh_forgot_to_set_dynamic_PIXELFOGTYPE + 0
