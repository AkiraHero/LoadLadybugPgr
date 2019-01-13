#include "LoadLadyBugPgr.h"


#define COLOR_PROCESSING_METHOD		LADYBUG_HQLINEAR
#define OUTPUT_IMAGE_TYPE			LADYBUG_PANORAMIC

//#pragma comment( lib, "ladybug.lib" )

PGR::PGR()
{
	context=NULL;
	readContext=NULL;
	iTextureWidth=0;
	iTextureHeight=0;
	cur=-1;
	PANORAMIC_IMAGE_COLUMNS = 2048 * ResizeLadybugRadio;
	PANORAMIC_IMAGE_ROWS = 1024 * ResizeLadybugRadio;
	//GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
#if isRectifyNeeded
	RECTIFIED_IMAGE_COLUMNS = 808 * ResizeLadybugRadio;
	RECTIFIED_IMAGE_ROWS = 616 * ResizeLadybugRadio;
#endif
}

PGR::~PGR()
{
	if(readContext!=NULL)
	{
		ladybugStopStream(readContext);
		ladybugDestroyStreamContext( &readContext);
		readContext=NULL;
	}
	if(context!=NULL)
	{
		ladybugDestroyContext( &context);
		context=NULL;
	}
	cur=-1;
	//GdiplusShutdown(gdiplusToken);
}



LadybugError PGR::operator()(string file_name)
{	

	LadybugError error;
	char pszTempConfigName[MAX_PATH];

	error = ladybugCreateContext( &context);
	if( error != LADYBUG_OK ) 
		return error;
	error = ladybugCreateStreamContext(&readContext);
	if( error != LADYBUG_OK ) 
		return error;
	error = ladybugInitializeStreamForReading( readContext, file_name.c_str(),true);
	if( error != LADYBUG_OK ) 
		return error;
	strcpy_s( pszTempConfigName, "temp.cal");
	error = ladybugGetStreamConfigFile( readContext , pszTempConfigName );
	if( error != LADYBUG_OK ) return error;
	error = ladybugLoadConfig( context, pszTempConfigName );
	if( error != LADYBUG_OK ) return error;

	error = ladybugGetStreamNumOfImages(readContext,&imgnums);
	if( error != LADYBUG_OK ) 
		return error;

	error = ladybugSetColorProcessingMethod(context, LADYBUG_NEAREST_NEIGHBOR_FAST);
	if( error != LADYBUG_OK ) return error;
	error = ladybugGetStreamHeader( readContext, &streamHeaderInfo );
	if( error != LADYBUG_OK ) return error;
//	error = ladybugSetColorTileFormat( context, streamHeaderInfo.stippledFormat );
//	if( error != LADYBUG_OK ) return error;
	
	error=ladybugGoToImage(readContext,0);
	if( error != LADYBUG_OK ) return error;

	error = ladybugReadImageFromStream( readContext, &image);
	if( error != LADYBUG_OK ) return error;

//	if ( COLOR_PROCESSING_METHOD == LADYBUG_DOWNSAMPLE4 || COLOR_PROCESSING_METHOD == LADYBUG_MONO)
//	{
//		iTextureWidth = image.uiCols / 2;
//		iTextureHeight = image.uiRows / 2;
//	}
//	else 
//	{
		iTextureWidth = image.uiCols;
		iTextureHeight = image.uiRows;
//	}
//	for( int i = 0; i < LADYBUG_NUM_CAMERAS; i++) 
//	{
//		arpTextureBuffers[ i ] = new unsigned char[ iTextureWidth * iTextureHeight * 4  ];
//	}

	error = ladybugInitializeAlphaMasks( context, iTextureWidth, iTextureHeight );
	if( error != LADYBUG_OK ) return error;
	error = ladybugSetAlphaMasking( context, true );
	if( error != LADYBUG_OK ) return error;

	//error = ladybugEnableSoftwareRendering( context, true );
	//if( error != LADYBUG_OK ) return error;

	error = ladybugConfigureOutputImages( context, LADYBUG_PANORAMIC );
	if( error != LADYBUG_OK ) return error;
	error = ladybugSetOffScreenImageSize( context, LADYBUG_PANORAMIC, PANORAMIC_IMAGE_COLUMNS, PANORAMIC_IMAGE_ROWS );  
	if( error != LADYBUG_OK ) return error;

	(*this)(0);
	return LADYBUG_OK;
}

LadybugError PGR::operator()(int num)
{
	if(num<0||num==cur)
	{
		//return LADYBUG_FAILED;
	}

	LadybugError error;

	error=ladybugGoToImage( readContext, num);
	if( error != LADYBUG_OK ) return error;
	error = ladybugReadImageFromStream( readContext, &image);
	if( error != LADYBUG_OK ) return error;
	error = ladybugConvertImage( context, &image, NULL );
	if( error != LADYBUG_OK ) return error;
	error = ladybugUpdateTextures( context, LADYBUG_NUM_CAMERAS, NULL);
	if( error != LADYBUG_OK ) return error;
	//error = ladybugSetOffScreenImageSize( context, LADYBUG_PANORAMIC, PANORAMIC_IMAGE_COLUMNS,PANORAMIC_IMAGE_ROWS); //只需要第一次设置
	//if( error != LADYBUG_OK ) return error;
	error = ladybugRenderOffScreenImage( context,LADYBUG_PANORAMIC,LADYBUG_BGR,&processedImage);
	if (error != LADYBUG_OK) return error;
#if isRectifyNeeded
	error = ladybugSetOffScreenImageSize(
		context,
		LADYBUG_ALL_RECTIFIED_IMAGES,
		RECTIFIED_IMAGE_COLUMNS,
		RECTIFIED_IMAGE_ROWS);
	for (int iCamera = 0; iCamera < LADYBUG_NUM_CAMERAS; iCamera++)
	{
		switch (iCamera)
		{
		case 0:
			error = ladybugRenderOffScreenImage(context, LADYBUG_RECTIFIED_CAM0, LADYBUG_BGR, &RectifiedImages[iCamera]);
			break;
		case 1:
			error = ladybugRenderOffScreenImage(context, LADYBUG_RECTIFIED_CAM1, LADYBUG_BGR, &RectifiedImages[iCamera]);
			break;
		case 2:
			error = ladybugRenderOffScreenImage(context, LADYBUG_RECTIFIED_CAM2, LADYBUG_BGR, &RectifiedImages[iCamera]);
			break;
		case 3:
			error = ladybugRenderOffScreenImage(context, LADYBUG_RECTIFIED_CAM3, LADYBUG_BGR, &RectifiedImages[iCamera]);
			break;
		case 4:
			error = ladybugRenderOffScreenImage(context, LADYBUG_RECTIFIED_CAM4, LADYBUG_BGR, &RectifiedImages[iCamera]);
			break;
		case 5:
			error = ladybugRenderOffScreenImage(context, LADYBUG_RECTIFIED_CAM5, LADYBUG_BGR, &RectifiedImages[iCamera]);
			break;
		}
	}
#endif
	cur=num;
	return LADYBUG_OK;
}

void PGR::operator()(unsigned int columns, unsigned int rows)
{
	PANORAMIC_IMAGE_COLUMNS=columns;
	PANORAMIC_IMAGE_ROWS=rows;
	return;
}


LadybugError PGR::outputBMP(string file_name, int num)
{
	if(num<0)
	{
		num=cur;
	}
	if(cur==num)
	{
		return ladybugSaveImage( context, &processedImage, file_name.c_str(), LADYBUG_FILEFORMAT_BMP );
	}
	else
	{
		(*this)(num);
		return ladybugSaveImage( context, &processedImage, file_name.c_str(), LADYBUG_FILEFORMAT_BMP );
	}
}

LadybugProcessedImage PGR::getProcessedImage()
{
	return processedImage;
}
#if isRectifyNeeded
LadybugProcessedImage PGR::getRectifiedImage(int iCamera)
{
	return RectifiedImages[iCamera];
}

LadybugError PGR::XZY2RC(double dLadybugX, double dLadybugY, double dLadybugZ, unsigned int  uiCamera,
	double *  pdRectifiedRow, double *  pdRectifiedCol)
{
	double row, col;
	LadybugError rtn = ladybugXYZtoRC(context, dLadybugX, dLadybugY, dLadybugZ, uiCamera, &row, &col, NULL);
	(*pdRectifiedRow) = row;
	(*pdRectifiedCol) = col;
	return rtn;
}
#endif
LadybugImage PGR::getImage()
{
	return image;
}



long int PGR::getulMicroSeconds()
{
	
	return image.timeStamp.ulMicroSeconds;

}

time_t PGR::getulSeconds()
{

	return image.timeStamp.ulSeconds;
}

int PGR::getulNumberofImages()
{
	return imgnums;
}


unsigned int PGR::cols()
{
	return processedImage.uiCols;
}

unsigned int PGR::rows()
{
	return processedImage.uiRows;
}
#if isRectifyNeeded
unsigned int PGR::rectifiedCols()
{
	return RECTIFIED_IMAGE_COLUMNS;
}

unsigned int PGR::rectifiedRows()
{
	return RECTIFIED_IMAGE_ROWS;
}
#endif
int PGR::getCur()
{
    return cur;
}

int PGR::getImageNum()
{
    return this->imgnums;

}

LadybugError PGR::saveRectifiedImg(const char* filename,LadybugProcessedImage* img, LadybugSaveFileFormat format)
{
    return ladybugSaveImage(context, img, filename, format);

}
