import json
import random
import numpy as np
from scipy import stats
import math
import warnings
from sklearn.linear_model import Lasso, LogisticRegression, LassoCV, ElasticNetCV, RidgeClassifierCV, LogisticRegressionCV
from sklearn.model_selection import cross_val_score
from sklearn.model_selection import LeaveOneOut
from sklearn.cluster import DBSCAN, KMeans
from sklearn.preprocessing import StandardScaler, MinMaxScaler
import pandas as pd
import os
from scipy.stats import norm
import subprocess
import time

warnings.filterwarnings('ignore')

# fn2为用户信息文件

fn2 = 'subject_all_198.txt'
feature_name = ['FA', 'MD', 'AxD', 'RD', 'Strength']


# 获取每类用户的数目
def getNum(subject_list):
	femaleAD = [0]*4
	femaleHealthy = [0]*4
	maleAD = [0]*4
	maleHealthy = [0]*4
	femaleAD1 = [0]*4
	maleAD1 = [0]*4
	femaleAD2 = [0]*4
	maleAD2 = [0]*4
	for x in range(len(subject_list)):
		if int(subject_list[x][1]) == 0:
			if int(subject_list[x][2]) == 1:
				age = float(subject_list[x][3])
				if age >= 90:
					age = 89
				maleHealthy[int((age-50)/10)] += 1
			else:
				age = float(subject_list[x][3])
				if age >= 90:
					age = 89
				femaleHealthy[int((age-50)/10)] += 1
		elif int(subject_list[x][1]) == 1:
			if int(subject_list[x][2]) == 1:
				age = float(subject_list[x][3])
				if age >= 90:
					age = 89
				maleAD1[int((age-50)/10)] += 1
			else:
				age = float(subject_list[x][3])
				if age >= 90:
					age = 89
				femaleAD1[int((age-50)/10)] += 1
		elif int(subject_list[x][1]) == 2:
			if int(subject_list[x][2]) == 1:
				age = float(subject_list[x][3])
				if age >= 90:
					age = 89
				maleAD2[int((age-50)/10)] += 1
			else:
				age = float(subject_list[x][3])
				if age >= 90:
					age = 89
				femaleAD2[int((age-50)/10)] += 1
		else:
			if int(subject_list[x][2]) == 1:
				age = float(subject_list[x][3])
				if age >= 90:
					age = 89
				maleAD[int((age-50)/10)] += 1
			else:
				age = float(subject_list[x][3])
				if age >= 90:
					age = 89
				femaleAD[int((age-50)/10)] += 1
	return femaleAD, femaleHealthy, maleAD, maleHealthy, femaleAD1, maleAD1, femaleAD2, maleAD2



# 根据用户属性更新sunburst
def updateSunburstForFMri(propertiesArr):
	# print(propertiesArr)
	res_dict = {}
	PROPERTIES = json.loads(propertiesArr)

	subject_list = [x.rstrip().split(' ') for x in open(fn2, 'r')]
	subject_name = []
	label = []
	gender = []
	agegroup = []
	for s in subject_list:
		subject_name.append(s[0])
		label.append(int(s[1]))
		if int(s[2]) == 1:
			gender.append(-1)
		else:
			gender.append(1)
		if float(s[3]) >= 50 and float(s[3]) < 60:
			agegroup.append(0)
		elif float(s[3]) >= 60 and float(s[3]) < 70:
			agegroup.append(1)
		elif float(s[3]) >= 70 and float(s[3]) < 80:
			agegroup.append(2)
		elif float(s[3]) >= 80:
			agegroup.append(3)
	label = np.array(label)
	gender = np.array(gender)
	agegroup = np.array(agegroup)
	indices = np.arange(len(subject_name))

	
	res_dict['code'] = 0
	res_dict['data'] = {'name':'all', 'children':[]}
	res_dict['propertiesArr'] = []
	if len(PROPERTIES) == 1:
		item = PROPERTIES[0]
		res_dict['propertiesArr'].append(item['message'])
		indices_mask = np.array([True]*len(subject_name))
		for chd in item['children']:
			indices_mask1 = []
			if chd == 'Healthy':
				indices_mask1 = indices_mask & (label == 0)
			elif chd == 'eMCI':
				indices_mask1 = indices_mask & (label == 1)
			elif chd == 'lMCI':
				indices_mask1 = indices_mask & (label == 2)
			elif chd == 'AD':
				indices_mask1 = indices_mask & (label == 3)
			elif chd == 'Male':
				indices_mask1 = indices_mask & (gender < 0)
			elif chd == 'Female':
				indices_mask1 = indices_mask & (gender > 0)
			elif chd == '50s':
				indices_mask1 = indices_mask & (agegroup == 0)
			elif chd == '60s':
				indices_mask1 = indices_mask & (agegroup == 1)
			elif chd == '70s':
				indices_mask1 = indices_mask & (agegroup == 2)
			elif chd == '80s':
				indices_mask1 = indices_mask & (agegroup == 3)
			elif chd == 'Unhealthy':
				indices_mask1 = indices_mask & ((label == 1) | (label == 2) | (label == 3))
			temp_dict = {'name':chd, 'size':len(indices[indices_mask1])}
			res_dict['data']['children'].append(temp_dict)
	elif len(PROPERTIES) == 2:
		item1 = PROPERTIES[0]
		item2 = PROPERTIES[1]
		res_dict['propertiesArr'].append(item1['message'])
		res_dict['propertiesArr'].append(item2['message'])
		indices_mask = np.array([True]*len(subject_name))
		for chd1 in item1['children']:
			indices_mask1 = []
			if chd1 == 'Healthy':
				indices_mask1 = indices_mask & (label == 0)
			elif chd1 == 'eMCI':
				indices_mask1 = indices_mask & (label == 1)
			elif chd1 == 'lMCI':
				indices_mask1 = indices_mask & (label == 2)
			elif chd1 == 'AD':
				indices_mask1 = indices_mask & (label == 3)
			elif chd1 == 'Male':
				indices_mask1 = indices_mask & (gender < 0)
			elif chd1 == 'Female':
				indices_mask1 = indices_mask & (gender > 0)
			elif chd1 == '50s':
				indices_mask1 = indices_mask & (agegroup == 0)
			elif chd1 == '60s':
				indices_mask1 = indices_mask & (agegroup == 1)
			elif chd1 == '70s':
				indices_mask1 = indices_mask & (agegroup == 2)
			elif chd1 == '80s':
				indices_mask1 = indices_mask & (agegroup == 3)
			elif chd1 == 'Unhealthy':
				indices_mask1 = indices_mask & ((label == 1) | (label == 2) | (label == 3))
			temp_dict1 = {'name':chd1, 'children':[]}
			for chd2 in item2['children']:
				indices_mask2 = []
				if chd2 == 'Healthy':
					indices_mask2 = indices_mask1 & (label == 0)
				elif chd2 == 'eMCI':
					indices_mask2 = indices_mask1 & (label == 1)
				elif chd2 == 'lMCI':
					indices_mask2 = indices_mask1 & (label == 2)
				elif chd2 == 'AD':
					indices_mask2 = indices_mask1 & (label == 3)
				elif chd2 == 'Male':
					indices_mask2 = indices_mask1 & (gender < 0)
				elif chd2 == 'Female':
					indices_mask2 = indices_mask1 & (gender > 0)
				elif chd2 == '50s':
					indices_mask2 = indices_mask1 & (agegroup == 0)
				elif chd2 == '60s':
					indices_mask2 = indices_mask1 & (agegroup == 1)
				elif chd2 == '70s':
					indices_mask2 = indices_mask1 & (agegroup == 2)
				elif chd2 == '80s':
					indices_mask2 = indices_mask1 & (agegroup == 3)
				elif chd2 == 'Unhealthy':
					indices_mask2 = indices_mask1 & ((label == 1) | (label == 2) | (label == 3))
				temp_dict2 = {'name':chd2, 'size':len(indices[indices_mask2])}
				temp_dict1['children'].append(temp_dict2)
			res_dict['data']['children'].append(temp_dict1)
	elif len(PROPERTIES) == 3:
		item1 = PROPERTIES[0]
		item2 = PROPERTIES[1]
		item3 = PROPERTIES[2]
		res_dict['propertiesArr'].append(item1['message'])
		res_dict['propertiesArr'].append(item2['message'])
		res_dict['propertiesArr'].append(item3['message'])
		indices_mask = np.array([True]*len(subject_name))
		for chd1 in item1['children']:
			indices_mask1 = []
			if chd1 == 'Healthy':
				indices_mask1 = indices_mask & (label == 0)
			elif chd1 == 'eMCI':
				indices_mask1 = indices_mask & (label == 1)
			elif chd1 == 'lMCI':
				indices_mask1 = indices_mask & (label == 2)
			elif chd1 == 'AD':
				indices_mask1 = indices_mask & (label == 3)
			elif chd1 == 'Male':
				indices_mask1 = indices_mask & (gender < 0)
			elif chd1 == 'Female':
				indices_mask1 = indices_mask & (gender > 0)
			elif chd1 == '50s':
				indices_mask1 = indices_mask & (agegroup == 0)
			elif chd1 == '60s':
				indices_mask1 = indices_mask & (agegroup == 1)
			elif chd1 == '70s':
				indices_mask1 = indices_mask & (agegroup == 2)
			elif chd1 == '80s':
				indices_mask1 = indices_mask & (agegroup == 3)
			elif chd1 == 'Unhealthy':
				indices_mask1 = indices_mask & ((label == 1) | (label == 2) | (label == 3))
			temp_dict1 = {'name':chd1, 'children':[]}
			for chd2 in item2['children']:
				indices_mask2 = []
				if chd2 == 'Healthy':
					indices_mask2 = indices_mask1 & (label == 0)
				elif chd2 == 'eMCI':
					indices_mask2 = indices_mask1 & (label == 1)
				elif chd2 == 'lMCI':
					indices_mask2 = indices_mask1 & (label == 2)
				elif chd2 == 'AD':
					indices_mask2 = indices_mask1 & (label == 3)
				elif chd2 == 'Male':
					indices_mask2 = indices_mask1 & (gender < 0)
				elif chd2 == 'Female':
					indices_mask2 = indices_mask1 & (gender > 0)
				elif chd2 == '50s':
					indices_mask2 = indices_mask1 & (agegroup == 0)
				elif chd2 == '60s':
					indices_mask2 = indices_mask1 & (agegroup == 1)
				elif chd2 == '70s':
					indices_mask2 = indices_mask1 & (agegroup == 2)
				elif chd2 == '80s':
					indices_mask2 = indices_mask1 & (agegroup == 3)
				elif chd2 == 'Unhealthy':
					indices_mask2 = indices_mask1 & ((label == 1) | (label == 2) | (label == 3))
				temp_dict2 = {'name':chd2, 'children':[]}
				for chd3 in item3['children']:
					indices_mask3 = []
					if chd3 == 'Healthy':
						indices_mask3 = indices_mask2 & (label == 0)
					elif chd3 == 'eMCI':
						indices_mask3 = indices_mask2 & (label == 1)
					elif chd3 == 'lMCI':
						indices_mask3 = indices_mask2 & (label == 2)
					elif chd3 == 'AD':
						indices_mask3 = indices_mask2 & (label == 3)
					elif chd3 == 'Male':
						indices_mask3 = indices_mask2 & (gender < 0)
					elif chd3 == 'Female':
						indices_mask3 = indices_mask2 & (gender > 0)
					elif chd3 == '50s':
						indices_mask3 = indices_mask2 & (agegroup == 0)
					elif chd3 == '60s':
						indices_mask3 = indices_mask2 & (agegroup == 1)
					elif chd3 == '70s':
						indices_mask3 = indices_mask2 & (agegroup == 2)
					elif chd3 == '80s':
						indices_mask3 = indices_mask2 & (agegroup == 3)
					elif chd3 == 'Unhealthy':
						indices_mask3 = indices_mask2 & ((label == 1) | (label == 2) | (label == 3))
					temp_dict3 = {'name':chd3, 'size':len(indices[indices_mask3])}
					temp_dict2['children'].append(temp_dict3)
				temp_dict1['children'].append(temp_dict2)
			res_dict['data']['children'].append(temp_dict1)
	else:
		print('propertiesArr is wrong: ', PROPERTIES)
	res_dict['message'] = '获取成功'
	print(res_dict)
	return res_dict
		

# 根据用户组返回p-value
def getPValueSingleForFMri(groupid, subjectsArr):
	start_time = time.time()
	print(subjectsArr)
	# subjectsArr = json.loads(subjectsArr)
	SUBJECTS = ["", ""]
	SUBJECTS[int(groupid)-1] = subjectsArr
	normVal = []
	with open('normalValue_fmri_198.json', 'r') as ifile:
		normVal = json.load(ifile)
	subject_list = [x.rstrip().split(' ') for x in open(fn2, 'r')]
	subject_name = []
	label = []
	gender = []
	agegroup = []
	childrenNum = len(subjectsArr.split('-'))
	children1 = subjectsArr.split('-')
	age = -1
	for chd in children1:
		if chd == '50s':
			age = 0
		elif chd == '60s':
			age = 1
		elif chd == '70s':
			age = 2
		elif chd == '80s':
			age = 3
	print('Age: ', age)
	for s in subject_list:
		subject_name.append(s[0])
		label.append(int(s[1]))
		if int(s[2]) == 1:
			gender.append(-1)
		else:
			gender.append(1)
		if float(s[3]) >= 50 and float(s[3]) < 60:
			agegroup.append(0)
		elif float(s[3]) >= 60 and float(s[3]) < 70:
			agegroup.append(1)
		elif float(s[3]) >= 70 and float(s[3]) < 80:
			agegroup.append(2)
		elif float(s[3]) >= 80:
			agegroup.append(3)

	label = np.array(label)
	gender = np.array(gender)
	agegroup = np.array(agegroup)
	indices = np.arange(len(subject_name))
	
	g1_indices = []
	g2_indices = []
	indices_mask = np.array([True]*len(subject_name))
	for chd in children1:
		if chd == 'all':
			continue
		if chd == 'Healthy':
			indices_mask = indices_mask & (label == 0)
		elif chd == 'eMCI':
			indices_mask = indices_mask & (label == 1)
		elif chd == 'lMCI':
			indices_mask = indices_mask & (label == 2)
		elif chd == 'AD':
			indices_mask = indices_mask & (label == 3)
		elif chd == 'Male':
			indices_mask = indices_mask & (gender < 0)
		elif chd == 'Female':
			indices_mask = indices_mask & (gender > 0 )
		elif chd == '50s':
			indices_mask = indices_mask & (agegroup == 0)
		elif chd == '60s':
			indices_mask = indices_mask & (agegroup == 1)
		elif chd == '70s':
			indices_mask = indices_mask & (agegroup == 2)
		elif chd == '80s':
			indices_mask = indices_mask & (agegroup == 3)
		elif chd == 'Unhealthy':
			indices_mask = indices_mask & ((label == 1) | (label == 2) | (label == 3))
		else:
			print('subjectsArr is wrong: ', subjectsArr)
	if int(groupid) == 1:
		g1_indices = indices[indices_mask]
	elif int(groupid) == 2:
		g2_indices = indices[indices_mask]
	else:
		print('groupid is wrong: ', groupid)

	print('g1: ',g1_indices)
	print('g2: ',g2_indices)
		
	group1data = []
	group2data = []
	if int(groupid) == 1: 
		for i in g1_indices:
			name = subject_name[i]
			nv = normVal[i]
			part = []
			for j in range(int(len(nv)/5)):
				li = j*5
				# [Strength, FA, MD, RD, AXD]
				line = [float(nv[li+4]),float(nv[li]),float(nv[li+1]),float(nv[li+3]),float(nv[li+2]),name,j]
				part.append(line)
			group1data.append(part)
	else:
		for i in g2_indices:
			name = subject_name[i]
			nv = normVal[i]
			part = []
			for j in range(int(len(nv)/5)):
				li = j*5
				# [Strength, FA, MD, RD, AXD]
				line = [float(nv[li+4]),float(nv[li]),float(nv[li+1]),float(nv[li+3]),float(nv[li+2]),name,j]
				part.append(line)
			group2data.append(part)
	
	print('getPvalueTime: ', time.time()-start_time)
	if int(groupid) == 1:
		return {"code":0,"data": group1data,"subjectsArr":subjectsArr, 'subjectList':g1_indices.tolist(), "message":"\u83b7\u53d6\u6210\u529f"}
	else:
		return {"code":0,"data": group2data,"subjectsArr":subjectsArr, 'subjectList':g2_indices.tolist(), "message":"\u83b7\u53d6\u6210\u529f"}

# ks-test
def kstest(data):
	return stats.kstest(data, 'norm', (np.mean(data), np.std(data)))

# t-test
def ttest(data1, data2, testIndex):
	# scores = stats.levene(data1,data2)
	# print(scores)
	# if scores.pvalue > 0.05:
	if testIndex == 0:
		scores = stats.ttest_ind(data1,data2,nan_policy='omit')
	else:
		scores = stats.ttest_ind(data1,data2,equal_var=False,nan_policy='omit')
	# print(scores)
	return scores

# mannWhitneyU-test
def mannWhitneyU(biggerData, lessData):
	return stats.mannwhitneyu(biggerData, lessData, alternative='greater')

# 计算两组数据的p-value
# data1：组1，data2：组2，testIndex：0和1为两种不同的t-test，2为mannWhitneyU-test
def groupComparison(data1, data2, testIndex):
	signal = 0
	if len(data1) == 0 or len(data2) == 0:
		return 1.0, 0, signal
	p = 0   
	if int(testIndex) == 0 or int(testIndex) == 1:
		testResult = ttest(data1, data2, int(testIndex))
		p = testResult.pvalue 
		signal = 1
		if math.isnan(p):
			p = 1.0
			signal = 0
	else:
		if  len(np.intersect1d(data1, data2))==len(data1) or len(np.intersect1d(data1, data2))==len(data2):
			return 1.0, 0, signal
		testResult = mannWhitneyU(data1, data2)
		p = testResult.pvalue
		if p > 0.5:
			testResult = mannWhitneyU(data2, data1)
			p = testResult.pvalue
		p = p 
		signal = 1
		if math.isnan(p):
			p = 1.0
			signal = 0
	return p, data1.mean() - data2.mean(), signal



# 根据正常值过滤数据
def filter_function(data, thes):
	normal_value = []
	with open('normalValue_fmri_198.json', 'r') as ifile:
		normal_value = json.load(ifile)
	normal_value = np.array(normal_value)
	tempArr = []
	for idx in range(5*2485):
		f = data[:,idx]
		selected_f = normal_value[:, idx]>thes
		filted_f = [f[x] if selected_f[x] else 0 for x in range(data.shape[0])]
		tempArr.append(filted_f)
	tempArr = np.array(tempArr)
	data = tempArr.T
	return data

# 根据阈值过滤数据
def filter_function_grl(data, type_id):
	threshold = [0.05, 0.0005, 0.0005, 0.0005, 4]
	thes = threshold[type_id]
	tempArr = []
	for idx in range(data.shape[-1]):
		f = data[:,idx]
		filted_f = [f[x] if f[x] > thes else f.mean() for x in range(data.shape[0])]
		tempArr.append(filted_f)
	tempArr = np.array(tempArr)
	data = tempArr.T
	return data

# 计算ROI的Z-score
def getZScore(pArr, pNum, quality):
	if len(pArr) != 2485:
		print(len(pArr),'length is wrong')
		return [0]*70
	z_scores = []
	for index in range(len(pArr)):
		if pArr[index] == 0:
			z_scores.append(0)
		else:
			z_scores.append(norm.ppf(1 - pArr[index]))
	w_z_scores = []
	matrix1 = np.zeros((70,70))
	matrix2 = np.zeros((70,70))
	k = 0
	for i in range(70):
		for j in range(i, 70):
			if k in quality:
				matrix1[i][j] = z_scores[k]
				matrix2[i][j] = pNum[k]
			else:
				matrix1[i][j] = 0
				matrix2[i][j] = 0
			k += 1
	for i in range(70):
		for j in range(0, i):
			matrix1[i][j] = matrix1[j][i]
			matrix2[i][j] = matrix2[j][i]
	for i in range(70):
		z = 0.0
		count = 0
		for j in range(70):
			z += matrix1[i][j]*matrix2[i][j]
			count += matrix2[i][j] ** 2
		if count == 0:
			print(count, 'count is zero')
		w_z_scores.append(z/math.sqrt(count))
	return w_z_scores

	
	
# 单独返回feature view的pvalue
def getFeatureViewPValueSingleForFMri(featureIndex_, pvalueThreshold, validThreshold, testIndex, groupName_, subjectList_):
	start_time = time.time()
	pvalueThreshold = float(pvalueThreshold) / 100
	validThreshold = float(validThreshold) / 100
	featureIndex = int(featureIndex_)

	# featureIndex: 0=strength,1=FA,2=MD,3=RD,4=AxD
	if featureIndex == 0:
		featureIndex = 4
	elif featureIndex == 1:
		featureIndex = 0
	elif featureIndex == 2:
		featureIndex = 1
	elif featureIndex == 3:
		featureIndex = 3
	else:
		featureIndex = 2
	
	normVal = []
	with open('normalValue_fmri_198.json', 'r') as ifile:
		normVal = json.load(ifile)
	normVal = np.array(normVal)
	data = []
	with open('all_feature_fmri_198.json', 'r') as ifile:
		data = json.load(ifile)
	data = np.array(data)
	subject_list = [x.rstrip().split(' ') for x in open(fn2, 'r')]
	
	pvalueArr = [[],[],[],[],[]]
	originPvalueArr = [[],[],[],[],[]]
	featureArr1 = [[],[],[],[],[]]
	featureArr2 = [[],[],[],[],[]]
	featureArrTotal = [[],[],[],[],[]]
	featureArrG1 = [[],[],[],[],[]]
	featureArrG2 = [[],[],[],[],[]]
	peopleNumArr = [[],[],[],[],[]]
	peopleNumArrG1 = [[],[],[],[],[]]
	peopleNumArrG2 = [[],[],[],[],[]]
	threshold = [4,0.4,0.02,0.001,0.0004]
	res_dict = {}
	res_dict['code'] = 0
	res_dict['data'] = {}
	
	subjectList = json.loads(subjectList_)
	g1_indices = subjectList['groupOneList']
	g2_indices = subjectList['groupTwoList']
	
	
	quality = [[],[],[],[],[]]

	for index_ in range(1, 2485+1):
		index = (index_ - 1)*5 + featureIndex
		f = data[:,index]
		nv = normVal[:, index]
		group1data = f[g1_indices]
		g1_nv = nv[g1_indices]
		group2data = f[g2_indices]
		g2_nv = nv[g2_indices]
		totalNum = len(group1data) + len(group2data)
		group1data = group1data[g1_nv >= pvalueThreshold]
		group2data = group2data[g2_nv >= pvalueThreshold]
		pv, pr, _ = groupComparison(group1data, group2data, testIndex)
		
		# if pv <= 0.05:
		#     print('g1d: ', group1data)
		#     print('g2d: ', group2data)
		#     print(pv)
		
		validRate = (len(group1data) + len(group2data)) / totalNum

		pv_log = 0
		if pv!= 0:
			pv_log = -math.log(pv)
		if pv_log == 0:
			pv_log = 0.00000001
		if validRate < validThreshold:
			pvalueArr[int(index%5)].append(0)
			originPvalueArr[int(index%5)].append(1)
		elif pr > 0:
			pvalueArr[int(index%5)].append(pv_log)
			originPvalueArr[int(index%5)].append(pv)
		else:
			pvalueArr[int(index%5)].append(-pv_log)
			originPvalueArr[int(index%5)].append(-pv)
		
		
		if len(f) != 0 and validRate >= validThreshold:
			peopleNumArr[int(index%5)].append(len(group1data) + len(group2data))
			peopleNumArrG1[int(index%5)].append(len(group1data))
			peopleNumArrG2[int(index%5)].append(len(group2data))
		else:
			peopleNumArr[int(index%5)].append(0)
			peopleNumArrG1[int(index%5)].append(0)
			peopleNumArrG2[int(index%5)].append(0)
		'''
		if _ != 0 and validRate >= validThreshold:
			featureArr1[int(index%5)].append(group1data.mean())
			featureArr2[int(index%5)].append(group2data.mean())
			quality[int(index%5)].append(index)
		else:
			featureArr1[int(index%5)].append(0)
			featureArr2[int(index%5)].append(0)
		'''
		f = f[nv >= pvalueThreshold]
		if len(f) != 0 and validRate >= validThreshold:
			featureArrTotal[int(index)%5].append(f)
		else:
			featureArrTotal[int(index)%5].append(np.array([0]))
		if len(group1data) != 0 and validRate >= validThreshold:
			featureArrG1[int(index)%5].append(group1data)
		else:
			featureArrG1[int(index)%5].append(np.array([0]))
		if len(group2data) != 0 and validRate >= validThreshold:
			featureArrG2[int(index)%5].append(group2data)
		else:
			featureArrG2[int(index)%5].append(np.array([0]))

	res_dict["data"]["originPvalue"] = originPvalueArr[featureIndex]
	res_dict["data"]["pvalue"] = pvalueArr[featureIndex]
	res_dict["data"]["average"] = [x.mean() for x in featureArrTotal[featureIndex] ]
	res_dict["data"]["variance"] = [x.std() for x in featureArrTotal[featureIndex] ]
	res_dict["data"]["totalPeople"] = peopleNumArr[featureIndex]

	res_dict["data"]["averageGroupOne"] = [x.mean() for x in featureArrG1[featureIndex] ]
	res_dict["data"]["varianceGroupOne"] = [x.std() for x in featureArrG1[featureIndex] ]
	res_dict["data"]["totalPeopleGroupOne"] = peopleNumArrG1[featureIndex]

	res_dict["data"]["averageGroupTwo"] = [x.mean() for x in featureArrG2[featureIndex] ]
	res_dict["data"]["varianceGroupTwo"] = [x.std() for x in featureArrG2[featureIndex] ]
	res_dict["data"]["totalPeopleGroupTwo"] = peopleNumArrG2[featureIndex]
	print('singleFeature: ', time.time()-start_time)
	'''
	with open('pvalue-'+str(featureIndex)+'-'+str(pvalueThreshold)+'-'+str(validThreshold)+'.json','w') as ifile:
		json.dump(pvalueArr[featureIndex], ifile)
	'''
	return res_dict

def GLLobe(Z,D,index_,selected_d,pvalueArrGrp1,pvalueArrGrp2,pNumArrGrp,featureIndex,indices_,matrix):
	selected_f2 = np.array([True if x==1 else False for x in matrix])
	 # 计算z-score
	z_score1 = np.array(getZScore(pvalueArrGrp1[featureIndex], pNumArrGrp[featureIndex], indices_[selected_f2]))
	z_score2 = np.array(getZScore(pvalueArrGrp2[featureIndex], pNumArrGrp[featureIndex], indices_[selected_f2]))
	
	indices = np.arange(70)
	indices1 = indices[z_score1>Z]
	indices2 = indices[z_score2>Z]
	indices = np.concatenate((indices1, indices2))

	# lobe分组
	lobe_clusters = [
			[3,12,14,17,18,19,20,24,27,28,32],
			[38,47,49,52,53,54,55,59,62,63,67],#frontal
			[8,21,22,25,29,31],
			[43,56,57,60,64,66],#parietal
			[5,11,13],
			[40,46,48],#occipital
			[1,6,7,9,15,30,33,34],
			[36,41,42,44,50,65,68,69],#temporal 
			[2,10,16,23,26],
			[37,45,51,58,61],#limbic
			[0,35],
			[4,39]
			]
	roi_to_connect_mask = []
	group_indices_pack1 = []
	group_indices_pack2 = []

	for i in range(70):
		for j in range(i, 70):
			# 根据lobe进行分组
			if i in indices and j in indices:
				for lc in range(len(lobe_clusters)):
					if i in lobe_clusters[lc]:
						group_indices_pack1.append(lc)
					if j in lobe_clusters[lc]:
						group_indices_pack2.append(lc)
				roi_to_connect_mask.append(True)
			else:
				group_indices_pack1.append(-1)
				group_indices_pack2.append(-1)
				roi_to_connect_mask.append(False)

	roi_to_connect_mask = np.array(roi_to_connect_mask) & selected_f2

	# 去掉自连接
	not_self_connect_mask = []
	for i in range(70):
		for j in range(i, 70):
			if i == j:
				not_self_connect_mask.append(False)
			else:
				not_self_connect_mask.append(True)
	not_self_connect_mask = np.array(not_self_connect_mask)
	roi_to_connect_mask = roi_to_connect_mask & not_self_connect_mask

	data_ = selected_d[:,roi_to_connect_mask]

	indices_grp = np.arange(2485)
	indices_grp = indices_grp[roi_to_connect_mask]
	
	group_indices_pack1 = np.array(group_indices_pack1)
	group_indices_pack2 = np.array(group_indices_pack2)
	group_indices_pack1 = group_indices_pack1[roi_to_connect_mask]
	group_indices_pack2 = group_indices_pack2[roi_to_connect_mask]
	group_indices_pack = np.concatenate((group_indices_pack1, group_indices_pack2))
	data_ = filter_function_grl(data_, index_)

	data_ = np.concatenate((data_,data_), axis=1)
	indices_grp = np.concatenate((indices_grp, indices_grp), axis=0)
	
	data_ = data_[:, group_indices_pack!=-1]
	indices_grp = indices_grp[group_indices_pack!=-1]
	
	group_indices_pack = group_indices_pack[group_indices_pack!=-1]
	same_index = []
	select_same_index = [False]*data_.shape[-1]
	for i in range(data_.shape[-1]):
		line = data_[:, i]
		gid = group_indices_pack[i]
		for j in range(i+1, data_.shape[-1]):
			if sum(line - data_[:,j]) == 0 and gid == group_indices_pack[j]:
				same_index.append(i)
				select_same_index[i] = True
				select_same_index[j] = True
	# print(same_index)
	select_same_index = np.array(select_same_index)
	
	# 去掉自己与自己相连复制后的重复部分
	data_ = np.delete(data_, same_index, 1)
	indices_grp = np.delete(indices_grp, same_index, 0)
	group_indices_pack = np.delete(group_indices_pack, same_index, 0)

	# 去掉非满秩的block
	notFullRankGroupIndex = []
	nRow = data_.shape[0]
	numPerGroup = [0] * int((max(group_indices_pack) + 1))
	for i in range(data_.shape[-1]):
		gid = int(group_indices_pack[i])
		numPerGroup[gid] += 1
	for i in range(len(numPerGroup)):
		if numPerGroup[i] > nRow:
			notFullRankGroupIndex.append(i)
	for i in range(len(notFullRankGroupIndex)):
		gid = notFullRankGroupIndex[i]
		data_ = data_[:, group_indices_pack!=gid]
		indices_grp = indices_grp[group_indices_pack!=gid]
		group_indices_pack = group_indices_pack[group_indices_pack!=gid]
	return data_, indices_grp, group_indices_pack

def GLROI(Z,D,index_,selected_d,pvalueArrGrp1,pvalueArrGrp2,pNumArrGrp,featureIndex,indices_,matrix):
	selected_f2 = np.array([True if x==1 else False for x in matrix])
	 # 计算z-score
	z_score1 = np.array(getZScore(pvalueArrGrp1[featureIndex], pNumArrGrp[featureIndex], indices_[selected_f2]))
	z_score2 = np.array(getZScore(pvalueArrGrp2[featureIndex], pNumArrGrp[featureIndex], indices_[selected_f2]))
	
	indices = np.arange(70)
	indices1 = indices[z_score1>Z]
	indices2 = indices[z_score2>Z]
	indices = np.concatenate((indices1, indices2))

	
	roi_to_connect_mask = []
	group_indices_pack1 = []
	group_indices_pack2 = []

	for i in range(70):
		for j in range(i, 70):
			# 根据lobe进行分组
			if i in indices and j in indices:	
				group_indices_pack1.append(i)
				group_indices_pack2.append(j)
				roi_to_connect_mask.append(True)
			else:
				group_indices_pack1.append(-1)
				group_indices_pack2.append(-1)
				roi_to_connect_mask.append(False)

	roi_to_connect_mask = np.array(roi_to_connect_mask) & selected_f2

	# 去掉自连接
	not_self_connect_mask = []
	for i in range(70):
		for j in range(i, 70):
			if i == j:
				not_self_connect_mask.append(False)
			else:
				not_self_connect_mask.append(True)
	not_self_connect_mask = np.array(not_self_connect_mask)
	roi_to_connect_mask = roi_to_connect_mask & not_self_connect_mask

	data_ = selected_d[:,roi_to_connect_mask]

	indices_grp = np.arange(2485)
	indices_grp = indices_grp[roi_to_connect_mask]
	
	group_indices_pack1 = np.array(group_indices_pack1)
	group_indices_pack2 = np.array(group_indices_pack2)
	group_indices_pack1 = group_indices_pack1[roi_to_connect_mask]
	group_indices_pack2 = group_indices_pack2[roi_to_connect_mask]
	group_indices_pack = np.concatenate((group_indices_pack1, group_indices_pack2))
	data_ = filter_function_grl(data_, index_)

	data_ = np.concatenate((data_,data_), axis=1)
	indices_grp = np.concatenate((indices_grp, indices_grp), axis=0)
	
	data_ = data_[:, group_indices_pack!=-1]
	indices_grp = indices_grp[group_indices_pack!=-1]
	
	group_indices_pack = group_indices_pack[group_indices_pack!=-1]
	same_index = []
	select_same_index = [False]*data_.shape[-1]
	for i in range(data_.shape[-1]):
		line = data_[:, i]
		gid = group_indices_pack[i]
		for j in range(i+1, data_.shape[-1]):
			if sum(line - data_[:,j]) == 0 and gid == group_indices_pack[j]:
				same_index.append(i)
				select_same_index[i] = True
				select_same_index[j] = True
	# print(same_index)
	select_same_index = np.array(select_same_index)
	
	# 去掉自己与自己相连复制后的重复部分
	data_ = np.delete(data_, same_index, 1)
	indices_grp = np.delete(indices_grp, same_index, 0)
	group_indices_pack = np.delete(group_indices_pack, same_index, 0)

	# 去掉非满秩的block
	notFullRankGroupIndex = []
	nRow = data_.shape[0]
	numPerGroup = [0] * int((max(group_indices_pack) + 1))
	for i in range(data_.shape[-1]):
		gid = int(group_indices_pack[i])
		numPerGroup[gid] += 1
	for i in range(len(numPerGroup)):
		if numPerGroup[i] > nRow:
			notFullRankGroupIndex.append(i)
	for i in range(len(notFullRankGroupIndex)):
		gid = notFullRankGroupIndex[i]
		data_ = data_[:, group_indices_pack!=gid]
		indices_grp = indices_grp[group_indices_pack!=gid]
		group_indices_pack = group_indices_pack[group_indices_pack!=gid]
	return data_, indices_grp, group_indices_pack

def GLDBScan(Z,D,index_,selected_d,pvalueArrGrp1,pvalueArrGrp2,pNumArrGrp,featureIndex,indices_,matrix):
	selected_f2 = np.array([True if x==1 else False for x in matrix])
	 # 计算z-score
	z_score1 = np.array(getZScore(pvalueArrGrp1[featureIndex], pNumArrGrp[featureIndex], indices_[selected_f2]))
	z_score2 = np.array(getZScore(pvalueArrGrp2[featureIndex], pNumArrGrp[featureIndex], indices_[selected_f2]))
	
	pos  = []
	with open('nodes.json', 'r') as ifile:
		pos = json.load(ifile)
	merge_input = []
	for i in range(70):
		x = float(pos[i]['x'])
		y = float(pos[i]['y']) 
		z = float(pos[i]['z']) 
		merge_input.append([x,y,z])
	# 聚类(-1过多？)
	# 如何解决重复索引
	# 满秩问题如何解决（复制两份，同时属于一组）
	
	indices = np.arange(70)
	X = np.array(merge_input)
	X1 = X[z_score1>Z,:]
	indices1 = indices[z_score1>Z]
	merge_input1 = pd.DataFrame(X1, index = indices1, columns=['x','y','z'])

	X2 = X[z_score2>Z,:]
	indices2 = indices[z_score2>Z]
	
	try:
		db1 = DBSCAN(eps=D, min_samples=2).fit(X1)
	except:
		print('something wrong with DBSCAN')
	merge_input1['cluster_id'] = db1.labels_
	
	
	# 删除重复索引
	for i in indices2:
		if i in indices1:
			wh = np.where(indices2==i)
			X2 = np.delete(X2, wh, 0)
			indices2 = np.delete(indices2, wh)
			
	merge_input2 = pd.DataFrame(X2, index = indices2, columns=['x','y','z'])
	
	try:
		db2 = DBSCAN(eps=D, min_samples=2).fit(X2)
	except:
		print('something wrong with DBSCAN')
	db2_labels = []
	for i in range(len(db2.labels_)):
		if db2.labels_[i] != -1:
			db2_labels.append(max(db1.labels_) + db2.labels_[i] + 1)
		else:
			db2_labels.append(-1)
	merge_input2['cluster_id'] = db2_labels
	merge_input = merge_input1.append(merge_input2)
	merge_input = merge_input.drop_duplicates()
	indices = np.concatenate((indices1, indices2))

	roi_to_connect_mask = []
	group_indices_pack1 = []
	group_indices_pack2 = []

	for i in range(70):
		for j in range(i, 70):
			if i in indices and j in indices:
				# 根据ROI聚类结果分组
				group_indices_pack1.append(merge_input.loc[i]['cluster_id'])
				group_indices_pack2.append(merge_input.loc[j]['cluster_id'])
				roi_to_connect_mask.append(True)
			else:
				group_indices_pack1.append(-1)
				group_indices_pack2.append(-1)
				roi_to_connect_mask.append(False)
			

	roi_to_connect_mask = np.array(roi_to_connect_mask) & selected_f2

	# 去掉自连接
	not_self_connect_mask = []
	for i in range(70):
		for j in range(i, 70):
			if i == j:
				not_self_connect_mask.append(False)
			else:
				not_self_connect_mask.append(True)
	not_self_connect_mask = np.array(not_self_connect_mask)
	roi_to_connect_mask = roi_to_connect_mask & not_self_connect_mask

	data_ = selected_d[:,roi_to_connect_mask]

	indices_grp = np.arange(2485)
	indices_grp = indices_grp[roi_to_connect_mask]
	
	group_indices_pack1 = np.array(group_indices_pack1)
	group_indices_pack2 = np.array(group_indices_pack2)
	group_indices_pack1 = group_indices_pack1[roi_to_connect_mask]
	group_indices_pack2 = group_indices_pack2[roi_to_connect_mask]
	group_indices_pack = np.concatenate((group_indices_pack1, group_indices_pack2))
	data_ = filter_function_grl(data_, index_)

	data_ = np.concatenate((data_,data_), axis=1)
	indices_grp = np.concatenate((indices_grp, indices_grp), axis=0)
	
	data_ = data_[:, group_indices_pack!=-1]
	indices_grp = indices_grp[group_indices_pack!=-1]
	
	group_indices_pack = group_indices_pack[group_indices_pack!=-1]
	same_index = []
	select_same_index = [False]*data_.shape[-1]
	for i in range(data_.shape[-1]):
		line = data_[:, i]
		gid = group_indices_pack[i]
		for j in range(i+1, data_.shape[-1]):
			if sum(line - data_[:,j]) == 0 and gid == group_indices_pack[j]:
				same_index.append(i)
				select_same_index[i] = True
				select_same_index[j] = True
	# print(same_index)
	select_same_index = np.array(select_same_index)
	
	# 去掉自己与自己相连复制后的重复部分
	data_ = np.delete(data_, same_index, 1)
	indices_grp = np.delete(indices_grp, same_index, 0)
	group_indices_pack = np.delete(group_indices_pack, same_index, 0)

	# 去掉非满秩的block
	notFullRankGroupIndex = []
	nRow = data_.shape[0]
	numPerGroup = [0] * int((max(group_indices_pack) + 1))
	for i in range(data_.shape[-1]):
		gid = int(group_indices_pack[i])
		numPerGroup[gid] += 1
	for i in range(len(numPerGroup)):
		if numPerGroup[i] > nRow:
			notFullRankGroupIndex.append(i)
	for i in range(len(notFullRankGroupIndex)):
		gid = notFullRankGroupIndex[i]
		data_ = data_[:, group_indices_pack!=gid]
		indices_grp = indices_grp[group_indices_pack!=gid]
		group_indices_pack = group_indices_pack[group_indices_pack!=gid]
	return data_, indices_grp, group_indices_pack


# 返回comparison view单独数据
def getComparisonViewSingleForFMri(featureIndex_, pvalueThreshold, validThreshold, functionIndex, ROI_z_value, DBScan_radius, lambda_, groupName_, subjectList_):
	start_time = time.time()
	featureIndex = int(featureIndex_)

	# featureIndex: 0=strength,1=FA,2=MD,3=RD,4=AxD
	if featureIndex == 0:
		featureIndex = 4
	elif featureIndex == 1:
		featureIndex = 0
	elif featureIndex == 2:
		featureIndex = 1
	elif featureIndex == 3:
		featureIndex = 3
	else:
		featureIndex = 2
	
	pvalueThreshold = float(pvalueThreshold) / 100
	validThreshold = float(validThreshold) / 100
	Z = float(ROI_z_value) / 100 * 5
	D = float(DBScan_radius) / 100 * 2 * 44.38
	normVal = []
	with open('normalValue_fmri_198.json', 'r') as ifile:
		normVal = json.load(ifile)
	normVal = np.array(normVal)
	data = []
	with open('all_feature_fmri_198.json', 'r') as ifile:
		data = json.load(ifile)
	data = np.array(data)
	subject_list = [x.rstrip().split(' ') for x in open(fn2, 'r')]
	SUBJECTS = json.loads(groupName_)
	SUBJECTS = [SUBJECTS['groupOneName'], SUBJECTS['groupTwoName']]
	target1 = SUBJECTS[1].split('-')[-1]
	target2 = SUBJECTS[0].split('-')[-1]
	label = []
	for s in subject_list:
		if target1 == 'Healthy' and target2=='eMCI':
			if int(s[1]) == 0:
				label.append(0)
			elif int(s[1]) == 1:
				label.append(1)
			else:
				label.append(-1)
		elif target1 == 'Healthy' and target2=='lMCI':
			if int(s[1]) == 0:
				label.append(0)
			elif int(s[1]) == 2:
				label.append(1)
			else:
				label.append(-1)
		elif target1 == 'Healthy' and target2=='AD':
			if int(s[1]) == 0:
				label.append(0)
			elif int(s[1]) == 3:
				label.append(1)
			else:
				label.append(-1)
		elif target1 == 'eMCI' and target2=='Healthy':
			if int(s[1]) == 0:
				label.append(1)
			elif int(s[1]) == 1:
				label.append(0)
			else:
				label.append(-1)
		elif target1 == 'eMCI' and target2=='lMCI':
			if int(s[1]) == 2:
				label.append(1)
			elif int(s[1]) == 1:
				label.append(0)
			else:
				label.append(-1)
		elif target1 == 'eMCI' and target2=='AD':
			if int(s[1]) == 3:
				label.append(1)
			elif int(s[1]) == 1:
				label.append(0)
			else:
				label.append(-1)
		elif target1 == 'lMCI' and target2=='Healthy':
			if int(s[1]) == 0:
				label.append(1)
			elif int(s[1]) == 2:
				label.append(0)
			else:
				label.append(-1)
		elif target1 == 'lMCI' and target2=='eMCI':
			if int(s[1]) == 1:
				label.append(1)
			elif int(s[1]) == 2:
				label.append(0)
			else:
				label.append(-1)
		elif target1 == 'lMCI' and target2=='AD':
			if int(s[1]) == 3:
				label.append(1)
			elif int(s[1]) == 2:
				label.append(0)
			else:
				label.append(-1)
		elif target1 == 'AD' and target2=='Healthy':
			if int(s[1]) == 0:
				label.append(1)
			elif int(s[1]) == 3:
				label.append(0)
			else:
				label.append(-1)
		elif target1 == 'AD' and target2=='eMCI':
			if int(s[1]) == 1:
				label.append(1)
			elif int(s[1]) == 3:
				label.append(0)
			else:
				label.append(-1)
		elif target1 == 'AD' and target2=='lMCI':
			if int(s[1]) == 2:
				label.append(1)
			elif int(s[1]) == 3:
				label.append(0)
			else:
				label.append(-1)
		elif target1 == 'Healthy' and target2 == 'Unhealthy':
			if int(s[1]) == 0:
				label.append(0)
			else:
				label.append(1)
		elif target1 == 'Unhealthy' and target2 == 'Healthy':
			if int(s[1]) == 0:
				label.append(1)
			else:
				label.append(0)
		elif target1 == 'Male':
			if int(s[2]) == 1:
				label.append(0)
			else:
				label.append(1)
		elif target1 == 'Female':
			if int(s[2]) == 2:
				label.append(1)
			else:
				label.append(0)
		else:
			# print('分组不支持')
			if float(s[3]) >= 50 and float(s[3]) < 60:
				if target1 == "50s":
					label.append(0)
				elif target2 == "50s":
					label.append(1)
				else:
					label.append(-1)
			elif float(s[3]) >= 60 and float(s[3]) < 70:
				if target1 == "60s":
					label.append(0)
				elif target2 == "60s":
					label.append(1)
				else:
					label.append(-1)
			elif float(s[3]) >= 70 and float(s[3]) < 80:
				if target1 == "70s":
					label.append(0)
				elif target2 == "70s":
					label.append(1)
				else:
					label.append(-1)
			elif float(s[3]) >= 80:
				if target1 == "80s":
					label.append(0)
				elif target2 == "80s":
					label.append(1)
				else:
					label.append(-1)
			else:
				print("年龄分组有问题")
	label = np.array(label)
	
	pvalueArr = [[],[],[],[],[]]
	originPvalueArr = [[],[],[],[],[]]
	featureArr1 = [[],[],[],[],[]]
	featureArr2 = [[],[],[],[],[]]
	featureArrTotal = [[],[],[],[],[]]
	peopleNumArr = [[],[],[],[],[]]
	threshold = [4,0.4,0.02,0.001,0.0004]
	res_dict = {}
	res_dict['code'] = 0
	res_dict['data'] = {}
	res_dict['message'] = 'Success'
	
	name = ['FA', 'MD', 'AxD', 'RD', 'Strength']
	subjectList = json.loads(subjectList_)
	g1_indices = subjectList['groupOneList']
	g2_indices = subjectList['groupTwoList']
	print(g1_indices, g2_indices)
	quality = [[],[],[],[],[]]

	# pvalue for grplasso
	pvalueArrGrp1 = [[],[],[],[],[]]
	pvalueArrGrp2 = [[],[],[],[],[]]
	pNumArrGrp = [[],[],[],[],[]]

	for index_ in range(1, 2485+1):
		index = (index_-1) * 5 + featureIndex
		f = data[:,index]
		nv = normVal[:, index]
		group1data = f[g1_indices]
		g1_nv = nv[g1_indices]
		group2data = f[g2_indices]
		g2_nv = nv[g2_indices]
		totalNum = len(group1data) + len(group2data)
		group1data = group1data[g1_nv >= pvalueThreshold]
		group2data = group2data[g2_nv >= pvalueThreshold]
		
		# 固定使用t-test
		pv, pr, _ = groupComparison(group1data, group2data, 0)
		validRate = (len(group1data) + len(group2data)) / totalNum

		pv_log = 0
		if pv!= 0:
			pv_log = -math.log(pv)
		if validRate < validThreshold:
			pvalueArr[int(index%5)].append(0)
			originPvalueArr[int(index%5)].append(1)
		elif pr > 0:
			pvalueArr[int(index%5)].append(pv_log)
			originPvalueArr[int(index%5)].append(pv)
		else:
			pvalueArr[int(index%5)].append(-pv_log)
			originPvalueArr[int(index%5)].append(-pv)

		if pr > 0:
			pvalueArrGrp1[int(index%5)].append(pv)
			pvalueArrGrp2[int(index%5)].append(0)
		else:
			pvalueArrGrp1[int(index%5)].append(0)
			pvalueArrGrp2[int(index%5)].append(pv)
		pNumArrGrp[int(index%5)].append(len(group1data) + len(group2data))
		
		peopleNumArr[int(index%5)].append(len(group1data) + len(group2data))
		if _ != 0 and validRate >= validThreshold:
			featureArr1[int(index%5)].append(group1data.mean())
			featureArr2[int(index%5)].append(group2data.mean())
			quality[int(index%5)].append(index)
		else:
			featureArr1[int(index%5)].append(0)
			featureArr2[int(index%5)].append(0)
		f = f[nv >= pvalueThreshold]
		if len(f) != 0 and validRate >= validThreshold:
			featureArrTotal[int(index)%5].append(f)
		else:
			featureArrTotal[int(index)%5].append(np.array([0]))
	g_indices = np.concatenate((g1_indices, g2_indices))
	data = data[g_indices, :]
	print('len of label: ',len(label))
	print('len of indices', len(g_indices))
	
	label = label[g_indices]
	print('label: ', label)
	indices = np.arange(5*2485)
	indices_ = np.arange(2485)
	matrix_all = []
	scores_all = []
	coef_all = []
	score = 0
	best_lambda = 0
	indices1_ = []
	data_, indices_grp, group_indices_pack = [], [], []
	
	for index in range(featureIndex, featureIndex+1):
		selected_f = np.array([True if x%5==index else False for x in range(5*2485)])
		selected_d = data[:, selected_f]
		matrix = []
		scores = [0]*2485
		coef = [0]*2485
		for j in range(2485):
			j_ = j*5+index
			if j_ in quality[index]:
				matrix.append(1)
			else:
				matrix.append(0)
		mask = []
		if int(functionIndex) < 2:
			# if int(functionIndex) == 2:
			# 	data_, indices_grp, group_indices_pack = GLDBScan(Z,D,index,selected_d,pvalueArrGrp1,pvalueArrGrp2,pNumArrGrp,index,indices_,matrix)
			if int(functionIndex) == 1:
				data_, indices_grp, group_indices_pack = GLLobe(Z,D,index,selected_d,pvalueArrGrp1,pvalueArrGrp2,pNumArrGrp,index,indices_,matrix)
			else:
				data_, indices_grp, group_indices_pack = GLROI(Z,D,index,selected_d,pvalueArrGrp1,pvalueArrGrp2,pNumArrGrp,index,indices_,matrix)

			print(data_.shape[-1])
			print(group_indices_pack)
			print(indices_grp)
			if data_.shape[-1] == 0:
				scores_all.append([0]*2485)
				matrix_all.append([0]*2485)
				coef_all.append([0]*2485)
				print('data len is 0')
				res_dict['message'] = 'No data left after filted'
				break
			
			with open('cache_fmri/feature_'+str(index)+str(ROI_z_value)+str(DBScan_radius)+SUBJECTS[0]+SUBJECTS[1]+'.json', 'w') as inputfile:
				json.dump(data_.tolist(), inputfile)
			with open('cache_fmri/label_'+str(index)+str(ROI_z_value)+str(DBScan_radius)+SUBJECTS[0]+SUBJECTS[1]+'.json', 'w') as inputfile:
				json.dump(label.tolist(), inputfile)
			with open('cache_fmri/index_'+str(index)+str(ROI_z_value)+str(DBScan_radius)+SUBJECTS[0]+SUBJECTS[1]+'.json', 'w') as inputfile:
				json.dump(group_indices_pack.tolist(), inputfile)
			if float(lambda_) == -1:
				proc = subprocess.Popen('Rscript grplasso_fmri.R '+str(index)+' '+str(ROI_z_value)+' '+str(DBScan_radius)+' '+SUBJECTS[0]+' '+SUBJECTS[1], shell=True)
				proc.wait()
				with open('cache_fmri/lambda_'+str(index)+str(ROI_z_value)+str(DBScan_radius)+SUBJECTS[0]+SUBJECTS[1]+'.json','r') as ifile:
					best_lambda = json.load(ifile)
					best_lambda = best_lambda[-1]
			else:
				proc = subprocess.Popen('Rscript grplasso_with_lambda_fmri.R '+str(index)+' '+str(ROI_z_value)+' '+str(DBScan_radius)+' '+SUBJECTS[0]+' '+SUBJECTS[1]+' '+lambda_, shell=True)
				proc.wait()
				best_lambda = float(lambda_)
			
			groups = group_indices_pack
			
			with open('cache_fmri/lass_coef_'+str(index)+str(ROI_z_value)+str(DBScan_radius)+SUBJECTS[0]+SUBJECTS[1]+'.json','r') as ifile:
				mask = json.load(ifile)
			mask = np.array(mask)
			mask = mask.reshape(-1)
			# print('mask: ', mask)
			try:
				indices_grp_mask = indices_grp[mask>0]
			except:
				scores_all.append([0]*2485)
				matrix_all.append([0]*2485)
				coef_all.append([0]*2485)
				print('Not full rank')
				res_dict['message'] = 'No selected feature'
				break
			for i in range(len(indices_grp_mask)):
				coef[indices_grp_mask[i]] = mask[i]
			data_ = data_[:,mask>0]
			indices1_ = indices_grp[mask>0]
		else:
			model = []
			# 去掉自连接
			not_self_conect_mask = []
			for i in range(70):
				for j in range(i, 70):
					if i==j :
						not_self_conect_mask.append(False)
					else:
						not_self_conect_mask.append(True)
			not_self_conect_mask = np.array(not_self_conect_mask)
			selected_d = selected_d[:,not_self_conect_mask]

			if int(functionIndex) == 2:
				if float(lambda_) == -1:
					model = LassoCV(n_jobs = -1).fit(selected_d, label)
				else:
					model = LassoCV(alphas = [float(lambda_)],n_jobs = -1).fit(selected_d, label)
			elif int(functionIndex) == 3:
				if float(lambda_) == -1:
					model = ElasticNetCV(n_jobs = -1).fit(selected_d, label)
				else:
					model = ElasticNetCV(alphas = [float(lambda_)],n_jobs = -1).fit(selected_d, label)
			else:
				if float(lambda_) == -1:
					model = RidgeClassifierCV(cv = 3).fit(selected_d, label)
				else:
					model = RidgeClassifierCV(alphas = [float(lambda_)], cv = 3).fit(selected_d, label)
			mask = model.coef_
			best_lambda = model.alpha_
			mask = mask.reshape(-1)
			# print('mask:',mask)
			# print('lambda:',best_lambda)
			data_ = selected_d
			indices_grp = np.arange(2485)
			indices_grp = indices_grp[not_self_conect_mask]
			data_ = data_[:,mask!=0]
			indices1_ = indices_grp[mask!=0]
		if data_.shape[-1] == 0:
			scores_all.append([0]*2485)
			matrix_all.append([0]*2485)
			coef_all.append([0]*2485)
			print('data len is 0')
			res_dict['message'] = 'No data left after filted'
			break
			
		print('Shape of data: ', data_.shape)
		# model = LogisticRegression(penalty='l2', C=1000, max_iter=1000)
		model = LogisticRegressionCV(max_iter=10000, n_jobs=-1)
		model.fit(data_, label)
		
		score = cross_val_score(model, data_, label, cv=5, scoring='accuracy').mean()
		print('Score: ', score)
		# score = score - 0.52
		'''
		if score < 0.52:
			score = 0
		'''
		for i in indices1_:
			# matrix[i] = 0
			scores[i] = score
		for s in range(len(scores)):
			if scores[s] != 0:
				matrix[s] = 1
			else:
				matrix[s] = 0
		
		matrix_all.append(matrix)
		scores_all.append(scores)
		coef_all.append(coef)
	
	# print('matrix: ',matrix_all[0])
	res_dict['data']["accuracy"] = score
	res_dict['data']["lambda"] = best_lambda
	res_dict['data']["AccSelectConnection"] = matrix_all[0]
	res_dict['data']["weight"] = coef_all[0]
	res_dict['data']["WeightSelectConnection"] = matrix_all[0]
	res_dict["data"]["originPvalue"] = originPvalueArr[featureIndex]
	res_dict["data"]["pvalue"] = pvalueArr[featureIndex]
	print('multitime: ', time.time()-start_time)
	return res_dict


# 利用向量X乘求点到直线距离
def get_pt_line_dis(pt, line):
	QP = np.array([pt[0] - line[0][0], pt[1] - line[0][1], pt[2] - line[0][2]])
	v = np.array([line[0][0] - line[1][0], line[0][1] - line[1][1], line[0][2] - line[1][2]])
	h = np.linalg.norm(np.cross(QP, v)/np.linalg.norm(v))
	return h

def get_pts_dis(pt1, pt2):
	return math.sqrt((pt2[0]-pt1[0])**2+(pt2[1]-pt1[1])**2+(pt2[2]-pt1[2])**2)

def cost_dist(dj, dmax):
	if dj >= dmax:
		return 0
	else:
		return 1


def getDi(line, dmax, n, signal, R0, R1):
	length = math.sqrt((line[1][0]-line[0][0])**2+(line[1][1]-line[0][1])**2+(line[1][2]-line[0][2])**2)
	lmin = 0
	r = 0
	iscover = 0
	if signal[0] == 0:
		length2 = length - R0
		r = R0
	elif signal[0] == 1:
		length2 = length - R1
		r = R1
	else:
		print('Error')
	if signal[1] == 0:
		length2 = length2 - R0
	elif signal[1] == 1:
		length2 = length2 - R1
	else:
		print('Error')

	if length2 < dmax * (n+1):
		lmin = length2/(n+1)
		iscover = 1
	else:
		lmin = dmax

	vec = np.array([line[1][0]-line[0][0], line[1][1]-line[0][1], line[1][2]-line[0][2]])
	unit_vec = vec / length

	di = []
	nmax = int(length / dmax) - 1
	for i in range(1,nmax+1):
		new_vec = np.array([line[0][0], line[0][1], line[0][2]]) + unit_vec*r + unit_vec*i*lmin
		di.append([new_vec[0], new_vec[1], new_vec[2]])
	return di, lmin, iscover


# 具体算法
def adjust_algorithm(totalData_, circleNum_, R1_, R0_, dmax_):
	ALPHA = 3
	n = int(circleNum_)
	R1 = float(R1_)
	R0 = float(R0_)
	dmax = float(dmax_)
	nodes  = []
	with open('nodes.json', 'r') as ifile:
		nodes = json.load(ifile)
	nodes_pos = []
	for i in range(70):
		x = float(nodes[i]['x'])
		y = float(nodes[i]['y']) 
		z = float(nodes[i]['z']) 
		nodes_pos.append([x,y,z])
	edges = []
	for i in range(70):
		for j in range(i, 70):
			edges.append([nodes_pos[i], nodes_pos[j]])
	mask = []
	totalData = json.loads(totalData_)
	
	for item in totalData:
		mask.append([int(item), int(item)])
	# Step1: 根据球的半径计算放球点位置Di，每个Di计算相应的cost
	circlePoints = []
	full = []
	for eIndex in range(len(edges)):
		e = edges[eIndex]
		signal = mask[eIndex]
		dis, lmin, iscover= getDi(e, dmax, n, signal, R0, R1)
		full.append(iscover)
		cdis = []
		for di_ in dis:
			mindis = 999999999
			for e_ in edges:
				tempdis = get_pt_line_dis(di_, e_)
				if tempdis < mindis:
					mindis = tempdis
			cdis.append(cost_dist(tempdis, dmax))
		# Step2
		cost_all1 = 0
		cost_dist_all1 = 0
		case = 0
		# case1
		
		for dIndex in range(n):
			cost_dist_all1 += cdis[dIndex]
		cost_all1 = cost_dist_all1 + 1/ALPHA*(get_pts_dis(dis[0], dis[n-1])/lmin-n)

		# case2
		cost_all2 = 0
		cost_dist_all2 = 0
		
		pos = []
		lastIdx = 0
		for dIndex in range(len(cdis)):
			if cdis[dIndex] == 0:
				pos.append(dis[dIndex])
				lastIdx = dIndex
		if len(pos) < n:
			cost_dist_all2 = 99999999
		else:
			cost_dist_all2 = cost_all2 + 1/ALPHA*(get_pts_dis(pos[0], pos[n-1])/lmin-n)

		if cost_dist_all1 < cost_dist_all2:
			# 选择第一种缺省方案
			case = 1
		else:
			# 选择第二种缺省方案
			case = 2
		# Step3
		print('Case: ', case)
		selected_indices = []
		cost_all = 99999999
		if case == 1:
			
			for dIndex in range(n-1, len(dis)):
				# 找出前n-1cost_dist最小的情况, 剪枝
				cdis_dict = {}
				for cIndex in range(dIndex):
					cdis_dict[str(cIndex)] = cdis[cIndex]
				sorted_cdis = sorted(cdis_dict.items(), key=lambda x:x[1])
				temp_cost_all = 0
				temp_selected_indices = []
				for sIndex in range(n-1):
					temp_cost_all += sorted_cdis[sIndex][1]
					temp_selected_indices.append(int(sorted_cdis[sIndex][0]))
				temp_selected_indices.append(dIndex)
				temp_selected_indices = sorted(temp_selected_indices)
				temp_cost_all += temp_cost_all + cdis[dIndex] + 1/ALPHA*(get_pts_dis(dis[temp_selected_indices[0]], dis[temp_selected_indices[-1]])/lmin-n)
				if temp_cost_all < cost_all:
					cost_all = temp_cost_all
					selected_indices = temp_selected_indices
		elif case == 2:
			for dIndex in range(lastIdx, len(dis)):
				# 找出前n-1cost_dist最小的情况, 剪枝
				cdis_dict = {}
				for cIndex in range(dIndex):
					cdis_dict[str(cIndex)] = cdis[cIndex]
				sorted_cdis = sorted(cdis_dict.items(), key=lambda x:x[1])
				
				temp_cost_all = 0
				temp_selected_indices = []
				for sIndex in range(n-1):
					temp_cost_all += sorted_cdis[sIndex][1]
					temp_selected_indices.append(int(sorted_cdis[sIndex][0]))
				temp_selected_indices.append(dIndex)
				
				temp_selected_indices = sorted(temp_selected_indices)
				temp_cost_all += temp_cost_all + cdis[dIndex] + 1/ALPHA*(get_pts_dis(dis[temp_selected_indices[0]], dis[temp_selected_indices[-1]])/lmin-n)
				if temp_cost_all < cost_all:
					cost_all = temp_cost_all
					selected_indices = temp_selected_indices

		final_position = []
		for idx in selected_indices:
			final_position.append(dis[idx])
		circlePoints.append(final_position)
	res_dict = {}
	res_dict['circlePoints'] = circlePoints
	res_dict['full'] = full
	return res_dict

if __name__ == '__main__':
	subjectsArr = json.dumps(["all-AD","all-healthy"])
	res_dict = getPValue(subjectsArr)
	
	
	
		
	