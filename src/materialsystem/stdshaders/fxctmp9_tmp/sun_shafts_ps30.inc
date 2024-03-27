#include "shaderlib/cshader.h"
class sun_shafts_ps30_Static_Index
{
public:
	sun_shafts_ps30_Static_Index( )
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
#define shaderStaticTest_sun_shafts_ps30 0
class sun_shafts_ps30_Dynamic_Index
{
private:
	int m_nQUALITY;
#ifdef _DEBUG
	bool m_bQUALITY;
#endif
public:
	void SetQUALITY( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nQUALITY = i;
#ifdef _DEBUG
		m_bQUALITY = true;
#endif
	}
	void SetQUALITY( bool i )
	{
		m_nQUALITY = i ? 1 : 0;
#ifdef _DEBUG
		m_bQUALITY = true;
#endif
	}
private:
	int m_nSUNBEHIND;
#ifdef _DEBUG
	bool m_bSUNBEHIND;
#endif
public:
	void SetSUNBEHIND( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSUNBEHIND = i;
#ifdef _DEBUG
		m_bSUNBEHIND = true;
#endif
	}
	void SetSUNBEHIND( bool i )
	{
		m_nSUNBEHIND = i ? 1 : 0;
#ifdef _DEBUG
		m_bSUNBEHIND = true;
#endif
	}
public:
	sun_shafts_ps30_Dynamic_Index()
	{
#ifdef _DEBUG
		m_bQUALITY = false;
#endif // _DEBUG
		m_nQUALITY = 0;
#ifdef _DEBUG
		m_bSUNBEHIND = false;
#endif // _DEBUG
		m_nSUNBEHIND = 0;
	}
	int GetIndex()
	{
		// Asserts to make sure that we aren't using any skipped combinations.
		// Asserts to make sure that we are setting all of the combination vars.
#ifdef _DEBUG
		bool bAllDynamicVarsDefined = m_bQUALITY && m_bSUNBEHIND;
		Assert( bAllDynamicVarsDefined );
#endif // _DEBUG
		return ( 1 * m_nQUALITY ) + ( 3 * m_nSUNBEHIND ) + 0;
	}
};
#define shaderDynamicTest_sun_shafts_ps30 psh_forgot_to_set_dynamic_QUALITY + psh_forgot_to_set_dynamic_SUNBEHIND + 0
