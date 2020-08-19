//与后端进行交互的接口
import axios from 'axios';

const GET_REQUEST = 'get';
const POST_REQUEST = 'post';
//const dev_api_url = 'http://localhost:8888';
const dev_api_url = 'http://118.190.210.193:5050/api';
//const dev_api_url = 'http://192.168.0.118:5050/api';

//get/post,对传回来的数据进行callback函数操作
function request(url, params, type, callback){
    let func;
    if (type === GET_REQUEST){
        func = axios.get;
    }
    else if(type === POST_REQUEST){
        func = axios.post;
    }
    func(url, params).then((res) => {
        if (res.status === 200){
            // console.log(res);
            callback(res);
        }
        else{
            callback(null);
        }
    }).catch((err) => {
        callback(null, err);
    })
}

//初始化时获得属性列表
//data:[{message: 'Sicken',checkval: false,order: '',},{message: 'Gender',checkval: false,order: '',},{message: 'Ages',checkval: false,order: '',},{ message: 'Others',checkval: false,order: '',}]
//{"code":0,"data":[{"message":"Sicken","checkval":false,"order":""},{"message":"Gender","checkval":false,"order":""},{"message":"Ages","checkval":false,"order":""},{"message":"Others","checkval":false,"order":""}],"message":"获取成功"}
//修改为{"code":0,"data":[{"message":"Sicken","checkval":false,"order":"","children":["Healthy","AD"]},{"message":"Gender","checkval":false,"order":"","children":["Female","Male"]},{"message":"Ages","checkval":false,"order":"","children":[">70","<70"]}],"message":"\u83b7\u53d6\u6210\u529f"}
function getAllProperties(callback) {
    //const url = `${dev_api_url}/test5.php?`;
    const url = `${dev_api_url}/getAllPropertiesForFMri`;
    const params = {};
    // request(url, {params: params}, GET_REQUEST, callback);
    request(url, params, GET_REQUEST, callback);
}

//根据选择的properties次序得到sunburst的json,eg: propertiesArr=["Gender","Sicken","Ages"] (此处的"Gender"与上面函数message:'Gender'相对应)
//{"code":0,"data":{"name":"all","children":[{"name":"healthy","children":[{"name":"female","size":2},{"name":"male","size":2}]},{"name":"AD","children":[{"name":"female","size":5},{"name":"male","size":7}]}]},"propertiesArr":["a","b","c"],"message":"获取成功"}
function updateSunburstData(propertiesArr, callback){
    var jsonArr = JSON.stringify(propertiesArr);
    //const url = `${dev_api_url}/test4.php?propertiesArr=${jsonArr}`;
    const url = `${dev_api_url}/updateSunburstDataForFMri/${jsonArr}`;
    const params = {};
    // request(url, {params: params}, GET_REQUEST, callback);
    request(url, params, GET_REQUEST, callback);
}

//eg: subjectsArr=["all-healthy-female","all-healthy-male"]
//根据选择的分组得到两个组的三维数组(dataGroupOne,dataGroupTwo)。每个人的每个连接的[curvature, entropy, torsion, strength, length, name]的p-value值
//{"code":0,"dataGroupOne":[[[0.1,0.2,0.3,0.4,0.5,"a"],[0.2,0.4,0.5,0.3,0.4,"b"],[0.1,0.4,0.7,0.4,0.5,"c"]],[[0.2,0.4,0.4,0.5,0.3,"d"],[0.4,0.3,0.1,0.4,0.5,"e"],[0.4,0.5,0.5,0.7,0.3,"f"]],[[0.4,0.5,0.2,0.4,0.2,"h"],[0.4,0.5,0.4,0.3,0.7,"i"],[0.4,0.5,0.5,0.7,0.7,"j"]]],"dataGroupTwo":[[[0.2,0.4,0.1,0.2,0.3,"a"],[0.2,0.3,0.2,0.4,0.4,"b"],[0.1,0.4,0.7,0.2,0.4,"c"]],[[0.2,0.4,0.3,0.2,0.4,"d"],[0.4,0.3,0.1,0.2,0.4,"e"],[0.5,0.7,0.3,0.2,0.4,"f"]],[[0.2,0.4,0.2,0.4,0.2,"h"],[0.4,0.3,0.2,0.4,0.7,"i"],[0.5,0.7,0.7,0.2,0.4,"j"]],[[0.2,0.4,0.7,0.2,0.4,"k"],[0.4,0.3,0.2,0.4,0.7,"l"],[0.5,0.7,0.7,0.2,0.4,"m"]]],"subjectsArr":["all-healthy-female","all-healthy-male"],"message":"获取成功"}
function getPValueData(subjectsArr, callback) {
    var jsonArr = JSON.stringify(subjectsArr);
    //console.log('getPValueData', jsonArr);
    //const url = `${dev_api_url}/test6.php?subjectsArr=${jsonArr}`;
    const url = `${dev_api_url}/getPValueDataForFMri/${jsonArr}`;
    const params = {};
    // request(url, {params: params}, GET_REQUEST, callback);
    request(url, params, GET_REQUEST, callback);   
}

//eg: groupName="all-healthy-female"
//根据选择的分组得到该组的三维数组(dataGroupOne,dataGroupTwo)。每个人的每个连接的[curvature, entropy, torsion, strength, length, name]的p-value值
//{"code":0,"data":[[[0.1,0.2,0.3,0.4,0.5,"a"],[0.2,0.4,0.5,0.3,0.4,"b"],[0.1,0.4,0.7,0.4,0.5,"c"]],[[0.2,0.4,0.4,0.5,0.3,"d"],[0.4,0.3,0.1,0.4,0.5,"e"],[0.4,0.5,0.5,0.7,0.3,"f"]],[[0.4,0.5,0.2,0.4,0.2,"h"],[0.4,0.5,0.4,0.3,0.7,"i"],[0.4,0.5,0.5,0.7,0.7,"j"]]],"message":"获取成功"}
function getPValueDataForOneGroup(groupIndex, groupName, callback) {
    console.log('getPValueDataForOneGroup', groupName);
    const url = `${dev_api_url}/getPValueDataForOneGroupForFMri/${groupIndex}/${groupName}`;
    const params = {};
    // request(url, {params: params}, GET_REQUEST, callback);
    request(url, params, GET_REQUEST, callback);   
}

//根据p-value threshold、valid threshold计算得到范围内的人以及连接，计算variance, average, totalPeople, (过滤掉的连接记录为0)更新2D的数据， groupOneData, groupTwoData为原始数据
//{"code":0,"curvature":{"groupOneData":[508.4],"groupTwoData":[508.4],"pvalue":[508.4],"average":[508.4],"variance":[508.4],"totalPeople":[524.7857143]},"entropy":{"groupOneData":[508.4],"groupTwoData":[508.4],"pvalue":[508.4],"average":[508.4],"variance":[508.4],"totalPeople":[524.7857143]},"torsion":{"groupOneData":[508.4],"groupTwoData":[508.4],"pvalue":[508.4],"average":[508.4],"variance":[508.4],"totalPeople":[524.7857143]},"strength":{"groupOneData":[508.4],"groupTwoData":[508.4],"pvalue":[508.4],"average":[508.4],"variance":[508.4],"totalPeople":[524.7857143]},"length":{"groupOneData":[508.4],"groupTwoData":[508.4],"pvalue":[508.4],"average":[508.4],"variance":[508.4],"totalPeople":[524.7857143]},"message":"获取成功"}
//上述groupOneData等均是长度为2485的array，对应于0-0，0-1，0-2....69-69的roi连接
//testIndex="0" / "1" / "2",分别对应t-test, welch t-test, mann whiteney test
//每个特征多了"accuracy":[0.5]以及"selectConnection":[0,1数组，代表每个连接是否应用，长度2485]
//数组长度为2485
//最新:"curvature":{"groupOneData":[508.4],"groupTwoData":[508.4],"pvalue":[508.4],"average":[508.4],"variance":[508.4],"totalPeople":[524.7857143],"accuracy":[0.5,..],"AccSelectConnection":[1,...], "weight":[508.4,...], "WeightSelectConnection": [1,..]},
//weight给出真实值，绝对值我这边计算
function update2D(pvalueThreshold, validThreshold, testIndex, lamba, ROI_z_value, DBScan_radius, callback) {
    if(pvalueThreshold < 0) pvalueThreshold = 0;
    if(validThreshold < 0) validThreshold = 0;
    if(lamba < 0) lamba = 0;
    if(ROI_z_value < 0) ROI_z_value = 0;
    if(DBScan_radius < 0) DBScan_radius = 0;
    //const url = `${dev_api_url}/test7.php?pvalueThreshold=${pvalueThreshold}&validThreshold=${validThreshold}&testIndex=${testIndex}&lamba=${lamba}&ROIZValue=${ROI_z_value}&DBScanRadius=${DBScan_radius}`;
    const url = `${dev_api_url}/update2DForFMri/${pvalueThreshold}/${validThreshold}/${testIndex}/${lamba}/${ROI_z_value}/${DBScan_radius}`;
    const params = {};
    // request(url, {params: params}, GET_REQUEST, callback);
    request(url, params, GET_REQUEST, callback);
}

//featureIndex: 0=curvature,1=entropy,2=torsion,3=strength,3=length
//{"code":0,"data":{"originPvalue":[2.0], "pvalue":[1.0], "average":[508.4],"variance":[508.4],"totalPeople":[504]},"message":"获取成功"}
function singleAnalyzeForOnefeature(featureIndex, pvalueThreshold, validThreshold, testIndex, groupName2, subjectList2, callback) {
    if(pvalueThreshold < 0) pvalueThreshold = 0;
    if(validThreshold < 0) validThreshold = 0;
    var groupName = JSON.stringify(groupName2);
    var subjectList = JSON.stringify(subjectList2);
    //const url = `${dev_api_url}/test9.php?featureIndex=${featureIndex}&pvalueThreshold=${pvalueThreshold}&validThreshold=${validThreshold}&testIndex=${testIndex}`;
    const url = `${dev_api_url}/singleAnalyzeForOnefeatureForFMri/${featureIndex}/${pvalueThreshold}/${validThreshold}/${testIndex}/${groupName}/${subjectList}`;
    console.log('singleAnalyzeForOnefeature', url);
    const params = {};
    request(url, params, GET_REQUEST, callback);
}

//featureIndex: 0=curvature,1=entropy,2=torsion,3=strength,3=length
//functionIndex:  "GL-ROI": "0", "GL-Lobe": "1","GL-DBScan": "2", "Lasso": "3","ElasticNet": "4","Ridge Regression": "5"
//lambda默认为-1, 非0实数提供
//{"code":0,"data":{"accuracy":0.5, "lambda": 1,"AccSelectConnection":[1,...], "weight":[508.4,...], "WeightSelectConnection": [1,..]},"message":"获取成功"}
function multiAnalyzeForOnefeature(featureIndex, pvalueThreshold, validThreshold, functionIndex,ROI_z_value, DBScan_radius, lamba, groupName2,subjectList2, callback) {
    if(pvalueThreshold < 0) pvalueThreshold = 0;
    if(validThreshold < 0) validThreshold = 0;
    if(ROI_z_value < 0) ROI_z_value = 0;
    if(DBScan_radius < 0) DBScan_radius = 0;
    var groupName = JSON.stringify(groupName2);
    var subjectList = JSON.stringify(subjectList2);
    //const url = `${dev_api_url}/test10.php?featureIndex=${featureIndex}&pvalueThreshold=${pvalueThreshold}&validThreshold=${validThreshold}&ROIZValue=${ROI_z_value}&DBScanRadius=${DBScan_radius}`;
    const url = `${dev_api_url}/multiAnalyzeForOnefeatureForFMri/${featureIndex}/${pvalueThreshold}/${validThreshold}/${functionIndex}/${ROI_z_value}/${DBScan_radius}/${lamba}/${groupName}/${subjectList}`;
    console.log('multiAnalyzeForOnefeature', url);
    const params = {};
    request(url, params, GET_REQUEST, callback);
}


function getCirclePoint(totalData, circleNum, dmax, callback) {
    var R1 = 3.5;
    var R0 = 0;
    var jsonArr = JSON.stringify(totalData);
    //const url = `http://localhost:8888/test12.php?totalData=${jsonArr}&circleNum=${circleNum}`;
    const url = `${dev_api_url}/getCirclePointForFMri/${jsonArr}/${circleNum}/${R1}/${R0}/${dmax}`;
    console.log('getCirclePoint', url);
    const params = {};
    request(url, params, GET_REQUEST, callback);
}

/*
function getAccuracy(pvalueThreshold, validThreshold, callback){
    if(pvalueThreshold < 0) pvalueThreshold = 0;
    if(validThreshold < 0) validThreshold = 0;
    const url = `${dev_api_url}/test8.php?pvalueThreshold=${pvalueThreshold}&validThreshold=${validThreshold}`;
    const params = {};
    request(url, params, GET_REQUEST, callback);
}
*/
//点击散点图中某点
//返回数据如{"code":0,"data":[["balance","subjects","pvalue","featurename"],[10,8,0.1,"a"],[1,8,4.1,"b"],[5,4,2.1,"c"]],"femaleAD":3,"femaleHealthy":3,"maleAD":3,"maleHealthy":3,"message":"获取成功"}
//注意"balance"等词不可删除
function getDataWithProAndFea(properties, featurename, callback) {
    const url = `${dev_api_url}/test1.php?properties=${properties}&featurename=${featurename}`;
    const params = {};
    // request(url, {params: params}, GET_REQUEST, callback);
    request(url, params, GET_REQUEST, callback);
}

//选择属性
//返回数据如{"code":0,"data":[["balance","subjects","pvalue","featurename"],[10,8,0.1,"a"],[1,8,4.1,"b"],[5,4,2.1,"c"]],"femaleAD":4,"femaleHealthy":4,"maleAD":4,"maleHealthy":4,"message":"获取成功"}
function getDataWithPro(properties, callback) {
    const url = `${dev_api_url}/test2.php?properties=${properties}`;
    const params = {};
    // request(url, {params: params}, GET_REQUEST, callback);
    request(url, params, GET_REQUEST, callback);
}

//获取原始数据
//{"code":0,"data":[["balance","subjects","pvalue","featurename"],[5,8,0.1,"a"],[0,3,4.1,"b"],[1,4,2.1,"c"]],"femaleAD":5,"femaleHealthy":1,"maleAD":5,"maleHealthy":1,"message":"获取成功"}
function getOriginData(callback) {
    const url = `${dev_api_url}/test3.php`;
    const params = {};
    // request(url, {params: params}, GET_REQUEST, callback);
    request(url, params, GET_REQUEST, callback);
}

//从不同网址get/post数据并进行callback
function all_securities(callback){
    const url = `${dev_api_url}/all_securities`;
    const params = {};
    request(url, params, GET_REQUEST, callback);
}

function all_industries(callback){
    const url = `${dev_api_url}/all_industries`;
    const params = {};
    request(url, params, GET_REQUEST, callback);
}

function get_stock_info(code_or_name, callback){
    const url = `${dev_api_url}/stock_info/${code_or_name}`;
    const params = {};
    request(url, params, GET_REQUEST, callback);
}

// 根据 industry 获取所有股票的信息
function get_stocks_info(industry, callback){
    const url = `${dev_api_url}/industry/${industry}`;
    const params = {};
    request(url, params, GET_REQUEST, callback);
}

function get_prediction_data(start_date, end_date, kwargs, callback){
    const url = `${dev_api_url}/get_prediction_data`;
    const params = {"start_date": start_date, "end_date": end_date, "kwargs": kwargs};
    // request(url, {params: params}, GET_REQUEST, callback);
    request(url, params, POST_REQUEST, callback);
}

function get_bunch_single_backtest(start_date, end_date, kwargs, callback){
    const url = `${dev_api_url}/get_bunch_single_backtest`;
    const params = {"start_date": start_date, "end_date": end_date, "kwargs": kwargs};
    // request(url, {params: params}, GET_REQUEST, callback);
    request(url, params, POST_REQUEST, callback);
}

function get_backtest(start_date, end_date, kwargs, callback){
    const url = `${dev_api_url}/get_backtest`;
    const params = {"start_date":start_date, "end_date":end_date, "kwargs":kwargs};
    // request(url, {params:params}, GET_REQUEST, callback);
    request(url, params, POST_REQUEST, callback);
}

export default{
    getDataWithProAndFea,
    getDataWithPro,
    getOriginData,
    all_industries,
    all_securities,
    get_stock_info,
    get_stocks_info,
    get_prediction_data,
    get_bunch_single_backtest,
    get_backtest,
    updateSunburstData,
    getAllProperties,
    getPValueData,
    update2D,
    singleAnalyzeForOnefeature,
    multiAnalyzeForOnefeature,
    getPValueDataForOneGroup,
    getCirclePoint
}