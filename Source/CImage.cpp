
#include "CImage.h"
#include <IL/il.h>
#include <IL/ilu.h>
#include "StringUtil.h"


/****************************************************************************\
*                                                                            *
*                            Variables EXTERNES                              *
*                                                                            *
\****************************************************************************/


/****************************************************************************\
*                                                                            *
*                            Variables GLOBALES                              *
*                                                                            *
\****************************************************************************/


/****************************************************************************\
*                                                                            *
*                             Variables LOCALES                              *
*                                                                            *
\****************************************************************************/
bool CImage::bIsLibraryInitialized = false;


/*************************************************************************\
*                                                                         *
*                                                                         *
*                                                                         *
\*************************************************************************/
void CImage::InitLibrary()
{
	if (bIsLibraryInitialized == false)
	{
		assert(sizeof(ILuint) == sizeof(u32));	// just to be sure, because I replace ILuint by u32 in the header
	
		ILint ILversion = ilGetInteger(IL_VERSION_NUM);
		if (ILversion < IL_VERSION)
		{
			ConsoleWrite("CImage::CImage() : DevIL DLL version problem ! (compiled with: v%d, DLL: v%d", ILversion, IL_VERSION);
			return;
		}
	
		// ================= //
		// IL Initialisation //
		// ================= //
		ilInit();
		ILenum iErr = ilGetError();
		if (iErr)
		{
			do
			{
				ConsoleWriteErr("CImage::CImage() : ilInit() failed ! (error: 0x%X)", iErr);
				iErr = ilGetError();
			}
			while (iErr != IL_NO_ERROR);
			return;
		}
		
		// ================== //
		// ILU Initialisation //
		// ================== //
		iluInit();
		iErr = ilGetError();
		if (iErr)
		{
			do
			{
				ConsoleWriteErr("CImage::CImage() : iluInit() failed ! (error: 0x%X)", iErr);
				iErr = ilGetError();
			}
			while (iErr != IL_NO_ERROR);
			return;
		}

		string vendor = WStringToString(ilGetString(IL_VENDOR));
		ConsoleWriteOk("DevIL (image library) :");
		ConsoleWrite  (" -> version : %d.%d.%d", ILversion/100, (ILversion/10)%10, ILversion%10);
		ConsoleWrite  (" -> vendor  : %s", vendor.c_str());

		bIsLibraryInitialized = true;
	}
}

/*************************************************************************\
*                                                                         *
*  Create an empty image (no memory allocation)                           *
*                                                                         *
\*************************************************************************/
CImage::CImage()
{
	InitLibrary();
	InitData();
}

/*************************************************************************\
*                                                                         *
*  Create an image (filled with the specified color)                      *
*  RGBA: R=hi-order byte, A=low-order byte.                               *
*                                                                         *
\*************************************************************************/
CImage::CImage(int w, int h, int BitPerPixel, u32 RGBA)
{
	InitLibrary();
	InitData();
	
	ilGenImages(1, &ILimgName);
	ilBindImage(ILimgName);
	ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
	ilEnable(IL_ORIGIN_SET);

	if (BitPerPixel == 24)
	{
		pixelSize = 3;
		ilTexImage(w,h,1,pixelSize, IL_RGB, IL_UNSIGNED_BYTE, NULL);
	}
	else if (BitPerPixel == 32)
	{
		pixelSize = 4;
		ilTexImage(w,h,1,pixelSize, IL_RGBA, IL_UNSIGNED_BYTE, NULL);
	}
	else if (BitPerPixel == 8)
	{
		pixelSize = 1;
		ilTexImage(w,h,1,pixelSize, IL_LUMINANCE, IL_UNSIGNED_BYTE, NULL);
		ilClearImage();
		lenx		= w;
		leny		= h;
		hasRgbOrder	= false;
		isOK		= true;
		data		= (u8*)ilGetData();
		return;
	}
	
	u8 r,g,b,a;
	r = (RGBA & 0xFF000000) >> 24;
	g = (RGBA & 0x00FF0000) >> 16;
	b = (RGBA & 0x0000FF00) >> 8;
	a = (RGBA & 0x000000FF);
	ilClearColour(r,g,b,a);
	ilClearImage();

	lenx		= w;
	leny		= h;
	hasRgbOrder	= true;
	isOK		= true;
	data		= (u8*)ilGetData();
}

/*************************************************************************\
*                                                                         *
*  copy constructor     CODE A CONDENSER - DOUBLON                        *
*                                                                         *
\*************************************************************************/
CImage::CImage(const CImage& img)
{
	InitLibrary();
	InitData();

	ilGenImages(1, &ILimgName);
	ilBindImage(ILimgName);
	ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
	ilEnable(IL_ORIGIN_SET);
	ilCopyImage(img.ILimgName);
	
	lenx		= img.lenx;
	leny		= img.leny;
	pixelSize	= img.pixelSize;
	hasRgbOrder	= img.hasRgbOrder;
	isOK		= img.isOK;
	data		= (u8*)ilGetData();
	name		= img.name;
}

/*************************************************************************\
*                                                                         *
*  Destructor                                                             *
*                                                                         *
\*************************************************************************/
CImage::~CImage()
{
	Unload();
}

/*************************************************************************\
*                                                                         *
*  Load an image                                                          *
*                                                                         *
\*************************************************************************/
bool CImage::Load(const string& _fileName)
{
	if (bIsLibraryInitialized)
	{
		Unload();
		
		wstring file = StringToWString(_fileName);

		ilGenImages(1, &ILimgName);
		ilBindImage(ILimgName);
		ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
		ilEnable(IL_ORIGIN_SET);
		ilLoadImage(file.c_str());

		ILenum err = ilGetError();
		if (err == IL_NO_ERROR)
		{
			ILuint fmt = (int)ilGetInteger(IL_IMAGE_FORMAT);
			if (fmt == IL_RGBA || fmt == IL_BGRA)
			{
				ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
			}
			else
			{
				ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
			}

			lenx		= (int)ilGetInteger(IL_IMAGE_WIDTH);
			leny		= (int)ilGetInteger(IL_IMAGE_HEIGHT);
			pixelSize	= (int)ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
			hasRgbOrder	= true;
			isOK		= true;
			data		= (u8*)ilGetData();

			return true;
		}
		else
		{
			while (err != IL_NO_ERROR)
			{
				string errMsg = WStringToString(iluErrorString(err));
				ConsoleWriteErr("CImage::LoadImage() : DevIL failed ! (error: %s)", errMsg.c_str());
				ConsoleWriteErr("                    : file: %s",_fileName.c_str());
				err = ilGetError();
			}
			Unload();
		}
	}
	
	ConsoleWriteErr("CImage::LoadImage() : failed on \"%s\"", _fileName.c_str());
	lenx		= 64;
	leny		= 64;
	pixelSize	= 3;
	data		= new u8[lenx*leny*pixelSize];
	isOK		= true;
	RandomFill();
	
	return false;
}

/*************************************************************************\
*                                                                         *
*  Slow, for debug purpose only                                           *
*                                                                         *
\*************************************************************************/
uint8_t* CImage::getPixel(int x, int y)
{

#if defined(DEBUG)
	if (x<0 || x>=lenx || y<0 || y>=leny)
	{
		assert(false);
	}
#endif

	return data+pixelSize*(x+y*lenx);	
}

/*************************************************************************\
*                                                                         *
*                                                                         *
*                                                                         *
\*************************************************************************/
void CImage::InitData()
{
	name.clear();
	ILimgName	= 0;
	data		= NULL;
	lenx		= 0;
	leny		= 0;
	pixelSize	= 3;
	isOK		= false;
	hasRgbOrder	= true;
}

/*************************************************************************\
*                                                                         *
*                                                                         *
*                                                                         *
\*************************************************************************/
void CImage::Unload()
{
	if (ILimgName != 0)
	{
		ilDeleteImages(1, &ILimgName);
	}
	InitData();
}

/*************************************************************************\
*                                                                         *
*   Verticaly invert the image                                            *
*                                                                         *
\*************************************************************************/
void CImage::VerticalFlip()
{
	ilBindImage(ILimgName);	// check if there is no bug in here, I had crashes
	iluFlipImage();
}

/*************************************************************************\
*                                                                         *
*  Flips pixel order from rgb[a] to bgr[a].                               *
*  2 calls cancel the fliping.                                            *
*                                                                         *
\*************************************************************************/
void CImage::PixelOrderFlip()
{
	hasRgbOrder = !hasRgbOrder;
		
	int len = lenx*leny*pixelSize;
	for (int i=0; i<len; i+=pixelSize)
	{
		u8 tmp    = data[i+0];
		data[i+0] = data[i+2];
		data[i+2] = tmp;
	} 
}

/*************************************************************************\
*                                                                         *
*  Fill image with random values                                          *
*                                                                         *
\*************************************************************************/
void CImage::RandomFill()
{
	if (isOK)
	{
		int len = lenx*leny*pixelSize;
		for (int i=0; i<len; i++)
		{
			data[i] = rand()&0xFF;
		}	
	}
}

/*************************************************************************\
*                                                                         *
*  Zooms the image to the wanted size.                                    *
*  If lockAspect is true, the function fits the image in the new size,    *
*  and background borders use RGBA_backColor.                             *
*                                                                         *
\*************************************************************************/
void CImage::Zoom(int newWidth, int newHeight, bool lockAspect, int RGBA_backColor)
{
	if (isOK == false)
	{
		ConsoleWriteErr("CImage::Zoom(..) : invalid image");
		return;
	}
	if (pixelSize != 3 && pixelSize != 4) 
	{
		ConsoleWriteErr("CImage::Zoom(..) : wrong pixelSize (%d)", pixelSize);
		return;
	}
	if (newWidth == 0 || newHeight == 0)
	{
		ConsoleWriteErr("CImage::Zoom(..) : wrong size (%d x %d)", newWidth, newHeight);
		return;
	}
	
	u32  subImgW=1, subImgH=1;	// locked-aspect ratio "sub-image" size in new image.

	float newAspect = (float)newWidth/(float)newHeight;
	float oldAspect = (float)lenx    /(float)leny;
	float delta     = 0.001f;

	if (lockAspect)
	{
		if (newAspect < oldAspect-delta)
		{
			subImgW = newWidth;
			subImgH = (int)( (float)(leny*newWidth) / (float)lenx );
			if (subImgH == 0)
			{
				subImgH++;
			}
		}
		else if (newAspect > oldAspect+delta)
		{
			subImgW = (int)( (float)(lenx*newHeight) / (float)leny );
			subImgH = newHeight;
			if (subImgW == 0)
			{
				subImgW++;
			}
		}
	}
	else
	{
		subImgW = newWidth;
		subImgH = newHeight;
	}

	ilBindImage(ILimgName);
	iluImageParameter(ILU_FILTER,    ILU_BILINEAR);
	iluImageParameter(ILU_PLACEMENT, ILU_CENTER  );
	if (lockAspect)
	{
		u8 r,g,b,a;
		r = (RGBA_backColor & 0xFF000000) >> 24;
		g = (RGBA_backColor & 0x00FF0000) >> 16;
		b = (RGBA_backColor & 0x0000FF00) >> 8;
		a = (RGBA_backColor & 0x000000FF);
		ilClearColour(r,g,b,a);
		iluScale(subImgW, subImgH, 1);
		iluEnlargeCanvas(newWidth, newHeight, 1);
	}
	else
	{
		iluScale(newWidth, newHeight, 1);
	}
	
	lenx = newWidth;
	leny = newHeight;
	data = (u8*)ilGetData();
}

/*************************************************************************\
*                                                                         *
*   get a a sub part from an image                                        *
*                                                                         *
\*************************************************************************/
CImage* CImage::GetRectangle(int x,int y, int w,int h)
{
	if (isOK == false)
	{
		ConsoleWriteErr("CImage::GetRectangle(..) : invalid image");
		return NULL;
	}
	if (x+w>=lenx || y+h>=leny || w==0 || h==0)
	{
		ConsoleWriteErr("CImage::GetRectangle(..) : out of range parameter(s)");
		return NULL;
	}
	CImage*	newImg = new CImage(w, h, pixelSize*8, 0);

	ILenum Format = 0;
	if (pixelSize == 3)
	{
		Format = hasRgbOrder ? IL_RGB : IL_BGR;
	}
	else if (pixelSize == 4)
	{
		Format = hasRgbOrder ? IL_RGBA : IL_BGRA;
	}
	
	ilBindImage(ILimgName);
	ilCopyPixels(x,y,1,  w,h,1,  Format, IL_UNSIGNED_BYTE, newImg->data);
	

	return newImg;
}

/*************************************************************************\
*                                                                         *
*   RGBA->RGB conversion                                                  *
*                                                                         *
\*************************************************************************/
void CImage::ConvertRgbaToRgb()
{
	if (isOK && pixelSize==4)
	{
		ilBindImage(ILimgName);
		
		ILuint fmt = (int)ilGetInteger(IL_IMAGE_FORMAT);
		if (fmt == IL_RGBA || fmt == IL_BGRA)
		{
			ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
			pixelSize	= (int)ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
			data		= (u8*)ilGetData();
		}
	}
}
