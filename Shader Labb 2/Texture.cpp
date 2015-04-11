#include "stdafx.h"
#include "Texture.h"
#include <d3d10.h>
#include <d3dx10.h>
#include "Engine.h"

DXGI_FORMAT MAKE_SRGB( DXGI_FORMAT format )
{
/*    if( !DXUTIsInGammaCorrectMode() )
        return format;*/

    switch( format )
    {
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UINT:
        case DXGI_FORMAT_R8G8B8A8_SNORM:
        case DXGI_FORMAT_R8G8B8A8_SINT:
            return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

        case DXGI_FORMAT_BC1_TYPELESS:
        case DXGI_FORMAT_BC1_UNORM:
            return DXGI_FORMAT_BC1_UNORM_SRGB;
        case DXGI_FORMAT_BC2_TYPELESS:
        case DXGI_FORMAT_BC2_UNORM:
            return DXGI_FORMAT_BC2_UNORM_SRGB;
        case DXGI_FORMAT_BC3_TYPELESS:
        case DXGI_FORMAT_BC3_UNORM:
            return DXGI_FORMAT_BC3_UNORM_SRGB;

    };

    return format;
}

//--------------------------------------------------------------------------------------
DXGI_FORMAT MAKE_TYPELESS( DXGI_FORMAT format )
{
/*    if( !DXUTIsInGammaCorrectMode() )
        return format;
*/
    switch( format )
    {
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UINT:
        case DXGI_FORMAT_R8G8B8A8_SNORM:
        case DXGI_FORMAT_R8G8B8A8_SINT:
            return DXGI_FORMAT_R8G8B8A8_TYPELESS;

        case DXGI_FORMAT_BC1_UNORM_SRGB:
        case DXGI_FORMAT_BC1_UNORM:
            return DXGI_FORMAT_BC1_TYPELESS;
        case DXGI_FORMAT_BC2_UNORM_SRGB:
        case DXGI_FORMAT_BC2_UNORM:
            return DXGI_FORMAT_BC2_TYPELESS;
        case DXGI_FORMAT_BC3_UNORM_SRGB:
        case DXGI_FORMAT_BC3_UNORM:
            return DXGI_FORMAT_BC3_TYPELESS;
    };

    return format;
}


Texture::Texture()
{
	myTexture=NULL;
	myFileName="empty";
}

Texture::Texture( ID3D10ShaderResourceView* aTexture )
{
	myTexture = aTexture;
	myFileName = "Generated";
}

Texture::~Texture()
{
}

std::string Texture::GetFileName()
{
	return(myFileName);
}

bool Texture::LoadTexture(std::string aTextureName,bool aUseSRGBFlag)
{
   if(LoadTextureAsShaderResourceView(aTextureName,aUseSRGBFlag)==false)
   {
	   return(false);
   }
	myFileName=aTextureName;
	return(true);	
}


bool Texture::LoadTextureAsShaderResourceView(std::string aTextureName,bool aUseSRGBFlag)
{
	D3DX10_IMAGE_LOAD_INFO loadingInfo;
	D3DX10_IMAGE_INFO	imageInfo;
	HRESULT hr = S_OK;
	
	
	SetupLoadingData(loadingInfo,imageInfo,aTextureName);

	ID3D10Texture2D* currentTexture = NULL;
	hr = D3DX10CreateTextureFromFile( Engine::GetInstance()->GetDevice(), aTextureName.c_str(), &loadingInfo, NULL, reinterpret_cast<ID3D10Resource**>(&currentTexture), NULL );
    if( FAILED( hr ) )
	{
		assert( false + "Could not load the texture &s .", aTextureName );
		return false;
	}

	if(aUseSRGBFlag)
	{
		if(CompensateForSRGB(loadingInfo,currentTexture)==false)
		{
			aUseSRGBFlag=false;
		}
	}

	if(BindTextureToView(loadingInfo,currentTexture,aUseSRGBFlag)==false)
	{
		return(false);
	}
	return(true);
}

    

    

bool Texture::SetupLoadingData(D3DX10_IMAGE_LOAD_INFO& someInfo,D3DX10_IMAGE_INFO& sourceInfo,std::string aFileName)
{

    D3DX10GetImageInfoFromFile( aFileName.c_str(), NULL, &sourceInfo, NULL );
    someInfo.pSrcInfo = &sourceInfo;
    someInfo.Format = sourceInfo.Format;

	return(true);
}

bool Texture::CompensateForSRGB(D3DX10_IMAGE_LOAD_INFO& someInfo,ID3D10Texture2D*& aTexture)
{
    // This is a workaround so that we can load linearly, but sample in SRGB.  Right now, we can't load
    // as linear since D3DX will try to do conversion on load.  Loading as TYPELESS doesn't work either, and
    // loading as typed _UNORM doesn't allow us to create an SRGB view.
    if( someInfo.pSrcInfo->ResourceDimension == D3D10_RESOURCE_DIMENSION_TEXTURE2D )
    {
		HRESULT hr = S_OK;
        ID3D10Texture2D* copyTexure = NULL;
        D3D10_TEXTURE2D_DESC copyDescription;
        aTexture->GetDesc( &copyDescription );
        copyDescription.Format = MAKE_TYPELESS( copyDescription.Format );
        hr = Engine::GetInstance()->GetDevice()->CreateTexture2D( &copyDescription, NULL, &copyTexure );
        if( FAILED( hr ) )
            return false;

       Engine::GetInstance()->GetDevice()->CopyResource( copyTexure, aTexture );
        aTexture->Release();

        aTexture = copyTexure;
		return(true);
    }
    return(false);
}

bool Texture::BindTextureToView(D3DX10_IMAGE_LOAD_INFO& someInfo,ID3D10Texture2D*& aTexture,bool aUseSRGBFlag)
{
	HRESULT hr = S_OK;


    D3D10_SHADER_RESOURCE_VIEW_DESC shaderResViewDescription;
    if( aUseSRGBFlag)
        shaderResViewDescription.Format = MAKE_SRGB( someInfo.Format );
    else
        shaderResViewDescription.Format = someInfo.Format;

    if( someInfo.pSrcInfo->ResourceDimension == D3D10_RESOURCE_DIMENSION_TEXTURE2D )
    {
        shaderResViewDescription.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
        shaderResViewDescription.Texture2D.MostDetailedMip = 0;
        shaderResViewDescription.Texture2D.MipLevels = someInfo.pSrcInfo->MipLevels;

        if( someInfo.pSrcInfo->MiscFlags & D3D10_RESOURCE_MISC_TEXTURECUBE )
        {
            shaderResViewDescription.ViewDimension = D3D10_SRV_DIMENSION_TEXTURECUBE;
        }
    }
    else
	{
		assert(0 && "INVALID TEXTURE FORMAT");
	}

    hr = Engine::GetInstance()->GetDevice()->CreateShaderResourceView( aTexture, &shaderResViewDescription, &myTexture );
    aTexture->Release();
    if( FAILED( hr ) )
       return false;
	return(true);

}

ID3D10ShaderResourceView* Texture::GetTexture()
{
	return(myTexture);
}

