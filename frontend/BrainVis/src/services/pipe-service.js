import Vue from 'vue';
import Vuex from 'vuex';

Vue.use(Vuex);

function deepCopy(parm){
    let res;
    if(Object.prototype.toString.call(parm) == "[object Array]"){   //数组
        res = [];
        for(var i = 0;i < parm.length;i++){
            res[i] = deepCopy(parm[i]);
        }
    }
    else if(Object.prototype.toString.call(parm) == "[object Object]"){  //对象
        res = {};
        for(var i in parm){
            res[i] = deepCopy(parm[i]);
        }
    }
    else{
        return parm;
    }
    return res;
};

function copy(res, source) {
    for (var i in source) {
        res[i] = source[i];
    }
};

//管理共享状态，集中式管理所有组件的状态
const store = new Vuex.Store({
    state: {
        width: 0,
        height: 0,
        stock_list: [],
        start_date: undefined,
        end_date: undefined,

        data_overview: undefined,
        data_individuals: undefined,
        data_aggragate: undefined,
        
        selected_factors:[],
        data_bunch_backtest: undefined,
        selected_stocks:[],
        data_backtest:[],

        selected_subjects:[], //nodes
        selected_subjects_name: [],
        subjects_valid: true,
        showFunction: "0",
        eyeside: "0",
        linksVal: "3",
        multiVal: "1",
        testVal: "0",
        multiGroupVal: "0",
        modelVal: "0",
        features: [
            {
                "index": "0",
                "name": 'Strength',
                "g1name": "",
                "g2name": "",
                "options": {},
                "data": {},
            },
            {
                "index":"1",
                "name": 'Length',
                "g1name": "",
                "g2name": "",
                "options": {},
                "data": {},
            },
            {
                "index":"2",
                "name": 'Curvature',
                "g1name": "",
                "g2name": "",
                "options": {},
                "data": {},
            },
            {
                "index":"3",
                "name": 'Torsion',
                "g1name": "",
                "g2name": "",
                "options": {},
                "data": {},
            },
            {
                "index":"4",
                "name": 'Entropy',
                "g1name": "",
                "g2name": "",
                "options": {},
                "data": {},
            }
        ],

        singlefeatures: [],
        multifeatures: [],
        feature_values: [],
        pValueThreshold: [50, 50, 50, 50, 50],
        validThreshold: [50, 50, 50, 50, 50],
        lambda: 50,
        ROIZValue: 20,
        DBScanRadius: 50,
        picBgOne: "",
        picBgTwo: "",
        beginROI: 0,
        endROI: 0,
        featureIndex: 0,
        g1name: '',
        g2name: '',

        localFeature: {
            "index": "0",
            "name": 'curvature',
            "data": {},
        },

        selected_index: 0,
        feature_index: 0,
        num: 0,
        self: [0, 70, 139, 207, 274, 340, 405, 469, 532, 594, 655, 715, 774, 832, 889, 945, 1000, 1054, 1107, 1159, 1210, 1260, 1309, 1357, 1404, 1450, 1495, 1539, 1582, 1624, 1665, 1705, 1744, 1782, 1819, 1855, 1890, 1924, 1957, 1989, 2020, 2050, 2079, 2107, 2134, 2160, 2185, 2209, 2232, 2254, 2275, 2295, 2314, 2332, 2349, 2365, 2380, 2394, 2407, 2419, 2430, 2440, 2449, 2457, 2464, 2470, 2475, 2479, 2482, 2484],
        needReDraw: false,
        dmax: 16,
        subjectList: {
            "groupOneList": [],
            "groupTwoList": [],
        },
        groupName: {
            "groupOneName": "",
            "groupTwoName": "",
        },
        picindex: 16,
        //testVal: "0",
        testValName: "t-test",
        multiGroupValName: "GL-ROI",
                testValPros: [
                    {
                        "label": "t-test",
                        "index": "0",
                    },
                    {
                        "label": "welch",
                        "index": "1",
                    },

                    /*
                    {
                        "message": "mann-whiteney",
                        "index": "2",
                    }
                    */
                ],
                multiGroupValPros: [
                    {
                        "label": "GL-ROI",
                        "index": "0",
                    },
                    {
                        "label": "GL-Lobe",
                        "index": "1",
                    },
                    /*
                    {
                        "label": "GL-DBScan",
                        "index": "2",
                    },
                    */
                    {
                        "label": "Lasso",
                        "index": "2",
                    },
                    {
                        "label": "ElasticNet",
                        "index": "3",
                    },
                    {
                        "label": "Ridge",
                        "index": "4",
                    },
                ],
    },
    mutations: {
        width(state, width){
            state.width = width;
        },
        height(state, height){
            state.height = height;
        },
        start_date(state, start_date){
            state.start_date = start_date;
        },
        end_date(state, end_date){
            state.end_date = end_date;
        },


        // control panel
        append_stock(state, stock) {
            // deduplicate
            var duplicated = false;
            // console.log('stock: ', stock);
            for (let i = 0; i < state.stock_list.length; i++) {
                let a = state.stock_list[i].ts_code;
                let b = stock.ts_code;
                if (a === b) {
                    duplicated = true;
                    console.log('break');
                    break;
                }
            }
            if (!duplicated) {
                state.stock_list.push(stock);
            }
        },

        drop_stock(state, ts_code) {
            // console.log('[before drop] stock: ', ts_code);
            let res = -1;
            for(let i=0;i<state.stock_list.length;i++){
                let stock = state.stock_list[i];
                if(stock.ts_code === ts_code){
                res = i;
                break;
                }
            }
            if (res >= 0) {
                state.stock_list.splice(res, 1);
            }
            // console.log('[after drop] stock: ', ts_code);
        },

        add_select_factor(state, factor_name){
            let duplicated = false;
            for (let i = 0; i < state.selected_factors.length; i++) {
                if (state.selected_factors[i] === factor_name) {
                    duplicated = true;
                    break;
                }
            }
            if (!duplicated) {
                state.selected_factors.push(factor_name);
            }
        },
        rm_selected_factor(state, factor_name){
            let res = -1;
            for(let i=0;i<state.selected_factors.length;i++){
                if (state.selected_factors[i] === factor_name) {
                    res = i;
                    break;
                }
            }
            if (res >= 0) {
                state.selected_factors.splice(res, 1);
            }
        },
        reset_selected_factors(state){
            state.selected_factors = [];
        },

        add_select_stock(state, ts_code){
            let duplicated = false;
            for (let i = 0; i < state.selected_stocks.length; i++) {
                if (state.selected_stocks[i] === ts_code) {
                    duplicated = true;
                    break;
                }
            }
            if (!duplicated) {
                state.selected_stocks.push(ts_code);
            }
        },
        rm_selected_stock(state, ts_code){
            let res = -1;
            for(let i=0;i<state.selected_stocks.length;i++){
                if (state.selected_stocks[i] === ts_code) {
                    res = i;
                    break;
                }
            }
            if (res >= 0) {
                state.selected_stocks.splice(res, 1);
            }
        },
        reset_selected_stocks(state){
            state.selected_stocks = [];
        },

        draw_overview(state, overview_data){
            state.data_overview = [];
            for(let dt in overview_data){
                state.data_overview.push(overview_data[dt]);
            }
        },

        draw_aggragate(state, aggragate_data){
            state.data_aggragate = aggragate_data;
        },

        draw_individuals(state, individuals_data){
            state.data_individuals = individuals_data;
        },

        draw_bunch_backtest(state, bunch_backtest_data){
            state.data_bunch_backtest = bunch_backtest_data;
        },

        draw_backtest(state, backtest_data){
            state.data_backtest.push(backtest_data);
        },

        reset_bunch_backtest(state){
            state.data_bunch_backtest = undefined;
        },

        reset_backtest(state){
            state.data_backtest = [];
        },
        get_selected_subjects_name(state) {
            state.selected_subjects_name = [];
            for (var i in state.selected_subjects){
                var n = deepCopy(state.selected_subjects[i]);
                var name = n.data.name;
                while(n.parent != null){
                    n = n.parent;
                    name = n.data.name + '-' + name;
                }
                state.selected_subjects_name.push(name);
            }
        },
        
        add_selected_subjects(state, node){
            let duplicated = true;
            //判断是否在内
            if(state.selected_subjects.length == 0) {
                duplicated = false;
            }
            for (let i = 0; i < state.selected_subjects.length; i++) {
                var node1 = state.selected_subjects[i];
                var node2 = node;
                while(node1 != null) {
                    if(node1.data.name != node2.data.name) {
                        duplicated = false;
                        break;
                    }
                    node1 = node1.parent;
                    node2 = node2.parent;
                }
                /*
                if (state.selected_subjects[i] == node) {
                    duplicated = true;
                    break;
                }*/
            }
            if (!duplicated) {
                //如果不在, 判断subjects数目是否大于2，如果大于等于2，清空，否则直接添加
                //添加后如果数目为2判断路径是否一致，如果不一致清空
                if (state.selected_subjects.length >= 2) {
                    state.selected_subjects = [];
                    state.subjects_valid = true;
                    //state.selected_subjects.push(node);
                }
                else if (state.selected_subjects.length == 0) {
                    //直接添加
                    state.subjects_valid = true;
                    state.selected_subjects.push(node);
                }
                else {
                    //判断是否合法
                    var isValid = true;
                    if(state.selected_subjects[0].depth == node.depth) {
                        var tmp1 = node.parent;
                        var tmp2 = state.selected_subjects[0].parent;
                        while(tmp1 != null) {
                            if(tmp1.data.name != tmp2.data.name) {
                                isValid = false
                                break;
                            }
                            tmp1 = tmp1.parent;
                            tmp2 = tmp2.parent;
                            //console.log(tmp1, tmp2);
                        }
                    }else {
                        isValid = false;
                    }
                    if (isValid) {
                        state.subjects_valid = true;
                        state.selected_subjects.push(node); //开头添加
                    }
                    else {
                        state.subjects_valid = false;
                        state.selected_subjects = [];
                    }
                }      
            }
            console.log('selected_subjects', state.selected_subjects);
            //get_selected_subjects_name(state);
            state.selected_subjects_name = [];
            for (var i in state.selected_subjects){
                var n = state.selected_subjects[i];
                var name = n.data.name;
                while(n.parent != null){
                    n = n.parent;
                    name = n.data.name + '-' + name;
                }
                state.selected_subjects_name.push(name);
            }
        },
        //移除
        rm_selected_subjects(state, node){
            let res = -1;
            for(let i=0;i<state.selected_subjects.length;i++){
                if (state.selected_subjects[i] === node) {
                    res = i;
                    break;
                }
            }
            if (res >= 0) {
                state.selected_subjects.splice(res, 1);
            }
        },
        reset_selected_subjects(state){
            state.selected_subjects = [];
            state.subjects_valid = true;
        },
        set_feature_values(state, values) {
            state.feature_values = values;
            //console.log('feature_values', state.feature_values);
        },
        set_subjectListOne(state, list) {
            state.subjectList.groupOneList = deepCopy(list);
        },
        set_subjectListTwo(state, list) {
            state.subjectList.groupTwoList = deepCopy(list);
        },
        set_groupNameOne(state, name) {
            state.groupName.groupOneName = deepCopy(name);
        },
        set_groupNameTwo(state, name) {
            state.groupName.groupTwoName = deepCopy(name);
        },
        delete_feature_values(state, index) {
            var res = [];
            for(var i in state.feature_values) {
                if(i != index) {
                    res.push(state.feature_values[i]);
                }
            }
            while(state.feature_values.length != 0){
                state.feature_values.pop();
            }
            for(var i in res) {
                state.feature_values.push(res[i]);
            }
        },
        set_features_data(state, data) {
            state.features[0].data = data[0];
            state.features[1].data = data[1];
            state.features[2].data = data[2];
            state.features[3].data = data[3];
            state.features[4].data = data[4];
        },
        set_localFeature_data(state, index) {
            state.localFeature.index = state.features[index].index;
            state.localFeature.name = state.features[index].name;
        },
        set_localFeature_data_for_single(state, data) {
            state.localFeature.data.originPvalue = data.originPvalue;
            state.localFeature.data.pvalue = data.pvalue;
            state.localFeature.data.average = data.average;
            state.localFeature.data.variance = data.variance;
            state.localFeature.data.totalPeople = data.totalPeople;
        },
        set_localFeature_data_for_multi(state, data) {
            state.localFeature.data.accuracy = data.accuracy;
            state.localFeature.data.AccSelectConnection = data.AccSelectConnection;
            state.localFeature.data.weight = data.weight;
            state.localFeature.data.WeightSelectConnection = data.WeightSelectConnection;
        },
        set_features_g1name(state, name) {
            name = name.substr(0,1).toUpperCase() + name.substr(1, name.length);
            for(var i in state.features){
                state.features[i].g1name = name;
            }
        },
        set_features_g2name(state, name) {
            name = name.substr(0,1).toUpperCase() + name.substr(1, name.length);
            for(var i in state.features){
                state.features[i].g2name = name;
            }
        },
        //添加singlefeatures
        add_singlefeatures(state, index) {
            var obj = new Object();
            obj.index = state.features[index].index;
            obj.name = state.features[index].name;
            obj.g1name = state.features[index].g1name;
            obj.g2name = state.features[index].g2name;
            obj.data = new Object();
            obj.data.num = state.num++;
            obj.linksVal = state.linksVal;
            obj.testVal = state.testVal;
            obj.testValName = state.testValPros[state.testVal].label;
            obj.multiGroupVal = state.multiGroupVal;
            obj.multiGroupValName = state.multiGroupValPros[Number(state.multiGroupVal)].label;
            obj.modelVal = state.modelVal;
            obj.multiVal = state.multiVal;
            obj.ROIZValue = state.ROIZValue;
            obj.DBScanRadius = state.DBScanRadius;
            obj.showFunction = state.showFunction;
            obj.eyeside = state.eyeside;
            obj.singleLoading = false;
            obj.multiLoading = false;
            obj.multifilterRate = [0, 100];
            obj.singlefilterRate = [0,100];
            obj.sync = [];
            obj.filtersingledataLen = [0];
            obj.filtermultidataLen = [0];
            obj.loadsingle = false;
            obj.loadmulti = false;
            obj.lambda = [-1];
            obj.minLambda = 0;
            obj.maxLambda = 10;
            obj.message = "";
            obj.subjectList = deepCopy(state.subjectList);
            obj.groupName = deepCopy(state.groupName);
            state.singlefeatures.push(obj);
            state.feature_index = Number(obj.index);
            console.log("multiGroupValName", obj.multiGroupValName);
        },
        //复制index和name
        set_selectedFeature_data(state, index) {
            state.singlefeatures[state.selected_index].index = state.features[index].index;
            state.singlefeatures[state.selected_index].name = state.features[index].name;
            state.singlefeatures[state.selected_index].g1name = state.features[index].g1name;
            state.singlefeatures[state.selected_index].g2name = state.features[index].g2name;
        },
        //获取单变量
        set_selectedFeature_data_for_single(state, [data, index]) {
            //console.log('singlefeatures', state.singlefeatures, index);
            state.singlefeatures[index].data.originPvalue = data.originPvalue;
            state.singlefeatures[index].data.pvalue = data.pvalue;
            state.singlefeatures[index].data.average = data.average;
            state.singlefeatures[index].data.variance = data.variance;
            state.singlefeatures[index].data.totalPeople = data.totalPeople;

            state.singlefeatures[index].data.averageGroupOne = data.averageGroupOne;
            state.singlefeatures[index].data.averageGroupTwo = data.averageGroupTwo;

            state.singlefeatures[index].data.varianceGroupOne = data.varianceGroupOne;
            state.singlefeatures[index].data.varianceGroupTwo = data.varianceGroupTwo;

            state.singlefeatures[index].data.totalPeopleGroupOne = data.totalPeopleGroupOne;
            state.singlefeatures[index].data.totalPeopleGroupTwo = data.totalPeopleGroupTwo;
            state.singlefeatures[index].singleLoading = false;
        },
        set_selectedFeature_data_for_multi(state, [data, index, message]) {
            state.singlefeatures[index].data.accuracy = data.accuracy;
            state.singlefeatures[index].data.AccSelectConnection = data.AccSelectConnection;
            state.singlefeatures[index].data.weight = data.weight;
            state.singlefeatures[index].data.WeightSelectConnection = data.WeightSelectConnection;
            state.singlefeatures[index].message = message;
            state.singlefeatures[index].multiLoading = false;
            //state.singlefeatures[index].lambda = data.lambda;
        },
        set_selectedFeature_lambda_for_multi(state, [lambda, index]) {
            //state.singlefeatures[index].lambda[0] = (Math.log(lambda) - Math.log(0.0001)) / (Math.log(10) - Math.log(0.0001)) * 100;
            var maxLambda = lambda * 100.0;
            var minLambda = lambda / 100.0;
            state.singlefeatures[index].lambda[0] = (Math.log(lambda) - Math.log(minLambda)) / (Math.log(maxLambda) - Math.log(minLambda)) * 100;
            state.singlefeatures[index].maxLambda = Math.log(maxLambda);
            state.singlefeatures[index].minLambda = Math.log(minLambda);
        },
        set_selectedFeature_singleLoading(state, [index, value]) {
            state.singlefeatures[index].singleLoading = value;
        },
        set_selectedFeature_multiLoading(state, [index, value]) {
            state.singlefeatures[index].multiLoading = value;
        },
        add_features(state, index) {
            //state.multifeatures.push(deepCopy(state.features[index]));
            var obj = new Object();
            obj.index = state.features[index].index;
            obj.name = state.features[index].name;
            obj.g1name = state.features[index].g1name;
            obj.g2name = state.features[index].g2name;
            obj.data = new Object();
            obj.data.data = state.features[index].data.data;
            obj.singleFunc = state.features[index].singleFunc;
            //obj.data.groupOneData = state.features[index].data.groupOneData;
            //obj.data.groupTwoData = state.features[index].data.groupTwoData;
            obj.data.multidata = state.features[index].data.multidata;
            obj.data.weight = state.features[index].data.weight;
            obj.data.selectConnection = state.features[index].data.selectConnection;
            obj.multidata = state.features[index].multidata;
            state.singlefeatures.push(deepCopy(obj));
            //state.singlefeatures.push(deepCopy(state.features[index]));
        },
        add_localFeature(state) {
            /*
            var obj = new Object();
            obj.index = state.localFeature.index;
            obj.name = state.localFeature.name;
            obj.data = new Object();
            obj.data.data = state.localFeature.data.data;
            obj.singleFunc = state.localFeature.singleFunc;
            obj.data.multidata = state.localFeature.data.multidata;
            obj.data.weight = state.localFeature.data.weight;
            obj.data.selectConnection = state.localFeature.data.selectConnection;
            obj.multidata = state.localFeature.multidata;
            obj.data.origindata = state.localFeature.data.origindata;
            state.singlefeatures.push(deepCopy(obj));
            */
            state.singlefeatures.push(deepCopy(state.localFeature));
        },
        delete_singlefeatures(state, index) {
            //判断selected_index是否大于等于index，如果是，selected_index -= 1, 否则不变
            if (state.selected_index == index && index == 0) {
                state.selected_index = 0;
                state.feature_index = state.singlefeatures[state.selected_index].index;
            }
            else if (state.selected_index >= index) {
                state.selected_index = state.selected_index - 1;
                state.feature_index = state.singlefeatures[state.selected_index].index;
            }
            else if (state.selected_index == -1) {
                state.selected_index = Math.max(0, state.singlefeatures.length - 2);
                state.feature_index = state.singlefeatures[state.selected_index].index;
            }
            /*
            //console.log('delete_singlefeatures', state.singlefeatures[index]);
            for(var i = 0; i < state.singlefeatures.length; i++) {
                if (i == state.singlefeatures.length - 1) {
                    state.singlefeatures.pop();
                    break;
                }
                else if(i >= index) {
                    //console.log('i',i, state.singlefeatures[i]);
                    //Object.assign({}, state.singlefeatures[i], state.singlefeatures[i+1]);
                    state.singlefeatures[i].index = "" + state.singlefeatures[i+1].index;
                    state.singlefeatures[i].name = "" + state.singlefeatures[i+1].name;
                    //state.singlefeatures[i].data = new Object();
                    //state.singlefeatures[i].data.originPvalue = deepCopy(state.singlefeatures[i+1].data.originPvalue);
                    //state.singlefeatures[i].data.pvalue = deepCopy(state.singlefeatures[i+1].data.pvalue);
                    //state.singlefeatures[i].data.average = deepCopy(state.singlefeatures[i+1].data.average);
                    //state.singlefeatures[i].data.variance = deepCopy(state.singlefeatures[i+1].data.variance);
                    //state.singlefeatures[i].data.totalPeople = deepCopy(state.singlefeatures[i+1].data.totalPeople);
                    state.singlefeatures[i].data.data = deepCopy(state.singlefeatures[i+1].data.data);
                    state.singlefeatures[i].data.origindata = deepCopy(state.singlefeatures[i+1].data.origindata);
                    //state.singlefeatures[i].data.accuracy = deepCopy(state.singlefeatures[i+1].data.accuracy);
                    //state.singlefeatures[i].data.AccSelectConnection = deepCopy(state.singlefeatures[i+1].data.AccSelectConnection);
                    //state.singlefeatures[i].data.weight = deepCopy(state.singlefeatures[i+1].data.weight);
                    //state.singlefeatures[i].data.WeightSelectConnection = deepCopy(state.singlefeatures[i+1].data.WeightSelectConnection);
                    //state.singlefeatures[i].data.multidata = deepCopy(state.singlefeatures[i+1].data.multidata);
                    //state.singlefeatures[i].data.selectConnection = deepCopy(state.singlefeatures[i+1].data.selectConnection);
                    //state.singlefeatures[i].data.num = 1 + state.singlefeatures[i].data.num;
                    //state.singlefeatures[i].data = JSON.parse(JSON.stringify(state.singlefeatures[i].data));
                    Vue.set(state.singlefeatures[i], 'data', state.singlefeatures[i].data);
                    //Vue.set(state.singlefeatures[i].data, 'data', state.singlefeatures[i+1].data.data);
                    state.singlefeatures[i].singleFunc = "" + state.singlefeatures[i+1].singleFunc;
                    state.singlefeatures[i].multiFunc = "" + state.singlefeatures[i+1].multiFunc;
                    
                } 
            }
            */
           
            state.singlefeatures = state.singlefeatures.filter(function(x, i){return i != index});
            
            if(state.singlefeatures.length > 0 && state.singlefeatures[state.selected_index].hasOwnProperty('linksVal')){
                console.log('load');
                state.linksVal = state.singlefeatures[state.selected_index].linksVal;
                state.testVal = state.singlefeatures[state.selected_index].testVal;
                state.multiGroupVal = state.singlefeatures[state.selected_index].multiGroupVal;
                state.testValName = state.singlefeatures[state.selected_index].testValName;
                state.multiGroupValName = state.singlefeatures[state.selected_index].multiGroupValName;
                state.modelVal = state.singlefeatures[state.selected_index].modelVal;
                state.multiVal = state.singlefeatures[state.selected_index].multiVal;
                state.ROIZValue = state.singlefeatures[state.selected_index].ROIZValue;
                state.DBScanRadius = state.singlefeatures[state.selected_index].DBScanRadius;
            }

            /*
            var res = deepCopy(state.singlefeatures.filter(function(x,i) {return i != index;}));
            while(state.singlefeatures.length != 0){
                state.singlefeatures.pop();
            }
            for(var i in res) {
                state.singlefeatures.push(res[i]);
            }
            */
        },
        set_singlefeatures(state, val) {
            state.singlefeatures = val;
        },
        delete_multifeatures(state, index) {
            var res = deepCopy(state.multifeatures.filter(function(x,i) {return i != index;}));
            while(state.multifeatures.length != 0){
                state.multifeatures.pop();
            }
            for(var i in res) {
                state.multifeatures.push(res[i]);
            }
        },
        set_pValueThreshold(state, value) {
            console.log('pValueThreshold', value);
            state.pValueThreshold = value;
        },
        set_validThreshold(state, value) {
            console.log('validThreshold', value);
            state.validThreshold = value;
        },
        set_lambda(state, value) {
            state.lambda = value;
        },
        set_ROIZValue(state, value) {
            state.ROIZValue = value;
        },
        set_DBScanRadius(state, value) {
            state.DBScanRadius = value;
        },
        set_features_data_data(state, index) {
            if(index == 0) {
                for(var i in state.features) {
                    var obj = {};
                    state.features[i].data.data = state.features[i].data.average;
                    state.features[i].data = JSON.parse(JSON.stringify(state.features[i].data));
                    state.features[i].singleFunc = "Mean";
                }
            }
            else if(index == 1) {
                for(var i in state.features) {
                    state.features[i].data.data = state.features[i].data.variance;
                    state.features[i].data = JSON.parse(JSON.stringify(state.features[i].data));
                    state.features[i].singleFunc = "Std.";
                }
            }
            else if(index == 2) {
                for(var i in state.features) {
                    state.features[i].data.data = state.features[i].data.totalPeople;
                    state.features[i].data = JSON.parse(JSON.stringify(state.features[i].data));
                    state.features[i].singleFunc = "#Subject";
                }
            }
            else if(index == 3) {
                for(var i in state.features) {
                    state.features[i].data.data = state.features[i].data.pvalue;
                    state.features[i].data = JSON.parse(JSON.stringify(state.features[i].data));
                    state.features[i].singleFunc = "p-value";
                }
            }
            //console.log('set_features_data_data', state.features);
        },
        set_localFeature_data_data(state, index) {
            if(index == 0) {
                state.localFeature.data.data = state.localFeature.data.average;
                state.localFeature.data.origindata = state.localFeature.data.average;
                state.localFeature.data = JSON.parse(JSON.stringify(state.localFeature.data));
                state.localFeature.singleFunc = "Mean";
            }
            else if(index == 1) {
                state.localFeature.data.data = state.localFeature.data.variance;
                state.localFeature.data.origindata = state.localFeature.data.variance;
                state.localFeature.data = JSON.parse(JSON.stringify(state.localFeature.data));
                state.localFeature.singleFunc = "Std.";
            }
            else if(index == 2) {
                state.localFeature.data.data = state.localFeature.data.totalPeople;
                state.localFeature.data.origindata = state.localFeature.data.totalPeople;
                state.localFeature.data = JSON.parse(JSON.stringify(state.localFeature.data));
                state.localFeature.singleFunc = "#Subject";
            }
            else if(index == 3) {
                state.localFeature.data.data = state.localFeature.data.pvalue;
                state.localFeature.data.origindata = state.localFeature.data.originPvalue;
                state.localFeature.data = JSON.parse(JSON.stringify(state.localFeature.data));
                state.localFeature.singleFunc = "p-value";
            }
        },
        set_selectedFeature_data_data(state, [index, selected_index]) {
            //展示界面为单变量多变量时
            if(state.showFunction == 0) {
                state.singlefeatures[selected_index].sidebysideMax = 0;
                state.singlefeatures[selected_index].sidebysideMin = 0;
                state.singlefeatures[selected_index].sidebysideTrueMax = 0;
                if(index == 0) {
                    state.singlefeatures[selected_index].data.data = state.singlefeatures[selected_index].data.average;
                    state.singlefeatures[selected_index].filterSingleData = deepCopy(state.singlefeatures[selected_index].data.data);
                    state.singlefeatures[selected_index].data.origindata = state.singlefeatures[selected_index].data.average;
                    state.singlefeatures[selected_index].data = JSON.parse(JSON.stringify(state.singlefeatures[selected_index].data));
                    state.singlefeatures[selected_index].singleFunc = "Mean";
                }
                else if(index == 1) {
                    state.singlefeatures[selected_index].data.data = state.singlefeatures[selected_index].data.variance;
                    state.singlefeatures[selected_index].filterSingleData = deepCopy(state.singlefeatures[selected_index].data.data);
                    state.singlefeatures[selected_index].data.origindata = state.singlefeatures[selected_index].data.variance;
                    state.singlefeatures[selected_index].data = JSON.parse(JSON.stringify(state.singlefeatures[selected_index].data));
                    state.singlefeatures[selected_index].singleFunc = "Std.";
                }
                else if(index == 2) {
                    state.singlefeatures[selected_index].data.data = state.singlefeatures[selected_index].data.totalPeople;
                    state.singlefeatures[selected_index].filterSingleData = deepCopy(state.singlefeatures[selected_index].data.data);
                    state.singlefeatures[selected_index].data.origindata = state.singlefeatures[selected_index].data.totalPeople;
                    state.singlefeatures[selected_index].data = JSON.parse(JSON.stringify(state.singlefeatures[selected_index].data));
                    state.singlefeatures[selected_index].singleFunc = "#Subject";
                }
                else if(index == 3) {
                    state.singlefeatures[selected_index].data.data = state.singlefeatures[selected_index].data.pvalue;
                    state.singlefeatures[selected_index].filterSingleData = deepCopy(state.singlefeatures[selected_index].data.data);
                    state.singlefeatures[selected_index].data.origindata = state.singlefeatures[selected_index].data.originPvalue;
                    state.singlefeatures[selected_index].data = JSON.parse(JSON.stringify(state.singlefeatures[selected_index].data));
                    state.singlefeatures[selected_index].singleFunc = "p-value";
                    //console.log('pvalue', state.singlefeatures[selected_index]);
                }
            }
            else {
                var ones = [];
                var negative = [];
                for(var i = 0; i < 2485; i++) {
                    ones.push(1);
                    negative.push(1);
                }

                if(index == 0) {
                    state.singlefeatures[selected_index].data.data = state.singlefeatures[selected_index].data.averageGroupOne;
                    state.singlefeatures[selected_index].filterSingleData = deepCopy(state.singlefeatures[selected_index].data.data);
                    state.singlefeatures[selected_index].data.origindata = state.singlefeatures[selected_index].data.averageGroupOne;

                    state.singlefeatures[selected_index].data.multidata = state.singlefeatures[selected_index].data.averageGroupTwo;
                    state.singlefeatures[selected_index].filterMultiData = deepCopy(state.singlefeatures[selected_index].data.multidata);
                    state.singlefeatures[selected_index].data.multisign = negative;
                    state.singlefeatures[selected_index].data.selectConnection = ones;
                    state.singlefeatures[selected_index].multiFunc = "Mean";

                    state.singlefeatures[selected_index].data = JSON.parse(JSON.stringify(state.singlefeatures[selected_index].data));
                    state.singlefeatures[selected_index].singleFunc = "Mean";

                    
                }
                else if(index == 1) {
                    state.singlefeatures[selected_index].data.data = state.singlefeatures[selected_index].data.varianceGroupOne;
                    state.singlefeatures[selected_index].filterSingleData = deepCopy(state.singlefeatures[selected_index].data.data);
                    state.singlefeatures[selected_index].data.origindata = state.singlefeatures[selected_index].data.varianceGroupOne;

                    state.singlefeatures[selected_index].data.multidata = state.singlefeatures[selected_index].data.varianceGroupTwo;
                    state.singlefeatures[selected_index].filterMultiData = deepCopy(state.singlefeatures[selected_index].data.multidata);
                    state.singlefeatures[selected_index].data.multisign = negative;
                    state.singlefeatures[selected_index].data.selectConnection = ones;
                    state.singlefeatures[selected_index].multiFunc = "Std.";

                    state.singlefeatures[selected_index].data = JSON.parse(JSON.stringify(state.singlefeatures[selected_index].data));
                    state.singlefeatures[selected_index].singleFunc = "Std.";

                }
                else if(index == 2) {
                    state.singlefeatures[selected_index].data.data = state.singlefeatures[selected_index].data.totalPeopleGroupOne;
                    state.singlefeatures[selected_index].filterSingleData = deepCopy(state.singlefeatures[selected_index].data.data);
                    state.singlefeatures[selected_index].data.origindata = state.singlefeatures[selected_index].data.totalPeopleGroupOne;

                    state.singlefeatures[selected_index].data.multidata = state.singlefeatures[selected_index].data.totalPeopleGroupTwo;
                    state.singlefeatures[selected_index].filterMultiData = deepCopy(state.singlefeatures[selected_index].data.multidata);
                    state.singlefeatures[selected_index].data.multisign = negative;
                    state.singlefeatures[selected_index].data.selectConnection = ones;
                    state.singlefeatures[selected_index].multiFunc = "#Subject";

                    state.singlefeatures[selected_index].data = JSON.parse(JSON.stringify(state.singlefeatures[selected_index].data));
                    state.singlefeatures[selected_index].singleFunc = "#Subject";

                }
                else if(index == 3) {
                    state.singlefeatures[selected_index].data.data = state.singlefeatures[selected_index].data.pvalue;
                    state.singlefeatures[selected_index].filterSingleData = deepCopy(state.singlefeatures[selected_index].data.data);
                    state.singlefeatures[selected_index].data.origindata = state.singlefeatures[selected_index].data.originPvalue;

                    state.singlefeatures[selected_index].data.multidata = state.singlefeatures[selected_index].data.pvalue;
                    state.singlefeatures[selected_index].filterMultiData = deepCopy(state.singlefeatures[selected_index].data.multidata);
                    state.singlefeatures[selected_index].data.multisign = state.singlefeatures[selected_index].data.pvalue;
                    state.singlefeatures[selected_index].data.selectConnection = ones;
                    state.singlefeatures[selected_index].multiFunc = "p-value";

                    state.singlefeatures[selected_index].data = JSON.parse(JSON.stringify(state.singlefeatures[selected_index].data));
                    state.singlefeatures[selected_index].singleFunc = "p-value";
                    //console.log('pvalue', state.singlefeatures[selected_index]);
                }
                //console.log('min max', state.singlefeatures[selected_index].sidebysideMin, state.singlefeatures[selected_index].sidebysideMax);
            }
            state.singlefeatures[selected_index].loadsingle = true;
            if(state.singlefeatures[selected_index].singleFunc == state.singlefeatures[selected_index].multiFunc && state.singlefeatures[selected_index].loadmulti) {
                //计算sidebysidemin sidebysidemax
                state.singlefeatures[selected_index].sameFunc = 1;
                var total = [];
                for(var i in state.singlefeatures[selected_index].data.data){
                    if(state.self.indexOf(Number(i)) == -1){
                        if(Math.abs(state.singlefeatures[selected_index].data.data[i]) > 0)
                            total.push(Math.abs(state.singlefeatures[selected_index].data.data[i]));
                        if(Math.abs(state.singlefeatures[selected_index].data.multidata[i]) > 0)
                            total.push(Math.abs(state.singlefeatures[selected_index].data.multidata[i]));
                    }
                }
                if(total.length == 0) {
                    state.singlefeatures[selected_index].sidebysideMin = 0;
                    state.singlefeatures[selected_index].sidebysideMax = 0;
                    state.singlefeatures[selected_index].sidebysideTrueMax = 0;
                }
                else {
                    total.sort(function(xi, xj){ return xi - xj;});
                    state.singlefeatures[selected_index].sidebysideMin = total[Math.floor(total.length * 0.01)];
                    state.singlefeatures[selected_index].sidebysideMax = total[Math.floor(total.length * 0.99)];
                    state.singlefeatures[selected_index].sidebysideTrueMax = total[total.length - 1];
                }

            }
            else {
                state.singlefeatures[selected_index].sameFunc = 0;
                state.singlefeatures[selected_index].sidebysideMin = 0;
                state.singlefeatures[selected_index].sidebysideMax = 0;
                state.singlefeatures[selected_index].sidebysideTrueMax = 0;
            }
        },
        set_features_data_multidata(state, index) {
            if (index == 0) {
                for(var i in state.features) {
                    var obj = {};
                    state.features[i].data.multidata = state.features[i].data.weight;
                    state.features[i].data.selectConnection = state.features[i].data.WeightSelectConnection;
                    state.features[i].data = JSON.parse(JSON.stringify(state.features[i].data));
                    state.features[i].multiFunc = "Weight";
                }
            }
            else if (index == 2) {
                for(var i in state.features) {
                    var obj = {};
                    state.features[i].data.multidata = state.features[i].data.accuracy;
                    state.features[i].data.selectConnection = state.features[i].data.AccSelectConnection;
                    state.features[i].data = JSON.parse(JSON.stringify(state.features[i].data));
                    state.features[i].multiFunc = "Accuracy";
                }
            }
            else if (index == 1) {
                for(var i in state.features) {
                    var obj = {};
                    state.features[i].data.multidata = state.features[i].data.pvalue;
                    state.features[i].data.selectConnection = state.features[i].data.WeightSelectConnection;
                    state.features[i].data = JSON.parse(JSON.stringify(state.features[i].data));
                    state.features[i].multiFunc = "p-value";
                }
            }
            console.log('set_features_data_multidata', state.features);
        },
        set_localFeature_data_multidata(state, index) {
            if (index == 0) {
                var obj = {};
                state.localFeature.data.multidata = state.localFeature.data.weight;
                state.localFeature.data.selectConnection = state.localFeature.data.WeightSelectConnection;
                state.localFeature.data = JSON.parse(JSON.stringify(state.localFeature.data));
                state.localFeature.multiFunc = "Weight";
            }
            else if (index == 2) {
                var obj = {};
                state.localFeature.data.multidata = state.localFeature.data.accuracy;
                state.localFeature.data.selectConnection = state.localFeature.data.AccSelectConnection;
                state.localFeature.data = JSON.parse(JSON.stringify(state.localFeature.data));
                state.localFeature.multiFunc = "Accuracy";
            }
            else if (index == 1) {
                var obj = {};
                state.localFeature.data.multidata = state.localFeature.data.pvalue;
                state.localFeature.data.selectConnection = state.localFeature.data.WeightSelectConnection;
                state.localFeature.data = JSON.parse(JSON.stringify(state.localFeature.data));
                state.localFeature.multiFunc = "p-value";
            }

        },
        set_selectedFeature_data_multidata(state, [index, selected_index]) {
            if(state.showFunction == 0) {
                state.singlefeatures[selected_index].sidebysideMax = 0;
                state.singlefeatures[selected_index].sidebysideMin = 0;
                state.singlefeatures[selected_index].sidebysideTrueMax = 0;

                if (index == 0) {
                    var obj = {};
                    state.singlefeatures[selected_index].data.multidata = state.singlefeatures[selected_index].data.weight;
                    state.singlefeatures[selected_index].filterMultiData = deepCopy(state.singlefeatures[selected_index].data.multidata);
                    state.singlefeatures[selected_index].data.multisign = state.singlefeatures[selected_index].data.weight;
                    state.singlefeatures[selected_index].data.selectConnection = state.singlefeatures[selected_index].data.WeightSelectConnection;
                    state.singlefeatures[selected_index].data = JSON.parse(JSON.stringify(state.singlefeatures[selected_index].data));
                    state.singlefeatures[selected_index].multiFunc = "Weight";
                }
                else if (index == 2) {
                    var obj = {};
                    state.singlefeatures[selected_index].data.multidata = state.singlefeatures[selected_index].data.accuracy;
                    state.singlefeatures[selected_index].filterMultiData = deepCopy(state.singlefeatures[selected_index].data.multidata);
                    state.singlefeatures[selected_index].data.multisign = state.singlefeatures[selected_index].data.accuracy;
                    state.singlefeatures[selected_index].data.selectConnection = state.singlefeatures[selected_index].data.AccSelectConnection;
                    state.singlefeatures[selected_index].data = JSON.parse(JSON.stringify(state.singlefeatures[selected_index].data));
                    state.singlefeatures[selected_index].multiFunc = "Accuracy";
                }
                else if (index == 1) {
                    var obj = {};
                    state.singlefeatures[selected_index].data.multidata = state.singlefeatures[selected_index].data.pvalue;
                    state.singlefeatures[selected_index].filterMultiData = deepCopy(state.singlefeatures[selected_index].data.multidata);
                    state.singlefeatures[selected_index].data.multisign = state.singlefeatures[selected_index].data.pvalue;
                    state.singlefeatures[selected_index].data.selectConnection = state.singlefeatures[selected_index].data.WeightSelectConnection;
                    state.singlefeatures[selected_index].data = JSON.parse(JSON.stringify(state.singlefeatures[selected_index].data));
                    state.singlefeatures[selected_index].multiFunc = "p-value";
                }
            }
            else {
                var ones = [];
                var negative = [];
                for(var i = 0; i < 2485; i++) {
                    ones.push(1);
                    negative.push(1);
                }
                if(state.linksVal == 0) {
                    state.singlefeatures[selected_index].data.multidata = state.singlefeatures[selected_index].data.averageGroupTwo;
                    state.singlefeatures[selected_index].filterMultiData = deepCopy(state.singlefeatures[selected_index].data.multidata);
                    state.singlefeatures[selected_index].data.multisign = negative;
                    state.singlefeatures[selected_index].data.selectConnection = ones;
                    state.singlefeatures[selected_index].data = JSON.parse(JSON.stringify(state.singlefeatures[selected_index].data));
                    state.singlefeatures[selected_index].multiFunc = "Mean";
                    
                }
                else if(state.linksVal == 1) {
                    state.singlefeatures[selected_index].data.multidata = state.singlefeatures[selected_index].data.varianceGroupTwo;
                    state.singlefeatures[selected_index].filterMultiData = deepCopy(state.singlefeatures[selected_index].data.multidata);
                    state.singlefeatures[selected_index].data.multisign = negative;
                    state.singlefeatures[selected_index].data.selectConnection = ones;
                    state.singlefeatures[selected_index].data = JSON.parse(JSON.stringify(state.singlefeatures[selected_index].data));
                    state.singlefeatures[selected_index].multiFunc = "Std.";
                   
                }
                else if(state.linksVal == 2) {
                    state.singlefeatures[selected_index].data.multidata = state.singlefeatures[selected_index].data.totalPeopleGroupTwo;
                    state.singlefeatures[selected_index].filterMultiData = deepCopy(state.singlefeatures[selected_index].data.multidata);
                    state.singlefeatures[selected_index].data.multisign = negative;
                    state.singlefeatures[selected_index].data.selectConnection = ones;
                    state.singlefeatures[selected_index].data = JSON.parse(JSON.stringify(state.singlefeatures[selected_index].data));
                    state.singlefeatures[selected_index].multiFunc = "#Subject";
                    
                }
                else if(state.linksVal == 3) {
                    state.singlefeatures[selected_index].data.multidata = state.singlefeatures[selected_index].data.pvalue;
                    state.singlefeatures[selected_index].filterMultiData = deepCopy(state.singlefeatures[selected_index].data.multidata);
                    state.singlefeatures[selected_index].data.multisign = state.singlefeatures[selected_index].data.pvalue;
                    state.singlefeatures[selected_index].data.selectConnection = ones;
                    state.singlefeatures[selected_index].data = JSON.parse(JSON.stringify(state.singlefeatures[selected_index].data));
                    state.singlefeatures[selected_index].multiFunc = "p-value";
                    
                }
                //console.log('min max', state.singlefeatures[selected_index].sidebysideMin, state.singlefeatures[selected_index].sidebysideMax);
            }
            state.singlefeatures[selected_index].loadmulti = true;
            if(state.singlefeatures[selected_index].singleFunc == state.singlefeatures[selected_index].multiFunc && state.singlefeatures[selected_index].loadsingle) {
                //计算sidebysidemin sidebysidemax
                state.singlefeatures[selected_index].sameFunc = 1;
                var total = [];
                for(var i in state.singlefeatures[selected_index].data.data){
                    if(state.self.indexOf(Number(i)) == -1){
                        if(Math.abs(state.singlefeatures[selected_index].data.data[i]) > 0)
                            total.push(Math.abs(state.singlefeatures[selected_index].data.data[i]));
                        if(Math.abs(state.singlefeatures[selected_index].data.multidata[i]) > 0)
                            total.push(Math.abs(state.singlefeatures[selected_index].data.multidata[i]));
                    }
                }
                if(total.length == 0) {
                    state.singlefeatures[selected_index].sidebysideMin = 0;
                    state.singlefeatures[selected_index].sidebysideMax = 0;
                    state.singlefeatures[selected_index].sidebysideTrueMax = 0;
                }
                else {
                    total.sort(function(xi, xj){ return xi - xj;});
                    state.singlefeatures[selected_index].sidebysideMin = total[Math.floor(total.length * 0.01)];
                    state.singlefeatures[selected_index].sidebysideMax = total[Math.floor(total.length * 0.99)];
                    state.singlefeatures[selected_index].sidebysideTrueMax = total[total.length - 1];
                }
            }
            else {
                state.singlefeatures[selected_index].sameFunc = 0;
                state.singlefeatures[selected_index].sidebysideMin = 0;
                state.singlefeatures[selected_index].sidebysideMax = 0;
                state.singlefeatures[selected_index].sidebysideTrueMax = 0;
            }
            
        },
        set_linksVal(state, value) {
            state.linksVal = value;
            state.singlefeatures[state.selected_index].linksVal = value;
        },
        set_showFunction(state, value) {
            state.singlefeatures[state.selected_index].showFunction = value;
            state.showFunction = value;
        },
        set_eyeside(state, value) {
            state.eyeside = value;
            state.singlefeatures[state.selected_index].eyeside = value;
        },
        set_multiVal(state, value) {
            state.multiVal = value;
            state.singlefeatures[state.selected_index].multiVal = value;
        },
        set_testVal(state, value) {
            state.testVal = value;
            state.singlefeatures[state.selected_index].testVal = value;
            state.testValName = state.testValPros[value].label;
            state.singlefeatures[state.selected_index].testValName = state.testValName;
        },
        set_multiGroupVal(state, value) {
            state.multiGroupVal = value;
            state.singlefeatures[state.selected_index].multiGroupVal = value;
            state.multiGroupValName = state.multiGroupValPros[Number(value)].label;
            state.singlefeatures[state.selected_index].multiGroupValName = state.multiGroupValName;
        },
        set_modelVal(state, value) {
            state.modelVal = value;
            state.singlefeatures[state.selected_index].modelVal = value;
        },
        set_picBgOne(state, value) {
            state.picBgOne = value;
        },
        set_picBgTwo(state, value) {
            state.picBgTwo = value;
        },
        set_Bg_by_index(state, index) {
            state.featureIndex = index;
            var s = state.g1name + '_' + state.g2name + state.beginROI.toString() + '_' + state.endROI.toString() + '_' + state.featureIndex.toString();
            state.picBgOne = s;
            state.picBgTwo = s;   
        },
        set_Bg_by_roi(state, beginROI, endROI) {
            state.beginROI = beginROI;
            state.endROI = endROI;
            var s = state.g1name + '_' + state.g2name + state.beginROI.toString() + '_' + state.endROI.toString() + '_' + state.featureIndex.toString();
            state.picBgOne = s;
            state.picBgTwo = s;   
        },
        set_g1name(state, value){
            state.g1name = value;
            var s1 = state.g1name + '_' + state.beginROI.toString() + '_' + state.endROI.toString() + '_' + state.picindex.toString();
            var s2 = state.g2name + '_' + state.beginROI.toString() + '_' + state.endROI.toString() + '_' + state.picindex.toString();
            state.picBgOne = s1;
            state.picBgTwo = s2;
        },
        set_g2name(state, value) {
            state.g2name = value;
            var s1 = state.g1name + '_' + state.beginROI.toString() + '_' + state.endROI.toString() + '_' + state.picindex.toString();
            var s2 = state.g2name + '_' + state.beginROI.toString() + '_' + state.endROI.toString() + '_' + state.picindex.toString();
            state.picBgOne = s1;
            state.picBgTwo = s2;
        },
        set_beginROI(state, value) {
            state.beginROI = value;
            var s1 = state.g1name + '_' + state.beginROI.toString() + '_' + state.endROI.toString() + '_' + state.picindex.toString();
            var s2 = state.g2name + '_' + state.beginROI.toString() + '_' + state.endROI.toString() + '_' + state.picindex.toString();
            state.picBgOne = s1;
            state.picBgTwo = s2;
        },
        set_endROI(state, value) {
            state.endROI = value;
            var s1 = state.g1name + '_' + state.beginROI.toString() + '_' + state.endROI.toString() + '_' + state.picindex.toString();
            var s2 = state.g2name + '_' + state.beginROI.toString() + '_' + state.endROI.toString() + '_' + state.picindex.toString();
            state.picBgOne = s1;
            state.picBgTwo = s2;
        },
        set_picindex(state, value) {
            state.picindex = value;
            var s1 = state.g1name + '_' + state.beginROI.toString() + '_' + state.endROI.toString() + '_' + state.picindex.toString();
            var s2 = state.g2name + '_' + state.beginROI.toString() + '_' + state.endROI.toString() + '_' + state.picindex.toString();
            state.picBgOne = s1;
            state.picBgTwo = s2;
        },
        set_pic(state, [g1name,g2name,beginROI,endROI]) {
            state.g1name = g1name;
            state.g2name = g2name;
            if(beginROI < endROI) {
                state.beginROI = beginROI;
                state.endROI = endROI;
            }else {
                state.beginROI = endROI;
                state.endROI = beginROI;
            }
            var s1 = state.g1name + '_' + state.beginROI.toString() + '_' + state.endROI.toString() + '_' + state.picindex.toString();
            var s2 = state.g2name + '_' + state.beginROI.toString() + '_' + state.endROI.toString() + '_' + state.picindex.toString();
            state.picBgOne = s1;
            state.picBgTwo = s2;
        },
        set_selected_index(state, index) {
            /*
            if(state.selected_index != -1){ 
                state.singlefeatures[state.selected_index].linksVal = state.linksVal;
            state.singlefeatures[state.selected_index].testVal = state.testVal;
            state.singlefeatures[state.selected_index].multiGroupVal = state.multiGroupVal;
            state.singlefeatures[state.selected_index].testValName = state.testValName;
            state.singlefeatures[state.selected_index].multiGroupValName = state.multiGroupValName;
            state.singlefeatures[state.selected_index].modelVal = state.modelVal;
            state.singlefeatures[state.selected_index].multiVal = state.multiVal;
            state.singlefeatures[state.selected_index].ROIZValue = state.ROIZValue;
            state.singlefeatures[state.selected_index].DBScanRadius = state.DBScanRadius;
            state.singlefeatures[state.selected_index].showFunction = state.showFunction;
            state.singlefeatures[state.selected_index].eyeside = state.eyeside;
            }
            */

            if(state.selected_index == index) {
                state.selected_index = -1;
            }
            else {
                 state.selected_index = index;
                 state.feature_index = state.singlefeatures[state.selected_index].index;

            if(state.singlefeatures.length > 0 && state.singlefeatures[state.selected_index].hasOwnProperty('linksVal')){
                console.log('load');
                state.linksVal = state.singlefeatures[state.selected_index].linksVal;
                state.testVal = state.singlefeatures[state.selected_index].testVal;
                state.multiGroupVal = state.singlefeatures[state.selected_index].multiGroupVal;
                state.testValName = state.singlefeatures[state.selected_index].testValName;
                state.multiGroupValName = state.singlefeatures[state.selected_index].multiGroupValName;
                state.modelVal = state.singlefeatures[state.selected_index].modelVal;
                state.multiVal = state.singlefeatures[state.selected_index].multiVal;
                state.ROIZValue = state.singlefeatures[state.selected_index].ROIZValue;
                state.DBScanRadius = state.singlefeatures[state.selected_index].DBScanRadius;
                state.showFunction = state.singlefeatures[state.selected_index].showFunction;
                state.eyeside = state.singlefeatures[state.selected_index].eyeside;
            }
            }
            
        },
        set_selected_index_newest(state) {
            if(state.selected_index != -1) {
            state.singlefeatures[state.selected_index].linksVal = state.linksVal;
            state.singlefeatures[state.selected_index].testVal = state.testVal;
            state.singlefeatures[state.selected_index].multiGroupVal = state.multiGroupVal;
            state.singlefeatures[state.selected_index].testValName = state.testValName;
            state.singlefeatures[state.selected_index].multiGroupValName = state.multiGroupValName;
            state.singlefeatures[state.selected_index].modelVal = state.modelVal;
            state.singlefeatures[state.selected_index].multiVal = state.multiVal;
            state.singlefeatures[state.selected_index].ROIZValue = state.ROIZValue;
            state.singlefeatures[state.selected_index].DBScanRadius = state.DBScanRadius;
            state.singlefeatures[state.selected_index].showFunction = state.showFunction;
            state.singlefeatures[state.selected_index].eyeside = state.eyeside;
            }
            state.selected_index = state.singlefeatures.length - 1;
            state.feature_index = state.singlefeatures[state.selected_index].index;

            if(state.singlefeatures.length > 0 && state.singlefeatures[state.selected_index].hasOwnProperty('linksVal')){
                console.log('load');
                state.linksVal = state.singlefeatures[state.selected_index].linksVal;
                state.testVal = state.singlefeatures[state.selected_index].testVal;
                state.multiGroupVal = state.singlefeatures[state.selected_index].multiGroupVal;
                state.testValName = state.singlefeatures[state.selected_index].testValName;
                state.multiGroupValName = state.singlefeatures[state.selected_index].multiGroupValName;
                state.modelVal = state.singlefeatures[state.selected_index].modelVal;
                state.multiVal = state.singlefeatures[state.selected_index].multiVal;
                state.ROIZValue = state.singlefeatures[state.selected_index].ROIZValue;
                state.DBScanRadius = state.singlefeatures[state.selected_index].DBScanRadius;
                state.showFunction = state.singlefeatures[state.selected_index].showFunction;
                state.eyeside = state.singlefeatures[state.selected_index].eyeside;
            }
        },
        save_single_value(state) {
            state.singlefeatures[state.selected_index].linksVal = state.linksVal;
            state.singlefeatures[state.selected_index].testVal = state.testVal;
            state.singlefeatures[state.selected_index].testValName = state.testValName;
        },
        save_multi_value(state) {
            state.singlefeatures[state.selected_index].multiGroupVal = state.multiGroupVal;
            state.singlefeatures[state.selected_index].multiGroupValName = state.multiGroupValName;
            state.singlefeatures[state.selected_index].modelVal = state.modelVal;
            state.singlefeatures[state.selected_index].multiVal = state.multiVal;
            state.singlefeatures[state.selected_index].ROIZValue = state.ROIZValue;
            state.singlefeatures[state.selected_index].DBScanRadius = state.DBScanRadius;
        },
        load_single_value(state) {
            state.linksVal = state.singlefeatures[state.selected_index].linksVal;
            state.testVal = state.singlefeatures[state.selected_index].testVal;
            state.testValName = state.singlefeatures[state.selected_index].testValName;
        },
        load_multi_value(state) {
            state.multiGroupVal = state.singlefeatures[state.selected_index].multiGroupVal;
            state.multiGroupValName = state.singlefeatures[state.selected_index].multiGroupValName;
            state.modelVal = state.singlefeatures[state.selected_index].modelVal;
            state.multiVal = state.singlefeatures[state.selected_index].multiVal;
            state.ROIZValue = state.singlefeatures[state.selected_index].ROIZValue;
            state.DBScanRadius = state.singlefeatures[state.selected_index].DBScanRadius;
        },
        setneedReDraw(state, value) {
            state.needReDraw = value;
        }
    }
});

export default store;
