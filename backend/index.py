from flask import Flask, jsonify
from flask_cors import CORS
from utils import getData, getDataByPro, getDataByPF, updateSunburst, getPValue, updateTwoD, getFeatureViewPValueSingle, getComparisonViewSingle, getPValueSingle, adjust_algorithm
from utilsForFMri import updateSunburstForFMri, getFeatureViewPValueSingleForFMri, getComparisonViewSingleForFMri, getPValueSingleForFMri

app = Flask(__name__)
CORS(app)

@app.route('/')
def hello_world():
    return 'Hello, World!'

@app.route('/api/getCirclePoint/<totalData>/<circleNum>/<R1>/<R0>/<dmax>')
def getCirclePoint(totalData, circleNum, R1, R0, dmax):
    res_dict = adjust_algorithm(totalData, circleNum, R1, R0, dmax)
    return jsonify(res_dict)

@app.route('/api/getCirclePointForFMri/<totalData>/<circleNum>/<R1>/<R0>/<dmax>')
def getCirclePointForFMri(totalData, circleNum, R1, R0, dmax):
    res_dict = adjust_algorithm(totalData, circleNum, R1, R0, dmax)
    return jsonify(res_dict)

@app.route('/api/getOriginData')
def getOriginData():
    res_dict = getData()
    return jsonify(res_dict)

@app.route('/api/getDataWithPro/<properties>')
def getDataWithPro(properties):
    res_dict = getDataByPro(properties)
    return jsonify(res_dict)

@app.route('/api/getDataWithProAndFea/<properties>/<featurename>')
def getDataWithProAndFea(properties, featurename):
    res_dict = getDataByPF(properties, featurename)
    return jsonify(res_dict)

@app.route('/api/getAllProperties')
def getAllProperties():
    # res_dict = {"code":0,"data":[{"message":"Sicken","checkval":False,"order":""},{"message":"Gender","checkval":False,"order":""},{"message":"Ages","checkval":False,"order":""},{"message":"Others","checkval":False,"order":""}],"message":"\u83b7\u53d6\u6210\u529f"}
    res_dict = {"code":0,"data":[{"message":"Sicken","checkval":False,"order":"","children":["Healthy","eMCI","lMCI","AD"]},{"message":"Gender","checkval":False,"order":"","children":["Female","Male"]},{"message":"Ages","checkval":False,"order":"","children":["50s","60s","70s","80s"]},{"message":"Sicken2","checkval":False,"order":"","children":["Healthy","Unhealthy"]}],"message":"\u83b7\u53d6\u6210\u529f"}
    return jsonify(res_dict)

@app.route('/api/updateSunburstData/<jsonArr>')
def updateSunburstData(jsonArr):
    res_dict = updateSunburst(jsonArr)
    return jsonify(res_dict)

@app.route('/api/getPValueData/<jsonArr>')
def getPValueData(jsonArr):
    res_dict = getPValue(jsonArr)
    return jsonify(res_dict)

@app.route('/api/getPValueDataForOneGroup/<groupIndex>/<groupName>')
def getPValueDataForOneGroup(groupIndex, groupName):
    res_dict = getPValueSingle(groupIndex, groupName)
    return jsonify(res_dict)

@app.route('/api/update2D/<pvalueThreshold>/<validThreshold>/<testIndex>/<lamba>/<ROI_z_value>/<DBScan_radius>')
def update2D(pvalueThreshold, validThreshold, testIndex, lamba, ROI_z_value, DBScan_radius):
    res_dict = updateTwoD(pvalueThreshold, validThreshold, testIndex, lamba, ROI_z_value, DBScan_radius)
    return jsonify(res_dict)

@app.route('/api/singleAnalyzeForOnefeature/<featureIndex>/<pvalueThreshold>/<validThreshold>/<testIndex>/<groupName>/<subjectList>')
def singleAnalyzeForOnefeature(featureIndex, pvalueThreshold, validThreshold, testIndex, groupName, subjectList):
    res_dict = getFeatureViewPValueSingle(featureIndex, pvalueThreshold, validThreshold, testIndex, groupName, subjectList)
    return jsonify(res_dict)

@app.route('/api/multiAnalyzeForOnefeature/<featureIndex>/<pvalueThreshold>/<validThreshold>/<functionIndex>/<ROI_z_value>/<DBScan_radius>/<lamba>/<groupName>/<subjectList>')
def multiAnalyzeForOnefeature(featureIndex, pvalueThreshold, validThreshold, functionIndex, ROI_z_value, DBScan_radius, lamba, groupName, subjectList):
    res_dict = getComparisonViewSingle(featureIndex, pvalueThreshold, validThreshold, functionIndex, ROI_z_value, DBScan_radius, lamba, groupName, subjectList)
    return jsonify(res_dict)

# fMri
@app.route('/api/getAllPropertiesForFMri')
def getAllPropertiesForFMri():
    # res_dict = {"code":0,"data":[{"message":"Sicken","checkval":False,"order":""},{"message":"Gender","checkval":False,"order":""},{"message":"Ages","checkval":False,"order":""},{"message":"Others","checkval":False,"order":""}],"message":"\u83b7\u53d6\u6210\u529f"}
    res_dict = {"code":0,"data":[{"message":"Sicken","checkval":False,"order":"","children":["Healthy","eMCI","lMCI","AD"]},{"message":"Gender","checkval":False,"order":"","children":["Female","Male"]},{"message":"Ages","checkval":False,"order":"","children":["50s","60s","70s","80s"]},{"message":"Sicken2","checkval":False,"order":"","children":["Healthy","Unhealthy"]}],"message":"\u83b7\u53d6\u6210\u529f"}
    return jsonify(res_dict)

@app.route('/api/updateSunburstDataForFMri/<jsonArr>')
def updateSunburstDataForFMri(jsonArr):
    res_dict = updateSunburstForFMri(jsonArr)
    return jsonify(res_dict)

@app.route('/api/getPValueDataForOneGroupForFMri/<groupIndex>/<groupName>')
def getPValueDataForOneGroupForFMri(groupIndex, groupName):
    res_dict = getPValueSingleForFMri(groupIndex, groupName)
    return jsonify(res_dict)

@app.route('/api/singleAnalyzeForOnefeatureForFMri/<featureIndex>/<pvalueThreshold>/<validThreshold>/<testIndex>/<groupName>/<subjectList>')
def singleAnalyzeForOnefeatureForFMri(featureIndex, pvalueThreshold, validThreshold, testIndex, groupName, subjectList):
    res_dict = getFeatureViewPValueSingleForFMri(featureIndex, pvalueThreshold, validThreshold, testIndex, groupName, subjectList)
    return jsonify(res_dict)

@app.route('/api/multiAnalyzeForOnefeatureForFMri/<featureIndex>/<pvalueThreshold>/<validThreshold>/<functionIndex>/<ROI_z_value>/<DBScan_radius>/<lamba>/<groupName>/<subjectList>')
def multiAnalyzeForOnefeatureForFMri(featureIndex, pvalueThreshold, validThreshold, functionIndex, ROI_z_value, DBScan_radius, lamba, groupName, subjectList):
    res_dict = getComparisonViewSingleForFMri(featureIndex, pvalueThreshold, validThreshold, functionIndex, ROI_z_value, DBScan_radius, lamba, groupName, subjectList)
    return jsonify(res_dict)
