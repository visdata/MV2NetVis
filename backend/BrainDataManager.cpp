#include "BrainDataManager.h"
#include "StreamlineCharacteristics.h"
#include <mat.h>
#include <definition.h>
#include <stack>
#include <iostream>
#include <algorithm>
#include "StreamlineDistance.h"
#include <fstream>
#include <direct.h>
#include <io.h>
#include "include/json/json.h"
#include <fstream>

//#define MIN_NUM 15
//#define MIN_DISTANCE 1.0f 
#define THRESHOLD 5
#define L 100

using namespace alglib;

extern "C" void compute3dBinsHost(vec3f* velos, short* bins, const int& numPoint, const int& numMagBin, const int& numSphereBin);
extern "C" void compute3dBinsHostDensity(vec3f* velos, int* bins, const int& numPoint, const int& numX, const int& numY, const int& numZ);
extern "C" void compute3dPixelBinsHostDensity(vec3f* velos, int* bins, const int& numPoint, const int& numX, const int& numY, const int& numZ, float* matrixmp);
//extern "C" vec3f* gaussianSmooth3D_h(cudaArray* vec_field_d, const vec3i& dim, const float& delta, const int& num_sample, const float& step_size);

//矩阵相乘 ret = project * modelview 
void computeMatrix2(float* ret, float modelview[16], float project[16]) {
	memset(ret, 0, sizeof(float) * 16);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				//note that modelview and projection matrix are column-majored
				ret[i * 4 + k] += project[j * 4 + i] * modelview[k * 4 + j];
			}
		}
	}
}

BrainDataManager::BrainDataManager(const int file_num, int norm_fibers, const int minNum, const int minDistance)
{
	getVolumeMat("D:\\Jun\\workspace\\brainfiber\\braindata\\smallDatasets\\smallDatasets\\subjects\\003_S_4081_1-stripped.nii\\matlab.mat");
	mminNum = minNum;
	mminDistance = minDistance;
	//readAllPath();
	readROIPathAndFindCorrPointPath(); //生成文件数组
	mROI = new ROI();
	std::string pP = PointPath.at(2 * file_num + norm_fibers - 1); //norm_fiber取值1/2，点数据由两个文件合成
	std::string rP = ROIPath.at(file_num);
	//peopleId = pP.substr(58, 10); //template文件夹
	peopleId = pP.substr(50, 10); //FA
	getMapMat(); //ROI映射矩阵
	//生成mStlArray_h
	getPathMat(pP.c_str(), "ptnum", norm_fibers); //注意先后顺序要先获得线数组 每条streamline的point的数量
	//ROIids，计算mStlArray_h的ROI_num，beginROI,endROI（此处计算的beginROI,endROI非0，与下面二选一
	getROIMat(rP.c_str(), norm_fibers); //每个点的ROI
	//Points，计算mStlArray_h的length,torsion,curvature,beginROI,endROI（这边计算的beginROI,endROI可为0
	if (norm_fibers == 1) {
		getPointMat(pP.c_str(), "norm_fibers1"); //点数据
	}
	else {
		getPointMat(pP.c_str(), "norm_fibers2");
	}
	//计算mStlArray_h的entropy
	entropyForStreamline(); 
	//updateColorvelos(); //ROI颜色
	updateColorvelosByROIs();
	//writeJson();
	connectiveInROIs();
}

//根据peopleId数组,开始ROI，结束ROI，读取该组人该ROI的所有数据
BrainDataManager::BrainDataManager(const int* peopleIds, const int len,const int beginROI, const int endROI)
{
	//mminNum = minNum;
	//mminDistance = minDistance;
	//readAllPath();
	readROIPathAndFindCorrPointPath();
	//获得ROI映射矩阵
	getMapMat();
	//mROI = new ROI();
	//初始化点数目和线数目
	mNumPoint = 0;
	mNumStl = 0;
	old_mNumPoint = 0;
	old_mNumStl = 0;
	//对于该组每个人的数据
	for (int i = 0; i < len; i++) {
		//ROI数据文件名
		std::string rP = ROIPath.at(peopleIds[i]);
		peopleId = rP.substr(69, 10);
		//获得线数组，即每条streamline的点数量
		getPathMat(rP.c_str(), "ptnum");
		//获取每个点的ROI数据,以及streamline的beginROI和endROI,计算符合条件的streamline数目,
		getROIMat(rP.c_str(), beginROI, endROI);
		//获取每个点的点数据（FA等
		//std::string Faname = "D:\\Jun\\workspace\\brainfiber\\braindata\\labeled_tract_mci\\labeled_tract_mci\\007_S_2394_1_labeled_fibers.mat";
		std::string Faname = "E:\\bradindata\\fmri_fibers_p1\\" + peopleId + "_1_fibers_fa.mat"; 
		getFADataMat(Faname.c_str(), beginROI, endROI);
		Faname = "E:\\bradindata\\fmri_fibers_p1\\" + peopleId + "_1_fibers_md.mat";
		getMDDataMat(Faname.c_str(), beginROI, endROI);
		Faname = "E:\\bradindata\\fmri_fibers_p1\\" + peopleId + "_1_fibers_rd.mat";
		getRDDataMat(Faname.c_str(), beginROI, endROI);
		Faname = "E:\\bradindata\\fmri_fibers_p1\\" + peopleId + "_1_fibers_axd.mat";
		getAXDDataMat(Faname.c_str(), beginROI, endROI);
		if (old_mNumStl != mNumStl) {
			for (int j = 0; j < 2; j++) {
				//获取点数据文件名
				std::string pP = PointPath.at(2 * peopleIds[i] + j);
				//根据beginROI/endROI获得对应的streamline以及point
				if (j == 0) {
					getPointMat(pP.c_str(), "norm_fibers1", j, beginROI, endROI); //点数据
				}
				else {
					getPointMat(pP.c_str(), "norm_fibers2", j, beginROI, endROI);
				}
			}
		}
	}
	if (mNumStl > 0) {
		//计算mStlArray_h的entropy
		entropyForStreamline();
		//updateColorvelos(); //ROI颜色
		//updateColorvelosByROIs();
		//writeJson();
		//connectiveInROIs();
		//vf_h = vf_w = vf_d = 100;
		//calDensityData();
		findCentralLine();
		writeStreamlineBinary();
		writePointsBinary();
		writeCenterStreamlineBinary();
		writeCenterPointsBinary();
		writeFABinary();
		writeMDBinary();
		writeRDBinary();
		writeAXDBinary();
	}
	
}

BrainDataManager::BrainDataManager(const int peopleIdNum)
{
	readROIPathAndFindCorrPointPath();
	getMapMat();
	//mROI = new ROI();
	//初始化点数目和线数目
	mNumPoint = 0;
	mNumStl = 0;
	old_mNumPoint = 0;
	old_mNumStl = 0;
	centralLineNumPoint = 0;
	centralLineNumStl = 0;
	//对于该组每个人的数据
	//ROI数据文件名
	std::string rP = ROIPath.at(peopleIdNum);
	peopleId = rP.substr(69, 10);
	//获得线数组，即每条streamline的点数量
	getPathMat(rP.c_str(), "ptnum");
	//获得每个点的ROI数据
	getROIMat(rP.c_str());
	//获取点数据
	for (int j = 0; j < 2; j++) {
		//获取点数据文件名
		std::string pP = PointPath.at(2 * peopleIdNum + j);
		if (j == 0) {
			getPointMat(pP.c_str(), "norm_fibers1", j); //点数据
		}
		else {
			getPointMat(pP.c_str(), "norm_fibers2", j);
		}
	}
	//根据ROI获取相应streamline和points以及centralline
	for (int i = 15; i < 16; i++) { //(int i = 0; i < 71; i++)
		for (int j = 29; j < 30; j++) { //(int j = i; j < 71; j++)
			getPointMatByROI(i, j);
			if (mNumStl > 0) {
				//计算mStlArray_h的entropy
				entropyForStreamline();
				//updateColorvelos(); //ROI颜色
				//updateColorvelosByROIs();
				findCentralLine();
				writeStreamlineBinary();
				writePointsBinary();
				writeCenterStreamlineBinary();
				writeCenterPointsBinary();
			}
		}
	}
	readStreamlineBinary(15, 29);
	readPointsBinary(15, 29);
	readCenterStreamlineBinary(15, 29);
	readCenterPointsBinary(15, 29);
	
}

BrainDataManager::BrainDataManager(const int* peopleIds, const int len, const int beginROI, const int endROI, const int minNum, const int minDistance)
{
	mminNum = minNum;
	mminDistance = minDistance;
	//readAllPath();
	readROIPathAndFindCorrPointPath();
	//获得ROI映射矩阵
	getMapMat();
	//mROI = new ROI();
	//初始化点数目和线数目
	mNumPoint = 0;
	mNumStl = 0;
	centralLineNumPoint = 0;
	centralLineNumStl = 0;
	old_mNumPoint = 0;
	old_mNumStl = 0;
	//对于该组每个人的数据
	for (int i = 0; i < len; i++) {
		//ROI数据文件名
		std::string rP = ROIPath.at(peopleIds[i]);
		peopleId = rP.substr(69, 10);
		/*
		readStreamlineBinary(beginROI, endROI);
		readPointsBinary(beginROI, endROI);
		readCenterStreamlineBinary(beginROI, endROI);
		readCenterPointsBinary(beginROI, endROI);
		*/
		readMoreStreamlineBinary(beginROI, endROI);
		readMorePointsBinary(beginROI, endROI);
		readMoreCenterStreamlineBinary(beginROI, endROI);
		readMoreCenterPointsBinary(beginROI, endROI);

		readMoreFABinary(beginROI, endROI);
		readMoreMDBinary(beginROI, endROI);
		readMoreRDBinary(beginROI, endROI);
		readMoreAXDBinary(beginROI, endROI);
	}
	//中心化所有的点
	center.x = center.y = center.z = 0;
	if (mNumPoint > 0) {
		for (int i = 0; i < mNumPoint; i++) {
			center.x += Points[i].x;
			center.y += Points[i].y;
			center.z += Points[i].z;
		}
		center.x = center.x / mNumPoint;
		center.y = center.y / mNumPoint;
		center.z = center.z / mNumPoint;
		for (int i = 0; i < mNumPoint; i++) {
			Points[i].x = Points[i].x - center.x;
			Points[i].y = Points[i].y - center.y;
			Points[i].z = Points[i].z - center.z;
		}
		for (int i = 0; i < centralLineNumPoint; i++) {
			centralLinePoints[i].x = centralLinePoints[i].x - center.x;
			centralLinePoints[i].y = centralLinePoints[i].y - center.y;
			centralLinePoints[i].z = centralLinePoints[i].z - center.z;
		}
	}
	updateColorvelosByROIs();
	vf_h = vf_w = 256;
	vf_d = 48;
	//calDensityData();
	CTL_centralLineNumStl = 0;
	if(centralLineNumStl)
		findCentralLineInCtl();
}


BrainDataManager::~BrainDataManager()
{
	/*
	//用于画centralline的centralline
	if (mNumStl > 0) {
		//delete(points);
		free(Points);
		free(mStlArray_h);
		//free(veloArray);
		free(colorVelos);
		free(centralLinePoints);
		free(centralLineStlArray_h);
		free(CTL_centralLinePoints);
		free(CTL_centralLineStlArray_h);
		//free(tmp_Points);
		free(volume_h);
		free(pointCurvature);
		free(pointTorsion);
		free(pointLength);
		free(pointEntropy);
		//free(pointStrength);
		free(volumeCurvature);
		free(volumeEntropy);
		free(volumeTorsion);
		free(volumeLength);
		free(volumeStrength);
		free(FA);
		free(volumeFA);
		free(MD);
		free(volumeMD);
		free(RD);
		free(volumeRD);
		free(AXD);
		free(volumeAXD);
		
		for(int i = 0; i < vf_w * vf_h * vf_d; i++) {
			delete(alCurvature[i]);
		}
		
		free(alCurvature);
		//free(alEntropy);
		//free(alStrength);
		//free(alLength);
		//free(alTorsion);
		//free(alFA);
		//free(alMD);
		//free(alRD);
		//free(alAXD);
		
		free(alvolumeCurvature);
		free(alvolumeEntropy);
		free(alvolumeStrength);
		free(alvolumeLength);
		free(alvolumeTorsion);
		free(alvolumeFA);
		free(alvolumeMD);
		free(alvolumeRD);
		free(alvolumeAXD);

		free(stdvoluemCurvature);
		free(stdvoluemEntropy);
		free(stdvoluemStrength);
		free(stdvoluemLength);
		free(stdvoluemTorsion);
		free(stdvoluemFA);
		free(stdvoluemMD);
		free(stdvoluemRD);
		free(stdvoluemAXD);

		free(hist);
	}
	free(map);
	//free(ROIids);
	//free(tmp_mStlArray_h);
	PointPath.clear();
	ROIPath.clear();
	mStlCurvature.clear();
	mStlTorsion.clear();
	mStlEntropy.clear();
	*/
	
	//用于预处理
	if (mNumStl > 0) {
		//delete(points);
		free(Points);
		free(mStlArray_h);
		free(veloArray);
		//free(colorVelos);
		free(centralLinePoints);
		free(centralLineStlArray_h);
		//free(tmp_Points);
		//free(volume_h);
		free(FA);
		free(MD);
		free(RD);
		free(AXD);
	}
	free(map);
	free(ROIids);
	free(tmp_mStlArray_h);
	PointPath.clear();
	ROIPath.clear();
	mStlCurvature.clear();
	mStlTorsion.clear();
	mStlEntropy.clear();
	
	/*
	//用于生成json
	if (mNumStl > 0) {
		free(points);
		free(Points);
		free(mStlArray_h);
		free(veloArray);
		free(colorVelos);
		//free(centralLinePoints);
		//free(centralLineStlArray_h);
		//free(tmp_Points);
		free(volume_h);
	}
	free(map);
	free(ROIids);
	//free(tmp_mStlArray_h);
	PointPath.clear();
	ROIPath.clear();
	mStlCurvature.clear();
	mStlTorsion.clear();
	mStlEntropy.clear();
	*/
}

//计算mStlArray_h的length,torsion,curvature,beginROI,endROI
void BrainDataManager::getPointMat(const char* file_name, const char* table_name)
{
	MATFile* pmatFile1 = matOpen(file_name, "r");
	mxArray* pMxArray1 = matGetVariable(pmatFile1, table_name);
	double* initA;
	initA = (double*)mxGetData(pMxArray1);//按列、行、第3维的顺序存储/
	int D1 = mxGetNumberOfDimensions(pMxArray1); //维度/
	int point_M1 = mxGetM(pMxArray1); //M行
	int point_N1 = mxGetN(pMxArray1) / D1; //N列
	//给Points分配空间
	Points = (vec3f*)malloc(sizeof(vec3f) * mNumPoint);
	//给points分配空间
	points = (vec3f*)malloc(sizeof(vec3f) * point_M1 * point_N1);
	//points赋值
	for (int j = 0; j < point_N1; j++) {
		for (int i = 0; i < point_M1; i++) {
			points[i * point_N1 + j].x = (float)initA[point_M1 * j + i]; // - 130.3271f;//mat按列排序
			points[i * point_N1 + j].y = (float)initA[point_M1 * point_N1 + point_M1 * j + i]; // -105.3417f;//mat按列排序
			points[i * point_N1 + j].z = (float)initA[point_M1 * point_N1 * 2 + point_M1 * j + i]; // -20.4762f;//mat按列排序
		}
	}
	int tmp = 0;
	float curvature, torsion;
	for (int i = 0; i < point_M1; i++) {
		//计算纤维长度
		float length = 0;
		for (int j = 0; j < mStlArray_h[i].numPoint; j++) {
			Points[tmp].x = points[i * point_N1 + j].x;
			Points[tmp].y = points[i * point_N1 + j].y;
			Points[tmp].z = points[i * point_N1 + j].z;
			//length
			if (j != 0) {
				length += dist(Points[tmp], Points[tmp - 1]);
				if (dist(Points[tmp], Points[tmp - 1]) > 80.0f) {
					//std::cout << i << " ";
				}
			}
			int r = ROIids[tmp];
			if (r != 0) {
				//mROI->points[r - 1][] = Points[tmp];
			}
			//记录fiber开始的ROI, 结束的ROI，ROI开始结束可为0，设定阈值
			if (j == 0) {
				//查找THRESHOLD个内非0的
				int threshold = 0;
				while (ROIids[tmp + threshold] == 0 && threshold < THRESHOLD && threshold < mStlArray_h[i].numPoint) {
					threshold++;
				}
				if (threshold < THRESHOLD && threshold < mStlArray_h[i].numPoint) {
					mStlArray_h[i].beginROI = ROIids[tmp + threshold];
				}
				else {
					mStlArray_h[i].beginROI = ROIids[tmp + threshold - 1];
				}
				//mStlArray_h[i].beginROI = ROIids[tmp];
			}
			if (j == mStlArray_h[i].numPoint - 1) {
				//结束roi查找THRESHOLD个内非0的
				int threshold = 0;
				while (ROIids[tmp - threshold] == 0 && threshold < THRESHOLD && threshold < mStlArray_h[i].numPoint) {
					threshold++;
				}
				if (threshold < THRESHOLD && threshold < mStlArray_h[i].numPoint) {
					mStlArray_h[i].endROI = ROIids[tmp - threshold];
				}
				else {
					mStlArray_h[i].endROI = ROIids[tmp - threshold + 1];
				}
				//mStlArray_h[i].endROI = ROIids[tmp];
			}
			tmp++;
		}
		//给center赋值
		if (mStlArray_h[i].numPoint < point_N1) {
			center.x = points[i * point_N1 + mStlArray_h[i].numPoint].x;
			center.y = points[i * point_N1 + mStlArray_h[i].numPoint].y;
			center.z = points[i * point_N1 + mStlArray_h[i].numPoint].z;
		}
		mStlArray_h[i].length = length;
		averageDiscreteCurvatureTorsion(points + i * point_N1, mStlArray_h[i].numPoint, curvature, torsion);
		mStlCurvature.push_back(curvature);
		mStlTorsion.push_back(abs(torsion)); 
		mStlArray_h[i].curvature = curvature;
		mStlArray_h[i].torsion = abs(torsion);
	}
	//free((void*)points);
	matClose(pmatFile1);
	mxFree(initA);
}

void BrainDataManager::getPointMat(const char* file_name, const char* table_name, int type)
{
	MATFile* pmatFile1 = matOpen(file_name, "r");
	mxArray* pMxArray1 = matGetVariable(pmatFile1, table_name);
	double* initA;
	initA = (double*)mxGetData(pMxArray1);//按列、行、第3维的顺序存储/
	int D1 = mxGetNumberOfDimensions(pMxArray1); //维度/
	int point_M1 = mxGetM(pMxArray1); //M行
	int point_N1 = mxGetN(pMxArray1) / D1; //N列
	//给points分配空间
	vec3f* points = (vec3f*)malloc(sizeof(vec3f) * point_M1 * point_N1);
	//points赋值
	for (int j = 0; j < point_N1; j++) {
		for (int i = 0; i < point_M1; i++) {
			points[i * point_N1 + j].x = (float)initA[point_M1 * j + i]; // - 130.3271f;//mat按列排序
			points[i * point_N1 + j].y = (float)initA[point_M1 * point_N1 + point_M1 * j + i]; // -105.3417f;//mat按列排序
			points[i * point_N1 + j].z = (float)initA[point_M1 * point_N1 * 2 + point_M1 * j + i]; // -20.4762f;//mat按列排序
		}
	}	
	int pianyi = 0;
	if (type == 1) {
		pianyi = tmp_mNumStl / 2;
	}
	else {
		tmp_Points = (vec3f*)malloc(sizeof(vec3f) * tmp_mNumPoint);
	}
	//寻找center
	for (int i = 0; i < point_M1; i++) {
		if (tmp_mStlArray_h[i + pianyi].numPoint < point_N1) {
			center.x = points[i * point_N1 + tmp_mStlArray_h[i + pianyi].numPoint].x;
			center.y = points[i * point_N1 + tmp_mStlArray_h[i + pianyi].numPoint].y;
			center.z = points[i * point_N1 + tmp_mStlArray_h[i + pianyi].numPoint].z;
			break;
		}
	}
	int tmp = 0, tmp_stl = 0;
	float curvature, torsion;

	for (int i = 0; i < point_M1; i++) {
		float length = 0;
		for (int j = 0; j < tmp_mStlArray_h[i + pianyi].numPoint; j++) {
			tmp_Points[tmp + pianyi].x = points[i * point_N1 + j].x - center.x;
			tmp_Points[tmp + pianyi].y = points[i * point_N1 + j].y - center.y;
			tmp_Points[tmp + pianyi].z = points[i * point_N1 + j].z - center.z;
			//length
			if (j != 0) {
				length += dist(tmp_Points[tmp], tmp_Points[tmp - 1]);
			}
			tmp++;
		}
		tmp_mStlArray_h[i + pianyi].length = length;
		averageDiscreteCurvatureTorsion(points + i * point_N1, tmp_mStlArray_h[i + pianyi].numPoint, curvature, torsion);
		mStlCurvature.push_back(curvature);
		tmp_mStlArray_h[i + pianyi].curvature = curvature;
		tmp_mStlArray_h[i + pianyi].torsion = abs(torsion);
	}
	//free((void*)points);
	free(points);
	matClose(pmatFile1);
	mxFree(initA);
}

void BrainDataManager::getPointMat(const char* file_name, const char* table_name, int type, const int beginROI, const int endROI)
{
	MATFile* pmatFile1 = matOpen(file_name, "r");
	mxArray* pMxArray1 = matGetVariable(pmatFile1, table_name);
	double* initA;
	initA = (double*)mxGetData(pMxArray1);//按列、行、第3维的顺序存储/
	int D1 = mxGetNumberOfDimensions(pMxArray1); //维度/
	int point_M1 = mxGetM(pMxArray1); //M行
	int point_N1 = mxGetN(pMxArray1) / D1; //N列
	//复制以前的Points, mStl，并且给新的分配空间
	if (old_mNumStl) {
		vec3f* pointTmp = (vec3f*)malloc(sizeof(vec3f) * old_mNumPoint);
		for (int i = 0; i < old_mNumPoint; i++) {
			pointTmp[i].x = Points[i].x;
			pointTmp[i].y = Points[i].y;
			pointTmp[i].z = Points[i].z;
		}
		Streamline* mStlArrayTmp = (Streamline*)malloc(sizeof(Streamline) * old_mNumStl);
		for (int i = 0; i < old_mNumStl; i++) {
			mStlArrayTmp[i].beginROI = mStlArray_h[i].beginROI;
			mStlArrayTmp[i].endROI = mStlArray_h[i].endROI;
			mStlArrayTmp[i].curvature = mStlArray_h[i].curvature;
			mStlArrayTmp[i].entropy = mStlArray_h[i].entropy;
			mStlArrayTmp[i].length = mStlArray_h[i].length;
			mStlArrayTmp[i].numPoint = mStlArray_h[i].numPoint;
			mStlArrayTmp[i].ROI_num = mStlArray_h[i].ROI_num;
			mStlArrayTmp[i].sid = mStlArray_h[i].sid;
			mStlArrayTmp[i].start = mStlArray_h[i].start;
			mStlArrayTmp[i].torsion = mStlArray_h[i].torsion;
		}
		free(Points);
		free(mStlArray_h);
		mStlArray_h = (Streamline*)malloc(sizeof(Streamline) * mNumStl);
		Points = (vec3f*)malloc(sizeof(vec3f) * mNumPoint);
		for (int i = 0; i < old_mNumPoint; i++) {
			Points[i].x = pointTmp[i].x;
			Points[i].y = pointTmp[i].y;
			Points[i].z = pointTmp[i].z;
		}
		for (int i = 0; i < old_mNumStl; i++) {
			mStlArray_h[i].beginROI = mStlArrayTmp[i].beginROI;
			mStlArray_h[i].endROI = mStlArrayTmp[i].endROI;
			mStlArray_h[i].curvature = mStlArrayTmp[i].curvature;
			mStlArray_h[i].entropy = mStlArrayTmp[i].entropy;
			mStlArray_h[i].length = mStlArrayTmp[i].length;
			mStlArray_h[i].numPoint = mStlArrayTmp[i].numPoint;
			mStlArray_h[i].ROI_num = mStlArrayTmp[i].ROI_num;
			mStlArray_h[i].sid = mStlArrayTmp[i].sid;
			mStlArray_h[i].start = mStlArrayTmp[i].start;
			mStlArray_h[i].torsion = mStlArrayTmp[i].torsion;
		}
		free(pointTmp);
		free(mStlArrayTmp);
	}
	else {
		mStlArray_h = (Streamline*)malloc(sizeof(Streamline) * mNumStl);
		Points = (vec3f*)malloc(sizeof(vec3f) * mNumPoint);
	}
	//给points分配空间
	vec3f* points = (vec3f*)malloc(sizeof(vec3f) * point_M1 * point_N1);
	//points赋值
	for (int j = 0; j < point_N1; j++) {
		for (int i = 0; i < point_M1; i++) {
			points[i * point_N1 + j].x = (float)initA[point_M1 * j + i]; // - 130.3271f;//mat按列排序
			points[i * point_N1 + j].y = (float)initA[point_M1 * point_N1 + point_M1 * j + i]; // -105.3417f;//mat按列排序
			points[i * point_N1 + j].z = (float)initA[point_M1 * point_N1 * 2 + point_M1 * j + i]; // -20.4762f;//mat按列排序
		}
	}
	int pianyi = 0;
	if (type == 1) {
		pianyi = tmp_mNumStl / 2;
	}
	//寻找center
	for (int i = 0; i < point_M1; i++) {
		if (tmp_mStlArray_h[i + pianyi].numPoint < point_N1) {
			center.x = points[i * point_N1 + tmp_mStlArray_h[i + pianyi].numPoint].x;
			center.y = points[i * point_N1 + tmp_mStlArray_h[i + pianyi].numPoint].y;
			center.z = points[i * point_N1 + tmp_mStlArray_h[i + pianyi].numPoint].z;
			break;
		}
	}
	int tmp = 0, tmp_stl = 0;
	float curvature, torsion;

	for (int i = 0; i < point_M1; i++) {
		//假设是寻找的endROI\beginROI,将点中心化后放进Points，将streamline放进mStlArray_h
		if ((tmp_mStlArray_h[i + pianyi].beginROI == beginROI && tmp_mStlArray_h[i + pianyi].endROI == endROI) || (tmp_mStlArray_h[i + pianyi].beginROI == endROI && tmp_mStlArray_h[i + pianyi].endROI == beginROI)) {
			//计算纤维长度
			float length = 0;
			for (int j = 0; j < tmp_mStlArray_h[i + pianyi].numPoint; j++) {
				Points[tmp + old_mNumPoint].x = points[i * point_N1 + j].x - center.x;
				Points[tmp + old_mNumPoint].y = points[i * point_N1 + j].y - center.y;
				Points[tmp + old_mNumPoint].z = points[i * point_N1 + j].z - center.z;
				//length
				if (j != 0) {
					length += dist(Points[tmp], Points[tmp - 1]);
					if (dist(Points[tmp], Points[tmp - 1]) > 80.0f) {
						//std::cout << i << " ";
					}
				}
				tmp++;
			}
			tmp_mStlArray_h[i + pianyi].length = length;
			averageDiscreteCurvatureTorsion(points + i * point_N1, tmp_mStlArray_h[i + pianyi].numPoint, curvature, torsion);
			mStlCurvature.push_back(curvature);
			tmp_mStlArray_h[i + pianyi].curvature = curvature;
			tmp_mStlArray_h[i + pianyi].torsion = abs(torsion);
			mStlArray_h[tmp_stl + old_mNumStl].beginROI = tmp_mStlArray_h[i + pianyi].beginROI;
			mStlArray_h[tmp_stl + old_mNumStl].endROI = tmp_mStlArray_h[i + pianyi].endROI;
			mStlArray_h[tmp_stl + old_mNumStl].curvature = tmp_mStlArray_h[i + pianyi].curvature;
			mStlArray_h[tmp_stl + old_mNumStl].torsion = tmp_mStlArray_h[i + pianyi].torsion;
			mStlArray_h[tmp_stl + old_mNumStl].entropy = tmp_mStlArray_h[i + pianyi].entropy;
			mStlArray_h[tmp_stl + old_mNumStl].length = tmp_mStlArray_h[i + pianyi].length;
			mStlArray_h[tmp_stl + old_mNumStl].ROI_num = tmp_mStlArray_h[i + pianyi].ROI_num;
			mStlArray_h[tmp_stl + old_mNumStl].numPoint = tmp_mStlArray_h[i + pianyi].numPoint;
			if ((tmp_stl + old_mNumPoint) == 0) {
				mStlArray_h[tmp_stl + old_mNumStl].start = 0;
			}
			else {
				mStlArray_h[tmp_stl + old_mNumStl].start = mStlArray_h[tmp_stl + old_mNumStl - 1].start + mStlArray_h[tmp_stl + old_mNumStl - 1].numPoint;
			}
			mStlArray_h[tmp_stl + old_mNumStl].sid = old_mNumStl + tmp_stl;
			tmp_stl++;
		}
	}
	old_mNumStl += tmp_stl;
	old_mNumPoint += tmp;
	//free((void*)points);
	free(points);
	matClose(pmatFile1);
	mxFree(initA);
}

void BrainDataManager::getPointMatByROI(const int beginROI, const int endROI)
{
	if (mNumStl != 0) {
		free(Points);
		free(mStlArray_h);
	}
	Points = (vec3f*)malloc(sizeof(vec3f) * tmp_mNumPoint);
	mStlArray_h = (Streamline*)malloc(sizeof(Streamline) * tmp_mNumStl);
	old_mNumPoint = mNumPoint;
	mNumPoint = 0;
	mNumStl = 0;
	for (int i = 0; i < tmp_mNumStl; i++) {
		//假设是寻找的endROI\beginROI,将点中心化后放进Points，将streamline放进mStlArray_h
		if ((tmp_mStlArray_h[i].beginROI == beginROI && tmp_mStlArray_h[i].endROI == endROI) || (tmp_mStlArray_h[i].beginROI == endROI && tmp_mStlArray_h[i].endROI == beginROI)) {
			for (int j = 0; j < tmp_mStlArray_h[i].numPoint; j++) {
				Points[mNumPoint].x = tmp_Points[tmp_mStlArray_h[i].start + j].x;
				Points[mNumPoint].y = tmp_Points[tmp_mStlArray_h[i].start + j].y;
				Points[mNumPoint].z = tmp_Points[tmp_mStlArray_h[i].start + j].z;
				mNumPoint++;
			}
			
			mStlArray_h[mNumStl].beginROI = tmp_mStlArray_h[i].beginROI;
			mStlArray_h[mNumStl].endROI = tmp_mStlArray_h[i].endROI;
			mStlArray_h[mNumStl].curvature = tmp_mStlArray_h[i].curvature;
			mStlArray_h[mNumStl].torsion = tmp_mStlArray_h[i].torsion;
			mStlArray_h[mNumStl].entropy = tmp_mStlArray_h[i].entropy;
			mStlArray_h[mNumStl].length = tmp_mStlArray_h[i].length;
			mStlArray_h[mNumStl].ROI_num = tmp_mStlArray_h[i].ROI_num;
			mStlArray_h[mNumStl].numPoint = tmp_mStlArray_h[i].numPoint;
			if (mNumStl == 0) {
				mStlArray_h[mNumStl].start = 0;
			}
			else {
				mStlArray_h[mNumStl].start = mStlArray_h[mNumStl - 1].start + mStlArray_h[mNumStl - 1].numPoint;
			}
			mStlArray_h[mNumStl].sid = mNumStl;
			mNumStl++;
		}
	}
	if (mNumStl != 0) {
		Points = (vec3f*)realloc(Points, sizeof(vec3f) * mNumPoint);
		mStlArray_h = (Streamline*)realloc(mStlArray_h, sizeof(Streamline) * mNumStl); 
	}
	else {
		free(Points);
		free(mStlArray_h);
	}
}

void BrainDataManager::getPathMat(const char* file_name, const char* table_name)
{
	MATFile* pmatFile = matOpen(file_name, "r");
	mxArray* pMxArray = matGetVariable(pmatFile, table_name);
	double* initA = (double*)mxGetData(pMxArray); //注意各向异性的是double类型，template的是int类型
	//INT64* initA = (INT64*)mxGetData(pMxArray);
	tmp_mNumStl = mxGetM(pMxArray);
	//mNumStl = mxGetM(pMxArray); 
	int N = mxGetN(pMxArray); //N == 1
	//if (tmp_mStlArray_h != NULL) free(tmp_mStlArray_h);
	tmp_mStlArray_h = (Streamline*)malloc(sizeof(Streamline) * tmp_mNumStl);
	for (int i = 0; i < tmp_mNumStl; i++) {
		if (i == 0) {
			(tmp_mStlArray_h + i)->sid = i;
			(tmp_mStlArray_h + i)->start = 0;
			(tmp_mStlArray_h + i)->numPoint = (int)(initA[i] + 0.5f); //防止double转为int时发生错误
			//(stl + i)->numPoint = (int)(initA[i + mNumStl] + 0.5f); //2
		}
		else {
			(tmp_mStlArray_h + i)->sid = i;
			(tmp_mStlArray_h + i)->start = tmp_mStlArray_h[i - 1].start + tmp_mStlArray_h[i - 1].numPoint;
			(tmp_mStlArray_h + i)->numPoint = (int)(initA[i] + 0.5f); //防止double转为int时发生错误
			//(stl + i)->numPoint = (int)(initA[i + mNumStl] + 0.5f);//2
		}
	}
	tmp_mNumPoint = (tmp_mStlArray_h + tmp_mNumStl - 1)->start + (tmp_mStlArray_h + tmp_mNumStl - 1)->numPoint;//点数目
	matClose(pmatFile);
	mxFree(initA);
}

void BrainDataManager::getPathMat(const char* file_name, const char* table_name, int type)
{
	MATFile* pmatFile = matOpen(file_name, "r");
	mxArray* pMxArray = matGetVariable(pmatFile, table_name);
	double* initA = (double*)mxGetData(pMxArray); //注意各向异性的是double类型，template的是int类型
	//INT64* initA = (INT64*)mxGetData(pMxArray);
	mNumStl = mxGetM(pMxArray) / 2;
	//mNumStl = mxGetM(pMxArray); 
	int N = mxGetN(pMxArray); //N == 1
	mStlArray_h = (Streamline*)malloc(sizeof(Streamline) * mNumStl);
	int pianyi = 0;
	if (type == 2) {
		pianyi = mNumStl;
	}
	for (int i = 0; i < mNumStl; i++) {
		if (i == 0) {
			(mStlArray_h + i)->sid = i;
			(mStlArray_h + i)->start = 0;
			(mStlArray_h + i)->numPoint = (int)(initA[i + pianyi] + 0.5f); //防止double转为int时发生错误
			//(stl + i)->numPoint = (int)(initA[i + mNumStl] + 0.5f); //2
		}
		else {
			(mStlArray_h + i)->sid = i;
			(mStlArray_h + i)->start = mStlArray_h[i - 1].start + mStlArray_h[i - 1].numPoint;
			(mStlArray_h + i)->numPoint = (int)(initA[i + pianyi] + 0.5f); //防止double转为int时发生错误
			//(stl + i)->numPoint = (int)(initA[i + mNumStl] + 0.5f);//2
		}
	}
	mNumPoint = (mStlArray_h + mNumStl - 1)->start + (mStlArray_h + mNumStl - 1)->numPoint;//点数目
	matClose(pmatFile);
	mxFree(initA);
}

//获取ROI并且计算streaamline的ROI
void BrainDataManager::getROIMat(const char* file_name)
{
	MATFile* pmatFile = matOpen(file_name, "r");
	mxArray* pMxArray = matGetVariable(pmatFile, "fiber_ROI");
	double* initA = (double*)mxGetData(pMxArray);
	int point_M = mxGetM(pMxArray); //M行
	int point_N = mxGetN(pMxArray); //N列
	//给ROIids分配空间
	//if (ROIids != NULL) delete(ROIids);
	ROIids = (int*)malloc(sizeof(int) * tmp_mNumPoint);
	int* id = (int*)malloc(sizeof(int) * point_M * point_N);
	for (int j = 0; j < point_N; j++) {
		for (int i = 0; i < point_M; i++) {
			id[i * point_N + j] = map[(int)(initA[point_M * j + i] + 0.5)];//mat按列排序
		}
	}
	int tmp = 0;
	for (int i = 0; i < tmp_mNumStl; i++) {
		for (int j = 0; j < tmp_mStlArray_h[i].numPoint; j++) {
			ROIids[tmp] = id[i * point_N + j];
			tmp++;
		}
	}
	tmp = 0;
	int change = 0;
	for (int i = 0; i < tmp_mNumStl; i++) {
		for (int j = 0; j < tmp_mStlArray_h[i].numPoint; j++) {
			int r = ROIids[tmp];
			//记录fiber开始的ROI, 结束的ROI，ROI开始结束可为0，设定阈值
			if (j == 0) {
				//查找THRESHOLD个内非0的
				int threshold = 0;
				while (ROIids[tmp + threshold] == 0 && threshold < THRESHOLD && threshold < tmp_mStlArray_h[i].numPoint) {
					threshold++;
				}
				if (threshold < THRESHOLD && threshold < tmp_mStlArray_h[i].numPoint) {
					tmp_mStlArray_h[i].beginROI = ROIids[tmp + threshold];
				}
				else {
					tmp_mStlArray_h[i].beginROI = ROIids[tmp + threshold - 1];
				}
			}
			if (j == tmp_mStlArray_h[i].numPoint - 1) {
				//结束roi查找THRESHOLD个内非0的
				int threshold = 0;
				while (ROIids[tmp - threshold] == 0 && threshold < THRESHOLD && threshold < tmp_mStlArray_h[i].numPoint) {
					threshold++;
				}
				if (threshold < THRESHOLD && threshold < tmp_mStlArray_h[i].numPoint) {
					tmp_mStlArray_h[i].endROI = ROIids[tmp - threshold];
				}
				else {
					tmp_mStlArray_h[i].endROI = ROIids[tmp - threshold + 1];
				}
			}
			tmp++;
		}
	}
	free(id);
	matClose(pmatFile);
	mxFree(initA);
}

void BrainDataManager::getROIMat(const char* file_name, int type)
{
	MATFile* pmatFile = matOpen(file_name, "r");
	mxArray* pMxArray = matGetVariable(pmatFile, "fiber_ROI");
	double* initA = (double*)mxGetData(pMxArray);
	int point_M = mxGetM(pMxArray); //M行
	int point_N = mxGetN(pMxArray); //N列
	//给ROIids分配空间
	ROIids = (int*)malloc(sizeof(int) * mNumPoint);
	int* id = (int*)malloc(sizeof(int) * point_M * point_N);
	for (int j = 0; j < point_N; j++) {
		for (int i = 0; i < point_M; i++) {
			id[i * point_N + j] = map[(int)(initA[point_M * j + i] + 0.5)];//mat按列排序
		}
	}
	int pianyi = 0;
	if (type == 2) {
		pianyi = mNumStl;
	}
	int tmp = 0;
	for (int i = 0; i < mNumStl; i++) {
		//计算ROI个数,初始化
		//ROIs：在一条fiber中，每个ROI有多少个点
		int ROIs[ROI_NUM] = { 0 };
		std::stack<int> calBeginAndEndROI;
		for (int j = 0; j < ROI_NUM; j++) {
			ROIs[j] = 0;
		}
		for (int j = 0; j < mStlArray_h[i].numPoint; j++) {
			ROIids[tmp] = id[i * point_N + j + pianyi * point_N];
			int r = ROIids[tmp];
			if (r != 0) {
				ROIs[r - 1] += 1;
				calBeginAndEndROI.push(r);
			}
			tmp++;
		}
		//统计经过的ROI个数
		int total = 0;
		for (int j = 0; j < ROI_NUM; j++) {
			if (ROIs[j] != 0) {
				total++;
				//更新mROI中的数据
				mROI->increasePointNum(j, ROIs[j]);
				mROI->increasePathNum(j, 1);
			}
		}
		mStlArray_h[i].ROI_num = total;
		//cal beginROI and endROI, ROI开始结束都非0
		/*
		if (!calBeginAndEndROI.empty()) {
			mStlArray_h[i].endROI = calBeginAndEndROI.top();
			int bottom = calBeginAndEndROI.top();
			while (!calBeginAndEndROI.empty())
			{
				bottom = calBeginAndEndROI.top();
				calBeginAndEndROI.pop();
			}
			mStlArray_h[i].beginROI = bottom;
		}
		else {
			mStlArray_h[i].beginROI = 0;
			mStlArray_h[i].endROI = 0;
		}
		*/
	}
	//给ROI的列表分配内存
	/*
	int id_num = mROI->maxPathNum();
	int p_num = mROI->maxPointNum();
	for (int i = 0; i < ROI_NUM; i++) {
		mROI->path_id[i] = (int*)malloc(sizeof(int) * id_num);
		mROI->points[i] = (vec3f*)malloc(sizeof(vec3f) * p_num);
	}
	*/
	free(id);
	matClose(pmatFile);
	mxFree(initA);
}

void BrainDataManager::getROIMat(const char* file_name, const int beginROI, const int endROI)
{
	MATFile* pmatFile = matOpen(file_name, "r");
	mxArray* pMxArray = matGetVariable(pmatFile, "fiber_ROI");
	double* initA = (double*)mxGetData(pMxArray);
	int point_M = mxGetM(pMxArray); //M行
	int point_N = mxGetN(pMxArray); //N列
	//给ROIids分配空间
	//if (ROIids != NULL) delete(ROIids);
	ROIids = (int*)malloc(sizeof(int) * tmp_mNumPoint);
	int* id = (int*)malloc(sizeof(int) * point_M * point_N);
	for (int j = 0; j < point_N; j++) {
		for (int i = 0; i < point_M; i++) {
			id[i * point_N + j] = map[(int)(initA[point_M * j + i] + 0.5)];//mat按列排序
		}
	}
	int tmp = 0;
	for (int i = 0; i < tmp_mNumStl; i++) {
		for (int j = 0; j < tmp_mStlArray_h[i].numPoint; j++) {
			ROIids[tmp] = id[i * point_N + j];
			tmp++;
		}
	}
	tmp = 0;
	int change = 0;
	for (int i = 0; i < tmp_mNumStl; i++) {
		for (int j = 0; j < tmp_mStlArray_h[i].numPoint; j++) {
			int r = ROIids[tmp];
			//记录fiber开始的ROI, 结束的ROI，ROI开始结束可为0，设定阈值
			if (j == 0) {
				//查找THRESHOLD个内非0的
				int threshold = 0;
				while (ROIids[tmp + threshold] == 0 && threshold < THRESHOLD && threshold < tmp_mStlArray_h[i].numPoint) {
					threshold++;
				}
				if (threshold < THRESHOLD && threshold < tmp_mStlArray_h[i].numPoint) {
					tmp_mStlArray_h[i].beginROI = ROIids[tmp + threshold];
				}
				else {
					tmp_mStlArray_h[i].beginROI = ROIids[tmp + threshold - 1];
				}
			}
			if (j == tmp_mStlArray_h[i].numPoint - 1) {
				//结束roi查找THRESHOLD个内非0的
				int threshold = 0;
				while (ROIids[tmp - threshold] == 0 && threshold < THRESHOLD && threshold < tmp_mStlArray_h[i].numPoint) {
					threshold++;
				}
				if (threshold < THRESHOLD && threshold < tmp_mStlArray_h[i].numPoint) {
					tmp_mStlArray_h[i].endROI = ROIids[tmp - threshold];
				}
				else {
					tmp_mStlArray_h[i].endROI = ROIids[tmp - threshold + 1];
				}
			}
			tmp++;
		}
		//假设该streamline是要找的，添加线条数量和点数量
		if ((tmp_mStlArray_h[i].beginROI == beginROI && tmp_mStlArray_h[i].endROI == endROI) || (tmp_mStlArray_h[i].beginROI == endROI && tmp_mStlArray_h[i].endROI == beginROI)) {
			if (change == 0) {
				old_mNumStl = mNumStl;
				old_mNumPoint = mNumPoint;
				change = 1;
			}
			mNumStl++;
			mNumPoint += tmp_mStlArray_h[i].numPoint;
		}
	}
	free(id);
	matClose(pmatFile);
	mxFree(initA);
}

void BrainDataManager::getPointDataMat(const char* file_name, const int beginROI, const int endROI)
{
	MATFile* pmatFile = matOpen(file_name, "r");
	mxArray* pMxArray = matGetVariable(pmatFile, "fiber_fa");
	double* initA = (double*)mxGetData(pMxArray);
	int point_M = mxGetM(pMxArray); //M行
	int point_N = mxGetN(pMxArray); //N列

	//给FA分配空间
	FA = (float*)malloc(sizeof(float) * mNumPoint);
	double* id = (double*)malloc(sizeof(double) * point_M * point_N);
	for (int j = 0; j < point_N; j++) {
		for (int i = 0; i < point_M; i++) {
			id[i * point_N + j] = (double)(initA[point_M * j + i]);//mat按列排序
		}
	}
	int tmp = 0;
	for (int i = 0; i < tmp_mNumStl; i++) {
		if ((tmp_mStlArray_h[i].beginROI == beginROI && tmp_mStlArray_h[i].endROI == endROI)
			|| (tmp_mStlArray_h[i].beginROI == endROI && tmp_mStlArray_h[i].endROI == beginROI)) {
			for (int j = 0; j < tmp_mStlArray_h[i].numPoint; j++) {
				FA[tmp] = id[i * point_N + j];
				tmp++;
			}
		}
	}
	
	free(id);
	matClose(pmatFile);
	mxFree(initA);
}

void BrainDataManager::getFADataMat(const char* file_name, const int beginROI, const int endROI)
{
	MATFile* pmatFile = matOpen(file_name, "r");
	mxArray* pMxArray = matGetVariable(pmatFile, "fiber_fa");
	double* initA = (double*)mxGetData(pMxArray);
	int point_M = mxGetM(pMxArray); //M行
	int point_N = mxGetN(pMxArray); //N列

	//给FA分配空间
	FA = (float*)malloc(sizeof(float) * mNumPoint);
	double* id = (double*)malloc(sizeof(double) * point_M * point_N);
	for (int j = 0; j < point_N; j++) {
		for (int i = 0; i < point_M; i++) {
			id[i * point_N + j] = (double)(initA[point_M * j + i]);//mat按列排序
		}
	}
	int tmp = 0;
	for (int i = 0; i < tmp_mNumStl; i++) {
		if ((tmp_mStlArray_h[i].beginROI == beginROI && tmp_mStlArray_h[i].endROI == endROI)
			|| (tmp_mStlArray_h[i].beginROI == endROI && tmp_mStlArray_h[i].endROI == beginROI)) {
			for (int j = 0; j < tmp_mStlArray_h[i].numPoint; j++) {
				FA[tmp] = id[i * point_N + j];
				tmp++;
			}
		}
	}

	free(id);
	matClose(pmatFile);
	mxFree(initA);
}

void BrainDataManager::getMDDataMat(const char* file_name, const int beginROI, const int endROI)
{
	MATFile* pmatFile = matOpen(file_name, "r");
	mxArray* pMxArray = matGetVariable(pmatFile, "fiber_fa");
	double* initA = (double*)mxGetData(pMxArray);
	int point_M = mxGetM(pMxArray); //M行
	int point_N = mxGetN(pMxArray); //N列

	//给MD分配空间
	MD = (float*)malloc(sizeof(float) * mNumPoint);
	double* id = (double*)malloc(sizeof(double) * point_M * point_N);
	for (int j = 0; j < point_N; j++) {
		for (int i = 0; i < point_M; i++) {
			id[i * point_N + j] = (double)(initA[point_M * j + i]);//mat按列排序
		}
	}
	int tmp = 0;
	for (int i = 0; i < tmp_mNumStl; i++) {
		if ((tmp_mStlArray_h[i].beginROI == beginROI && tmp_mStlArray_h[i].endROI == endROI)
			|| (tmp_mStlArray_h[i].beginROI == endROI && tmp_mStlArray_h[i].endROI == beginROI)) {
			for (int j = 0; j < tmp_mStlArray_h[i].numPoint; j++) {
				MD[tmp] = id[i * point_N + j];
				tmp++;
			}
		}
	}

	free(id);
	matClose(pmatFile);
	mxFree(initA);
}

void BrainDataManager::getRDDataMat(const char* file_name, const int beginROI, const int endROI)
{
	MATFile* pmatFile = matOpen(file_name, "r");
	mxArray* pMxArray = matGetVariable(pmatFile, "fiber_fa");
	double* initA = (double*)mxGetData(pMxArray);
	int point_M = mxGetM(pMxArray); //M行
	int point_N = mxGetN(pMxArray); //N列

	//给RD分配空间
	RD = (float*)malloc(sizeof(float) * mNumPoint);
	double* id = (double*)malloc(sizeof(double) * point_M * point_N);
	for (int j = 0; j < point_N; j++) {
		for (int i = 0; i < point_M; i++) {
			id[i * point_N + j] = (double)(initA[point_M * j + i]);//mat按列排序
		}
	}
	int tmp = 0;
	for (int i = 0; i < tmp_mNumStl; i++) {
		if ((tmp_mStlArray_h[i].beginROI == beginROI && tmp_mStlArray_h[i].endROI == endROI)
			|| (tmp_mStlArray_h[i].beginROI == endROI && tmp_mStlArray_h[i].endROI == beginROI)) {
			for (int j = 0; j < tmp_mStlArray_h[i].numPoint; j++) {
				RD[tmp] = id[i * point_N + j];
				tmp++;
			}
		}
	}

	free(id);
	matClose(pmatFile);
	mxFree(initA);
}

void BrainDataManager::getAXDDataMat(const char* file_name, const int beginROI, const int endROI)
{
	MATFile* pmatFile = matOpen(file_name, "r");
	mxArray* pMxArray = matGetVariable(pmatFile, "fiber_fa");
	double* initA = (double*)mxGetData(pMxArray);
	int point_M = mxGetM(pMxArray); //M行
	int point_N = mxGetN(pMxArray); //N列

	//给AXD分配空间
	AXD = (float*)malloc(sizeof(float) * mNumPoint);
	double* id = (double*)malloc(sizeof(double) * point_M * point_N);
	for (int j = 0; j < point_N; j++) {
		for (int i = 0; i < point_M; i++) {
			id[i * point_N + j] = (double)(initA[point_M * j + i]);//mat按列排序
		}
	}
	int tmp = 0;
	for (int i = 0; i < tmp_mNumStl; i++) {
		if ((tmp_mStlArray_h[i].beginROI == beginROI && tmp_mStlArray_h[i].endROI == endROI)
			|| (tmp_mStlArray_h[i].beginROI == endROI && tmp_mStlArray_h[i].endROI == beginROI)) {
			for (int j = 0; j < tmp_mStlArray_h[i].numPoint; j++) {
				AXD[tmp] = id[i * point_N + j];
				tmp++;
			}
		}
	}

	free(id);
	matClose(pmatFile);
	mxFree(initA);
}

void BrainDataManager::getMapMat()
{
	MATFile* pmatFile = matOpen("D:\\Jun\\workspace\\brainfiber\\braindata\\map.mat", "r");
	mxArray* pMxArray = matGetVariable(pmatFile, "index");
	double* initA = (double*)mxGetData(pMxArray); //2035
	int point_M = mxGetM(pMxArray) + 1; //M行, 2036
	int point_N = mxGetN(pMxArray); //N列, 1
	//给map分配空间
	map = (int*)malloc(sizeof(int) * point_M);
	for (int i = 1; i < point_M; i++) {
		map[i] = (int)(initA[i - 1] + 0.5);//mat按列排序			
	}
	map[0] = 0;
	matClose(pmatFile);
	mxFree(initA);
}


void BrainDataManager::getVolumeMat(const char* file_name)
{
	MATFile* pmatFile1 = matOpen(file_name, "r");
	mxArray* pMxArray1 = matGetVariable(pmatFile1, "img");
	UINT16* initA = (UINT16*)mxGetData(pMxArray1);//按列、行、第3维的顺序存储/
	vf_h = mxGetM(pMxArray1); //M行 196
	vf_w = sqrt(mxGetN(pMxArray1)); //N列 256
	vf_d = vf_w; //256
	//给volume_h分配空间
	volume_h = (UINT*)malloc(sizeof(UINT) * vf_d * vf_h * vf_w);
	//赋值
	int tmp = 0;
	for (int i = 0; i < vf_d; i++) {
		for (int k = 0; k < vf_w; k++) {
			for (int j = 0; j < vf_h; j++) {
				volume_h[i * vf_h * vf_w + j * vf_w + k] = initA[i * vf_h * vf_w + k * vf_h + j];
				//volume_h[j * vf_d * vf_w + k * vf_d + i] = initA[i * vf_h * vf_w + k * vf_h + j];
				/*
				if (volume_h[tmp] != 0) {
					std::cout << volume_h[tmp] << " ";
				}
				tmp++;
				*/
			}
		}
	}
	/*
	//change深度变成列，列变为行，行变为深
	int dep = vf_d;
	vf_d = vf_h;
	vf_h = vf_w;
	vf_w = dep;
	*/
	matClose(pmatFile1);
	mxFree(initA);
}

float BrainDataManager::getStreamLineLength(int id)
{
	if (id < 0 || id >= mNumStl) return -1; //越界访问
	return mStlArray_h[id].length;
}

float BrainDataManager::getStreamLineCurvature(int id)
{
	if (id < 0 || id >= mNumStl) return -1; //越界访问
	return mStlCurvature[id];
}

float BrainDataManager::getStreamLineTorsion(int id)
{
	if (id < 0 || id >= mNumStl) return -1; //越界访问
	return mStlTorsion[id];
}

float BrainDataManager::getStreamLineEntropy(int id)
{
	if (id < 0 || id >= mNumStl) return -1; //越界访问
	return mStlEntropy[id];
}

int BrainDataManager::getStreamLineROINum(int id)
{
	if (id < 0 || id >= mNumStl) return -1; //越界访问
	return mStlArray_h[id].ROI_num;
}

int BrainDataManager::getROISLNum(int id)
{
	return mROI->getPathNum(id);
}

std::vector<int> BrainDataManager::getStlIdByROI(int beginROI, int endROI)
{
	std::vector<int> result;
	for (int i = 0; i < mNumStl; i++) {
		if ((mStlArray_h[i].beginROI == beginROI && mStlArray_h[i].endROI == endROI) || (mStlArray_h[i].beginROI == endROI && mStlArray_h[i].endROI == beginROI)) {
			result.push_back(mStlArray_h[i].sid);
		}
	}
	return result;
}

std::vector<int> BrainDataManager::getPartOfStlIdByROI(int beginROI, int endROI, float rate)
{
	std::vector<int> all = getStlIdByROI(beginROI, endROI);
	std::vector<int> result;
	int num = int(all.size() * rate);
	//没有的时候输出all
	if (num == 0) {
		return all;
	}
	random_shuffle(all.begin(), all.end());
	for (int i = 0; i < num; i++) {
		result.push_back(all.at(i));
	}
	return result;
}
bool com(const std::vector<float>::iterator& a, const std::vector<float>::iterator& b)
{
	return *a > *b;
}
bool comp(const std::vector<float>::iterator& a, const std::vector<float>::iterator& b)
{
	return *a < *b;
}
std::vector<int> BrainDataManager::getPartOfStlIdByROIAndMaxLength(int beginROI, int endROI, float rate)
{
	std::vector<int> all = getStlIdByROI(beginROI, endROI);
	std::vector<float> length;
	for (int i = 0; i < all.size(); i++) {
		length.push_back(mStlArray_h[all[i]].length);
	}
	std::vector<int> tmp;
	int num = int(all.size() * rate);
	//没有的时候输出all
	if (num == 0) {
		return all;
	}
	std::vector<std::vector<float>::iterator >vi;
	for (std::vector<float>::iterator it = length.begin(); it != length.end(); it++)
		vi.push_back(it);
	sort(vi.begin(), vi.end(), com);
	for (std::vector<std::vector<float>::iterator >::iterator it = vi.begin(); it != vi.end(); it++) {
		tmp.push_back(all.at(*it - length.begin()));
	}
	std::vector<int> result;
	for (int i = 0; i < num; i++) {
		result.push_back(tmp.at(i));
	}
	return result;
}

std::vector<int> BrainDataManager::getPartOfStlIdByROIAndMinLength(int beginROI, int endROI, float rate)
{
	std::vector<int> all = getStlIdByROI(beginROI, endROI);
	std::vector<float> length;
	for (int i = 0; i < all.size(); i++) {
		length.push_back(mStlArray_h[all[i]].length);
	}
	std::vector<int> tmp;
	int num = int(all.size() * rate);
	//没有的时候输出all
	if (num == 0) {
		return all;
	}
	std::vector<std::vector<float>::iterator >vi;
	for (std::vector<float>::iterator it = length.begin(); it != length.end(); it++)
		vi.push_back(it);
	sort(vi.begin(), vi.end(), comp);
	for (std::vector<std::vector<float>::iterator >::iterator it = vi.begin(); it != vi.end(); it++) {
		tmp.push_back(all.at(*it - length.begin()));
	}
	std::vector<int> result;
	for (int i = 0; i < num; i++) {
		result.push_back(tmp.at(i));
	}
	return result;
}

std::vector<int> BrainDataManager::getPartOfStlIdByROIAndMaxCurvature(int beginROI, int endROI, float rate)
{
	std::vector<int> all = getStlIdByROI(beginROI, endROI);
	std::vector<float> length;
	for (int i = 0; i < all.size(); i++) {
		length.push_back(mStlArray_h[all[i]].curvature);
	}
	std::vector<int> tmp;
	int num = int(all.size() * rate);
	//没有的时候输出all
	if (num == 0) {
		return all;
	}
	std::vector<std::vector<float>::iterator >vi;
	for (std::vector<float>::iterator it = length.begin(); it != length.end(); it++)
		vi.push_back(it);
	sort(vi.begin(), vi.end(), com);
	for (std::vector<std::vector<float>::iterator >::iterator it = vi.begin(); it != vi.end(); it++) {
		tmp.push_back(all.at(*it - length.begin()));
	}
	std::vector<int> result;
	for (int i = 0; i < num; i++) {
		result.push_back(tmp.at(i));
	}
	return result;
}

std::vector<int> BrainDataManager::getPartOfStlIdByROIAndMinCurvature(int beginROI, int endROI, float rate)
{
	std::vector<int> all = getStlIdByROI(beginROI, endROI);
	std::vector<float> length;
	for (int i = 0; i < all.size(); i++) {
		length.push_back(mStlArray_h[all[i]].curvature);
	}
	std::vector<int> tmp;
	int num = int(all.size() * rate);
	//没有的时候输出all
	if (num == 0) {
		return all;
	}
	std::vector<std::vector<float>::iterator >vi;
	for (std::vector<float>::iterator it = length.begin(); it != length.end(); it++)
		vi.push_back(it);
	sort(vi.begin(), vi.end(), comp);
	for (std::vector<std::vector<float>::iterator >::iterator it = vi.begin(); it != vi.end(); it++) {
		tmp.push_back(all.at(*it - length.begin()));
	}
	std::vector<int> result;
	for (int i = 0; i < num; i++) {
		result.push_back(tmp.at(i));
	}
	return result;
}

std::vector<int> BrainDataManager::getPartOfStlIdByROIAndMaxTorsion(int beginROI, int endROI, float rate)
{
	std::vector<int> all = getStlIdByROI(beginROI, endROI);
	std::vector<float> length;
	for (int i = 0; i < all.size(); i++) {
		length.push_back(mStlArray_h[all[i]].torsion);
	}
	std::vector<int> tmp;
	int num = int(all.size() * rate);
	//没有的时候输出all
	if (num == 0) {
		return all;
	}
	std::vector<std::vector<float>::iterator >vi;
	for (std::vector<float>::iterator it = length.begin(); it != length.end(); it++)
		vi.push_back(it);
	sort(vi.begin(), vi.end(), com);
	for (std::vector<std::vector<float>::iterator >::iterator it = vi.begin(); it != vi.end(); it++) {
		tmp.push_back(all.at(*it - length.begin()));
	}
	std::vector<int> result;
	for (int i = 0; i < num; i++) {
		result.push_back(tmp.at(i));
	}
	return result;
}

std::vector<int> BrainDataManager::getPartOfStlIdByROIAndMinTorsion(int beginROI, int endROI, float rate)
{
	std::vector<int> all = getStlIdByROI(beginROI, endROI);
	std::vector<float> length;
	for (int i = 0; i < all.size(); i++) {
		length.push_back(mStlArray_h[all[i]].torsion);
	}
	std::vector<int> tmp;
	int num = int(all.size() * rate);
	//没有的时候输出all
	if (num == 0) {
		return all;
	}
	std::vector<std::vector<float>::iterator >vi;
	for (std::vector<float>::iterator it = length.begin(); it != length.end(); it++)
		vi.push_back(it);
	sort(vi.begin(), vi.end(), comp);
	for (std::vector<std::vector<float>::iterator >::iterator it = vi.begin(); it != vi.end(); it++) {
		tmp.push_back(all.at(*it - length.begin()));
	}
	std::vector<int> result;
	for (int i = 0; i < num; i++) {
		result.push_back(tmp.at(i));
	}
	return result;
}

std::vector<int> BrainDataManager::getPartOfStlIdByROIAndMaxEntropy(int beginROI, int endROI, float rate)
{
	std::vector<int> all = getStlIdByROI(beginROI, endROI);
	std::vector<float> length;
	for (int i = 0; i < all.size(); i++) {
		length.push_back(mStlArray_h[all[i]].entropy);
	}
	std::vector<int> tmp;
	int num = int(all.size() * rate);
	//没有的时候输出all
	if (num == 0) {
		return all;
	}
	std::vector<std::vector<float>::iterator >vi;
	for (std::vector<float>::iterator it = length.begin(); it != length.end(); it++)
		vi.push_back(it);
	sort(vi.begin(), vi.end(), com);
	for (std::vector<std::vector<float>::iterator >::iterator it = vi.begin(); it != vi.end(); it++) {
		tmp.push_back(all.at(*it - length.begin()));
	}
	std::vector<int> result;
	for (int i = 0; i < num; i++) {
		result.push_back(tmp.at(i));
	}
	return result;
}

std::vector<int> BrainDataManager::getPartOfStlIdByROIAndMinEntropy(int beginROI, int endROI, float rate)
{
	std::vector<int> all = getStlIdByROI(beginROI, endROI);
	std::vector<float> length;
	for (int i = 0; i < all.size(); i++) {
		length.push_back(mStlArray_h[all[i]].entropy);
	}
	std::vector<int> tmp;
	int num = int(all.size() * rate);
	//没有的时候输出all
	if (num == 0) {
		return all;
	}
	std::vector<std::vector<float>::iterator >vi;
	for (std::vector<float>::iterator it = length.begin(); it != length.end(); it++)
		vi.push_back(it);
	sort(vi.begin(), vi.end(), comp);
	for (std::vector<std::vector<float>::iterator >::iterator it = vi.begin(); it != vi.end(); it++) {
		tmp.push_back(all.at(*it - length.begin()));
	}
	std::vector<int> result;
	for (int i = 0; i < num; i++) {
		result.push_back(tmp.at(i));
	}
	return result;
}

bool BrainDataManager::vecAtPos3d(vec3f& ret, const vec3f& pos)
{
	if (pos.x <= 0.0000001f || pos.x >= (vf_w - 1.0000001f) || pos.y <= 0.0000001f || pos.y >= (vf_h - 1.0000001f) || pos.z <= 0.0000001f || pos.z >= (vf_d - 1.0000001f)) {
		return false;
	}

	int x = floorf(pos.x);
	int y = floorf(pos.y);
	int z = floorf(pos.z);
	int idx1 = x + y * vf_w + z * vf_w * vf_h;
	int idx2 = x + 1 + y * vf_w + z * vf_w * vf_h;
	int idx3 = x + (y + 1) * vf_w + z * vf_w * vf_h;
	int idx4 = x + 1 + (y + 1) * vf_w + z * vf_w * vf_h;
	int idx5 = x + y * vf_w + (z + 1) * vf_w * vf_h;
	int idx6 = x + 1 + y * vf_w + (z + 1) * vf_w * vf_h;
	int idx7 = x + (y + 1) * vf_w + (z + 1) * vf_w * vf_h;
	int idx8 = x + 1 + (y + 1) * vf_w + (z + 1) * vf_w * vf_h;

	float facx = pos.x - x, facy = pos.y - y, facz = pos.z - z;

	ret = (1 - facx) * (1 - facy) * (1 - facz) * points[idx1]\
		+ facx * (1 - facy) * (1 - facz) * points[idx2]\
		+ (1 - facx) * facy * (1 - facz) * points[idx3]\
		+ facx * facy * (1 - facz) * points[idx4]\
		+ (1 - facx) * (1 - facy) * facz * points[idx5]\
		+ facx * (1 - facy) * facz * points[idx6]\
		+ (1 - facx) * facy * facz * points[idx7]\
		+ facx * facy * facz * points[idx8];
	return true;
}

void BrainDataManager::recomputeVelos()
{
	int tmp = 0;
	for (int i = 0; i < mNumStl; i++) {
		for (int j = 0; j < mStlArray_h[i].numPoint; j++) {
			if (j == (mStlArray_h[i].numPoint - 1)) {
				//如果是最后一个点
				veloArray[tmp] = { 0,0,0 };
				tmp++;
			}
			else {
				veloArray[tmp] = Points[tmp + 1] - Points[tmp];
				tmp++;
			}
		}
	}
}

//计算mStlArray_h的entropy
void BrainDataManager::entropyForStreamline()
{
	//if (old_mNumPoint != 0) free(veloArray);
	veloArray = (vec3f*)malloc(sizeof(vec3f) * mNumPoint);
	recomputeVelos();
	std::vector<vec3f> vel;
	for (int a = 0; a < mNumPoint; a++) {
		vel.push_back(veloArray[a]);
	}
	short* bins = new short[mNumPoint];
	//将3d区域划分成NUM_MAG_BIN层个圈，每层分为NUM_SPHERE_BIN个区域，总共NUM_SPHERE_BIN*NUM_MAG_BIN个区域
	//判断每个向量（后一点减该点）落在哪个哪个区域，然后bins[i]记录该点在哪个区域
	compute3dBinsHost(veloArray, bins, mNumPoint, NUM_MAG_BIN, NUM_SPHERE_BIN);
	//根据bin计算hist然后计算entropy
	float log2 = logf(2.0f), tmp;
	int hist[NUM_3D_BIN];
	for (int i = 0; i < mNumStl; ++i) {
		memset(hist, 0, sizeof(int) * NUM_3D_BIN);
		Streamline s = mStlArray_h[i];
		short* b = &bins[s.start];
		for (int j = 0; j < s.numPoint; ++j) {
			++hist[b[j]];
		}
		tmp = 0.0f;
		for (int j = 0; j < NUM_3D_BIN; j++) {
			if (hist[j] != 0) {
				tmp += hist[j] * logf(hist[j]);
			}
		}
		float entropy = (-(tmp / s.numPoint) + logf(s.numPoint)) / log2; //maxentropy = log2(s.numPoint) = log2(226) = 7.8
		mStlEntropy.push_back(entropy);
		mStlArray_h[i].entropy = entropy;
	}
	delete[] bins;
}

void BrainDataManager::calPointsValue()
{
	if (mNumStl <= 0) return;
	pointCurvature = (float*)malloc(sizeof(float) * mNumPoint);
	pointEntropy = (float*)malloc(sizeof(float) * mNumPoint);
	pointTorsion = (float*)malloc(sizeof(float) * mNumPoint);
	pointLength = (float*)malloc(sizeof(float) * mNumPoint);
	//pointStrength = (float*)malloc(sizeof(float) * mNumPoint);
	//计算每点curvature和torsion
	discreteCurvatureTorsion(Points, mStlArray_h, mNumStl, pointCurvature, pointTorsion);
	//计算每点length和entropy
	int tmp = 0;
	for (int i = 0; i < mNumStl; i++) {
		for (int j = 0; j < mStlArray_h[i].numPoint; j++) {
			pointEntropy[tmp] = mStlArray_h[i].entropy;
			pointLength[tmp] = mStlArray_h[i].length;
			/*
			if (j == mStlArray_h[i].numPoint - 1) {
				pointLength[tmp] = dist(Points[mStlArray_h[i].start + mStlArray_h[i].numPoint - 1],Points[mStlArray_h[i].start + mStlArray_h[i].numPoint - 2]);
			}
			else {
				pointLength[tmp] = dist(Points[mStlArray_h[i].start + j], Points[mStlArray_h[i].start + j + 1]);
			}
			*/
			tmp++;
			
		}
	}

	//每个像素点的平均值
	volumeCurvature = (float*)malloc(sizeof(float) * vf_d * vf_h * vf_w);
	volumeEntropy = (float*)malloc(sizeof(float) * vf_d * vf_h * vf_w);
	volumeTorsion = (float*)malloc(sizeof(float) * vf_d * vf_h * vf_w);
	volumeLength = (float*)malloc(sizeof(float) * vf_d * vf_h * vf_w);
	volumeStrength = (float*)malloc(sizeof(float) * vf_d * vf_h * vf_w);
	volumeFA = (float*)malloc(sizeof(float) * vf_d * vf_h * vf_w);
	volumeMD = (float*)malloc(sizeof(float) * vf_d * vf_h * vf_w);
	volumeRD = (float*)malloc(sizeof(float) * vf_d * vf_h * vf_w);
	volumeAXD = (float*)malloc(sizeof(float) * vf_d * vf_h * vf_w);
	memset(volumeCurvature, 0, sizeof(float) * vf_w * vf_h * vf_d);
	memset(volumeEntropy, 0, sizeof(float) * vf_w * vf_h * vf_d);
	memset(volumeTorsion, 0, sizeof(float) * vf_w * vf_h * vf_d);
	memset(volumeLength, 0, sizeof(float) * vf_w * vf_h * vf_d);
	memset(volumeStrength, 0, sizeof(float) * vf_w * vf_h * vf_d);
	memset(volumeFA, 0, sizeof(float) * vf_w * vf_h * vf_d);
	memset(volumeMD, 0, sizeof(float) * vf_w * vf_h * vf_d);
	memset(volumeRD, 0, sizeof(float) * vf_w * vf_h * vf_d);
	memset(volumeAXD, 0, sizeof(float) * vf_w * vf_h * vf_d);

	alvolumeCurvature = (float*)malloc(sizeof(float) * vf_d * vf_h * vf_w);
	alvolumeEntropy = (float*)malloc(sizeof(float) * vf_d * vf_h * vf_w);
	alvolumeTorsion = (float*)malloc(sizeof(float) * vf_d * vf_h * vf_w);
	alvolumeLength = (float*)malloc(sizeof(float) * vf_d * vf_h * vf_w);
	alvolumeStrength = (float*)malloc(sizeof(float) * vf_d * vf_h * vf_w);
	alvolumeFA = (float*)malloc(sizeof(float) * vf_d * vf_h * vf_w);
	alvolumeMD = (float*)malloc(sizeof(float) * vf_d * vf_h * vf_w);
	alvolumeRD = (float*)malloc(sizeof(float) * vf_d * vf_h * vf_w);
	alvolumeAXD = (float*)malloc(sizeof(float) * vf_d * vf_h * vf_w);

	memset(alvolumeCurvature, 0, sizeof(float) * vf_w * vf_h * vf_d);
	memset(alvolumeEntropy, 0, sizeof(float) * vf_w * vf_h * vf_d);
	memset(alvolumeTorsion, 0, sizeof(float) * vf_w * vf_h * vf_d);
	memset(alvolumeLength, 0, sizeof(float) * vf_w * vf_h * vf_d);
	memset(alvolumeStrength, 0, sizeof(float) * vf_w * vf_h * vf_d);
	memset(alvolumeFA, 0, sizeof(float) * vf_w * vf_h * vf_d);
	memset(alvolumeMD, 0, sizeof(float) * vf_w * vf_h * vf_d);
	memset(alvolumeRD, 0, sizeof(float) * vf_w * vf_h * vf_d);
	memset(alvolumeAXD, 0, sizeof(float) * vf_w * vf_h * vf_d);
	
	alCurvature = (real_1d_array**)malloc(sizeof(real_1d_array*) * vf_d * vf_h * vf_w);
	//alTorsion = (real_1d_array**)malloc(sizeof(real_1d_array*) * vf_d * vf_h * vf_w);
	//alEntropy = (real_1d_array**)malloc(sizeof(real_1d_array*) * vf_d * vf_h * vf_w);
	//alLength = (real_1d_array**)malloc(sizeof(real_1d_array*) * vf_d * vf_h * vf_w);
	//alStrength = (real_1d_array**)malloc(sizeof(real_1d_array*) * vf_d * vf_h * vf_w);
	//alFA = (real_1d_array**)malloc(sizeof(real_1d_array*) * vf_d * vf_h * vf_w);
	//alMD = (real_1d_array**)malloc(sizeof(real_1d_array*) * vf_d * vf_h * vf_w);
	//alRD = (real_1d_array**)malloc(sizeof(real_1d_array*) * vf_d * vf_h * vf_w);
	//alAXD = (real_1d_array**)malloc(sizeof(real_1d_array*) * vf_d * vf_h * vf_w);
	//alStrengthLen = (int*)malloc(sizeof(int) * vf_d * vf_h * vf_w);

	stdvoluemCurvature = (float*)malloc(sizeof(float) * vf_d * vf_h * vf_w);
	stdvoluemTorsion = (float*)malloc(sizeof(float) * vf_d * vf_h * vf_w);
	stdvoluemEntropy = (float*)malloc(sizeof(float) * vf_d * vf_h * vf_w);
	stdvoluemLength = (float*)malloc(sizeof(float) * vf_d * vf_h * vf_w);
	stdvoluemStrength = (float*)malloc(sizeof(float) * vf_d * vf_h * vf_w);
	stdvoluemFA = (float*)malloc(sizeof(float) * vf_d * vf_h * vf_w);
	stdvoluemMD = (float*)malloc(sizeof(float) * vf_d * vf_h * vf_w);
	stdvoluemRD = (float*)malloc(sizeof(float) * vf_d * vf_h * vf_w);
	stdvoluemAXD = (float*)malloc(sizeof(float) * vf_d * vf_h * vf_w);

	memset(stdvoluemCurvature, 0, sizeof(float) * vf_w * vf_h * vf_d);
	memset(stdvoluemTorsion, 0, sizeof(float) * vf_w * vf_h * vf_d);
	memset(stdvoluemEntropy, 0, sizeof(float) * vf_w * vf_h * vf_d);
	memset(stdvoluemLength, 0, sizeof(float) * vf_w * vf_h * vf_d);
	memset(stdvoluemStrength, 0, sizeof(float) * vf_w * vf_h * vf_d);
	memset(stdvoluemFA, 0, sizeof(float) * vf_w * vf_h * vf_d);
	memset(stdvoluemMD, 0, sizeof(float) * vf_w * vf_h * vf_d);
	memset(stdvoluemRD, 0, sizeof(float) * vf_w * vf_h * vf_d);
	memset(stdvoluemAXD, 0, sizeof(float) * vf_w * vf_h * vf_d);
}

void BrainDataManager::normalizeValue(float* value, int valueLen)
{
	float max = value[0];
	float min = value[0];
	for (int i = 0; i < valueLen; i++) {
		if (value[i] > max) {
			max = value[i];
		}
		if (value[i] < min) {
			min = value[i];
		}
	}
	for (int i = 0; i < valueLen; i++) {
		value[i] = (value[i] - min) / (max - min);
	}

	//直方图均衡化
	//float rk[L];
	float* rk = (float*)malloc(sizeof(float) * L);
	float delta = 0.00000000000001;
	int totalNum = 0;
	float step = 1.0 / L;
	for (int i = 0; i < L; i++) {
		rk[i] = 0;
	}
	for (int i = 0; i < valueLen; i++) {
		if (value[i] > 0) {
			int index = floor((value[i] - delta) / step);
			totalNum++;
			rk[index] = rk[index] + 1;
		}
	}
	//概率
	for (int i = 0; i < L; i++) {
		rk[i] = rk[i] / totalNum;
	}
	//积分
	//float sk[L];
	float* sk = (float*)malloc(sizeof(float) * L);
	for (int i = 0; i < L; i++) {
		if (i == 0) {
			sk[i] = rk[i];
		}
		else {
			sk[i] = sk[i - 1] + rk[i];
		}
	}
	for (int i = 0; i < L; i++) {
		sk[i] = sk[i] * (L - 1);
	}
	//映射
	for (int i = 0; i < valueLen; i++) {
		if (value[i] > 0) {
			int index = floor((value[i] - delta) / step);
			value[i] = (round(sk[index]) + 1) * 1.0 / L;
		}
	}
	free(rk);
	free(sk);
}

void BrainDataManager::calDensityData(int index)
{
	if (mNumStl <= 0) return;
	int* b = new int[mNumPoint];
	//计算每个点落在哪里
	compute3dBinsHostDensity(Points, b, mNumPoint, vf_w, vf_h, vf_d);
	hist = (UINT*)malloc(sizeof(UINT) * vf_w * vf_h * vf_d);
	memset(hist, 0, sizeof(UINT) * vf_w * vf_h * vf_d);
	for (int j = 0; j < mNumPoint; ++j) {
		//每个点在hist的分布
		++hist[b[j]];
		volumeCurvature[b[j]] += pointCurvature[j];
		volumeEntropy[b[j]] += pointEntropy[j];
		volumeTorsion[b[j]] += pointTorsion[j];
		volumeLength[b[j]] += pointLength[j];
		volumeFA[b[j]] += FA[j];
		volumeMD[b[j]] += MD[j];
		volumeRD[b[j]] += RD[j];
		volumeAXD[b[j]] += AXD[j];
	}
	//分配空间
	//求pvalue
	
	double** tmpCurvature = (double**)malloc(sizeof(double*) * vf_w * vf_h * vf_d);
	UINT* tmpCurvatureLen = (UINT*)malloc(sizeof(UINT) * vf_w * vf_h * vf_d);
	memset(tmpCurvatureLen, 0, sizeof(UINT) * vf_w * vf_h * vf_d);
	for (int j = 0; j < vf_w * vf_h * vf_d; j++) {
		tmpCurvature[j] = (double*)malloc(sizeof(double) * hist[j]);
	}
	//记录每个点的数组
	for (int j = 0; j < mNumPoint; j++) {
		if(index == 1) tmpCurvature[b[j]][tmpCurvatureLen[b[j]]] = pointCurvature[j];
		if (index == 2) tmpCurvature[b[j]][tmpCurvatureLen[b[j]]] = pointEntropy[j];
		if (index == 3) tmpCurvature[b[j]][tmpCurvatureLen[b[j]]] = pointTorsion[j];
		if (index == 4) tmpCurvature[b[j]][tmpCurvatureLen[b[j]]] = pointLength[j];
		if (index == 5) tmpCurvature[b[j]][tmpCurvatureLen[b[j]]] = FA[j];
		if (index == 6) tmpCurvature[b[j]][tmpCurvatureLen[b[j]]] = MD[j];
		if (index == 7) tmpCurvature[b[j]][tmpCurvatureLen[b[j]]] = RD[j];
		if (index == 8) tmpCurvature[b[j]][tmpCurvatureLen[b[j]]] = AXD[j];
		tmpCurvatureLen[b[j]]++;
	}
	for (int j = 0; j < vf_w * vf_h * vf_d; j++) {
		alCurvature[j] = new real_1d_array();
		alCurvature[j]->setcontent(tmpCurvatureLen[j], tmpCurvature[j]);
		free(tmpCurvature[j]);
	}
	free(tmpCurvature);
	free(tmpCurvatureLen);
	
	volume_h = (UINT*)malloc(sizeof(UINT) * vf_d * vf_h * vf_w);
	//求平均值
	for (int i = 0; i < vf_d * vf_h * vf_w; i++) {
		volume_h[i] = hist[i];
		if (volume_h[i] != 0) {
			volumeCurvature[i] = volumeCurvature[i] / volume_h[i];
			volumeEntropy[i] = volumeEntropy[i] / volume_h[i];
			volumeTorsion[i] = volumeTorsion[i] / volume_h[i];
			volumeLength[i] = volumeLength[i] / volume_h[i];
			volumeFA[i] = volumeFA[i] / volume_h[i];
			volumeMD[i] = volumeMD[i] / volume_h[i];
			volumeRD[i] = volumeRD[i] / volume_h[i];
			volumeAXD[i] = volumeAXD[i] / volume_h[i];
		}
	}
	//求方差
	for (int j = 0; j < mNumPoint; ++j) {
		stdvoluemCurvature[b[j]] += pow((pointCurvature[j] - volumeCurvature[b[j]]), 2);
		stdvoluemEntropy[b[j]] += pow((pointEntropy[j] - volumeEntropy[b[j]]), 2);
		stdvoluemTorsion[b[j]] += pow((pointTorsion[j] - volumeTorsion[b[j]]), 2);
		stdvoluemLength[b[j]] += pow((pointLength[j] - volumeLength[b[j]]), 2);
		stdvoluemFA[b[j]] += pow((FA[j] - volumeFA[b[j]]), 2);
		stdvoluemMD[b[j]] += pow((MD[j] - volumeMD[b[j]]), 2);
		stdvoluemRD[b[j]] += pow((RD[j] - volumeRD[b[j]]), 2);
		stdvoluemAXD[b[j]] += pow((AXD[j] - volumeAXD[b[j]]), 2);
	}
	//求标准差
	for (int i = 0; i < vf_d * vf_h * vf_w; i++) {
		if (volume_h[i] != 0) {
			stdvoluemCurvature[i] = sqrt(stdvoluemCurvature[i] / volume_h[i]);
			stdvoluemEntropy[i] = sqrt(stdvoluemEntropy[i] / volume_h[i]);
			stdvoluemTorsion[i] = sqrt(stdvoluemTorsion[i] / volume_h[i]);
			stdvoluemLength[i] = sqrt(stdvoluemLength[i] / volume_h[i]);
			stdvoluemFA[i] = sqrt(stdvoluemFA[i] / volume_h[i]);
			stdvoluemMD[i] = sqrt(stdvoluemMD[i] / volume_h[i]);
			stdvoluemRD[i] = sqrt(stdvoluemRD[i] / volume_h[i]);
			stdvoluemAXD[i] = sqrt(stdvoluemAXD[i] / volume_h[i]);
		}
	}
	//pvalue时不正则化
	if (index == 0) {
		normalizeValue(volumeCurvature, vf_d * vf_h * vf_w);
		normalizeValue(volumeEntropy, vf_d * vf_h * vf_w);
		normalizeValue(volumeTorsion, vf_d * vf_h * vf_w);
		normalizeValue(volumeLength, vf_d * vf_h * vf_w);
		normalizeValue(volumeFA, vf_d * vf_h * vf_w);
		normalizeValue(volumeMD, vf_d * vf_h * vf_w);
		normalizeValue(volumeRD, vf_d * vf_h * vf_w);
		normalizeValue(volumeAXD, vf_d * vf_h * vf_w);
	}
	normalizeValue(stdvoluemCurvature, vf_d * vf_h * vf_w);
	normalizeValue(stdvoluemEntropy, vf_d* vf_h* vf_w);
	normalizeValue(stdvoluemTorsion, vf_d* vf_h* vf_w);
	normalizeValue(stdvoluemLength, vf_d* vf_h* vf_w);
	normalizeValue(stdvoluemFA, vf_d* vf_h* vf_w);
	normalizeValue(stdvoluemMD, vf_d* vf_h* vf_w);
	normalizeValue(stdvoluemRD, vf_d* vf_h* vf_w);
	normalizeValue(stdvoluemAXD, vf_d* vf_h* vf_w);

	free(b);
	//free(hist);
}

void BrainDataManager::calPvalueData(alglib::real_1d_array** alcurvature2, alglib::real_1d_array** alentropy2, alglib::real_1d_array** altorsion2, alglib::real_1d_array** allength2, alglib::real_1d_array** alstrength2, alglib::real_1d_array** alfa2, alglib::real_1d_array** almd2, alglib::real_1d_array** alrd2, alglib::real_1d_array** alaxd2, UINT* hist2)
{
	for (int j = 0; j < vf_d * vf_h * vf_w; j++) {
		double result = 0;
		double left = 0;
		double right = 0;
		alglib::studentttest2(*alCurvature[j], hist[j], *alcurvature2[j], hist2[j], result, left, right);
		alvolumeCurvature[j] = min(-log(result), 5.1) / 5.1;

		if (result != 0 && result != 1 && result < 0) {
			double a = result;
		}
		//alglib::studentttest2(*alTorsion[j], hist[j], *altorsion2[j], hist2[j], result, left, right);
		//alvolumeTorsion[j] = result;
		//alglib::studentttest2(*alEntropy[j], hist[j], *alentropy2[j], hist2[j], result, left, right);
		//alvolumeEntropy[j] = result;
		//alglib::studentttest2(*alLength[j], hist[j], *allength2[j], hist2[j], result, left, right);
		//alvolumeLength[j] = result;
		//alglib::studentttest2(*alStrength[j], hist[j], *alstrength2[j], hist2[j], result, left, right);
		/*
		//alvolumeStrength[j] = result;
		//alglib::studentttest2(*alFA[j], hist[j], *alfa2[j], hist2[j], result, left, right);
		*/
		//alvolumeFA[j] = result;
		//alglib::studentttest2(*alMD[j], hist[j], *almd2[j], hist2[j], result, left, right);
		//alvolumeMD[j] = result;
		//alglib::studentttest2(*alRD[j], hist[j], *alrd2[j], hist2[j], result, left, right);
		//alvolumeRD[j] = result;
		//alglib::studentttest2(*alAXD[j], hist[j], *alaxd2[j], hist2[j], result, left, right);
		//alvolumeAXD[j] = result;
		
	}
	normalizeValue(alvolumeCurvature, vf_d * vf_h * vf_w);
	//normalizeValue(alvolumeTorsion, vf_d * vf_h * vf_w);
	//normalizeValue(alvolumeEntropy, vf_d * vf_h * vf_w);
	//normalizeValue(alvolumeLength, vf_d * vf_h * vf_w);
	//normalizeValue(alvolumeStrength, vf_d * vf_h * vf_w);
	
	//normalizeValue(alvolumeFA, vf_d * vf_h * vf_w);
	//normalizeValue(alvolumeMD, vf_d * vf_h * vf_w);
	//normalizeValue(alvolumeRD, vf_d * vf_h * vf_w);
	//normalizeValue(alvolumeAXD, vf_d * vf_h * vf_w);
	
}

void BrainDataManager::calPvalueData(alglib::real_1d_array** alcurvature2, UINT* hist2, int type, float* g1, float* g2)
{
	for (int j = 0; j < vf_d * vf_h * vf_w; j++) {
		double result = 0;
		double left = 0;
		double right = 0;
		alglib::studentttest2(*alCurvature[j], hist[j], *alcurvature2[j], hist2[j], result, left, right);
		if (type == 1) {
			//组1小于组2的置零
			if (g1[j] < g2[j]) {
				alvolumeCurvature[j] = 0;
			}
			else {
				alvolumeCurvature[j] = min(-log(result), 5.1) / 5.1;
			}
		}
		else {
			//组1大于组2的置零
			if (g1[j] >= g2[j]) {
				alvolumeCurvature[j] = 0;
			}
			else {
				alvolumeCurvature[j] = min(-log(result), 5.1) / 5.1;
			}
		}
		

		if (result != 0 && result != 1) {
			double a = result;
		}

	}
	normalizeValue(alvolumeCurvature, vf_d * vf_h * vf_w);
}

/*
void BrainDataManager::calPvalueData(real_1d_array **alcurvature2, UINT* hist2)
{
	for (int j = 0; j < vf_d * vf_h * vf_w; j++) {
		double result = 0;
		double left = 0;
		double right = 0;
		alglib::studentttest2(*alCurvature[j], hist[j], *alcurvature2[j], hist2[j], result, left, right);
		alvolumeCurvature[j] = result;
		if (result != 1) {
			double a = result;
		}
	}
	normalizeValue(alvolumeCurvature, vf_d * vf_h * vf_w);

	//real_1d_array a = "[1,1,1,1]";
	//real_2d_array a("[[1]]");
	//real_1d_array a = "[1,1,1,1]";
	//real_1d_array *a = new real_1d_array();
	//double b[4] = { 1, 1, 1, 1 };
	//a->setcontent(4, b);
	
	//alglib::studentttest2(*a, 4, *a, 1, result, left, right);
}
*/
void BrainDataManager::calPixelDensityData(float* matrixm, float* matrixp, int xNum, int yNum, int zNum)
{
	int* bins = new int[mNumPoint];
	//计算每个点落在哪里
	//矩阵
	float* matrixpm = (float*)malloc(16 * sizeof(float));
	computeMatrix2(matrixpm, matrixm, matrixp);
	
	float j;
	for (int i = 0; i < 16; i++) {
		j = matrixpm[i];
	}
	
	//将matrix写入device
	cudaMalloc((void**)& matrixpm_d, sizeof(float) * 16);
	auto cuda_status = cudaMemcpy(matrixpm_d, matrixpm, (sizeof(float) * 16), cudaMemcpyHostToDevice);
	if (cuda_status != cudaSuccess)
	{
		fprintf(stderr, "cudaMallocArray Failed");
		return;
		//exit( EXIT_FAILURE );
	}
	compute3dPixelBinsHostDensity(Points, bins, mNumPoint, xNum, yNum, zNum, matrixpm_d);
	int* hist = (int*)malloc(sizeof(int) * xNum * yNum * zNum);
	memset(hist, 0, sizeof(int) * xNum * yNum * zNum);
	for (int j = 0; j < mNumPoint; ++j) {
		++hist[bins[j]];
	}
	volume_h = (UINT*)malloc(sizeof(UINT) * xNum * yNum * zNum);
	for (int i = 0; i < xNum * yNum * zNum; i++) {
		volume_h[i] = hist[i];
		if (hist[i] == mNumPoint || hist[i] > 0) {
			//std::cout << i << std::endl;
		}
	}
	free(bins);
	free(hist);
	free(matrixpm);
}

void BrainDataManager::readFileJson()
{
	Json::Reader reader;
	Json::Value root;

	//从文件中读取，保证当前文件有test.json文件
	//std::ifstream in("E:\\braindata\\FA_pvalue\\FA_json\\007_S_4911_1_pvalue_FA.json", std::ios::binary);
	std::ifstream in("E:\\braindata\\FA_S_pvalue\\FA_S_json\\007_S_4911_1_pvalue_FA.json", std::ios::binary);
	//in.open("test.json", ios::binary);

	if (!in.is_open())
	{
		std::cout << "Error opening file\n";
		return;
	}

	if (reader.parse(in, root))
	{
		int tmp = 0;
		for (unsigned int i = 0; i < root["data"].size(); i++) {
			for (unsigned int j = 0; j < root["data"][i].size(); j++) {
				for (unsigned int k = 0; k < root["data"][i][j].size(); k++) {
					float result = root["data"][i][j][k].asFloat();
					if (result > 0 && volume_h[tmp] > 0) {
						alvolumeCurvature[tmp] = min(-log(fabs(result)), 5.1) / 5.1;
					}
					else {
						alvolumeCurvature[tmp] = 0;
					}
					/*
					if (fabs(result) != 1) {
						std::cout << "no 1" << std::endl;
					}
					*/
					tmp++;
				}
			}
		}
		std::cout << "Reading Complete!" << std::endl;
	}
	else
	{
		std::cout << "parse error\n" << std::endl;
	}
	//normalizeValue(alvolumeCurvature, vf_d * vf_h * vf_w);
	in.close();
}

void BrainDataManager::connectiveInROIs()
{
	for (int i = 0; i < mNumStl; i++) {
		mROI->connective[mStlArray_h[i].beginROI][mStlArray_h[i].endROI]++;
		mROI->avCurvature[mStlArray_h[i].beginROI][mStlArray_h[i].endROI] += mStlArray_h[i].curvature;
		mROI->avEntropy[mStlArray_h[i].beginROI][mStlArray_h[i].endROI] += mStlArray_h[i].entropy;
		mROI->avTorsion[mStlArray_h[i].beginROI][mStlArray_h[i].endROI] += mStlArray_h[i].torsion;
		mROI->avLength[mStlArray_h[i].beginROI][mStlArray_h[i].endROI] += mStlArray_h[i].length;
	}
}

//简易简化
void BrainDataManager::easySimplify()
{
	std::vector<vec3f> tmpPoints;
	std::vector<Streamline> tmpStl;
	int countStl = 0;
	int a = 0;
	for (int i = 0; i < mNumStl; i++) {
		if (mStlArray_h[i].numPoint <= 1) continue; //长度小于等于1的都不要
		a++;
		int countPoint = 0;
		Streamline localStl;
		if (countStl == 0) {
			localStl.length = mStlArray_h[i].length;
			localStl.ROI_num = mStlArray_h[i].ROI_num;
			localStl.sid = countStl;
			localStl.start = 0;
		}
		else {
			localStl.length = mStlArray_h[i].length;
			localStl.ROI_num = mStlArray_h[i].ROI_num;
			localStl.sid = countStl;
			localStl.start = tmpStl.back().start + tmpStl.back().numPoint;
		}
		//先把第一个点放进去
		int start = mStlArray_h[i].start;
		vec3f prePoint = Points[start];
		tmpPoints.push_back(prePoint);
		countPoint++;
		//当两个点距离大于等于MIN_DISTANCE，放进去
		for (int j = 1; j < mStlArray_h[i].numPoint; j++) { 
			if (dist(Points[start + j], prePoint) >= mminDistance) {
				prePoint = Points[start + j];
				tmpPoints.push_back(prePoint);
				countPoint++;
			}
		}
		localStl.numPoint = countPoint;
		//假设选择后的点大于等于MIN_NUM，则放进去，否则把对应的point弹出来
		if (countPoint >= mminNum) {
			tmpStl.push_back(localStl);
			countStl++;
		}
		else {
			for (int j = 0; j < countPoint; j++) {
				tmpPoints.pop_back();
			}
		}
	}
	mSimNumStl = countStl;
	mSimStlArray_h = (Streamline*)malloc(sizeof(Streamline) * mSimNumStl);
	//装载stl
	int tmp = 0;
	for (std::vector<Streamline>::iterator iter = tmpStl.begin(); iter != tmpStl.end(); iter++)
	{
		mSimStlArray_h[tmp].length = iter->length;
		mSimStlArray_h[tmp].numPoint = iter->numPoint;
		mSimStlArray_h[tmp].ROI_num = iter->ROI_num;
		mSimStlArray_h[tmp].sid = iter->sid;
		mSimStlArray_h[tmp].start = iter->start;
		tmp++;
	}
	mSimNumPoint = (mSimStlArray_h + mSimNumStl - 1)->start + (mSimStlArray_h + mSimNumStl - 1)->numPoint;
	SimPoints = (vec3f*)malloc(sizeof(vec3f) * mSimNumPoint);
	//装载point
	tmp = 0;
	for (std::vector<vec3f>::iterator iter = tmpPoints.begin(); iter != tmpPoints.end(); iter++)
	{
		SimPoints[tmp] = *iter;
		tmp++;
	}
	tmpStl.clear();
	tmpPoints.clear();
}

cudaStreamlineRenderer* BrainDataManager::getcudaStreamlineRenderer(const int& num_faces, const float& radius)
{
	cudaStreamlineRenderer* renderer = new cudaStreamlineRenderer(Points, mNumPoint, mStlArray_h, mNumStl, center, num_faces, radius);
	//赋值颜色
	renderer->updateStreamlineColorVelocity(colorVelos, COLOR_MAP_D3_NO_GRAY);
	return renderer;
}

cudaStreamlineRenderer* BrainDataManager::getcudaStreamlineRendererWithoutCenter(const int& num_faces, const float& radius)
{
	if (mNumStl > 0) {
		cudaStreamlineRenderer* renderer = new cudaStreamlineRenderer(Points, mNumPoint, mStlArray_h, mNumStl, num_faces, radius);
		//赋值颜色
		renderer->updateStreamlineColorVelocity(colorVelos, COLOR_MAP_D3_NO_GRAY);
		return renderer;
	}
	return NULL;
}

cudaStreamlineRenderer* BrainDataManager::getcudaCentralStreamline(const int& num_faces, const float& radius)
{
	if (centralLineNumStl > 0) {
		//findCentralLine();
		cudaStreamlineRenderer* renderer = new cudaStreamlineRenderer(centralLinePoints, centralLineNumPoint, centralLineStlArray_h, centralLineNumStl, num_faces, radius);
		//赋值颜色
		renderer->updateStreamlineColorVelocity(colorVelos, COLOR_MAP_D3_NO_GRAY);
		return renderer;
	}
	return NULL;
}

cudaStreamlineRenderer* BrainDataManager::getcudaCentralStreamlineInCtl(const int& num_faces, const float& radius)
{
	if (CTL_centralLineNumStl > 0) {
		cudaStreamlineRenderer* renderer = new cudaStreamlineRenderer(CTL_centralLinePoints, CTL_centralLineNumPoint, CTL_centralLineStlArray_h, CTL_centralLineNumStl, num_faces, radius);
		//赋值颜色
		renderer->updateStreamlineColorVelocity(colorVelos, COLOR_MAP_D3_NO_GRAY);
		return renderer;
	}
	return NULL;
}

cudaStreamlineRenderer* BrainDataManager::getSimplifycudaStreamlineRenderer(const int& num_faces, const float& radius, int minNum, int minDistance)
{
	mminNum = minNum;
	mminDistance = minDistance;
	easySimplify();
	cudaStreamlineRenderer* renderer = new cudaStreamlineRenderer(SimPoints, mSimNumPoint, mSimStlArray_h, mSimNumStl, center, num_faces, radius);
	return renderer;
}

volumeRender* BrainDataManager::getVolumeRenderer(int width, int height, int index)
{
	if (mNumStl > 0) {
		//volumeRender* renderer = new volumeRender(volume_h, vf_w, vf_h, vf_d, width, height, index);
		volumeRender* renderer = new volumeRender(volume_h, alvolumeCurvature, vf_w, vf_h, vf_d, width, height, index);
		//volumeRender* renderer = new volumeRender(volume_h, stdvoluemEntropy, vf_w, vf_h, vf_d, width, height, index);
		return renderer;
	}
	return NULL;
}

volumeRender* BrainDataManager::getVolumeRenderer(int width, int height, UINT* volume_copy, int index)
{
	if (mNumStl > 0) {
		UINT* volume_add = (UINT*)malloc(sizeof(UINT) * vf_d * vf_h * vf_w);
		for (int a = 0; a < vf_d * vf_h * vf_w; a++) {
			volume_add[a] = volume_h[a] + volume_copy[a];
		}
		//volumeRender* renderer = new volumeRender(volume_add, vf_w, vf_h, vf_d, width, height, index);
		volumeRender* renderer = new volumeRender(volume_add, alvolumeCurvature, vf_w, vf_h, vf_d, width, height, index);
		free(volume_add);
		return renderer;
	}
	return NULL;
}

volumeRender* BrainDataManager::getVolumeRenderer(int width, int height, float* matrixm, float* matrixp, int xNum, int yNum, int zNum, int index)
{
	//计算volume数据，再生成volume
	calPixelDensityData(matrixm, matrixp, xNum, yNum, zNum);
	volumeRender* renderer = new volumeRender(volume_h, vf_w, vf_h, vf_d, width, height, index);
	return renderer;
}

int fileNameFilter(const dirent* cur)
{
	std::string str(cur->d_name);
	if (str.find(".mat") != std::string::npos) {
		return 1;
	}
	return 0;
}

std::vector<std::string> getDirBinsSortedPath(std::string dirPath)
{
	struct dirent** namelist;
	std::vector<std::string> ret;
	int n = scandir(dirPath.c_str(), &namelist, fileNameFilter, alphasort);
	if (n < 0) {
		return ret;
	}
	for (int i = 0; i < n; ++i) {
		std::string filePath(namelist[i]->d_name);
		ret.push_back(dirPath + "\\" + filePath);
		free(namelist[i]);
	};
	free(namelist);
	return ret;
}

void BrainDataManager::readAllPath()
{
	PointPath = getDirBinsSortedPath("D:\\Jun\\workspace\\brainfiber\\braindata\\tract\\tract");
	//PointPath = getDirBinsSortedPath("D:\\Jun\\workspace\\brainfiber\\braindata\\tract_TEM\\tract_mat");
	ROIPath = getDirBinsSortedPath("D:\\Jun\\workspace\\brainfiber\\braindata\\labeled_tract\\labeled_tract\\90ROI");
	//ROIPath = getDirBinsSortedPath("D:\\Jun\\workspace\\brainfiber\\braindata\\tract_TEM\\roi");
}

void BrainDataManager::readROIPathAndFindCorrPointPath()
{
	PointPath.clear();
	//ROIPath = getDirBinsSortedPath("D:\\Jun\\workspace\\brainfiber\\braindata\\labeled_tract_mci\\labeled_tract_mci");
	ROIPath = getDirBinsSortedPath("D:\\Jun\\workspace\\brainfiber\\braindata\\labeled_tract\\labeled_tract\\DK");
	std::string PATH = "D:\\Jun\\workspace\\brainfiber\\braindata\\tract\\tract\\";
	for (int a = 0; a < ROIPath.size(); a++) {
		std::string id = ROIPath[a].substr(69, 10); //69dk 74mci
		std::string pid1 = PATH + id + "_1_fibers_FA_normed1.mat"; //每条streamline每个点的x,y,z
		std::string pid2 = PATH + id + "_1_fibers_FA_normed2.mat";
		PointPath.push_back(pid1);
		PointPath.push_back(pid2);
	}
}

void BrainDataManager::updateColorvelos()
{
	colorVelos = (vec3f*)malloc(sizeof(vec3f) * mNumPoint);
	for (int i = 0; i < mNumPoint; i++) {
		float tmp = ROIids[i] * 1.0;
		colorVelos[i] = { 0, 0, tmp };
	}
}

void BrainDataManager::updateColorvelosByROIs()
{
	colorVelos = (vec3f*)malloc(sizeof(vec3f) * mNumPoint);
	int tmp = 0;
	for (int i = 0; i < mNumStl; i++) {
		int sRoi = mStlArray_h[i].beginROI;
		int eRoi = mStlArray_h[i].endROI;
		//将其统一成sRoi>=eRoi,下三角形
		if (sRoi < eRoi) {
			int r = sRoi;
			sRoi = eRoi;
			eRoi = r;
		}
		//将roi对编码
		float code = (1 + sRoi) * sRoi / 2 + eRoi;
		for (int j = 0; j < mStlArray_h[i].numPoint; j++) {
			colorVelos[tmp] = { 0, 0, code };
			tmp++;
		}
		if (sRoi == 45 && eRoi == 10) {
			int a = code;
		}
	}
}

void BrainDataManager::writeJson()
{
	//生成dom元素和分配器
	Document doc;
	doc.SetObject();
	Document::AllocatorType& allocator = doc.GetAllocator();
	//people id, 是否患病，其他属性可在这添加
	Value pID;
	pID.SetString(peopleId.c_str(), allocator);
	doc.AddMember("id", pID, allocator);
	doc.AddMember("isPatient", 0, allocator);
	//ROI数组
	Value ROIarray(kArrayType);
	//对于每个ROI，找到以它出发的streamline
	for (int i = 0; i < 91; i++) {
		Value ROI(kObjectType);
		//ROI的一些属性
		ROI.AddMember("ROI_id", i, allocator);
		Value streamlineArray(kArrayType);
		for (int j = 0; j < mNumStl; j++) {
			if (mStlArray_h[j].beginROI == i) {
				//streamline也是obj
				Value streamline(kObjectType);
				//streamline的一些属性
				Value streamlineAttr(kObjectType);
				streamlineAttr.AddMember("length", mStlArray_h[j].length, allocator);
				streamlineAttr.AddMember("pointsNum", mStlArray_h[j].numPoint, allocator);
				streamlineAttr.AddMember("entropy", mStlArray_h[j].entropy, allocator);
				streamlineAttr.AddMember("curvature", mStlArray_h[j].curvature, allocator);
				streamlineAttr.AddMember("torsion", mStlArray_h[j].torsion, allocator);
				streamline.AddMember("Attribute", streamlineAttr, allocator);
				//streamline包含的点
				Value pointArray(kArrayType);
				for (int k = 0; k < min(mStlArray_h[j].numPoint, 2); k++) { // k < mStlArray_h[j].numPoint
					Value point(kObjectType);
					int index = mStlArray_h[j].start + k;
					point.AddMember("x", Points[index].x, allocator);
					point.AddMember("y", Points[index].y, allocator);
					point.AddMember("z", Points[index].z, allocator);
					pointArray.PushBack(point, allocator);
				}
				streamline.AddMember("Points", pointArray, allocator);
				streamlineArray.PushBack(streamline, allocator);
			}
		}
		ROI.AddMember("streamlineArray", streamlineArray, allocator);
		ROIarray.PushBack(ROI, allocator);
	}
	doc.AddMember("ROIs", ROIarray, allocator);
/*
	//构建键值对
	doc.AddMember("name", "jack", allocator);
	doc.AddMember("age", 18, allocator);
	//构建数组
	Value array_sub(kArrayType);
	array_sub.PushBack("a", allocator);
	array_sub.PushBack("b", allocator);
	doc.AddMember("sub", array_sub, allocator);
	//构建数组object[{"a":"A","b":"B"},{}]
	Value array_json(kArrayType);
	Value obj(kObjectType);
	obj.AddMember("a", "A", allocator);
	obj.AddMember("b", "B", allocator);
	array_json.PushBack(obj, allocator);
	Value obj1(kObjectType);
	obj1.AddMember("a", "C22", allocator);
	obj1.AddMember("b", "D", allocator);
	array_json.PushBack(obj1, allocator);
	doc.AddMember("elp", array_json, allocator);
*/
	StringBuffer s;
	Writer<StringBuffer> writer(s);
	doc.Accept(writer);

	//写入json
	std::string jsonPath = "D:\\Jun\\workspace\\brainfiber\\braindata\\brain1.json";
	std::ofstream outfile;
	outfile.open(jsonPath.c_str());
	if (!outfile.fail()) {
		outfile << s.GetString();
		outfile.close();
	}
}

void BrainDataManager::findCentralLine()
{
	/*
	if (centralLineNumStl != 0) {
		free(centralLinePoints);
		free(centralLineStlArray_h);
	}
	*/
	std::vector<int> sample;
	if (mNumStl > 1000) {
		//随机取样100
		std::vector<int> allSample;
		for (int i = 0; i < mNumStl; i++) {
			allSample.push_back(i);
		}
		random_shuffle(allSample.begin(), allSample.end());
		for (int i = 0; i < 1000; i++) {
			sample.push_back(allSample[i]);
		}
	}
	else {
		for (int i = 0; i < mNumStl; i++) {
			sample.push_back(i);
		}
	}
	int point_num = 0;
	Streamline* stls = (Streamline*)malloc(sizeof(Streamline) * sample.size());
	for (int i = 0; i < sample.size(); i++) {
		stls[i].beginROI = mStlArray_h[sample[i]].beginROI;
		stls[i].endROI = mStlArray_h[sample[i]].endROI;
		stls[i].curvature = mStlArray_h[sample[i]].curvature;
		stls[i].entropy = mStlArray_h[sample[i]].entropy;
		stls[i].length = mStlArray_h[sample[i]].length;
		stls[i].numPoint = mStlArray_h[sample[i]].numPoint;
		stls[i].ROI_num = mStlArray_h[sample[i]].ROI_num;
		stls[i].torsion = mStlArray_h[sample[i]].torsion;
		stls[i].sid = i;
		if (i == 0) {
			stls[i].start = 0;
		}
		else {
			stls[i].start = stls[i-1].start + stls[i-1].numPoint;;
		}
		point_num += stls[i].numPoint;
	}
	vec3f* stls_points = (vec3f*)malloc(sizeof(vec3f) * point_num);
	int tmp = 0;
	for (int i = 0; i < sample.size(); i++) {
		for (int j = 0; j < mStlArray_h[sample[i]].numPoint; j++) {
			stls_points[tmp].x = Points[mStlArray_h[sample[i]].start + j].x;
			stls_points[tmp].y = Points[mStlArray_h[sample[i]].start + j].y;
			stls_points[tmp].z = Points[mStlArray_h[sample[i]].start + j].z;
			tmp++;
		}
	}
	
	//计算距离矩阵
	int* distance = genConnectionDistance(stls, stls, stls_points, stls_points, sample.size(), sample.size(), 5.0f);
	//寻找距离矩阵的最大值
	int max = -999;
	int max_index = 0;
	for (int i = 0; i < sample.size(); i++) {
		int maxtmp = 0;
		for (int j = 0; j < sample.size(); j++) {
			maxtmp += distance[i*sample.size() + j];
		}
		if (maxtmp > max) {
			max = maxtmp;
			max_index = i;
		}
	}
	delete[] distance;
	centralLineStlArray_h = (Streamline*)malloc(sizeof(Streamline) * 1);
	centralLineNumStl = 1;
	centralLineStlArray_h[0].beginROI = stls[max_index].beginROI;
	centralLineStlArray_h[0].endROI = stls[max_index].endROI;
	centralLineStlArray_h[0].entropy = stls[max_index].entropy;
	centralLineStlArray_h[0].length = stls[max_index].length;
	centralLineStlArray_h[0].torsion = stls[max_index].torsion;
	centralLineStlArray_h[0].curvature = stls[max_index].curvature;
	centralLineStlArray_h[0].numPoint = stls[max_index].numPoint;
	centralLineStlArray_h[0].ROI_num = stls[max_index].ROI_num;
	centralLineStlArray_h[0].start = 0;
	centralLineStlArray_h[0].sid = 0;

	centralLineNumPoint = centralLineStlArray_h[0].numPoint;
	centralLinePoints = (vec3f*)malloc(sizeof(vec3f) * centralLineNumPoint);
	
	for (int i = 0; i < stls[max_index].numPoint; i++) {
		centralLinePoints[i].x = stls_points[stls[max_index].start + i].x;
		centralLinePoints[i].y = stls_points[stls[max_index].start + i].y;
		centralLinePoints[i].z = stls_points[stls[max_index].start + i].z;
	}

	free(stls);
	free(stls_points);
	sample.clear();
	/**/
}

void BrainDataManager::findCentralLineInCtl()
{
	std::vector<int> sample;
	if (centralLineNumStl > 1000) {
		//随机取样100
		std::vector<int> allSample;
		for (int i = 0; i < centralLineNumStl; i++) {
			allSample.push_back(i);
		}
		random_shuffle(allSample.begin(), allSample.end());
		for (int i = 0; i < 1000; i++) {
			sample.push_back(allSample[i]);
		}
	}
	else {
		for (int i = 0; i < centralLineNumStl; i++) {
			sample.push_back(i);
		}
	}
	int point_num = 0;
	Streamline* stls = (Streamline*)malloc(sizeof(Streamline) * sample.size());
	for (int i = 0; i < sample.size(); i++) {
		stls[i].beginROI = centralLineStlArray_h[sample[i]].beginROI;
		stls[i].endROI = centralLineStlArray_h[sample[i]].endROI;
		stls[i].curvature = centralLineStlArray_h[sample[i]].curvature;
		stls[i].entropy = centralLineStlArray_h[sample[i]].entropy;
		stls[i].length = centralLineStlArray_h[sample[i]].length;
		stls[i].numPoint = centralLineStlArray_h[sample[i]].numPoint;
		stls[i].ROI_num = centralLineStlArray_h[sample[i]].ROI_num;
		stls[i].torsion = centralLineStlArray_h[sample[i]].torsion;
		stls[i].sid = i;
		if (i == 0) {
			stls[i].start = 0;
		}
		else {
			stls[i].start = stls[i - 1].start + stls[i - 1].numPoint;;
		}
		point_num += stls[i].numPoint;
	}
	vec3f* stls_points = (vec3f*)malloc(sizeof(vec3f) * point_num);
	int tmp = 0;
	for (int i = 0; i < sample.size(); i++) {
		for (int j = 0; j < centralLineStlArray_h[sample[i]].numPoint; j++) {
			stls_points[tmp].x = centralLinePoints[centralLineStlArray_h[sample[i]].start + j].x;
			stls_points[tmp].y = centralLinePoints[centralLineStlArray_h[sample[i]].start + j].y;
			stls_points[tmp].z = centralLinePoints[centralLineStlArray_h[sample[i]].start + j].z;
			tmp++;
		}
	}

	//计算距离矩阵
	int* distance = genConnectionDistance(stls, stls, stls_points, stls_points, sample.size(), sample.size(), 5.0f);
	//寻找距离矩阵的最大值
	int max = -999;
	int max_index = 0;
	for (int i = 0; i < sample.size(); i++) {
		int maxtmp = 0;
		for (int j = 0; j < sample.size(); j++) {
			maxtmp += distance[i * sample.size() + j];
		}
		if (maxtmp > max) {
			max = maxtmp;
			max_index = i;
		}
	}
	delete[] distance;
	CTL_centralLineStlArray_h = (Streamline*)malloc(sizeof(Streamline) * 1);
	CTL_centralLineNumStl = 1;
	CTL_centralLineStlArray_h[0].beginROI = stls[max_index].beginROI;
	CTL_centralLineStlArray_h[0].endROI = stls[max_index].endROI;
	CTL_centralLineStlArray_h[0].entropy = stls[max_index].entropy;
	CTL_centralLineStlArray_h[0].length = stls[max_index].length;
	CTL_centralLineStlArray_h[0].torsion = stls[max_index].torsion;
	CTL_centralLineStlArray_h[0].curvature = stls[max_index].curvature;
	CTL_centralLineStlArray_h[0].numPoint = stls[max_index].numPoint;
	CTL_centralLineStlArray_h[0].ROI_num = stls[max_index].ROI_num;
	CTL_centralLineStlArray_h[0].start = 0;
	CTL_centralLineStlArray_h[0].sid = 0;

	CTL_centralLineNumPoint = CTL_centralLineStlArray_h[0].numPoint;
	CTL_centralLinePoints = (vec3f*)malloc(sizeof(vec3f) * CTL_centralLineNumPoint);

	for (int i = 0; i < stls[max_index].numPoint; i++) {
		CTL_centralLinePoints[i].x = stls_points[stls[max_index].start + i].x;
		CTL_centralLinePoints[i].y = stls_points[stls[max_index].start + i].y;
		CTL_centralLinePoints[i].z = stls_points[stls[max_index].start + i].z;
	}

	free(stls);
	free(stls_points);
	sample.clear();
}

void BrainDataManager::writeBinaryDoc()
{
	std::ofstream outFile("test.dat", std::ios::out | std::ios::binary);
	outFile.write((char*)& mNumPoint, sizeof(mNumPoint));
	for (int i = 0; i < mNumPoint; i++) {
		outFile.write((char*)& Points[i], sizeof(vec3f));
	}
	outFile.close();
}

void BrainDataManager::readBinaryDoc()
{
	vec3f p;
	int number;
	std::ifstream inFile("test.dat", std::ios::in | std::ios::binary);
	int count = 0;
	if (inFile) {
		inFile.read((char*)& number, sizeof(number));
		while (inFile.read((char*)& p, sizeof(p))) {
			//int readedBytes = inFile.gcount();
			count++;
		}
	}
	else {
		std::cout << "read dat error" << std::endl;
	}
}

void BrainDataManager::writeStreamlineBinary()
{
	std::string pathName;
	if (mStlArray_h[0].beginROI < mStlArray_h[0].endROI) {
		pathName = std::to_string(mStlArray_h[0].beginROI) + "_" + std::to_string(mStlArray_h[0].endROI);// +"\\streamline\\test.dat";
	}
	else {
		pathName = std::to_string(mStlArray_h[0].endROI) + "_" + std::to_string(mStlArray_h[0].beginROI);
	}
	 
	pathName = "D:\\Jun\\workspace\\brainfiber\\braindata\\preData\\" + pathName +  "\\streamline\\" +  peopleId +".dat";
	char fileName[200];
	pathName.copy(fileName, pathName.length());
	char* tag;
	for (tag = fileName; *tag; tag++)
	{
		if (*tag == '\\')
		{
			char buf[1000], path[1000];
			strcpy(buf, fileName);
			buf[strlen(fileName) - strlen(tag) + 1] = NULL;
			strcpy(path, buf);
			if (access(path, 6) == -1)
			{
				mkdir(path);
			}
		}
	}
	std::ofstream outFile(pathName, std::ios::out | std::ios::binary);
	outFile.write((char*)& mNumStl, sizeof(mNumStl));
	for (int i = 0; i < mNumStl; i++) {
		outFile.write((char*)& mStlArray_h[i], sizeof(Streamline));
	}
	outFile.close();
}

void BrainDataManager::readStreamlineBinary(int beginROI, int endROI)
{
	std::string pathName;
	if (beginROI < endROI) {
		pathName = std::to_string(beginROI) + "_" + std::to_string(endROI);// +"\\streamline\\test.dat";
	}
	else {
		pathName = std::to_string(endROI) + "_" + std::to_string(beginROI);
	}
	pathName = "D:\\Jun\\workspace\\brainfiber\\braindata\\preData\\" + pathName + "\\streamline\\" + peopleId + ".dat";
	Streamline p;
	int number;
	std::ifstream inFile(pathName, std::ios::in | std::ios::binary);
	int count = 0;
	if (inFile) {
		inFile.read((char*)& number, sizeof(number));
		mNumStl = number;
		mStlArray_h = (Streamline*)malloc(sizeof(Streamline) * number);
		while (inFile.read((char*)& p, sizeof(p))) {
			//int readedBytes = inFile.gcount();
			mStlArray_h[count] = p;
			count++;
		}
	}
	else {
		std::cout << "read dat error" << std::endl;
	}
}

void BrainDataManager::readMoreStreamlineBinary(int beginROI, int endROI)
{
	std::string pathName;
	if (beginROI < endROI) {
		pathName = std::to_string(beginROI) + "_" + std::to_string(endROI);// +"\\streamline\\test.dat";
	}
	else {
		pathName = std::to_string(endROI) + "_" + std::to_string(beginROI);
	}
	pathName = "D:\\Jun\\workspace\\brainfiber\\braindata\\preData\\" + pathName + "\\streamline\\" + peopleId + ".dat";
	Streamline p;
	int number;
	std::ifstream inFile(pathName, std::ios::in | std::ios::binary);
	int count = 0;
	if (inFile) {
		inFile.read((char*)& number, sizeof(number));
		if (mNumStl == 0) {
			mStlArray_h = (Streamline*)malloc(sizeof(Streamline) * number);
		}
		else {
			/*
			Streamline* tmp = (Streamline*)malloc(sizeof(Streamline) * mNumStl);
			memcpy(tmp, mStlArray_h, mNumStl);
			delete(mStlArray_h);
			mStlArray_h = (Streamline*)malloc(sizeof(Streamline) * (number + mNumStl));
			memcpy(mStlArray_h, tmp, mNumStl);
			delete(tmp);
			*/
			mStlArray_h = (Streamline*)realloc(mStlArray_h,sizeof(Streamline) * (number + mNumStl));
			if (!mStlArray_h) {
				std::cout << "realloc error" << std::endl;
			}
		}
		while (inFile.read((char*)& p, sizeof(p))) {
			//int readedBytes = inFile.gcount();
			mStlArray_h[mNumStl + count] = p;
			mStlArray_h[mNumStl + count].sid = mNumStl + count;
			if (mNumStl + count == 0)
				mStlArray_h[mNumStl + count].start = 0;
			else mStlArray_h[mNumStl + count].start = mStlArray_h[mNumStl + count - 1].start + mStlArray_h[mNumStl + count - 1].numPoint;
			if (mStlArray_h[mNumStl + count].start < 0) {
				std::cout << "error" << std::endl;
			}
			count++;
		}
		mNumStl = mNumStl + count;
	}
	else {
		std::cout << "read dat error" << std::endl;
	}
}

void BrainDataManager::writePointsBinary()
{
	std::string pathName;
	if (mStlArray_h[0].beginROI < mStlArray_h[0].endROI) {
		pathName = std::to_string(mStlArray_h[0].beginROI) + "_" + std::to_string(mStlArray_h[0].endROI);// +"\\streamline\\test.dat";
	}
	else {
		pathName = std::to_string(mStlArray_h[0].endROI) + "_" + std::to_string(mStlArray_h[0].beginROI);
	}

	pathName = "D:\\Jun\\workspace\\brainfiber\\braindata\\preData\\" + pathName + "\\points\\" + peopleId + ".dat";
	char fileName[200];
	pathName.copy(fileName, pathName.length());
	char* tag;
	for (tag = fileName; *tag; tag++)
	{
		if (*tag == '\\')
		{
			char buf[1000], path[1000];
			strcpy(buf, fileName);
			buf[strlen(fileName) - strlen(tag) + 1] = NULL;
			strcpy(path, buf);
			if (access(path, 6) == -1)
			{
				mkdir(path);
			}
		}
	}
	std::ofstream outFile(pathName, std::ios::out | std::ios::binary);
	outFile.write((char*)& mNumPoint, sizeof(mNumPoint));
	for (int i = 0; i < mNumPoint; i++) {
		outFile.write((char*)& Points[i], sizeof(vec3f));
	}
	outFile.close();
}

void BrainDataManager::readPointsBinary(int beginROI, int endROI)
{
	std::string pathName;
	if (beginROI < endROI) {
		pathName = std::to_string(beginROI) + "_" + std::to_string(endROI);// +"\\streamline\\test.dat";
	}
	else {
		pathName = std::to_string(endROI) + "_" + std::to_string(beginROI);
	}
	pathName = "D:\\Jun\\workspace\\brainfiber\\braindata\\preData\\" + pathName + "\\points\\" + peopleId + ".dat";
	vec3f p;
	int number;
	std::ifstream inFile(pathName, std::ios::in | std::ios::binary);
	int count = 0;
	if (inFile) {
		inFile.read((char*)& number, sizeof(number));
		mNumPoint = number;
		Points = (vec3f*)malloc(sizeof(vec3f) * number);
		while (inFile.read((char*)& p, sizeof(p))) {
			//int readedBytes = inFile.gcount();
			Points[count] = p;
			count++;
		}
	}
	else {
		std::cout << "read dat error" << std::endl;
	}
}

void BrainDataManager::writeFABinary()
{
	std::string pathName;
	if (mStlArray_h[0].beginROI < mStlArray_h[0].endROI) {
		pathName = std::to_string(mStlArray_h[0].beginROI) + "_" + std::to_string(mStlArray_h[0].endROI);// +"\\streamline\\test.dat";
	}
	else {
		pathName = std::to_string(mStlArray_h[0].endROI) + "_" + std::to_string(mStlArray_h[0].beginROI);
	}

	pathName = "D:\\Jun\\workspace\\brainfiber\\braindata\\preData\\" + pathName + "\\FA\\" + peopleId + ".dat";
	char fileName[200];
	pathName.copy(fileName, pathName.length());
	char* tag;
	for (tag = fileName; *tag; tag++)
	{
		if (*tag == '\\')
		{
			char buf[1000], path[1000];
			strcpy(buf, fileName);
			buf[strlen(fileName) - strlen(tag) + 1] = NULL;
			strcpy(path, buf);
			if (access(path, 6) == -1)
			{
				mkdir(path);
			}
		}
	}
	std::ofstream outFile(pathName, std::ios::out | std::ios::binary);
	outFile.write((char*)&mNumPoint, sizeof(mNumPoint));
	for (int i = 0; i < mNumPoint; i++) {
		outFile.write((char*)&FA[i], sizeof(float));
	}
	outFile.close();
}

void BrainDataManager::readFABinary(int beginROI, int endROI)
{
}

void BrainDataManager::readMoreFABinary(int beginROI, int endROI)
{
	std::string pathName;
	if (beginROI < endROI) {
		pathName = std::to_string(beginROI) + "_" + std::to_string(endROI);// +"\\streamline\\test.dat";
	}
	else {
		pathName = std::to_string(endROI) + "_" + std::to_string(beginROI);
	}
	pathName = "D:\\Jun\\workspace\\brainfiber\\braindata\\preData\\" + pathName + "\\FA\\" + peopleId + ".dat";
	float p;
	int number;
	std::ifstream inFile(pathName, std::ios::in | std::ios::binary);
	int count = 0;
	if (inFile) {
		inFile.read((char*)&number, sizeof(number));
		if (FAnum == 0) {
			FA = (float*)malloc(sizeof(float) * number);
		}
		else {
			FA = (float*)realloc(FA, sizeof(float) * (number + FAnum));
			if (!FA) {
				std::cout << "realloc error" << std::endl;
			}
		}
		while (inFile.read((char*)&p, sizeof(p))) {
			//int readedBytes = inFile.gcount();
			FA[FAnum + count] = p;
			count++;
		}
		FAnum = FAnum + count;
	}
	else {
		std::cout << "read dat error" << std::endl;
	}
}

void BrainDataManager::writeMDBinary()
{
	std::string pathName;
	if (mStlArray_h[0].beginROI < mStlArray_h[0].endROI) {
		pathName = std::to_string(mStlArray_h[0].beginROI) + "_" + std::to_string(mStlArray_h[0].endROI);// +"\\streamline\\test.dat";
	}
	else {
		pathName = std::to_string(mStlArray_h[0].endROI) + "_" + std::to_string(mStlArray_h[0].beginROI);
	}

	pathName = "D:\\Jun\\workspace\\brainfiber\\braindata\\preData\\" + pathName + "\\MD\\" + peopleId + ".dat";
	char fileName[200];
	pathName.copy(fileName, pathName.length());
	char* tag;
	for (tag = fileName; *tag; tag++)
	{
		if (*tag == '\\')
		{
			char buf[1000], path[1000];
			strcpy(buf, fileName);
			buf[strlen(fileName) - strlen(tag) + 1] = NULL;
			strcpy(path, buf);
			if (access(path, 6) == -1)
			{
				mkdir(path);
			}
		}
	}
	std::ofstream outFile(pathName, std::ios::out | std::ios::binary);
	outFile.write((char*)&mNumPoint, sizeof(mNumPoint));
	for (int i = 0; i < mNumPoint; i++) {
		outFile.write((char*)&MD[i], sizeof(float));
	}
	outFile.close();
}

void BrainDataManager::readMoreMDBinary(int beginROI, int endROI)
{
	std::string pathName;
	if (beginROI < endROI) {
		pathName = std::to_string(beginROI) + "_" + std::to_string(endROI);// +"\\streamline\\test.dat";
	}
	else {
		pathName = std::to_string(endROI) + "_" + std::to_string(beginROI);
	}
	pathName = "D:\\Jun\\workspace\\brainfiber\\braindata\\preData\\" + pathName + "\\MD\\" + peopleId + ".dat";
	float p;
	int number;
	std::ifstream inFile(pathName, std::ios::in | std::ios::binary);
	int count = 0;
	if (inFile) {
		inFile.read((char*)&number, sizeof(number));
		if (MDnum == 0) {
			MD = (float*)malloc(sizeof(float) * number);
		}
		else {
			MD = (float*)realloc(MD, sizeof(float) * (number + MDnum));
			if (!MD) {
				std::cout << "realloc error" << std::endl;
			}
		}
		while (inFile.read((char*)&p, sizeof(p))) {
			//int readedBytes = inFile.gcount();
			MD[MDnum + count] = p;
			count++;
		}
		MDnum = MDnum + count;
	}
	else {
		std::cout << "read dat error" << std::endl;
	}
}

void BrainDataManager::writeRDBinary()
{
	std::string pathName;
	if (mStlArray_h[0].beginROI < mStlArray_h[0].endROI) {
		pathName = std::to_string(mStlArray_h[0].beginROI) + "_" + std::to_string(mStlArray_h[0].endROI);// +"\\streamline\\test.dat";
	}
	else {
		pathName = std::to_string(mStlArray_h[0].endROI) + "_" + std::to_string(mStlArray_h[0].beginROI);
	}

	pathName = "D:\\Jun\\workspace\\brainfiber\\braindata\\preData\\" + pathName + "\\RD\\" + peopleId + ".dat";
	char fileName[200];
	pathName.copy(fileName, pathName.length());
	char* tag;
	for (tag = fileName; *tag; tag++)
	{
		if (*tag == '\\')
		{
			char buf[1000], path[1000];
			strcpy(buf, fileName);
			buf[strlen(fileName) - strlen(tag) + 1] = NULL;
			strcpy(path, buf);
			if (access(path, 6) == -1)
			{
				mkdir(path);
			}
		}
	}
	std::ofstream outFile(pathName, std::ios::out | std::ios::binary);
	outFile.write((char*)&mNumPoint, sizeof(mNumPoint));
	for (int i = 0; i < mNumPoint; i++) {
		outFile.write((char*)&RD[i], sizeof(float));
	}
	outFile.close();
}

void BrainDataManager::readMoreRDBinary(int beginROI, int endROI)
{
	std::string pathName;
	if (beginROI < endROI) {
		pathName = std::to_string(beginROI) + "_" + std::to_string(endROI);// +"\\streamline\\test.dat";
	}
	else {
		pathName = std::to_string(endROI) + "_" + std::to_string(beginROI);
	}
	pathName = "D:\\Jun\\workspace\\brainfiber\\braindata\\preData\\" + pathName + "\\RD\\" + peopleId + ".dat";
	float p;
	int number;
	std::ifstream inFile(pathName, std::ios::in | std::ios::binary);
	int count = 0;
	if (inFile) {
		inFile.read((char*)&number, sizeof(number));
		if (RDnum == 0) {
			RD = (float*)malloc(sizeof(float) * number);
		}
		else {
			RD = (float*)realloc(RD, sizeof(float) * (number + RDnum));
			if (!RD) {
				std::cout << "realloc error" << std::endl;
			}
		}
		while (inFile.read((char*)&p, sizeof(p))) {
			//int readedBytes = inFile.gcount();
			RD[RDnum + count] = p;
			count++;
		}
		RDnum = RDnum + count;
	}
	else {
		std::cout << "read dat error" << std::endl;
	}
}

void BrainDataManager::writeAXDBinary()
{
	std::string pathName;
	if (mStlArray_h[0].beginROI < mStlArray_h[0].endROI) {
		pathName = std::to_string(mStlArray_h[0].beginROI) + "_" + std::to_string(mStlArray_h[0].endROI);// +"\\streamline\\test.dat";
	}
	else {
		pathName = std::to_string(mStlArray_h[0].endROI) + "_" + std::to_string(mStlArray_h[0].beginROI);
	}

	pathName = "D:\\Jun\\workspace\\brainfiber\\braindata\\preData\\" + pathName + "\\AXD\\" + peopleId + ".dat";
	char fileName[200];
	pathName.copy(fileName, pathName.length());
	char* tag;
	for (tag = fileName; *tag; tag++)
	{
		if (*tag == '\\')
		{
			char buf[1000], path[1000];
			strcpy(buf, fileName);
			buf[strlen(fileName) - strlen(tag) + 1] = NULL;
			strcpy(path, buf);
			if (access(path, 6) == -1)
			{
				mkdir(path);
			}
		}
	}
	std::ofstream outFile(pathName, std::ios::out | std::ios::binary);
	outFile.write((char*)&mNumPoint, sizeof(mNumPoint));
	for (int i = 0; i < mNumPoint; i++) {
		outFile.write((char*)&AXD[i], sizeof(float));
	}
	outFile.close();
}

void BrainDataManager::readMoreAXDBinary(int beginROI, int endROI)
{
	std::string pathName;
	if (beginROI < endROI) {
		pathName = std::to_string(beginROI) + "_" + std::to_string(endROI);// +"\\streamline\\test.dat";
	}
	else {
		pathName = std::to_string(endROI) + "_" + std::to_string(beginROI);
	}
	pathName = "D:\\Jun\\workspace\\brainfiber\\braindata\\preData\\" + pathName + "\\AXD\\" + peopleId + ".dat";
	float p;
	int number;
	std::ifstream inFile(pathName, std::ios::in | std::ios::binary);
	int count = 0;
	if (inFile) {
		inFile.read((char*)&number, sizeof(number));
		if (AXDnum == 0) {
			AXD = (float*)malloc(sizeof(float) * number);
		}
		else {
			AXD = (float*)realloc(AXD, sizeof(float) * (number + AXDnum));
			if (!AXD) {
				std::cout << "realloc error" << std::endl;
			}
		}
		while (inFile.read((char*)&p, sizeof(p))) {
			//int readedBytes = inFile.gcount();
			AXD[AXDnum + count] = p;
			count++;
		}
		AXDnum = AXDnum + count;
	}
	else {
		std::cout << "read dat error" << std::endl;
	}
}

void BrainDataManager::readMorePointsBinary(int beginROI, int endROI)
{
	std::string pathName;
	if (beginROI < endROI) {
		pathName = std::to_string(beginROI) + "_" + std::to_string(endROI);// +"\\streamline\\test.dat";
	}
	else {
		pathName = std::to_string(endROI) + "_" + std::to_string(beginROI);
	}
	pathName = "D:\\Jun\\workspace\\brainfiber\\braindata\\preData\\" + pathName + "\\points\\" + peopleId + ".dat";
	vec3f p;
	int number;
	std::ifstream inFile(pathName, std::ios::in | std::ios::binary);
	int count = 0;
	if (inFile) {
		inFile.read((char*)& number, sizeof(number));
		if (mNumPoint == 0) {
			Points = (vec3f*)malloc(sizeof(vec3f) * number);
		}
		else {
			/*
			vec3f* tmp = (vec3f*)malloc(sizeof(vec3f) * mNumPoint);
			memcpy(tmp, Points, mNumPoint);
			delete(Points);
			Points = (vec3f*)malloc(sizeof(vec3f) * (number + mNumPoint));
			memcpy(Points, tmp, mNumPoint);
			delete(tmp);
			*/
			Points = (vec3f*)realloc(Points,sizeof(vec3f) * (number + mNumPoint)); 
			if (!Points) {
				std::cout << "realloc error" << std::endl;
			}
		}
		while (inFile.read((char*)& p, sizeof(p))) {
			//int readedBytes = inFile.gcount();
			Points[mNumPoint + count] = p;
			count++;
		}
		mNumPoint = mNumPoint + count;
	}
	else {
		std::cout << "read dat error" << std::endl;
	}
}

void BrainDataManager::writeCenterStreamlineBinary()
{
	std::string pathName;
	if (mStlArray_h[0].beginROI < mStlArray_h[0].endROI) {
		pathName = std::to_string(mStlArray_h[0].beginROI) + "_" + std::to_string(mStlArray_h[0].endROI);// +"\\streamline\\test.dat";
	}
	else {
		pathName = std::to_string(mStlArray_h[0].endROI) + "_" + std::to_string(mStlArray_h[0].beginROI);
	}

	pathName = "D:\\Jun\\workspace\\brainfiber\\braindata\\preData\\" + pathName + "\\centralStl\\" + peopleId + ".dat";
	char fileName[200];
	pathName.copy(fileName, pathName.length());
	char* tag;
	for (tag = fileName; *tag; tag++)
	{
		if (*tag == '\\')
		{
			char buf[1000], path[1000];
			strcpy(buf, fileName);
			buf[strlen(fileName) - strlen(tag) + 1] = NULL;
			strcpy(path, buf);
			if (access(path, 6) == -1)
			{
				mkdir(path);
			}
		}
	}
	std::ofstream outFile(pathName, std::ios::out | std::ios::binary);
	outFile.write((char*)& centralLineNumStl, sizeof(centralLineNumStl));
	for (int i = 0; i < centralLineNumStl; i++) {
		outFile.write((char*)& centralLineStlArray_h[i], sizeof(Streamline));
	}
	outFile.close();
}

void BrainDataManager::readCenterStreamlineBinary(int beginROI, int endROI)
{
	std::string pathName;
	if (beginROI < endROI) {
		pathName = std::to_string(beginROI) + "_" + std::to_string(endROI);// +"\\streamline\\test.dat";
	}
	else {
		pathName = std::to_string(endROI) + "_" + std::to_string(beginROI);
	}
	pathName = "D:\\Jun\\workspace\\brainfiber\\braindata\\preData\\" + pathName + "\\centralStl\\" + peopleId + ".dat";
	Streamline p;
	int number;
	std::ifstream inFile(pathName, std::ios::in | std::ios::binary);
	int count = 0;
	if (inFile) {
		inFile.read((char*)& number, sizeof(number));
		centralLineNumStl = number;
		centralLineStlArray_h = (Streamline*)malloc(sizeof(Streamline) * number);
		while (inFile.read((char*)& p, sizeof(p))) {
			//int readedBytes = inFile.gcount();
			centralLineStlArray_h[count] = p;
			count++;
		}
	}
	else {
		std::cout << "read dat error" << std::endl;
	}
}

void BrainDataManager::readMoreCenterStreamlineBinary(int beginROI, int endROI)
{
	std::string pathName;
	if (beginROI < endROI) {
		pathName = std::to_string(beginROI) + "_" + std::to_string(endROI);// +"\\streamline\\test.dat";
	}
	else {
		pathName = std::to_string(endROI) + "_" + std::to_string(beginROI);
	}
	pathName = "D:\\Jun\\workspace\\brainfiber\\braindata\\preData\\" + pathName + "\\centralStl\\" + peopleId + ".dat";
	Streamline p;
	int number;
	std::ifstream inFile(pathName, std::ios::in | std::ios::binary);
	int count = 0;
	if (inFile) {
		inFile.read((char*)& number, sizeof(number));
		if (centralLineNumStl == 0) {
			centralLineStlArray_h = (Streamline*)malloc(sizeof(Streamline) * number);
		}
		else {
			/*
			Streamline* tmp = (Streamline*)malloc(sizeof(Streamline) * centralLineNumStl);
			memcpy(tmp, centralLineStlArray_h, centralLineNumStl);
			delete(centralLineStlArray_h);
			centralLineStlArray_h = (Streamline*)malloc(sizeof(Streamline) * (number + centralLineNumStl));
			memcpy(centralLineStlArray_h, tmp, centralLineNumStl);
			delete(tmp);
			*/
			centralLineStlArray_h = (Streamline*)realloc(centralLineStlArray_h, sizeof(Streamline) * (number + centralLineNumStl));
			if (!centralLineStlArray_h) {
				std::cout << "realloc error" << std::endl;
			}
		}
		while (inFile.read((char*)& p, sizeof(p))) {
			//int readedBytes = inFile.gcount();
			centralLineStlArray_h[centralLineNumStl + count] = p;
			centralLineStlArray_h[centralLineNumStl + count].sid = centralLineNumStl + count;
			if (centralLineNumStl + count == 0)
				centralLineStlArray_h[centralLineNumStl + count].start = 0;
			else centralLineStlArray_h[centralLineNumStl + count].start = centralLineStlArray_h[centralLineNumStl + count - 1].start + centralLineStlArray_h[centralLineNumStl + count - 1].numPoint;
			count++;
		}
		centralLineNumStl = centralLineNumStl + count;
	}
	else {
		std::cout << "read dat error" << std::endl;
	}
}

void BrainDataManager::writeCenterPointsBinary()
{
	std::string pathName;
	if (mStlArray_h[0].beginROI < mStlArray_h[0].endROI) {
		pathName = std::to_string(mStlArray_h[0].beginROI) + "_" + std::to_string(mStlArray_h[0].endROI);// +"\\streamline\\test.dat";
	}
	else {
		pathName = std::to_string(mStlArray_h[0].endROI) + "_" + std::to_string(mStlArray_h[0].beginROI);
	}

	pathName = "D:\\Jun\\workspace\\brainfiber\\braindata\\preData\\" + pathName + "\\centralPoints\\" + peopleId + ".dat";
	char fileName[200];
	pathName.copy(fileName, pathName.length());
	char* tag;
	for (tag = fileName; *tag; tag++)
	{
		if (*tag == '\\')
		{
			char buf[1000], path[1000];
			strcpy(buf, fileName);
			buf[strlen(fileName) - strlen(tag) + 1] = NULL;
			strcpy(path, buf);
			if (access(path, 6) == -1)
			{
				mkdir(path);
			}
		}
	}
	std::ofstream outFile(pathName, std::ios::out | std::ios::binary);
	outFile.write((char*)& centralLineNumPoint, sizeof(centralLineNumPoint));
	for (int i = 0; i < centralLineNumPoint; i++) {
		outFile.write((char*)& centralLinePoints[i], sizeof(vec3f));
	}
	outFile.close();
}

void BrainDataManager::readCenterPointsBinary(int beginROI, int endROI)
{
	std::string pathName;
	if (beginROI < endROI) {
		pathName = std::to_string(beginROI) + "_" + std::to_string(endROI);// +"\\streamline\\test.dat";
	}
	else {
		pathName = std::to_string(endROI) + "_" + std::to_string(beginROI);
	}
	pathName = "D:\\Jun\\workspace\\brainfiber\\braindata\\preData\\" + pathName + "\\centralPoints\\" + peopleId + ".dat";
	vec3f p;
	int number;
	std::ifstream inFile(pathName, std::ios::in | std::ios::binary);
	int count = 0;
	if (inFile) {
		inFile.read((char*)& number, sizeof(number));
		centralLineNumPoint = number;
		centralLinePoints = (vec3f*)malloc(sizeof(vec3f) * number);
		while (inFile.read((char*)& p, sizeof(p))) {
			//int readedBytes = inFile.gcount();
			centralLinePoints[count] = p;
			count++;
		}
	}
	else {
		std::cout << "read dat error" << std::endl;
	}
}

void BrainDataManager::readMoreCenterPointsBinary(int beginROI, int endROI)
{
	std::string pathName;
	if (beginROI < endROI) {
		pathName = std::to_string(beginROI) + "_" + std::to_string(endROI);// +"\\streamline\\test.dat";
	}
	else {
		pathName = std::to_string(endROI) + "_" + std::to_string(beginROI);
	}
	pathName = "D:\\Jun\\workspace\\brainfiber\\braindata\\preData\\" + pathName + "\\centralPoints\\" + peopleId + ".dat";
	vec3f p;
	int number;
	std::ifstream inFile(pathName, std::ios::in | std::ios::binary);
	int count = 0;
	if (inFile) {
		inFile.read((char*)& number, sizeof(number));
		if (centralLineNumPoint == 0) {
			centralLinePoints = (vec3f*)malloc(sizeof(vec3f) * number);
		}
		else {
			/*
			vec3f* tmp = (vec3f*)malloc(sizeof(vec3f) * centralLineNumPoint);
			memcpy(tmp, centralLinePoints, centralLineNumPoint);
			delete(centralLinePoints);
			centralLinePoints = (vec3f*)malloc(sizeof(vec3f) * (number + centralLineNumPoint));
			memcpy(centralLinePoints, tmp, centralLineNumPoint);
			delete(tmp);
			*/
			centralLinePoints = (vec3f*)realloc(centralLinePoints, sizeof(vec3f) * (number + centralLineNumPoint));
			if (!centralLinePoints) {
				std::cout << "realloc error" << std::endl;
			}
		}
		while (inFile.read((char*)& p, sizeof(p))) {
			//int readedBytes = inFile.gcount();
			centralLinePoints[centralLineNumPoint + count] = p;
			count++;
		}
		centralLineNumPoint = centralLineNumPoint + count;
	}
	else {
		std::cout << "read dat error" << std::endl;
	}
}
