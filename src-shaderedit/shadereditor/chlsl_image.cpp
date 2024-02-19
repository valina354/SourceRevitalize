
#include "cbase.h"
#include "editorCommon.h"
#include "view_shared.h"

#include "vtf/vtf.h"
//#include "magick++.h"
#ifdef SHADER_EDITOR_DLL_2006
#include "setjmp.h"
#endif

#include "jpeglib/jpeglib.h"

CHLSL_Image::CHLSL_Image()
{
	pVtf = NULL;
	bEnvmap = false;
	pKVM = NULL;
}

CHLSL_Image::~CHLSL_Image()
{
	DestroyImage();
}

void CHLSL_Image::DestroyImage()
{
	if ( pVtf )
		DestroyVTFTexture( pVtf );
	pVtf = NULL;
}


bool CHLSL_Image::LoadFromVTF( const char *path )
{
	pVtf = CreateVTFTexture();
	if ( !pVtf )
		return false;

	CUtlBuffer vtf;
	bool bSuccess = g_pFullFileSystem->ReadFile( path, 0, vtf ) && pVtf->Unserialize( vtf );
	vtf.Purge();

	if ( !bSuccess )
	{
		DestroyImage();
		return false;
	}

	pVtf->ConvertImageFormat( IMAGE_FORMAT_DEFAULT, false );
	bEnvmap = ( pVtf->Flags() & TEXTUREFLAGS_ENVMAP ) != 0;
	return true;
}

void CHLSL_Image::InitProceduralMaterial()
{
}

void CHLSL_Image::DestroyProceduralMaterial()
{
}

#include "bitmap/tgawriter.h"



CUtlVector<JOCTET> jOut;
#define BLOCK_SIZE 16384

void jInit_destination(j_compress_ptr cinfo)
{
	jOut.Purge();
	jOut.SetSize(BLOCK_SIZE);
	cinfo->dest->next_output_byte = &jOut[0];
	cinfo->dest->free_in_buffer = jOut.Count();
}

boolean jEmpty_output_buffer(j_compress_ptr cinfo)
{
	size_t oldsize = jOut.Count();
	jOut.SetCountNonDestructively(oldsize + BLOCK_SIZE);

	cinfo->dest->next_output_byte = &jOut[oldsize];
	cinfo->dest->free_in_buffer = jOut.Count() - oldsize;
	return true;
}

void jTerm_destination(j_compress_ptr cinfo)
{
	jOut.SetCountNonDestructively(jOut.Count() - cinfo->dest->free_in_buffer);
}