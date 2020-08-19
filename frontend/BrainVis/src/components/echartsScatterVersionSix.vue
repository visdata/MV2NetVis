<template>
    <div>
        <div class="card border-light mb-3">
            <div class="card-header">Subject View</div>
            <div class="card-body">
                <!--选择性别还是是否患病-->
                <div :style="{'margin-top': '-10px', 'float':'left', 'z-index': '99', 'width':'25%'}">
                    <ul class="ui-controlgroup ui-listview" :style="{'list-style-type':'none', 'margin':'1px', 'padding':'0px', 'text-align':'left'}">
                        <li v-for="(item, index) in pro_selections_items" :key="item.index">
                            <input type="checkbox" @click="multipleSelect(item, item.checkval)" :checked="item.checkval" :style="{'opacity': '0', 'width':'17px','height':'19px', 'position':'absolute','left':'20px'}" />
                            <span :style="{'display':'inline-block', 'width':'13px','height':'13px', 'display':'inline-block', 'outline':'1px solid black', 'text-align':'center', 'font-size':'12px', 'font-weight':'bolder', 'font':'small-caption','vertical-align':'top'}">{{ item.order }}</span>
                            <span> {{ item.message }} </span>
                        </li>
                    </ul>
                    <button class="btn btn-secondary" @click="reload_properties()" :style="{'width': '70px'}">
                        reload
                    </button>
                </div>
                <!--第一列上sunburst-->
                <sunburst-two :data="sunburstData" :show='true' :style="{'margin-top': '-10px','height': '220px', 'width':'75%','float':'left'}" >
                    <!-- Add behaviors -->
                    <template slot-scope='{ on, actions }'>
                        <highlightOnHover v-bind='{ on, actions }'/>
                        <zoomOnClick v-bind='{ on, actions }'/>
                        <markedOnClick v-bind='{ on, actions }'/>
                    </template>
                    <!-- 信息 -->
                    <nodeInfoDisplayer slot='top' slot-scope='{ nodes }' :current='nodes.mouseOver' :root='nodes.root' description='of visits begin' />
                    <!-- 层次 -->
                    <breadcrumbTrail slot='legend' slot-scope='{ nodes, colorGetter, width }' :current='nodes.mouseOver' :root='nodes.root' :colorGetter='colorGetter' :from='nodes.clicked' :width='width' />
                    <breadcrumbTrailTwo slot='selectionone' slot-scope='{ nodes, colorGetter, width }' :colorGetter='colorGetter' :width='width' :index='0'/>
                    <breadcrumbTrailTwo slot='selectiontwo' slot-scope='{ nodes, colorGetter, width }' :colorGetter='colorGetter' :width='width' :index='1'/>
                </sunburst-two>
            </div>
        </div>
    
        <div class="card border-light mb-3">
            <div class="card-header">Feature View</div>
            <div class="card-body">
                <div style="margin-bottom:0px;position: absolute;padding-left: 79%">
                    <SwitchButton v-model="sumModel" onText="Sum" offText="Sub" style="width:62px"></SwitchButton>
                </div>
                <div :style="{'margin-top': '-8px', 'width':'39px', 'float':'left'}">
                    <div>
                        <div :style="{'padding-bottom':'10px', 'float':'left', 'width':'20px', 'padding-left':'10px'}">
                            
                        </div>
                        <!--slider :min=0 :max=100 v-model="pValueThreshold" :style="{'width': '200px'}"></slider-->
                        <vertical-slider class="custom-range" :min=-0.000000001 :max=100 :message='pvalueName' v-model = "pValueThreshold" :style="{'height': '130px','float':'left','padding-right':'5px'}"></vertical-slider>
                        <!--Slider v-model="pValueThreshold" :showTip="true" :rule="[0, '|', 25, '|',50, '|', 75, '|', 100]" style="width: 150px" :step='20'></Slider-->
                    </div>
                    <div>
                        <div :style="{'padding-bottom':'10px', 'float':'left', 'width':'20px', 'padding-left':'10px'}">
                            
                        </div>
                        <vertical-slider :min=-0.000000001 :max=100 :message='validName' v-model = "validThreshold" :style="{'height': '130px','float':'left','padding-right':'5px'}"></vertical-slider>
                        <!--slider :min=0 :max=100 v-model="validThreshold" :style="{'width': '200px'}"></slider-->
                        <!--Slider v-model='validThreshold' :showTip="true" :rule="[0, '|', 25, '|',50, '|', 75, '|', 100]" style="width: 150px" :step='20'></Slider-->
                    </div>
                </div>
    <!--第一列下热力图-->
    <div :style="{'height': '320px', 'width': '300px','float':'left'}">
        <happy-scroll color="rgba(0,0,0,0.5)" size="5">
            <div class="featureView" :style="{'text-align': 'left'}" >
                <div v-for="feature in features" :key="feature.index" style="margin-bottom:0px">
                    <CheckBox :value="feature.index" :multiple="true" v-model="values"></CheckBox>
                    <Label>{{feature.name}}</Label>
                    <chart :options="feature.options" :auto-resize='false' :style="{width: '300px', height: '120px', margin: 'auto'}"></chart>
                </div>
            </div>  
        </happy-scroll>
    </div>
            </div>
        </div>
    
    </div>
</template>

<script>
    //echarts
    import echarts from 'echarts'
    import 'echarts/lib/chart/scatter'
    import 'echarts/lib/component/tooltip'
    import 'echarts/lib/component/legend'
    import 'echarts/lib/component/title'
    import 'echarts/lib/component/visualMap'
    import dataGender from "../../data/dataGender"
    import dataSicken from "../../data/dataSicken"
    import {
        //breadcrumbTrail,
        highlightOnHover,
        nodeInfoDisplayer,
        sunburst,
        zoomOnClick
    } from 'vue-d3-sunburst'
    import "vue-d3-sunburst/dist/vue-d3-sunburst.css"
    import {UiCheckbox, UiRadioGroup, UiButton} from 'keen-ui'
    import NetService from '@/services/net-service'
    import markedOnClick from './markedOnClick.js'
    import { mapState } from 'vuex'
    import slider from './slider'
    import verticalSlider from './verticalSlider'
    import breadcrumbTrailTwo from './breadcrumbTrailTwo'
    import breadcrumbTrail from './breadcrumbTrail'
    import sunburstTwo from './sunburstTwo'

    export default {
        name: "echartsScatterVersionSix",
        components: {
            breadcrumbTrail,
            highlightOnHover,
            nodeInfoDisplayer,
            sunburst,
            zoomOnClick,
            UiRadioGroup,
            UiCheckbox,
            markedOnClick,
            UiButton,
            slider,
            verticalSlider,
            breadcrumbTrailTwo,
            sunburstTwo
        },
        data () {
            return {
                sunburstData: {
                    "name": "all",
                    "size": 87,
                    "index": "0",
                    "depth": 0,
                    "peer": 1,
                },
                pro_selections_items: [],
                
                handleArr: [], //选中数组
                handleNum: 0, //选中的顺序
                
                pValueThreshold: 50, //0~100
                validThreshold: 50, //0~100满足p-threshold的占比
                maxSubjectLength: 10, //长的那组人的长度
                
                pValueDataUp: [],
                pValueDataBottom: [],

                pvalueName: "pvalue",
                validName: "valid",

                values: [],
                /*
                features: [
                {
                    index: 0,
                    name: 'curvature',
                    options: {}
                },
                {
                    index:1,
                    name: 'entropy',
                    options: {}
                },
                {
                    index:2,
                    name: 'torsion',
                    options: {}
                },
                {
                    index:3,
                    name: 'strength',
                    options: {}
                },
                {
                    index:4,
                    name: 'length',
                    options: {}
                }],*/
                sumModel: true,
            }
        },
        created() {
            //this.getPointData()
            NetService.getAllProperties((res, err) => {
                if(err){
                    console.error(err);
                    return;
                }
                if(!res.data){
                    console.log('data not exists!');
                    return;
                }
                console.log('getAllProperties');
                //{"code":0,"data":[{"message":"Sicken","checkval":false,"order":""},{"message":"Gender","checkval":false,"order":""},{"message":"Ages","checkval":false,"order":""},{"message":"Others","checkval":false,"order":""}],"message":"\u83b7\u53d6\u6210\u529f"}
                this.pro_selections_items = res.data.data;
            });
        },
        //mounted在html渲染出来后进行操作
        mounted() {
            
        },
        computed: {
            ...mapState(['linksVal', 'testVal', 'features', 'subjects_valid', 'selected_subjects', 'selected_subjects_name', 'lambda']),
        },
        methods: {
            sliderFun(){
                console.log('sliderChange');
            },
            addIndex(obj, j) {
                obj.depth = j;
                if(obj.hasOwnProperty('children')) {
                    var len = obj.children.length;
                    for(var i in obj.children) {
                        obj.children[i].index = i;
                        obj.children[i].peer = len;
                        this.addIndex(obj.children[i], j+1);
                    }
                }
            },
            multipleSelect(val, status){
                val.checkval = !status; //反选
                if (val.checkval){
                    this.handleArr.push(val); //选中则添加进数组
                    val.order = (this.handleNum <= this.handleArr.length ? this.handleNum+=1 : this.handleNum = this.handleArr.length);
                }
                else {
                    //反选删除选中的数组
                    this.handleArr.splice(this.handleArr.indexOf(val), 1);
                    //循环绑定的数据来判断顺序值是否需要减少
                    for (let item in this.pro_selections_items) {
                        if (this.pro_selections_items[item].order > 0 && this.pro_selections_items[item].order > val.order) {
                            this.pro_selections_items[item].order--;
                            this.handleNum--;
                        }
                    }
                    //同步数组长度
                    this.handleNum = this.handleArr.length;
                    val.order = ''; //反选清空顺序值
                }
                //更新propertiesData
                NetService.updateSunburstData(this.handleArr, (res, err) => {
                    if(err){
                        console.error(err);
                        return;
                    }
                    if(!res.data){
                        console.log('data not exists!');
                        return;
                    }
                    console.log('updateSunburstData');
                    //{"code":0,"data":{"name":"all","children":[{"name":"healthy","children":[{"name":"female","size":2},{"name":"male","size":2}]},{"name":"AD","children":[{"name":"female","size":5},{"name":"male","size":7}]}]},"propertiesArr":["a","b","c"],"message":"获取成功"}
                    var sunData = res.data.data;
                    this.addIndex(sunData, 0);
                    sunData.index = "0"; //在同辈中排第几
                    sunData.peer = "1"; //同辈份的总数
                    console.log('sunData ', sunData);
                    this.sunburstData = sunData; //res.data.data;
                });
            },
            reload_properties(){
                this.sunburstData = {
                    "name": "all",
                    "size": 87,
                    "index": "0",
                    "depth": 0,
                    "peer": 1,
                };
                this.handleArr = [];
                this.handleNum = 0;
                for(var a in this.pro_selections_items){
                    this.pro_selections_items[a].checkval = false;
                    this.pro_selections_items[a].order = '';
                }
                this.$store.commit('reset_selected_subjects');
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
            //排序方法：横坐标为subjects，纵坐标为links，上下组横向分别按照达标数目排序
            setOption(pointUp, pointBottom, index){
                //point为三维数组， index指示某一列为该特征
                //var point = [[[0.1,0.2,0.3,'a'],[0.2,0.3,0.4,'b'],[0.1,0.4,0.7,'c']],[[0.2,0.4,0.3,'d'],[0.4,0.3,0.1,'e'],[0.5,0.7,0.3,'f']],[[0.2,0.4,0.2,'h'],[0.4,0.3,0.7,'i'],[0.5,0.7,0.7,'j']]];
                //var index = 2;
                //深度拷贝,拷贝下半部分
                var tmp = pointBottom.concat([]);
                var subjectlen = tmp.length;
                var maxNum = subjectlen;
                var linkslen = tmp[0].length; //2000+
                var featurelen = tmp[0][0].length; //6
                var that = this;
                var pValueThreshold = that.pValueThreshold / 100.0;
                
                //对于每个人,2000+行，代表连接
                for (var i in tmp) {
                    tmp[i].sort(function(x, y){
                        return x[index] - y[index];
                    }).reverse(); 
                }
                //横向排序
                tmp.sort(function(x,y) {
                    return x.filter(function(z){
                        return z[index] >= pValueThreshold;
                    }).length - y.filter(function(z){
                        return z[index] >= pValueThreshold;
                    }).length;
                }).reverse();
                //获得直线
                var lineDataBottom = [];
                var valid = this.validThreshold / 100.0 * linkslen;
                var pushZero = false;
                for (var i in tmp) {
                    var validLinks = tmp[i].filter(function(z){return z[index] >= pValueThreshold}).length;
                    if (validLinks>= valid){
                        lineDataBottom.push(-validLinks);
                    } else {
                        pushZero = true;
                        lineDataBottom.push(0);
                        break;
                    }
                }
                if(!pushZero) {
                    lineDataBottom.push(0);
                }
                var data2 = [];
                for (var i in tmp) {
                    for (var j = tmp[i].length - 1; j >= 0; j--) {
                        data2.push([Number(i), -Number(j)-1, tmp[i][j][index], tmp[i][j][featurelen - 1]]);
                    }
                }
                //拷贝上半部分
                tmp = [];
                tmp = pointUp.concat([]);
                subjectlen = tmp.length;
                if (subjectlen > maxNum) maxNum = subjectlen;
                //featurelen = tmp[0][0].length; 

                //对于每个人,2000+行，代表连接
                for (var i in tmp) {
                    tmp[i].sort(function(x, y){
                        return x[index] - y[index];
                    }).reverse(); 
                }
                //横向排序
                tmp.sort(function(x,y) {
                    return x.filter(function(z){
                        return z[index] >= pValueThreshold;
                    }).length - y.filter(function(z){
                        return z[index] >= pValueThreshold;
                    }).length;
                }).reverse();
                //获得直线
                var lineDataTop = [];
                pushZero = false;
                for (var i in tmp) {
                    var validLinks = tmp[i].filter(function(z){return z[index] >= pValueThreshold}).length;
                    if (validLinks>= valid){
                        lineDataTop.push(validLinks);
                    } else {
                        pushZero = true;
                        lineDataTop.push(0);
                        break;
                    }
                }
                if(!pushZero){
                    lineDataTop.push(0);
                }
                var data = [];
                for (var i in tmp) {
                    for (var j in tmp[i]) {
                        data.push([Number(i), Number(j), tmp[i][j][index], tmp[i][j][featurelen - 1]]);
                    }
                }
                data = data.map(function (item) {
                    return [item[0], item[1], item[2], item[3] || '-'];
                });
                data2 = data2.map(function (item) {
                    return [item[0], item[1], item[2], item[3] || '-'];
                });
                this.features[index].options = {
                    tooltip: {
                        position: 'top',
                        
                        formatter: function (params) {
                            if (params.value.length > 1) {
                                return params.value[3] + ' ' + params.value[2];
                            }
                        }
                    },
                    animation: false,
                    grid: {
                        height: 200,
                        width: 200,
                        top: 0
                    },
                    xAxis: [{
                        type: 'category',
                        //data: hours,
                        axisLabel: {
                            show: false
                        },
                        //max: this.maxSubjectLength
                        //offset: -100
                    },{
                        type: 'category',
                        max: maxNum,
                        boundaryGap: false,
                        axisLabel: {
                            show: false
                        },
                        axisTick: {
                            show: false
                        },
                        axisLine:{
                            show: false
                        }
                    }
                    ],
                    yAxis: [{
                        type: 'category',
                        //data: days,
                        axisLabel: {
                            show: false
                        },
                        axisTick: {
                            show: false
                        }
                    },{
                        type: 'value',
                        max: linkslen,
                        boundaryGap: false,
                        axisLabel: {
                            show: false
                        },
                        axisTick: {
                            show: false
                        },
                        axisLine:{
                            show: false
                        },
                        splitLine: {
                            show: false
                        }
                    }],
                    visualMap: {
                        min: 0,
                        max: 1,
                        calculable: true,
                        orient: 'vertical',
                        left: 'right',
                        bottom: 'center',
                        dimension: 2,
                        padding: 20,
                        itemHeight: 100,
                        precision: 2,
                        inRange: {color: ['white', 'blue']}
                    },
                    color: 'rgba(255,255,255,1)',
                    series: [{
                        name: 'Punch Card',
                        type: 'heatmap',
                        data: data2,
                        label: {
                            show: false
                        },
                        emphasis: {
                            itemStyle: {
                                shadowBlur: 10,
                                shadowColor: 'rgba(0, 0, 0, 0.5)'
                            }
                        }
                    },
                    {
                        name: 'Punch Card',
                        type: 'heatmap',
                        data: data,
                        label: {
                            show: false
                        },
                        emphasis: {
                            itemStyle: {
                                shadowBlur: 10,
                                shadowColor: 'rgba(0, 0, 0, 0.5)'
                            }
                        },

                    },
                    {
                        name: 'Step Middle 1',
                        type: 'line',
                        step: 'end',
                        data: lineDataTop,
                        xAxisIndex: 1,
                        yAxisIndex: 1,
                        symbol: 'none',
                        itemStyle: {
                            normal: { 
                                lineStyle: { type: 'solid', color:'red'},
                                label: { show: false, position:'left' }
                            }
                        },
                    },
                    {
                        name: 'Step Middle 2',
                        type: 'line',
                        step: 'end',
                        data: lineDataBottom,
                        xAxisIndex: 1,
                        yAxisIndex: 1,
                        symbol: 'none',
                        itemStyle: {
                            normal: { 
                                lineStyle: { type: 'solid', color:'red'},
                                label: { show: false, position:'left' }
                            }
                        },
                    },
                    {
                        name: 'markLine',
                        type: 'line',
                        step: 'end',
                        xAxisIndex: 1,
                        yAxisIndex: 1,
                        markLine:{
                            symbol: ['none', 'none'],//去掉箭头
                            itemStyle: {
                                normal: { 
                                    lineStyle: { type: 'solid', color:'red'},
                                    label: { show: false, position:'left' }
                                }
                            },
                            yAxisIndex: 1,
                            data: [{
                                name: 'Y 轴值为 100 的水平线',
                                yAxis: 0
                            },]
                        },
                    },
                    {
                        name: 'point',
                        type: 'scatter',
                        step: 'end',
                        data: [[0, linkslen],[0, -linkslen]],
                        xAxisIndex: 1,
                        yAxisIndex: 1,
                        symbolSize: 0,
                    }
                    ]
                };
            },
            //排序方法：横坐标为subjects，纵坐标为links，上下组横向合起来按照达标之和数目排序
            setOptionSum(pointUp, pointBottom, index){
                //point为三维数组,index指示某一列为该特征
                //var point = [[[0.1,0.2,0.3,'a'],[0.2,0.3,0.4,'b'],[0.1,0.4,0.7,'c']],[[0.2,0.4,0.3,'d'],[0.4,0.3,0.1,'e'],[0.5,0.7,0.3,'f']],[[0.2,0.4,0.2,'h'],[0.4,0.3,0.7,'i'],[0.5,0.7,0.7,'j']]];
                //var index = 2;
                //深度拷贝,拷贝下半部分
                var tmp = this.deepCopy(pointBottom);
                var subjectlen = tmp.length;
                var maxNum = subjectlen;
                var downSubjectslen = tmp[0].length; //多少个人
                var featurelen = tmp[0][0].length; //6
                var that = this;
                var pValueThreshold = that.pValueThreshold / 100.0;
                
                //对每一列进行排序，从小到大
                for (var i in tmp) {
                    tmp[i].sort(function(x, y){
                        return x[index] - y[index];
                    }); 
                }        

                //对上半部分操作
                var tmpUp = this.deepCopy(pointUp);
                var upSubjectslen = tmpUp[0].length;

                //每一列进行排序，从大到小
                for (var i in tmpUp) {
                    tmpUp[i].sort(function(x, y){
                        return x[index] - y[index];
                    }).reverse(); 
                }

                //将tmp和tmpUp合并到tmp
                for (var i in tmp){
                    tmp[i].push.apply(tmp[i], tmpUp[i]);
                }
                //tmp.push.apply(tmp, tmpUp);

                //总人数，达标阈值
                var totalSubjects = tmp[0].length;
                var validSubjectsThreshold = totalSubjects * this.validThreshold / 100;
                var halfSubjects = downSubjectslen; //下半部分人数

                //横向排序，按达标人数排序
                tmp.sort(function(x,y) {
                    return x.filter(function(z){
                        return z[index] >= pValueThreshold;
                    }).length - y.filter(function(z){
                        return z[index] >= pValueThreshold;
                    }).length;
                }).reverse();
                
                //获取上曲线和下曲线
                //对于每个连接,数据小-大-小
                var lineDataBottom = [];
                var lineDataTop = [];
                for(var i in tmp) {
                    var findDown = false;
                    var findUp = false;
                    for(var j in tmp[i]) {
                        if(tmp[i][j][index] >= pValueThreshold && j < halfSubjects && !findDown) {
                            lineDataBottom.push(j - halfSubjects);
                            findDown = true;
                        }
                        if(tmp[i][j][index] < pValueThreshold && j >= halfSubjects  && !findUp) {
                            lineDataTop.push(j - halfSubjects);
                            findUp = true;
                        }
                        //top的边界
                        if(tmp[i][j][index] >= pValueThreshold && (j == (totalSubjects - 1))  && !findUp) {
                            lineDataTop.push(j - halfSubjects + 1);
                            findUp = true;
                        }
                    }
                    if(!findDown){
                        lineDataBottom.push(0);
                    }
                    if(!findUp){
                        lineDataTop.push(0);
                    }
                }
                var last = lineDataBottom.pop();
                lineDataBottom.push(last);
                lineDataBottom.push(last);
                last = lineDataTop.pop();
                lineDataTop.push(last);
                lineDataTop.push(last);

                //找到人数达标与非达标的分界线
                var validSubjects = tmp.filter(function(x) {
                    return x.filter(function(z){
                        return z[index] >= pValueThreshold;
                    }).length >= validSubjectsThreshold;
                }).length;

                var data = [];
                for (var i in tmp) {
                    for (var j in tmp[i]) {
                        data.push([Number(i), Number(j - halfSubjects), tmp[i][j][index], tmp[i][j][featurelen - 1]]);
                    }
                }

                console.log('top', lineDataTop);
                console.log('bottom', lineDataBottom);
                console.log('validSubjects', validSubjects);
                console.log(index, data);
                console.log(tmp);
                
                var xdata = [];
                for(var i in tmp){
                    xdata.push(i);
                }
                data = data.map(function (item) {
                    return [item[0], item[1], item[2], item[3] || '-'];
                });

                this.features[index].options = {
                    tooltip: {
                        position: 'top',
                        
                        formatter: function (params) {
                            if (params.value.length > 1) {
                                return params.value[3] + ' ' + params.value[2];
                            }
                        }
                    },
                    toolbox: {
                        show: true,
                        feature: {
                            //saveAsImage: {}
                        }
                    },
                    animation: false,
                    grid: {
                        height: 200,
                        width: 200,
                        top: 10
                    },
                    xAxis: [{
                        type: 'category',
                        //data: hours,
                        //z: 999,
                        axisLabel: {
                            show: false
                        },
                        //max: this.maxSubjectLength
                        //offset: -100
                    },{
                        type: 'category',
                        data: xdata,
                        max: maxNum,
                        boundaryGap: false,
                        axisLabel: {
                            show: false
                        },
                        axisTick: {
                            show: false
                        },
                        axisLine:{
                            show: false
                        }
                    }
                    ],
                    yAxis: [{
                        type: 'category',
                        //data: days,
                        //name: 'Group 1',
                        //nameLocation: 'end',
                        axisLabel: {
                            show: false
                        },
                        axisTick: {
                            show: false
                        }
                    },{
                        type: 'value',
                        //name: 'Group 2',
                        //nameLocation: 'start',
                        //position: 'left',
                        //z: 99,
                        //max: upSubjectslen + downSubjectslen,
                        boundaryGap: false,
                        min: 'dataMin', //最小值为刻度最小值
                        max: 'dataMax',
                        axisLabel: {
                            show: false
                        },
                        axisTick: {
                            show: false
                        },
                        axisLine:{
                            show: false
                        },
                        splitLine: {
                            show: false
                        }
                    },{
                        type: 'category',
                        data: ['Group 2', 'Group 1'],
                        position: 'left',
                        //boundaryGap: false,
                        axisTick: {
                            show: false
                        },
                        axisLine:{
                            show: false
                        },
                        splitLine: {
                            show: false
                        },
                        axisLabel: {
                            show: true,
                            rotate: 90
                        },
                    }],
                    visualMap: {
                        min: 0,
                        max: 1,
                        z: -1,
                        calculable: true,
                        orient: 'vertical',
                        left: 'right',
                        top: 'top',
                        dimension: 2,
                        padding: 20,
                        itemHeight: 100,
                        precision: 2,
                        inRange: {color: ['white', 'blue']}
                    },
                    //color: 'rgba(255,255,255,1)',
                    series: [
                    {
                        name: 'Punch Card',
                        type: 'heatmap',
                        data: data,
                        label: {
                            show: false
                        },
                        emphasis: {
                            itemStyle: {
                                shadowBlur: 10,
                                shadowColor: 'rgba(0, 0, 0, 0.5)'
                            }
                        },
                    },
                    {
                        name: 'Up line',
                        type: 'line',
                        step: 'end',
                        data: lineDataTop,
                        xAxisIndex: 1,
                        yAxisIndex: 1,
                        symbol: 'none',
                        itemStyle: {
                            normal: { 
                                lineStyle: { type: 'solid', color:'red', width: 0.1},
                                label: { show: false, position:'left' }
                            }
                        },
                    },
                    {
                        name: 'Down line',
                        type: 'line',
                        step: 'end',
                        data: lineDataBottom,
                        xAxisIndex: 1,
                        yAxisIndex: 1,
                        symbol: 'none',
                        itemStyle: {
                            normal: { 
                                lineStyle: { type: 'solid', color:'red', width:0.1},
                                label: { show: false, position:'left' }
                            }
                        },
                    },
                    {
                        name: 'xAxis',
                        type: 'line',
                        xAxisIndex: 1,
                        yAxisIndex: 1,
                        //z: 999,
                        markLine:{
                            symbol: ['none', 'none'],//去掉箭头
                            itemStyle: {
                                normal: { 
                                    lineStyle: { type: 'solid', width: 3, color:'white'},
                                    label: { show: false, position:'left' }
                                }
                            },
                            data: [{
                                name: 'Y 轴值为 0 的水平线',
                                yAxis: 0
                            }]
                        },
                    },
                    /*
                    {
                        name: 'xAxis',
                        type: 'line',
                        xAxisIndex: 1,
                        yAxisIndex: 1,
                        //z: 99,
                        markLine:{
                            symbol: ['none', 'none'],//去掉箭头
                            itemStyle: {
                                normal: { 
                                    lineStyle: { type: 'solid', width: 3, color:'red'},
                                    label: { show: false, position:'left' }
                                }
                            },
                            data: [
                            {
                                name: 'valid',
                                xAxis: validSubjects,
                            }]
                        },
                    },*/
                    {
                        name: 'valid',
                        type: 'line',
                        xAxisIndex: 1,
                        yAxisIndex: 1,
                        z:999,
                        data: [
                            [validSubjects, lineDataTop[validSubjects]],
                            [validSubjects, lineDataBottom[validSubjects]]
                        ],
                        smooth: true,
                        showSymbol: false,
                        itemStyle: {
                            normal: {
                                lineStyle: { type: 'solid', width: 3, color:'green'},
                                label: { show: false, position:'left' }
                            }
                        }
                    },
                    {
                        name: 'point',
                        type: 'scatter',
                        data: [[0, upSubjectslen],[0, -downSubjectslen]],
                        xAxisIndex: 1,
                        yAxisIndex: 1,
                        symbolSize: 0,
                    }
                    ]
                };
            },
            //排序方法：横坐标为subjects，纵坐标为links，上下组横向合起来按照达标之和数目排序
            setOptionSub(pointUp, pointBottom, index){
                //point为三维数组,index指示某一列为该特征
                //var point = [[[0.1,0.2,0.3,'a'],[0.2,0.3,0.4,'b'],[0.1,0.4,0.7,'c']],[[0.2,0.4,0.3,'d'],[0.4,0.3,0.1,'e'],[0.5,0.7,0.3,'f']],[[0.2,0.4,0.2,'h'],[0.4,0.3,0.7,'i'],[0.5,0.7,0.7,'j']]];
                //var index = 2;
                //深度拷贝,拷贝下半部分
                var tmp = this.deepCopy(pointBottom);
                var subjectlen = tmp.length;
                var maxNum = subjectlen;
                var downSubjectslen = tmp[0].length; //多少个人
                var featurelen = tmp[0][0].length; //6
                var that = this;
                var pValueThreshold = that.pValueThreshold / 100.0;
                
                //对每一列进行排序，从小到大
                for (var i in tmp) {
                    tmp[i].sort(function(x, y){
                        return x[index] - y[index];
                    }); 
                }        

                //对上半部分操作
                var tmpUp = this.deepCopy(pointUp);
                var upSubjectslen = tmpUp[0].length;

                //每一列进行排序，从大到小
                for (var i in tmpUp) {
                    tmpUp[i].sort(function(x, y){
                        return x[index] - y[index];
                    }).reverse(); 
                }

                //将tmp和tmpUp合并到tmp,4维矩阵
                for (var i in tmp){
                    tmp[i] = [tmp[i], tmpUp[i]];
                }
                //tmp.push.apply(tmp, tmpUp);

                //总人数，达标阈值
                var totalSubjects = tmp[0][0].length + tmp[0][1].length;
                var validSubjectsThreshold = totalSubjects * this.validThreshold / 100;
                var halfSubjects = downSubjectslen; //下半部分人数

                //横向排序，按达标人数的减排序,上减下
                tmp.sort(function(x,y) {
                    return (x[1].filter(function(z) {
                        return z[index] >= pValueThreshold;
                    }).length - x[0].filter(function(z) {
                        return z[index] >= pValueThreshold;
                    }).length) - (y[1].filter(function(z) {
                        return z[index] >= pValueThreshold;
                    }).length - y[0].filter(function(z) {
                        return z[index] >= pValueThreshold;
                    }).length);
                }).reverse();
                
                //获取上曲线和下曲线
                //对于每个连接,数据小-大-小
                var lineDataBottom = [];
                var lineDataTop = [];
                for(var i in tmp) {
                    //下部分
                    var findUp = false;
                    var findDown = false;
                    for(var j in tmp[i][0]) {
                        if(tmp[i][0][j][index] >= pValueThreshold) {
                            lineDataBottom.push(j - halfSubjects);
                            findDown = true;
                            break;
                        }
                    }
                    //上部分
                    for(var j in tmp[i][1]) {
                        if(tmp[i][1][j][index] < pValueThreshold) {
                            lineDataTop.push(j - 0);
                            findUp = true;
                            break;
                        }
                    }
                    if(!findDown) {
                        lineDataBottom.push(0);
                    }
                    if(!findUp) {
                        lineDataTop.push(upSubjectslen);
                    }
                }
                var last = lineDataBottom.pop();
                lineDataBottom.push(last);
                lineDataBottom.push(last);
                last = lineDataTop.pop();
                lineDataTop.push(last);
                lineDataTop.push(last);

                /*
                //找到人数达标与非达标的分界线
                var validSubjects = tmp.filter(function(x) {
                    return x.filter(function(z){
                        return z[index] >= pValueThreshold;
                    }).length >= validSubjectsThreshold;
                }).length;
                */

                var data = [];
                for (var i in tmp) {
                    for (var j in tmp[i][0]) {
                        //x坐标，y坐标，数值，名称
                        data.push([Number(i), Number(j - halfSubjects), tmp[i][0][j][index], tmp[i][0][j][featurelen - 1]]);
                        //data.push([Number(i ), Number(j - halfSubjects ), tmp[i][j][index], tmp[i][j][featurelen - 1]]);
                    }
                    for(var j in tmp[i][1]) {
                        data.push([Number(i), Number(j), tmp[i][1][j][index], tmp[i][1][j][featurelen - 1]]);
                    }
                }

                console.log('top', lineDataTop);
                console.log('bottom', lineDataBottom);
                //console.log('validSubjects', validSubjects);
                console.log(index, data);
                
                var xdata = [];
                for(var i in tmp){
                    xdata.push(i);
                }
                data = data.map(function (item) {
                    return [item[0], item[1], item[2], item[3] || '-'];
                });

                this.features[index].options = {
                    tooltip: {
                        position: 'top',
                        
                        formatter: function (params) {
                            if (params.value.length > 1) {
                                return params.value[3] + ' ' + params.value[2];
                            }
                        }
                    },
                    toolbox: {
                        show: true,
                        feature: {
                            //saveAsImage: {}
                        }
                    },
                    animation: false,
                    grid: {
                        height: 200,
                        width: 200,
                        top: 10
                    },
                    xAxis: [{
                        type: 'category',
                        //data: hours,
                        //z: 999,
                        axisLabel: {
                            show: false
                        },
                        //max: this.maxSubjectLength
                        //offset: -100
                    },{
                        type: 'category',
                        data: xdata,
                        max: maxNum,
                        boundaryGap: false,
                        axisLabel: {
                            show: false
                        },
                        axisTick: {
                            show: false
                        },
                        axisLine:{
                            show: false
                        }
                    }
                    ],
                    yAxis: [{
                        type: 'category',
                        //data: days,
                        //name: 'Group 1',
                        //nameLocation: 'end',
                        axisLabel: {
                            show: false
                        },
                        axisTick: {
                            show: false
                        }
                    },{
                        type: 'value',
                        //name: 'Group 2',
                        //nameLocation: 'start',
                        //position: 'left',
                        //z: 99,
                        //max: upSubjectslen + downSubjectslen,
                        boundaryGap: false,
                        min: 'dataMin',
                        max: 'dataMax',
                        axisLabel: {
                            show: false
                        },
                        axisTick: {
                            show: false
                        },
                        axisLine:{
                            show: false
                        },
                        splitLine: {
                            show: false
                        }
                    },{
                        type: 'category',
                        data: ['Group 2', 'Group 1'],
                        position: 'left',
                        //boundaryGap: false,
                        //min: 'dataMin',
                        //max: 'dataMax',
                        axisTick: {
                            show: false
                        },
                        axisLine:{
                            show: false
                        },
                        splitLine: {
                            show: false
                        },
                        axisLabel: {
                            show: true,
                            rotate: 90
                        },
                    }],
                    visualMap: {
                        min: 0,
                        max: 1,
                        calculable: true,
                        orient: 'vertical',
                        left: 'right',
                        bottom: 'center',
                        dimension: 2,
                        padding: 20,
                        itemHeight: 100,
                        precision: 2,
                        inRange: {color: ['white', 'blue']}
                    },
                    color: 'rgba(255,255,255,1)',
                    series: [
                    {
                        name: 'Punch Card',
                        type: 'heatmap',
                        data: data,
                        label: {
                            show: false
                        },
                        emphasis: {
                            itemStyle: {
                                shadowBlur: 10,
                                shadowColor: 'rgba(0, 0, 0, 0.5)'
                            }
                        },
                    },
                    {
                        name: 'Up line',
                        type: 'line',
                        step: 'end',
                        data: lineDataTop,
                        xAxisIndex: 1,
                        yAxisIndex: 1,
                        symbol: 'none',
                        itemStyle: {
                            normal: { 
                                lineStyle: { type: 'solid', color:'red', width: 0.1},
                                label: { show: false, position:'left' }
                            }
                        },
                    },
                    {
                        name: 'Down line',
                        type: 'line',
                        step: 'end',
                        data: lineDataBottom,
                        xAxisIndex: 1,
                        yAxisIndex: 1,
                        symbol: 'none',
                        itemStyle: {
                            normal: { 
                                lineStyle: { type: 'solid', color:'red', width: 0.1},
                                label: { show: false, position:'left' }
                            }
                        },
                    },
                    {
                        name: 'xAxis',
                        type: 'line',
                        xAxisIndex: 1,
                        yAxisIndex: 1,
                        //z: 999,
                        markLine:{
                            symbol: ['none', 'none'],//去掉箭头
                            itemStyle: {
                                normal: { 
                                    lineStyle: { type: 'solid', width: 3, color:'white'},
                                    label: { show: false, position:'left' }
                                }
                            },
                            data: [{
                                name: 'Y 轴值为 0 的水平线',
                                yAxis: 0
                            }]
                        },
                    },
                    /*
                    {
                        name: 'xAxis',
                        type: 'line',
                        xAxisIndex: 1,
                        yAxisIndex: 1,
                        //z: 99,
                        markLine:{
                            symbol: ['none', 'none'],//去掉箭头
                            itemStyle: {
                                normal: { 
                                    lineStyle: { type: 'solid', width: 3, color:'red'},
                                    label: { show: false, position:'left' }
                                }
                            },
                            data: [
                            {
                                name: 'valid',
                                xAxis: validSubjects,
                            }]
                        },
                    },*/
                    /*
                    {
                        name: 'valid',
                        type: 'line',
                        xAxisIndex: 1,
                        yAxisIndex: 1,
                        z:999,
                        data: [
                            [validSubjects, lineDataTop[validSubjects]],
                            [validSubjects, lineDataBottom[validSubjects]]
                        ],
                        smooth: true,
                        showSymbol: false,
                        itemStyle: {
                            normal: {
                                lineStyle: { type: 'solid', width: 3, color:'green'},
                                label: { show: false, position:'left' }
                            }
                        }
                    },*/
                    {
                        name: 'point',
                        type: 'scatter',
                        data: [[0, upSubjectslen],[0, -downSubjectslen]],
                        xAxisIndex: 1,
                        yAxisIndex: 1,
                        symbolSize: 0,
                    }
                    ]
                };
            },
            //排序方法：横坐标为subjects，纵坐标为links，上下组横向合起来按照达标之和数目排序
            setOptionSumLine(pointUp, pointBottom, index){
                //point为三维数组,index指示某一列为该特征
                //var point = [[[0.1,0.2,0.3,'a'],[0.2,0.3,0.4,'b'],[0.1,0.4,0.7,'c']],[[0.2,0.4,0.3,'d'],[0.4,0.3,0.1,'e'],[0.5,0.7,0.3,'f']],[[0.2,0.4,0.2,'h'],[0.4,0.3,0.7,'i'],[0.5,0.7,0.7,'j']]];
                //var index = 2;
                //深度拷贝,拷贝下半部分
                var tmp = this.deepCopy(pointBottom);
                var subjectlen = tmp.length;
                var maxNum = subjectlen;
                var downSubjectslen = tmp[0].length; //多少个人
                var featurelen = tmp[0][0].length; //6
                var that = this;
                var pValueThreshold = that.pValueThreshold / 100.0;
                
                //对每一列进行排序，从小到大
                for (var i in tmp) {
                    tmp[i].sort(function(x, y){
                        return x[index] - y[index];
                    }); 
                }        

                //对上半部分操作
                var tmpUp = this.deepCopy(pointUp);
                var upSubjectslen = tmpUp[0].length;

                //每一列进行排序，从大到小
                for (var i in tmpUp) {
                    tmpUp[i].sort(function(x, y){
                        return x[index] - y[index];
                    }).reverse(); 
                }

                //将tmp和tmpUp合并到tmp,4维矩阵
                for (var i in tmp){
                    tmp[i] = [tmp[i], tmpUp[i]];
                }
                //tmp.push.apply(tmp, tmpUp);

                //总人数，达标阈值
                var totalSubjects = tmp[0][0].length + tmp[0][1].length;
                var validSubjectsThreshold = totalSubjects * this.validThreshold / 100;
                var halfSubjects = downSubjectslen; //下半部分人数

                //横向排序，按达标人数的加排序,上加下
                tmp.sort(function(x,y) {
                    return (x[1].filter(function(z) {
                        return z[index] >= pValueThreshold;
                    }).length + x[0].filter(function(z) {
                        return z[index] >= pValueThreshold;
                    }).length) - (y[1].filter(function(z) {
                        return z[index] >= pValueThreshold;
                    }).length + y[0].filter(function(z) {
                        return z[index] >= pValueThreshold;
                    }).length);
                }).reverse();
                
                //sample
                var sampleRate = Math.ceil(tmp.length / ((window.screen.width / window.innerWidth) * 250));
                tmp = tmp.filter(function(x,i){
                    return i % sampleRate == 0;
                });
                
                console.log('sampleRate', sampleRate);
                //找每一分段人数1, [0.9, 1), ...,(0,0.1),0
                //0和1应单独分出来
                var numTop = new Array();
                var numBottom = new Array();
                var seg = 10;
                //对于每一分段，找每组的该分段人数
                for (var i = 0; i < 11; i++) {
                    numTop[i] = new Array();
                    numBottom[i] = new Array();
                    for(var j in tmp) {
                        if(i == 0) {//>=1
                            numTop[i][j] = tmp[j][0].filter(function(z){
                                return (z[index] >= seg / 10) && (z[index] <= ((seg + 1)/10));
                            }).length;
                            numBottom[i][j] = tmp[j][1].filter(function(z){
                                return (z[index] >= seg / 10) && (z[index] <= ((seg + 1)/10));
                            }).length;    
                        }
                        else if(i == 10){//(0,0.1)
                            numTop[i][j] = tmp[j][0].filter(function(z){
                                return (z[index] > seg / 10) && (z[index] < ((seg + 1)/10));
                            }).length;
                            numBottom[i][j] = tmp[j][1].filter(function(z){
                                return (z[index] > seg / 10) && (z[index] < ((seg + 1)/10));
                            }).length;
                        }
                        else {
                            numTop[i][j] = tmp[j][0].filter(function(z){
                                return (z[index] >= seg / 10) && (z[index] < ((seg + 1)/10));
                            }).length;
                            numBottom[i][j] = tmp[j][1].filter(function(z){
                                return (z[index] >= seg / 10) && (z[index] < ((seg + 1)/10));
                            }).length;   
                        }
                    }
                    seg = seg - 1;
                }
                numTop[11] = new Array();
                numBottom[11] = new Array();
                for(var j in tmp) {
                    numTop[11][j] = tmp[j][0].filter(function(z){
                                return z[index] == 0;
                            }).length;
                    numBottom[11][j] = tmp[j][1].filter(function(z){
                                return z[index] == 0;
                            }).length;    
                }
                

                console.log('num', numTop, numBottom);
                console.log('seg', seg);
                //获取上曲线和下曲线
                //对于每个连接,数据小-大-小
                var lineDataBottom = [];
                var lineDataTop = [];
                var linexAxis = [];
                for(var i in tmp) {
                    linexAxis[i] = i;
                    //下部分
                    var findUp = false;
                    var findDown = false;
                    for(var j in tmp[i][0]) {
                        if(tmp[i][0][j][index] >= pValueThreshold) {
                            lineDataBottom.push(j - halfSubjects);
                            findDown = true;
                            break;
                        }
                    }
                    //上部分
                    for(var j in tmp[i][1]) {
                        if(tmp[i][1][j][index] < pValueThreshold) {
                            lineDataTop.push(j - 0);
                            findUp = true;
                            break;
                        }
                    }
                    if(!findDown) {
                        lineDataBottom.push(0);
                    }
                    if(!findUp) {
                        lineDataTop.push(upSubjectslen);
                    }
                }
                for (var i in lineDataBottom) {
                    lineDataBottom[i] = lineDataBottom[i] + halfSubjects;
                    lineDataTop[i] = lineDataTop[i] + halfSubjects;
                }
                /*
                var last = lineDataBottom.pop();
                lineDataBottom.push(last);
                lineDataBottom.push(last);
                last = lineDataTop.pop();
                lineDataTop.push(last);
                lineDataTop.push(last);
                */
                //找到人数达标与非达标的分界线
                var validSubjects = tmp.filter(function(x) {
                    return (x[0].filter(function(z){
                        return z[index] >= pValueThreshold;
                    }).length + x[1].filter(function(z){
                        return z[index] >= pValueThreshold;
                    }).length) >= validSubjectsThreshold;
                }).length;

                if(validSubjects != 0) validSubjects = validSubjects - 1;

                this.features[index].options = {
                    title: {
                        //text: '堆叠区域图'
                    },
                    tooltip: {
                        trigger: 'axis',
                        axisPointer: {
                            type: 'cross',
                            label: {
                                backgroundColor: '#6a7985'
                            }
                        },
                    },
                    legend: {
                        show: false
                    },
                    toolbox: {
                        feature: {
                            //saveAsImage: {}
                        }
                    },
                    grid: {
                        height: 100,
                        width: 250,
                        top: 10
                    },
                    xAxis: [
                        {
                            type: 'category',
                            boundaryGap: false,
                            axisLabel: {
                                show: false
                            },
                            axisTick: {
                                show: false
                            },
                            axisLine:{
                                show: false
                            },
                            data: linexAxis
                        }
                    ],
                    yAxis: [
                        {
                            axisLabel: {
                                show: false
                            },
                            axisTick: {
                                show: false
                            },
                            axisLine:{
                                show: false
                            },
                            splitLine: {
                                show: false
                            },
                            type: 'value'
                        },
                        {
                            type: 'category',
                            data: ['G2', 'G1'],
                            position: 'left',
                            axisTick: {
                                show: false
                            },
                            axisLine:{
                         
                                show: false
                            },
                            splitLine: {
                                show: false
                            },
                            axisLabel: {
                                show: true,
                                rotate: 90
                            },
                        }
                    ],
                    series: [
                        {
                            symbol: 'none',
                            name: '组00',
                            type: 'line',
                            stack: '总量',
                            lineStyle:{
                                width: 0,
                            },
                            areaStyle: {
                                //color: 'rgb(0,0,125)'
                                //color: 'rgb(63,0,125)'
                                //color: 'rgb(84,39,143)'
                                //color: 'rgb(106,81,163)'
                                //color: 'rgb(128,125,186)'
                                //color: 'rgb(158,154,200)'
                                //color: 'rgb(188,189,220)'
                                //color: 'rgb(218,218,235)'
                                //color: 'rgb(239,237,245)'
                                color: 'rgb(252,251,253)'
                            },
                            data: numTop[11]
                        },
                        {
                            symbol: 'none',
                            name: '组2(0,0.1)',
                            type: 'line',
                            stack: '总量',
                            lineStyle:{
                                width: 0,
                            },
                            areaStyle: {
                                //color: 'rgb(0,0,125)'
                                //color: 'rgb(63,0,125)'
                                //color: 'rgb(84,39,143)'
                                //color: 'rgb(106,81,163)'
                                //color: 'rgb(128,125,186)'
                                //color: 'rgb(158,154,200)'
                                //color: 'rgb(188,189,220)'
                                //color: 'rgb(218,218,235)'
                                //color: 'rgb(239,237,245)'
                                color: 'rgb(244,244,250)'
                                //color: 'rgb(252,251,253)'
                            },
                            data: numTop[10]
                        },
                        {
                            symbol: 'none',
                            name: '组2[0.1,0.2)',
                            type: 'line',
                            stack: '总量',
                            lineStyle:{
                                width: 0,
                            },
                            areaStyle: {
                                //color: 'rgb(0,0,125)'
                                //color: 'rgb(63,0,125)'
                                //color: 'rgb(84,39,143)'
                                //color: 'rgb(106,81,163)'
                                //color: 'rgb(128,125,186)'
                                //color: 'rgb(158,154,200)'
                                //color: 'rgb(188,189,220)'
                                //color: 'rgb(218,218,235)'
                                color: 'rgb(239,237,245)'
                                //color: 'rgb(252,251,253)'
                            },
                            data: numTop[9]
                        },
                        
                        {
                            symbol: 'none',
                            name: '组2[0.2,0.3)',
                            type: 'line',
                            stack: '总量',
                            lineStyle:{
                                width: 0,
                            },
                            areaStyle: {
                                //color: 'rgb(0,0,125)'
                                //color: 'rgb(63,0,125)'
                                //color: 'rgb(84,39,143)'
                                //color: 'rgb(106,81,163)'
                                //color: 'rgb(128,125,186)'
                                //color: 'rgb(158,154,200)'
                                //color: 'rgb(188,189,220)'
                                color: 'rgb(218,218,235)'
                                //color: 'rgb(239,237,245)'
                                //color: 'rgb(252,251,253)'
                            },
                            data: numTop[8]
                        },
                        {
                            symbol: 'none',
                            name: '组2[0.3,0.4)',
                            type: 'line',
                            stack: '总量',
                            lineStyle:{
                                width: 0,
                            },
                            areaStyle: {
                                //color: 'rgb(0,0,125)'
                                //color: 'rgb(63,0,125)'
                                //color: 'rgb(84,39,143)'
                                //color: 'rgb(106,81,163)'
                                //color: 'rgb(128,125,186)'
                                //color: 'rgb(158,154,200)'
                                color: 'rgb(188,189,220)'
                                //color: 'rgb(218,218,235)'
                                //color: 'rgb(239,237,245)'
                                //color: 'rgb(252,251,253)'
                            },
                            data: numTop[7]
                        },
                        {
                            symbol: 'none',
                            name: '组2[0.4,0.5)',
                            type: 'line',
                            stack: '总量',
                            lineStyle:{
                                width: 0,
                            },
                            areaStyle: {
                                //color: 'rgb(0,0,125)'
                                //color: 'rgb(63,0,125)'
                                //color: 'rgb(84,39,143)'
                                //color: 'rgb(106,81,163)'
                                //color: 'rgb(128,125,186)'
                                color: 'rgb(158,154,200)'
                                //color: 'rgb(188,189,220)'
                                //color: 'rgb(218,218,235)'
                                //color: 'rgb(239,237,245)'
                                //color: 'rgb(252,251,253)'
                            },
                            data: numTop[6]
                        },
                        {
                            symbol: 'none',
                            name: '组2[0.5,0.6)',
                            type: 'line',
                            stack: '总量',
                            lineStyle:{
                                width: 0,
                            },
                            areaStyle: {
                                //color: 'rgb(0,0,125)'
                                //color: 'rgb(63,0,125)'
                                //color: 'rgb(84,39,143)'
                                //color: 'rgb(106,81,163)'
                                color: 'rgb(128,125,186)'
                                //color: 'rgb(158,154,200)'
                                //color: 'rgb(188,189,220)'
                                //color: 'rgb(218,218,235)'
                                //color: 'rgb(239,237,245)'
                                //color: 'rgb(252,251,253)'
                            },
                            data: numTop[5]
                        },
                        {
                            symbol: 'none',
                            name: '组2[0.6,0.7)',
                            type: 'line',
                            stack: '总量',
                            lineStyle:{
                                width: 0,
                            },
                            areaStyle: {
                                //color: 'rgb(0,0,125)'
                                //color: 'rgb(63,0,125)'
                                //color: 'rgb(84,39,143)'
                                color: 'rgb(106,81,163)'
                                //color: 'rgb(128,125,186)'
                                //color: 'rgb(158,154,200)'
                                //color: 'rgb(188,189,220)'
                                //color: 'rgb(218,218,235)'
                                //color: 'rgb(239,237,245)'
                                //color: 'rgb(252,251,253)'
                            },
                            data: numTop[4]
                        },
                        {
                            symbol: 'none',
                            name: '组2[0.7,0.8)',
                            type: 'line',
                            stack: '总量',
                            lineStyle:{
                                width: 0,
                            },
                            areaStyle: {
                                //color: 'rgb(0,0,125)'
                                //color: 'rgb(63,0,125)'
                                //color: 'rgb(84,39,143)'
                                color: 'rgb(94,60,153)'
                                //color: 'rgb(106,81,163)'
                                //color: 'rgb(128,125,186)'
                                //color: 'rgb(158,154,200)'
                                //color: 'rgb(188,189,220)'
                                //color: 'rgb(218,218,235)'
                                //color: 'rgb(239,237,245)'
                                //color: 'rgb(252,251,253)'
                            },
                            data: numTop[3]
                        },
                        {
                            symbol: 'none',
                            name: '组2[0.8,0.9)',
                            type: 'line',
                            stack: '总量',
                            lineStyle:{
                                width: 0,
                            },
                            label: {
                                normal: {
                                    show: false,
                                    position: 'top'
                                }
                            },
                            areaStyle: {
                                //color: 'rgb(0,0,125)'
                                //color: 'rgb(63,0,125)'
                                color: 'rgb(84,39,143)'
                                //color: 'rgb(106,81,163)'
                                //color: 'rgb(128,125,186)'
                                //color: 'rgb(158,154,200)'
                                //color: 'rgb(188,189,220)'
                                //color: 'rgb(218,218,235)'
                                //color: 'rgb(239,237,245)'
                                //color: 'rgb(252,251,253)'
                                //color: 'rgb(255,255,255)'
                            },
                            data: numTop[2]
                        },
                        {
                            symbol: 'none',
                            name: '组2[0.9,1.0)',
                            type: 'line',
                            stack: '总量',
                            lineStyle:{
                                width: 0,
                            },
                            label: {
                                normal: {
                                    show: false,
                                    position: 'top'
                                }
                            },
                            areaStyle: {
                                //color: 'rgb(0,0,125)'
                                color: 'rgb(63,0,125)'
                                //color: 'rgb(84,39,143)'
                                //color: 'rgb(106,81,163)'
                                //color: 'rgb(128,125,186)'
                                //color: 'rgb(158,154,200)'
                                //color: 'rgb(188,189,220)'
                                //color: 'rgb(218,218,235)'
                                //color: 'rgb(239,237,245)'
                                //color: 'rgb(252,251,253)'
                                //color: 'rgb(255,255,255)'
                            },
                            data: numTop[1]
                        },
                        {
                            symbol: 'none',
                            name: '组2_1',
                            type: 'line',
                            stack: '总量',
                            lineStyle:{
                                width: 0,
                            },
                            label: {
                                normal: {
                                    show: false,
                                    position: 'top'
                                }
                            },
                            areaStyle: {
                                color: 'rgb(40,0,125)'
                                //color: 'rgb(63,0,125)'
                                //color: 'rgb(84,39,143)'
                                //color: 'rgb(106,81,163)'
                                //color: 'rgb(128,125,186)'
                                //color: 'rgb(158,154,200)'
                                //color: 'rgb(188,189,220)'
                                //color: 'rgb(218,218,235)'
                                //color: 'rgb(239,237,245)'
                                //color: 'rgb(252,251,253)'
                                //color: 'rgb(255,255,255)'
                            },
                            data: numTop[0]
                        },
                        {
                            symbol: 'none',
                            name: '组1_1',
                            type: 'line',
                            stack: '总量',
                            lineStyle:{
                                width: 0,
                            },
                            areaStyle: {
                                color: 'rgb(40,0,125)'
                                //color: 'rgb(63,0,125)'
                                //color: 'rgb(84,39,143)'
                                //color: 'rgb(106,81,163)'
                                //color: 'rgb(128,125,186)'
                                //color: 'rgb(158,154,200)'
                                //color: 'rgb(188,189,220)'
                                //color: 'rgb(218,218,235)'
                                //color: 'rgb(239,237,245)'
                                //color: 'rgb(252,251,253)'
                                //color: 'rgb(255,255,255)'
                            },
                            data: numBottom[0]
                        },
                        {
                            symbol: 'none',
                            name: '组1[0.9,1)',
                            type: 'line',
                            stack: '总量',
                            lineStyle:{
                                width: 0,
                            },
                            areaStyle: {
                                //color: 'rgb(0,0,125)'
                                color: 'rgb(63,0,125)'
                                //color: 'rgb(84,39,143)'
                                //color: 'rgb(106,81,163)'
                                //color: 'rgb(128,125,186)'
                                //color: 'rgb(158,154,200)'
                                //color: 'rgb(188,189,220)'
                                //color: 'rgb(218,218,235)'
                                //color: 'rgb(239,237,245)'
                                //color: 'rgb(252,251,253)'
                                //color: 'rgb(255,255,255)'
                            },
                            data: numBottom[1]
                        },
                        {
                            symbol: 'none',
                            name: '组1[0.8,0.9)',
                            type: 'line',
                            stack: '总量',
                            lineStyle:{
                                width: 0,
                            },
                            areaStyle: {
                                //color: 'rgb(0,0,125)'
                                //color: 'rgb(63,0,125)'
                                color: 'rgb(84,39,143)'
                                //color: 'rgb(106,81,163)'
                                //color: 'rgb(128,125,186)'
                                //color: 'rgb(158,154,200)'
                                //color: 'rgb(188,189,220)'
                                //color: 'rgb(218,218,235)'
                                //color: 'rgb(239,237,245)'
                                //color: 'rgb(252,251,253)'
                                //color: 'rgb(255,255,255)'
                            },
                            data: numBottom[2]
                        },
                        {
                            symbol: 'none',
                            name: '组1[0.7,0.8)',
                            type: 'line',
                            stack: '总量',
                            lineStyle:{
                                width: 0,
                            },
                            areaStyle: {
                                //color: 'rgb(0,0,125)'
                                //color: 'rgb(63,0,125)'
                                //color: 'rgb(84,39,143)'
                                color: 'rgb(94,60,153)'
                                //color: 'rgb(106,81,163)'
                                //color: 'rgb(128,125,186)'
                                //color: 'rgb(158,154,200)'
                                //color: 'rgb(188,189,220)'
                                //color: 'rgb(218,218,235)'
                                //color: 'rgb(239,237,245)'
                                //color: 'rgb(252,251,253)'
                            },
                            data: numBottom[3]
                        },
                        {
                            symbol: 'none',
                            name: '组1[0.6,0.7)',
                            type: 'line',
                            stack: '总量',
                            lineStyle:{
                                width: 0,
                            },
                            areaStyle: {
                                //color: 'rgb(0,0,125)'
                                //color: 'rgb(63,0,125)'
                                //color: 'rgb(84,39,143)'
                                color: 'rgb(106,81,163)'
                                //color: 'rgb(128,125,186)'
                                //color: 'rgb(158,154,200)'
                                //color: 'rgb(188,189,220)'
                                //color: 'rgb(218,218,235)'
                                //color: 'rgb(239,237,245)'
                                //color: 'rgb(252,251,253)'
                            },
                            data: numBottom[4]
                        },
                        
                        {
                            symbol: 'none',
                            name: '组1[0.5,0.6)',
                            type: 'line',
                            stack: '总量',
                            lineStyle:{
                                width: 0,
                            },
                            areaStyle: {
                                //color: 'rgb(0,0,125)'
                                //color: 'rgb(63,0,125)'
                                //color: 'rgb(84,39,143)'
                                //color: 'rgb(106,81,163)'
                                color: 'rgb(128,125,186)'
                                //color: 'rgb(158,154,200)'
                                //color: 'rgb(188,189,220)'
                                //color: 'rgb(218,218,235)'
                                //color: 'rgb(239,237,245)'
                                //color: 'rgb(252,251,253)'
                            },
                            data: numBottom[5]
                        },
                        {
                            symbol: 'none',
                            name: '组1[0.4,0.5)',
                            type: 'line',
                            stack: '总量',
                            lineStyle:{
                                width: 0,
                            },
                            areaStyle: {
                                //color: 'rgb(0,0,125)'
                                //color: 'rgb(63,0,125)'
                                //color: 'rgb(84,39,143)'
                                //color: 'rgb(106,81,163)'
                                //color: 'rgb(128,125,186)'
                                color: 'rgb(158,154,200)'
                                //color: 'rgb(188,189,220)'
                                //color: 'rgb(218,218,235)'
                                //color: 'rgb(239,237,245)'
                                //color: 'rgb(252,251,253)'
                            },
                            data: numBottom[6]
                        },
                        {
                            symbol: 'none',
                            name: '组1[0.3,0.4)',
                            type: 'line',
                            stack: '总量',
                            lineStyle:{
                                width: 0,
                            },
                            areaStyle: {
                                //color: 'rgb(0,0,125)'
                                //color: 'rgb(63,0,125)'
                                //color: 'rgb(84,39,143)'
                                //color: 'rgb(106,81,163)'
                                //color: 'rgb(128,125,186)'
                                //color: 'rgb(158,154,200)'
                                color: 'rgb(188,189,220)'
                                //color: 'rgb(218,218,235)'
                                //color: 'rgb(239,237,245)'
                                //color: 'rgb(252,251,253)'
                            },
                            data: numBottom[7]
                        },
                        {
                            symbol: 'none',
                            name: '组1[0.2,0.3)',
                            type: 'line',
                            stack: '总量',
                            lineStyle:{
                                width: 0,
                            },
                            areaStyle: {
                                //color: 'rgb(0,0,125)'
                                //color: 'rgb(63,0,125)'
                                //color: 'rgb(84,39,143)'
                                //color: 'rgb(106,81,163)'
                                //color: 'rgb(128,125,186)'
                                //color: 'rgb(158,154,200)'
                                //color: 'rgb(188,189,220)'
                                color: 'rgb(218,218,235)'
                                //color: 'rgb(239,237,245)'
                                //color: 'rgb(252,251,253)'
                            },
                            data: numBottom[8]
                        },
                        {
                            symbol: 'none',
                            name: '组1[0.1,0.4)',
                            type: 'line',
                            stack: '总量',
                            lineStyle:{
                                width: 0,
                            },
                            areaStyle: {
                                //color: 'rgb(0,0,125)'
                                //color: 'rgb(63,0,125)'
                                //color: 'rgb(84,39,143)'
                                //color: 'rgb(106,81,163)'
                                //color: 'rgb(128,125,186)'
                                //color: 'rgb(158,154,200)'
                                //color: 'rgb(188,189,220)'
                                //color: 'rgb(218,218,235)'
                                color: 'rgb(239,237,245)'
                                //color: 'rgb(252,251,253)'
                            },
                            data: numBottom[9]
                        },
                        {
                            symbol: 'none',
                            name: '组1(0.0,0.1)',
                            type: 'line',
                            stack: '总量',
                            lineStyle:{
                                width: 0,
                            },
                            areaStyle: {
                                //color: 'rgb(0,0,125)'
                                //color: 'rgb(63,0,125)'
                                //color: 'rgb(84,39,143)'
                                //color: 'rgb(106,81,163)'
                                //color: 'rgb(128,125,186)'
                                //color: 'rgb(158,154,200)'
                                //color: 'rgb(188,189,220)'
                                //color: 'rgb(218,218,235)'
                                //color: 'rgb(239,237,245)'
                                color: 'rgb(244,244,250)'
                            },
                            data: numBottom[10]
                        },
                        {
                            symbol: 'none',
                            name: '组1_0',
                            type: 'line',
                            stack: '总量',
                            lineStyle:{
                                width: 0,
                            },
                            areaStyle: {
                                //color: 'rgb(0,0,125)'
                                //color: 'rgb(63,0,125)'
                                //color: 'rgb(84,39,143)'
                                //color: 'rgb(106,81,163)'
                                //color: 'rgb(128,125,186)'
                                //color: 'rgb(158,154,200)'
                                //color: 'rgb(188,189,220)'
                                //color: 'rgb(218,218,235)'
                                //color: 'rgb(239,237,245)'
                                color: 'rgb(252,251,253)'
                            },
                            data: numBottom[11]
                        },
                        {
                            name: 'Up line',
                            type: 'line',
                            //step: 'end',
                            data: lineDataTop,
                            symbol: 'none',
                            itemStyle: {
                                normal: { 
                                    lineStyle: { type: 'solid', color:'red', width: 0.3},
                                    label: { show: false, position:'left' }
                                }
                            },
                        },
                        {
                            name: 'Down line',
                            type: 'line',
                            //step: 'end',
                            data: lineDataBottom,
                            symbol: 'none',
                            itemStyle: {
                                normal: { 
                                    lineStyle: { type: 'solid', color:'red', width:0.3},
                                    label: { show: false, position:'left' }
                                }
                            },
                        },
                        {
                            name: 'xAxis',
                            type: 'line',
                            //z: 999,
                            markLine:{
                                symbol: ['none', 'none'],//去掉箭头
                                itemStyle: {
                                    normal: { 
                                        lineStyle: { type: 'solid', width: 3, color:'white'},
                                        label: { show: false, position:'left' }
                                    }
                                },
                                data: [{
                                    name: 'Y 轴值为 0 的水平线',
                                    //yAxis: 0
                                    yAxis: halfSubjects
                                }]
                            },
                        },
                        {
                            name: 'valid',
                            type: 'line',
                            // z:999,
                            data: [
                                [validSubjects, lineDataTop[validSubjects]],
                                [validSubjects, lineDataBottom[validSubjects]]
                            ],
                            smooth: true,
                            showSymbol: false,
                            itemStyle: {
                                normal: {
                                    lineStyle: { type: 'solid', width: 3, color:'green'},
                                    label: { show: false, position:'left' }
                                }
                            }
                        },

                        
                    ]
                };
            },
        },
        watch: {
            validThreshold: {
                deep: true,
                handler: function(newVal, oldVal) {
                    if(newVal){
                        console.log("validThreshold change: " + newVal);
                        this.$store.commit('set_pValueThreshold', newVal);
                        if(this.sumModel){
                            this.setOptionSumLine(this.pValueDataUp, this.pValueDataBottom, 0);
                            this.setOptionSumLine(this.pValueDataUp, this.pValueDataBottom, 1);
                            this.setOptionSumLine(this.pValueDataUp, this.pValueDataBottom, 2);
                            this.setOptionSumLine(this.pValueDataUp, this.pValueDataBottom, 3);
                            this.setOptionSumLine(this.pValueDataUp, this.pValueDataBottom, 4);
                        } else {
                            this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 0);
                            this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 1);
                            this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 2);
                            this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 3);
                            this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 4);   
                        }
                        NetService.update2D(this.pValueThreshold, this.validThreshold, this.testVal, this.lambda, (res, err) => {
                            if(err){
                                console.error(err);
                                return;
                            }
                            if(!res.data){
                                console.log('data not exists!');
                                return;
                            }
                            console.log('update2D_test', res.data);
                            //{"code":0,"curvature":{"groupOneData":[508.4],"groupTwoData":[508.4],"pvalue":[508.4],"average":[508.4],"variance":[508.4],"totalPeople":[524.7857143]},"entropy":{"groupOneData":[508.4],"groupTwoData":[508.4],"pvalue":[508.4],"average":[508.4],"variance":[508.4],"totalPeople":[524.7857143]},"torsion":{"groupOneData":[508.4],"groupTwoData":[508.4],"pvalue":[508.4],"average":[508.4],"variance":[508.4],"totalPeople":[524.7857143]},"strength":{"groupOneData":[508.4],"groupTwoData":[508.4],"pvalue":[508.4],"average":[508.4],"variance":[508.4],"totalPeople":[524.7857143]},"length":{"groupOneData":[508.4],"groupTwoData":[508.4],"pvalue":[508.4],"average":[508.4],"variance":[508.4],"totalPeople":[524.7857143]},"message":"获取成功"}
                            this.$store.commit('set_features_data', [res.data.curvature, res.data.entropy, res.data.torsion,res.data.strength, res.data.length]);
                            this.$store.commit('set_features_data_data', this.linksVal);
                        });

                    }
                }
            },
            pValueThreshold: {
                deep: true,
                handler: function(newVal, oldVal) {
                    if(newVal){
                        console.log("pValueThreshold change: " + newVal);
                        this.$store.commit('set_validThreshold', newVal);
                        if(this.sumModel){
                            this.setOptionSumLine(this.pValueDataUp, this.pValueDataBottom, 0);
                            this.setOptionSumLine(this.pValueDataUp, this.pValueDataBottom, 1);
                            this.setOptionSumLine(this.pValueDataUp, this.pValueDataBottom, 2);
                            this.setOptionSumLine(this.pValueDataUp, this.pValueDataBottom, 3);
                            this.setOptionSumLine(this.pValueDataUp, this.pValueDataBottom, 4);
                        } else {
                            this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 0);
                            this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 1);
                            this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 2);
                            this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 3);
                            this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 4);   
                        }
                        NetService.update2D(this.pValueThreshold, this.validThreshold, this.testVal, this.lambda, (res, err) => {
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
                        });
                    }
                }
            },
            sumModel: {
                deep: true,
                handler: function(newVal, oldVal) {
                        console.log('sumModelChange:',newVal);
                        if(this.sumModel){
                            this.setOptionSumLine(this.pValueDataUp, this.pValueDataBottom, 0);
                            this.setOptionSumLine(this.pValueDataUp, this.pValueDataBottom, 1);
                            this.setOptionSumLine(this.pValueDataUp, this.pValueDataBottom, 2);
                            this.setOptionSumLine(this.pValueDataUp, this.pValueDataBottom, 3);
                            this.setOptionSumLine(this.pValueDataUp, this.pValueDataBottom, 4);
                        } else {
                            this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 0);
                            this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 1);
                            this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 2);
                            this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 3);
                            this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 4);   
                        }
                }
            },
            values: {
                deep: true,
                handler: function(newVal, oldVal) {
                    this.$store.commit('set_feature_values', newVal);
                }
            },
            selected_subjects: {
                deep: true,
                handler: function(newVal, oldVal) {
                    if(newVal.length == 2){
                        
                        NetService.getPValueData(this.selected_subjects_name, (res, err) => {
                            if(err){
                                console.error(err);
                                return;
                            }
                            if(!res.data){
                                console.log('data not exists!');
                                return;
                            }
                            console.log('getPValueData');
                            //{"code":0,"dataGroupOne":[[[0.1,0.2,0.3,0.4,0.5,"a"],[0.2,0.4,0.5,0.3,0.4,"b"],[0.1,0.4,0.7,0.4,0.5,"c"]],[[0.2,0.4,0.4,0.5,0.3,"d"],[0.4,0.3,0.1,0.4,0.5,"e"],[0.4,0.5,0.5,0.7,0.3,"f"]],[[0.4,0.5,0.2,0.4,0.2,"h"],[0.4,0.5,0.4,0.3,0.7,"i"],[0.4,0.5,0.5,0.7,0.7,"j"]]],"dataGroupTwo":[[[0.2,0.4,0.1,0.2,0.3,"a"],[0.2,0.3,0.2,0.4,0.4,"b"],[0.1,0.4,0.7,0.2,0.4,"c"]],[[0.2,0.4,0.3,0.2,0.4,"d"],[0.4,0.3,0.1,0.2,0.4,"e"],[0.5,0.7,0.3,0.2,0.4,"f"]],[[0.2,0.4,0.2,0.4,0.2,"h"],[0.4,0.3,0.2,0.4,0.7,"i"],[0.5,0.7,0.7,0.2,0.4,"j"]],[[0.2,0.4,0.7,0.2,0.4,"k"],[0.4,0.3,0.2,0.4,0.7,"l"],[0.5,0.7,0.7,0.2,0.4,"m"]]],"subjectsArr":["all-healthy-female","all-healthy-male"],"message":"\u83b7\u53d6\u6210\u529f"}
                            var up = res.data.dataGroupOne;
                            var down = res.data.dataGroupTwo;
                            //转置
                            this.pValueDataUp = up[0].map(function(col, i) {
                                return up.map(function(row) {
                                    return row[i];
                                })
                            });
                            this.pValueDataBottom = down[0].map(function(col, i) {
                                return down.map(function(row) {
                                    return row[i];
                                })
                            });
                            if(this.sumModel){
                                this.setOptionSumLine(this.pValueDataUp, this.pValueDataBottom, 0);
                                this.setOptionSumLine(this.pValueDataUp, this.pValueDataBottom, 1);
                                this.setOptionSumLine(this.pValueDataUp, this.pValueDataBottom, 2);
                                this.setOptionSumLine(this.pValueDataUp, this.pValueDataBottom, 3);
                                this.setOptionSumLine(this.pValueDataUp, this.pValueDataBottom, 4);
                            } else {
                                this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 0);
                                this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 1);
                                this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 2);
                                this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 3);
                                this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 4);   
                            }

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
                                //{"code":0,"curvature":{"groupOneData":[508.4],"groupTwoData":[508.4],"pvalue":[508.4],"average":[508.4],"variance":[508.4],"totalPeople":[524.7857143]},"entropy":{"groupOneData":[508.4],"groupTwoData":[508.4],"pvalue":[508.4],"average":[508.4],"variance":[508.4],"totalPeople":[524.7857143]},"torsion":{"groupOneData":[508.4],"groupTwoData":[508.4],"pvalue":[508.4],"average":[508.4],"variance":[508.4],"totalPeople":[524.7857143]},"strength":{"groupOneData":[508.4],"groupTwoData":[508.4],"pvalue":[508.4],"average":[508.4],"variance":[508.4],"totalPeople":[524.7857143]},"length":{"groupOneData":[508.4],"groupTwoData":[508.4],"pvalue":[508.4],"average":[508.4],"variance":[508.4],"totalPeople":[524.7857143]},"message":"获取成功"}
                                this.$store.commit('set_features_data', [res.data.curvature, res.data.entropy, res.data.torsion,res.data.strength, res.data.length]);
                                this.$store.commit('set_features_data_data', this.linksVal);
                            });
                        });
                        
                        NetService.update2D(this.pValueThreshold, this.validThreshold, this.testVal, this.lambda, (res, err) => {
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
                        });
                        /*
                        var up =[];
                        var down = [];
                        for(var i = 0; i < 100; i++) {
                            var tmp = [];
                            for(var j = 0; j < 1000; j++) {
                                tmp.push([Math.random(),Math.random(),Math.random(),Math.random(),Math.random(),i+'-'+'j']);
                            }
                            up.push(tmp);
                        }
                        for(var i = 0; i < 200; i++) {
                            var tmp = [];
                            for(var j = 0; j < 1000; j++) {
                                tmp.push([Math.random(),Math.random(),Math.random(),Math.random(),Math.random(),i+'-'+'j']);
                            }
                            down.push(tmp);
                        }
                        //转置
                        this.pValueDataUp = up[0].map(function(col, i) {
                            return up.map(function(row) {
                                return row[i];
                            })
                        });
                        this.pValueDataBottom = down[0].map(function(col, i) {
                            return down.map(function(row) {
                                return row[i];
                            })
                        });
                        this.setOptionSum(this.pValueDataUp, this.pValueDataBottom, 0);
                        this.setOptionSum(this.pValueDataUp, this.pValueDataBottom, 1);
                        this.setOptionSum(this.pValueDataUp, this.pValueDataBottom, 2);
                        this.setOptionSum(this.pValueDataUp, this.pValueDataBottom, 3);
                        this.setOptionSum(this.pValueDataUp, this.pValueDataBottom, 4);
                        */
                    }
                }
            }
        }
    }
</script>
