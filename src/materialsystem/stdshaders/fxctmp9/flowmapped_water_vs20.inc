#include "shaderlib/cshader.h"
class flowmapped_water_vs20_Static_Index
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
public:
	flowmapped_water_vs20_Static_Index( )
	{
#ifdef _DEBUG
		m_bBASETEXTURE = false;
#endif // _DEBUG
		m_nBASETEXTURE = 0;
#ifdef _DEBUG
		m_bFLASHLIGHT = false;
#endif // _DEBUG
		m_nFLASHLIGHT = 0;
#ifdef _DEBUG
		m_bLIGHTMAPWATERFOG = false;
#endif // _DEBUG
		m_nLIGHTMAPWATERFOG = 0;
	}
	int GetIndex()
	{
		// Asserts to make sure that we aren't using any skipped combinations.
		// Asserts to make sure that we are setting all of the combination vars.
#ifdef _DEBUG
		bool bAllStaticVarsDefined = m_bBASETEXTURE && m_bFLASHLIGHT && m_bLIGHTMAPWATERFOG;
		Assert( bAllStaticVarsDefined );
#endif // _DEBUG
		return ( 1 * m_nBASETEXTURE ) + ( 2 * m_nFLASHLIGHT ) + ( 4 * m_nLIGHTMAPWATERFOG ) + 0;
	}
};
#define shaderStaticTest_flowmapped_water_vs20 vsh_forgot_to_set_static_BASETEXTURE + vsh_forgot_to_set_static_FLASHLIGHT + vsh_forgot_to_set_static_LIGHTMAPWATERFOG + 0
class flowmapped_water_vs20_Dynamic_Index
{
public:
	flowmapped_water_vs20_Dynamic_Index()
	{
	}
	int GetIndex()
	{
		// Asserts to make sure that we aren't using any skipped combinations.
		// Asserts to make sure that we are setting all of the combination vars.
#ifdef _DEBUG
#endif // _DEBUG
		return 0;
	}
};
#define shaderDynamicTest_flowmapped_water_vs20 0
