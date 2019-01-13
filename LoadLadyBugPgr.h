#ifndef PGR_H
#define PGR_H

#include <Windows.h>
#include <ladybug.h>
#include <ladybugrenderer.h>
#include <ladybuggeom.h>
#include <ladybugstream.h>

#include <thread>
#include <opencv2/core/core.hpp>

#include <string>
using std::string;

#define ResizeLadybugRadio 1
#define isRectifyNeeded 1

class PGR{
private:
	LadybugContext context;
	LadybugStreamContext readContext;
	LadybugStreamHeadInfo streamHeaderInfo;
	unsigned int iTextureWidth, iTextureHeight;
	LadybugImage image;
	unsigned int PANORAMIC_IMAGE_COLUMNS;
	unsigned int PANORAMIC_IMAGE_ROWS;
	//GdiplusStartupInput gdiplusStartupInput;
	//ULONG_PTR gdiplusToken;

	LadybugProcessedImage processedImage;
	int cur;
	unsigned int imgnums;

#if isRectifyNeeded
	unsigned int RECTIFIED_IMAGE_COLUMNS;
	unsigned int RECTIFIED_IMAGE_ROWS;
	LadybugProcessedImage RectifiedImages[LADYBUG_NUM_CAMERAS];
#endif
public:
	PGR();
	~PGR();
	
	LadybugError operator()(string file_name);		//����PGR���ݣ�����ʼ��Ϊ��0֡
	LadybugError operator()(int num);	//ָ����num֡
	LadybugError operator()(int num, cv::Mat& cvImg);	//ָ����num֡
	void operator()(unsigned int columns, unsigned int rows);
	int	getCur();
    int getImageNum();
    LadybugError saveRectifiedImg(const char *filename, LadybugProcessedImage *img, LadybugSaveFileFormat format);

	LadybugError outputBMP(string file_name, int num);
	unsigned int cols();	//��ȡͼ����
	unsigned int rows();	//��ȡͼ��߶�

	LadybugProcessedImage getProcessedImage();
	LadybugImage getImage();

	long int getulMicroSeconds();
	time_t getulSeconds();

	int getulNumberofImages();
#if isRectifyNeeded
	unsigned int rectifiedCols();	//��ȡͼ����
	unsigned int rectifiedRows();	//��ȡͼ��߶�
	LadybugProcessedImage getRectifiedImage(int iCamera);
	LadybugError XZY2RC(double dLadybugX, double dLadybugY, double dLadybugZ, unsigned int  uiCamera,
		double *  pdRectifiedRow, double *  pdRectifiedCol);
#endif
};

#endif
