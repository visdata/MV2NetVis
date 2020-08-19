<template>
    <div>
    <h5>Subject View</h5>
    <!--选择性别还是是否患病-->
    <div :style="{'margin-top': '0px', 'position':'absolute', 'padding-left': '70%', 'z-index': '99'}">
        <ul class="ui-controlgroup ui-listview" :style="{'list-style-type':'none', 'margin':'1px', 'padding':'0px', 'text-align':'left'}">
            <li v-for="(item, index) in pro_selections_items" :key="item.index">
                <span :style="{'display':'inline-block', 'width':'13px','height':'13px', 'display':'inline-block', 'outline':'1px solid black', 'text-align':'center', 'font-size':'12px', 'font-weight':'bolder', 'font':'small-caption'}">{{ item.order }}</span>
                <input type="checkbox" @click="multipleSelect(item, item.checkval)" :checked="item.checkval" />
                <span> {{ item.message }} </span>
            </li>
        </ul>
        <button @click="reload_properties()" :style="{'width': '60px'}">
            reload
        </button>
    </div>
    
    <!--显示选择的subjects-->
    <!--
    <div :style="{'text-align': 'right', 'float': 'right', 'min-width':'200px'}">
       
        <span>选择的subjects:</span>
        <div :style="{'min-height': '48px'}">
            <ul :style="{'margin':'0', 'padding':'0','list-style-position': 'inside'}">
                <li v-for="item in selected_subjects">{{item}}</li>
            </ul>
        </div>
        <div v-bind:style="{'visibility' : subjects_valid ? 'hidden' : 'visible'}">多属性不一致</div>
        <div :style="{'text-align': 'right'}">
            <button @click="reload_properties()" :style="{'width': '60px'}">
                reload
            </button>
        </div>
    </div>
    -->
    <!--第一列上sunburst-->
    <sunburst-two :data="sunburstData" :show='true' :style="{height: '350px'}" >
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
    <hr style="height:1px;border:none;border-top:1px solid #555555;padding: 0; margin: 7px; margin-top: -22px;" />
    <h5>Feature View</h5>
    <div :style="{'min-height': '56px'}">
        <div :style="{'display':'inline', 'width':'150px', 'float':'left'}">
        <div :style="{'padding-bottom':'10px'}">pvalue threshold(%):</div>
        <!--slider :min=0 :max=100 v-model="pValueThreshold" :style="{'width': '200px'}"></slider-->
        <Slider 
                v-model='pValueThreshold'
                :showTip="true"
                :rule="[0, '|', 25, '|',50, '|', 75, '|', 100]">
        </Slider>
        </div>
        <div :style="{'display':'inline', 'width':'180px', 'float':'left','padding-left':'30px'}">
        <div :style="{'padding-bottom':'10px'}">valid threshold:</div>
        <!--slider :min=0 :max=100 v-model="validThreshold" :style="{'width': '200px'}"></slider-->
        <Slider 
                v-model='validThreshold'
                :showTip="true"
                :rule="[0, '|', 25, '|',50, '|', 75, '|', 100]">
        </Slider>
        </div>
    </div>
    <!--第一列下热力图-->
    <div :style="{'height': '260px', 'width': '300px', 'padding-top':'30px'}">
        <happy-scroll color="rgba(0,0,0,0.5)" size="5">
            <div class="featureView" :style="{'text-align': 'left'}" >
                <div v-for="feature in features" :key="feature.index" style="margin-bottom:10px">
                    <CheckBox :value="feature.name" :multiple="true" v-model="values"></CheckBox>
                    <Label>{{feature.name}}</Label>
                    <chart :options="feature.options" :auto-resize='false' :style="{width: '300px', height: '250px', margin: 'auto'}"></chart>
                </div>
                <p v-if="values">You selected: {{values}}</p>
                <!--
                <div :style="{'padding-bottom':'5px'}">curvature:</div>
                <chart :options='curvatureOptions' :auto-resize='false' :style="{width: '300px', height: '250px', margin: 'auto'}"></chart>
                <div :style="{'padding-bottom':'5px'}">entropy:</div>
                <chart :options='entropyOptions' :auto-resize='false' :style="{width: '300px', height: '250px', margin: 'auto'}"></chart>
                <div :style="{'padding-bottom':'5px'}">torsion:</div>
                <chart :options='torsionOptions' :auto-resize='false' :style="{width: '300px', height: '250px', margin: 'auto'}"></chart>
                <div :style="{'padding-bottom':'5px'}">strength:</div>
                <chart :options='strengthOptions' :auto-resize='false' :style="{width: '300px', height: '250px', margin: 'auto'}"></chart>
                <div :style="{'padding-bottom':'5px'}">length:</div>
                <chart :options='lengthOptions' :auto-resize='false' :style="{width: '300px', height: '250px', margin: 'auto'}"></chart>
                -->
            </div>
        </happy-scroll>
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
        breadcrumbTrail,
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
    import breadcrumbTrailTwo from './breadcrumbTrailTwo'
    import sunburstTwo from './sunburstTwo'

    export default {
        name: "echartsScatterVersionTwo",
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
            breadcrumbTrailTwo,
            sunburstTwo
        },
        data () {
            return {
                sunburstData: {
                    "name": "all",
                    "size": 0
                },
                pro_selections_items: [
                    {
                        message: 'Sicken',
                        checkval: false,
                        order: '',
                    },
                    {
                        message: 'Gender',
                        checkval: false,
                        order: '',
                    },
                    {
                        message: 'Ages',
                        checkval: false,
                        order: '',
                    },
                    {
                        message: 'Others',
                        checkval: false,
                        order: '',
                    }
                ],
                handleArr: [], //选中数组
                handleNum: 0, //选中的顺序
                /*
                curvatureOptions: {},
                entropyOptions: {},
                torsionOptions: {},
                strengthOptions: {},
                lengthOptions: {},
                */
                pValueThreshold: 60, //0~100
                validThreshold: 50, //0~100满足p-threshold的占比
                maxSubjectLength: 10, //长的那组人的长度
                
                pValueDataUp: [],
                pValueDataBottom: [],

                values: [],
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
                }]
            }
        },
        created() {
            //this.getPointData()
        },
        //mounted在html渲染出来后进行操作
        mounted() {
            //this.pValueDataUp = [[[0.1,0.2,0.3,'a'],[0.2,0.3,0.4,'b'],[0.1,0.4,0.7,'c']],[[0.2,0.4,0.3,'d'],[0.4,0.3,0.1,'e'],[0.5,0.7,0.3,'f']],[[0.2,0.4,0.2,'h'],[0.4,0.3,0.7,'i'],[0.5,0.7,0.7,'j']]];
            //this.pValueDataBottom = [[[0.1,0.2,0.3,'a'],[0.2,0.3,0.4,'b'],[0.1,0.4,0.7,'c']],[[0.2,0.4,0.3,'d'],[0.4,0.3,0.1,'e'],[0.5,0.7,0.3,'f']],[[0.2,0.4,0.2,'h'],[0.4,0.3,0.7,'i'],[0.5,0.7,0.7,'j']],[[0.2,0.4,0.7,'k'],[0.4,0.3,0.7,'l'],[0.5,0.7,0.7,'m']]];
            
            for(var i = 0; i < 10; i++) {
                var tmp = [];
                for(var j = 0; j < 10; j++) {
                    tmp.push([Math.random(),Math.random(),Math.random(),Math.random(),Math.random(),i+'-'+'j']);
                }
                this.pValueDataUp.push(tmp);
            }
            for(var i = 0; i < 15; i++) {
                var tmp = [];
                for(var j = 0; j < 10; j++) {
                    tmp.push([Math.random(),Math.random(),Math.random(),Math.random(),Math.random(),i+'-'+'j']);
                }
                this.pValueDataBottom.push(tmp);
            }
            this.setOption(this.pValueDataUp, this.pValueDataBottom, 0);
            this.setOption(this.pValueDataUp, this.pValueDataBottom, 1);
            this.setOption(this.pValueDataUp, this.pValueDataBottom, 2);
            this.setOption(this.pValueDataUp, this.pValueDataBottom, 3);
            this.setOption(this.pValueDataUp, this.pValueDataBottom, 4);
            /*
            this.setCurvatureOption(this.pValueDataUp, this.pValueDataBottom, 0);
            this.setEntropyOption(this.pValueDataUp, this.pValueDataBottom, 1);
            this.setTorsionOption(this.pValueDataUp, this.pValueDataBottom, 2);
            this.setStrengthOption(this.pValueDataUp, this.pValueDataBottom, 3);
            this.setLengthOption(this.pValueDataUp, this.pValueDataBottom, 4);
            */
        },
        computed: {
            ...mapState(['subjects_valid', 'selected_subjects']),
        },
        methods: {
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
                    this.sunburstData = res.data.data;
                });
            },
            reload_properties(){
                this.sunburstData = {
                    "name": "all",
                    "size": 0
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
            setLengthOption(pointUp, pointBottom, index){
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
                this.lengthOptions = {
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
            setCurvatureOption(pointUp, pointBottom, index){
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
                this.curvatureOptions = {
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
            setEntropyOption(pointUp, pointBottom, index){
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
                this.entropyOptions = {
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
            setTorsionOption(pointUp, pointBottom, index){
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
                this.torsionOptions = {
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
            setStrengthOption(pointUp, pointBottom, index){
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
                this.strengthOptions = {
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
        },
        watch: {
            validThreshold: {
                deep: true,
                handler: function(newVal, oldVal) {
                    if(newVal){
                        console.log("validThreshold change: " + newVal);
                        this.setOption(this.pValueDataUp, this.pValueDataBottom, 0);
                        this.setOption(this.pValueDataUp, this.pValueDataBottom, 1);
                        this.setOption(this.pValueDataUp, this.pValueDataBottom, 2);
                        this.setOption(this.pValueDataUp, this.pValueDataBottom, 3);
                        this.setOption(this.pValueDataUp, this.pValueDataBottom, 4);
                        /*
                        this.setCurvatureOption(this.pValueDataUp, this.pValueDataBottom, 0);
                        this.setEntropyOption(this.pValueDataUp, this.pValueDataBottom, 1);
                        this.setTorsionOption(this.pValueDataUp, this.pValueDataBottom, 2);
                        this.setStrengthOption(this.pValueDataUp, this.pValueDataBottom, 3);
                        this.setLengthOption(this.pValueDataUp, this.pValueDataBottom, 4);
                        */
                    }
                }
            },
            pValueThreshold: {
                deep: true,
                handler: function(newVal, oldVal) {
                    if(newVal){
                        console.log("pValueThreshold change: " + newVal);
                        this.setOption(this.pValueDataUp, this.pValueDataBottom, 0);
                        this.setOption(this.pValueDataUp, this.pValueDataBottom, 1);
                        this.setOption(this.pValueDataUp, this.pValueDataBottom, 2);
                        this.setOption(this.pValueDataUp, this.pValueDataBottom, 3);
                        this.setOption(this.pValueDataUp, this.pValueDataBottom, 4);
                        /*
                        this.setCurvatureOption(this.pValueDataUp, this.pValueDataBottom, 0);
                        this.setEntropyOption(this.pValueDataUp, this.pValueDataBottom, 1);
                        this.setTorsionOption(this.pValueDataUp, this.pValueDataBottom, 2);
                        this.setStrengthOption(this.pValueDataUp, this.pValueDataBottom, 3);
                        this.setLengthOption(this.pValueDataUp, this.pValueDataBottom, 4);
                        */
                    }
                }
            },
        }
    }
</script>
