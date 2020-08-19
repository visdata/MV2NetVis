<template>
	<div style="border-top: 1px solid rgba(0, 0, 0, 0.1);">
        <div style="border-right: 1px solid rgba(0, 0, 0, 0.1); padding: 0px;width: 74%; float: left;">
            <div class="card border-light mb-3" >
                <div class="card-body" style="padding-right: 5px; height: 700px;">
                    <div :style="{'margin-top': '-10px'}">
                        <p :style="{'font-style': 'italic'}">Comparison View</p>
                    </div>
                    <div>
                        <div style="float: left;width: 100%;background-color: rgb(249,249,249);margin-top: -10px; border-bottom: 1px solid rgba(0,0,0,0.05); margin-bottom: 1px;">
                            <div style="text-align: left; width: 40%; float: left;margin-right: 10px;border-right: 1px solid rgba(0,0,0,0.1)">
                                <div v-for="pro in linksPros" :key="pro.index" style="margin-bottom:0px; float: left; ">
                                    <RadioButton  name="group1" :inputId="pro.message" :value="pro.index" v-model="linksVal"></RadioButton>
                                    <Label for="pro" style="width: 90px; padding-right: 0px;">{{pro.message}}</Label>
                                </div>
                            </div>
                            <div style="text-align: left; width: 40%; float: left; margin-right: 10px; border-right: 1px solid rgba(0,0,0,0.1)">
                                <div v-for="pro in testValPros" :key="pro.index" style="margin-bottom:0px; float: left;">
                                    <RadioButton  name="group2" :inputId="pro.message" :value="pro.index" v-model="testVal"></RadioButton>
                                    <Label for="pro" style="width: 90px; padding-right: 0px;">{{pro.message}}</Label>
                                </div>
                            </div>
                            <div>
                                <label style="float: left; margin-top: 5px; margin-right: 8px; margin-bottom: 0px;">lambda:</label>
                                <div :style="{'float':'left', 'margin-top':'5px'}">
                                    <slider :min=-0.000000001 :max=100 v-model = "lambda" :style="{'width': '100px'}"></slider>
                                </div>
                            </div>
                        </div>
                        <div>
                            <div v-for="feature_value in feature_values" >
                                <div style="width: 20%;margin: auto; float: left;text-align: center;padding-bottom: 10px;">
                                    <div style="border-right: 1px solid rgba(0,0,0,0.05); border-bottom: 1px solid rgba(0,0,0,0.05);">
                                        <div style="float: left; z-index: 999; width: 95px; text-align: left; padding-left: 5px;">
                                            <CheckBox :value="feature_value" name="group3" v-model="selectValue" :multiple="true"> </CheckBox>
                                            <Label style="width: 55px; padding-right: 0px;">{{features[feature_value].name}}</Label>
                                        </div>
                                        <superposed-graph v-bind="{'strength':features[feature_value].data}" :index='feature_value' :heightPer='0.2'></superposed-graph>
                                    </div>
                                    <div style="border-right: 1px solid rgba(0,0,0,0.05); padding-bottom: 20px;">
                                        <div>
                                            <CheckBox :value="feature_value+5" name="group4" v-model="selectValue" :multiple="true"></CheckBox>
                                            <Label style="width: 150px">multi-{{features[feature_value].name}}</Label>
                                        </div>
                                        <superposed-graph-for-multi v-bind="{'strength':features[feature_value].data}" :index='feature_value' :heightPer='0.2'></superposed-graph-for-multi>
                                    </div>
                                </div>
                            </div>
                        </div>
                        <!--
                        <div style="height: 500px; width: 700px; float: left;">
                            <happy-scroll color="rgba(0,0,0,0.5)" size="5" >
                                <div :style="{'height': '5000px'}">
                                    <div>
                                        <div v-for="feature_value in feature_values" >
                                            <div style="width: 100%;margin: auto; float: left;text-align: center;padding-bottom: 10px;">
                                                <div>
                                                    <CheckBox :value="feature_value" name="group3" v-model="selectValue" :multiple="true"></CheckBox>
                                                    <Label style="width: 200px">{{features[feature_value].name}}</Label>
                                                </div>
                                                <superposed-graph v-bind="{'strength':features[feature_value].data}" :index='feature_value' :heightPer='0.2'></superposed-graph>
                                            </div>
                                            <div style="width: 300px;margin: auto; float: left;text-align: center;padding-bottom: 10px;height: 516px;">
                                                <div>
                                                    <Label style="width: 200px">muti-{{features[feature_value].name}}</Label>
                                                </div>
                                                <superposed-graph-for-multi v-bind="{'strength':features[feature_value].data}" :index='feature_value' :heightPer='0.2'></superposed-graph-for-multi>
                                            </div>
                                        </div>
                                    </div>
                                </div>
                            </happy-scroll>
                        </div>
                        -->
                    </div>
                </div>
            </div>
    </div>
        <div class="card border-light mb-3" style="border-right: 1px solid rgba(0, 0, 0, 0.1); padding: 0px;width: 26%;float: left;">
            <div class="card-body" style="padding-left: 5px; padding-right: 5px; height: 700px;">
                <div :style="{'margin-top': '-10px'}">
                    <p :style="{'font-style': 'italic'}">Superposed View</p>
                </div>
                <div style="text-align: left; background-color: rgb(249,249,249); height: 30px; margin-top: -10px; border-bottom: 1px solid rgba(0,0,0,0.05); margin-bottom: 1px;">
                    <div style="width: 162px; height: 30px; border-right: 1px solid rgba(0,0,0,0.1);float: left;margin-right: 10px;">
                        <div v-for="pro in fuc" :key="pro.id" style="margin-bottom:0px; float: left; width: 40px;">
                            <RadioButton name="group2" :inputId="pro.id" :value="pro.id" v-model="fucValue"></RadioButton>
                            <Label for="pro" style="width: 20px">{{pro.name}}</Label>
                        </div>
                    </div>
                    <div style="width: 150px; float: left;">
                        <label style="float: left; margin-top: 5px; margin-right: 8px; margin-bottom: 0px;">filter:</label>
                        <div :style="{'float':'left', 'margin-top':'5px'}">
                            <slider :min=-0.000000001 :max=100 v-model = "filterRate[1]" :style="{'width': '100px'}"></slider>
                        </div>
                    </div>
                </div>
                <div style="float: left; width: 100%; margin-top: 40px; margin-left: 8px;">
                    <superposed-graph-for-cal v-bind="{'strength':totalData, 'filterRate':filterRate}" :heightPer='0.4' :index="String(10)"></superposed-graph-for-cal>
                </div>
            </div>
        </div>    
    </div>
</template>

<script>
    //echarts
    import superposedGraph from "./superposedGraph"
    import superposedGraphForMulti from "./superposedGraphForMulti"
    import superposedGraphForCal from "./superposedGraphForCal"
    import strength from "../../data/strength"
    import { mapState } from 'vuex'
    import NetService from '@/services/net-service'
    import slider from './slider'

    export default {
        name: "comparisonViewVersionTwo",
        components: {
            superposedGraph,
            superposedGraphForMulti,
            superposedGraphForCal,
            slider
        },
        data () {
            return {
                strength,
                lambda: 50,
                totalData: {},
                linksVal: "0",
                linksPros: [
                    {
                        "message": "average",//"平均特征值",
                        "index": "0",
                    },
                    {
                        "message": "variance",//"特征值总方差",
                        "index": "1",
                    },
                    {
                        "message": "people num",
                        "index": "2",
                    }
                ],
                testVal: "0",
                testValPros: [
                    {
                        "message": "t-test",
                        "index": "0",
                    },
                    {
                        "message": "welch",
                        "index": "1",
                    },
                    {
                        "message": "mann-whiteney",
                        "index": "2",
                    }
                ],
                height: {
                    height: '',
                },
                values: [],
                fuc: [
                {
                    id: "0",
                    name: '+',
                },
                {
                    id: "1",
                    name: '-'
                },
                {
                    id: "2",
                    name: '*'
                },
                {
                    id: "3",
                    name: "/"
                }
                ],
                fucValue: "0",
                newName: "",
                selectValue: [],
                filterRate: [0, 100],
            }
        },
        created(){
            //this.$store.commit('set_features_data', [this.strength, this.strength,this.strength,this.strength,this.strength]);
            this.setTotalDataZero();
        },
        computed: {
            ...mapState(['pValueThreshold', 'validThreshold', 'features', 'feature_values']),
        },
        //mounted在html渲染出来后进行操作
        mounted() {
            
        },
        methods: {
            //归一化相加减
            addData(data1, data2) {
                var max1 = Math.max(...data1);
                var min1 = Math.min(...data1);
                var max2 = Math.max(...data2);
                var min2 = Math.min(...data2)
                var result = [];
                for(var i in data1) {
                    var tmp1 = (data1[i] - min1) / (max1 - min1);
                    var tmp2 = (data2[i] - min2) / (max2 - min2);
                    var tmp = tmp1 + tmp2;
                    result.push(tmp);
                }
                return result;
            },
            subData(data1, data2) {
                var max1 = Math.max(...data1);
                var min1 = Math.min(...data1);
                var max2 = Math.max(...data2);
                var min2 = Math.min(...data2)
                var result = [];
                for(var i in data1) {
                    var tmp1 = (data1[i] - min1) / (max1 - min1);
                    var tmp2 = (data2[i] - min2) / (max2 - min2);
                    var tmp = tmp1 - tmp2;
                    result.push(tmp);
                }
                return result;
            },
            mulData(data1, data2) {
                var max1 = Math.max(...data1);
                var min1 = Math.min(...data1);
                var max2 = Math.max(...data2);
                var min2 = Math.min(...data2)
                var result = [];
                for(var i in data1) {
                    var tmp1 = (data1[i] - min1) / (max1 - min1);
                    var tmp2 = (data2[i] - min2) / (max2 - min2);
                    var tmp = tmp1 * tmp2;
                    result.push(tmp);
                }
                return result;
            },
            devData(data1, data2) {
                var max1 = Math.max(...data1);
                var min1 = Math.min(...data1);
                var max2 = Math.max(...data2);
                var min2 = Math.min(...data2)
                var result = [];
                for(var i in data1) {
                    var tmp1 = (data1[i] - min1) / (max1 - min1);
                    var tmp2 = (data2[i] - min2) / (max2 - min2);
                    if (tmp2 == 0) {
                        result.push(0);
                    }
                    else{
                        var tmp = tmp1 / tmp2;
                        result.push(tmp);
                    }
                }
                return result;
            },
            normalize(data) {
                var result = [];
                var max = Math.max(...data);
                var min = Math.min(...data);
                for (var i in data) {
                    result.push((data[i] - min) / (max - min));
                }
                return result;
            },
            calData(data1, data2, index) {
                if (index == 0) {
                    return this.addData(data1, data2);
                } else if (index == 1) {
                    return this.subData(data1, data2);
                } else if (index == 2) {
                    return this.mulData(data1, data2);
                } else if (index == 3) {
                    return this.devData(data1, data2);
                }
            },
            setTotalDataZero() {
                var zero = [];
                for(var i = 0; i < 2485; i++) {
                    zero.push(0);
                }
                this.totalData.groupOneData = zero;
                this.totalData.groupTwoData = zero;
                this.totalData.pvalue = zero;
                this.totalData.data = zero;
            },
            deepCopy(parm){
                let res;
                if(Object.prototype.toString.call(parm) == "[object Array]"){   //数组
                    res = [];
                    for(var i = 0;i < parm.length;i++){
                        res[i] = this.deepCopy(parm[i]);
                    }
                }else if(Object.prototype.toString.call(parm) == "[object Object]"){  //对象
                    res = {};
                    for(var i in parm){
                        res[i] = this.deepCopy(parm[i]);
                    }
                }else{
                    return parm;
                }
                return res;
            },
        },
        watch: {
            lambda: {
                deep: true,
                handler: function(newVal, oldVal) {
                    console.log("lambda change: " + newVal);
                    this.$store.commit('set_lambda', newVal);
                    NetService.update2D(this.pValueThreshold, this.validThreshold, this.testVal,this.lambda, (res, err) => {
                        if(err){
                            console.error(err);
                            return;
                        }
                        if(!res.data){
                            console.log('data not exists!');
                            return;
                        }
                        console.log('update2D_test', res.data);
                        this.$store.commit('set_features_data', [res.data.curvature, res.data.entropy, res.data.torsion,res.data.strength, res.data.length]);
                        this.$store.commit('set_features_data_data', this.linksVal);
                        if(this.selectValue.length == 1) {
                            //叠加视图显示第一个视图
                            this.totalData = this.deepCopy(this.features[this.selectValue[0] % 5].data);
                        }
                        else if(this.selectValue.length == 2) {
                            this.totalData.groupOneData = this.normalize(this.features[this.selectValue[0] % 5].data.data);
                            this.totalData.groupTwoData = this.normalize(this.features[this.selectValue[1] % 5].data.data);
                            //根据符号计算
                            this.totalData.data = this.calData(this.features[this.selectValue[0] % 5].data.data, this.features[this.selectValue[1] % 5].data.data, this.fucValue);
                            this.totalData.pvalue = this.calData(this.features[this.selectValue[0] % 5].data.pvalue, this.features[this.selectValue[1] % 5].data.pvalue, this.fucValue);
                        }
                    });
                }
            },
            linksVal: {
                deep: true,
                handler: function(newVal, oldVal) {
                    this.$store.commit('set_features_data_data', newVal);
                    this.$store.commit('set_linksVal', newVal);
                    if(this.selectValue.length == 1) {
                        //叠加视图显示第一个视图
                        this.totalData = this.deepCopy(this.features[this.selectValue[0] % 5].data);
                    }
                    else if(this.selectValue.length == 2) {
                        /*
                        if(this.linksVal == 0) {
                            this.totalData.groupOneData = this.features[newVal[0]].data.average;
                            this.totalData.groupTwoData = this.features[newVal[1]].data.average;
                        }
                        else if (this.linksVal == 1) {
                            this.totalData.groupOneData = this.features[newVal[0]].data.variance;
                            this.totalData.groupTwoData = this.features[newVal[1]].data.variance;
                        }
                        else if (this.linksVal == 1) {
                            this.totalData.groupOneData = this.features[newVal[0]].data.totalPeople;
                            this.totalData.groupTwoData = this.features[newVal[1]].data.totalPeople;
                        }
                        */
                        this.totalData.groupOneData = this.normalize(this.features[this.selectValue[0] % 5].data.data);
                        this.totalData.groupTwoData = this.normalize(this.features[this.selectValue[1] % 5].data.data);
                        //根据符号计算
                        this.totalData.data = this.calData(this.features[this.selectValue[0] % 5].data.data, this.features[this.selectValue[1] % 5].data.data, this.fucValue);
                        this.totalData.pvalue = this.calData(this.features[this.selectValue[0] % 5].data.pvalue, this.features[this.selectValue[1] % 5].data.pvalue, this.fucValue);
                        /*
                        if(this.fucValue == 0) { //加法
                            this.totalData.data = this.addData(this.features[this.selectValue[0]].data.data, this.features[this.selectValue[1]].data.data);
                            this.totalData.pvalue = this.addData(this.features[this.selectValue[0]].data.pvalue, this.features[this.selectValue[1]].data.pvalue);
                        }
                        else if(this.fucValue == 1) {
                            this.totalData.data = this.subData(this.features[this.selectValue[0]].data.data, this.features[this.selectValue[1]].data.data);
                            this.totalData.pvalue = this.subData(this.features[this.selectValue[0]].data.pvalue, this.features[this.selectValue[1]].data.pvalue);
                        }*/
                    }
                }
            },
            testVal: {
                deep: true,
                handler: function(newVal, oldVal) {
                    this.$store.commit('set_testVal', newVal);
                    NetService.update2D(this.pValueThreshold, this.validThreshold, newVal, this.lambda, (res, err) => {
                            if(err){
                                console.error(err);
                                return;
                            }
                            if(!res.data){
                                console.log('data not exists!');
                                return;
                            }
                            console.log('update2D');
                            //{"code":0,"curvature":{"groupOneData":[508.4],"groupTwoData":[508.4],"pvalue":[508.4],"average":[508.4],"variance":[508.4],"totalPeople":[524.7857143]},"entropy":{"groupOneData":[508.4],"groupTwoData":[508.4],"pvalue":[508.4],"average":[508.4],"variance":[508.4],"totalPeople":[524.7857143]},"torsion":{"groupOneData":[508.4],"groupTwoData":[508.4],"pvalue":[508.4],"average":[508.4],"variance":[508.4],"totalPeople":[524.7857143]},"strength":{"groupOneData":[508.4],"groupTwoData":[508.4],"pvalue":[508.4],"average":[508.4],"variance":[508.4],"totalPeople":[524.7857143]},"length":{"groupOneData":[508.4],"groupTwoData":[508.4],"pvalue":[508.4],"average":[508.4],"variance":[508.4],"totalPeople":[524.7857143]},"message":"获取成功"}
                            this.$store.commit('set_features_data', [res.data.curvature, res.data.entropy, res.data.torsion,res.data.strength, res.data.length]);
                            this.$store.commit('set_features_data_data', this.linksVal);
                            if(this.selectValue.length == 1) {
                                //叠加视图显示第一个视图
                                this.totalData = this.deepCopy(this.features[this.selectValue[0] % 5].data);
                            }
                            else if(this.selectValue.length == 2) {
                                this.totalData.groupOneData = this.normalize(this.features[this.selectValue[0] % 5].data.data);
                                this.totalData.groupTwoData = this.normalize(this.features[this.selectValue[1] % 5].data.data);
                                //根据符号计算
                                this.totalData.data = this.calData(this.features[this.selectValue[0] % 5].data.data, this.features[this.selectValue[1] % 5].data.data, this.fucValue);
                                this.totalData.pvalue = this.calData(this.features[this.selectValue[0] % 5].data.pvalue, this.features[this.selectValue[1] % 5].data.pvalue, this.fucValue);
                            }
                        });
                }
            },
            features:{
                deep: true,
                handler: function(newVal, oldVal) {
                    console.log('comparsionViewFeatures', newVal);
                    if(this.selectValue.length == 1) {
                        //叠加视图显示第一个视图
                        this.totalData = this.deepCopy(this.features[this.selectValue[0] % 5].data);
                    }
                    else if(this.selectValue.length == 2) {
                        this.totalData.groupOneData = this.normalize(this.features[this.selectValue[0] % 5].data.data);
                        this.totalData.groupTwoData = this.normalize(this.features[this.selectValue[1] % 5].data.data);
                        //根据符号计算
                        this.totalData.data = this.calData(this.features[this.selectValue[0] % 5].data.data, this.features[this.selectValue[1] % 5].data.data, this.fucValue);
                        this.totalData.pvalue = this.calData(this.features[this.selectValue[0] % 5].data.pvalue, this.features[this.selectValue[1] % 5].data.pvalue, this.fucValue);
                        /*
                        if(this.fucValue == 0) { //加法
                            this.totalData.data = this.addData(this.features[this.selectValue[0]].data.data, this.features[this.selectValue[1]].data.data);
                            this.totalData.pvalue = this.addData(this.features[this.selectValue[0]].data.pvalue, this.features[this.selectValue[1]].data.pvalue);
                        }
                        else if(this.fucValue == 1) {
                            this.totalData.data = this.subData(this.features[this.selectValue[0]].data.data, this.features[this.selectValue[1]].data.data);
                            this.totalData.pvalue = this.subData(this.features[this.selectValue[0]].data.pvalue, this.features[this.selectValue[1]].data.pvalue);
                        }*/
                    }
                }
            },
            fucValue: {
                deep: true,
                handler: function(newVal, oldVal) {
                    if(this.selectValue.length == 2) {
                        /*
                        if(this.linksVal == 0) {
                            this.totalData.groupOneData = this.features[newVal[0]].data.average;
                            this.totalData.groupTwoData = this.features[newVal[1]].data.average;
                        }
                        else if (this.linksVal == 1) {
                            this.totalData.groupOneData = this.features[newVal[0]].data.variance;
                            this.totalData.groupTwoData = this.features[newVal[1]].data.variance;
                        }
                        else if (this.linksVal == 1) {
                            this.totalData.groupOneData = this.features[newVal[0]].data.totalPeople;
                            this.totalData.groupTwoData = this.features[newVal[1]].data.totalPeople;
                        }
                        */
                        console.log('fucValuechange,', this.features);
                        this.totalData.groupOneData = this.normalize(this.features[this.selectValue[0] % 5].data.data);
                        this.totalData.groupTwoData = this.normalize(this.features[this.selectValue[1] % 5].data.data);
                        //根据符号计算
                        this.totalData.data = this.calData(this.features[this.selectValue[0] % 5].data.data, this.features[this.selectValue[1] % 5].data.data, newVal);
                        this.totalData.pvalue = this.calData(this.features[this.selectValue[0] % 5].data.pvalue, this.features[this.selectValue[1] % 5].data.pvalue, newVal);
                        /*
                        if(newVal == 0) { //加法
                            this.totalData.data = this.addData(this.features[this.selectValue[0]].data.data, this.features[this.selectValue[1]].data.data);
                            this.totalData.pvalue = this.addData(this.features[this.selectValue[0]].data.pvalue, this.features[this.selectValue[1]].data.pvalue);
                        }
                        else if(newVal == 1) {
                            this.totalData.data = this.subData(this.features[this.selectValue[0]].data.data, this.features[this.selectValue[1]].data.data);
                            this.totalData.pvalue = this.subData(this.features[this.selectValue[0]].data.pvalue, this.features[this.selectValue[1]].data.pvalue);
                        }*/
                    }
                }
            },
            feature_values: {
                deep: true,
                handler: function(newVal, oldVal) {
                    //删除不在feature_Values的linksVal元素
                    //console.log('features_values添加删除', newVal);
                    var tmp = [];
                    for(var i in this.selectValue) {
                        if(newVal.indexOf(this.selectValue[i]) != -1) {
                            console.log('添加');
                            tmp.push(this.selectValue[i]);
                        }
                    }
                    this.selectValue = tmp;
                }
            },
            selectValue: {
                deep: true,
                handler: function(newVal, oldVal) {
                    if(newVal.length > 2) {
                        this.selectValue = [];
                    }
                    else if(newVal.length == 0) {
                        this.setTotalDataZero();        
                    }
                    else if(newVal.length == 1) {
                        //叠加视图显示第一个视图
                        this.totalData = this.deepCopy(this.features[newVal[0] % 5].data);
                    }
                    else if(newVal.length == 2) {
                        /*
                        if(this.linksVal == 0) {
                            this.totalData.groupOneData = this.features[newVal[0]].data.average;
                            this.totalData.groupTwoData = this.features[newVal[1]].data.average;
                        }
                        else if (this.linksVal == 1) {
                            this.totalData.groupOneData = this.features[newVal[0]].data.variance;
                            this.totalData.groupTwoData = this.features[newVal[1]].data.variance;
                        }
                        else if (this.linksVal == 1) {
                            this.totalData.groupOneData = this.features[newVal[0]].data.totalPeople;
                            this.totalData.groupTwoData = this.features[newVal[1]].data.totalPeople;
                        }
                        */
                        this.totalData.groupOneData = this.normalize(this.features[newVal[0] % 5].data.data);
                        this.totalData.groupTwoData = this.normalize(this.features[newVal[1] % 5].data.data);
                        //根据符号计算
                        this.totalData.data = this.calData(this.features[newVal[0] % 5].data.data, this.features[newVal[1] % 5].data.data, this.fucValue);
                        this.totalData.pvalue = this.calData(this.features[newVal[0] % 5].data.pvalue, this.features[newVal[1] % 5].data.pvalue, this.fucValue);
                        /*
                        if(this.fucValue == 0) { //加法
                            this.totalData.data = this.addData(this.features[newVal[0]].data.data, this.features[newVal[1]].data.data);
                            this.totalData.pvalue = this.addData(this.features[newVal[0]].data.pvalue, this.features[newVal[1]].data.pvalue);
                        }
                        else if(this.fucValue == 1) {
                            this.totalData.data = this.subData(this.features[newVal[0]].data.data, this.features[newVal[1]].data.data);
                            this.totalData.pvalue = this.subData(this.features[newVal[0]].data.pvalue, this.features[newVal[1]].data.pvalue);
                        }*/
                    }
                }
            }

        }
    }
</script>
