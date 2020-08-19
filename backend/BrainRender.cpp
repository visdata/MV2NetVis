#include "BrainRender.h"
#include "Sphere.h"
#include "ShaderUtilities.h"
#include <DataManager.h>
#include <MessageCenter.h>
#include <iostream>

#define SINGLE_WIN_SIZE 900

BrainRender::BrainRender(int x, int y, int w, int h, std::string name)
	:DisplayWidget(x, y, w, h, name),
	DataUser(),
	mRenderer1(NULL),
	mRenderer2(NULL),
	mVolumeRenderer(NULL),
	num(5),
	distance(5.0f),
	minNum(name + ".minimum number"),
	minDistance(name + ".minimum distance")
{
	for (int a = 0; a < 1; a++) {
		data1 = new BrainDataManager(a, 1, num, distance);
		data2 = new BrainDataManager(a, 2, num, distance);
		writeJson();
		delete(data1);
		delete(data2);
	}
	peopleID = 86;
	data1 = new BrainDataManager(peopleID, 1, num, distance);
	data2 = new BrainDataManager(peopleID, 2, num, distance);
	addMenuItem("Adjust Parameters");
	DataManager* manager = DataManager::sharedManager();
	manager->createInt(minNum, num, 0, 50, this, false);
	manager->createFloat(minDistance, distance, 0.0f, 5.0f, this, false);
}


//画某对ROI连接
BrainRender::BrainRender(int peopleID, int x, int y, int w, int h, std::string name)
	:DisplayWidget(x, y, w, h, name),
	DataUser(),
	mRenderer1(NULL),
	mRenderer2(NULL),
	mVolumeRenderer(NULL),
	num(5),
	distance(5.0f),
	peopleID(peopleID),
	minNum(name + ".minimum number"),
	minDistance(name + ".minimum distance")
{
	data1 = new BrainDataManager(peopleID, 1, num, distance);
	data2 = new BrainDataManager(peopleID, 2, num, distance);
	mROI = new ROI();
	calROI();
	//生成drawlist
	//连接数量max10%
	/*
	//maxConn = mROI->getMaxConnective();
	maxConn = mROI->getMaxConnectiveExceptZero();
	renderMaxROIConn();
	//minConn = mROI->getMinConnective();
	minConn = mROI->getMinConnectiveExceptZero();
	renderMinROIConn();
	*/
	/*
	maxConn = mROI->getMaxLength();
	minConn = mROI->getMinLength();
	renderMaxLength();
	renderMinLength();
	*/
	/*
	maxConn = mROI->getMaxCurvature();
	minConn = mROI->getMinCurvature();
	renderMaxCurvature();
	renderMinCurvature();
	*/
	
	maxConn = mROI->getMaxTorsion();
	minConn = mROI->getMinTorsion();
	renderMaxTorsion();
	renderMinTorsion();
	
	/*
	maxConn = mROI->getMaxEntropy();
	minConn = mROI->getMinEntropy();
	renderMaxEntropy();
	renderMinEntropy();
	*/
	//渲染某对roi连接
	//drawList1 = data1->getStlIdByROI(roi_start, roi_end);
	//drawList2 = data2->getStlIdByROI(roi_start, roi_end);
	addMenuItem("Adjust Parameters");
	DataManager* manager = DataManager::sharedManager();
	manager->createInt(minNum, num, 0, 50, this, false);
	manager->createFloat(minDistance, distance, 0.0f, 5.0f, this, false);
	//manager->createInterface(2, minNum, minDistance);
}

BrainRender::BrainRender(int *peopleId, int len, int beginROI, int endROI, int valueIndex ,int x, int y, int w, int h, std::string name)
	:DisplayWidget(x, y, w, h, name),
	DataUser(),
	mRenderer1(NULL),
	mRenderer2(NULL),
	mVolumeRenderer(NULL),
	num(5),
	distance(5.0f),
	peopleID(peopleID),
	minNum(name + ".minimum number"),
	minDistance(name + ".minimum distance"),
	valueIndex(valueIndex)
{
	//data1 = new BrainDataManager(peopleId, len, beginROI, endROI, num, distance);
	//write predata
	
	/*
	int tmp[1];
	for (int a = 0; a < 198; a++) {
		tmp[0] = a;
		data1 = new BrainDataManager(tmp, 1, 7, 33);
		delete(data1);
	}
	*/
	//41
	//for(int a = 0; a < 87; a++)
	//8-16 finish
	//12-26finish
	
	//int tmp[1];
	/*
	for (int i = 29; i < 71; i++) {
		for (int j = i + 1; j < 71; j++) {
			for (int a = 0; a < 198; a++) {
				tmp[0] = a;
				data1 = new BrainDataManager(tmp, 1, i, j);
				delete(data1);
			}
		}
	}
	*/
	//i=42,41finish
	//i=10
	/*
	int beROIs[15] = {42, 43, 45, 46, 48, 49, 50, 51, 52, 56, 58, 60, 61, 63, 64};
	int ROIs[4] = { 29,45,64,70 };
	for (int i = 10; i < 15; i++) {
		for (int j = 0; j < 4; j++) {
			int ber = beROIs[i];
			int endr = ROIs[j];
			if (ber > endr) {
				int tr = ber;
				ber = endr;
				endr = tr;
			}
			if (ber == endr) {
				break;
			}
			for (int a = 0; a < 198; a++) {
				tmp[0] = a;
				data1 = new BrainDataManager(tmp, 1, ber,endr);
				delete(data1);
			}
		}
	}
	*/
	/*
	for (int a = 0; a < 198; a++) {
		tmp[0] = a;
		data1 = new BrainDataManager(tmp, 1, 45, 64);
		delete(data1);
	}
	*/
	//data1 = new BrainDataManager(0);
	data1 = new BrainDataManager(peopleId, len, beginROI, endROI, num, distance);
	//data1 = new BrainDataManager(peopleId, len, beginROI, endROI);
	//mROI = new ROI();
	//calROI();
	/*
	addMenuItem("Adjust Parameters");
	DataManager* manager = DataManager::sharedManager();
	manager->createInt(minNum, num, 0, 50, this, false);
	manager->createFloat(minDistance, distance, 0.0f, 5.0f, this, false);
	*/
	//manager->createInterface(2, minNum, minDistance);

}

BrainRender::~BrainRender()
{
	delete(data1);
	//delete(data2);
}

void BrainRender::setRenderer(const int& num_faces, const float& radius)
{
	this->num_faces = num_faces;
	this->radius = radius;
	//setRenderer(data1->getSimplifycudaStreamlineRenderer(num_faces, radius, num, distance), data2->getSimplifycudaStreamlineRenderer(num_faces, radius, num, distance));
	setRenderer(data1->getcudaStreamlineRenderer(num_faces, radius), data2->getcudaStreamlineRenderer(num_faces, radius));
	mVolumeRenderer = data1->getVolumeRenderer(volumeWidth, volumeHeight, valueIndex);
}

void BrainRender::setOneRenderer(const int& num_faces, const float& radius)
{
	this->num_faces = num_faces;
	this->radius = radius;
	setRenderer(data1->getcudaStreamlineRendererWithoutCenter(num_faces, radius));
	//setRenderer(data1->getcudaCentralStreamline(num_faces, radius));
}

void BrainRender::setDataAndCenterLineRenderer(const int& num_faces, const float& radius)
{
	this->num_faces = num_faces;
	this->radius = radius;
	//setRenderer(data1->getcudaStreamlineRendererWithoutCenter(num_faces, radius), data1->getcudaCentralStreamline(num_faces, radius));
	makeCurrentGLContext();
	setRenderer(data1->getcudaStreamlineRendererWithoutCenter(num_faces, radius), data1->getcudaCentralStreamlineInCtl(num_faces, radius));

	if (mRenderer1 != NULL) {
		data1->calPointsValue();
		int index = valueIndex;
		if (valueIndex >= 16) index = index - 16 + 1;
		else index = 0;
		data1->calDensityData(index);
		//data1->calPvalueData();
		data1->readFileJson();
	}
	
}

void BrainRender::setmVolumeRenderer()
{
	mVolumeRenderer = data1->getVolumeRenderer(volumeWidth, volumeHeight, valueIndex);
}

void BrainRender::setmVolumeRenderer(UINT* volume)
{
	mVolumeRenderer = data1->getVolumeRenderer(volumeWidth, volumeHeight, volume, valueIndex);
}



void BrainRender::renderMaxROIConn()
{
	//get the drawlist
	drawList1max = data1->getPartOfStlIdByROI(maxConn.startId, maxConn.endId, 1.0f);
	drawList2max = data2->getPartOfStlIdByROI(maxConn.startId, maxConn.endId, 1.0f);
}

void BrainRender::renderMinROIConn()
{
	/*
	std::vector<int> a = data1->getPartOfStlIdByROI(minConn.startId, minConn.endId, 1.0f);
	while (!a.empty()) {
		drawList1.push_back(a.back());
		a.pop_back();
	}
	a = data2->getPartOfStlIdByROI(minConn.startId, minConn.endId, 1.0f);
	while (!a.empty()) {
		drawList2.push_back(a.back());
		a.pop_back();
	}
	*/
	drawList1min = data1->getPartOfStlIdByROI(minConn.startId, minConn.endId, 1.0f);
	drawList2min = data2->getPartOfStlIdByROI(minConn.startId, minConn.endId, 1.0f);
}

void BrainRender::renderMaxLength()
{
	drawList1max = data1->getPartOfStlIdByROIAndMaxLength(maxConn.startId, maxConn.endId, 1.0f);
	drawList2max = data2->getPartOfStlIdByROIAndMaxLength(maxConn.startId, maxConn.endId, 1.0f);
}

void BrainRender::renderMinLength()
{
	/*
	std::vector<int> a = data1->getPartOfStlIdByROIAndMinLength(minConn.startId, minConn.endId, 1.0f);
	while (!a.empty()) {
		drawList1.push_back(a.back());
		a.pop_back();
	}
	a = data2->getPartOfStlIdByROIAndMinLength(minConn.startId, minConn.endId, 1.0f);
	while (!a.empty()) {
		drawList2.push_back(a.back());
		a.pop_back();
	}
	*/
	drawList1min = data1->getPartOfStlIdByROIAndMinLength(minConn.startId, minConn.endId, 1.0f);
	drawList2min = data2->getPartOfStlIdByROIAndMinLength(minConn.startId, minConn.endId, 1.0f);
}

void BrainRender::renderMaxCurvature()
{
	drawList1max = data1->getPartOfStlIdByROIAndMaxCurvature(maxConn.startId, maxConn.endId, 1.0f);
	drawList2max = data2->getPartOfStlIdByROIAndMaxCurvature(maxConn.startId, maxConn.endId, 1.0f);
	//drawList1max = data1->getPartOfStlIdByROIAndMaxCurvature(25, 64, 1.0f);
	//drawList2max = data2->getPartOfStlIdByROIAndMaxCurvature(25, 64, 1.0f);
}

void BrainRender::renderMinCurvature()
{
	/*
	std::vector<int> a = data1->getPartOfStlIdByROIAndMinCurvature(minConn.startId, minConn.endId, 1.0f);
	while (!a.empty()) {
		drawList1.push_back(a.back());
		a.pop_back();
	}
	a = data2->getPartOfStlIdByROIAndMinCurvature(minConn.startId, minConn.endId, 1.0f);
	while (!a.empty()) {
		drawList2.push_back(a.back());
		a.pop_back();
	}
	*/
	drawList1min = data1->getPartOfStlIdByROIAndMinCurvature(minConn.startId, minConn.endId, 1.0f);
	drawList2min = data2->getPartOfStlIdByROIAndMinCurvature(minConn.startId, minConn.endId, 1.0f);
	//drawList1min = data1->getPartOfStlIdByROIAndMinCurvature(62, 70, 1.0f);
	//drawList2min = data2->getPartOfStlIdByROIAndMinCurvature(62, 70, 1.0f);
}

void BrainRender::renderMaxTorsion()
{
	drawList1max = data1->getPartOfStlIdByROIAndMaxTorsion(maxConn.startId, maxConn.endId, 1.0f);
	drawList2max = data2->getPartOfStlIdByROIAndMaxTorsion(maxConn.startId, maxConn.endId, 1.0f);
}

void BrainRender::renderMinTorsion()
{
	/*
	std::vector<int> a = data1->getPartOfStlIdByROIAndMinTorsion(minConn.startId, minConn.endId, 1.0f);
	while (!a.empty()) {
		drawList1.push_back(a.back());
		a.pop_back();
	}
	a = data2->getPartOfStlIdByROIAndMinTorsion(minConn.startId, minConn.endId, 1.0f);
	while (!a.empty()) {
		drawList2.push_back(a.back());
		a.pop_back();
	}
	*/
	drawList1min = data1->getPartOfStlIdByROIAndMinTorsion(minConn.startId, minConn.endId, 1.0f);
	drawList2min = data2->getPartOfStlIdByROIAndMinTorsion(minConn.startId, minConn.endId, 1.0f);
}

void BrainRender::renderMaxEntropy()
{
	drawList1max = data1->getPartOfStlIdByROIAndMaxEntropy(maxConn.startId, maxConn.endId, 1.0f);
	drawList2max = data2->getPartOfStlIdByROIAndMaxEntropy(maxConn.startId, maxConn.endId,1.0f);
}

void BrainRender::renderMinEntropy()
{
	/*
	std::vector<int> a = data1->getPartOfStlIdByROIAndMinEntropy(minConn.startId, minConn.endId, 1.0f);
	while (!a.empty()) {
		drawList1.push_back(a.back());
		a.pop_back();
	}
	a = data2->getPartOfStlIdByROIAndMinEntropy(minConn.startId, minConn.endId,1.0f);
	while (!a.empty()) {
		drawList2.push_back(a.back());
		a.pop_back();
	}
	*/
	drawList1min = data1->getPartOfStlIdByROIAndMinEntropy(minConn.startId, minConn.endId, 1.0f);
	drawList2min = data2->getPartOfStlIdByROIAndMinEntropy(minConn.startId, minConn.endId, 1.0f);
}

void BrainRender::updateRenderer()
{
	setRenderer(data1->getSimplifycudaStreamlineRenderer(num_faces, radius, num, distance), data2->getSimplifycudaStreamlineRenderer(num_faces, radius, num, distance));
}

void BrainRender::updateData(int* peopleId, int len, int beginROI, int endROI, int value)
{
	valueIndex = value;
	delete(data1);
	delete(mRenderer1);
	delete(mRenderer2);
	delete(mVolumeRenderer);
	data1 = new BrainDataManager(peopleId, len, beginROI, endROI, num, distance);
	makeCurrentGLContext();
	setRenderer(data1->getcudaStreamlineRendererWithoutCenter(num_faces, radius), data1->getcudaCentralStreamlineInCtl(num_faces, radius));
	//delete(mRenderer1);
	//delete(mRenderer2);
	data1->calPointsValue();
	int index = valueIndex;
	if (valueIndex >= 16) index = index - 16 + 1;
	else index = 0;
	data1->calDensityData(index);
	//delete(data1);
	//mVolumeRenderer = data1->getVolumeRenderer(volumeWidth, volumeHeight);
}

void BrainRender::changeIndex(int index)
{
	valueIndex = index;
}


void BrainRender::animate()
{
	mRenderer1->drawAllStreamline();
	mRenderer2->drawAllStreamline();
	mVolumeRenderer->draw();
}

void BrainRender::writeJson()
{
	if (data1->getPeopleId() != data2->getPeopleId()) {
		std::cout << "read wrong data" << std::endl;
	}
	Streamline* stl = data1->getStreamLine();
	vec3f* Points = data1->getPoints();
	Streamline* stl2 = data2->getStreamLine();
	vec3f* Points2 = data2->getPoints();
	//生成dom元素和分配器
	Document doc;
	doc.SetObject();
	Document::AllocatorType& allocator = doc.GetAllocator();
	//people id, 是否患病，其他属性可在这添加
	Value pID;
	pID.SetString(data1->getPeopleId().c_str(), allocator);
	doc.AddMember("id", pID, allocator);
	doc.AddMember("isPatient", 0, allocator);
	//ROI数组
	Value ROIarray(kArrayType);
	//对于每对ROI，找到以i出发以j结束的streamline
	for (int i = 0; i < 71; i++) {
		for (int j = 0; j < 71; j++) {
			Value ROI(kObjectType);
			ROI.AddMember("ROIStartId", i, allocator);
			ROI.AddMember("ROIEndId", j, allocator);
			int count = 0;
			Value streamlineArray(kArrayType);
			
			for (int k = 0; k < data1->getStlNum(); k++) {
				if (stl[k].beginROI == i && stl[k].endROI == j) {
					count++;
					Value streamline(kObjectType);
					//streamline的一些属性
					Value streamlineAttr(kObjectType);
					streamlineAttr.AddMember("length", stl[k].length, allocator);
					streamlineAttr.AddMember("pointsNum", stl[k].numPoint, allocator);
					streamlineAttr.AddMember("entropy", stl[k].entropy, allocator);
					streamlineAttr.AddMember("curvature", stl[k].curvature, allocator);
					streamlineAttr.AddMember("torsion", stl[k].torsion, allocator);
					streamline.AddMember("Attribute", streamlineAttr, allocator);
					//streamline包含的点
					/*
					Value pointArray(kArrayType);
					for (int l = 0; l < stl[k].numPoint; l++) {
						Value point(kObjectType);
						int index = stl[k].start + l;
						point.AddMember("x", Points[index].x, allocator);
						point.AddMember("y", Points[index].y, allocator);
						point.AddMember("z", Points[index].z, allocator);
						pointArray.PushBack(point, allocator);
					}
					streamline.AddMember("Points", pointArray, allocator);
					*/
					streamlineArray.PushBack(streamline, allocator);
				}
			}
			for (int k = 0; k < data2->getStlNum(); k++) {
				if (stl2[k].beginROI == i && stl2[k].endROI == j) {
					count++;
					Value streamline(kObjectType);
					//streamline的一些属性
					Value streamlineAttr(kObjectType);
					streamlineAttr.AddMember("length", stl2[k].length, allocator);
					streamlineAttr.AddMember("pointsNum", stl2[k].numPoint, allocator);
					streamlineAttr.AddMember("entropy", stl2[k].entropy, allocator);
					streamlineAttr.AddMember("curvature", stl2[k].curvature, allocator);
					streamlineAttr.AddMember("torsion", stl2[k].torsion, allocator);
					streamline.AddMember("Attribute", streamlineAttr, allocator);
					//streamline包含的点
					/*
					Value pointArray(kArrayType);
					for (int l = 0; l < stl2[k].numPoint; l++) {
						Value point(kObjectType);
						int index = stl2[k].start + l;
						point.AddMember("x", Points2[index].x, allocator);
						point.AddMember("y", Points2[index].y, allocator);
						point.AddMember("z", Points2[index].z, allocator);
						pointArray.PushBack(point, allocator);
					}
					streamline.AddMember("Points", pointArray, allocator);
					*/
					streamlineArray.PushBack(streamline, allocator);
				}
			}
			ROI.AddMember("streamlineArray", streamlineArray, allocator);
			ROI.AddMember("StreamlineNum", count, allocator);
			ROIarray.PushBack(ROI, allocator);
		}
	}
	doc.AddMember("ROIs", ROIarray, allocator);

	StringBuffer s;
	Writer<StringBuffer> writer(s);
	doc.Accept(writer);

	//写入json
	std::string jsonPath = "D:\\Jun\\workspace\\brainfiber\\braindata\\JSON5\\" + data1->getPeopleId() + ".json";
	std::ofstream outfile;
	outfile.open(jsonPath.c_str());
	if (!outfile.fail()) {
		outfile << s.GetString();
		outfile.close();
	}
}


void BrainRender::menuCallback(const std::string& message)
{
	if (message == "Adjust Parameters") {
		DataManager* manager = DataManager::sharedManager();
		manager->createInterface(2, minNum, minDistance);
	}
	else {
		MessageCenter* center = MessageCenter::sharedCenter();
		center->processMessage(message, getName());
		updateRenderer();
		redraw();
	}
}

void BrainRender::onDataItemChanged(const std::string& name)
{
	std::string& widgetName = getName();
	DataManager* manager = DataManager::sharedManager();
	bool bSuccess;

	if (name == minNum) {
		num = manager->getIntValue(name, bSuccess);
	}
	else if (name == minDistance) {
		distance = manager->getFloatValue(name, bSuccess);
	}
	updateRenderer();
	redraw();
}

void BrainRender::calROI()
{
	ROI* ROI1 = data1->getmROI();
	ROI* ROI2 = data2->getmROI();
	for (int i = 0; i < ROI_NUM; i++) {
		for (int j = 0; j < ROI_NUM; j++) {
			mROI->connective[i][j] = ROI1->connective[i][j] + ROI2->connective[i][j];
			mROI->avCurvature[i][j] = ROI1->avCurvature[i][j] + ROI2->avCurvature[i][j];
			mROI->avEntropy[i][j] = ROI1->avEntropy[i][j] + ROI2->avEntropy[i][j];
			mROI->avTorsion[i][j] = ROI1->avTorsion[i][j] + ROI2->avTorsion[i][j];
			mROI->avLength[i][j] = ROI1->avLength[i][j] + ROI2->avLength[i][j];
			if (i != j) {
				//连接变成无方向的
				mROI->connective[i][j] += ROI1->connective[j][i] + ROI2->connective[j][i];
				mROI->avCurvature[i][j] += ROI1->avCurvature[j][i] + ROI2->avCurvature[j][i];
				mROI->avEntropy[i][j] += ROI1->avEntropy[j][i] + ROI2->avEntropy[j][i];
				mROI->avTorsion[i][j] += ROI1->avTorsion[j][i] + ROI2->avTorsion[j][i];
				mROI->avLength[i][j] += ROI1->avLength[j][i] + ROI2->avLength[j][i];
			}
		}
	}
	for (int i = 0; i < ROI_NUM; i++) {
		for (int j = 0; j < ROI_NUM; j++) {
			if (mROI->connective[i][j] != 0) {
				mROI->avCurvature[i][j] = mROI->avCurvature[i][j] / mROI->connective[i][j];
				mROI->avEntropy[i][j] = mROI->avEntropy[i][j] / mROI->connective[i][j];
				mROI->avTorsion[i][j] = mROI->avTorsion[i][j] / mROI->connective[i][j];
				mROI->avLength[i][j] = mROI->avLength[i][j] / mROI->connective[i][j];
			}
		}
	}
	//calculate 0 to any
	int sum = 0;
	for (int i = 0; i < ROI_NUM; i++) {
		sum += mROI->connective[0][i];
	}

	//printf roi infomation
	std::cout << "connective: " << mROI->connective[roi_start][roi_end] << std::endl;
	std::cout << "length:     " << mROI->avLength[roi_start][roi_end] << std::endl;
	std::cout << "curvature:  " << mROI->avCurvature[roi_start][roi_end] << std::endl;
	std::cout << "torsion:    " << mROI->avTorsion[roi_start][roi_end] << std::endl;
	std::cout << "entropy:    " << mROI->avEntropy[roi_start][roi_end] << std::endl;

 }

void BrainRender::init()
{
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	float ambient = 0.5f;
	float diffuse = 0.5f;
	float specular = 0.5f;

	float lightDist = 500.0f;

	GLfloat light0_ambient[4] = { ambient, ambient, ambient, 1.0f };
	GLfloat light0_diffuse[4] = { diffuse, diffuse, diffuse, 1.0f };
	GLfloat light0_specular[4] = { specular, specular, specular, 1.0f };
	GLfloat light0_position[4] = { 0.0f, 0.0f, lightDist, 1.0f };
	GLfloat light0_direction[3] = { -1.0f, -1.0f, -1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light0_direction);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);

	GLfloat mat_ambient[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat mat_diffuse[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat mat_specular[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, 4.0f);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
}

void BrainRender::display()
{
	makeCurrentGLContext();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//与glPopAttrib连用分别代表属性入栈，出栈恢复
	glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_CURRENT_BIT);

	//打开灯光
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	//声明接下来的工作GL_PROJECTION投影、GL_MODELVIEW模型视图，GL_TEXTURE纹理
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0f, 1.0f, 1.0f, 1000.0f); //(视觉大小,窗口纵横比,近处裁面到眼睛距离,远处裁面到眼睛距离)
	//画个物体看起来是3D的
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	float cam_dist = 100.0f + getCameraDistance() / 64.0f;
	//设置视图方向（y朝上，z向前，x向右？
	gluLookAt(0.0f, 0.0f, cam_dist, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	vec2f translate = getTranslation();
	//平移函数
	//glTranslatef(-100.0f, -100.0f, 0);
	glTranslatef(translate.x * 0.3f * cam_dist, translate.y * 0.3f * cam_dist, 0.0f);
	//旋转函数
	float* rotMat = getRotationMatrix();
	float* inVrotMat = getinvRotationMatrix();
	float* quat = getRotationQuat();
	float3 angles;
	float x = quat[0];
	float y = quat[1];
	float z = quat[2];
	float w = quat[3];
	float a = w * w + x * x + y * y + z * z;
	//x-axis rotation
	double sinr_cosp = 2 * (w * x + y * z);
	double cosr_cosp = 1 - 2 * (x * x + y * y);
	angles.x = atan2(sinr_cosp, cosr_cosp);
	//y-axis rotation
	double sinp = 2 * (w * y - z * x);
	if (abs(sinp) >= 1) {
		angles.y = copysign(PI / 2, sinp);
	}
	else {
		angles.y = asin(sinp);
	}
	//z-axis rotation
	double siny_cosp = 2 * (w * z + x * y);
	double cosy_cosp = 1 - 2 * (y * y + z * z);
	angles.z = atan2(siny_cosp, cosy_cosp);

	angles.x = angles.x * 180 / PI;
	angles.y = angles.y * 180 / PI;
	angles.z = angles.z * 180 / PI;
	/*
	//float b1 = atan2(2 * (w * x + y * z), (1 - 2 * (x * x + y * y))) * 180 / PI;
	float b1 = atan2(2 * (w * x + y * z), w * w - x * x - y * y + z * z) * 180 / PI;
	float b2 = asin(2 * (w * y - z * x)) * 360 / PI ;
	//float b3 = atan2(2 * (w * z + x * y), (1 - 2 * (z * z + y * y))) * 180 / PI;
	float b3 = atan2(2 * (w * z + x * y), w * w + x * x - y * y - z * z) * 180 / PI;
	*/
	glMultMatrixf(rotMat);
	
	//直接画线
	//
	//mRenderer2->drawSingleStreamline(0, colorMax);
	//画中心线
	if(mRenderer2!=NULL) mRenderer2->drawAllStreamline();
	mRenderer1->drawAllStreamline();
	/*
	mRenderer1->drawListStreamline(drawList1max,colorMax);
	mRenderer1->drawListStreamline(drawList1min,colorMin);
	mRenderer2->drawListStreamline(drawList2max,colorMax);
	mRenderer2->drawListStreamline(drawList2min, colorMin);
	*/
	//render某对roi
	//mRenderer1->drawListStreamline(drawList1, colorMin);
	//mRenderer2->drawListStreamline(drawList2, colorMin);

	//mRenderer1->putAllStreamline();
	//mRenderer2->putAllStreamline();
	
	//for volume render
	
	//                                                                                                                                                                                                                                                                                                                                                
	GLfloat depth[900*900]; //取值范围为0-1
	glReadPixels(0, 0, SINGLE_WIN_SIZE, SINGLE_WIN_SIZE, GL_DEPTH_COMPONENT, GL_FLOAT, depth);

	//获取变换矩阵
	GLfloat modelView[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelView);

	//获取投影矩阵
	GLfloat project[16];
	glGetFloatv(GL_PROJECTION_MATRIX, project);
	glPopMatrix();

	glDepthMask(GL_FALSE);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluOrtho2D(0, 900, 0, 900);
	gluPerspective(30.0f, 1.0f, 1.0f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, cam_dist, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//根据depth做volume render

	//mVolumeRenderer->setMatrix(modelView);
	//mVolumeRenderer->renderByDepth(depth);
	//mVolumeRenderer->draw();

	//mVolumeRenderer->display(angles, make_float3(-translate.x * 0.3f * cam_dist, -translate.y * 0.3f * cam_dist, -2.0f), depth, modelView, project);
	rotMat[12] = 0;
	rotMat[13] = 0;
	rotMat[14] = 0;
	//mVolumeRenderer->display(rotMat, make_float3(-translate.x * 0.3f * cam_dist, -translate.y * 0.3f * cam_dist, -2.0f), depth, modelView, project);
	if (mVolumeRenderer != NULL) {
		float* val;
		if (valueIndex == 0) val = data1->getCurvatue();
		else if (valueIndex == 1) val = data1->getEntropy();
		else if (valueIndex == 2) val = data1->getTorsion();
		else if (valueIndex == 3) val = data1->getLength();
		else if (valueIndex == 4) val = data1->getFA();
		else if (valueIndex == 5) val = data1->getMD();
		else if (valueIndex == 6) val = data1->getRD();
		else if (valueIndex == 7) val = data1->getAXD();
		
		else if (valueIndex == 8) val = data1->getstdCurvature();
		else if (valueIndex == 9) val = data1->getstdEntropy();
		else if (valueIndex == 10) val = data1->getstdTorsion();
		else if (valueIndex == 11) val = data1->getstdLength();
		else if (valueIndex == 12) val = data1->getstdFA();
		else if (valueIndex == 13) val = data1->getstdMD();
		else if (valueIndex == 14) val = data1->getstdRD();
		else if (valueIndex == 15) val = data1->getstdAXD();
		else val = data1->getalvolumeCurvature();
		mVolumeRenderer->display(rotMat, make_float3(-translate.x * 0.3f * cam_dist, -translate.y * 0.3f * cam_dist, -2.0f), val, data1->getValueSize());
	}
	//mVolumeRenderer->display(inVrotMat, depth, modelView, project);
	GLfloat modelView3[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelView3);
	GLfloat project2[16];
	glGetFloatv(GL_PROJECTION_MATRIX, project2);
	glPopMatrix();

	glDepthMask(GL_TRUE);
	
	glPopAttrib();
}



