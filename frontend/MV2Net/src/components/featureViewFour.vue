<template>
    <div style="height: 100%;">
        <div style="height: 100%;">
            <div class="mycard" style="margin-top: 10px; margin-left: 0px; margin-right: 22px; height: 100%">
                <div :style="{'padding-top': '5px', 'padding-left':'5px'}">
                    <p class="mytitle">Feature Heatmap</p>
                </div>
                <div class="featureView" :style="{'text-align': 'left', 'margin-left':'0px', 'margin-top':'-24px', 'clear':'both'}" >
                    <div v-for="(feature, fi) in features" :key="feature.index" style="margin-bottom:0px;float: left; width: 19.5%; margin-right: 5px; border-right: 1px solid rgba(0,0,0,0.05); position: relative;">
                        <div class="loading" style="margin-top: 6px;" v-show="loading[fi]"></div>
                        <div style="margin-left: 5px; position: absolute; z-index: 999; width: 100%; text-align: center; padding-top: 10px;">
                            <div>
                            <!--CheckBox :value="feature.index" :multiple="true" v-model="values"></CheckBox-->
                                <button class="mybtn" @click="duplicate(feature.index)" :style="{'z-index':'999'}">
                                    +
                                </button>
                                <Label style="">{{validLinks[fi] == 0 ? feature.name : (feature.name + ": " + validLinks[fi] + " features")}}</Label>
                                
                            </div>
                        </div>
                        <chart ref="featuresChart" :options="feature.options" :auto-resize='false' :style="{width: '100%', height: '220px', margin: 'auto', 'margin-top': '10px'}"></chart>
                    </div>
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
        name: "featureViewFour",
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
                pValueThreshold: [50, 50, 50, 50, 50], //0~100
                validThreshold: [50, 50, 50, 50, 50], //0~100满足p-threshold的占比
                maxSubjectLength: 10, //长的那组人的长度
                
                pValueDataUp: [],
                pValueDataBottom: [],
                pValueData: [new Object(), new Object(), new Object(), new Object(), new Object()], //记录处理后所有的数据
                hasLoad: false,

                pvalueName: "pvalue",
                validName: "valid",

                values: [],
                
                sumModel: true,

                colNum: 25, //多少列合并
                rowNum: 2, //多少行合并

                validLinks: [0,0,0,0,0],
                loading: {
                    0: false,
                    1: false,
                    2: false,
                    3: false,
                    4: false,
                },
                //loading: [false, false, false, false, false],


                colorMapOptions: {
                    visualMap: {
                        min: 0,
                        max: 1,
                        calculable: false,
                        orient: 'horizontal',
                        left: 'center',
                        bottom: '7%',
                        precision: 2,
                        itemHeight: 100,
                        itemWidth: 8,
                        inRange: { color: [ //灰蓝橙红
                            /*'rgba(240, 240, 240, 0.5)',
                            'rgba(0, 116, 188, 1.0)',
                            'rgba(0, 170, 226, 1.0)',
                            'rgba(68, 199, 239, 1.0)',
                            'rgba(154, 217, 238, 1.0)', 
                            'rgba(216, 236, 241, 1.0)', 
                            'rgba(242, 238, 197, 1.0)', 
                            'rgba(249, 216, 168, 1.0)',
                            'rgba(245, 177, 139, 1.0)',
                            'rgba(239, 133, 122, 1.0)',
                            'rgba(216, 82, 88, 1.0)', 
                            'rgba(175, 53, 71, 1.0)',   
                            */
                            /*
                            '#DCDCDC',
                            '#DFC6C6',
                            '#E3B0B0',
                            '#E69A9A',
                            '#EA8484',
                            '#ED6E6E',
                            '#F15858',
                            '#F44242',
                            '#F82C2C',
                            '#FB1616',
                            '#FF0000',
                            */
                            '#EFEFEF',
                            '#FF0000',

                        ]
                        },
                    }
                },
            }
        },
        created() {

        },
        //mounted在html渲染出来后进行操作
        mounted() {
            var that = this;
            setInterval(function(){
                //console.log(that.rowNum);
                if (that.hasLoad){
                    var myCharts = that.$refs.featuresChart;
                    //console.log('myCharts', myCharts);
                    for(var i in myCharts) {
                        var data = that.pValueData[i].validData;
                        //valid
                        var graphic1 = echarts.util.map(data, function (item, dataIndex) {
                            function onPointDragging(featureIndex, dataIndex, dx, dy) {
                                var myCharts = that.$refs.featuresChart;
                                var i = featureIndex;
                                that.pValueData[i].validSubjects = myCharts[i].convertFromPixel({xAxisIndex: 1, yAxisIndex: 1}, this.position)[0];
                                if(that.pValueData[i].validSubjects < 0) that.pValueData[i].validSubjects = 0;
                                if(that.pValueData[i].validSubjects >= that.pValueData[i].tmp.length) that.pValueData[i].validSubjects = that.pValueData[i].tmp.length - 1;
                                //计算上下限
                                that.pValueData[i].validData = [
                                    [that.pValueData[i].validSubjects, that.pValueData[i].lineDataTop[that.pValueData[i].validSubjects]],
                                    [that.pValueData[i].validSubjects, that.pValueData[i].lineDataBottom[that.pValueData[i].validSubjects]]
                                ];
                                //计算valid
                                var validData = that.pValueData[i].validData;
                                var totalSubjects = that.pValueData[i].upSubjectslen + that.pValueData[i].downSubjectslen;
                                var validSubjects = validData[0][1] - validData[1][1];
                                var validThreshold = validSubjects / totalSubjects * 100;
                                //实线和虚线
                                var lineDataTopSolid = that.deepCopy(that.pValueData[i].lineDataTop);
                                var lineDataTopDotted = that.deepCopy(that.pValueData[i].lineDataTop);
                                var lineDataBottomSolid = that.deepCopy(that.pValueData[i].lineDataBottom);
                                var lineDataBottomDotted = that.deepCopy(that.pValueData[i].lineDataBottom);
                                for (var j in that.pValueData[i].lineDataTop) {
                                    if (j <= that.pValueData[i].validSubjects) {
                                        lineDataTopDotted[j] = "-";
                                        lineDataBottomDotted[j] = "-";
                                    }
                                    else {
                                        lineDataTopSolid[j] = "-";
                                        lineDataBottomSolid[j] = "-";
                                    }
                                }
                                myCharts[i].mergeOptions({
                                    series: [
                                    {
                                        name: 'Up line',
                                        type: 'line',
                                        step: 'end',
                                        data: lineDataTopSolid,
                                        xAxisIndex: 1,
                                        yAxisIndex: 1,
                                        symbol: 'none',
                                        itemStyle: {
                                            normal: { 
                                                lineStyle: { type: 'dotted', color:'rgb(59, 124, 244)', width: 3},
                                                label: { show: false, position:'left' }
                                            }
                                        },
                                    },
                                    {
                                        name: 'Down line',
                                        type: 'line',
                                        step: 'end',
                                        data: lineDataBottomSolid,
                                        xAxisIndex: 1,
                                        yAxisIndex: 1,
                                        symbol: 'none',
                                        itemStyle: {
                                            normal: { 
                                                lineStyle: { type: 'dotted', color:'rgb(59, 124, 244)', width: 3},
                                                label: { show: false, position:'left' }
                                            }
                                        },
                                    },                                                
                                    {
                                        name: 'valid',
                                        type: 'line',
                                        xAxisIndex: 1,
                                        yAxisIndex: 1,
                                        z:9,
                                        data: that.pValueData[i].validData,
                                        smooth: true,
                                        showSymbol: false,
                                        markPoint: {
                                            itemStyle: {
                                                color: 'rgb(59, 124, 244)', 
                                                borderWidth: 0,
                                                //shadowBlur: 10,
                                                //shadowColor: 'rgb(150, 150, 150)',
                                            },
                                            label: {
                                                color: 'white',
                                            },
                                            data: [
                                                {
                                                    name: 'validThreshold', 
                                                    value: validThreshold.toFixed(0) + '%', 
                                                    xAxis: validData[0][0], 
                                                    yAxis: validData[0][1],
                                                    symbolSize: 40,
                                                    label: {
                                                        padding: [0, -2, 0, 0],
                                                        fontSize: 10,
                                                        position: 'insideRight'
                                                    },
                                                    symbolRotate: -90,
                                                },
                                                
                                                {
                                                    name: 'validThreshold', 
                                                    value: ((that.pValueData[i].validSubjects) * 34).toFixed(0), 
                                                    xAxis: validData[0][0], 
                                                    yAxis: validData[0][1],
                                                    symbolSize: 40,
                                                    label: {
                                                        fontSize: 10,
                                                        position: 'insideLeft',
                                                    },
                                                    symbolRotate: 90,
                                                }
                                                
                                            ]
                                        },
                                        itemStyle: {
                                            normal: {
                                                lineStyle: { type: 'dotted', width: 3, color:'rgb(59, 124, 244)'},
                                                label: { show: false, position:'left' }
                                            }
                                        }
                                    }
                                    ]
                                }); 
                                }
                                function onPointDraggingEnd(valueIndex, dataIndex, dx, dy) {
                                    var myCharts = that.$refs.featuresChart;
                                    var i = valueIndex;
                                    that.loading[i] = true;
                                    that.pValueData[i].validSubjects = myCharts[i].convertFromPixel({xAxisIndex: 1, yAxisIndex: 1}, this.position)[0];
                                    if(that.pValueData[i].validSubjects < 0) that.pValueData[i].validSubjects = 0;
                                    if(that.pValueData[i].validSubjects >= that.pValueData[i].tmp.length) that.pValueData[i].validSubjects = that.pValueData[i].tmp.length - 1;
                                    that.pValueData[i].validData = [
                                    [that.pValueData[i].validSubjects, that.pValueData[i].lineDataTop[that.pValueData[i].validSubjects]],
                                    [that.pValueData[i].validSubjects, that.pValueData[i].lineDataBottom[that.pValueData[i].validSubjects]]
                                    ];
                                    //计算出validThreshold然后更新所有视图
                                    var validData = that.pValueData[valueIndex].validData;
                                    var totalSubjects = that.pValueData[valueIndex].upSubjectslen + that.pValueData[valueIndex].downSubjectslen;
                                    var validSubjects = validData[0][1] - validData[1][1];
                                    that.validThreshold[i] = validSubjects / totalSubjects * 100;

                                    console.log("validThreshold change: " + that.validThreshold[i]);
                                    that.$store.commit('set_validThreshold', that.validThreshold);
                                    if(that.sumModel){
                                        setTimeout(function(){
                                            that.setOptionSumLineThree(that.pValueDataUp, that.pValueDataBottom, i);
                                        }, 0);
                                        //that.setOptionSumLineThree(that.pValueDataUp, that.pValueDataBottom, i);
                                    } else {
                                        that.setOptionSub(that.pValueDataUp, that.pValueDataBottom, i);
                                    }
                                    /*
                                    for(var i in that.pValueData){
                                        that.setOptionSumLineThree(that.pValueDataUp, that.pValueDataBottom, i);
                                    }
                                    */
                                }
                                return {
                                    type: 'line',
                                    position: myCharts[i].convertToPixel({xAxisIndex: 1, yAxisIndex: 1}, item),
                                    shape: {
                                        x0: 0,
                                        y0: 0,
                                        x1: 0,
                                        y1: dataIndex > 0 ? (myCharts[i].convertToPixel({xAxisIndex: 1, yAxisIndex: 1}, data[0])[1] - myCharts[i].convertToPixel({xAxisIndex: 1, yAxisIndex: 1}, data[1])[1]) / 2 : (myCharts[i].convertToPixel({xAxisIndex: 1, yAxisIndex: 1}, data[1])[1] - myCharts[i].convertToPixel({xAxisIndex: 1, yAxisIndex: 1}, data[0])[1]) / 2,
                                        percent: 1,
                                    },
                                    invisible: true, //是否不可见
                                    draggable: true,
                                    cursor: "e-resize",
                                    ondrag: echarts.util.curry(onPointDragging, i, dataIndex),
                                    ondragend: echarts.util.curry(onPointDraggingEnd, i, dataIndex), //更新所有
                                    //onmousemove: echarts.util.curry(showTooltip, dataIndex),
                                    //onmouseout: echarts.util.curry(hideTooltip, dataIndex),
                                    z: 100
                                };
                            });
                        //upline
                        var data2 = [];
                        for(var j in that.pValueData[i].lineDataTop) {
                            data2[j] = [j, that.pValueData[i].lineDataTop[j]];
                        }
                        var graphic2 = echarts.util.map(data2, function (item, dataIndex) {
                                function onPointDragging(featureIndex, dataIndex, dx, dy) {
                                    var myCharts = that.$refs.featuresChart;
                                    var index = featureIndex;
                                    //获得该点的pvalue值，然后计算新的线
                                    var newPosX = Math.floor(myCharts[index].convertFromPixel({xAxisIndex: 1, yAxisIndex: 1}, this.position)[0]);
                                    if(newPosX < 0) newPosX = 0;
                                    if(newPosX >= that.pValueData[index].tmp.length) newPosX = that.pValueData[index].tmp.length - 1;
                                    var newPosY = Math.floor(myCharts[index].convertFromPixel({xAxisIndex: 1, yAxisIndex: 1}, this.position)[1]);
                                    var upSubjectslen = that.pValueData[index].upSubjectslen;
                                    if (newPosY < 0) newPosY  = 0;
                                    if (newPosY >= upSubjectslen) newPosY = upSubjectslen - 1;
                                    var tmp = that.pValueData[index].tmp;
                                    var downSubjectslen = that.pValueData[index].downSubjectslen;
                                    var totalSubjects = that.pValueData[index].downSubjectslen + that.pValueData[index].upSubjectslen;
                                    //var pValueThreshold = tmp[dataIndex][newPosY + downSubjectslen][index];
                                    var pValueThreshold = tmp[newPosX][newPosY + downSubjectslen][index];
                                    if(pValueThreshold <= 0) pValueThreshold = 0.0000;
                                    var lineDataBottom = [];
                                    var lineDataTop = [];
                                    for(var i in tmp) {
                                        var findDown = false;
                                        var findUp = false;
                                        for(var j in tmp[i]) {
                                            if(tmp[i][j][index] >= pValueThreshold && j < downSubjectslen && !findDown) {
                                                lineDataBottom.push(j - downSubjectslen);
                                                findDown = true;
                                            }
                                            if(tmp[i][j][index] < pValueThreshold && j >= downSubjectslen  && !findUp) {
                                                lineDataTop.push(j - downSubjectslen);
                                                findUp = true;
                                            }
                                            //top的边界
                                            if(tmp[i][j][index] >= pValueThreshold && (j == (totalSubjects - 1))  && !findUp) {
                                                lineDataTop.push(j - downSubjectslen + 1);
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
                                    //重新赋值
                                    that.pValueData[index].lineDataTop = lineDataTop;
                                    that.pValueData[index].lineDataBottom = lineDataBottom;
                                    that.pValueData[index].validData = [
                                                [that.pValueData[index].validSubjects, that.pValueData[index].lineDataTop[that.pValueData[index].validSubjects]],
                                                [that.pValueData[index].validSubjects, that.pValueData[index].lineDataBottom[that.pValueData[index].validSubjects]]
                                            ];
                                    var lineDataTopSolid = that.deepCopy(that.pValueData[index].lineDataTop);
                                    var lineDataTopDotted = that.deepCopy(that.pValueData[index].lineDataTop);
                                    var lineDataBottomSolid = that.deepCopy(that.pValueData[index].lineDataBottom);
                                    var lineDataBottomDotted = that.deepCopy(that.pValueData[index].lineDataBottom);
                                    for (var j in that.pValueData[index].lineDataTop) {
                                        if (j <= that.pValueData[index].validSubjects) {
                                            lineDataTopDotted[j] = "-";
                                            lineDataBottomDotted[j] = "-";
                                        }
                                        else {
                                            lineDataTopSolid[j] = "-";
                                            lineDataBottomSolid[j] = "-";
                                        }
                                    }
                                    for(var j in that.pValueData[index].lineDataTop) {
                                        data2[j] = [j, that.pValueData[index].lineDataTop[j]];
                                    }
                                    //更新chart
                                    myCharts[index].mergeOptions({
                                        series: [    
                                        {
                                            name: 'Up line',
                                            type: 'line',
                                            step: 'end',
                                            data: lineDataTopSolid,
                                            xAxisIndex: 1,
                                            yAxisIndex: 1,
                                            symbol: 'none',
                                            markPoint: {
                                                
                                                itemStyle: {
                                                    //color: 'white', 
                                                    color: 'rgb(59, 124, 244)' ,
                                                    borderWidth: 0,
                                                    //shadowBlur: 10,
                                                    //shadowColor: 'rgb(150, 150, 150)',
                                                },
                                                label: {
                                                    //color: 'rgb(59, 124, 244)' ,
                                                    color: 'white', 
                                                },
                                                data: [
                                                    {
                                                        name: 'pValueThreshold', 
                                                        value: pValueThreshold.toFixed(2), 
                                                        xAxis: 0, 
                                                        yAxis: lineDataTopSolid[0],
                                                        symbolSize: 40,
                                                        label: {
                                                            fontSize: 10,
                                                            //position: 'insideBottom'
                                                        },
                                                        //symbolRotate: 180,
                                                    },
                                                    /*
                                                    {
                                                        name: 'pValueThreshold', 
                                                        value: pValueThreshold.toFixed(2), 
                                                        xAxis: 0, 
                                                        yAxis: lineDataTopSolid[0],
                                                        symbolSize: 30,
                                                        label: {
                                                            fontSize: 7,
                                                        },
                                                    }
                                                    */
                                                ]
                                            },
                                            itemStyle: {
                                                normal: { 
                                                    lineStyle: { type: 'dotted', color:'rgb(59, 124, 244)', width: 3},
                                                    label: { show: false, position:'left' }
                                                }
                                            },
                                        },
                                        {
                                            name: 'Down line',
                                            type: 'line',
                                            step: 'end',
                                            data: lineDataBottomSolid,
                                            xAxisIndex: 1,
                                            yAxisIndex: 1,
                                            symbol: 'none',
                                            itemStyle: {
                                                normal: { 
                                                    lineStyle: { type: 'dotted', color:'rgb(59, 124, 244)', width:3},
                                                    label: { show: false, position:'left' }
                                                }
                                            },
                                        },
                                        {
                                            name: 'valid',
                                            type: 'line',
                                            xAxisIndex: 1,
                                            yAxisIndex: 1,
                                            z:9,
                                            data: that.pValueData[index].validData,
                                            smooth: true,
                                            showSymbol: false,
                                            itemStyle: {
                                                normal: {
                                                    lineStyle: { type: 'dotted', width: 3, color:'rgb(59, 124, 244)'},
                                                    label: { show: false, position:'left' }
                                                }
                                            }
                                        }]
                                    });                        
                                }
                                function onPointDraggingEnd(valueIndex, dataIndex, dx, dy) {
                                    //计算出pvalueThreshold然后更新所有视图
                                    var index = valueIndex;
                                    that.loading[index] = true;
                                    //获得该点的pvalue值，然后计算新的线
                                    var newPosX = Math.floor(myCharts[index].convertFromPixel({xAxisIndex: 1, yAxisIndex: 1}, this.position)[0]);
                                    if(newPosX < 0) newPosX = 0;
                                    if(newPosX >= that.pValueData[index].tmp.length) newPosX = that.pValueData[index].tmp.length - 1;
                                    var newPosY = Math.floor(myCharts[index].convertFromPixel({xAxisIndex: 1, yAxisIndex: 1}, this.position)[1]);
                                    var upSubjectslen = that.pValueData[index].upSubjectslen;
                                    if (newPosY < 0) newPosY  = 0;
                                    if (newPosY >= upSubjectslen) newPosY = upSubjectslen - 1;
                                    var tmp = that.pValueData[index].tmp;
                                    var downSubjectslen = that.pValueData[index].downSubjectslen;
                                    //var pValueThreshold = tmp[dataIndex][newPosY + downSubjectslen][index];
                                    var pValueThreshold = tmp[newPosX][newPosY + downSubjectslen][index];
                                    if(pValueThreshold <= 0) pValueThreshold = 0.0; //0.00001;
                                    that.pValueThreshold[index] = pValueThreshold * 100;

                                    console.log("pValueThreshold change: " + that.pValueThreshold[index]);
                                    that.$store.commit('set_pValueThreshold', that.pValueThreshold);
                                    if(that.sumModel){
                                        setTimeout(function(){
                                            that.setOptionSumLineThree(that.pValueDataUp, that.pValueDataBottom, index);
                                        }, 0);
                                        //that.setOptionSumLineThree(that.pValueDataUp, that.pValueDataBottom, index);
                                    } else {
                                        that.setOptionSub(that.pValueDataUp, that.pValueDataBottom, index);
                                    }
                                }
                                return {
                                    type: 'circle',
                                    position: myCharts[i].convertToPixel({xAxisIndex: 1, yAxisIndex: 1}, item),
                                    shape: {
                                        cx: 0,
                                        cy: 0,
                                        r: 5
                                    },
                                    invisible: true,
                                    draggable: true,
                                    cursor: "n-resize",
                                    ondrag: echarts.util.curry(onPointDragging, i, dataIndex),
                                    ondragend: echarts.util.curry(onPointDraggingEnd, i, dataIndex),
                                    //onmousemove: echarts.util.curry(showTooltip, dataIndex),
                                    //onmouseout: echarts.util.curry(hideTooltip, dataIndex),
                                    z: 100
                                };
                            });
                        //downline
                        var data3 = [];
                        for(var j in that.pValueData[i].lineDataBottom) {
                            data3[j] = [j, that.pValueData[i].lineDataBottom[j]];
                        }
                        var graphic3 = echarts.util.map(data3, function (item, dataIndex) {
                                function onPointDragging(featureIndex, dataIndex, dx, dy) {
                                    var myCharts = that.$refs.featuresChart;
                                    var index = featureIndex;
                                    //获得该点的pvalue值，然后计算新的线
                                    var newPosX = Math.floor(myCharts[index].convertFromPixel({xAxisIndex: 1, yAxisIndex: 1}, this.position)[0]);
                                    if(newPosX < 0) newPosX = 0;
                                    if(newPosX >= that.pValueData[index].tmp.length) newPosX = that.pValueData[index].tmp.length - 1;
                                    var newPosY = Math.floor(myCharts[index].convertFromPixel({xAxisIndex: 1, yAxisIndex: 1}, this.position)[1]);
                                    var downSubjectslen = that.pValueData[index].downSubjectslen;
                                    if (newPosY > 0) newPosY = -1;
                                    if (newPosY <= -downSubjectslen) newPosY = -downSubjectslen;
                                    var tmp = that.pValueData[index].tmp;
                                    var totalSubjects = that.pValueData[index].downSubjectslen + that.pValueData[index].upSubjectslen;
                                    //var pValueThreshold = tmp[dataIndex][newPosY + downSubjectslen][index];
                                    var pValueThreshold = tmp[newPosX][newPosY + downSubjectslen][index];
                                    if(pValueThreshold <= 0) pValueThreshold = 0.0000;
                                    var lineDataBottom = [];
                                    var lineDataTop = [];
                                    for(var i in tmp) {
                                        var findDown = false;
                                        var findUp = false;
                                        for(var j in tmp[i]) {
                                            if(tmp[i][j][index] >= pValueThreshold && j < downSubjectslen && !findDown) {
                                                lineDataBottom.push(j - downSubjectslen);
                                                findDown = true;
                                            }
                                            if(tmp[i][j][index] < pValueThreshold && j >= downSubjectslen  && !findUp) {
                                                lineDataTop.push(j - downSubjectslen);
                                                findUp = true;
                                            }
                                            //top的边界
                                            if(tmp[i][j][index] >= pValueThreshold && (j == (totalSubjects - 1))  && !findUp) {
                                                lineDataTop.push(j - downSubjectslen + 1);
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
                                    //重新赋值
                                    that.pValueData[index].lineDataTop = lineDataTop;
                                    that.pValueData[index].lineDataBottom = lineDataBottom;
                                    that.pValueData[index].validData = [
                                                [that.pValueData[index].validSubjects, that.pValueData[index].lineDataTop[that.pValueData[index].validSubjects]],
                                                [that.pValueData[index].validSubjects, that.pValueData[index].lineDataBottom[that.pValueData[index].validSubjects]]
                                            ];
                                    var lineDataTopSolid = that.deepCopy(that.pValueData[index].lineDataTop);
                                    var lineDataTopDotted = that.deepCopy(that.pValueData[index].lineDataTop);
                                    var lineDataBottomSolid = that.deepCopy(that.pValueData[index].lineDataBottom);
                                    var lineDataBottomDotted = that.deepCopy(that.pValueData[index].lineDataBottom);
                                    for (var j in that.pValueData[index].lineDataTop) {
                                        if (j <= that.pValueData[index].validSubjects) {
                                            lineDataTopDotted[j] = "-";
                                            lineDataBottomDotted[j] = "-";
                                        }
                                        else {
                                            lineDataTopSolid[j] = "-";
                                            lineDataBottomSolid[j] = "-";
                                        }
                                    }
                                    for(var j in that.pValueData[index].lineDataBottom) {
                                        data3[j] = [j, that.pValueData[index].lineDataBottom[j]];
                                    }
                                    //更新chart
                                    myCharts[index].mergeOptions({
                                        series: [
                                        {
                                            name: 'Up line',
                                            type: 'line',
                                            step: 'end',
                                            data: lineDataTopSolid,
                                            xAxisIndex: 1,
                                            yAxisIndex: 1,
                                            symbol: 'none',
                                            markPoint: {
                                                itemStyle: {
                                                    color: 'rgb(59, 124, 244)',
                                                    //color: 'white', 
                                                    borderWidth: 0,
                                                    //shadowBlur: 10,
                                                    //shadowColor: 'rgb(150, 150, 150)',
                                                },
                                                label: {
                                                    //color: 'rgb(59, 124, 244)' ,
                                                    color: 'white'
                                                },
                                                data: [
                                                    {
                                                        name: 'pValueThreshold', 
                                                        value: pValueThreshold.toFixed(2), 
                                                        xAxis: 0, 
                                                        yAxis: lineDataBottomSolid[0],
                                                        symbolSize: 40,
                                                        label: {
                                                            fontSize: 10,
                                                            //position: 'insideBottom',
                                                        },
                                                        //symbolRotate: 180,
                                                    },
                                                    /*
                                                    {
                                                        name: 'pValueThreshold', 
                                                        value: pValueThreshold.toFixed(2), 
                                                        xAxis: 0, 
                                                        yAxis: lineDataTopSolid[0],
                                                        symbolSize: 30,
                                                        label: {
                                                            fontSize: 7,
                                                        },
                                                    }
                                                    */
                                                ]
                                            },
                                            itemStyle: {
                                                normal: { 
                                                    lineStyle: { type: 'dotted', color:'rgb(59, 124, 244)', width: 3},
                                                    label: { show: false, position:'left' }
                                                }
                                            },
                                        },
                                        {
                                            name: 'Down line',
                                            type: 'line',
                                            step: 'end',
                                            data: lineDataBottomSolid,
                                            xAxisIndex: 1,
                                            yAxisIndex: 1,
                                            symbol: 'none',
                                            
                                            itemStyle: {
                                                normal: { 
                                                    lineStyle: { type: 'dotted', color:'rgb(59, 124, 244)', width: 3},
                                                    label: { show: false, position:'left' }
                                                }
                                            },
                                        },
                                        {
                                            name: 'valid',
                                            type: 'line',
                                            xAxisIndex: 1,
                                            yAxisIndex: 1,
                                            z:9,
                                            data: that.pValueData[index].validData,
                                            smooth: true,
                                            showSymbol: false,
                                            itemStyle: {
                                                normal: {
                                                    lineStyle: { type: 'dotted', width: 3, color:'rgb(59, 124, 244)'},
                                                    label: { show: false, position:'left' }
                                                }
                                            }
                                        }]
                                    });
                                }
                                function onPointDraggingEnd(valueIndex, dataIndex, dx, dy) {
                                    //计算出pvalueThreshold然后更新所有视图
                                    var index = valueIndex;
                                    that.loading[index] = true;
                                    var newPosX = Math.floor(myCharts[index].convertFromPixel({xAxisIndex: 1, yAxisIndex: 1}, this.position)[0]);
                                    if(newPosX < 0) newPosX = 0;
                                    if(newPosX >= that.pValueData[index].tmp.length) newPosX = that.pValueData[index].tmp.length - 1;
                                    var newPosY = Math.floor(myCharts[index].convertFromPixel({xAxisIndex: 1, yAxisIndex: 1}, this.position)[1]);
                                    var downSubjectslen = that.pValueData[index].downSubjectslen;
                                    if (newPosY > 0) newPosY = -1;
                                    if (newPosY <= -downSubjectslen) newPosY = -downSubjectslen;
                                    var tmp = that.pValueData[index].tmp;
                                    //var pValueThreshold = tmp[dataIndex][newPosY + downSubjectslen][index];
                                    var pValueThreshold = tmp[newPosX][newPosY + downSubjectslen][index];
                                    if(pValueThreshold <= 0) pValueThreshold = 0.0000;
                                    that.pValueThreshold[index] = pValueThreshold * 100;

                                    console.log("pValueThreshold change: " + that.pValueThreshold[index]);
                                    that.$store.commit('set_pValueThreshold', that.pValueThreshold);
                                    if(that.sumModel){
                                        setTimeout(function(){
                                            that.setOptionSumLineThree(that.pValueDataUp, that.pValueDataBottom, index);
                                        }, 0);
                                        //that.setOptionSumLineThree(that.pValueDataUp, that.pValueDataBottom, index);
                                    } else {
                                        that.setOptionSub(that.pValueDataUp, that.pValueDataBottom, index);
                                    }
                                }
                                return {
                                    type: 'circle',
                                    position: myCharts[i].convertToPixel({xAxisIndex: 1, yAxisIndex: 1}, item),
                                    shape: {
                                        cx: 0,
                                        cy: 0,
                                        r: 5
                                    },
                                    invisible: true,
                                    draggable: true,
                                    cursor: "n-resize",
                                    ondrag: echarts.util.curry(onPointDragging, i, dataIndex),
                                    ondragend: echarts.util.curry(onPointDraggingEnd, i, dataIndex),
                                    //onmousemove: echarts.util.curry(showTooltip, dataIndex),
                                    //onmouseout: echarts.util.curry(hideTooltip, dataIndex),
                                    z: 100
                                };
                            });
                        graphic1.push.apply(graphic1, graphic2);
                        graphic1.push.apply(graphic1, graphic3);
                        myCharts[i].mergeOptions({
                            graphic: graphic1,
                        });            
                    }
                    //console.log('refs', that.$refs.featuresChart);
                }
            }, 0);
        },
        computed: {
            ...mapState(['multiVal', 'linksVal', 'testVal', 'features', 'subjects_valid', 'selected_subjects', 'selected_subjects_name', 'lambda', 'ROIZValue', 'DBScanRadius', 'selected_index', 'modelVal', 'singlefeatures','self']),
        },
        methods: {
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
            duplicate(index){
                //添加一个singlefeature
                this.$store.commit('add_singlefeatures', index);
                //设置当前index为最新那个
                this.$store.commit('set_selected_index_newest', index);
                //记录当前index
                var localindex = this.selected_index;
                this.$store.commit('set_selectedFeature_singleLoading', [localindex, true]);
                this.$store.commit('set_selectedFeature_multiLoading', [localindex, true]);
                //添加单变量
                NetService.singleAnalyzeForOnefeature(index, this.pValueThreshold[Number(index)], this.validThreshold[Number(index)], this.testVal, this.singlefeatures[localindex].groupName, this.singlefeatures[localindex].subjectList, (res, err) => {
                    if(err){
                        console.error(err);
                        return;
                    }
                    if(!res.data){
                        console.log('data not exists!');
                        return;
                    }
                    console.log('singleAnalyzeForOnefeature', res, localindex);
                    //{"code":0,"data":{"originPvalue":[2.0], "pvalue":[1.0], "average":[508.4],"variance":[508.4],"totalPeople":[504]},"message":"获取成功"}
                    this.$store.commit('set_selectedFeature_data_for_single', [res.data.data, localindex]);
                    this.$store.commit('set_selectedFeature_data_data', [this.linksVal, localindex]);

                    //获取多变量
                 NetService.multiAnalyzeForOnefeature(index, this.pValueThreshold[Number(index)], this.validThreshold[Number(index)], this.$store.state.multiGroupVal, this.ROIZValue, this.DBScanRadius, -1, this.singlefeatures[localindex].groupName, this.singlefeatures[localindex].subjectList, (res, err) => {
                    if(err){
                        console.error(err);
                        return;
                    }
                    if(!res.data){
                        console.log('data not exists!');
                        return;
                    }
                    console.log('multiAnalyzeForOnefeature', res.data, localindex, res.data.message);
                    //{"code":0,"data":{"accuracy":[0.5,..],"AccSelectConnection":[1,...], "weight":[508.4,...], "WeightSelectConnection": [1,..]},"message":"获取成功"}
                    this.$store.commit('set_selectedFeature_data_for_multi', [res.data.data, localindex, res.data.message]);
                    this.$store.commit('set_selectedFeature_lambda_for_multi', [res.data.data.lambda, localindex]);
                    this.$store.commit('set_selectedFeature_data_multidata', [this.multiVal, localindex]);
                });
                    
                });

                
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
                var pValueThreshold = that.pValueThreshold[index] / 100.0;
                
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
                var validSubjectsThreshold = totalSubjects * this.validThreshold[index] / 100;
                var tenPerValid = totalSubjects * 0.1;
                //console.log('tenPerValid', tenPerValid);
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
                                shadowBlur: 0,
                                //shadowColor: 'rgba(0, 0, 0, 0.5)'
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
            setOptionSumLineThree(pointUp, pointBottom, index){
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
                var pValueThreshold = that.pValueThreshold[index] / 100.0;
                
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
                var validSubjectsThreshold = totalSubjects * this.validThreshold[index] / 100;
                var tenPerValid = totalSubjects * 0.1;
                //console.log('tenPerValid', tenPerValid);
                var halfSubjects = downSubjectslen; //下半部分人数

                var originUpSubjects = upSubjectslen;
                var originDownSubjects = downSubjectslen;

                var originUpSubjectsBeforePadding = upSubjectslen;
                var originDownSubjectsBeforePadding = downSubjectslen;
               //console.log('originDownSubjectsBeforePadding', originUpSubjectsBeforePadding, originDownSubjectsBeforePadding);

               var tmpthreshold = pValueThreshold;
               if(tmpthreshold == 0) tmpthreshold = 1;
                //横向排序，按达标人数排序
                tmp.sort(function(x,y) {
                    return x.filter(function(z){
                        return z[index] >= tmpthreshold;
                    }).length - y.filter(function(z){
                        return z[index] >= tmpthreshold;
                    }).length;
                }).reverse();

                this.validLinks[index] = tmp.filter(function(x) {
                    return x.filter(function(z){
                        return z[index] >= pValueThreshold;
                    }).length >= validSubjectsThreshold;
                }).length;
                /*
                if(index == 0) {
                    var fil = tmp.filter(function(x) {
                    return x.filter(function(z){
                        return z[index] >= pValueThreshold;
                    }).length >= validSubjectsThreshold;
                    });
                    console.log("filter", fil);
                    var filterArr = [];
                    for(var i in fil) {
                        filterArr.push(fil[i][0][6]);
                    }
                    console.log("filterArr", filterArr.sort(function(i, j){return i - j;}));
                }
                */
                //对横向的进行过滤 根据tenPerValid
                /*
                tmp = tmp.filter(function(x) {
                    return x.filter(function(z){
                        return z[index] >= pValueThreshold;
                    }).length >= tenPerValid;
                });
                */
                //横向过滤
                var constvalid = 1000;
                tmp = tmp.filter(function(x, xi) {
                    return xi < constvalid;
                });

                var originTotalSubjects = tmp.length;

                //sample
                /*
                tmp = tmp.filter(function(x,i){
                    return i % 3 == 0;
                });
                */
                //将列数变为30
                this.colNum = Math.ceil(originTotalSubjects / 30);
                //console.log('colNum', this.colNum, index);
                //将行数变为20
                this.rowNum = Math.ceil((upSubjectslen + downSubjectslen) / 20);

                maxNum = tmp.length;
                var xdata2 = [];
                for(var i in tmp){
                    xdata2.push(i);
                }

                //console.log('tmp', tmp, upSubjectslen, downSubjectslen);
                //padding to col
                var uL = upSubjectslen;
                var dL = downSubjectslen;
                for (var i in tmp) {
                    uL = upSubjectslen;
                    dL = downSubjectslen;
                    while(dL % this.rowNum != 0) {
                        dL++;
                        tmp[i].unshift(tmp[i][0]);
                        //tmp[i].push.apply([tmp[i][0]], tmp[i]);
                    }
                    while(uL % this.rowNum != 0) {
                        uL++;
                        tmp[i].push(tmp[i][tmp[i].length - 1]);
                        //tmp[i].push.apply(tmp[i], [tmp[i][tmp[i].length - 1]]);
                    }
                }
                upSubjectslen = uL;
                downSubjectslen = dL;

                //padding to row
                var tL = maxNum;
                while(tL % this.colNum != 0) {
                    tL++;
                    tmp.push.apply(tmp, [tmp[maxNum - 1]]);
                }
                maxNum = tmp.length;

                originTotalSubjects = maxNum;
                originDownSubjects = downSubjectslen;
                originUpSubjects = upSubjectslen;

                //console.log('changeData', maxNum, upSubjectslen, downSubjectslen);
                //根据colNum和rowNum取平均
                uL = upSubjectslen / this.rowNum;
                dL = downSubjectslen / this.rowNum;
                tL = maxNum / this.colNum;
                var averageTmp = new Array();
                //console.log('tmp', tmp, upSubjectslen, downSubjectslen);
                for(var i = 0; i < tL; i++) {
                    averageTmp[i] = new Array();
                    for (var j = 0; j < uL + dL; j++) {
                        averageTmp[i][j] = this.deepCopy(tmp[i*this.colNum][j*this.rowNum]);
                        //console.log('averageTmp[i][j]',i,j,averageTmp[i][j]);
                        averageTmp[i][j][index] = 0;
                        for (var k = 0; k < this.colNum; k++) {
                            for (var l = 0; l < this.rowNum; l++) {
                                averageTmp[i][j][index] += tmp[i*this.colNum + k][j*this.rowNum + l][index];
                            }
                        }
                        averageTmp[i][j][index] /= this.colNum * this.rowNum;
                    }
                }
                tmp = averageTmp;
                upSubjectslen = uL;
                downSubjectslen = dL;
                maxNum = tL;
                validSubjectsThreshold = (upSubjectslen + downSubjectslen) * this.validThreshold[index] / 100;
                totalSubjects = upSubjectslen + downSubjectslen;

                //获取上曲线和下曲线
                //对于每个连接,数据小-大-小
                var lineDataBottom = [];
                var lineDataTop = [];
                for(var i in tmp) {
                    var findDown = false;
                    var findUp = false;
                    for(var j in tmp[i]) {
                        if(tmp[i][j][index] >= pValueThreshold && j < downSubjectslen && !findDown) {
                            lineDataBottom.push(j - downSubjectslen);
                            findDown = true;
                        }
                        if(tmp[i][j][index] < pValueThreshold && j >= downSubjectslen  && !findUp) {
                            lineDataTop.push(j - downSubjectslen);
                            findUp = true;
                        }
                        //top的边界
                        if(tmp[i][j][index] >= pValueThreshold && (j == (totalSubjects - 1))  && !findUp) {
                            lineDataTop.push(j - downSubjectslen + 1);
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

                /*
                var data = [];
                for (var i in tmp) {
                    for (var j in tmp[i]) {
                        data.push([Number(i), Number(j - downSubjectslen), tmp[i][j][index], tmp[i][j][featurelen - 1]]);
                    }
                }
                */
                var data = [];
                for (var i in tmp) {
                    for (var j = 0; j < downSubjectslen; j++) {
                        data.push([Number(i), Number(j - downSubjectslen), tmp[i][j][index], tmp[i][j][featurelen - 1]]);
                    }
                }
                var data2 = [];
                for (var i in tmp) {
                    for (var j = downSubjectslen; j < tmp[i].length; j++) {
                        data2.push([Number(i), Number(j - downSubjectslen), tmp[i][j][index], tmp[i][j][featurelen - 1]]);
                    }
                }

                var xdata = [];
                for(var i in tmp){
                    xdata.push(i);
                }
                //xdata.push(i);

                data = data.map(function (item) {
                    return [item[0], item[1], item[2], item[3] || '-'];
                });

                data2 = data2.map(function (item) {
                    return [item[0], item[1], item[2], item[3] || '-'];
                });

                var validData = [
                            [validSubjects, lineDataTop[validSubjects]],
                            [validSubjects, lineDataBottom[validSubjects]]
                        ];

                //记录处理后的数据
                this.pValueData[index].data = data;
                this.pValueData[index].lineDataTop = lineDataTop;
                this.pValueData[index].lineDataBottom = lineDataBottom;
                this.pValueData[index].validSubjects = validSubjects;
                this.pValueData[index].upSubjectslen = upSubjectslen;
                this.pValueData[index].downSubjectslen = downSubjectslen;
                this.pValueData[index].validData = validData;
                this.pValueData[index].tmp = tmp;

                //实线和虚线
                var lineDataTopSolid = this.deepCopy(lineDataTop);
                var lineDataTopDotted = this.deepCopy(lineDataTop);
                var lineDataBottomSolid = this.deepCopy(lineDataBottom);
                var lineDataBottomDotted = this.deepCopy(lineDataBottom);
                for (var i in lineDataTop) {
                    if (i <= validSubjects) {
                        lineDataTopDotted[i] = "-";
                        lineDataBottomDotted[i] = "-";
                    }
                    else {
                        lineDataTopSolid[i] = "-";
                        lineDataBottomSolid[i] = "-";
                    }
                }
                //console.log('selected_subjects', this.selected_subjects);

                this.features[index].options = {
                    tooltip: {
                        position: 'top',
                        
                        formatter: function (params) {
                            //if (params.value.length > 1) {
                            //    return params.value[3] + ' ' + params.value[2];
                            //}
                        }
                    },
                    toolbox: {
                        feature: {
                            //saveAsImage: {}
                            //dataZoom: {
                            //    xAxisIndex: [0, 1],
                            //    yAxisIndex: 'none',
                            //},
                        }
                    },
                    animation: false,
                    grid: {
                        height: '155px',
                        width: '85%',
                        top: '40px',
                        left: '10%',
                    },
                    xAxis: [{
                        type: 'category',
                        //data: hours,
                        //z: 999,
                        axisLabel: {
                            show: false,
                        },
                        //max: this.maxSubjectLength
                        //offset: -100
                    },{
                        type: 'category',
                        data: xdata,
                        min: 0,
                        max: maxNum,
                        position: 'bottom',
                        boundaryGap: false,
                        axisLabel: {
                            show: false,
                            color: 'black',
                            fontSize: 9,
                            margin: 5,
                            interval: 4,
                        },
                        axisTick: {
                            show: false,
                            interval: 4,
                        },
                        axisLine:{
                            show: false,
                            onZero: true,
                            onZeroAxisIndex: 1,
                            lineStyle: {
                                color: 'white',  
                                width: 3,
                            },
                        },
                        z: 99,
                    },{
                        type: 'category',
                        data: xdata2,
                        min: 0,
                        max: originTotalSubjects,
                        position: 'bottom',
                        boundaryGap: false,
                        axisLabel: {
                            show: true,
                            color: 'black',
                            fontSize: 12,
                            margin: 5,
                            interval: 199,
                        },
                        axisTick: {
                            show: true,
                            interval: 199,
                        },
                        axisLine:{
                            show: true,
                            onZero: true,
                            onZeroAxisIndex: 2,
                            lineStyle: {
                                color: 'white',  
                                width: 3,
                            },
                        },
                        z: 9,
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
                        min: -downSubjectslen,//'dataMin', //最小值为刻度最小值
                        max: upSubjectslen,//'dataMax',
                        position: 'left',
                        nameGap: 10,
                        axisLabel: {
                            show: false,
                        },
                        axisTick: {
                            show: false,
                            length: 3,
                        },
                        axisLine:{
                            show: false,
                        },
                        splitLine: {
                            show: false
                        },
                        axisLabel: {
                            show: false,
                            fontSize: 9,
                            margin: 5,
                            showMaxLabel: false,
                            showMinLabel: false,
                        }
                    },{
                        type: 'value',
                        boundaryGap: false,
                        min: -originDownSubjects,//'dataMin', //最小值为刻度最小值
                        max: originUpSubjects,//'dataMax',
                        position: 'left',
                        //nameGap: 10,
                        axisLabel: {
                            show: true,
                        },
                        axisTick: {
                            show: false,
                            //length: 3,
                            //interval: 9,
                            /*
                            interval: function(i, st){
                                if(i % 5 == 0) return true;
                                if(i == originUpSubjectsBeforePadding || i == -originDownSubjectsBeforePadding){
                                    return true;
                                }
                                return false;
                            },
                            */
                        },
                        axisLine:{
                            show: true,
                        },
                        splitLine: {
                            show: false
                        },
                        axisLabel: {
                            fontSize: 12,
                            margin: 2,
                            showMaxLabel: false,
                            showMinLabel: false,
                            //interval: 9,
                            /*
                            interval: function(i, st){
                                if(i % 5 == 0) return true;
                                if(i == originUpSubjectsBeforePadding || i == -originDownSubjectsBeforePadding){
                                    return true;
                                }
                                return false;
                            },
                            */
                        }
                    },{
                        type: 'category',
                        data: [this.selected_subjects.length > 1 ? this.selected_subjects[1].data.firstName : '', this.selected_subjects[0].data.firstName],
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
                            rotate: 90,
                            margin: 14,
                            fontSize: 14,
                        },
                    }],
                    visualMap: [{
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
                        //inRange: {color: ['white', 'blue']},
                        inRange: {
                            color: [
                                //'#EFEFEF',
                                //'#FF0000',                
                                'rgb(242, 237, 241)',
                                'rgb(117, 90, 170)',
                            ]
                        },
                        show: false,
                        seriesIndex: 1,
                    },{
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
                        //inRange: {color: ['white', 'blue']},
                        inRange: {
                            color: [
                                //'#EFEFEF',
                                //'#FF0000',  
                                'rgb(242, 237, 241)',              
                                '#FE8F00',
                            ]
                        },
                        show: false,
                        seriesIndex: 0,
                    }],
                    silent: true,
                    itemStyle: {
                        borderColor: '#CBCBCB',
                        borderType: 'solid',
                        shadowBlur: 0,
                        borderWidth: 0.4,
                        z: 2,
                    },
                    //color: 'rgba(255,255,255,1)',
                    series: [
                    {
                        name: 'Punch Card',
                        type: 'heatmap',
                        data: data,
                        progressiveThreshold: 4000000,
                        progressive: 4000000,
                        label: {
                            show: false
                        },
                        emphasis: {
                            itemStyle: {
                                shadowBlur: 0,
                                //shadowColor: 'rgba(0, 0, 0, 0.5)'
                            }
                        },
                    },
                    {
                        name: 'Punch Card',
                        type: 'heatmap',
                        data: data2,
                        progressiveThreshold: 4000000,
                        progressive: 4000000,
                        label: {
                            show: false
                        },
                        emphasis: {
                            itemStyle: {
                                shadowBlur: 0,
                                //shadowColor: 'rgba(0, 0, 0, 0.5)'
                            }
                        },
                    },
                    {
                        name: 'Up line',
                        type: 'line',
                        step: 'end',
                        data: lineDataTopSolid,
                        xAxisIndex: 1,
                        yAxisIndex: 1,
                        symbol: 'none',
                        markPoint: {
                            itemStyle: {
                                color: 'rgb(59, 124, 244)',
                                //color: 'white', 
                                borderWidth: 0,
                                //shadowBlur: 10,
                                //shadowColor: 'rgb(150, 150, 150)',
                            },
                            label: {
                                color: 'white', 
                                //color: 'rgb(59, 124, 244)',
                            },
                            data: [
                                {
                                    name: 'pValueThreshold', 
                                    value: (that.pValueThreshold[index] / 100).toFixed(2), 
                                    xAxis: 0, 
                                    yAxis: lineDataTopSolid[0],
                                    symbolSize: 40,
                                    label: {
                                        fontSize: 10,
                                        //position: 'insideBottom', 
                                    },
                                    //symbolRotate: 180,
                                }
                            ]
                        },
                        itemStyle: {
                            normal: { 
                                lineStyle: { type: 'solid', color:'rgb(59, 124, 244)', width: 2},
                                label: { show: false, position:'left' }
                            }
                        },
                    },
                    /*
                    {
                        name: 'Up line dotted',
                        type: 'line',
                        step: 'end',
                        data: lineDataTopDotted,
                        xAxisIndex: 1,
                        yAxisIndex: 1,
                        symbol: 'none',
                        itemStyle: {
                            normal: { 
                                lineStyle: { type: 'dotted', color:'rgb(59, 124, 244)', width: 2},
                                label: { show: false, position:'left' }
                            }
                        },
                    },
                    */
                    {
                        name: 'Down line',
                        type: 'line',
                        step: 'end',
                        data: lineDataBottomSolid,
                        xAxisIndex: 1,
                        yAxisIndex: 1,
                        symbol: 'none',
                        /*
                        markPoint: {
                            itemStyle: {
                                color: 'white', 
                                borderWidth: 0,
                                shadowBlur: 10,
                                shadowColor: 'rgb(150, 150, 150)',
                            },
                            label: {
                                color: 'rgb(59, 124, 244)' 
                            },
                            data: [
                                {
                                    name: 'pValueThreshold', 
                                    value: (that.pValueThreshold[index] / 100).toFixed(2), 
                                    xAxis: 0, 
                                    yAxis: lineDataBottomSolid[0],
                                    symbolSize: 30,
                                    label: {
                                        fontSize: 7,
                                    }
                                }
                            ]
                        },
                        */
                        itemStyle: {
                            normal: { 
                                lineStyle: { type: 'solid', color:'rgb(59, 124, 244)', width: 2},
                                label: { show: false, position:'left' }
                            }
                        },
                    },
                    /*
                    {
                        name: 'Down line dotted',
                        type: 'line',
                        step: 'end',
                        data: lineDataBottomDotted,
                        xAxisIndex: 1,
                        yAxisIndex: 1,
                        symbol: 'none',
                        itemStyle: {
                            normal: { 
                                lineStyle: { type: 'dotted', color:'rgb(59, 124, 244)', width: 2},
                                label: { show: false, position:'left' }
                            }
                        },
                    },
                    */
                    {
                        name: 'valid',
                        type: 'line',
                        xAxisIndex: 1,
                        yAxisIndex: 1,
                        z:9,
                        data: validData,
                        smooth: true,
                        showSymbol: false,
                        markPoint: {
                            itemStyle: {
                                color: 'rgb(59, 124, 244)', 
                                borderWidth: 0,
                                //shadowBlur: 10,
                                //shadowColor: 'rgb(150, 150, 150)',
                            },
                            label: {
                                color: 'white' 
                            },
                            z: 99,
                            data: [
                                {
                                    name: 'validThreshold', 
                                    value: that.validThreshold[index].toFixed(0) + '%', 
                                    xAxis: validData[0][0], 
                                    yAxis: validData[0][1],
                                    symbolSize: 40,
                                    label: {
                                        padding: [0, -2, 0, 0],
                                        fontSize: 10,
                                        position: 'insideRight'
                                    },
                                    symbolRotate: -90,
                                }
                            ]
                        },
                        itemStyle: {
                            normal: {
                                lineStyle: { type: 'solid', width: 3, color:'rgb(59, 124, 244)'},
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
                    },
                    {
                        name: 'point',
                        type: 'scatter',
                        data: [[0, originUpSubjects],[0, -originDownSubjects]],
                        xAxisIndex: 2,
                        yAxisIndex: 2,
                        symbolSize: 0,
                    }
                    ]
                };
                this.loading[index] = false;
            },
            //排序方法：横坐标为subjects，纵坐标为links，上下组横向合起来按照达标之和数目排序
            setOptionSumLineTwo(pointUp, pointBottom, index){
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
                var pValueThreshold = that.pValueThreshold[index] / 100.0;
                
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
                var validSubjectsThreshold = totalSubjects * this.validThreshold[index] / 100;
                var tenPerValid = totalSubjects * 0.1;
                var halfSubjects = downSubjectslen; //下半部分人数

                //横向排序，按达标人数排序
                tmp.sort(function(x,y) {
                    return x.filter(function(z){
                        return z[index] >= pValueThreshold;
                    }).length - y.filter(function(z){
                        return z[index] >= pValueThreshold;
                    }).length;
                }).reverse();

                //对横向的进行过滤
                /*
                tmp = tmp.filter(function(x) {
                    return x.filter(function(z){
                        return z[index] >= pValueThreshold;
                    }).length >= tenPerValid;
                });
                */
                var constvalid = 1000;
                tmp = tmp.filter(function(x, xi) {
                    return xi < constvalid
                });

                //sample
                /*
                tmp = tmp.filter(function(x,i){
                    return i % 3 == 0;
                });
                */
                maxNum = tmp.length;

                //console.log('tmp', tmp, upSubjectslen, downSubjectslen);
                //padding to col
                var uL = upSubjectslen;
                var dL = downSubjectslen;
                for (var i in tmp) {
                    uL = upSubjectslen;
                    dL = downSubjectslen;
                    while(dL % this.rowNum != 0) {
                        dL++;
                        tmp[i].unshift(tmp[i][0]);
                        //tmp[i].push.apply([tmp[i][0]], tmp[i]);
                    }
                    while(uL % this.rowNum != 0) {
                        uL++;
                        tmp[i].push(tmp[i][tmp[i].length - 1]);
                        //tmp[i].push.apply(tmp[i], [tmp[i][tmp[i].length - 1]]);
                    }
                }
                upSubjectslen = uL;
                downSubjectslen = dL;

                //padding to row
                var tL = maxNum;
                while(tL % this.colNum != 0) {
                    tL++;
                    tmp.push.apply(tmp, [tmp[maxNum - 1]]);
                }
                maxNum = tmp.length;

                //console.log('changeData', maxNum, upSubjectslen, downSubjectslen);
                //根据colNum和rowNum取平均
                uL = upSubjectslen / this.rowNum;
                dL = downSubjectslen / this.rowNum;
                tL = maxNum / this.colNum;
                var averageTmp = new Array();
                //console.log('tmp', tmp, upSubjectslen, downSubjectslen);
                for(var i = 0; i < tL; i++) {
                    averageTmp[i] = new Array();
                    for (var j = 0; j < uL + dL; j++) {
                        averageTmp[i][j] = this.deepCopy(tmp[i*this.colNum][j*this.rowNum]);
                        //console.log('averageTmp[i][j]',i,j,averageTmp[i][j]);
                        averageTmp[i][j][index] = 0;
                        for (var k = 0; k < this.colNum; k++) {
                            for (var l = 0; l < this.rowNum; l++) {
                                averageTmp[i][j][index] += tmp[i*this.colNum + k][j*this.rowNum + l][index];
                            }
                        }
                        averageTmp[i][j][index] /= this.colNum * this.rowNum;
                    }
                }
                tmp = averageTmp;
                upSubjectslen = uL;
                downSubjectslen = dL;
                maxNum = tL;
                validSubjectsThreshold = (upSubjectslen + downSubjectslen) * this.validThreshold[index] / 100;
                totalSubjects = upSubjectslen + downSubjectslen;

                //获取上曲线和下曲线
                //对于每个连接,数据小-大-小
                var lineDataBottom = [];
                var lineDataTop = [];
                for(var i in tmp) {
                    var findDown = false;
                    var findUp = false;
                    for(var j in tmp[i]) {
                        if(tmp[i][j][index] >= pValueThreshold && j < downSubjectslen && !findDown) {
                            lineDataBottom.push(j - downSubjectslen);
                            findDown = true;
                        }
                        if(tmp[i][j][index] < pValueThreshold && j >= downSubjectslen  && !findUp) {
                            lineDataTop.push(j - downSubjectslen);
                            findUp = true;
                        }
                        //top的边界
                        if(tmp[i][j][index] >= pValueThreshold && (j == (totalSubjects - 1))  && !findUp) {
                            lineDataTop.push(j - downSubjectslen + 1);
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
                        data.push([Number(i), Number(j - downSubjectslen), tmp[i][j][index], tmp[i][j][featurelen - 1]]);
                    }
                }
                
                var xdata = [];
                for(var i in tmp){
                    xdata.push(i);
                }
                //xdata.push(i);

                data = data.map(function (item) {
                    return [item[0], item[1], item[2], item[3] || '-'];
                });

                //console.log('selected_subjects', this.selected_subjects);

                this.features[index].options = {
                    tooltip: {
                        position: 'top',
                        
                        formatter: function (params) {
                            //if (params.value.length > 1) {
                            //    return params.value[3] + ' ' + params.value[2];
                            //}
                        }
                    },
                    dataZoom: [
                    {
                        start: 0,
                        end: 100,
                        handleIcon: 'M10.7,11.9v-1.3H9.3v1.3c-4.9,0.3-8.8,4.4-8.8,9.4c0,5,3.9,9.1,8.8,9.4v1.3h1.3v-1.3c4.9-0.3,8.8-4.4,8.8-9.4C19.5,16.3,15.6,12.2,10.7,11.9z M13.3,24.4H6.7V23h6.6V24.4z M13.3,19.6H6.7v-1.4h6.6V19.6z',
                        handleSize: '80%',
                        handleStyle: {
                            color: '#fff',
                            shadowBlur: 3,
                            shadowColor: 'rgba(0, 0, 0, 0.6)',
                            shadowOffsetX: 2,
                            shadowOffsetY: 2
                        },
                        xAxisIndex: [0,1],
                        minValueSpan: 5,
                        //yAxisIndex: [0,1],
                        //showDataShadow: false,

                    }],
                    toolbox: {
                        feature: {
                            //saveAsImage: {}
                            //dataZoom: {
                            //    xAxisIndex: [0, 1],
                            //    yAxisIndex: 'none',
                            //},
                        }
                    },
                    animation: false,
                    grid: {
                        height: 120,
                        width: 210,
                        top: 0
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
                        min: 0,
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
                        min: -downSubjectslen,//'dataMin', //最小值为刻度最小值
                        max: upSubjectslen,//'dataMax',
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
                        data: [this.selected_subjects[1].data.firstName, this.selected_subjects[0].data.firstName],
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
                        //inRange: {color: ['white', 'blue']},
                        /*
                        inRange: {color: [ //彩虹
                            'rgba(128, 128, 128, 0.5)',
                            'rgba(255, 0.0, 255, 1.0)',
                            'rgba(0.0, 0.0, 255, 1.0)',
                            'rgba(0.0, 255, 255, 1.0)', 
                            'rgba(0.0, 255, 0.0, 1.0)', 
                            'rgba(255, 128, 0.0, 1.0)', 
                            'rgba(255, 128, 0.0, 1.0)',
                            'rgba(255, 0.0, 0.0, 1.0)',       
                        ]},
                        */
                        inRange: {color: [ //灰蓝橙红
                            'rgba(240, 240, 240, 0.5)',
                            'rgba(0, 116, 188, 1.0)',
                            'rgba(0, 170, 226, 1.0)',
                            'rgba(68, 199, 239, 1.0)',
                            'rgba(154, 217, 238, 1.0)', 
                            'rgba(216, 236, 241, 1.0)', 
                            'rgba(242, 238, 197, 1.0)', 
                            'rgba(249, 216, 168, 1.0)',
                            'rgba(245, 177, 139, 1.0)',
                            'rgba(239, 133, 122, 1.0)',
                            'rgba(216, 82, 88, 1.0)', 
                            'rgba(175, 53, 71, 1.0)',   
                        ]},
                        show: false,
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
                         markLine: {
                            symbol: ['none', 'none'],
                            label: {show: false},
                            data: [
                                {xAxis: maxNum / 2},
                            ]
                        },
                        emphasis: {
                            itemStyle: {
                                shadowBlur: 0,
                                //shadowColor: 'rgba(0, 0, 0, 0.5)'
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
                                lineStyle: { type: 'solid', color:'green', width: 2},
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
                                lineStyle: { type: 'solid', color:'green', width: 2},
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
                            },
                            ]
                        },
                    },
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
            }
        },
        watch: {
            validThreshold: {
                deep: true,
                handler: function(newVal, oldVal) {
                    if(newVal){
                        /*
                        console.log("validThreshold change: " + newVal);
                        this.$store.commit('set_validThreshold', newVal);
                        if(this.sumModel){
                            this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 0);
                            this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 1);
                            this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 2);
                            this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 3);
                            this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 4);
                        } else {
                            this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 0);
                            this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 1);
                            this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 2);
                            this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 3);
                            this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 4);   
                        }
                        */
                        /*
                        NetService.update2D(this.pValueThreshold, this.validThreshold, this.testVal, this.lambda, this.ROIZValue, this.DBScanRadius, (res, err) => {
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
                            this.$store.commit('set_features_data_multidata', this.multiVal);
                        });
                        */

                    }
                }
            },
            pValueThreshold: {
                deep: true,
                handler: function(newVal, oldVal) {
                    if(newVal){
                        /*
                        console.log("pValueThreshold change: " + newVal);
                        this.$store.commit('set_pValueThreshold', newVal);
                        if(this.sumModel){
                            this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 0);
                            this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 1);
                            this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 2);
                            this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 3);
                            this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 4);
                        } else {
                            this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 0);
                            this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 1);
                            this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 2);
                            this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 3);
                            this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 4);   
                        }
                        */
                        /*
                        NetService.update2D(this.pValueThreshold, this.validThreshold, this.testVal, this.lambda, this.ROIZValue, this.DBScanRadius, (res, err) => {
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
                            this.$store.commit('set_features_data_multidata', this.multiVal);
                        });
                        */
                    }
                }
            },
            /*
            colNum: {
                deep: true,
                handler: function(newVal, oldVal) {
                    if(newVal){
                        if(this.sumModel){
                            this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 0);
                            this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 1);
                            this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 2);
                            this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 3);
                            this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 4);
                        } else {
                            this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 0);
                            this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 1);
                            this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 2);
                            this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 3);
                            this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 4);   
                        }
                    }
                }
            },
            rowNum: {
                deep: true,
                handler: function(newVal, oldVal) {
                    if(newVal){
                        if(this.sumModel){
                            this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 0);
                            this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 1);
                            this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 2);
                            this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 3);
                            this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 4);
                        } else {
                            this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 0);
                            this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 1);
                            this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 2);
                            this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 3);
                            this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 4);   
                        }
                    }
                }
            },
            */
            loading: {
                deep: true,
                handler: function(newVal, oldVal) {
                    if(newVal){
                        console.log("loading change");
                    }
                }
            },
            sumModel: {
                deep: true,
                handler: function(newVal, oldVal) {
                        console.log('sumModelChange:',newVal);
                        if(this.sumModel){
                            this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 0);
                            this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 1);
                            this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 2);
                            this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 3);
                            this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 4);
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
                    if(newVal.length == 1) {
                        for(var i in this.loading) {
                            this.loading[i] = true;
                        }
                        this.$store.commit('set_features_g1name', this.selected_subjects[0].data.name);
                        console.log('begin getPValueData');
                        NetService.getPValueDataForOneGroup(1, this.selected_subjects_name[0], (res, err) => {
                            if(err){
                                console.error(err);
                                return;
                            }
                            if(!res.data){
                                console.log('data not exists!');
                                return;
                            }
                            //{"code":0,"data":[[[0.1,0.2,0.3,0.4,0.5,"a"],[0.2,0.4,0.5,0.3,0.4,"b"],[0.1,0.4,0.7,0.4,0.5,"c"]],[[0.2,0.4,0.4,0.5,0.3,"d"],[0.4,0.3,0.1,0.4,0.5,"e"],[0.4,0.5,0.5,0.7,0.3,"f"]],[[0.4,0.5,0.2,0.4,0.2,"h"],[0.4,0.5,0.4,0.3,0.7,"i"],[0.4,0.5,0.5,0.7,0.7,"j"]]],"message":"获取成功", 'subjectList':[]}
                            console.log('end getPValueData');
                            this.$store.commit('set_subjectListOne', res.data.subjectList);
                            this.$store.commit('set_groupNameOne', this.selected_subjects_name[0]);
                            var up = res.data.data;
                            var down = [[]];
                            console.log('up', up);
                            for(var i in up[0]) {
                                down[0].push([0, 0, 0, 0, 0, '']);
                            }
                            //转置
                            var tmpDataUp = up[0].map(function(col, i) {
                                return up.map(function(row) {
                                    return row[i];
                                })
                            });
                            this.pValueDataUp = [];
                            this.pValueDataBottom = [];
                            var tmpDataBottom = down[0].map(function(col, i) {
                                return down.map(function(row) {
                                    return row[i];
                                })
                            });
                            for(var i in tmpDataUp) {
                                if(this.self.indexOf(Number(i)) == -1) {
                                    this.pValueDataUp.push(tmpDataUp[i]);
                                    this.pValueDataBottom.push(tmpDataBottom[i]);
                                } 
                                /*else {
                                    if(tmpDataUp[i][0][6] != i) {
                                        console.log("error", tmpDataUp[i][0][6], i);
                                    }
                                    console.log("index", tmpDataUp[i][0][6], i);
                                }
                                */
                            }
                            console.log('changeup', this.pValueDataUp);
                            if(this.sumModel){
                                this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 0);
                                this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 1);
                                this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 2);
                                this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 3);
                                this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 4);
                            } else {
                                this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 0);
                                this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 1);
                                this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 2);
                                this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 3);
                                this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 4);   
                            }
                            this.hasLoad = true;
                        });
                    }
                    if(newVal.length == 2){
                        for(var i in this.loading) {
                            this.loading[i] = true;
                        }
                        console.log('begin getPValueData');
                        this.$store.commit('set_features_g1name', this.selected_subjects[0].data.name);
                        this.$store.commit('set_features_g2name', this.selected_subjects[1].data.name);
                        NetService.getPValueDataForOneGroup(2, this.selected_subjects_name[1], (res, err) => {
                            if(err){
                                console.error(err);
                                return;
                            }
                            if(!res.data){
                                console.log('data not exists!');
                                return;
                            }
                            //{"code":0,"data":[[[0.1,0.2,0.3,0.4,0.5,"a"],[0.2,0.4,0.5,0.3,0.4,"b"],[0.1,0.4,0.7,0.4,0.5,"c"]],[[0.2,0.4,0.4,0.5,0.3,"d"],[0.4,0.3,0.1,0.4,0.5,"e"],[0.4,0.5,0.5,0.7,0.3,"f"]],[[0.4,0.5,0.2,0.4,0.2,"h"],[0.4,0.5,0.4,0.3,0.7,"i"],[0.4,0.5,0.5,0.7,0.7,"j"]]],"message":"获取成功"}
                            console.log('end getPValueData');
                            this.$store.commit('set_subjectListTwo', res.data.subjectList);
                            this.$store.commit('set_groupNameTwo', this.selected_subjects_name[1]);
                            var down = res.data.data;
                            //转置
                            this.pValueDataBottom = [];
                            var tmpDataBottom = down[0].map(function(col, i) {
                                return down.map(function(row) {
                                    return row[i];
                                })
                            });
                            for(var i in tmpDataBottom) {
                                if(this.self.indexOf(Number(i)) == -1) {
                                    this.pValueDataBottom.push(tmpDataBottom[i]);
                                }
                            }
                            if(this.sumModel){
                                this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 0);
                                this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 1);
                                this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 2);
                                this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 3);
                                this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 4);
                            } else {
                                this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 0);
                                this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 1);
                                this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 2);
                                this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 3);
                                this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 4);   
                            }
                            this.hasLoad = true;
                        });
                        /*
                        NetService.getPValueData(this.selected_subjects_name, (res, err) => {
                            if(err){
                                console.error(err);
                                return;
                            }
                            if(!res.data){
                                console.log('data not exists!');
                                return;
                            }
                            //console.log('getPValueData');
                            //{"code":0,"dataGroupOne":[[[0.1,0.2,0.3,0.4,0.5,"a"],[0.2,0.4,0.5,0.3,0.4,"b"],[0.1,0.4,0.7,0.4,0.5,"c"]],[[0.2,0.4,0.4,0.5,0.3,"d"],[0.4,0.3,0.1,0.4,0.5,"e"],[0.4,0.5,0.5,0.7,0.3,"f"]],[[0.4,0.5,0.2,0.4,0.2,"h"],[0.4,0.5,0.4,0.3,0.7,"i"],[0.4,0.5,0.5,0.7,0.7,"j"]]],"dataGroupTwo":[[[0.2,0.4,0.1,0.2,0.3,"a"],[0.2,0.3,0.2,0.4,0.4,"b"],[0.1,0.4,0.7,0.2,0.4,"c"]],[[0.2,0.4,0.3,0.2,0.4,"d"],[0.4,0.3,0.1,0.2,0.4,"e"],[0.5,0.7,0.3,0.2,0.4,"f"]],[[0.2,0.4,0.2,0.4,0.2,"h"],[0.4,0.3,0.2,0.4,0.7,"i"],[0.5,0.7,0.7,0.2,0.4,"j"]],[[0.2,0.4,0.7,0.2,0.4,"k"],[0.4,0.3,0.2,0.4,0.7,"l"],[0.5,0.7,0.7,0.2,0.4,"m"]]],"subjectsArr":["all-healthy-female","all-healthy-male"],"message":"\u83b7\u53d6\u6210\u529f"}
                            console.log('end getPValueData');
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
                                this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 0);
                                this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 1);
                                this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 2);
                                this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 3);
                                this.setOptionSumLineThree(this.pValueDataUp, this.pValueDataBottom, 4);
                            } else {
                                this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 0);
                                this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 1);
                                this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 2);
                                this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 3);
                                this.setOptionSub(this.pValueDataUp, this.pValueDataBottom, 4);   
                            }
                            this.hasLoad = true;
                        });
                        */
                        /*
                        NetService.update2D(this.pValueThreshold, this.validThreshold, this.testVal,this.lambda, this.ROIZValue, this.DBScanRadius, (res, err) => {
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
                                this.$store.commit('set_features_data_multidata', this.multiVal);
                                
                            });*/
                    }
                }
            }
        }
    }
</script>

<style>
</style>