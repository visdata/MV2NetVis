<template>
    <div>
    <!--选择性别还是是否患病-->
    <div :style="{'margin-top': '-20px', 'min-height': '70px'}">
        <ui-radio-group name="model_selection" :options="model_options" v-model="model_selection">
            Choose Model
        </ui-radio-group>
    </div>
    <!--第一列上sunburst-->
    <sunburst :data="model_selection==='sicken'? dataSicken: dataGender" :style="{width: '200px', height: '200px', margin: 'auto'}">
        <!-- Add behaviors -->
        <template slot-scope='{ on, actions }'>
            <highlightOnHover v-bind='{ on, actions }'/>
            <!--zoomOnClick v-bind='{ on, actions }'/-->
        </template>
        <!-- 信息 -->
        <nodeInfoDisplayer slot='top' slot-scope='{ nodes }' :current='nodes.mouseOver' :root='nodes.root' description='of visits begin with this sequence of pages' />
        <!-- 层次 -->
        <breadcrumbTrail slot='legend' slot-scope='{ nodes, colorGetter, width }' :current='nodes.mouseOver' :root='nodes.root' :colorGetter='colorGetter' :from='nodes.clicked' :width='width' />
    </sunburst>
    <!--第一列下散点图-->
    <div>
        <chart id="myChart" ref="myScatter" :options='options' :auto-resize='true' @click='clickThisPoint' :style="{width: '300px', height: '300px', margin: 'auto'}"></chart>
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
    import {UiRadioGroup} from 'keen-ui'
    import NetService from '@/services/net-service'

    export default {
        name: "echartsScatter",
        components: {
            breadcrumbTrail,
            highlightOnHover,
            nodeInfoDisplayer,
            sunburst,
            zoomOnClick,
            UiRadioGroup
        },
        data () {
            return {
                dataGender,
                dataSicken,
                points: [
                ['balance','subjects','pvalue','featurename'],
                [10.0, 8.0, 0.1, 'a'],
                [11.0, 12.0, 3.0, 'b'],
                [3.0, 3.0, 2.9, 'c'],
                [4.0, 1.0, 1.4, 'd'],
                [5.0, 7.0, 4.3, 'e'],
                [6.0, 2.0, 0.9, 'f'],
                [7.0, 4.0, 2.0, 'g'],
                [8.0, 5.9, 1.9, 'h']
                ],
                options: {},
                model_selection: 'sicken',
                model_options: [
                    {
                        label: 'Sicken',
                        value: 'sicken'
                    },
                    {
                        label: 'Gender',
                        value: 'gender'
                    }
                ],
            }
        },
        created() {
            //this.getPointData()
        },
        //mounted在html渲染出来后进行操作
        mounted() {
            //console.log('mounted'),
            //this.setOption(this.points)
            this.getPointData()
        },
        methods: {
            setOption(point){
                this.options = {
                //标题
                title: {
                    left: 'center',
                    show: false
                },
                //数据
                dataset: {
                    'source': point
                },
                //映射 range在[min, max]范围
                visualMap: {
                    min: 0,
                    max: 5,
                    range: [0, 5],
                    type: 'continuous',
                    show: true,
                    dimension: 2,
                    align: 'bottom',
                    precision: 3,
                    orient: 'horizontal',
                    bottom: 'bottom',
                    left: 'center',
                    inRange: {color: ['yellow','green', 'blue', 'red']},
                    itemHeight: 200,
                    padding: 1
                },
                //工具提示
                tooltip: {
                    showDelay: 0,
                    //鼠标移上去显示内容样式
                    formatter: function (params) {
                        if (params.value.length > 1) {
                            return params.value[3] + ' ' + params.value[2];
                        }
                    },
                    //气泡背景色
                    backgroundColor: '#fff',
                    //文字样式
                    textStyle: {
                        color: '#333'
                    },
                    //气泡边框颜色
                    borderColor: '#CBCBCB',
                    //气泡边框厚度
                    borderWidth: 1,
                    //气泡阴影效果
                    extraCssText: 'box-shadow: 0 0 3px rgba(0, 0, 0, 0.3);'
                },
                xAxis: [
                    {
                        type: 'value',
                        scale: false,
                        axisLabel: {
                            formatter: '{value}%'
                        },
                        //刻度线样式
                        axisTick:{
                            show: false
                        }
                    }
                ],
                yAxis: [
                    {
                        type: 'value',
                        scale: false,
                        axisLabel: {
                            formatter: '{value}%'
                        },
                        axisTick:{
                            show: false
                        }   
                    }
                ],
                series: [
                    {
                        type: 'scatter',
                        encode: {
                            x: 'balance',
                            y: 'subjects',
                        }
                    }
                ]
            };
            },
            //获取原始数据
            getPointData() {
                NetService.getOriginData((res, err) => {
                    if(err){
                        console.error(err);
                        return;
                    }
                    if(!res.data){
                        console.log('data not exists!');
                        return;
                    }
                    console.log('getOriginData');
                    //{"code":0,"data":[["balance","subjects","pvalue","featurename"],[5,8,0.1,"a"],[0,3,4.1,"b"],[1,4,2.1,"c"]],"femaleAD":5,"femaleHealthy":1,"maleAD":5,"maleHealthy":1,"message":"获取成功"}
                    this.points = res.data.data;
                    console.log(this.points);
                    //根据subject数据修改dataGender、dataSicken
                    var femaleAD = res.data.femaleAD;
                    var femaleHealthy = res.data.femaleHealthy;
                    var maleAD = res.data.maleAD;
                    var maleHealthy = res.data.maleHealthy;
                    console.log(this.model_selection);
                    if (this.model_selection == 'sicken') {
                        dataSicken.children[0].children[0].size = femaleHealthy;
                        dataSicken.children[0].children[1].size = maleHealthy;
                        dataSicken.children[1].children[0].size = femaleAD;
                        dataSicken.children[1].children[1].size = maleAD;
                    }
                    else if(this.model_selection == 'gender') {
                        dataGender.children[0].children[0].size = femaleHealthy;
                        dataGender.children[0].children[1].size = femaleAD;
                        dataGender.children[1].children[0].size = maleHealthy;
                        dataGender.children[1].children[1].size = maleAD;
                    }
                    this.setOption(this.points);
                });
                //this.points = [
                //    ['balance','subjects','pvalue','featurename'],
                //    [10.0, 8.0, 0.1, 'a'],
                //    [11.0, 12.0, 3.0, 'b'],
                //    [3.0, 3.0, 2.9, 'c'],
                //    [4.0, 1.0, 1.4, 'd'],
                //    [5.0, 7.0, 4.3, 'e'],
                //    [6.0, 2.0, 0.9, 'f'],
                //    [7.0, 4.0, 2.0, 'g'],
                //    [8.0, 5.9, 1.9, 'h']
                //]
                //for (var i = 0; i < 1000; i++) {
                //    this.points.push([Math.random()*12, Math.random()*12, (Math.random()*5).toFixed(3), 'a'+i]);
                //}
            },
            //点击该点
            forUpdate(event, instance, echarts){},
            clickThisPoint(event, instance, echarts) {
                var index = arguments[0].dataIndex;
                var featureName = arguments[0].data[3];
                //把该点数据交给后台，获取新的subject数据以及点数据【女病人，男病人，女健康，男健康，剩下feature的四元组】
                var that = this;
                NetService.getDataWithProAndFea(this.model_selection, featureName, (res, err) => {
                    if(err){
                        console.error(err);
                        return;
                    }
                    if(!res.data){
                        console.log('data not exists!');
                        return;
                    }
                    console.log('click point');
                    //{"code":0,"data":[["balance","subjects","pvalue","featurename"],[10,8,0.1,"a"],[1,8,4.1,"b"],[5,4,2.1,"c"]],"femaleAD":3,"femaleHealthy":3,"maleAD":3,"maleHealthy":3,"message":"获取成功"}
                    that.points = res.data.data;
                    console.log(this.points);
                    //根据subject数据修改dataGender、dataSicken
                    var femaleAD = res.data.femaleAD;
                    var femaleHealthy = res.data.femaleHealthy;
                    var maleAD = res.data.maleAD;
                    var maleHealthy = res.data.maleHealthy;
                    if (this.model_selection == 'sicken') {
                        dataSicken.children[0].children[0].size = femaleHealthy;
                        dataSicken.children[0].children[1].size = maleHealthy;
                        dataSicken.children[1].children[0].size = femaleAD;
                        dataSicken.children[1].children[1].size = maleAD;
                    }
                    else if(this.model_selection == 'gender') {
                        dataGender.children[0].children[0].size = femaleHealthy;
                        dataGender.children[0].children[1].size = femaleAD;
                        dataGender.children[1].children[0].size = maleHealthy;
                        dataGender.children[1].children[1].size = maleAD;
                    }
                    //刷新
                    this.setOption(this.points);
                });
                //console.log(dataGender.children[0].children[0].size);
                //根据点数据更新point
                //删除该点
                //delete this.points[index + 1];
                //this.points.splice(index+1, 1);
                //console.log(this.points);
                
            }   
        },
        watch: {
            //属性变化
            model_selection: {
                deep: true,
                handler: function(newVal, oldVal) {
                    if (newVal) {
                        console.log(newVal);
                        NetService.getDataWithPro(newVal, (res, err) => {
                            if(err){
                                console.error(err);
                                return;
                            }
                            if(!res.data){
                                console.log('data not exists!');
                                return;
                            }
                            console.log('selete properties');
                            //{"code":0,"data":[["balance","subjects","pvalue","featurename"],[10,8,0.1,"a"],[1,8,4.1,"b"],[5,4,2.1,"c"]],"femaleAD":4,"femaleHealthy":4,"maleAD":4,"maleHealthy":4,"message":"获取成功"}
                            this.points = res.data.data;
                            //根据subject数据修改dataGender、dataSicken
                            var femaleAD = res.data.femaleAD;
                            var femaleHealthy = res.data.femaleHealthy;
                            var maleAD = res.data.maleAD;
                            var maleHealthy = res.data.maleHealthy;
                            if (newVal == 'sicken') {
                                dataSicken.children[0].children[0].size = femaleHealthy;
                                dataSicken.children[0].children[1].size = maleHealthy;
                                dataSicken.children[1].children[0].size = femaleAD;
                                dataSicken.children[1].children[1].size = maleAD;
                            }
                            else if(newVal == 'gender') {
                                dataGender.children[0].children[0].size = femaleHealthy;
                                dataGender.children[0].children[1].size = femaleAD;
                                dataGender.children[1].children[0].size = maleHealthy;
                                dataGender.children[1].children[1].size = maleAD;
                            }
                            this.setOption(this.points);
                        });
                    }
                    else {
                        //console.log(oldVal);
                    }                   
                    //this.chart.resize();
                    console.log('change porperties');
                }
            },
            options: {
                deep: true,
                handler: function(newVal, oldVal) {
                    console.log('options change');
                    var myscatter = this.$refs.myScatter;
                    //myscatter.mergeOptions(this.options, false);
                    //myscatter.resize(this.options);
                }
            }
        }
    }
</script>
