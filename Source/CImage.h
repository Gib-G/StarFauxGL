#pragma once
#ifndef _CIMAGE_H_
#define _CIMAGE_H_

#include "Types.h"


/****************************************************************************\
*                                                                            *
*   Class to load & decode an image (PNG, JPG, ...).                         *
*   Decoded data is store in a linear buffer (the pointer "data").           *
*   It should be in format RGB or RGBA (3 or 4 bytes).                       *
*                                                                            *
\****************************************************************************/

class CImage
{
	private:
		
		static void		InitLibrary();							// 
		void			InitData();								// reset variables
		static bool 	bIsLibraryInitialized;					// Unique bool to initialize DevIL (static !)
		uint32_t		ILimgName;								// DevIL image ID (normally, it's an ILuint)


	public:
		 CImage();												// Create an empty image
		 CImage(int w, int h, int bitPerPixel, uint32_t RGBA);	// Create an image
		 CImage(const CImage& img);								// Copy constructor
		~CImage();

		bool		Load			(const string&);			// Detect format and load image file
		void		Unload			();
		
		void		VerticalFlip	();
		void		PixelOrderFlip	();							// Flip RGB <-> BGR (Alpha ?)
		void		RandomFill		();							// Fill image with random values
		void		ConvertRgbaToRgb();
		void		Zoom			(int w,int h,bool lk,int col);
		
		uint8_t*	getPixel		(int x, int y );			// Flow get pixel with assert
		CImage*		GetRectangle	(int x,int y, int w,int h);

		string		name;										// if you need a ID...
		int			lenx;										// largeur de l'image
		int			leny;										// hauteur de l'image
		int			pixelSize;									// 3 ou 4 (bpp or BytesPerPixel)
		bool		hasRgbOrder;								// RGB[A] or BGR[A]
		uint8_t*	data;										// pointer to data
		bool		isOK;										// true if all is ok
};

#endif
