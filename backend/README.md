## 一、简介
MV2Net系统的后端程序，基于Flask框架，主要包括前后端交互接口，支持系统功能的后端服务。

## 二、结构
```
|-- all_feature_198.json
|-- all_feature_fmri_198.json
|-- grplasso.R
|-- grplasso_fmri.R
|-- grplasso_with_lambda.R
|-- grplasso_with_lambda_fmri.R
|-- index.py
|-- node.json 
|-- normalValue_198.json 
|-- normalValue_fmri_198.json
|-- run.py 
|-- subject_all_198.txt 
|-- utils.py
|-- utilsForFMri.py
|-- BrainDataManager.cpp
|-- BrainRender.cpp
```

## 三、系统启动
python3.4 run.py

## 四、文件详细说明
### 1. all_feature_198.json: 
198人的特征数据（由getOriginData.py生成）。

### 2. all_feature_fmri_198.json
198人的特征数据（由getOriginDataForFMri.py生成）。

### 3. grplasso.R: 
R语言实现的group lasso代码，在utils.py中调用。

### 4. grplasso_fmri.R: 
同上，在utilsForFMri.py中调用。

### 5. grplasso_with_lambda.R: 
R语言实现group lasso代码，支持lambda作为参数输入，在utils.py中调用。

### 6. grplasso_with_lambda_fmri.R: 
同上，在utilsForFMri.py中调用。

### 7. index.py: 
定义前后端交互的API。

### 8. node.json: 
脑网络节点坐标信息，在utils.py和utilsForFMri.py中使用。

### 9. normalValue_198.json: 
198人的质量文件（由getOriginData.py生成）。

### 10. normalValue_fmri_198.json
198人的质量文件（由getOriginDataForFMri.py生成）。

### 11. run.py: 
后端服务启动脚本。

### 12. subject_all_198.txt: 
198人的信息。

### 13. utils.py: 
MV2Net系统后端处理脚本。
下面介绍几个重要方法：
#### 13.1 updateSunburst根据用户属性更新sunburst图。
#### 13.2 getPValueSingle根据用户组返回feature heatmap的特征质量数据。
#### 13.3 getFeatureViewPValueSingle 单独返回每个network comparison的统计数据。
#### 13.4 getComparisonViewSingle单独返回每个network comparison的模型分析结果。
#### 13.5 adjust_algorithm支持high-order composite的布局优化。

### 14. utilsForFMri.py:
同上。

### 15. BrainDataManager.cpp: 
生成纤维特征数据（中山大学负责的代码）。

### 16. BrainRender.cpp: 
将纤维特征数据保存为json（中山大学负责的代码）。

