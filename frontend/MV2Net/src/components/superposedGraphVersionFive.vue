<!--叠加图，strength从外部绑定 正负和粗细都是strength.data，原始数据为strength.origindata-->
<template>
    <div style="text-align: left; padding-bottom: 5px;">
        <div :class='setClass()' style="height:50%;">  
            <div style="float: left; z-index: 999; width: 100%; text-align: center; padding-left: 5px; border-bottom: 1px solid rgba(0,0,0,0.05);">
                <div style="padding-left: 16px;">
                    <Label style="padding-right: 0px; color: white;">{{showFunction == 0? "Univariate view:" : (g1name + " Group:")}}</Label>
                    <label class="myNum" style="width: 26px;">{{filterdataLen[0]}}</label>
                </div>
            </div>
            <div style="position: absolute; top: 30px; padding-left: 4px; z-index: 9; ">
                <div style="padding-bottom: 2px;">{{featureName ? (featureName + ": " + Func) : ""}}</div>
                <div>
                    <input class="colorcheckbox" type="checkbox" @click="changePurple()" />
                    <span :class="{'colorSpan':true, 'purpleCircle':true, 'hasOk': showPurple}">
                        <span class="f-full"></span>
                    </span>
                    <span style="vertical-align: super; margin-left: 2px;">
                        {{g1name}}
                    </span>
                </div>
                <div v-if="sidebyside == 0 || Func == 'p-value'">
                    <input class="colorcheckbox" type="checkbox" @click="changeOrange()" />
                    <span :class="{'colorSpan':true, 'orangeCircle':true, 'hasOk': showOrange}">
                        <span class="f-full"></span>
                    </span>
                    <span style="vertical-align: super; margin-left: 2px;">
                        {{g2name}}
                    </span>
                </div>
            </div>
            <!--div style="position: absolute; top: 235px; padding-left: 10px; z-index: 999;">
                <div>
                    <input class="colorcheckbox" type="checkbox" @click="changeEyeSide()" />
                    <span :class="{'colorSpan':true, 'grayCircle':true}">
                        <span style="color: white;">{{eyeside == 0 ? "U" : (eyeside == 1 ? "S" : "B")}}</span>
                    </span>
                </div>
            </div-->
            <div class="tipsDiv" style="z-index: 4; color: black; font-size: 10px; text-align: left;  position: absolute;visibility: hidden; height: 0px;"></div>
            <div class="leftSVG" style="padding-left: 20px; padding-top: 0px; min-height: 241px;">
                
            </div>
            <div class="panel" ref="myPanel" style="overflow: visible; width: auto; font-size: 12px; float: right; margin-right: 49px;">
                <!--div style="padding-bottom: 20px; margin-left: 55%;">
                    <SwitchButton v-model="reverse" onText="Forward" offText="Backward" style="width:100px"></SwitchButton>
                </div-->
                <div style="float: left; margin-top: -11px; width: auto; text-align: center; font-size: 14px; padding-right: 8px; line-height: 16px;">{{algorithm}}<br>{{Func ? (Func + ":") : ""}}</div>
                <div style="width: 95px; float: left;">
                    <reverse-slider :min=0 :max=100 v-model="filterRate[0]" :dataMin="thick_range.r1" :dataMax="thick_range.r2" :dataTrueMax="thick_range.r3" :ispvalue="Func == 'p-value' ? 1 : 0" :style="{'width': '95px'}"></reverse-slider>
                </div>
                <!--div>
                    <div style="float: left; width: 40%; text-align: left;">
                        <span>{{origindata_range.r1.toFixed(2)}}</span>
                    </div>
                    <div style="float: left; width: 60%; text-align: right;">
                        <span>{{origindata_range.r2.toFixed(2)}}</span>
                    </div>
                </div-->
                <!--Slider style="width:200px;margin: auto;"
                    v-model="filterRate"
                    :showTip="true"
                    :range="true"
                    :rule="[0,'|',25,'|',50,'|',75,'|',100]">
                </Slider-->
                <!--div class="text1" style="text-align: center;padding-top: 20px;"></div-->
            </div>
            <!--<div style="float:left;" id="rightSVG"></div>-->
        </div>
    </div>
</template>

<style scoped>
    .navbar-brand {
        padding-left: 1%;
        font-size: x-large;
        /* font-weight: bold; */
    }
</style>

<script>
    import * as d3 from "d3"
    //import strength from "../../data/strength"
    import originmatrix from "../../data/matrix"
    //import backgroundPic from "../assets/brain_xy.png"
    import slider from './slider'
    import reverseSlider from "./reverseSlider"

    //import $ from "jquery"
    export default {
        name: 'superposedGraphVersionFive',
        components: {
            slider,
            reverseSlider,
        },
        props: {
            //p-value
            strength: {
                required: false,
                type: Object,
            },
            //filterdata
            filterdata: {
                require: false,
                type: Array,
            },
            filterdataLen: {
               // required: true,
                type: Array,
            },
            //阈值低于该值时不显示,绝对不显示
            filterThreshold:{
                required: false,
                type: Number,
                default: 0
            },
            index: {
                required: false,
                type: String,
                default: "0"
            },
            heightPer: {
                required: false,
                type: Number,
                default: 0.4,
            },
            featureName: {
                required: true,
                type: String,
                default: "",
            },
            sidebyside: {
                required: false,
                type: Number,
                default: 0,
            },
            sidebysideMax: {
                required: false,
                type: Number,
                default: 0,
            },
            sidebysideTrueMax: {
                required: false,
                type: Number,
                default: 0,
            },
            sidebysideMin: {
                required: false,
                type: Number,
                default: 0,
            },
            Func: {
                required: false,
                type: String,
                default: "",
            },
            eyeside: {
                required: true,
                type: String,
                default: "0", //0从上往下，1旁边，2背后
            },
            g1name: {
                required: true,
                type: String,
                default: "", //0从上往下，1旁边，2背后
            },
            g2name: {
                required: true,
                type: String,
                default: "", //0从上往下，1旁边，2背后
            },
            selected: {
                required: false,
                type: Boolean
            },
            filterRate: {
                required: false,
                type: Array,
                //default:[0, 100],
            },
            showFunction: {
                required: false,
                type: String
            },
            algorithm: {
                required: false,
                type: String,
                default: "",
            },
        },
        data() {
            return {
                //strength,
                matrix: {},
                backgroundPicxy: require("../assets/brain_xy.png"),
                backgroundPicyz: require("../assets/brain_yz.png"),
                backgroundPicxz: require("../assets/brain_xz.png"),
                isFocus: false,
                //filterRate: [0, 100],
                weight_range: {
                    'r1' : 0,
                    'r2' : 0,
                },
                thick_range: {
                    'r1' : 0,
                    'r2' : 0,
                    'r3': 0,
                },
                origindata_range: {
                    'r1' : 0,
                    'r2' : 0,
                },
                reverse: true,
                showPurple: true,
                showOrange: true,
                x_range: {},
                y_range: {},
                z_range: {},
                width: 0,
                height: 0,
                //eyeside: "0",
            }
        },
        created() {
            this.matrix = this.deepCopy(originmatrix);
        },
        //mounted在html渲染出来后进行操作
        mounted() {
            if(this.strength.hasOwnProperty('data'))
                this.drawSidBySidePic();
        },
        methods: {
            changePurple() {
                this.showPurple = !this.showPurple;
            },
            changeOrange() {
                this.showOrange = !this.showOrange;
                
            },
            changeEyeSide() {
                //this.eyeside = Math.floor((Number(this.eyeside) + 1) % 3).toString();
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
            //不同的class
            setClass() {
                let obj = {};
                var classname = 'draw-area-' + this.index; 
                obj[classname] = true;
                //console.log('class:', obj);
                return obj;
            },
            //根据node的序号找到所在clusters的哪一行
            clusterNo(node, clusters)
            {
                for(var a in clusters){
                    for(var b in clusters){
                        if(Number(node) == clusters[a][b]){
                            return Number(a);
                        }
                    }
                }
                return null;
            },
            //颜色填充
            SaturationFillScaleRed(weight, maxWeight) {
                var s = weight / maxWeight;
                return d3.hsl(0, s, 0.5);
            },
            SaturationFillScaleGreen(weight, maxWeight) {
                var s = weight / maxWeight;
                return d3.hsl(120, s, 0.5);
            },
            LightnessFillScaleRed(weight, maxWeight) {
                var i = weight / maxWeight;
                return d3.hsl(0, 1, i);
            },
            LightnessFillScaleGreen(weight, maxWeight) {
                var i = weight / maxWeight;
                return d3.hsl(120, 1, i);
            },
            fillScaleRed(weight, maxWeight) {//weight越大越红
                var g = 255 * (1 - weight / maxWeight);
                return d3.rgb(255, g, g).toString();
            },
            fillScaleGreen(weight, maxWeight) {
                var g = 255 * (1 - weight / maxWeight);
                return d3.rgb(g, 255, g).toString();
            },
            fillScaleBlue(weight, maxWeight) {
                var g = 255 * (1- weight / maxWeight);
                return d3.rgb(g,g,255).toString();
            },
            fillScalePurple(weight, maxWeight) {
                if(maxWeight == 0) {
                    var g = 1;
                    return d3.rgb(205 - g * 138, 197 - g * 165, 224 - g * 85).toString();
                }
                var g = (weight / maxWeight);
                return d3.rgb(205 - g * 138, 197 - g * 165, 224 - g * 85).toString();
            },
            fillScaleOrange(weight, maxWeight) {
                if(maxWeight == 0) {
                    var g = 1;
                    return d3.rgb(254 - g * 5, 219 - g * 139, 192 - g * 192).toString();
                }
                var g = (weight / maxWeight);
                return d3.rgb(254 - g * 5, 219 - g * 139, 192 - g * 192).toString();
                //return d3.hsl(18, 0.94, 0.46*g);
            },
            fillScaleOne(weight, maxWeight) {
                return this.fillScalePurple(weight, maxWeight);
            },
            fillScaleTwo(weight, maxWeight) {
                return this.fillScaleOrange(weight, maxWeight);
            },
            getQueryString(name){
                var reg = new RegExp("(^|g)" + name + "=([^g]*)(&|$)");
                var r = window.location.search.substr(1).match(reg);
                if (r != null) {
                    return unescape(r[2]);
                }
                return null;
            },
            count(o){
                var t = typeof o;
                if (t == 'string') {
                    return o.length;
                }
                else if (t == 'object') {
                    var n = 0;
                    for (var i in o) {
                        n++;
                    }
                    return n;
                }
                return false;
            },
            drawSidBySidePic(){
                //先清空画布
                var s = ".draw-area-" + this.index;
                //console.log(d3.select(this.$el).select(s));
                d3.select(this.$el)         
                    .select(s)    /*返回第一个匹配的元素*/
                    .select(".leftSVG")
                    .selectAll("svg")     /* selectAll  选择在文档遍历次序中所有匹配的元素 */
                    .remove();

                    /*
                d3.select(this.$el)         
                    .select(s)
                    .select(".panel")
                    .select(".text1")
                    .remove();*/

                //0-69 70个ROI
                var clusters = [
                    [3,12,14,17,18,19,20,24,27,28,32],
                    [38,47,49,52,53,54,55,59,62,63,67],//frontal颚骨
                    [8,21,22,25,29,31],
                    [43,56,57,60,64,66],//parietal顶骨
                    [5,11,13],
                    [40,46,48],//occipital枕骨
                    [1,6,7,9,15,30,33,34],
                    [36,41,42,44,50,65,68,69],//temporal颞骨 
                    [2,10,16,23,26],
                    [37,45,51,58,61],//limbic边缘
                    [0,35],
                    [4,39]
                ];

                //console.log("clusterNo " + this.clusterNo(0, clusters));

                var height = window.screen.height * this.heightPer;
                var width = height * 905 / 1030;
                this.height = height;
                this.width = width;
                
                //构造一个新的序数比例尺，有20种类型的颜色
                //var color = d3.scaleOrdinal(d3.schemeCategory20);
                var color = d3.scale.category20();
                //console.log(color);

                //使用物理模拟排放链接节点的位置
                //电荷强度、链接强度、链接距离、重力大小、布局大小
                //var forceLeft = d3.forceSimulation();
                //var forceRight = d3.forceSimulation();

                //添加svg
                var svgLeft = d3.select(this.$el)         
                    .select(s)
                    .select(".leftSVG")
                    .append("svg")
                    .attr("width", width)
                    .attr("height", height)
                    .style('z-index', 99);

                /*
                var svgLegend = d3.select(this.$el)         
                    .select(s)
                    .select(".panel")
                    .append("svg")
                    .attr("width", 100 * width / 905)
                    .attr("height", 40 * height / 1030)
                    .append("g")
                */

                var panel = this.$refs.myPanel;

                //var weights; //定义权重
                var indexWeight = {}; //记录权重
                var indexThickness = {}; //记录粗细
                var sign = {}; //记录正负
                var selectedNodes = {}; //eg:{1:1,2:0}
                var left_range, right_range;
                var x_range = {"x1":1000, "x2":0};
                var y_range = {"y1":1000, "y2":0};
                var z_range = {"z1":1000, "z2":0};
                var fillMax = 0, fillMin = 0;
                var filter_value = 0;  //weight低于该值则不显示
                var origindata = {};
                var that = this;

                //console.log('debug', this.strength);
                //console.log(this.strength.pvalue[0]);
                //读取weight，thickness
                for(var i = 0; i < this.strength.data.length; i++) {
                    //indexWeight[i] = this.strength.pvalue[i];
                    indexThickness[i] = Math.abs(this.strength.data[i]);
                    sign[i] = this.strength.data[i];
                    origindata[i] = this.strength.origindata[i];
                }
                //console.log('indexThickness', indexThickness[0]);

                //var this.matrix.nodes = 
                //读取this.matrix里的node,节点位置，确定值域范围x_range, y_range, z_range
                this.matrix.nodes.forEach(function(d){
                    x_range.x1 = x_range.x1 < d.x ? x_range.x1 : d.x;
                    x_range.x2 = x_range.x2 > d.x ? x_range.x2 : d.x;
                    y_range.y1 = y_range.y1 < d.y ? y_range.y1 : d.y;
                    y_range.y2 = y_range.y2 > d.y ? y_range.y2 : d.y;
                    z_range.z1 = z_range.z1 < d.z ? z_range.z1 : d.z;
                    z_range.z2 = z_range.z2 > d.z ? z_range.z2 : d.z;
                });
                this.x_range = x_range;
                this.y_range = y_range;
                this.z_range = z_range;

                //增添一属性用来记录右图的roi连接及对应强度
                //this.matrix.linksRight = new Array();
                //range记录weight\thick的最大最小值
                var range = {"r1":10000, "r2":-10000};
                var rangeThick = {"r1":10000, "r2":-10000};
                var rangeOrigin = {"r1":10000, "r2":-10000};
                //将weight记录进links，左图roi连接及对应强度，并计算range。source < target上三角矩阵
                this.matrix.links.forEach(function(d,i) {
                    var tmp = {};
                    tmp.target = d.target;
                    tmp.source = d.source;
                    //自己到自己以及无weight的情况皆为0
                    if (d.target == d.source) {
                        //d.weight = 0;
                        d.thick = 0;
                        d.sign = 0;
                        d.origindata = origindata[i];
                    } else {
                        //d.weight = indexWeight[i];
                        //range.r1 = Math.min(range.r1,indexWeight[i]);
                        //range.r2 = Math.max(range.r2,indexWeight[i]);

                        d.thick = indexThickness[i];
                        rangeThick.r1 = Math.min(rangeThick.r1, indexThickness[i]);
                        rangeThick.r2 = Math.max(rangeThick.r2, indexThickness[i]);
                        d.sign = sign[i];

                        d.origindata = origindata[i];
                        rangeOrigin.r1 = Math.min(rangeOrigin.r1, Math.abs(origindata[i]));
                        rangeOrigin.r2 = Math.max(rangeOrigin.r2, Math.abs(origindata[i]));
                    }
                    //in指示weight的index
                    d.in = i;
                    tmp.in = i;
                });
                //console.log(this.matrix.links[1]);
                //console.log(this.matrix.linksRight[1]);
                //left_range = range;
                //right_range = range;
                //this.weight_range = range; //权重的最大最小值
                //filter_value = (range.r2 - range.r1) * this.filterThreshold + range.r1;
                //fillMax = this.weight_range.r2;
                //fillMin = this.weight_range.r1;
                //修改为大小的百分之5，百分之95
                this.thick_range = rangeThick;

                if(this.sidebyside !== 0) {
                    this.thick_range.r1 = this.sidebysideMin;
                    this.thick_range.r2 = this.sidebysideMax;
                    this.thick_range.r3 = this.sidebysideTrueMax;
                }
                else {
                    var sortthick = [];
                    this.matrix.links.forEach(function(d,i) {
                        if (d.target == d.source) {

                        } else {
                            if(d.thick > 0) {
                                sortthick.push(d.thick);
                            }
                        }
                    });
                    /*
                    for(var i in indexThickness) {
                        if(indexThickness[i] > 0) {
                            sortthick.push(indexThickness[i]);
                        }
                    }
                    */
                    if(sortthick.length == 0) {
                        this.thick_range.r1 = 0;
                        this.thick_range.r2 = 0;
                        this.thick_range.r3 = 0;
                    }
                    else {
                        sortthick.sort(function(x1,x2){return x1 - x2;});
                        var minindex = Math.floor(sortthick.length * 0.01);
                        if(minindex < 0) minindex = 0;
                        var maxindex = Math.floor(sortthick.length * 0.99);
                        if(maxindex >= sortthick.length) maxindex = sortthick.length - 1;
                        this.thick_range.r1 = sortthick[minindex];
                        this.thick_range.r2 = sortthick[maxindex];
                        this.thick_range.r3= sortthick[sortthick.length - 1];
                        //console.log("sortthick", sortthick);
                    }
                      
                }
                

                fillMin = this.filterRate[0] * (this.thick_range.r2 - this.thick_range.r1) / 100 + this.thick_range.r1;
                fillMax = this.filterRate[1] * (this.thick_range.r2 - this.thick_range.r1) / 100 + this.thick_range.r1;

                var countfilter = 0;
                //记录filter后的数据
                if(this.filterRate[0] == 0) {
                    //未过滤
                    that.matrix.links.forEach(function(d,i) {
                        if (d.target == d.source) {
                            that.filterdata[i] = 0;
                        } else {
                            if(d.thick > 0 && ((that.showPurple && d.sign >= 0) || (that.showOrange && d.sign < 0)) ) {
                                if(d.sign >= 0)
                                    that.filterdata[i] = d.thick;
                                else that.filterdata[i] = -d.thick;
                                countfilter++;
                            }
                            else {
                                that.filterdata[i] = 0;
                            }
                        }
                    });
                    /*
                    for(var i = 0; i < this.strength.data.length; i++) {
                        this.filterdata[i] = this.strength.data[i];
                    }
                    */
                }
                else {
                    that.matrix.links.forEach(function(d,i) {
                        if (d.target == d.source) {
                            that.filterdata[i] = 0;
                        } else {
                            if(d.thick >= fillMin && ((that.showPurple && d.sign >= 0) || (that.showOrange && d.sign < 0))) {
                                if(d.sign >= 0)
                                    that.filterdata[i] = d.thick;
                                else that.filterdata[i] = -d.thick;
                                countfilter++;
                            }
                            else {
                                that.filterdata[i] = 0;
                            }
                        }
                    });
                }

                that.filterdataLen[0] = countfilter;
                console.log("filterdataLen", that.filterdataLen);

                this.origindata_range = rangeOrigin;

                svgLeft.append("image").attr("class","background")
                    .attr("x", 0).attr("y", 0)
                        .attr("width", width)
                        .attr("height",height);
                

                //使用物理模拟排放链接节点的位置
                //电荷强度、链接强度、链接距离、重力大小、布局大小
                //
                var forceLeft = d3.layout.force()
                    .charge(0)
                    .linkStrength(0)
                    .linkDistance(0)
                    .gravity(0)
                    .size([width, height]);

                
                forceLeft
                    .nodes(this.matrix.nodes)
                    .links(this.matrix.links)
                    .start();


                var tipsDiv = d3.select(this.$el)         
                    .select(s)    /*返回第一个匹配的元素*/
                    //.select('.leftSVG')
                    .select(".tipsDiv");

                
                //添加线
                var linkLeft = svgLeft.selectAll(".link")
                    .data(that.matrix.links)
                    .enter()
                    .append("line")
                    .attr("class", function(d,i) {
                        return "leftl" + that.clusterNo(d.source.index, clusters) + "l" + that.clusterNo(d.target.index, clusters) + "leftl" + that.clusterNo(d.source.index, clusters) + "leftl" + that.clusterNo(d.target.index, clusters) +" link";
                    })
                    .style("stroke-width", function(d) {
                        if (that.reverse){
                            if (d.thick == 0) return 0;
                            return 1;
                            if(Math.abs(d.thick) > that.thick_range.r2) return 4;
                            if(Math.abs(d.thick) < that.thick_range.r1) return 1;
                            return (Math.abs(d.thick) - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 3 + 1;
                            /*
                            if (d.thick >= fillMin && d.thick <= fillMax){
                                if (d.thick == 0) return 0;
                                if (that.showPurple && d.sign >= 0) {
                                    return (Math.abs(d.thick) - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 3 + 1;
                                }
                                else if (that.showOrange && d.sign < 0) {
                                    return (Math.abs(d.thick) - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 3 + 1;
                                }
                                else {
                                    return 0;
                                }
                            }
                            else {
                                return 0;
                            }*/
                        }
                        else return 4 - (d.thick - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 4 
                    })  //路径宽度
                    .style("stroke", function(d){//路径颜色
                        if(d.sign >= 0){
                            var thicktmp = d.thick;
                            if(Math.abs(d.thick) > that.thick_range.r2) thicktmp = that.thick_range.r2;
                            if(Math.abs(d.thick) < that.thick_range.r1) thicktmp = that.thick_range.r1;
                            return that.fillScaleOne(thicktmp -  that.thick_range.r1,  that.thick_range.r2 -  that.thick_range.r1);
                        } else {
                            var thicktmp = d.thick;
                            if(Math.abs(d.thick) > that.thick_range.r2) thicktmp = that.thick_range.r2;
                            if(Math.abs(d.thick) < that.thick_range.r1) thicktmp = that.thick_range.r1;
                            return that.fillScaleTwo(thicktmp -  that.thick_range.r1,  that.thick_range.r2 -  that.thick_range.r1);
                        }
                        /*
                        if(d.thick >= fillMin && d.thick <= fillMax){
                            if(d.sign >= 0){
                                return that.fillScaleOne(d.thick - fillMin, fillMax - fillMin);
                                //return that.SaturationFillScaleRed(d.weight - fillMin, fillMax - fillMin);
                                //return that.LightnessFillScaleRed(d.weight - fillMin, fillMax - fillMin);
                            } else {
                                return that.fillScaleTwo(d.thick - fillMin, fillMax - fillMin);   
                                //return that.SaturationFillScaleGreen(d.weight - fillMin, fillMax - fillMin);
                                //return that.LightnessFillScaleGreen(d.weight - fillMin, fillMax - fillMin);
                            }
                        } else {
                            return d3.rgb(255, 255, 255).toString();
                        }*/
                    })
                    .style("stroke-opacity", function(d){ //不透明度
                        if(that.filterRate[0] == 0) {
                            //未过滤的thick大于1的显示
                            if(d.thick > 0) {
                                if (that.showPurple && d.sign >= 0) {
                                    return 1;
                                }
                                else if (that.showOrange && d.sign < 0) {
                                    return 1;
                                }
                                else {
                                    return 0;
                                }
                            }
                            else {
                                return 0;
                            }
                        }
                        else {
                            if(d.thick >= fillMin) {
                                if (that.showPurple && d.sign >= 0) {
                                    return 1;
                                }
                                else if (that.showOrange && d.sign < 0) {
                                    return 1;
                                }
                                else {
                                    return 0;
                                }
                            }
                            else {
                                return 0;
                            }
                        }
                    })
                    .sort(function(a, b) {
                        return a.thick - b.thick;
                    });


                //添加线(热线)
                var linkLeft2 = svgLeft.selectAll(".link2")
                    .data(that.matrix.links)
                    .enter()
                    .append("line")
                    .attr("class", function(d,i) {
                        return "leftl" + that.clusterNo(d.source.index, clusters) + "l" + that.clusterNo(d.target.index, clusters) + "leftl" + that.clusterNo(d.source.index, clusters) + "leftl" + that.clusterNo(d.target.index, clusters) +" link2";
                    })
                    .style("stroke-width", function(d) {
                        if (that.reverse){
                            var fmin;
                            if(that.filterRate[0] == 0) {
                                fmin = 0;
                            }
                            else {
                                fmin = fillMin;
                            }
                            if (d.thick >= fmin){
                                if (d.thick == 0) return 0;
                                if (that.showPurple && d.sign >= 0) {
                                    return 4;
                                }
                                else if (that.showOrange && d.sign < 0) {
                                    return 4;
                                }
                                else {
                                    return 0;
                                }
                            }
                            else {
                                return 0;
                            }
                        }
                        else return 4 - (d.thick - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 4 
                    })  //路径宽度
                    .style("stroke", function(d){//路径颜色
                        return d3.rgb(0, 0, 0).toString();
                    })
                    .style("stroke-opacity", function(d){ //不透明度
                        return 0;
                    })
                    .sort(function(a, b) {
                        return a.thick - b.thick;
                    })
                    //添加hover
                    .on("mouseenter", function(d, i) {
                        //console.log('mouseenter', d);
                        //两端点变红
                        svgLeft.selectAll(".node").filter(function(d1, i1){
                            return d1.index == d.target.index || d1.index == d.source.index;
                        }).select(".Circle").style("stroke", "red");
                        return tipsDiv
                            .style('visibility', 'visible')
                            //.style('left', (d3.event.clientX - 70) + 'px') //- 400
                            //.style('top', (d3.event.clientY - 390) + 'px') //- 110
                            //.style('left', '10px') //14
                            .style('padding-left', '7px')
                            .style('top', '228px') //224
                            .style('height', '36px')
                            .style('background', 'white')
                            .html('ROI: ' + (d.source.index + 1) + '  <->  ' + (d.target.index + 1) + '<br>' + that.Func + ': ' + d.origindata.toFixed(5));//tooltip.html(text);
                    })
                    .on("mouseout", function(d, i) {
                        svgLeft.selectAll(".node").filter(function(d1, i1){
                            return d1.index == d.target.index || d1.index == d.source.index;
                        }).select(".Circle").style("stroke", "black");
                        //return tooltip.style('visibility', 'hidden');
                        return tipsDiv.style('visibility', 'hidden');
                    })
                    //添加点击事件
                    .on("click", function(d){
                    });                
                

                //添加节点
                var nodeLeft = svgLeft.selectAll(".node")
                    .data(this.matrix.nodes)
                    .enter()
                    .append("g")
                    .attr("class", "node")
                    .attr("id", function(d,i){
                        return "index"+that.index+"node"+i;
                    })
                    .on("click", function(d){
                        
                    });
                    /*
                    .on("mouseenter", function(d, i) {
                       this.firstElementChild.setAttribute('r', 9);
                       this.lastElementChild.style.fontSize = "15px";
                    })
                    .on("mouseout", function(d, i) {
                       this.firstElementChild.setAttribute('r', 2);
                       this.lastElementChild.style.fontSize = "0px";
                    });
                    */

                    /*
                    svgLeft.selectAll(".node")
                                    .filter(function(d1, i){
                                        return selectedNodes[i] == 1;
                                    })
                                    .select("circle")
                                    .style("fill", "red");
                    */
                //添加图标显示节点
                nodeLeft.append("circle")
                    .attr("class", function(d,i){
                        return "lobe" + that.clusterNo(i, clusters) + "Circle" + that.index + " Circle";
                    })
                    .attr("r", 2) //圆圈大小
                    .style("fill", "white")
                    .style("stroke", "black") //边框颜色
                    .style("stroke-width", 1); //边框厚度
                    

                //文字
                nodeLeft.append("text")
                    .attr("class", function(d,i){
                        return "lobe" + that.clusterNo(i, clusters) + "Text" + that.index;
                    })
                    .attr("font-family", "Arial")
                    .attr("y", 6)
                    .style("text-anchor", "middle")
                    .style("stroke-width", 0)
                    .style("fill", "black")
                    .style("font-size", "0px") //字体大小
                    .style("font-weight", 300)
                    .text(function(d) {
                        return d.index + 1;
                    });

                //添加热点
                nodeLeft.append("circle")
                    .attr("class", function(d,i){
                        return "lobe" + that.clusterNo(i, clusters) + "Circlehot" + that.index;
                    })
                    .attr("r", 5)
                    .style("fill", "white")
                    .style("stroke", "black")
                    .style("stroke-width", 1)
                    .style("opacity", 0)
                    .on("mouseenter", function(d, i) {
                        var selectnode = nodeLeft.filter(function(d1){
                            return d.index == d1.index;
                        });
                        selectnode.selectAll(".Circle").style("r", 10).style("stroke","red");
                        selectnode.select("text").style("font-size", "14px").style("fill","red");
                        
                        var fmin;
                        if(that.filterRate[0] == 0) {
                            //未过滤的thick大于1的显示
                            fmin = 0;
                        }
                        else {
                            fmin = that.filterRate[0] * (that.thick_range.r2 - that.thick_range.r1) / 100 + that.thick_range.r1;;
                        }
                        var nodelinknum = 0;
                        that.matrix.links.filter(function(d1){
                            return d1.thick >= fmin && d1.thick != 0 && ((that.showPurple && d1.sign >= 0) || (that.showOrange && d1.sign < 0)) && (d1.target.index == d.index || d1.source.index == d.index);
                        }).forEach(function(d1){
                            nodelinknum++;
                        });

                        tipsDiv
                            .style('visibility', 'visible')
                            .style('padding-left', '7px')
                            .style('top', '228px')
                            .style('height', '36px')
                            .style('background', 'white')
                            .html('ROI: ' + (d.index + 1) + '<br>' + '#feature: ' + nodelinknum);//tooltip.html(text);
                    })
                    .on("mouseout", function(d, i) {
                        var selectnode = nodeLeft.filter(function(d1){
                            return d.index == d1.index;
                        });
                        selectnode.selectAll(".Circle").style("r", 2).style("stroke","black");
                        selectnode.select("text").style("font-size", "0px").style("fill","black");
                        return tipsDiv.style('visibility', 'hidden');
                    });

                if(that.eyeside == "0") {
                    var xScale = d3.scale.linear() //d3.scaleLinear()
                        .domain([0,x_range.x2])
                        .range([-100*width/905 + 20, width-100*width/905 + 20]);
                    var yScale = d3.scale.linear() //d3.scaleLinear()
                        .domain([0,y_range.y2])
                        .range([-70*height/1030, height-70*height/1030]);
                    //添加脑的背景图
                    //console.log(this.backgroundPic);
                    svgLeft.selectAll(".background")
                        .attr("xlink:href", this.backgroundPicxy)
                        .attr("x", 20).attr("y", 0)
                        .attr("width", width)
                        .attr("height",height);
                    //动态改变线和节点的位置
                    forceLeft.on("tick", function(){
                        linkLeft.attr("x1", function(d){
                            return xScale(d.source.x);
                        })
                        .attr("y1", function(d){
                            return yScale(d.source.y);
                        })
                        .attr("x2", function(d){
                            return xScale(d.target.x);
                        })
                        .attr("y2", function(d){
                            return yScale(d.target.y);
                        });
                        linkLeft2.attr("x1", function(d){
                            return xScale(d.source.x);
                        })
                        .attr("y1", function(d){
                            return yScale(d.source.y);
                        })
                        .attr("x2", function(d){
                            return xScale(d.target.x);
                        })
                        .attr("y2", function(d){
                            return yScale(d.target.y);
                        });

                        nodeLeft.attr("cx", function(d){
                            return xScale(d.x);
                        })
                        .attr("cy", function(d){
                            return yScale(d.y);
                        })
                        .attr("transform", function(d){
                            return "translate(" + xScale(d.x) + "," + yScale(d.y) +")";
                        });
                    
                    });
                }
                else if (that.eyeside == "1") {
                    var xScale = d3.scale.linear() //d3.scaleLinear()
                        .domain([y_range.y1, y_range.y2])
                        .range([55*width/905, width-55*width/905]);
                    var yScale = d3.scale.linear() //d3.scaleLinear()
                        .domain([z_range.z1, z_range.z2])
                        .range([40 + 632*width/760-160*height/1030, 40 + 160*height/1030]);
                    //添加脑的背景图
                    //console.log(this.backgroundPic);
                    svgLeft.selectAll(".background")
                        .attr("xlink:href", this.backgroundPicyz)
                        .attr("x", 0).attr("y", 40)
                        .attr("width", width)
                        .attr("height",632*width/760);
                    //动态改变线和节点的位置
                    forceLeft.on("tick", function(){
                        linkLeft.attr("x1", function(d){
                            return xScale(d.source.y);
                        })
                        .attr("y1", function(d){
                            return yScale(d.source.z);
                        })
                        .attr("x2", function(d){
                            return xScale(d.target.y);
                        })
                        .attr("y2", function(d){
                            return yScale(d.target.z);
                        });
                        linkLeft2.attr("x1", function(d){
                            return xScale(d.source.y);
                        })
                        .attr("y1", function(d){
                            return yScale(d.source.z);
                        })
                        .attr("x2", function(d){
                            return xScale(d.target.y);
                        })
                        .attr("y2", function(d){
                            return yScale(d.target.z);
                        });

                        nodeLeft.attr("cx", function(d){
                            return xScale(d.y);
                        })
                        .attr("cy", function(d){
                            return yScale(d.z);
                        })
                        .attr("transform", function(d){
                            return "translate(" + xScale(d.y) + "," + yScale(d.z) +")";
                        });
                    
                    });
                }
                else if(that.eyeside == "2") {
                    var xScale = d3.scale.linear() //d3.scaleLinear()
                        .domain([x_range.x1, x_range.x2])
                        .range([55*width/905,width - 55*width/905]);
                    var yScale = d3.scale.linear() //d3.scaleLinear()
                        .domain([z_range.z1, z_range.z2])
                        .range([50 + 507*width/633 - 30*height/1030, 50 + 30*height/1030]);
                    //添加脑的背景图
                    //console.log(this.backgroundPic);
                    svgLeft.selectAll(".background")
                        .attr("xlink:href", this.backgroundPicxz)
                        .attr("x", 0).attr("y", 50)
                        .attr("width", width)
                        .attr("height", 507*width/633);
                    //动态改变线和节点的位置
                    forceLeft.on("tick", function(){
                        linkLeft.attr("x1", function(d){
                            return xScale(d.source.x);
                        })
                        .attr("y1", function(d){
                            return yScale(d.source.z);
                        })
                        .attr("x2", function(d){
                            return xScale(d.target.x);
                        })
                        .attr("y2", function(d){
                            return yScale(d.target.z);
                        });
                        linkLeft2.attr("x1", function(d){
                            return xScale(d.source.x);
                        })
                        .attr("y1", function(d){
                            return yScale(d.source.z);
                        })
                        .attr("x2", function(d){
                            return xScale(d.target.x);
                        })
                        .attr("y2", function(d){
                            return yScale(d.target.z);
                        });

                        nodeLeft.attr("cx", function(d){
                            return xScale(d.x);
                        })
                        .attr("cy", function(d){
                            return yScale(d.z);
                        })
                        .attr("transform", function(d){
                            return "translate(" + xScale(d.x) + "," + yScale(d.z) +")";
                        });
                    
                    });
                }
               
                /*
                d3.select(this.$el)         
                    .select(s)
                    .select(".panel")
                    .select(".text1")
                    .html("Fill range: " + this.weight_range.r1 + "-" + this.weight_range.r2);
                */
            }
        },
        watch: {
            strength: {
                deep: true,
                handler: function(newVal, oldVal) {
                    if(newVal){
                        if(this.strength.hasOwnProperty('data')){
                            //console.log('strength', newVal);
                            this.drawSidBySidePic();
                            //this.$forceUpdate();
                        }
                    }
                }
            },
            filterRate: {
                deep: true,
                handler: function(newVal, oldVal) {
                    //console.log("filterRate", newVal);
                    //console.log(d3.select(this.$el));
                    var s = ".draw-area-" + this.index;
                    var svgLeft = d3.select(this.$el)      
                        .select(s).select(".leftSVG");
                    var that = this;
                    var fillMin = newVal[0] * (that.thick_range.r2 - that.thick_range.r1) / 100 + that.thick_range.r1;
                    var fillMax = newVal[1] * (that.thick_range.r2 - that.thick_range.r1) / 100 + that.thick_range.r1;
                    //记录filter后的数据
                    var countfilter = 0;
                    if(this.filterRate[0] == 0) {
                        //未过滤
                        that.matrix.links.forEach(function(d,i) {
                            if (d.target == d.source) {
                                that.filterdata[i] = 0;
                            } else {
                                if(d.thick > 0 && ((that.showPurple && d.sign >= 0) || (that.showOrange && d.sign < 0)) ) {
                                    if(d.sign >= 0)
                                        that.filterdata[i] = d.thick;
                                    else that.filterdata[i] = -d.thick;
                                    countfilter++;
                                }
                                else {
                                    that.filterdata[i] = 0;
                                }
                            }
                        });
                    }
                    else {
                        that.matrix.links.forEach(function(d,i) {
                            if (d.target == d.source) {
                                that.filterdata[i] = 0;
                            } else {
                                if(d.thick >= fillMin && ((that.showPurple && d.sign >= 0) || (that.showOrange && d.sign < 0))) {
                                    if(d.sign >= 0)
                                        that.filterdata[i] = d.thick;
                                    else that.filterdata[i] = -d.thick;
                                    countfilter++;
                                }
                                else {
                                    that.filterdata[i] = 0;
                                }
                            }
                        });
                    }
                    that.filterdataLen[0] = countfilter;

                    var fmin;
                    if(that.filterRate[0] == 0) {
                        //未过滤的thick大于1的显示
                        fmin = 0;
                    }
                    else {
                        fmin = fillMin;
                    }
                    console.log('fillMin', fmin);
                    svgLeft.selectAll(".link").filter(function(d){
                        return d.thick >= fmin && d.thick != 0 && ((that.showPurple && d.sign >= 0) || (that.showOrange && d.sign < 0));
                    }).style("stroke-opacity", 1);
                    svgLeft.selectAll(".link").filter(function(d){
                        return !(d.thick >= fmin && d.thick != 0 && ((that.showPurple && d.sign >= 0) || (that.showOrange && d.sign < 0)));
                    }).style("stroke-opacity", 0);

                    svgLeft.selectAll(".link2").filter(function(d){
                        return d.thick >= fmin && d.thick != 0 && ((that.showPurple && d.sign >= 0) || (that.showOrange && d.sign < 0));
                    }).style("stroke-width", 4);
                    svgLeft.selectAll(".link2").filter(function(d){
                        return !(d.thick >= fmin && d.thick != 0 && ((that.showPurple && d.sign >= 0) || (that.showOrange && d.sign < 0)));
                    }).style("stroke-width", 0);                    
                    
                    if(this.selected) {
                        this.$store.commit('setneedReDraw', true);
                    }
                }
            },
            reverse: {
                deep: true,
                handler: function(newVal, oldVal) {
                    //console.log("reverse", newVal);
                    var s = ".draw-area-" + this.index;
                    var svgLeft = d3.select(this.$el)         
                        .select(s).select(".leftSVG");
                    var that = this;
                    svgLeft.selectAll(".link")
                        .style("stroke-width", function(d) {
                            if (newVal)
                                return 1; //(d.thick - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 3 + 1;
                            else return 4 - (d.thick - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 4 
                        });
                }
            },
            showPurple: {
                deep: true,
                handler: function(newVal, oldVal) {
                    //console.log("filterRate", newVal);
                    //console.log(d3.select(this.$el));
                    var s = ".draw-area-" + this.index;
                    var svgLeft = d3.select(this.$el)      
                        .select(s).select(".leftSVG");
                    var that = this;
                    var fillMin = that.filterRate[0] * (that.thick_range.r2 - that.thick_range.r1) / 100 + that.thick_range.r1;
                    var fillMax = that.filterRate[1] * (that.thick_range.r2 - that.thick_range.r1) / 100 + that.thick_range.r1;
                    //记录filter后的数据
                    var countfilter = 0;
                    if(this.filterRate[0] == 0) {
                        //未过滤
                        that.matrix.links.forEach(function(d,i) {
                            if (d.target == d.source) {
                                that.filterdata[i] = 0;
                            } else {
                                if(d.thick > 0 && ((that.showPurple && d.sign >= 0) || (that.showOrange && d.sign < 0)) ) {
                                    if(d.sign >= 0)
                                        that.filterdata[i] = d.thick;
                                    else that.filterdata[i] = -d.thick;
                                    countfilter++;
                                }
                                else {
                                    that.filterdata[i] = 0;
                                }
                            }
                        });
                    }
                    else {
                        that.matrix.links.forEach(function(d,i) {
                            if (d.target == d.source) {
                                that.filterdata[i] = 0;
                            } else {
                                if(d.thick >= fillMin && ((that.showPurple && d.sign >= 0) || (that.showOrange && d.sign < 0))) {
                                    if(d.sign >= 0)
                                        that.filterdata[i] = d.thick;
                                    else that.filterdata[i] = -d.thick;
                                    countfilter++;
                                }
                                else {
                                    that.filterdata[i] = 0;
                                }
                            }
                        });
                    }
                    that.filterdataLen[0] = countfilter;
                    var fmin;
                    if(that.filterRate[0] == 0) {
                        //未过滤的thick大于1的显示
                        fmin = 0;
                    }
                    else {
                        fmin = fillMin;
                    }
                    console.log('fillMin', fmin);
                    svgLeft.selectAll(".link").filter(function(d){
                        return d.thick >= fmin && d.thick != 0 && ((that.showPurple && d.sign >= 0) || (that.showOrange && d.sign < 0));
                    }).style("stroke-opacity", 1);
                    svgLeft.selectAll(".link").filter(function(d){
                        return !(d.thick >= fmin && d.thick != 0 && ((that.showPurple && d.sign >= 0) || (that.showOrange && d.sign < 0)));
                    }).style("stroke-opacity", 0);

                    svgLeft.selectAll(".link2").filter(function(d){
                        return d.thick >= fmin && d.thick != 0 && ((that.showPurple && d.sign >= 0) || (that.showOrange && d.sign < 0));
                    }).style("stroke-width", 4);
                    svgLeft.selectAll(".link2").filter(function(d){
                        return !(d.thick >= fmin && d.thick != 0 && ((that.showPurple && d.sign >= 0) || (that.showOrange && d.sign < 0)));
                    }).style("stroke-width", 0);

                    if(this.selected) {
                        this.$store.commit('setneedReDraw', true);
                    }
                }
            },
            showOrange: {
                deep: true,
                handler: function(newVal, oldVal) {
                    //console.log("filterRate", newVal);
                    //console.log(d3.select(this.$el));
                    var s = ".draw-area-" + this.index;
                    var svgLeft = d3.select(this.$el)      
                        .select(s).select(".leftSVG");
                    var that = this;
                    var fillMin = that.filterRate[0] * (that.thick_range.r2 - that.thick_range.r1) / 100 + that.thick_range.r1;
                    var fillMax = that.filterRate[1] * (that.thick_range.r2 - that.thick_range.r1) / 100 + that.thick_range.r1;
                    //记录filter后的数据
                    var countfilter = 0;
                    if(this.filterRate[0] == 0) {
                        //未过滤
                        that.matrix.links.forEach(function(d,i) {
                            if (d.target == d.source) {
                                that.filterdata[i] = 0;
                            } else {
                                if(d.thick > 0 && ((that.showPurple && d.sign >= 0) || (that.showOrange && d.sign < 0)) ) {
                                    if(d.sign >= 0)
                                        that.filterdata[i] = d.thick;
                                    else that.filterdata[i] = -d.thick;
                                    countfilter++;
                                }
                                else {
                                    that.filterdata[i] = 0;
                                }
                            }
                        });
                    }
                    else {
                        that.matrix.links.forEach(function(d,i) {
                            if (d.target == d.source) {
                                that.filterdata[i] = 0;
                            } else {
                                if(d.thick >= fillMin && ((that.showPurple && d.sign >= 0) || (that.showOrange && d.sign < 0))) {
                                    if(d.sign >= 0)
                                        that.filterdata[i] = d.thick;
                                    else that.filterdata[i] = -d.thick;
                                    countfilter++;
                                }
                                else {
                                    that.filterdata[i] = 0;
                                }
                            }
                        });
                    }
                    that.filterdataLen[0] = countfilter;

                    var fmin;
                    if(that.filterRate[0] == 0) {
                        //未过滤的thick大于1的显示
                        fmin = 0;
                    }
                    else {
                        fmin = fillMin;
                    }
                    console.log('fillMin', fmin);
                    svgLeft.selectAll(".link").filter(function(d){
                        return d.thick >= fmin && d.thick != 0 && ((that.showPurple && d.sign >= 0) || (that.showOrange && d.sign < 0));
                    }).style("stroke-opacity", 1);
                    svgLeft.selectAll(".link").filter(function(d){
                        return !(d.thick >= fmin && d.thick != 0 && ((that.showPurple && d.sign >= 0) || (that.showOrange && d.sign < 0)));
                    }).style("stroke-opacity", 0);

                    svgLeft.selectAll(".link2").filter(function(d){
                        return d.thick >= fmin && d.thick != 0 && ((that.showPurple && d.sign >= 0) || (that.showOrange && d.sign < 0));
                    }).style("stroke-width", 4);
                    svgLeft.selectAll(".link2").filter(function(d){
                        return !(d.thick >= fmin && d.thick != 0 && ((that.showPurple && d.sign >= 0) || (that.showOrange && d.sign < 0)));
                    }).style("stroke-width", 0);

                    if(this.selected) {
                        this.$store.commit('setneedReDraw', true);
                    }
                }
            },
            eyeside: {
                deep: true,
                handler: function(newVal, oldVal) {
                    console.log('eyeside', newVal);
                    if(this.strength.hasOwnProperty('data')){
                            //console.log('strength', newVal);
                            this.drawSidBySidePic();
                            //this.$forceUpdate();
                        }
                    /*
                    var s = ".draw-area-" + this.index;
                    var svgLeft = d3.select(this.$el)      
                        .select(s).select(".leftSVG");
                    var that = this;
                    var width = this.width;
                    var height = this.height;
                    var x_range = this.x_range;
                    var y_range = this.y_range;
                    var z_range = this.z_range;
                    
                    if(that.eyeside == "0") {
                    var xScale = d3.scale.linear() //d3.scaleLinear()
                        .domain([0,x_range.x2])
                        .range([-100*width/905, width-100*width/905]);
                    var yScale = d3.scale.linear() //d3.scaleLinear()
                        .domain([0,y_range.y2])
                        .range([-70*height/1030, height-70*height/1030]);
                    //添加脑的背景图
                    //console.log(this.backgroundPic);
                    svgLeft.selectAll(".background")
                        .attr("xlink:href", this.backgroundPicxy)
                        .attr("x", 0).attr("y", 0)
                        .attr("width", width)
                        .attr("height",height);
                    //动态改变线和节点的位置
                    svgLeft.selectAll(".link")
                        .attr("x1", function(d){
                            return xScale(d.source.x);
                        })
                        .attr("y1", function(d){
                            return yScale(d.source.y);
                        })
                        .attr("x2", function(d){
                            return xScale(d.target.x);
                        })
                        .attr("y2", function(d){
                            return yScale(d.target.y);
                        });
                    svgLeft.selectAll(".link2")
                        .attr("x1", function(d){
                            return xScale(d.source.x);
                        })
                        .attr("y1", function(d){
                            return yScale(d.source.y);
                        })
                        .attr("x2", function(d){
                            return xScale(d.target.x);
                        })
                        .attr("y2", function(d){
                            return yScale(d.target.y);
                        });
                    svgLeft.selectAll(".node")
                        .attr("cx", function(d){
                            return xScale(d.x);
                        })
                        .attr("cy", function(d){
                            return yScale(d.y);
                        })
                        .attr("transform", function(d){
                            return "translate(" + xScale(d.x) + "," + yScale(d.y) +")";
                        });
                }
                else if (that.eyeside == "1") {
                    var xScale = d3.scale.linear() //d3.scaleLinear()
                        .domain([y_range.y1, y_range.y2])
                        .range([55*width/905, width-55*width/905]);
                    var yScale = d3.scale.linear() //d3.scaleLinear()
                        .domain([z_range.z1, z_range.z2])
                        .range([20 + 632*width/760-160*height/1030, 20 + 160*height/1030]);
                    //添加脑的背景图
                    //console.log(this.backgroundPic);
                    svgLeft.selectAll(".background")
                        .attr("xlink:href", this.backgroundPicyz)
                        .attr("x", 0).attr("y", 20)
                        .attr("width", width)
                        .attr("height",632*width/760);
                    //动态改变线和节点的位置
                    svgLeft.selectAll(".link")
                        .attr("x1", function(d){
                            return xScale(d.source.y);
                        })
                        .attr("y1", function(d){
                            return yScale(d.source.z);
                        })
                        .attr("x2", function(d){
                            return xScale(d.target.y);
                        })
                        .attr("y2", function(d){
                            return yScale(d.target.z);
                        });
                    svgLeft.selectAll(".link2")
                        .attr("x1", function(d){
                            return xScale(d.source.y);
                        })
                        .attr("y1", function(d){
                            return yScale(d.source.z);
                        })
                        .attr("x2", function(d){
                            return xScale(d.target.y);
                        })
                        .attr("y2", function(d){
                            return yScale(d.target.z);
                        });
                    svgLeft.selectAll(".node")
                        .attr("cx", function(d){
                            return xScale(d.y);
                        })
                        .attr("cy", function(d){
                            return yScale(d.z);
                        })
                        .attr("transform", function(d){
                            return "translate(" + xScale(d.y) + "," + yScale(d.z) +")";
                        });
                }
                else if(that.eyeside == "2") {
                    var xScale = d3.scale.linear() //d3.scaleLinear()
                        .domain([x_range.x1, x_range.x2])
                        .range([55*width/905,width - 55*width/905]);
                    var yScale = d3.scale.linear() //d3.scaleLinear()
                        .domain([z_range.z1, z_range.z2])
                        .range([50 + 507*width/633 - 30*height/1030, 50 + 30*height/1030]);
                    //添加脑的背景图
                    //console.log(this.backgroundPic);
                    svgLeft.selectAll(".background")
                        .attr("xlink:href", this.backgroundPicxz)
                        .attr("x", 0).attr("y", 50)
                        .attr("width", width)
                        .attr("height", 507*width/633);
                    //动态改变线和节点的位置
                    svgLeft.selectAll(".link")
                        .attr("x1", function(d){
                            return xScale(d.source.x);
                        })
                        .attr("y1", function(d){
                            return yScale(d.source.z);
                        })
                        .attr("x2", function(d){
                            return xScale(d.target.x);
                        })
                        .attr("y2", function(d){
                            return yScale(d.target.z);
                        });
                    svgLeft.selectAll(".link2")
                        .attr("x1", function(d){
                            return xScale(d.source.x);
                        })
                        .attr("y1", function(d){
                            return yScale(d.source.z);
                        })
                        .attr("x2", function(d){
                            return xScale(d.target.x);
                        })
                        .attr("y2", function(d){
                            return yScale(d.target.z);
                        });
                    svgLeft.selectAll(".node")
                        .attr("cx", function(d){
                            return xScale(d.x);
                        })
                        .attr("cy", function(d){
                            return yScale(d.z);
                        })
                        .attr("transform", function(d){
                            return "translate(" + xScale(d.x) + "," + yScale(d.z) +")";
                        });
                }
                */
                }
            },
        }
    }
</script>