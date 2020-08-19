<!--叠加图，strength从外部绑定-->
<template>
    <div style="text-align: left;">
        <div :class='setClass()' style="height:50%; position: relative;"> 
            <!--div class="loading" style="margin-top: 6px;" v-if="getLoading"></div-->
            <loading v-bind="{'loading': loading}"></loading>
            <div style="text-align: center; padding-top: 11px; margin-right: 24px; font-size: 14px; min-height: 46px;">{{titleName}}</div>
            <div class="leftSVG">
                <div class="tipsDiv" style="z-index: 10; color: black; font-size: 12px;text-align: right;position: absolute;visibility: hidden;"></div>
            </div>
            <!--div style="position: absolute; top: 130px; padding-left: 0px; z-index: 999;">
                <div>
                    <input class="colorcheckbox" type="checkbox" @click="changePurple()" />
                    <span :class="{'colorSpan':true, 'purpleCircle':true, 'hasOk': showPurple}">
                        <span class="f-full"></span>
                    </span>
                </div>
                <div>
                    <input class="colorcheckbox" type="checkbox" @click="changeOrange()" />
                    <span :class="{'colorSpan':true, 'orangeCircle':true, 'hasOk': showOrange}">
                        <span class="f-full"></span>
                    </span>
                </div>
                <div>
                    <input class="colorcheckbox" type="checkbox" @click="changeGray()" />
                    <span :class="{'colorSpan':true, 'grayCircle':true, 'hasOk': showGray}">
                        <span class="f-full"></span>
                    </span>
                </div>
            </div-->
            <!--<div style="float:left;" id="rightSVG"></div>-->
            <div class="panel" ref="myPanel" style="overflow: visible; margin-top: 50px; margin-left: 125px;">
                <div>
                    <div style="float: left; margin-top: -8px; margin-left: 0px; color: #8998AC; width: 60px; text-align: right;">{{"Pearl Size: "}}</div>
                    <slider :min=0 :max=100 v-model="ellipseWidth" :dataMin="ellipseMin" :dataMax="ellipseMax" :style="{'width': '150px'}"></slider>
                </div>
                <!--div style="padding-top: 20px;">
                <slider :min=0 :max=100 v-model="ellipseWidthMin" :dataMin="ellipseMinMin" :dataMax="ellipseMinMax" :style="{'width': '95px'}"></slider>
                </div-->
            </div>
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
    import backgroundPic from "../assets/brain_xy.png"
    import rightPic from "../assets/ok.png"
    import slider from './slider'
    import reverseSlider from "./reverseSlider"
    import loading from "./loading"

    //import $ from "jquery"
    export default {
        name: 'superposedGraphForCalTwoWithEllipseVersionTwo',
        components: {
            slider,
            reverseSlider,
            loading
        },
        props: {
            //p-value
            strength: {
                required: false,
                type: Object
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
            filterRate: {
                required: false,
                type: Array,
                default: [0, 100],
            },
            collection: {
                required: false,
                type: Object
            },
            /*
            allFeature: {
                required: false,
                type: Array,
                default: [],
            },
            allFunc: {
                required: false,
                type: Array,
                default: [],
            },
            allGroups: {
                required: false,
                type: Array,
                default: [],
            },
            */
        },
        data() {
            return {
                //strength,
                matrix: {},
                backgroundPic: require("../assets/brain_xy.png"),
                rightPic: require("../assets/ok.png"),
                isFocus: false,
                //filterRate: [0, 100],
                weight_range: {},
                thick_range: {},
                reverse: true,
                showPurple: true,
                showOrange: true,
                showGray: true,
                dataLength: 0,
                ellipseWidth: 16,
                ellipseWidthMin: 1,
                ellipseMin: 0,
                ellipseMax: 100,
                ellipseMinMin: 0,
                ellipseMinMax: 1,
                scaleWidth: 0,
                scaleHeight: 0,
                scalex_range: {},
                scaley_range: {},
                fillMin: 0,
                fillMax: 0,
                selectedNodes: {},
                selectedLink: {},
                widthScale: d3.scale.linear().domain([0, 1]).range([0, 1]),
                minx: 16,
                step: 5,
                alpha: 0.5,
                consx: 1,
                titleName: " ",
                loading: false,
                load: false,
                drag: "",
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
        computed: {
            getLoading() {
                return this.loading;
            }
        },
        methods: {
            //椭圆边框颜色
            strokeColor(value){
                if (value) {
                    return "#775AA9";
                }
                else {
                    return "#FE8F00";
                }
            },
            //椭圆填充颜色
            fillColor(value) {
                return "white";
                if (value) {
                    return "#775AA9";
                }
                else {
                    return "#FE8F00";
                }
            },
            //椭圆内部线条颜色
            lineColor(value) {
                //return "white";
                if (value) {
                    return "#775AA9";
                }
                else {
                    return "#FE8F00";
                }
            },
            //点到直线距离
            caldistance(x0, y0, x1, y1, x2, y2) {
                var A = y2 - y1;
                var B = x1 - x2;
                var C = (y1 - y2) * x1 + (x2 - x1) * y1;
                if(A == 0) {
                    return y0 - y2;
                }
                else if (B == 0) {
                    return x0 - x2;
                }
                else {
                    var d = Math.abs(A * x0 + B * y0 + C) / Math.sqrt(A * A + B * B);
                    return d;
                } 
            },
            //计算长度在直线的投影的xy,直线从(x0,y0)到(x1,y1)
            caldxdy(len, x0,  y0, x1, y1){
                var result = [];
                var dx = x1 - x0;
                var dy = y1 - y0;
                var sqrtdistance = Math.sqrt(dx * dx + dy * dy);
                if(sqrtdistance == 0) {
                    result = [0, 0];
                }
                else {
                    var dirx = dx / sqrtdistance;
                    var diry = dy / sqrtdistance;
                    result.push(len * dirx);
                    result.push(len * diry);
                }
                return result;         
            },
            pointdistance(x0, y0, x1, y1) {
                var dx = x0 - x1;
                var dy = y0 - y1;
                return Math.sqrt(dx * dx + dy * dy);
            },
            calposition(x0, y0, x1, y1, min_x, constx, dataNum) { //方向为(x0,y0)到(x1,y1)
                var result = [];
                var dx = x1 - x0;
                var dy = y1 - y0;
                var sqrtdistance = Math.sqrt(dx * dx + dy * dy);
                if (sqrtdistance == 0) {
                    for (var i = 0; i < dataNum; i++) {
                        result.push([x0, y0]);
                    }
                }
                else {
                    var dirx = dx / sqrtdistance;
                    var diry = dy / sqrtdistance;
                    for (var i = 0; i < dataNum; i++) {
                        var newx = x0 + (min_x + i * constx) * dirx;
                        var newy = y0 + (min_x + i * constx) * diry;
                        result.push([newx, newy]);
                    }
                }
                return result;
            },
            changePurple() {
                this.showPurple = !this.showPurple;
            },
            changeOrange() {
                this.showOrange = !this.showOrange;
            },
            changeGray() {
                this.showGray = !this.showGray;
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
            fillScaleGray(weight, maxWeight) {
                //var g = (weight / maxWeight);
                //return d3.rgb(255 - g * 200, 255 - g * 200, 255 - g * 200).toString();
                var g = (1 - weight / maxWeight) * 100 + 100;
                return d3.rgb(g, g, g).toString();
            },
            fillScalePurple(weight, maxWeight) {
                var g = (weight / maxWeight);
                return d3.rgb(205 - g * 138, 197 - g * 165, 224 - g * 85).toString();
            },
            fillScaleOrange(weight, maxWeight) {
                var g = (weight / maxWeight) * 0.3 + 0.7;
                return d3.rgb(254 - g * 5, 219 - g * 139, 192 - g * 255).toString();
                //return d3.hsl(18, 0.94, 0.46*g);
            },
            fillScaleOne(weight, maxWeight) {
                return d3.rgb(180, 180, 180).toString();
                return this.fillScaleGray(weight, maxWeight);
                //return this.fillScalePurple(weight, maxWeight);
            },
            fillScaleTwo(weight, maxWeight) {
                return d3.rgb(180, 180, 180).toString();
                return this.fillScaleGray(weight, maxWeight);
                //return this.fillScaleOrange(weight, maxWeight);
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
                var sizeobj = {};
                sizeobj.height = height;
                sizeobj.width = width;
                
                //构造一个新的序数比例尺，有20种类型的颜色
                //var color = d3.scaleOrdinal(d3.schemeCategory20);
                var color = d3.scale.category20();
                //console.log(color);

                //使用物理模拟排放链接节点的位置
                //电荷强度、链接强度、链接距离、重力大小、布局大小
                //var forceLeft = d3.forceSimulation();
                //var forceRight = d3.forceSimulation();

                //titleName为Group的并集
                var groupArray = [];
                if (this.collection.allGroups.length > 0) {
                    for(var i in this.collection.allGroups) {
                        for(var j in this.collection.allGroups[i]) {
                            //假设名字不在groupArray中，放进去
                            if(groupArray.indexOf(this.collection.allGroups[i][j]) == -1) {
                                groupArray.push(this.collection.allGroups[i][j]);
                            }
                        }
                    }
                    var groupString = "";
                    for(var i in groupArray) {
                        if(i != 0) groupString += ",";
                        groupString += groupArray[i];
                    }
                    groupString += " Group";
                    this.titleName = groupString;
                }

                //计算是否相同特征
                var sameFeature = true;
                for(var i in this.collection.allFeature) {
                    if(i != 0) {
                        if(this.collection.allFeature[i] != this.collection.allFeature[Number(i)-1]) {
                            sameFeature = false;
                        }
                    }
                }
                var toptitle = "";
                var pearlName = [];
                //假设特征相同
                if(sameFeature) {
                    toptitle = this.collection.allFunc[0] + " " + this.collection.allFeature[0];
                    //珠子的名字为不同的组
                    for(var i in this.collection.allGroups) {
                        var obj = {};
                        obj.name = this.collection.allGroups[i][0] + "," + this.collection.allGroups[i][1];
                        obj.index = Number(i);
                        pearlName.push(obj);
                    }

                }
                else {
                    toptitle = this.collection.allFunc[0] + " " + this.titleName;
                    //珠子的名字为不同的特征
                    var perlen = this.collection.allFeature.length;
                    for(var i in this.collection.allFeature) {
                        var obj = {};
                        obj.name = this.collection.allFeature[i] + "," + this.collection.allFeature[(Number(i) + 1) % perlen];
                        obj.index = Number(i);
                        pearlName.push(obj);
                    }
                }
                //console.log("pearlName", pearlName);

                //添加svg
                var svgLeft = d3.select(this.$el)         
                    .select(s)
                    .select(".leftSVG")
                    .append("svg")
                    .attr("width", width)
                    .attr("height", height);

                var svgLegend = d3.select(this.$el)         
                    .select(s)
                    .select(".panel")
                    .append("svg")
                    .attr("width", 100 * width / 905)
                    .attr("height", 40 * height / 1030)
                    .append("g")

                var panel = this.$refs.myPanel;

                //var weights; //定义权重
                var indexWeight = {}; //记录权重
                var indexThickness = {}; //记录粗细
                var groupOneData = {}; //组1数据
                var groupTwoData = {};
                var sign = {};
                this.selectedNodes = {}; //eg:{1:1,2:0}
                this.selectedLink = {};
                var left_range, right_range;
                var x_range = {"x1":1000, "x2":0};
                var y_range = {"y1":1000, "y2":0};
                var z_range = {"z1":1000, "z2":0};
                var filter_value = 0;  //weight低于该值则不显示
                var connection = [];
                for (var i = 0; i < 70; i++) {
                    connection.push(0);
                }

                //console.log(this.strength.pvalue[0]);
                //读取weight，thickness
                for(var i = 0; i < this.strength.data.length; i++) {
                    indexThickness[i] = Math.abs(this.strength.data[i]);
                    sign[i] = this.strength.sign[i];
                }

                this.dataLength = this.strength.allDatas.length; //多少个小图相加
                var dataNum = this.dataLength;
                if(dataNum == 0) this.loading = false;
                var allDatas = this.deepCopy(this.strength.allDatas); //每个子图的具体数据
                var allSigns = this.deepCopy(this.strength.allSigns); //每个子图的正负
                var sametype = this.strength.sametype;
                if(sametype) {
                    //找绝对值最大值并归一化
                    console.log('sametype', sametype);
                    var max = 0;
                    for (var i in allDatas) {
                        for (var j in allDatas[i]) {
                            if (Math.abs(allDatas[i][j]) > max) {
                                max = Math.abs(allDatas[i][j]);
                            }
                        }
                    }
                    if(max == 0) max = 1;
                    for (var i in allDatas) {
                        for (var j in allDatas[i]) {
                            allDatas[i][j] = Math.abs(allDatas[i][j] / max);
                        }
                    }
                } else {
                    //分别归一化
                    console.log('sametype', sametype);
                    for (var i in allDatas) {
                        var max = 0;
                        for (var j in allDatas[i]) {
                            if (Math.abs(allDatas[i][j]) > max) {
                                max = Math.abs(allDatas[i][j]);
                            }
                        }
                        for (var j in allDatas[i]) {
                            allDatas[i][j] = Math.abs(allDatas[i][j] / max);
                        }
                    }
                }
                console.log('allDatas', allDatas);
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

                //增添一属性用来记录右图的roi连接及对应强度
                //this.matrix.linksRight = new Array();
                //range记录weight\thick的最大最小值
                var range = {"r1":10000, "r2":-10000};
                var rangeThick = {"r1":10000, "r2":-10000};
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
                        d.data = [];
                        d.cx = [];
                        d.cy = [];
                        d.allSigns = [];
                        for (var a = 0; a < dataNum; a++) {
                            d.data[a] = 0;
                            d.allSigns[a] = 0;
                        }
                    } else {
                        //d.weight = indexWeight[i];
                        //range.r1 = Math.min(range.r1,indexWeight[i]);
                        //range.r2 = Math.max(range.r2,indexWeight[i]);

                        d.thick = indexThickness[i];
                        rangeThick.r1 = Math.min(rangeThick.r1, indexThickness[i]);
                        rangeThick.r2 = Math.max(rangeThick.r2, indexThickness[i]);

                        //d.groupOneData = groupOneData[i];
                        //d.groupTwoData = groupTwoData[i];
                        d.sign = sign[i];
                        d.data = [];
                        d.cx = [];
                        d.cy = [];
                        d.allSigns = [];
                        for (var a = 0; a < dataNum; a++) {
                            d.data.push(allDatas[a][i]);
                            d.allSigns.push(allSigns[a][i]);
                        }
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
                //that.fillMax = this.weight_range.r2;
                //that.fillMin = this.weight_range.r1;
                var that = this;
                this.thick_range = rangeThick;
                that.fillMin = this.filterRate[0] * (this.thick_range.r2 - this.thick_range.r1) / 100 + this.thick_range.r1;
                that.fillMax = this.filterRate[1] * (this.thick_range.r2 - this.thick_range.r1) / 100 + this.thick_range.r1 + 100;
                //console.log('thick_range', this.thick_range, indexThickness);
                //console.log('that.fillMin', that.fillMin, that.fillMax);

                var xScale = d3.scale.linear() //d3.scaleLinear()
                    .domain([0,x_range.x2])
                    .range([-100*width/905 + 30, width-100*width/905 + 30]);
                var yScale = d3.scale.linear() //d3.scaleLinear()
                    .domain([0,y_range.y2])
                    .range([-70*height/1030 + 20, height-70*height/1030 + 20]);
                
                that.widthScale = d3.scale.linear()
                    .domain([0, 1])
                    .range([that.ellipseWidthMin, that.ellipseWidth]);
                
                this.scaleWidth = width;
                this.scaleHeight = height;
                this.scalex_range = x_range;
                this.scaley_range = y_range;

                //添加脑的背景图
                //console.log(this.backgroundPic);
                svgLeft.append("image").attr("class","background")
                    .attr("xlink:href", this.backgroundPic)
                    .attr("x", 30).attr("y", 20)
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
                    .select('.leftSVG')
                    .select(".tipsDiv");

                var startx = 0;
                var starty = 0;
                this.drag = forceLeft.drag()
                    .on('dragstart', function(d){
                        startx = d3.event.sourceEvent.clientX;
                        starty = d3.event.sourceEvent.clientY;
                        //console.log('dragstart', d3.event.sourceEvent.clientX, d3.event.sourceEvent.clientY);
                    })
                    .on('dragend', function(d){
                        //console.log('dragend', d3.event, d3.event);
                        //console.log("dragend", d);
                    })
                    .on('drag', function(d){
                        var nowx = d3.event.sourceEvent.clientX;
                        var nowy = d3.event.sourceEvent.clientY;
                        //计算在这条直线上偏离的距离
                        var disx = nowx - startx;
                        var disy = nowy - starty;
                        var dis = Math.sqrt(disx * disx + disy * disy);
                        console.log("dis", dis);
                        //对于连接同一个点的直线的所有椭圆都偏移
                        var nodeNum = d.source.index;
                        var dx = d.target.x - d.source.x;
                        var dy = d.target.y - d.source.y;
                        if(connection[d.source.index] < connection[d.target.index]) {
                            nodeNum = d.target.index;
                            dx = d.source.x - d.target.x;
                            dy = d.source.y - d.target.y;
                        }
                        //判断是否同方向
                        if((dx != 0 && disx != 0 && dx*disx >= 0) || (dy != 0 && disy != 0 && dy * disy >= 0)) {
                            //同方向
                        }
                        else {
                            dis = 0 - dis;
                        }
                        /*
                        if(d.hasOwnProperty("tran")) {
                            dis = dis + d.tran;
                        }
                        else {
                            d.tran = dis;
                        }
                        */

                        svgLeft.selectAll(".polygon").filter(function(d1) {
                            return (d1.source.index == nodeNum && connection[d1.source.index] >= connection[d1.target.index]) || (d1.target.index == nodeNum && connection[d1.source.index] < connection[d1.target.index]);
                        }).selectAll("g").style("transform", function(d1, i){
                            var result;
                            if(d1.source.index == nodeNum) {
                                result = that.caldxdy(dis, xScale(d1.source.x), yScale(d1.source.y) , xScale(d1.target.x), yScale(d1.target.y));
                            }
                            else {
                                result = that.caldxdy(dis, xScale(d1.target.x), yScale(d1.target.y), xScale(d1.source.x), yScale(d1.source.y));
                            }
                            var minnodex = Math.min(xScale(d1.source.x), xScale(d1.target.x));
                                var maxnodex = Math.max(xScale(d1.source.x), xScale(d1.target.x));
                                var minnodey = Math.min(yScale(d1.source.y), yScale(d1.target.y));
                                var maxnodey = Math.max(yScale(d1.source.y), yScale(d1.target.y));
                                if(d1.cx[0] + result[0] < minnodex) {
                                    result[0] = minnodex - d1.cx[0];
                                }
                                else if(d1.cx[dataNum - 1] + result[0] > maxnodex) {
                                    result[0] = maxnodex - d1.cx[dataNum - 1];
                                }
                                if(d1.cy[0] + result[1] < minnodey) {
                                    result[1] = minnodey - d1.cy[0];
                                }
                                else if(d1.cy[dataNum - 1] + result[1] > maxnodey) {
                                    result[1] = maxnodey - d1.cy[dataNum - 1];
                                }
                            return "translate(" + result[0] + "px, " + result[1] + "px)";
                        });

                        svgLeft.selectAll(".polygonup").filter(function(d1) {
                            return (d1.source.index == nodeNum && connection[d1.source.index] >= connection[d1.target.index]) || (d1.target.index == nodeNum && connection[d1.source.index] < connection[d1.target.index]);
                        }).selectAll("g").style("transform", function(d1){
                            //计算偏移距离
                            var result;
                            if(d1.source.index == nodeNum) {
                                result = that.caldxdy(dis, xScale(d1.source.x), yScale(d1.source.y) , xScale(d1.target.x), yScale(d1.target.y));
                            }
                            else {
                                result = that.caldxdy(dis, xScale(d1.target.x), yScale(d1.target.y), xScale(d1.source.x), yScale(d1.source.y));
                            }
                            var minnodex = Math.min(xScale(d1.source.x), xScale(d1.target.x));
                                var maxnodex = Math.max(xScale(d1.source.x), xScale(d1.target.x));
                                var minnodey = Math.min(yScale(d1.source.y), yScale(d1.target.y));
                                var maxnodey = Math.max(yScale(d1.source.y), yScale(d1.target.y));
                                if(d1.cx[0] + result[0] < minnodex) {
                                    result[0] = minnodex - d1.cx[0];
                                }
                                else if(d1.cx[dataNum - 1] + result[0] > maxnodex) {
                                    result[0] = maxnodex - d1.cx[dataNum - 1];
                                }
                                if(d1.cy[0] + result[1] < minnodey) {
                                    result[1] = minnodey - d1.cy[0];
                                }
                                else if(d1.cy[dataNum - 1] + result[1] > maxnodey) {
                                    result[1] = maxnodey - d1.cy[dataNum - 1];
                                }
                            return "translate(" + result[0] + "px, " + result[1] + "px)";
                        });

                        //console.log('drag', d3.event.sourceEvent.offsetX, d3.event.sourceEvent.offsetY);
                    });

                //添加线
                var linkLeft = svgLeft.selectAll(".link")
                    .data(that.matrix.links)
                    .enter()
                    .append("line")
                    .attr("class", function(d,i) {
                        return "leftl" + that.clusterNo(d.source.index, clusters) + "l" + that.clusterNo(d.target.index, clusters) + "leftl" + that.clusterNo(d.source.index, clusters) + "leftl" + that.clusterNo(d.target.index, clusters) +" link";
                    })
                    .style("stroke-width", function(d) {
                        return 0;
                        if (that.reverse){
                            if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                if (d.thick == 0) return 0;
                                if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray)) {
                                    return (d.thick - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 3 + 1;
                                }
                                return 0;
                            }
                            else {
                                return 0;
                            }
                        }
                        else return 4 - (d.thick - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 4 
                    })  //路径宽度
                    .style("stroke", function(d){//路径颜色
                        if(d.thick >= that.fillMin && d.thick <= that.fillMax){
                            if(d.sign == 1){
                                return that.fillScaleOne(d.thick - that.fillMin, that.fillMax - that.fillMin);
                            }
                            else if(d.sign == -1) {
                                return that.fillScaleTwo(d.thick - that.fillMin, that.fillMax - that.fillMin);
                            }
                            else {
                                return that.fillScaleGray(d.thick - that.fillMin, that.fillMax - that.fillMin);
                            }
                        } else {
                            return d3.rgb(255, 255, 255).toString();
                        }
                    })
                    .style("stroke-opacity", function(d){ //不透明度
                        return 1;
                    })
                    .sort(function(a, b) {
                        return a.thick - b.thick;
                    })
                    //添加hover
                    /*
                    .on("mouseenter", function(d, i) {
                        //console.log('mouseenter', d);
                        return tipsDiv
                            .style('visibility', 'visible')
                            .style('left', (d3.event.clientX - 920) + 'px') //- 400
                            .style('top', (d3.event.clientY - 300) + 'px') //- 110
                            .html('ROI: ' + (d.source.index + 1) + '  <->  ' + (d.target.index + 1) + '<br>' + 'value: ' + d.thick.toFixed(5) + '<br>' + 'DataOne: ' + d.groupOneData.toFixed(5) + '<br>' + 'DataTwo: ' + d.groupTwoData.toFixed(5));//tooltip.html(text);
                    })
                    .on("mouseout", function(d, i) {
                        //return tooltip.style('visibility', 'hidden');
                        return tipsDiv.style('visibility', 'hidden');
                    })*/
                    //添加点击事件
                    .on("click", function(d){
                        //console.log('click', this);
                        /*
                        if(true){
                            var target = d.target.index;
                            var source = d.source.index;
                            //该图的线条 变粗以及变色
                            d3.select(this)
                                .style("stroke-width", function(d){
                                    return (d.thick - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 4;
                                })
                                .style("stroke", function(d){
                                    return that.fillScaleGreen(d.weight - that.fillMin, that.weight_range.r2 - that.fillMin);
                                })
                                .style("stroke-opacity", function(d){
                                    return d.weight < filter_value ? 0 : 1;
                                });
                        }
                        */
                    });


                //添加线
                var linkLeft2 = svgLeft.selectAll(".link2")
                    .data(that.matrix.links)
                    .enter()
                    .append("line")
                    .attr("class", function(d,i) {
                        return "leftl" + that.clusterNo(d.source.index, clusters) + "l" + that.clusterNo(d.target.index, clusters) + "leftl" + that.clusterNo(d.source.index, clusters) + "leftl" + that.clusterNo(d.target.index, clusters) +" link";
                    })
                    .style("stroke-width", function(d) {
                        if (that.reverse){
                            if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                if (d.thick == 0) return 0;
                                if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                    return 4;
                                return 0;
                            }
                            else {
                                return 0;
                            }
                        }
                        else return 4 - (d.thick - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 4 
                    })  //路径宽度
                    .style("stroke", function(d){//路径颜色
                        return d3.rgb(0, 0, 255).toString();
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
                        return tipsDiv
                            .style('visibility', 'visible')
                            //.style('left', (d3.event.clientX - 920) + 'px') //- 400
                            //.style('top', (d3.event.clientY - 300) + 'px') //- 110
                            .style('right', '14px')
                            //.style('top', '224px')
                            .style('bottom', '0px')
                            .html('ROI: ' + (d.source.index + 1) + '  <->  ' + (d.target.index + 1) + '<br>' + 'value: ' + d.thick.toFixed(5));//tooltip.html(text);
                    })
                    .on("mouseout", function(d, i) {
                        //return tooltip.style('visibility', 'hidden');
                        return tipsDiv.style('visibility', 'hidden');
                    })
                    //添加点击事件
                    .on("click", function(d){

                    });

                this.matrix.links.forEach(function(d,i){
                    if(d.thick != 0){
                        if (d.thick >= that.fillMin){
                            if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray)) {
                                connection[d.source.index]++;
                                connection[d.target.index]++;
                            }
                        }
                    }
                    
                });
                console.log("connection", connection);

                //粗细不一的线
                var polygon = svgLeft.selectAll(".polygon")
                    .data(that.matrix.links)
                    .enter()
                    .append("g")
                    .attr("class", function(d,i) {
                        return "polygon";
                    })
                    .sort(function(a, b) {
                        return a.thick - b.thick;
                    });
                polygon.append("polygon")
                    .style("fill", function(d) {
                        if(d.thick >= that.fillMin && d.thick <= that.fillMax){
                            if(d.sign == 1){
                                return that.fillScaleOne(d.thick - that.fillMin, that.fillMax - that.fillMin);
                            }
                            else if(d.sign == -1) {
                                return that.fillScaleTwo(d.thick - that.fillMin, that.fillMax - that.fillMin);
                            }
                            else {
                                return that.fillScaleGray(d.thick - that.fillMin, that.fillMax - that.fillMin);
                            }
                        } else {
                            return d3.rgb(255, 255, 255).toString();
                        }
                    })
                    .attr("points", function(d){
                        var source = [xScale(d.source.x), yScale(d.source.y)];
                        var target = [xScale(d.target.x), yScale(d.target.y)];
                        var x_distance = target[0] - source[0];
                        var y_distance = target[1] - source[1];
                        var c_distance = Math.pow(x_distance, 2) + Math.pow(y_distance, 2);
                        var distance = Math.sqrt(c_distance);

                        //当y_distance = 0时，即平行于x轴, y值直接变化
                        if(y_distance == 0) {
                             if(connection[d.source.index] >= connection[d.target.index])
                            return xScale(d.source.x).toString() + ',' + (yScale(d.source.y) - 2).toString() + " " + 
                                xScale(d.target.x).toString() + ',' + (yScale(d.target.y) - 0.5).toString() + " " +
                                xScale(d.target.x).toString() + ',' + (yScale(d.target.y) + 0.5).toString() + " " +
                                xScale(d.source.x).toString() + ',' + (yScale(d.source.y) + 2).toString() + " " ;
                        else return xScale(d.target.x).toString() + ',' + (yScale(d.target.y) - 2).toString() + " " + 
                                xScale(d.source.x).toString() + ',' + (yScale(d.source.y) - 0.5).toString() + " " +
                                xScale(d.source.x).toString() + ',' + (yScale(d.source.y) + 0.5).toString() + " " +
                                xScale(d.target.x).toString() + ',' + (yScale(d.target.y) + 2).toString() + " " ;
                        }
                        //当x_disatnce = 0, 即垂直于x轴时，x值变化
                        if(x_distance == 0) {
                             if(connection[d.source.index] >= connection[d.target.index])
                            return (xScale(d.source.x) - 2).toString() + ',' + (yScale(d.source.y)).toString() + " " + 
                                (xScale(d.target.x) - 0.5).toString() + ',' + (yScale(d.target.y)).toString() + " " +
                                (xScale(d.target.x) + 0.5).toString() + ',' + (yScale(d.target.y)).toString() + " " +
                                (xScale(d.source.x) + 2).toString() + ',' + (yScale(d.source.y)).toString() + " " ;
                        else return (xScale(d.target.x) - 2).toString() + ',' + (yScale(d.target.y)).toString() + " " + 
                                (xScale(d.source.x) - 0.5).toString() + ',' + (yScale(d.source.y)).toString() + " " +
                                (xScale(d.source.x) + 0.5).toString() + ',' + (yScale(d.source.y)).toString() + " " +
                                (xScale(d.target.x) + 2).toString() + ',' + (yScale(d.target.y)).toString() + " " ;
                        }
                        //否则计算斜率
                        var xielv = y_distance / x_distance;
                        //计算夹角,A为垂线与水平线夹角，锐角
                        var sinA = Math.abs(y_distance) / distance;
                        var cosA = Math.abs(x_distance) / distance;
                        //var bsource = source[1] + source[0] / xielv;
                        //var btarget = target[1] + target[0] / xielv;
                        if(xielv > 0){
                            if(connection[d.source.index] >= connection[d.target.index])
                                return (source[0] - 2 * sinA).toString() + ',' + (source[1] + 2 * cosA).toString() + " " + 
                                (target[0] - 0.5 * sinA).toString() + ',' + (target[1] + 0.5 * cosA).toString() + " " +
                                (target[0] + 0.5 * sinA).toString() + ',' + (target[1] - 0.5 * cosA).toString() + " " +
                                (source[0] + 2 * sinA).toString() + ',' + (source[1] - 2 * cosA).toString() + " " ;
                            else return (target[0] - 2 * sinA).toString() + ',' + (target[1] + 2 * cosA).toString() + " " + 
                                (source[0] - 0.5 * sinA).toString() + ',' + (source[1] + 0.5 * cosA).toString() + " " +
                                (source[0] + 0.5 * sinA).toString() + ',' + (source[1] - 0.5 * cosA).toString() + " " +
                                (target[0] + 2 * sinA).toString() + ',' + (target[1] - 2 * cosA).toString() + " " ;
                        }
                        else {
                            if(connection[d.source.index] >= connection[d.target.index])
                                return (source[0] - 2 * sinA).toString() + ',' + (source[1] - 2 * cosA).toString() + " " + 
                                (target[0] - 0.5 * sinA).toString() + ',' + (target[1] - 0.5 * cosA).toString() + " " +
                                (target[0] + 0.5 * sinA).toString() + ',' + (target[1] + 0.5 * cosA).toString() + " " +
                                (source[0] + 2 * sinA).toString() + ',' + (source[1] + 2 * cosA).toString() + " " ;
                            else return (target[0] - 2 * sinA).toString() + ',' + (target[1] - 2 * cosA).toString() + " " + 
                                (source[0] - 0.5 * sinA).toString() + ',' + (source[1] - 0.5 * cosA).toString() + " " +
                                (source[0] + 0.5 * sinA).toString() + ',' + (source[1] + 0.5 * cosA).toString() + " " +
                                (target[0] + 2 * sinA).toString() + ',' + (target[1] + 2 * cosA).toString() + " " ;
                        }

                        
                    })
                    .style("opacity", function(d) {
                        if (that.reverse){
                            if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                if (d.thick == 0) return 0;
                                if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                    return 1;
                                return 0;
                            }
                            else {
                                return 0;
                            }
                        }
                        else return 4 - (d.thick - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 4 
                    });

                //添加椭圆
                var maxx = 60;
                //var that.consx = 15;
                //var that.step = 5;
                //var that.alpha = 0.5;
                
                var xs = []; //存储每个节点的x
                var scores = []; //存储每个节点的score
                for(var i = 0; i < 70; i++) {
                    xs.push(that.minx);
                    scores.push(0);
                }
                
                if(dataNum > 0)
                //对于每个节点计算它的score
                for(var i = 0; i < 70; i++) {
                    //计算maxx
                    maxx = 999;
                    var count = 0;
                    that.matrix.links.filter(function(d1){
                        return ((d1.source.index == i && connection[d1.source.index] >= connection[d1.target.index]) || (d1.target.index == i && connection[d1.source.index] <= connection[d1.target.index])) && Math.abs(d1.thick) > 0;
                    }).forEach(function(d){
                        var dx = xScale(d.target.x) - xScale(d.source.x);
                        var dy = yScale(d.target.y) - yScale(d.source.y);
                        var sqrtdistance = Math.sqrt(dx * dx + dy * dy);
                        //maxx += sqrtdistance;
                        for(var si = 0; si < dataNum; si++) {
                            sqrtdistance -= (that.widthScale(d.data[si]) * 2);
                        }
                        if(sqrtdistance < 0) sqrtdistance = 0;
                        if(sqrtdistance < maxx) maxx = sqrtdistance;
                        count++;
                    });
                    //if(count != 0) maxx = maxx / count;
                    maxx = maxx - that.consx * (dataNum - 1);
                    if(maxx < 0) maxx = 0;
                    //计算每一步寻找最大的score
                    for(var x = that.minx; x < maxx; x=x+that.step) {
                        var score = 0;
                        //找到所有与该节点相连的线，该节点重要性大于另一节点
                        that.matrix.links.filter(function(d1){
                            return ((d1.source.index == i && connection[d1.source.index] >= connection[d1.target.index]) || (d1.target.index == i && connection[d1.source.index] <= connection[d1.target.index])) && Math.abs(d1.thick) > 0;
                        }).forEach(function(d){
                            //对于该线上的所有椭圆，计算minscore
                            //节点原始坐标
                            var originx = 0;
                            var originy = 0;
                            //延直线扩展的单位
                            var dirx = 0;
                            var diry = 0;
                            if(d.source.index == i) {
                                originx = xScale(d.source.x);
                                originy = yScale(d.source.y);
                                var dx = xScale(d.target.x) - xScale(d.source.x);
                                var dy = yScale(d.target.y) - yScale(d.source.y);
                                var sqrtdistance = Math.sqrt(dx * dx + dy * dy);
                                dirx = dx / sqrtdistance;
                                diry = dy / sqrtdistance;
                            }
                            else {
                                originx = xScale(d.target.x);
                                originy = yScale(d.target.y);
                                var dx = xScale(d.source.x) - xScale(d.target.x);
                                var dy = yScale(d.source.y) - yScale(d.target.y);
                                var sqrtdistance = Math.sqrt(dx * dx + dy * dy);
                                dirx = dx / sqrtdistance;
                                diry = dy / sqrtdistance;
                            }
                            for (var j = 0; j < dataNum; j++) {
                                var minscore = 9999;
                                //找到与该线条不同的所有线,计算点到直线的最小距离
                                that.matrix.links.filter(function(d1){
                                    return !(d1.in == d.in) && Math.abs(d1.thick) > 0;
                                }).forEach(function(d1) {
                                    var x0 = originx + (x + j * that.consx) * dirx;
                                    var y0 = originy + (x + j * that.consx) * diry;
                                    var distance = that.caldistance(x0, y0, xScale(d1.source.x), yScale(d1.source.y), xScale(d1.target.x), yScale(d1.target.y));
                                    if (distance < minscore) {
                                        minscore = distance;
                                    }
                                });
                                score += minscore;
                            }
                        });
                        
                        if(score > scores[i]) {
                            scores[i] = score;
                            xs[i] = x;
                        }
                    }
                }
                
                console.log('xs', xs, 'scores', scores);

                for(var numindex = 0; numindex < dataNum; numindex++) {
                    var classname = ".ellipse" + numindex;
                    var ellipse = polygon.append("g")
                        .attr("class", function(d,i){
                            return "ellipse" + numindex + " ellipse";
                        });

                    ellipse.append("ellipse")
                        .attr("rx", function(d){
                            //console.log('d.data', d.data[numindex]);
                            if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                if (d.thick == 0) return 0;
                                if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray)){
                                    return that.widthScale(d.data[numindex]);
                                    //return Math.abs(d.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                return 0;
                            }
                            else {
                                return 0;
                            }            
                        })
                        .attr("ry", function(d) {
                            if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                if (d.thick == 0) return 0;
                                if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray)){
                                    return that.widthScale(d.data[(numindex + 1) % dataNum]);
                                    //return Math.abs(d.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                return 0;
                            }
                            else {
                                return 0;
                            }

                        })
                        .attr("cx", function(d) {
                            
                            var originx = 0;
                            var originy = 0;
                            var targetx = 0;
                            var targety = 0;
                            //延直线扩展的单位
                            var dirx = 0;
                            var diry = 0;
                            var noedeindex = 0;
                            var sqrtdistance = 0;
                            if(connection[d.source.index] >= connection[d.target.index]) {
                                noedeindex = Number(d.source.index);
                                originx = xScale(d.source.x);
                                originy = yScale(d.source.y);
                                targetx = xScale(d.target.x);
                                targety = yScale(d.target.y);
                                var dx = xScale(d.target.x) - xScale(d.source.x);
                                var dy = yScale(d.target.y) - yScale(d.source.y);
                                sqrtdistance = Math.sqrt(dx * dx + dy * dy);
                                if(sqrtdistance == 0) {
                                    d.cx.push(originx);
                                    return originx;
                                }
                                dirx = dx / sqrtdistance;
                                diry = dy / sqrtdistance;
                            }
                            else {
                                noedeindex = Number(d.target.index);
                                originx = xScale(d.target.x);
                                originy = yScale(d.target.y);
                                targetx = xScale(d.source.x);
                                targety = yScale(d.source.y);
                                var dx = xScale(d.source.x) - xScale(d.target.x);
                                var dy = yScale(d.source.y) - yScale(d.target.y);
                                sqrtdistance = Math.sqrt(dx * dx + dy * dy);
                                if(sqrtdistance == 0) {
                                    d.cx.push(originx);
                                    return originx;
                                }
                                dirx = dx / sqrtdistance;
                                diry = dy / sqrtdistance;
                            }
                            var forwardEllipse = 0;
                            var localEllipse = that.widthScale(d.data[(numindex + 1) % dataNum]);
                            for(var ti = 0; ti < numindex; ti++) {
                                forwardEllipse = that.widthScale(d.data[(ti + 1) % dataNum]) * 2 + that.consx;
                            }

                            var result = originx + (xs[noedeindex] +forwardEllipse + localEllipse) * dirx;
                            if((xs[noedeindex] + forwardEllipse + localEllipse) >= sqrtdistance) {
                                result = targetx;
                                d.full = true; //放不下
                            }
                            else {
                                d.full = false;
                            }
                            d.cx.push(result);
                            //console.log('result', result);
                            return result;
                                
                            /*
                            if(connection[d.source.index] >= connection[d.target.index]){
                                d.cx.push((xScale(d.target.x) - xScale(d.source.x)) / (dataNum + 1) * (numindex + 1) + xScale(d.source.x));
                                return (xScale(d.target.x) - xScale(d.source.x)) / (dataNum + 1) * (numindex + 1) + xScale(d.source.x);
                            }
                            else{
                                d.cx.push((xScale(d.source.x) - xScale(d.target.x)) / (dataNum + 1) * (numindex + 1) + xScale(d.target.x));
                                return (xScale(d.source.x) - xScale(d.target.x)) / (dataNum + 1) * (numindex + 1) + xScale(d.target.x);
                            }
                            */
                        })
                        .attr("cy", function(d){
                            var originx = 0;
                            var originy = 0;
                            var targetx = 0;
                            var targety = 0;
                            //延直线扩展的单位
                            var dirx = 0;
                            var diry = 0;
                            var noedeindex = 0;
                            var sqrtdistance = 0;
                            if(connection[d.source.index] >= connection[d.target.index]) {
                                noedeindex = Number(d.source.index);
                                originx = xScale(d.source.x);
                                originy = yScale(d.source.y);
                                targetx = xScale(d.target.x);
                                targety = yScale(d.target.y);
                                var dx = xScale(d.target.x) - xScale(d.source.x);
                                var dy = yScale(d.target.y) - yScale(d.source.y);
                                sqrtdistance = Math.sqrt(dx * dx + dy * dy);
                                //console.log('sqrtdistance', sqrtdistance);
                                if(sqrtdistance == 0) {
                                    d.cy.push(originy);
                                    return originy;
                                }
                                dirx = dx / sqrtdistance;
                                diry = dy / sqrtdistance;
                            }
                            else {
                                noedeindex = Number(d.target.index);
                                originx = xScale(d.target.x);
                                originy = yScale(d.target.y);
                                targetx = xScale(d.source.x);
                                targety = yScale(d.source.y);
                                var dx = xScale(d.source.x) - xScale(d.target.x);
                                var dy = yScale(d.source.y) - yScale(d.target.y);
                                sqrtdistance = Math.sqrt(dx * dx + dy * dy);
                                //console.log('sqrtdistance', sqrtdistance);
                                if(sqrtdistance == 0) {
                                    d.cy.push(originy);
                                    return originy;
                                }
                                dirx = dx / sqrtdistance;
                                diry = dy / sqrtdistance;
                            }
                            var forwardEllipse = 0;
                            var localEllipse = that.widthScale(d.data[(numindex + 1) % dataNum]);
                            for(var ti = 0; ti < numindex; ti++) {
                                forwardEllipse = that.widthScale(d.data[(ti + 1) % dataNum]) * 2 + that.consx;
                            }

                            var result = originy + (xs[noedeindex] +forwardEllipse + localEllipse) * diry;
                            if((xs[noedeindex] + forwardEllipse + localEllipse) >= sqrtdistance) {
                                result = targety;
                                d.full = true; //放不下
                            }
                            else{
                                d.full = false;
                            }
                            d.cy.push(result);
                            return result;
                            
                            /*
                            if(connection[d.source.index] >= connection[d.target.index]){
                                d.cy.push((yScale(d.target.y) - yScale(d.source.y)) / (dataNum + 1) * (numindex + 1) + yScale(d.source.y));
                                return (yScale(d.target.y) - yScale(d.source.y)) / (dataNum + 1) * (numindex + 1) + yScale(d.source.y);
                            }
                            d.cy.push((yScale(d.source.y) - yScale(d.target.y)) / (dataNum + 1) * (numindex + 1) + yScale(d.target.y));
                            return (yScale(d.source.y) - yScale(d.target.y)) / (dataNum + 1) * (numindex + 1) + yScale(d.target.y);
                            */
                        })
                        .style("fill", function(d) {
                            return that.fillColor(d.allSigns[numindex] >= 0);
                            return "white";
                            if (d.allSigns[numindex] >= 0) {
                                return "#775AA9";
                            }
                            else {
                                return "#FE8F00";
                            }
                        })
                        .style("stroke-width", function(d) {
                            return 1;
                        }) //路径宽度
                        .style("stroke", function(d) {
                            return that.strokeColor(d.allSigns[numindex] >= 0);
                            if (d.allSigns[numindex] >= 0) {
                                return "#775AA9";
                            }
                            else {
                                return "#FE8F00";
                            }
                        })
                        .style("transform-origin", function(d) {
                            return (d.cx[numindex] / width * 100).toString() + "% "  +( d.cy[numindex] / height * 100).toString() +"%";
                        })
                        .style("transform", function(d) {
                            var deg = 0;
                            var source = [xScale(d.source.x), yScale(d.source.y)];
                            var target = [xScale(d.target.x), yScale(d.target.y)];
                            var x_distance = target[0] - source[0];
                            var y_distance = target[1] - source[1];
                            var c_distance = Math.pow(x_distance, 2) + Math.pow(y_distance, 2);
                            var distance = Math.sqrt(c_distance);
                            if (x_distance == 0) {
                                deg = 90;
                            }
                            else if (y_distance == 0) {
                                deg = 0;
                            } 
                            else {
                                var xielv = y_distance / x_distance;
                                var sinA = Math.abs(y_distance) / distance;
                                if (xielv < 0) {
                                    deg = 90 - Math.asin(sinA) * 180 / Math.PI;
                                }
                                else {
                                    deg = Math.asin(sinA) * 180 / Math.PI - 90;
                                    //console.log("-deg", deg);
                                }
                                
                            }
                            return "rotate("+ deg.toString() + "deg)";
                        })
                        //添加hover
                        .on("mouseenter", function(d, i) {
                        //console.log('mouseenter', d);
                        return tipsDiv
                            .style('visibility', 'visible')
                            //.style('left', (d3.event.clientX - 920) + 'px') //- 400
                            //.style('top', (d3.event.clientY - 300) + 'px') //- 110
                            .style('right', '14px')
                            //.style('top', '224px')
                            .style('bottom', '0px')
                            .html('ROI: ' + (d.source.index + 1) + '  <->  ' + (d.target.index + 1) + '<br>' + 'value: ' + d.thick.toFixed(5) + '<br>' + 'allDatas: ' + d.data);//tooltip.html(text);
                        })
                        .on("mouseout", function(d, i) {
                            //return tooltip.style('visibility', 'hidden');
                            return tipsDiv.style('visibility', 'hidden');
                        })
                        //添加点击事件
                        .on("click", function(d){

                        });

                        //添加线
                        var classname2 = ".rowlink" + numindex;
                        ellipse.append("line")
                            .attr("class", function(d,i) {
                                return ".rowlink" + numindex;
                            })
                            .style("stroke-width", function(d) {
                                if (that.reverse){
                                    if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                        if (d.thick == 0) return 0;
                                        if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray)) {
                                            return 1;
                                        }
                                        return 0;
                                    }
                                    else {
                                        return 0;
                                    }
                                }
                                else return 4 - (d.thick - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 4 
                            })  //路径宽度
                            .style("stroke", function(d) {
                                return that.lineColor(d.allSigns[numindex] >= 0);
                                if (d.allSigns[numindex] >= 0) {
                                    return "#775AA9";
                                }
                                else {
                                    return "#FE8F00";
                                }
                            })
                            .style("stroke-opacity", function(d){ //不透明度
                                return 1;
                            })
                            .attr("x1", function(d){
                                var rx = 0;
                                var ry = 0;
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        rx = that.widthScale(d.data[numindex]);
                                        //rx = Math.abs(d.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        ry = that.widthScale(d.data[(numindex + 1) % dataNum]);
                                        //ry = Math.abs(d.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                return d.cx[numindex] + rx;
                            })
                            .attr("y1", function(d){
                                var rx = 0;
                                var ry = 0;
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        rx = that.widthScale(d.data[numindex]);
                                        //rx = Math.abs(d.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        ry = that.widthScale(d.data[(numindex + 1) % dataNum]);
                                        //ry = Math.abs(d.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                return d.cy[numindex];
                            })
                            .attr("x2", function(d){
                                var rx = 0;
                                var ry = 0;
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        rx = that.widthScale(d.data[numindex]);
                                        //rx = Math.abs(d.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        ry = that.widthScale(d.data[(numindex + 1) % dataNum]);
                                        //ry = Math.abs(d.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                return d.cx[numindex] - rx;
                            })
                            .attr("y2", function(d){
                                var rx = 0;
                                var ry = 0;
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        rx = that.widthScale(d.data[numindex]);
                                        //rx = Math.abs(d.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        ry = that.widthScale(d.data[(numindex + 1) % dataNum]);
                                        //ry = Math.abs(d.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                return d.cy[numindex];
                            })
                            .style("transform-origin", function(d) {
                                    return (d.cx[numindex] / width * 100).toString() + "% "  +( d.cy[numindex] / height * 100).toString() +"%";
                            })
                            .style("transform", function(d) {
                                var deg = 0;
                                var source = [xScale(d.source.x), yScale(d.source.y)];
                                var target = [xScale(d.target.x), yScale(d.target.y)];
                                var x_distance = target[0] - source[0];
                                var y_distance = target[1] - source[1];
                                var c_distance = Math.pow(x_distance, 2) + Math.pow(y_distance, 2);
                                var distance = Math.sqrt(c_distance);
                                if (x_distance == 0) {
                                    deg = 90;
                                }
                                else if (y_distance == 0) {
                                    deg = 0;
                                } 
                                else {
                                    var xielv = y_distance / x_distance;
                                    var sinA = Math.abs(y_distance) / distance;
                                    if (xielv < 0) {
                                        deg = 90 - Math.asin(sinA) * 180 / Math.PI;
                                    }
                                    else {
                                        deg = Math.asin(sinA) * 180 / Math.PI - 90;
                                        //console.log("-deg", deg);
                                    }
                                
                                }
                                return "rotate("+ deg.toString() + "deg)";
                            });
                    }

                //console.log("that.matrix.links", that.matrix.links);
                /*
                var ellipse = [];
                ellipse[0] = svgLeft.selectAll(".ellipse")
                    .data(that.matrix.links)
                    .enter()
                    .append("ellipse")
                    .attr("class", function(d,i){
                        return "Ellipse" + d.source.index + "_" + d.target.index;
                    })
                    .attr("rx", 9)
                    .attr("ry", 4)
                    .style("fill", "white")
                    .style("stroke", "black")
                    .style("stroke-width", function(d){
                        if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                            if (d.thick == 0) return 0;
                            if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                return 1;
                            return 0;
                        }
                        else {
                            return 0;
                        }
                    })
                    .style("opacity", function(d){ //不透明度
                        if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                            if (d.thick == 0) return 0;
                            if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                return 1;
                            return 0;
                        }
                        else {
                            return 0;
                        }
                    })
                    .sort(function(a, b) {
                        return a.thick - b.thick;
                    });
                */
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
                        
                    })
                    .sort(function(a, b) {
                        return connection[a.index] - connection[b.index];
                    });


                //添加图标显示节点
                nodeLeft.append("circle")
                    .attr("class", function(d,i){
                        return "lobe" + that.clusterNo(i, clusters) + "Circle" + that.index;
                    })
                    .attr("r", 9)
                    .style("fill", "white")
                    .style("stroke", "black")
                    .style("stroke-width", 2);

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
                    .style("font-size", "13px")
                    .style("font-weight", 300)
                    .text(function(d) {
                        return d.index + 1;
                    });

                //添加虚线
                svgLeft.selectAll(".dottedlink")
                    .data(that.matrix.links)
                    .enter()
                    .append("line")
                    .attr("class", function(d,i) {
                        return "leftl" + that.clusterNo(d.source.index, clusters) + "l" + that.clusterNo(d.target.index, clusters) + "leftl" + that.clusterNo(d.source.index, clusters) + "leftl" + that.clusterNo(d.target.index, clusters) +" dottedlink";
                    })
                    .style("stroke-width", function(d){
                        if(d.full == true && d.thick > 0 && d.thick >= that.fillMin) return 1;
                        return 0;
                    })  //路径宽度
                    .style("stroke", function(d){//路径颜色
                        return "red";
                    })
                    .style("stroke-opacity", function(d){ //不透明度
                        if(d.full == true && d.thick > 0 && d.thick >= that.fillMin) return 1;
                        return 0;
                    })
                    .style("stroke-dasharray", "3")
                    .sort(function(a, b) {
                        return a.thick - b.thick;
                    })
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

                //添加mask
                var rect = svgLeft.selectAll(".rect")
                    .data([sizeobj])
                    .enter()
                    .append("rect")
                    .attr("class", function(d,i) {
                        return "rect";
                    })
                    .style("fill", "white")
                    .attr("x", 0)
                    .attr("y", 0)
                    .attr("width", 400)
                    .attr("height", 452)
                    .style("opacity", function(d){ //不透明度
                        return 0;
                    })
                    .on("click", function(d){
                        //console.log('strength', newVal);
                        var linkcount = 0;
                        for (var i in that.selectedLink) {
                            linkcount++;
                        }
                        if(linkcount > 0) {
                            //清空并设置透明
                            console.log("select link");
                            that.selectedLink = {};
                            svgLeft.selectAll(".nodeup").filter(function(d1, i){
                                return that.selectedNodes[d1.index] == null;
                            }).style("opacity", 0);
                            svgLeft.selectAll(".nodeup").filter(function(d1, i){
                                return that.selectedNodes[d1.index] == null;
                            }).select("circle").style("stroke", "black");

                            //未选择的线不显示
                            svgLeft.selectAll(".polygonup").style("opacity",  0);
                            svgLeft.selectAll(".rect").style("opacity", 0);
                            return;
                        }
                        that.loading = true;
                        setTimeout(function(){
                        //清空
                        console.log('click mask');
                        var nodecount = 0
                        var tmpselect = {};
                        for(var i in that.selectedNodes) {
                            connection[i] = connection[i] - 3000;
                            nodecount++;
                            tmpselect[i] = that.selectedNodes[i];
                        }
                        that.selectedNodes = {};

                        if(nodecount > 0) {
                        //找到与选择节点相连的三角线重画
                        var polygon = svgLeft.selectAll(".polygon");
                        polygon.filter(function(d1) {
                            return tmpselect[d1.target.index] != null ||  tmpselect[d1.source.index] != null;
                            //return d1.target.index == d.index || d1.source.index == d.index;
                        })
                        .select("polygon")
                        .attr("points", function(d1){
                            var source = [xScale(d1.source.x), yScale(d1.source.y)];
                            var target = [xScale(d1.target.x), yScale(d1.target.y)];
                            var x_distance = target[0] - source[0];
                            var y_distance = target[1] - source[1];
                            var c_distance = Math.pow(x_distance, 2) + Math.pow(y_distance, 2);
                            var distance = Math.sqrt(c_distance);
                            //当y_distance = 0时，即平行于x轴, y值直接变化
                            if(y_distance == 0) {
                                if(connection[d1.source.index] >= connection[d1.target.index])
                                    return xScale(d1.source.x).toString() + ',' + (yScale(d1.source.y) - 2).toString() + " " + 
                                        xScale(d1.target.x).toString() + ',' + (yScale(d1.target.y) - 0.5).toString() + " " +
                                        xScale(d1.target.x).toString() + ',' + (yScale(d1.target.y) + 0.5).toString() + " " +
                                        xScale(d1.source.x).toString() + ',' + (yScale(d1.source.y) + 2).toString() + " " ;
                                    else return xScale(d1.target.x).toString() + ',' + (yScale(d1.target.y) - 2).toString() + " " + 
                                        xScale(d1.source.x).toString() + ',' + (yScale(d1.source.y) - 0.5).toString() + " " +
                                        xScale(d1.source.x).toString() + ',' + (yScale(d1.source.y) + 0.5).toString() + " " +
                                        xScale(d1.target.x).toString() + ',' + (yScale(d1.target.y) + 2).toString() + " " ;
                            }
                            //当x_disatnce = 0, 即垂直于x轴时，x值变化
                            if(x_distance == 0) {
                                if(connection[d1.source.index] >= connection[d1.target.index])
                                    return (xScale(d1.source.x) - 2).toString() + ',' + (yScale(d1.source.y)).toString() + " " + 
                                        (xScale(d1.target.x) - 0.5).toString() + ',' + (yScale(d1.target.y)).toString() + " " +
                                        (xScale(d1.target.x) + 0.5).toString() + ',' + (yScale(d1.target.y)).toString() + " " +
                                        (xScale(d1.source.x) + 2).toString() + ',' + (yScale(d1.source.y)).toString() + " " ;
                                else return (xScale(d1.target.x) - 2).toString() + ',' + (yScale(d1.target.y)).toString() + " " + 
                                    (xScale(d1.source.x) - 0.5).toString() + ',' + (yScale(d1.source.y)).toString() + " " +
                                    (xScale(d1.source.x) + 0.5).toString() + ',' + (yScale(d1.source.y)).toString() + " " +
                                    (xScale(d1.target.x) + 2).toString() + ',' + (yScale(d1.target.y)).toString() + " " ;
                            }
                            //否则计算斜率
                            var xielv = y_distance / x_distance;
                            //计算夹角,A为垂线与水平线夹角，锐角
                            var sinA = Math.abs(y_distance) / distance;
                            var cosA = Math.abs(x_distance) / distance;
                            //var bsource = source[1] + source[0] / xielv;
                            //var btarget = target[1] + target[0] / xielv;
                            if(xielv > 0){
                                if(connection[d1.source.index] >= connection[d1.target.index])
                                    return (source[0] - 2 * sinA).toString() + ',' + (source[1] + 2 * cosA).toString() + " " + 
                                        (target[0] - 0.5 * sinA).toString() + ',' + (target[1] + 0.5 * cosA).toString() + " " +
                                        (target[0] + 0.5 * sinA).toString() + ',' + (target[1] - 0.5 * cosA).toString() + " " +
                                        (source[0] + 2 * sinA).toString() + ',' + (source[1] - 2 * cosA).toString() + " " ;
                                else return (target[0] - 2 * sinA).toString() + ',' + (target[1] + 2 * cosA).toString() + " " + 
                                        (source[0] - 0.5 * sinA).toString() + ',' + (source[1] + 0.5 * cosA).toString() + " " +
                                        (source[0] + 0.5 * sinA).toString() + ',' + (source[1] - 0.5 * cosA).toString() + " " +
                                        (target[0] + 2 * sinA).toString() + ',' + (target[1] - 2 * cosA).toString() + " " ;
                            }
                            else {
                                if(connection[d1.source.index] >= connection[d1.target.index])
                                    return (source[0] - 2 * sinA).toString() + ',' + (source[1] - 2 * cosA).toString() + " " + 
                                        (target[0] - 0.5 * sinA).toString() + ',' + (target[1] - 0.5 * cosA).toString() + " " +
                                        (target[0] + 0.5 * sinA).toString() + ',' + (target[1] + 0.5 * cosA).toString() + " " +
                                        (source[0] + 2 * sinA).toString() + ',' + (source[1] + 2 * cosA).toString() + " " ;
                                else return (target[0] - 2 * sinA).toString() + ',' + (target[1] - 2 * cosA).toString() + " " + 
                                        (source[0] - 0.5 * sinA).toString() + ',' + (source[1] - 0.5 * cosA).toString() + " " +
                                        (source[0] + 0.5 * sinA).toString() + ',' + (source[1] + 0.5 * cosA).toString() + " " +
                                        (target[0] + 2 * sinA).toString() + ',' + (target[1] + 2 * cosA).toString() + " " ;
                            }
                        });
                        var polygonup = svgLeft.selectAll(".polygonup");
                        polygonup.filter(function(d1) {
                            return tmpselect[d1.target.index] != null ||  tmpselect[d1.source.index] != null;
                            //return d1.target.index == d.index || d1.source.index == d.index;
                        })
                        .select("polygon")
                        .attr("points", function(d1){
                            var source = [xScale(d1.source.x), yScale(d1.source.y)];
                            var target = [xScale(d1.target.x), yScale(d1.target.y)];
                            var x_distance = target[0] - source[0];
                            var y_distance = target[1] - source[1];
                            var c_distance = Math.pow(x_distance, 2) + Math.pow(y_distance, 2);
                            var distance = Math.sqrt(c_distance);
                            //当y_distance = 0时，即平行于x轴, y值直接变化
                            if(y_distance == 0) {
                                if(connection[d1.source.index] >= connection[d1.target.index])
                                    return xScale(d1.source.x).toString() + ',' + (yScale(d1.source.y) - 2).toString() + " " + 
                                        xScale(d1.target.x).toString() + ',' + (yScale(d1.target.y) - 0.5).toString() + " " +
                                        xScale(d1.target.x).toString() + ',' + (yScale(d1.target.y) + 0.5).toString() + " " +
                                        xScale(d1.source.x).toString() + ',' + (yScale(d1.source.y) + 2).toString() + " " ;
                                    else return xScale(d1.target.x).toString() + ',' + (yScale(d1.target.y) - 2).toString() + " " + 
                                        xScale(d1.source.x).toString() + ',' + (yScale(d1.source.y) - 0.5).toString() + " " +
                                        xScale(d1.source.x).toString() + ',' + (yScale(d1.source.y) + 0.5).toString() + " " +
                                        xScale(d1.target.x).toString() + ',' + (yScale(d1.target.y) + 2).toString() + " " ;
                            }
                            //当x_disatnce = 0, 即垂直于x轴时，x值变化
                            if(x_distance == 0) {
                                if(connection[d1.source.index] >= connection[d1.target.index])
                                    return (xScale(d1.source.x) - 2).toString() + ',' + (yScale(d1.source.y)).toString() + " " + 
                                        (xScale(d1.target.x) - 0.5).toString() + ',' + (yScale(d1.target.y)).toString() + " " +
                                        (xScale(d1.target.x) + 0.5).toString() + ',' + (yScale(d1.target.y)).toString() + " " +
                                        (xScale(d1.source.x) + 2).toString() + ',' + (yScale(d1.source.y)).toString() + " " ;
                                else return (xScale(d1.target.x) - 2).toString() + ',' + (yScale(d1.target.y)).toString() + " " + 
                                    (xScale(d1.source.x) - 0.5).toString() + ',' + (yScale(d1.source.y)).toString() + " " +
                                    (xScale(d1.source.x) + 0.5).toString() + ',' + (yScale(d1.source.y)).toString() + " " +
                                    (xScale(d1.target.x) + 2).toString() + ',' + (yScale(d1.target.y)).toString() + " " ;
                            }
                            //否则计算斜率
                            var xielv = y_distance / x_distance;
                            //计算夹角,A为垂线与水平线夹角，锐角
                            var sinA = Math.abs(y_distance) / distance;
                            var cosA = Math.abs(x_distance) / distance;
                            //var bsource = source[1] + source[0] / xielv;
                            //var btarget = target[1] + target[0] / xielv;
                            if(xielv > 0){
                                if(connection[d1.source.index] >= connection[d1.target.index])
                                    return (source[0] - 2 * sinA).toString() + ',' + (source[1] + 2 * cosA).toString() + " " + 
                                        (target[0] - 0.5 * sinA).toString() + ',' + (target[1] + 0.5 * cosA).toString() + " " +
                                        (target[0] + 0.5 * sinA).toString() + ',' + (target[1] - 0.5 * cosA).toString() + " " +
                                        (source[0] + 2 * sinA).toString() + ',' + (source[1] - 2 * cosA).toString() + " " ;
                                else return (target[0] - 2 * sinA).toString() + ',' + (target[1] + 2 * cosA).toString() + " " + 
                                        (source[0] - 0.5 * sinA).toString() + ',' + (source[1] + 0.5 * cosA).toString() + " " +
                                        (source[0] + 0.5 * sinA).toString() + ',' + (source[1] - 0.5 * cosA).toString() + " " +
                                        (target[0] + 2 * sinA).toString() + ',' + (target[1] - 2 * cosA).toString() + " " ;
                            }
                            else {
                                if(connection[d1.source.index] >= connection[d1.target.index])
                                    return (source[0] - 2 * sinA).toString() + ',' + (source[1] - 2 * cosA).toString() + " " + 
                                        (target[0] - 0.5 * sinA).toString() + ',' + (target[1] - 0.5 * cosA).toString() + " " +
                                        (target[0] + 0.5 * sinA).toString() + ',' + (target[1] + 0.5 * cosA).toString() + " " +
                                        (source[0] + 2 * sinA).toString() + ',' + (source[1] + 2 * cosA).toString() + " " ;
                                else return (target[0] - 2 * sinA).toString() + ',' + (target[1] - 2 * cosA).toString() + " " + 
                                        (source[0] - 0.5 * sinA).toString() + ',' + (source[1] - 0.5 * cosA).toString() + " " +
                                        (source[0] + 0.5 * sinA).toString() + ',' + (source[1] + 0.5 * cosA).toString() + " " +
                                        (target[0] + 2 * sinA).toString() + ',' + (target[1] + 2 * cosA).toString() + " " ;
                            }
                        });
                        //假设为选择，则更改椭圆位置
                        //if(that.selectedNodes[d.index] == 1) {
                        if(1) {
                        var xs = []; //存储每个节点的x
                        var scores = []; //存储每个节点的score
                        for(var i = 0; i < 70; i++) {
                            xs.push(that.minx);
                            scores.push(0);
                        }
                        if(dataNum > 0) {
                            //对于每个节点计算它的score
                            for(var i = 0; i < 70; i++) {
                                //假设节点被选中，则其x=that.minx,否则计算x
                                if(that.selectedNodes[i] == 1) {
                                    xs[i] = that.minx;
                                }
                                else {
                                    //计算maxx
                                    maxx = 999;
                                    var count = 0;
                                    that.matrix.links.filter(function(d1){
                                        return ((d1.source.index == i && connection[d1.source.index] >= connection[d1.target.index]) || (d1.target.index == i && connection[d1.source.index] <= connection[d1.target.index])) && Math.abs(d1.thick) > 0;
                                    }).forEach(function(d1){
                                        var dx = xScale(d1.target.x) - xScale(d1.source.x);
                                        var dy = yScale(d1.target.y) - yScale(d1.source.y);
                                        var sqrtdistance = Math.sqrt(dx * dx + dy * dy);
                                        //maxx += sqrtdistance;
                                        for(var si = 0; si < dataNum; si++) {
                                            sqrtdistance -= (that.widthScale(d1.data[si]) * 2);
                                        }
                                        if(sqrtdistance < 0) sqrtdistance = 0;
                                        if(sqrtdistance < maxx) maxx = sqrtdistance;
                                        count++;
                                    });
                                    //if(count != 0) maxx = maxx / count;
                                    maxx = maxx - that.consx * (dataNum - 1);
                                    if(maxx < 0) maxx = 0;
                                    //计算每一步寻找最大的score
                                    for(var x = that.minx; x < maxx; x=x+that.step) {
                                        var score = 0;
                                        //找到所有与该节点相连的线，该节点重要性大于另一节点
                                        that.matrix.links.filter(function(d1){
                                            return ((d1.source.index == i && connection[d1.source.index] >= connection[d1.target.index]) || (d1.target.index == i && connection[d1.source.index] <= connection[d1.target.index])) && Math.abs(d1.thick) > 0;
                                        }).forEach(function(d2){
                                            //对于该线上的所有椭圆，计算minscore
                                            //节点原始坐标
                                            var originx = 0;
                                            var originy = 0;
                                            //延直线扩展的单位
                                            var dirx = 0;
                                            var diry = 0;
                                            if(d2.source.index == i) {
                                                originx = xScale(d2.source.x);
                                                originy = yScale(d2.source.y);
                                                var dx = xScale(d2.target.x) - xScale(d2.source.x);
                                                var dy = yScale(d2.target.y) - yScale(d2.source.y);
                                                var sqrtdistance = Math.sqrt(dx * dx + dy * dy);
                                                dirx = dx / sqrtdistance;
                                                diry = dy / sqrtdistance;
                                            }
                                            else {
                                                originx = xScale(d2.target.x);
                                                originy = yScale(d2.target.y);
                                                var dx = xScale(d2.source.x) - xScale(d2.target.x);
                                                var dy = yScale(d2.source.y) - yScale(d2.target.y);
                                                var sqrtdistance = Math.sqrt(dx * dx + dy * dy);
                                                dirx = dx / sqrtdistance;
                                                diry = dy / sqrtdistance;
                                            }
                                            //对于线上每个椭圆
                                            for (var j = 0; j < dataNum; j++) {
                                                var minscore = 9999;
                                                var x0 = originx + (x + j * that.consx) * dirx;
                                                var y0 = originy + (x + j * that.consx) * diry;
                                                //找到与该线条不同的所有线,计算点到直线的最小距离
                                                that.matrix.links.filter(function(d1){
                                                    return !(d1.in == d2.in) && Math.abs(d1.thick) > 0;
                                                }).forEach(function(d1) {
                                                    //var x0 = originx + (x + j * that.consx) * dirx;
                                                    //var y0 = originy + (x + j * that.consx) * diry;
                                                    var distance = that.caldistance(x0, y0, xScale(d1.source.x), yScale(d1.source.y), xScale(d1.target.x), yScale(d1.target.y));
                                                    if (distance < minscore) {
                                                        minscore = distance;
                                                    }
                                                });
                                                score = score + that.alpha * minscore;
                                                var minEM = 999;
                                                //找到高亮的所有线，计算点到线上椭圆的最小距离
                                                that.matrix.links.filter(function(d1){
                                                    return (that.selectedNodes[d1.source.index] == 1 || that.selectedNodes[d1.target.index] == 1) && Math.abs(d1.thick) > 0;
                                                }).forEach(function(d1) {
                                                    //计算出该条线上椭圆位置
                                                    var newpoints;
                                                    if(connection[d1.source.index] >= connection[d1.target.index]) {
                                                        //calposition(x0, y0, x1, y1, that.minx, constx, dataNum)
                                                        newpoints = that.calposition(xScale(d1.source.x), yScale(d1.source.y), xScale(d1.target.x), yScale(d1.target.y), that.minx, that.consx, dataNum);
                                                    }
                                                    else{
                                                        newpoints = that.calposition(xScale(d1.target.x), yScale(d1.target.y), xScale(d1.source.x), yScale(d1.source.y), that.minx, that.consx, dataNum);
                                                    }
                                                    for(var p in newpoints) {
                                                        var distance = that.pointdistance(newpoints[p][0], newpoints[p][1], x0, y0);
                                                        if (distance < minEM) {
                                                            minEM = distance;
                                                        }
                                                    }
                                                });
                                                score = score + (1 - that.alpha) * minEM;
                                            }
                                        });
                                        if(score > scores[i]) {
                                            scores[i] = score;
                                            xs[i] = x;
                                        }
                                    }
                                }
                            }
                        }
                        console.log('xs', xs);
                        //重画所有的椭圆
                        polygon.selectAll(".ellipse").remove();
                        for(var numindex = 0; numindex < dataNum; numindex++) {
                            var classname = ".ellipse" + numindex;
                            var ellipse = polygon.append("g")
                                .attr("class", function(d1,i){
                                    return "ellipse" + numindex + " ellipse";
                                });
                            ellipse.append("ellipse")
                                .attr("rx", function(d1){
                                    //console.log('d.data', d.data[numindex]);
                                    if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                        if (d1.thick == 0) return 0;
                                        if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray)){
                                            return that.widthScale(d1.data[numindex]);
                                            //return Math.abs(d1.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        return 0;
                                    }
                                    else {
                                        return 0;
                                    }            
                                })
                                .attr("ry", function(d1) {
                                    if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                        if (d1.thick == 0) return 0;
                                        if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray)){
                                            return that.widthScale(d1.data[(numindex+1)%dataNum]);
                                            //return Math.abs(d1.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        return 0;
                                    }
                                    else {
                                        return 0;
                                    }
                                })
                                .attr("cx", function(d1) {            
                                    var originx = 0;
                                    var originy = 0;
                                    var targetx = 0;
                                    var targety = 0;
                                    //延直线扩展的单位
                                    var dirx = 0;
                                    var diry = 0;
                                    var noedeindex = 0;
                                    var sqrtdistance = 0;
                                    if(connection[d1.source.index] >= connection[d1.target.index]) {
                                        noedeindex = Number(d1.source.index);
                                        originx = xScale(d1.source.x);
                                        originy = yScale(d1.source.y);
                                        targetx = xScale(d1.target.x);
                                        targety = yScale(d1.target.y);
                                        var dx = xScale(d1.target.x) - xScale(d1.source.x);
                                        var dy = yScale(d1.target.y) - yScale(d1.source.y);
                                        sqrtdistance = Math.sqrt(dx * dx + dy * dy);
                                        if(sqrtdistance == 0) {
                                            d1.cx[numindex] = originx;
                                            return originx;
                                        }
                                        dirx = dx / sqrtdistance;
                                        diry = dy / sqrtdistance;
                                    }
                                    else {
                                        noedeindex = Number(d1.target.index);
                                        originx = xScale(d1.target.x);
                                        originy = yScale(d1.target.y);
                                        targetx = xScale(d1.source.x);
                                        targety = yScale(d1.source.y);
                                        var dx = xScale(d1.source.x) - xScale(d1.target.x);
                                        var dy = yScale(d1.source.y) - yScale(d1.target.y);
                                        sqrtdistance = Math.sqrt(dx * dx + dy * dy);
                                        if(sqrtdistance == 0) {
                                            d1.cx[numindex] = originx;
                                            return originx;
                                        }
                                        dirx = dx / sqrtdistance;
                                        diry = dy / sqrtdistance;
                                    }
                                    var forwardEllipse = 0;
                                    var localEllipse = that.widthScale(d1.data[(numindex + 1) % dataNum]);
                                    for(var ti = 0; ti < numindex; ti++) {
                                        forwardEllipse = that.widthScale(d1.data[(ti + 1) % dataNum]) * 2 + that.consx;
                                    }
                                    var result = originx + (xs[noedeindex] +forwardEllipse + localEllipse) * dirx;
                                    if((xs[noedeindex] + forwardEllipse + localEllipse) >= sqrtdistance) {
                                        result = targetx;
                                        d1.full = true; //放不下
                                    }
                                    /*
                                    var result = originx + (xs[noedeindex] + numindex * that.consx) * dirx;
                                    if((xs[noedeindex] + numindex * that.consx) >= sqrtdistance) {
                                        result = targetx;
                                        d1.full = true; //放不下
                                    }
                                    */
                                    else {
                                        d1.full = false;
                                    }
                                    d1.cx[numindex] = result;
                                    //d.cx.push(result);
                                    //console.log('result', result);
                                    return result;
                                })
                                .attr("cy", function(d1){
                                    var originx = 0;
                                    var originy = 0;
                                    var targetx = 0;
                                    var targety = 0;
                                    //延直线扩展的单位
                                    var dirx = 0;
                                    var diry = 0;
                                    var noedeindex = 0;
                                    var sqrtdistance = 0;
                                    if(connection[d1.source.index] >= connection[d1.target.index]) {
                                        noedeindex = Number(d1.source.index);
                                        originx = xScale(d1.source.x);
                                        originy = yScale(d1.source.y);
                                        targetx = xScale(d1.target.x);
                                        targety = yScale(d1.target.y);
                                        var dx = xScale(d1.target.x) - xScale(d1.source.x);
                                        var dy = yScale(d1.target.y) - yScale(d1.source.y);
                                        sqrtdistance = Math.sqrt(dx * dx + dy * dy);
                                        //console.log('sqrtdistance', sqrtdistance);
                                        if(sqrtdistance == 0) {
                                            d1.cy[numindex] = originy;
                                            return originy;
                                        }
                                        dirx = dx / sqrtdistance;
                                        diry = dy / sqrtdistance;
                                    }
                                    else {
                                        noedeindex = Number(d1.target.index);
                                        originx = xScale(d1.target.x);
                                        originy = yScale(d1.target.y);
                                        targetx = xScale(d1.source.x);
                                        targety = yScale(d1.source.y);
                                        var dx = xScale(d1.source.x) - xScale(d1.target.x);
                                        var dy = yScale(d1.source.y) - yScale(d1.target.y);
                                        sqrtdistance = Math.sqrt(dx * dx + dy * dy);
                                        //console.log('sqrtdistance', sqrtdistance);
                                        if(sqrtdistance == 0) {
                                            d1.cy[numindex] = originy;
                                            return originy;
                                        }
                                        dirx = dx / sqrtdistance;
                                        diry = dy / sqrtdistance;
                                    }
                                    var forwardEllipse = 0;
                                    var localEllipse = that.widthScale(d1.data[(numindex + 1) % dataNum]);
                                    for(var ti = 0; ti < numindex; ti++) {
                                        forwardEllipse = that.widthScale(d1.data[(ti + 1) % dataNum]) * 2 + that.consx;
                                    }
                                    var result = originy + (xs[noedeindex] +forwardEllipse + localEllipse) * diry;
                                    if((xs[noedeindex] + forwardEllipse + localEllipse) >= sqrtdistance) {
                                        result = targety;
                                        d1.full = true; //放不下
                                    }
                                    /*
                                    var result = originy + (xs[noedeindex] + numindex * that.consx) * diry;
                                    if((xs[noedeindex] + numindex * that.consx) >= sqrtdistance) {
                                        result = targety;
                                        d1.full = true; //放不下
                                    }
                                    */
                                    else {
                                        d1.full = false;
                                    }
                                    d1.cy[numindex] = result;
                                    return result;
                                })
                                .style("fill", function(d1) {
                                    return that.fillColor(d1.allSigns[numindex] >= 0);
                                    return "white";
                                    if (d1.allSigns[numindex] >= 0) {
                                        return "#775AA9";
                                    }
                                    else {
                                        return "#FE8F00";
                                    }
                                })
                                .style("stroke-width", function(d1) {
                                    return 1;
                                }) //路径宽度
                                .style("stroke", function(d1) {
                                    return that.strokeColor(d1.allSigns[numindex] >= 0);
                                    if (d1.allSigns[numindex] >= 0) {
                                        return "#775AA9";
                                    }
                                    else {
                                        return "#FE8F00";
                                    }
                                })
                                .style("transform-origin", function(d1) {
                                    return (d1.cx[numindex] / width * 100).toString() + "% "  +( d1.cy[numindex] / height * 100).toString() +"%";
                                })
                                .style("transform", function(d1) {
                                    var deg = 0;
                                    var source = [xScale(d1.source.x), yScale(d1.source.y)];
                                    var target = [xScale(d1.target.x), yScale(d1.target.y)];
                                    var x_distance = target[0] - source[0];
                                    var y_distance = target[1] - source[1];
                                    var c_distance = Math.pow(x_distance, 2) + Math.pow(y_distance, 2);
                                    var distance = Math.sqrt(c_distance);
                                    if (x_distance == 0) {
                                        deg = 90;
                                    }
                                    else if (y_distance == 0) {
                                        deg = 0;
                                    } 
                                    else {
                                        var xielv = y_distance / x_distance;
                                        var sinA = Math.abs(y_distance) / distance;
                                        if (xielv < 0) {
                                            deg = 90 - Math.asin(sinA) * 180 / Math.PI;
                                        }
                                        else {
                                            deg = Math.asin(sinA) * 180 / Math.PI - 90;
                                            //console.log("-deg", deg);
                                        }
                                    }
                                    return "rotate("+ deg.toString() + "deg)";
                                })
                                //添加hover
                                .on("mouseenter", function(d1, i) {
                                    //console.log('mouseenter', d);
                                    return tipsDiv
                                        .style('visibility', 'visible')
                                        //.style('left', (d3.event.clientX - 920) + 'px') //- 400
                                        //.style('top', (d3.event.clientY - 300) + 'px') //- 110
                                        .style('right', '14px')
                                        //.style('top', '224px')
                                        .style('bottom', '0px')
                                        .html('ROI: ' + (d1.source.index + 1) + '  <->  ' + (d1.target.index + 1) + '<br>' + 'value: ' + d1.thick.toFixed(5) + '<br>' + 'allDatas: ' + d1.data);//tooltip.html(text);
                                    })
                                    .on("mouseout", function(d1, i) {
                                        //return tooltip.style('visibility', 'hidden');
                                        return tipsDiv.style('visibility', 'hidden');
                                    });
                                ellipse.append("line")
                                    .attr("class", function(d1,i) {
                                        return ".rowlink" + numindex;
                                    })
                                   .style("stroke-width", function(d1) {
                                        if (that.reverse){
                                            if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                                if (d1.thick == 0) return 0;
                                                if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray)) {
                                                    return 1;
                                                }
                                                return 0;
                                            }
                                            else {
                                                return 0;
                                            }
                                        }
                                        else return 4 - (d1.thick - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 4 
                                    })  //路径宽度
                                    .style("stroke", function(d1) {
                                        return that.lineColor(d1.allSigns[numindex] >= 0);
                                        if (d1.allSigns[numindex] >= 0) {
                                            return "#775AA9";
                                        }
                                        else {
                                            return "#FE8F00";
                                        }
                                    })
                                    .style("stroke-opacity", function(d1){ //不透明度
                                        return 1;
                                    })
                                    .attr("x1", function(d1){
                                        var rx = 0;
                                        var ry = 0;
                                        if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                            if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                                rx = that.widthScale(d1.data[numindex]);
                                                //rx = Math.abs(d1.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                            if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                                ry = that.widthScale(d1.data[(numindex+1)%dataNum]);
                                                //ry = Math.abs(d1.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        return d1.cx[numindex] + rx;
                                    })
                                    .attr("y1", function(d1){
                                        var rx = 0;
                                        var ry = 0;
                                        if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                            if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                                rx = that.widthScale(d1.data[numindex]);
                                                //rx = Math.abs(d1.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                            if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                                ry = that.widthScale(d1.data[(numindex+1)%dataNum]);
                                                //ry = Math.abs(d1.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        return d1.cy[numindex];
                                    })
                                    .attr("x2", function(d1){
                                        var rx = 0;
                                        var ry = 0;
                                        if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                            if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                                rx = that.widthScale(d1.data[numindex]);
                                                //rx = Math.abs(d1.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                            if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                                ry = that.widthScale(d1.data[(numindex+1)%dataNum]);
                                                //ry = Math.abs(d1.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        return d1.cx[numindex] - rx;
                                    })
                                    .attr("y2", function(d1){
                                        var rx = 0;
                                        var ry = 0;
                                        if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                            if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                                rx = that.widthScale(d1.data[numindex]);
                                                //rx = Math.abs(d1.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                            if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                                ry = that.widthScale(d1.data[(numindex+1)%dataNum]);
                                                //ry = Math.abs(d1.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        return d1.cy[numindex];
                                    })
                                    .style("transform-origin", function(d1) {
                                        return (d1.cx[numindex] / width * 100).toString() + "% "  +( d1.cy[numindex] / height * 100).toString() +"%";
                                    })
                                    .style("transform", function(d1) {
                                        var deg = 0;
                                        var source = [xScale(d1.source.x), yScale(d1.source.y)];
                                        var target = [xScale(d1.target.x), yScale(d1.target.y)];
                                        var x_distance = target[0] - source[0];
                                        var y_distance = target[1] - source[1];
                                        var c_distance = Math.pow(x_distance, 2) + Math.pow(y_distance, 2);
                                        var distance = Math.sqrt(c_distance);
                                        if (x_distance == 0) {
                                            deg = 90;
                                        }
                                        else if (y_distance == 0) {
                                           deg = 0;
                                        } 
                                        else {
                                            var xielv = y_distance / x_distance;
                                            var sinA = Math.abs(y_distance) / distance;
                                            if (xielv < 0) {
                                                deg = 90 - Math.asin(sinA) * 180 / Math.PI;
                                            }
                                            else {
                                                deg = Math.asin(sinA) * 180 / Math.PI - 90;
                                                //console.log("-deg", deg);
                                            }                
                                        }
                                        return "rotate("+ deg.toString() + "deg)";
                                    });
                        }

                        polygonup.selectAll(".ellipseup").remove();
                        for(var numindex = 0; numindex < dataNum; numindex++) {
                            var ellipse = polygonup.append("g")
                                .attr("class", function(d1,i){
                                    return "ellipseup" + numindex + " ellipseup";
                                });
                            ellipse.append("ellipse")
                                .attr("rx", function(d1){
                                    //console.log('d.data', d.data[numindex]);
                                    if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                        if (d1.thick == 0) return 0;
                                        if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray)){
                                            return that.widthScale(d1.data[numindex]);
                                            //return Math.abs(d1.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        return 0;
                                    }
                                    else {
                                        return 0;
                                    }            
                                })
                                .attr("ry", function(d1) {
                                    if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                        if (d1.thick == 0) return 0;
                                        if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray)){
                                            return that.widthScale(d1.data[(numindex+1)%dataNum]);
                                            //return Math.abs(d1.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        return 0;
                                    }
                                    else {
                                        return 0;
                                    }
                                })
                                .attr("cx", function(d1) {            
                                    return d1.cx[numindex];
                                })
                                .attr("cy", function(d1){
                                    return d1.cy[numindex];
                                })
                                .style("fill", function(d1) {
                                    return that.fillColor(d1.allSigns[numindex] >= 0);
                                    return "white";
                                    if (d1.allSigns[numindex] >= 0) {
                                        return "#775AA9";
                                    }
                                    else {
                                        return "#FE8F00";
                                    }
                                })
                                .style("stroke-width", function(d1) {
                                    return 1;
                                }) //路径宽度
                                .style("stroke", function(d1) {
                                    return that.strokeColor(d1.allSigns[numindex] >= 0);
                                    if (d1.allSigns[numindex] >= 0) {
                                        return "#775AA9";
                                    }
                                    else {
                                        return "#FE8F00";
                                    }
                                })
                                .style("transform-origin", function(d1) {
                                    return (d1.cx[numindex] / width * 100).toString() + "% "  +( d1.cy[numindex] / height * 100).toString() +"%";
                                })
                                .style("transform", function(d1) {
                                    var deg = 0;
                                    var source = [xScale(d1.source.x), yScale(d1.source.y)];
                                    var target = [xScale(d1.target.x), yScale(d1.target.y)];
                                    var x_distance = target[0] - source[0];
                                    var y_distance = target[1] - source[1];
                                    var c_distance = Math.pow(x_distance, 2) + Math.pow(y_distance, 2);
                                    var distance = Math.sqrt(c_distance);
                                    if (x_distance == 0) {
                                        deg = 90;
                                    }
                                    else if (y_distance == 0) {
                                        deg = 0;
                                    } 
                                    else {
                                        var xielv = y_distance / x_distance;
                                        var sinA = Math.abs(y_distance) / distance;
                                        if (xielv < 0) {
                                            deg = 90 - Math.asin(sinA) * 180 / Math.PI;
                                        }
                                        else {
                                            deg = Math.asin(sinA) * 180 / Math.PI - 90;
                                            //console.log("-deg", deg);
                                        }
                                    }
                                    return "rotate("+ deg.toString() + "deg)";
                                })
                                //添加hover
                                .on("mouseenter", function(d1, i) {
                                    //console.log('mouseenter', d);
                                    return tipsDiv
                                        .style('visibility', 'visible')
                                        //.style('left', (d3.event.clientX - 920) + 'px') //- 400
                                        //.style('top', (d3.event.clientY - 300) + 'px') //- 110
                                        .style('right', '14px')
                                        //.style('top', '224px')
                                        .style('bottom', '0px')
                                        .html('ROI: ' + (d1.source.index + 1) + '  <->  ' + (d1.target.index + 1) + '<br>' + 'value: ' + d1.thick.toFixed(5) + '<br>' + 'allDatas: ' + d1.data);//tooltip.html(text);
                                    })
                                    .on("mouseout", function(d1, i) {
                                        //return tooltip.style('visibility', 'hidden');
                                        return tipsDiv.style('visibility', 'hidden');
                                    })
                                    .call(that.drag);;
                                ellipse.append("line")
                                    .attr("class", function(d1,i) {
                                        return ".rowlink" + numindex;
                                    })
                                   .style("stroke-width", function(d1) {
                                        if (that.reverse){
                                            if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                                if (d1.thick == 0) return 0;
                                                if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray)) {
                                                    return 1;
                                                }
                                                return 0;
                                            }
                                            else {
                                                return 0;
                                            }
                                        }
                                        else return 4 - (d1.thick - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 4 
                                    })  //路径宽度
                                    .style("stroke", function(d1) {
                                        return that.lineColor(d1.allSigns[numindex] >= 0);
                                        if (d1.allSigns[numindex] >= 0) {
                                            return "#775AA9";
                                        }
                                        else {
                                            return "#FE8F00";
                                        }
                                    })
                                    .style("stroke-opacity", function(d1){ //不透明度
                                        return 1;
                                    })
                                    .attr("x1", function(d1){
                                        var rx = 0;
                                        var ry = 0;
                                        if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                            if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                                rx = that.widthScale(d1.data[numindex]);
                                                //rx = Math.abs(d1.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                            if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                                ry = that.widthScale(d1.data[(numindex+1)%dataNum]);
                                                //ry = Math.abs(d1.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        return d1.cx[numindex] + rx;
                                    })
                                    .attr("y1", function(d1){
                                        var rx = 0;
                                        var ry = 0;
                                        if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                            if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                                rx = that.widthScale(d1.data[numindex]);
                                                //rx = Math.abs(d1.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                            if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                                ry = that.widthScale(d1.data[(numindex+1)%dataNum]);
                                                //ry = Math.abs(d1.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        return d1.cy[numindex];
                                    })
                                    .attr("x2", function(d1){
                                        var rx = 0;
                                        var ry = 0;
                                        if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                            if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                                rx = that.widthScale(d1.data[numindex]);
                                                //rx = Math.abs(d1.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                            if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                                ry = that.widthScale(d1.data[(numindex+1)%dataNum]);
                                                //ry = Math.abs(d1.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        return d1.cx[numindex] - rx;
                                    })
                                    .attr("y2", function(d1){
                                        var rx = 0;
                                        var ry = 0;
                                        if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                            if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                                rx = that.widthScale(d1.data[numindex]);
                                                //rx = Math.abs(d1.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                            if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                                ry = that.widthScale(d1.data[(numindex+1)%dataNum]);
                                                //ry = Math.abs(d1.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        return d1.cy[numindex];
                                    })
                                    .style("transform-origin", function(d1) {
                                        return (d1.cx[numindex] / width * 100).toString() + "% "  +( d1.cy[numindex] / height * 100).toString() +"%";
                                    })
                                    .style("transform", function(d1) {
                                        var deg = 0;
                                        var source = [xScale(d1.source.x), yScale(d1.source.y)];
                                        var target = [xScale(d1.target.x), yScale(d1.target.y)];
                                        var x_distance = target[0] - source[0];
                                        var y_distance = target[1] - source[1];
                                        var c_distance = Math.pow(x_distance, 2) + Math.pow(y_distance, 2);
                                        var distance = Math.sqrt(c_distance);
                                        if (x_distance == 0) {
                                            deg = 90;
                                        }
                                        else if (y_distance == 0) {
                                           deg = 0;
                                        } 
                                        else {
                                            var xielv = y_distance / x_distance;
                                            var sinA = Math.abs(y_distance) / distance;
                                            if (xielv < 0) {
                                                deg = 90 - Math.asin(sinA) * 180 / Math.PI;
                                            }
                                            else {
                                                deg = Math.asin(sinA) * 180 / Math.PI - 90;
                                                //console.log("-deg", deg);
                                            }                
                                        }
                                        return "rotate("+ deg.toString() + "deg)";
                                    });
                            }

                        }


                        //选择的线显示,以及连接到的节点显示
                        svgLeft.selectAll(".polygonup").filter(function(d1) {
                            return that.selectedNodes[d1.source.index] == 1 || that.selectedNodes[d1.target.index] == 1;
                        }).style("opacity",  function(d1){
                            if (that.reverse){
                            if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                if (d1.thick == 0) return 0;
                                if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                    {
                                        d1.select = 1;
                                        svgLeft.selectAll(".nodeup").filter(function(d2, i){
                                            return d2.index == d1.source.index || d2.index == d1.target.index;
                                        }).style("opacity", 1);
                                        return 1;
                                    }
                                return 0;
                            }
                            else {
                                return 0;
                            }
                        }
                        else return 4 - (d1.thick - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 4 
                        });
                        //未选择的线不显示
                        svgLeft.selectAll(".polygonup").filter(function(d1) {
                            return !(that.selectedNodes[d1.source.index] == 1 || that.selectedNodes[d1.target.index] == 1);
                        }).style("opacity",  0);
                        //调整底层显示的虚线
                        svgLeft.selectAll(".dottedlink").style("stroke-opacity",  function(d1){
                            if(d1.full == true && d1.thick > 0 && d1.thick >= that.fillMin) return 1;
                            return 0;
                        })
                        .style("stroke-width", function(d1){
                            if(d1.full == true && d1.thick > 0 && d1.thick >= that.fillMin) return 1;
                            return 0;
                        });
                        //选择的虚线显示
                        svgLeft.selectAll(".dottedlinkup").filter(function(d1) {
                            return that.selectedNodes[d1.source.index] == 1 || that.selectedNodes[d1.target.index] == 1;
                        }).style("stroke-opacity",  function(d1){
                            if(d1.full == true && d1.thick > 0 && d1.thick >= that.fillMin) return 1;
                            return 0;
                        })
                        .style("stroke-width", function(d1){
                            if(d1.full == true && d1.thick > 0 && d1.thick >= that.fillMin) return 1;
                            return 0;
                        });
                        //选择不到的虚线不显示
                        svgLeft.selectAll(".dottedlinkup").filter(function(d1) {
                            return !(that.selectedNodes[d1.source.index] == 1 || that.selectedNodes[d1.target.index] == 1);
                        }).style("stroke-opacity",  0).style("stroke-width", 0);
                        
                        //mask透明度变化
                        svgLeft.selectAll(".rect").style("opacity", 0);
                        //所有选择的点显示
                        svgLeft.selectAll(".nodeup").filter(function(d1, i){
                            return that.selectedNodes[d1.index] == 1;
                        }).style("opacity", 1);
                        svgLeft.selectAll(".nodeup").filter(function(d1, i){
                            return that.selectedNodes[d1.index] == 1;
                        }).select("circle").style("stroke", "red");
                        //所有未选择的点不显示
                        svgLeft.selectAll(".nodeup").filter(function(d1, i){
                            return that.selectedNodes[d1.index] == null;
                        }).style("opacity", 0);
                        svgLeft.selectAll(".nodeup").filter(function(d1, i){
                            return that.selectedNodes[d1.index] == null;
                        }).select("circle").style("stroke", "black");    
                        }
                        
                        console.log('click mask end');
                        that.loading = false;
                        }, 0);
                    });

                //mask上层
                //粗细不一的线
                var polygonup = svgLeft.selectAll(".polygonup")
                    .data(that.matrix.links)
                    .enter()
                    .append("g")
                    .attr("class", function(d,i) {
                        return "polygonup" + d.source.index + "_" + d.target.index + " polygonup";
                    })
                    .sort(function(a, b) {
                        return a.thick - b.thick;
                    })
                    .style("opacity", 0)
                    .on("mouseenter", function(d, i) {
                        if(d.thick > 0) {
                            svgLeft.selectAll(".polygonup").filter(function(d1, i){
                            return d1.target.index == d.target.index && d1.source.index == d.source.index;
                        }).style("opacity", 1);
                        svgLeft.selectAll(".nodeup").filter(function(d1, i){
                            return d1.index == d.target.index || d1.index == d.source.index;
                        }).style("opacity", 1)
                        .select("circle").style("stroke","red");
                        }
                    })
                    .on("mouseout", function(d, i) {
                        if(d.thick > 0) {
                            //连接的点都未被选中
                        if(that.selectedNodes[d.source.index] == null && that.selectedNodes[d.target.index] == null && that.selectedLink[d.in] == null) {
                            svgLeft.selectAll(".polygonup").filter(function(d1, i){
                                return d1.target.index == d.target.index && d1.source.index == d.source.index;
                            }).style("opacity", 0);
                        }

                        var thisnode = svgLeft.selectAll(".nodeup").filter(function(d1, i){
                            return d1.index == d.target.index;
                        });
                        var thissourcenode = svgLeft.selectAll(".nodeup").filter(function(d1, i){
                            return d1.index == d.source.index;
                        });
                        //如果是选择的圆圈变黑，否则都变黑且沉下去
                        if(that.selectedNodes[d.source.index] == 1 || that.selectedLink[d.in] == 1) {
                            //thissourcenode.select("circle").style("stroke","black");
                        }
                        else {
                            //如果不与选择到的节点相连
                            if(that.selectedNodes[d.target.index] == null) {
                                thissourcenode.style("opacity", 0);
                            }
                            thissourcenode.select("circle").style("stroke","black");
                        }

                        if(that.selectedNodes[d.target.index] == 1 || that.selectedLink[d.in] == 1) {
                            //thisnode.select("circle").style("stroke","black");
                        }
                        else {
                            //如果不与选择到的节点相连
                            if(that.selectedNodes[d.source.index] == null) {
                                thisnode.style("opacity", 0);
                            }
                            thisnode.select("circle").style("stroke","black");
                        }
                        }
                        
                    });

                polygonup.append("polygon")
                    .style("fill", function(d) {
                        if(d.thick >= that.fillMin && d.thick <= that.fillMax){
                            if(d.sign == 1){
                                return that.fillScaleOne(d.thick - that.fillMin, that.fillMax - that.fillMin);
                            }
                            else if(d.sign == -1) {
                                return that.fillScaleTwo(d.thick - that.fillMin, that.fillMax - that.fillMin);
                            }
                            else {
                                return that.fillScaleGray(d.thick - that.fillMin, that.fillMax - that.fillMin);
                            }
                        } else {
                            return d3.rgb(255, 255, 255).toString();
                        }
                    })
                    .attr("points", function(d){
                        var source = [xScale(d.source.x), yScale(d.source.y)];
                        var target = [xScale(d.target.x), yScale(d.target.y)];
                        var x_distance = target[0] - source[0];
                        var y_distance = target[1] - source[1];
                        var c_distance = Math.pow(x_distance, 2) + Math.pow(y_distance, 2);
                        var distance = Math.sqrt(c_distance);

                        //当x_distance = 0时，即垂直于x轴, y值直接变化
                        if(x_distance == 0) {
                            //console.log("pall", d.source.index, d.target.index);
                            if(connection[d.source.index] >= connection[d.target.index])
                                return (xScale(d.source.x) - 2).toString() + ',' + (yScale(d.source.y)).toString() + " " + 
                                (xScale(d.target.x) - 0.5).toString() + ',' + (yScale(d.target.y)).toString() + " " +
                                (xScale(d.target.x) + 0.5).toString() + ',' + (yScale(d.target.y)).toString() + " " +
                                (xScale(d.source.x) + 2).toString() + ',' + (yScale(d.source.y)).toString() + " " ;
                            else return (xScale(d.target.x) - 2).toString() + ',' + (yScale(d.target.y)).toString() + " " + 
                                (xScale(d.source.x) - 0.5).toString() + ',' + (yScale(d.source.y)).toString() + " " +
                                (xScale(d.source.x) + 0.5).toString() + ',' + (yScale(d.source.y)).toString() + " " +
                                (xScale(d.target.x) + 2).toString() + ',' + (yScale(d.target.y)).toString() + " " ;
                        }
                        //当y_disatnce = 0, 即平行于x轴时，x值变化
                        if(y_distance == 0) {
                            //console.log("vertical", d.source.index, d.target.index);
                            if(connection[d.source.index] >= connection[d.target.index])
                                return xScale(d.source.x).toString() + ',' + (yScale(d.source.y) - 2).toString() + " " + 
                                xScale(d.target.x).toString() + ',' + (yScale(d.target.y) - 0.5).toString() + " " +
                                xScale(d.target.x).toString() + ',' + (yScale(d.target.y) + 0.5).toString() + " " +
                                xScale(d.source.x).toString() + ',' + (yScale(d.source.y) + 2).toString() + " " ;
                            else return xScale(d.target.x).toString() + ',' + (yScale(d.target.y) - 2).toString() + " " + 
                                xScale(d.source.x).toString() + ',' + (yScale(d.source.y) - 0.5).toString() + " " +
                                xScale(d.source.x).toString() + ',' + (yScale(d.source.y) + 0.5).toString() + " " +
                                xScale(d.target.x).toString() + ',' + (yScale(d.target.y) + 2).toString() + " " ;
                            
                        }
                        //否则计算斜率
                        var xielv = y_distance / x_distance;
                        //计算夹角,A为垂线与水平线夹角，锐角
                        var sinA = Math.abs(y_distance) / distance;
                        var cosA = Math.abs(x_distance) / distance;
                        if(d.source.index == 4 && d.target.index == 10) {
                            console.log("411", xielv, sinA, cosA);
                        }
                        //var bsource = source[1] + source[0] / xielv;
                        //var btarget = target[1] + target[0] / xielv;
                        if(xielv > 0){
                            if(connection[d.source.index] >= connection[d.target.index])
                                return (source[0] - 2 * sinA).toString() + ',' + (source[1] + 2 * cosA).toString() + " " + 
                                (target[0] - 0.5 * sinA).toString() + ',' + (target[1] + 0.5 * cosA).toString() + " " +
                                (target[0] + 0.5 * sinA).toString() + ',' + (target[1] - 0.5 * cosA).toString() + " " +
                                (source[0] + 2 * sinA).toString() + ',' + (source[1] - 2 * cosA).toString() + " " ;
                            else return (target[0] - 2 * sinA).toString() + ',' + (target[1] + 2 * cosA).toString() + " " + 
                                (source[0] - 0.5 * sinA).toString() + ',' + (source[1] + 0.5 * cosA).toString() + " " +
                                (source[0] + 0.5 * sinA).toString() + ',' + (source[1] - 0.5 * cosA).toString() + " " +
                                (target[0] + 2 * sinA).toString() + ',' + (target[1] - 2 * cosA).toString() + " " ;
                        }
                        else {
                            if(connection[d.source.index] >= connection[d.target.index])
                                return (source[0] - 2 * sinA).toString() + ',' + (source[1] - 2 * cosA).toString() + " " + 
                                (target[0] - 0.5 * sinA).toString() + ',' + (target[1] - 0.5 * cosA).toString() + " " +
                                (target[0] + 0.5 * sinA).toString() + ',' + (target[1] + 0.5 * cosA).toString() + " " +
                                (source[0] + 2 * sinA).toString() + ',' + (source[1] + 2 * cosA).toString() + " " ;
                            else return (target[0] - 2 * sinA).toString() + ',' + (target[1] - 2 * cosA).toString() + " " + 
                                (source[0] - 0.5 * sinA).toString() + ',' + (source[1] - 0.5 * cosA).toString() + " " +
                                (source[0] + 0.5 * sinA).toString() + ',' + (source[1] + 0.5 * cosA).toString() + " " +
                                (target[0] + 2 * sinA).toString() + ',' + (target[1] + 2 * cosA).toString() + " " ;
                        }
                    })
                    .style("opacity", function(d) {
                        //return 0;
                        if (that.reverse){
                            if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                if (d.thick == 0) return 0;
                                if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                    return 1;
                                return 0;
                            }
                            else {
                                return 0;
                            }
                        }
                        else return 4 - (d.thick - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 4 
                    })
                    .on("click", function(d){
                        if(d.thick != 0) {
                            //清空选择
                        for(var i in that.selectedNodes) {
                            connection[i] = connection[i] - 3000;
                        }
                        that.selectedNodes = {};
                        that.selectedLink = {};
                        that.selectedLink[d.in] = 1;

                        svgLeft.selectAll(".rect").style("opacity", 0.8);
                        
                        //所有选择的点显示且变红
                        svgLeft.selectAll(".nodeup").filter(function(d1, i){
                            return that.selectedNodes[d1.index] == 1;
                        }).style("opacity", 1);
                        svgLeft.selectAll(".nodeup").filter(function(d1, i){
                            return that.selectedNodes[d1.index] == 1;
                        }).select("circle").style("stroke", "red");
                        //所有未选择的点不显示且变黑
                        svgLeft.selectAll(".nodeup").filter(function(d1, i){
                            return that.selectedNodes[d1.index] == null;
                        }).style("opacity", 0);
                        svgLeft.selectAll(".nodeup").filter(function(d1, i){
                            return that.selectedNodes[d1.index] == null;
                        }).select("circle").style("stroke", "black");

                        //未选择的线不显示
                        svgLeft.selectAll(".polygonup").style("opacity",  0);
                        //选择的线显示,以及连接到的节点显示
                        svgLeft.selectAll(".polygonup").filter(function(d1) {
                            return d.source.index == d1.source.index && d.target.index == d1.target.index;
                        }).style("opacity",  function(d1){
                            if (that.reverse){
                            if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                if (d1.thick == 0) return 0;
                                if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                    {
                                        d1.select = 1;
                                        svgLeft.selectAll(".nodeup").filter(function(d2, i){
                                            return d2.index == d1.source.index || d2.index == d1.target.index;
                                        }).style("opacity", 1).select("circle").style("stroke","red");
                                        return 1;
                                    }
                                return 0;
                            }
                            else {
                                return 0;
                            }
                        }
                        else return 4 - (d1.thick - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 4 
                        });
                        
                        //调整底层显示的虚线
                        svgLeft.selectAll(".dottedlink").style("stroke-opacity",  function(d1){
                            if(d1.full == true && d1.thick > 0 && d1.thick >= that.fillMin) return 1;
                            return 0;
                        })
                        .style("stroke-width", function(d1){
                            if(d1.full == true && d1.thick > 0 && d1.thick >= that.fillMin) return 1;
                            return 0;
                        });;
                        //选择的虚线显示
                        svgLeft.selectAll(".dottedlinkup").filter(function(d1) {
                            return that.selectedNodes[d1.source.index] == 1 || that.selectedNodes[d1.target.index] == 1;
                        }).style("stroke-opacity",  function(d1){
                            if(d1.full == true && d1.thick > 0 && d1.thick >= that.fillMin) return 1;
                            return 0;
                        })
                        .style("stroke-width", function(d1){
                            if(d1.full == true && d1.thick > 0 && d1.thick >= that.fillMin) return 1;
                            return 0;
                        });
                        //选择不到的虚线不显示
                        svgLeft.selectAll(".dottedlinkup").filter(function(d1) {
                            return !(that.selectedNodes[d1.source.index] == 1 || that.selectedNodes[d1.target.index] == 1);
                        }).style("stroke-opacity",  0).style("stroke-width", 0);
                        }

                    });

                /*
                var pattern = svgLeft
                    .append("pattern")
                    .attr("x", 0)
                    .attr("y", 0)
                    .attr("height", 64)
                    .attr("width", 64);

                pattern.append("image")
                    .attr("xlink:href", this.rightPic)
                    .attr("x", 0)
                    .attr("y", 0)
                    .attr("height", 40)
                    .attr("width", 40);
                */
                //添加椭圆

                for(var numindex = 0; numindex < dataNum; numindex++) {
                    var classname = ".ellipseup" + numindex;
                    var ellipseup = polygonup.append("g")
                        .attr("class", function(d,i){
                            return "ellipseup" + numindex + " ellipseup";
                        });

                    ellipseup.append("ellipse")
                        .attr("rx", function(d){
                            //console.log('d.data', d.data[numindex]);
                            if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                if (d.thick == 0) return 0;
                                if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                    return that.widthScale(d.data[numindex]);
                                    //return Math.abs(d.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                return 0;
                            }
                            else {
                                return 0;
                            }
                            
                        })
                        .attr("ry", function(d) {
                            if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                if (d.thick == 0) return 0;
                                if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                    return that.widthScale(d.data[(numindex + 1) % dataNum]);
                                    //return Math.abs(d.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                return 0;
                            }
                            else {
                                return 0;
                            }

                        })
                        .attr("cx", function(d) {
                            return d.cx[numindex];
                            if(connection[d.source.index] >= connection[d.target.index])
                                return (xScale(d.target.x) - xScale(d.source.x)) / (dataNum + 1) * (numindex + 1) + xScale(d.source.x);
                            else return (xScale(d.source.x) - xScale(d.target.x)) / (dataNum + 1) * (numindex + 1) + xScale(d.target.x);
                        })
                        .attr("cy", function(d){
                            return d.cy[numindex];
                            if(connection[d.source.index] >= connection[d.target.index])
                                return (yScale(d.target.y) - yScale(d.source.y)) / (dataNum + 1) * (numindex + 1) + yScale(d.source.y);
                            return (yScale(d.source.y) - yScale(d.target.y)) / (dataNum + 1) * (numindex + 1) + yScale(d.target.y);
                        })
                        .style("transform-origin", function(d) {
                            //console.log("transform-origin", (d.cx[numindex] / width * 100).toString() + "% "  +( d.cy[numindex] / height * 100).toString() +"%");
                            return (d.cx[numindex] / width * 100).toString() + "% "  +( d.cy[numindex] / height * 100).toString() +"%";
                        })
                        .style("transform", function(d) {
                            var deg = 0;
                            var source = [xScale(d.source.x), yScale(d.source.y)];
                            var target = [xScale(d.target.x), yScale(d.target.y)];
                            var x_distance = target[0] - source[0];
                            var y_distance = target[1] - source[1];
                            var c_distance = Math.pow(x_distance, 2) + Math.pow(y_distance, 2);
                            var distance = Math.sqrt(c_distance);
                            if (x_distance == 0) {
                                deg = 90;
                            }
                            else if (y_distance == 0) {
                                deg = 0;
                            } 
                            else {
                                var xielv = y_distance / x_distance;
                                var sinA = Math.abs(y_distance) / distance;
                                if (xielv < 0) {
                                    deg = 90 - Math.asin(sinA) * 180 / Math.PI;
                                }
                                else {
                                    deg = Math.asin(sinA) * 180 / Math.PI - 90;
                                }
                                
                            }
                            return "rotate("+ deg.toString() + "deg)";
                        })
                        .style("fill", function(d) {
                            return that.fillColor(d.allSigns[numindex] >= 0);
                            return "white";
                            if (d.allSigns[numindex] >= 0) {
                                return "#775AA9";
                            }
                            else {
                                return "#FE8F00";
                            }
                        })
                        //.style("stroke", "none")
                        //.style("opacity", 0)
                        .style("stroke-width", function(d) {
                            return 1;
                        }) //路径宽度
                        .style("stroke", function(d) {
                            return that.strokeColor(d.allSigns[numindex] >= 0)
                            if (d.allSigns[numindex] >= 0) {
                                return "#775AA9";
                            }
                            else {
                                return "#FE8F00";
                            }
                        })
                        //添加hover
                        .on("mouseenter", function(d, i) {
                        //console.log('mouseenter', d);
                        return tipsDiv
                            .style('visibility', 'visible')
                            //.style('left', (d3.event.clientX - 920) + 'px') //- 400
                            //.style('top', (d3.event.clientY - 300) + 'px') //- 110
                            .style('right', '14px')
                            //.style('top', '224px')
                            .style('bottom', '0px')
                            .html('ROI: ' + (d.source.index + 1) + '  <->  ' + (d.target.index + 1) + '<br>' + 'value: ' + d.thick.toFixed(5) + '<br>' + 'allDatas: ' + d.data);//tooltip.html(text);
                        })
                        .on("mouseout", function(d, i) {
                            //return tooltip.style('visibility', 'hidden');
                            return tipsDiv.style('visibility', 'hidden');
                        })
                        //添加点击事件
                        .on("click", function(d){

                        })
                        .call(that.drag);

                        //添加线
                        ellipseup.append("line")
                            .attr("class", function(d,i) {
                                return ".rowlinkup" + numindex;
                            })
                            .style("stroke-width", function(d) {
                                if (that.reverse){
                                    if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                        if (d.thick == 0) return 0;
                                        if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray)) {
                                            return 1;
                                        }
                                        return 0;
                                    }
                                    else {
                                        return 0;
                                    }
                                }
                                else return 4 - (d.thick - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 4 
                            })  //路径宽度
                            .style("stroke", function(d) {
                                return that.lineColor(d.allSigns[numindex] >= 0);
                                if (d.allSigns[numindex] >= 0) {
                                    return "#775AA9";
                                }
                                else {
                                    return "#FE8F00";
                                }
                            })
                            //.style("stroke", "red")
                            .style("stroke-opacity", function(d){ //不透明度
                                //return 0;
                            })
                            .attr("x1", function(d){
                                var rx = 0;
                                var ry = 0;
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        rx = that.widthScale(d.data[numindex]);
                                        //rx = Math.abs(d.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        ry = that.widthScale(d.data[(numindex + 1) % dataNum]);
                                        //ry = Math.abs(d.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                return d.cx[numindex] + rx;
                            })
                            .attr("y1", function(d){
                                var rx = 0;
                                var ry = 0;
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        rx = that.widthScale(d.data[numindex]);
                                        //rx = Math.abs(d.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        ry = that.widthScale(d.data[(numindex + 1) % dataNum]);
                                        //ry = Math.abs(d.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                return d.cy[numindex];
                            })
                            .attr("x2", function(d){
                                var rx = 0;
                                var ry = 0;
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        rx = that.widthScale(d.data[numindex]);
                                        //rx = Math.abs(d.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        ry = that.widthScale(d.data[(numindex + 1) % dataNum]);
                                        //ry = Math.abs(d.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                return d.cx[numindex] - rx;
                            })
                            .attr("y2", function(d){
                                var rx = 0;
                                var ry = 0;
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        rx = that.widthScale(d.data[numindex]);
                                        //rx = Math.abs(d.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        ry = that.widthScale(d.data[(numindex + 1) % dataNum]);
                                        //ry = Math.abs(d.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                return d.cy[numindex];
                            })
                            .style("transform-origin", function(d) {
                                    return (d.cx[numindex] / width * 100).toString() + "% "  +( d.cy[numindex] / height * 100).toString() +"%";
                            })
                            .style("transform", function(d) {
                                var deg = 0;
                                var source = [xScale(d.source.x), yScale(d.source.y)];
                                var target = [xScale(d.target.x), yScale(d.target.y)];
                                var x_distance = target[0] - source[0];
                                var y_distance = target[1] - source[1];
                                var c_distance = Math.pow(x_distance, 2) + Math.pow(y_distance, 2);
                                var distance = Math.sqrt(c_distance);
                                if (x_distance == 0) {
                                    deg = 90;
                                }
                                else if (y_distance == 0) {
                                    deg = 0;
                                } 
                                else {
                                    var xielv = y_distance / x_distance;
                                    var sinA = Math.abs(y_distance) / distance;
                                    if (xielv < 0) {
                                        deg = 90 - Math.asin(sinA) * 180 / Math.PI;
                                    }
                                    else {
                                        deg = Math.asin(sinA) * 180 / Math.PI - 90;
                                        //console.log("-deg", deg);
                                    }
                                
                                }
                                return "rotate("+ deg.toString() + "deg)";
                            });
                    }

                //添加虚线
                svgLeft.selectAll(".dottedlinkup")
                    .data(that.matrix.links)
                    .enter()
                    .append("line")
                    .attr("class", function(d,i) {
                        return "leftl" + that.clusterNo(d.source.index, clusters) + "l" + that.clusterNo(d.target.index, clusters) + "leftl" + that.clusterNo(d.source.index, clusters) + "leftl" + that.clusterNo(d.target.index, clusters) +" dottedlinkup";
                    })
                    .style("stroke-width", function(d){ //不透明度
                        if(d.full == true && d.thick > 0 && d.thick >= that.fillMin) return 1;
                        return 0;
                    })  //路径宽度
                    .style("stroke", function(d){//路径颜色
                        return "red";
                    })
                    .style("stroke-opacity", function(d){ //不透明度
                        return 0;
                        if(d.full == true && d.thick > 0 && d.thick >= that.fillMin) return 1;
                        return 0;
                    })
                    .style("stroke-dasharray", "3")
                    .sort(function(a, b) {
                        return a.thick - b.thick;
                    })
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


                //添加节点
                var nodeLeftup = svgLeft.selectAll(".nodeup")
                    .data(this.matrix.nodes)
                    .enter()
                    .append("g")
                    .attr("class", "nodeup")
                    .attr("id", function(d,i){
                        return "index"+that.index+"node"+i;
                    })
                    .style("opacity", 0)
                    .sort(function(a, b) {
                        return connection[a.index] - connection[b.index];
                    })
                    .on("mouseenter", function(d, i) {
                        var thisnode = svgLeft.selectAll(".nodeup").filter(function(d1, i){
                            return d1.index == d.index;
                        }).style("opacity", 1);
                        //如果是选择的字体变红，否则都变红
                        if(that.selectedNodes[d.index] == 1) {
                            thisnode.select("text").style("fill","red");
                        }
                        else {
                            thisnode.select("circle").style("stroke","red");
                            thisnode.select("text").style("fill","red");
                        }
                    })
                    .on("mouseout", function(d, i) {
                        var thisnode = svgLeft.selectAll(".nodeup").filter(function(d1, i){
                            return d1.index == d.index;
                        });
                        //如果是选择的字体变黑，否则都变黑且沉下去
                        if(that.selectedNodes[d.index] == 1) {
                            thisnode.select("text").style("fill","black");
                        }
                        else {
                            //如果与选择到的节点相连或者与选择的线相连
                            var nodelinknum = 0;
                            that.matrix.links.filter(function(d1){
                                return (that.selectedLink[d1.in] == 1 && (d1.source.index == d.index || d1.target.index == d.index));
                            }).forEach(function(d1){
                                nodelinknum++;
                            });
                            if (nodelinknum == 0) {
                                that.matrix.links.filter(function(d1){
                                    return (d1.thick >= that.fillMin && d1.thick != 0 && ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray)) && (that.selectedNodes[d1.source.index] == 1 || that.selectedNodes[d1.target.index] == 1) && (d1.source.index == d.index || d1.target.index == d.index)) || (that.selectedLink[d1.in] == 1 && (d1.source.index == d.index || d1.target.index == d.index));
                                }).forEach(function(d1){
                                    nodelinknum++;
                                });

                                if(nodelinknum == 0) {
                                    thisnode.style("opacity", 0);
                                }
                                thisnode.select("circle").style("stroke","black");
                                thisnode.select("text").style("fill","black");
                            }
                            else {
                                thisnode.select("text").style("fill","black");
                            }  
                        }
                    })
                    .on("click", function(d){
                        if(connection[d.index] == 0) {
                            return;
                        }
                        that.loading = true;
                        setTimeout(function(){
                        that.selectedLink = {};
                        //反选
                        if(that.selectedNodes[d.index] != null) {
                            delete that.selectedNodes[d.index];
                            connection[d.index] = connection[d.index] - 3000;
                        }
                        else{
                            that.selectedNodes[d.index] = 1;
                            connection[d.index] = connection[d.index] + 3000;
                        }
                        var countnum = 0;
                        for(var noedeind in that.selectedNodes) {
                            countnum++;
                        }
                        if(countnum == 0) {
                            svgLeft.selectAll(".rect").style("opacity", 0);
                        }
                        else {
                            svgLeft.selectAll(".rect").style("opacity", 0.8);
                        }
                        //所有选择的点显示且变红
                        svgLeft.selectAll(".nodeup").filter(function(d1, i){
                            return that.selectedNodes[d1.index] == 1;
                        }).style("opacity", 1);
                        svgLeft.selectAll(".nodeup").filter(function(d1, i){
                            return that.selectedNodes[d1.index] == 1;
                        }).select("circle").style("stroke", "red");
                        //所有未选择的点不显示且变黑
                        svgLeft.selectAll(".nodeup").filter(function(d1, i){
                            return that.selectedNodes[d1.index] == null;
                        }).style("opacity", 0);
                        svgLeft.selectAll(".nodeup").filter(function(d1, i){
                            return that.selectedNodes[d1.index] == null;
                        }).select("circle").style("stroke", "black");
                        //根据重要性重画与该边连接的polygon
                        polygon.filter(function(d1) {
                            return d1.target.index == d.index || d1.source.index == d.index;
                        })
                        .select("polygon")
                        .attr("points", function(d1){
                            var source = [xScale(d1.source.x), yScale(d1.source.y)];
                            var target = [xScale(d1.target.x), yScale(d1.target.y)];
                            var x_distance = target[0] - source[0];
                            var y_distance = target[1] - source[1];
                            var c_distance = Math.pow(x_distance, 2) + Math.pow(y_distance, 2);
                            var distance = Math.sqrt(c_distance);
                            //当y_distance = 0时，即平行于x轴, y值直接变化
                            if(y_distance == 0) {
                                if(connection[d1.source.index] >= connection[d1.target.index])
                                    return xScale(d1.source.x).toString() + ',' + (yScale(d1.source.y) - 2).toString() + " " + 
                                        xScale(d1.target.x).toString() + ',' + (yScale(d1.target.y) - 0.5).toString() + " " +
                                        xScale(d1.target.x).toString() + ',' + (yScale(d1.target.y) + 0.5).toString() + " " +
                                        xScale(d1.source.x).toString() + ',' + (yScale(d1.source.y) + 2).toString() + " " ;
                                    else return xScale(d1.target.x).toString() + ',' + (yScale(d1.target.y) - 2).toString() + " " + 
                                        xScale(d1.source.x).toString() + ',' + (yScale(d1.source.y) - 0.5).toString() + " " +
                                        xScale(d1.source.x).toString() + ',' + (yScale(d1.source.y) + 0.5).toString() + " " +
                                        xScale(d1.target.x).toString() + ',' + (yScale(d1.target.y) + 2).toString() + " " ;
                            }
                            //当x_disatnce = 0, 即垂直于x轴时，x值变化
                            if(x_distance == 0) {
                                if(connection[d1.source.index] >= connection[d1.target.index])
                                    return (xScale(d1.source.x) - 2).toString() + ',' + (yScale(d1.source.y)).toString() + " " + 
                                        (xScale(d1.target.x) - 0.5).toString() + ',' + (yScale(d1.target.y)).toString() + " " +
                                        (xScale(d1.target.x) + 0.5).toString() + ',' + (yScale(d1.target.y)).toString() + " " +
                                        (xScale(d1.source.x) + 2).toString() + ',' + (yScale(d1.source.y)).toString() + " " ;
                                else return (xScale(d1.target.x) - 2).toString() + ',' + (yScale(d1.target.y)).toString() + " " + 
                                    (xScale(d1.source.x) - 0.5).toString() + ',' + (yScale(d1.source.y)).toString() + " " +
                                    (xScale(d1.source.x) + 0.5).toString() + ',' + (yScale(d1.source.y)).toString() + " " +
                                    (xScale(d1.target.x) + 2).toString() + ',' + (yScale(d1.target.y)).toString() + " " ;
                            }
                            //否则计算斜率
                            var xielv = y_distance / x_distance;
                            //计算夹角,A为垂线与水平线夹角，锐角
                            var sinA = Math.abs(y_distance) / distance;
                            var cosA = Math.abs(x_distance) / distance;
                            //var bsource = source[1] + source[0] / xielv;
                            //var btarget = target[1] + target[0] / xielv;
                            if(xielv > 0){
                                if(connection[d1.source.index] >= connection[d1.target.index])
                                    return (source[0] - 2 * sinA).toString() + ',' + (source[1] + 2 * cosA).toString() + " " + 
                                        (target[0] - 0.5 * sinA).toString() + ',' + (target[1] + 0.5 * cosA).toString() + " " +
                                        (target[0] + 0.5 * sinA).toString() + ',' + (target[1] - 0.5 * cosA).toString() + " " +
                                        (source[0] + 2 * sinA).toString() + ',' + (source[1] - 2 * cosA).toString() + " " ;
                                else return (target[0] - 2 * sinA).toString() + ',' + (target[1] + 2 * cosA).toString() + " " + 
                                        (source[0] - 0.5 * sinA).toString() + ',' + (source[1] + 0.5 * cosA).toString() + " " +
                                        (source[0] + 0.5 * sinA).toString() + ',' + (source[1] - 0.5 * cosA).toString() + " " +
                                        (target[0] + 2 * sinA).toString() + ',' + (target[1] - 2 * cosA).toString() + " " ;
                            }
                            else {
                                if(connection[d1.source.index] >= connection[d1.target.index])
                                    return (source[0] - 2 * sinA).toString() + ',' + (source[1] - 2 * cosA).toString() + " " + 
                                        (target[0] - 0.5 * sinA).toString() + ',' + (target[1] - 0.5 * cosA).toString() + " " +
                                        (target[0] + 0.5 * sinA).toString() + ',' + (target[1] + 0.5 * cosA).toString() + " " +
                                        (source[0] + 2 * sinA).toString() + ',' + (source[1] + 2 * cosA).toString() + " " ;
                                else return (target[0] - 2 * sinA).toString() + ',' + (target[1] - 2 * cosA).toString() + " " + 
                                        (source[0] - 0.5 * sinA).toString() + ',' + (source[1] - 0.5 * cosA).toString() + " " +
                                        (source[0] + 0.5 * sinA).toString() + ',' + (source[1] + 0.5 * cosA).toString() + " " +
                                        (target[0] + 2 * sinA).toString() + ',' + (target[1] + 2 * cosA).toString() + " " ;
                            }
                        });
                        polygonup.filter(function(d1) {
                            return d1.target.index == d.index || d1.source.index == d.index;
                        })
                        .select("polygon")
                        .attr("points", function(d1){
                            var source = [xScale(d1.source.x), yScale(d1.source.y)];
                            var target = [xScale(d1.target.x), yScale(d1.target.y)];
                            var x_distance = target[0] - source[0];
                            var y_distance = target[1] - source[1];
                            var c_distance = Math.pow(x_distance, 2) + Math.pow(y_distance, 2);
                            var distance = Math.sqrt(c_distance);
                            //当y_distance = 0时，即平行于x轴, y值直接变化
                            if(y_distance == 0) {
                                if(connection[d1.source.index] >= connection[d1.target.index])
                                    return xScale(d1.source.x).toString() + ',' + (yScale(d1.source.y) - 2).toString() + " " + 
                                        xScale(d1.target.x).toString() + ',' + (yScale(d1.target.y) - 0.5).toString() + " " +
                                        xScale(d1.target.x).toString() + ',' + (yScale(d1.target.y) + 0.5).toString() + " " +
                                        xScale(d1.source.x).toString() + ',' + (yScale(d1.source.y) + 2).toString() + " " ;
                                    else return xScale(d1.target.x).toString() + ',' + (yScale(d1.target.y) - 2).toString() + " " + 
                                        xScale(d1.source.x).toString() + ',' + (yScale(d1.source.y) - 0.5).toString() + " " +
                                        xScale(d1.source.x).toString() + ',' + (yScale(d1.source.y) + 0.5).toString() + " " +
                                        xScale(d1.target.x).toString() + ',' + (yScale(d1.target.y) + 2).toString() + " " ;
                            }
                            //当x_disatnce = 0, 即垂直于x轴时，x值变化
                            if(x_distance == 0) {
                                if(connection[d1.source.index] >= connection[d1.target.index])
                                    return (xScale(d1.source.x) - 2).toString() + ',' + (yScale(d1.source.y)).toString() + " " + 
                                        (xScale(d1.target.x) - 0.5).toString() + ',' + (yScale(d1.target.y)).toString() + " " +
                                        (xScale(d1.target.x) + 0.5).toString() + ',' + (yScale(d1.target.y)).toString() + " " +
                                        (xScale(d1.source.x) + 2).toString() + ',' + (yScale(d1.source.y)).toString() + " " ;
                                else return (xScale(d1.target.x) - 2).toString() + ',' + (yScale(d1.target.y)).toString() + " " + 
                                    (xScale(d1.source.x) - 0.5).toString() + ',' + (yScale(d1.source.y)).toString() + " " +
                                    (xScale(d1.source.x) + 0.5).toString() + ',' + (yScale(d1.source.y)).toString() + " " +
                                    (xScale(d1.target.x) + 2).toString() + ',' + (yScale(d1.target.y)).toString() + " " ;
                            }
                            //否则计算斜率
                            var xielv = y_distance / x_distance;
                            //计算夹角,A为垂线与水平线夹角，锐角
                            var sinA = Math.abs(y_distance) / distance;
                            var cosA = Math.abs(x_distance) / distance;
                            //var bsource = source[1] + source[0] / xielv;
                            //var btarget = target[1] + target[0] / xielv;
                            if(xielv > 0){
                                if(connection[d1.source.index] >= connection[d1.target.index])
                                    return (source[0] - 2 * sinA).toString() + ',' + (source[1] + 2 * cosA).toString() + " " + 
                                        (target[0] - 0.5 * sinA).toString() + ',' + (target[1] + 0.5 * cosA).toString() + " " +
                                        (target[0] + 0.5 * sinA).toString() + ',' + (target[1] - 0.5 * cosA).toString() + " " +
                                        (source[0] + 2 * sinA).toString() + ',' + (source[1] - 2 * cosA).toString() + " " ;
                                else return (target[0] - 2 * sinA).toString() + ',' + (target[1] + 2 * cosA).toString() + " " + 
                                        (source[0] - 0.5 * sinA).toString() + ',' + (source[1] + 0.5 * cosA).toString() + " " +
                                        (source[0] + 0.5 * sinA).toString() + ',' + (source[1] - 0.5 * cosA).toString() + " " +
                                        (target[0] + 2 * sinA).toString() + ',' + (target[1] - 2 * cosA).toString() + " " ;
                            }
                            else {
                                if(connection[d1.source.index] >= connection[d1.target.index])
                                    return (source[0] - 2 * sinA).toString() + ',' + (source[1] - 2 * cosA).toString() + " " + 
                                        (target[0] - 0.5 * sinA).toString() + ',' + (target[1] - 0.5 * cosA).toString() + " " +
                                        (target[0] + 0.5 * sinA).toString() + ',' + (target[1] + 0.5 * cosA).toString() + " " +
                                        (source[0] + 2 * sinA).toString() + ',' + (source[1] + 2 * cosA).toString() + " " ;
                                else return (target[0] - 2 * sinA).toString() + ',' + (target[1] - 2 * cosA).toString() + " " + 
                                        (source[0] - 0.5 * sinA).toString() + ',' + (source[1] - 0.5 * cosA).toString() + " " +
                                        (source[0] + 0.5 * sinA).toString() + ',' + (source[1] + 0.5 * cosA).toString() + " " +
                                        (target[0] + 2 * sinA).toString() + ',' + (target[1] + 2 * cosA).toString() + " " ;
                            }
                        });
                        //假设为选择，则更改椭圆位置
                        //if(that.selectedNodes[d.index] == 1) {
                        if(1) {
                        var xs = []; //存储每个节点的x
                        var scores = []; //存储每个节点的score
                        for(var i = 0; i < 70; i++) {
                            xs.push(that.minx);
                            scores.push(0);
                        }
                        if(dataNum > 0) {
                            //对于每个节点计算它的score
                            for(var i = 0; i < 70; i++) {
                                //假设节点被选中，则其x=that.minx,否则计算x
                                if(that.selectedNodes[i] == 1) {
                                    xs[i] = that.minx;
                                }
                                else {
                                    //计算maxx
                                    maxx = 999;
                                    var count = 0;
                                    that.matrix.links.filter(function(d1){
                                        return ((d1.source.index == i && connection[d1.source.index] >= connection[d1.target.index]) || (d1.target.index == i && connection[d1.source.index] <= connection[d1.target.index])) && Math.abs(d1.thick) > 0;
                                    }).forEach(function(d1){
                                        var dx = xScale(d1.target.x) - xScale(d1.source.x);
                                        var dy = yScale(d1.target.y) - yScale(d1.source.y);
                                        var sqrtdistance = Math.sqrt(dx * dx + dy * dy);
                                        //maxx += sqrtdistance;
                                        for(var si = 0; si < dataNum; si++) {
                                            sqrtdistance -= (that.widthScale(d1.data[si]) * 2);
                                        }
                                        if(sqrtdistance < 0) sqrtdistance = 0;
                                        if(sqrtdistance < maxx) maxx = sqrtdistance;
                                        count++;
                                    });
                                    //if(count != 0) maxx = maxx / count;
                                    maxx = maxx - that.consx * (dataNum - 1);
                                    if(maxx < 0) maxx = 0;
                                    //计算每一步寻找最大的score
                                    for(var x = that.minx; x < maxx; x=x+that.step) {
                                        var score = 0;
                                        //找到所有与该节点相连的线，该节点重要性大于另一节点
                                        that.matrix.links.filter(function(d1){
                                            return ((d1.source.index == i && connection[d1.source.index] >= connection[d1.target.index]) || (d1.target.index == i && connection[d1.source.index] <= connection[d1.target.index])) && Math.abs(d1.thick) > 0;
                                        }).forEach(function(d2){
                                            //对于该线上的所有椭圆，计算minscore
                                            //节点原始坐标
                                            var originx = 0;
                                            var originy = 0;
                                            //延直线扩展的单位
                                            var dirx = 0;
                                            var diry = 0;
                                            if(d2.source.index == i) {
                                                originx = xScale(d2.source.x);
                                                originy = yScale(d2.source.y);
                                                var dx = xScale(d2.target.x) - xScale(d2.source.x);
                                                var dy = yScale(d2.target.y) - yScale(d2.source.y);
                                                var sqrtdistance = Math.sqrt(dx * dx + dy * dy);
                                                dirx = dx / sqrtdistance;
                                                diry = dy / sqrtdistance;
                                            }
                                            else {
                                                originx = xScale(d2.target.x);
                                                originy = yScale(d2.target.y);
                                                var dx = xScale(d2.source.x) - xScale(d2.target.x);
                                                var dy = yScale(d2.source.y) - yScale(d2.target.y);
                                                var sqrtdistance = Math.sqrt(dx * dx + dy * dy);
                                                dirx = dx / sqrtdistance;
                                                diry = dy / sqrtdistance;
                                            }
                                            //对于线上每个椭圆
                                            for (var j = 0; j < dataNum; j++) {
                                                var minscore = 9999;
                                                var x0 = originx + (x + j * that.consx) * dirx;
                                                var y0 = originy + (x + j * that.consx) * diry;
                                                //找到与该线条不同的所有线,计算点到直线的最小距离
                                                that.matrix.links.filter(function(d1){
                                                    return !(d1.in == d2.in) && Math.abs(d1.thick) > 0;
                                                }).forEach(function(d1) {
                                                    //var x0 = originx + (x + j * that.consx) * dirx;
                                                    //var y0 = originy + (x + j * that.consx) * diry;
                                                    var distance = that.caldistance(x0, y0, xScale(d1.source.x), yScale(d1.source.y), xScale(d1.target.x), yScale(d1.target.y));
                                                    if (distance < minscore) {
                                                        minscore = distance;
                                                    }
                                                });
                                                score = score + that.alpha * minscore;
                                                var minEM = 999;
                                                //找到高亮的所有线，计算点到线上椭圆的最小距离
                                                that.matrix.links.filter(function(d1){
                                                    return (that.selectedNodes[d1.source.index] == 1 || that.selectedNodes[d1.target.index] == 1) && Math.abs(d1.thick) > 0;
                                                }).forEach(function(d1) {
                                                    //计算出该条线上椭圆位置
                                                    var newpoints;
                                                    if(connection[d1.source.index] >= connection[d1.target.index]) {
                                                        //calposition(x0, y0, x1, y1, that.minx, constx, dataNum)
                                                        newpoints = that.calposition(xScale(d1.source.x), yScale(d1.source.y), xScale(d1.target.x), yScale(d1.target.y), that.minx, that.consx, dataNum);
                                                    }
                                                    else{
                                                        newpoints = that.calposition(xScale(d1.target.x), yScale(d1.target.y), xScale(d1.source.x), yScale(d1.source.y), that.minx, that.consx, dataNum);
                                                    }
                                                    for(var p in newpoints) {
                                                        var distance = that.pointdistance(newpoints[p][0], newpoints[p][1], x0, y0);
                                                        if (distance < minEM) {
                                                            minEM = distance;
                                                        }
                                                    }
                                                });
                                                score = score + (1 - that.alpha) * minEM;
                                            }
                                        });
                                        if(score > scores[i]) {
                                            scores[i] = score;
                                            xs[i] = x;
                                        }
                                    }
                                }
                            }
                        }
                        console.log('xs', xs);
                        //重画所有的椭圆
                        polygon.selectAll(".ellipse").remove();
                        for(var numindex = 0; numindex < dataNum; numindex++) {
                            var classname = ".ellipse" + numindex;
                            var ellipse = polygon.append("g")
                                .attr("class", function(d1,i){
                                    return "ellipse" + numindex + " ellipse";
                                });
                            ellipse.append("ellipse")
                                .attr("rx", function(d1){
                                    //console.log('d.data', d.data[numindex]);
                                    if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                        if (d1.thick == 0) return 0;
                                        if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray)){
                                            return that.widthScale(d1.data[numindex]);
                                            //return Math.abs(d1.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        return 0;
                                    }
                                    else {
                                        return 0;
                                    }            
                                })
                                .attr("ry", function(d1) {
                                    if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                        if (d1.thick == 0) return 0;
                                        if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray)){
                                            return that.widthScale(d1.data[(numindex+1)%dataNum]);
                                            //return Math.abs(d1.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        return 0;
                                    }
                                    else {
                                        return 0;
                                    }
                                })
                                .attr("cx", function(d1) {            
                                    var originx = 0;
                                    var originy = 0;
                                    var targetx = 0;
                                    var targety = 0;
                                    //延直线扩展的单位
                                    var dirx = 0;
                                    var diry = 0;
                                    var noedeindex = 0;
                                    var sqrtdistance = 0;
                                    if(connection[d1.source.index] >= connection[d1.target.index]) {
                                        noedeindex = Number(d1.source.index);
                                        originx = xScale(d1.source.x);
                                        originy = yScale(d1.source.y);
                                        targetx = xScale(d1.target.x);
                                        targety = yScale(d1.target.y);
                                        var dx = xScale(d1.target.x) - xScale(d1.source.x);
                                        var dy = yScale(d1.target.y) - yScale(d1.source.y);
                                        sqrtdistance = Math.sqrt(dx * dx + dy * dy);
                                        if(sqrtdistance == 0) {
                                            d1.cx[numindex] = originx;
                                            return originx;
                                        }
                                        dirx = dx / sqrtdistance;
                                        diry = dy / sqrtdistance;
                                    }
                                    else {
                                        noedeindex = Number(d1.target.index);
                                        originx = xScale(d1.target.x);
                                        originy = yScale(d1.target.y);
                                        targetx = xScale(d1.source.x);
                                        targety = yScale(d1.source.y);
                                        var dx = xScale(d1.source.x) - xScale(d1.target.x);
                                        var dy = yScale(d1.source.y) - yScale(d1.target.y);
                                        sqrtdistance = Math.sqrt(dx * dx + dy * dy);
                                        if(sqrtdistance == 0) {
                                            d1.cx[numindex] = originx;
                                            return originx;
                                        }
                                        dirx = dx / sqrtdistance;
                                        diry = dy / sqrtdistance;
                                    }
                                    var forwardEllipse = 0;
                                    var localEllipse = that.widthScale(d1.data[(numindex + 1) % dataNum]);
                                    for(var ti = 0; ti < numindex; ti++) {
                                        forwardEllipse = that.widthScale(d1.data[(ti + 1) % dataNum]) * 2 + that.consx;
                                    }
                                    var result = originx + (xs[noedeindex] +forwardEllipse + localEllipse) * dirx;
                                    if((xs[noedeindex] + forwardEllipse + localEllipse) >= sqrtdistance) {
                                        result = targetx;
                                        d1.full = true; //放不下
                                    }
                                    else {
                                        d1.full = false;
                                    }
                                    d1.cx[numindex] = result;
                                    //d.cx.push(result);
                                    //console.log('result', result);
                                    return result;
                                })
                                .attr("cy", function(d1){
                                    var originx = 0;
                                    var originy = 0;
                                    var targetx = 0;
                                    var targety = 0;
                                    //延直线扩展的单位
                                    var dirx = 0;
                                    var diry = 0;
                                    var noedeindex = 0;
                                    var sqrtdistance = 0;
                                    if(connection[d1.source.index] >= connection[d1.target.index]) {
                                        noedeindex = Number(d1.source.index);
                                        originx = xScale(d1.source.x);
                                        originy = yScale(d1.source.y);
                                        targetx = xScale(d1.target.x);
                                        targety = yScale(d1.target.y);
                                        var dx = xScale(d1.target.x) - xScale(d1.source.x);
                                        var dy = yScale(d1.target.y) - yScale(d1.source.y);
                                        sqrtdistance = Math.sqrt(dx * dx + dy * dy);
                                        //console.log('sqrtdistance', sqrtdistance);
                                        if(sqrtdistance == 0) {
                                            d1.cy[numindex] = originy;
                                            return originy;
                                        }
                                        dirx = dx / sqrtdistance;
                                        diry = dy / sqrtdistance;
                                    }
                                    else {
                                        noedeindex = Number(d1.target.index);
                                        originx = xScale(d1.target.x);
                                        originy = yScale(d1.target.y);
                                        targetx = xScale(d1.source.x);
                                        targety = yScale(d1.source.y);
                                        var dx = xScale(d1.source.x) - xScale(d1.target.x);
                                        var dy = yScale(d1.source.y) - yScale(d1.target.y);
                                        sqrtdistance = Math.sqrt(dx * dx + dy * dy);
                                        //console.log('sqrtdistance', sqrtdistance);
                                        if(sqrtdistance == 0) {
                                            d1.cy[numindex] = originy;
                                            return originy;
                                        }
                                        dirx = dx / sqrtdistance;
                                        diry = dy / sqrtdistance;
                                    }
                                    var forwardEllipse = 0;
                                    var localEllipse = that.widthScale(d1.data[(numindex + 1) % dataNum]);
                                    for(var ti = 0; ti < numindex; ti++) {
                                        forwardEllipse = that.widthScale(d1.data[(ti + 1) % dataNum]) * 2 + that.consx;
                                    }
                                    var result = originy + (xs[noedeindex] +forwardEllipse + localEllipse) * diry;
                                    if((xs[noedeindex] + forwardEllipse + localEllipse) >= sqrtdistance) {
                                        result = targety;
                                        d1.full = true; //放不下
                                    }
                                    else {
                                        d1.full = false;
                                    }
                                    d1.cy[numindex] = result;
                                    return result;
                                })
                                .style("fill", function(d1) {
                                    return that.fillColor(d1.allSigns[numindex] >= 0);
                                    return "white";
                                    if (d1.allSigns[numindex] >= 0) {
                                        return "#775AA9";
                                    }
                                    else {
                                        return "#FE8F00";
                                    }
                                })
                                .style("stroke-width", function(d1) {
                                    return 1;
                                }) //路径宽度
                                .style("stroke", function(d1) {
                                    return that.strokeColor(d1.allSigns[numindex] >= 0);
                                    if (d1.allSigns[numindex] >= 0) {
                                        return "#775AA9";
                                    }
                                    else {
                                        return "#FE8F00";
                                    }
                                })
                                .style("transform-origin", function(d1) {
                                    return (d1.cx[numindex] / width * 100).toString() + "% "  +( d1.cy[numindex] / height * 100).toString() +"%";
                                })
                                .style("transform", function(d1) {
                                    var deg = 0;
                                    var source = [xScale(d1.source.x), yScale(d1.source.y)];
                                    var target = [xScale(d1.target.x), yScale(d1.target.y)];
                                    var x_distance = target[0] - source[0];
                                    var y_distance = target[1] - source[1];
                                    var c_distance = Math.pow(x_distance, 2) + Math.pow(y_distance, 2);
                                    var distance = Math.sqrt(c_distance);
                                    if (x_distance == 0) {
                                        deg = 90;
                                    }
                                    else if (y_distance == 0) {
                                        deg = 0;
                                    } 
                                    else {
                                        var xielv = y_distance / x_distance;
                                        var sinA = Math.abs(y_distance) / distance;
                                        if (xielv < 0) {
                                            deg = 90 - Math.asin(sinA) * 180 / Math.PI;
                                        }
                                        else {
                                            deg = Math.asin(sinA) * 180 / Math.PI - 90;
                                            //console.log("-deg", deg);
                                        }
                                    }
                                    return "rotate("+ deg.toString() + "deg)";
                                })
                                //添加hover
                                .on("mouseenter", function(d1, i) {
                                    //console.log('mouseenter', d);
                                    return tipsDiv
                                        .style('visibility', 'visible')
                                        //.style('left', (d3.event.clientX - 920) + 'px') //- 400
                                        //.style('top', (d3.event.clientY - 300) + 'px') //- 110
                                        .style('right', '14px')
                                        //.style('top', '224px')
                                        .style('bottom', '0px')
                                        .html('ROI: ' + (d1.source.index + 1) + '  <->  ' + (d1.target.index + 1) + '<br>' + 'value: ' + d1.thick.toFixed(5) + '<br>' + 'allDatas: ' + d1.data);//tooltip.html(text);
                                    })
                                    .on("mouseout", function(d1, i) {
                                        //return tooltip.style('visibility', 'hidden');
                                        return tipsDiv.style('visibility', 'hidden');
                                    });
                                ellipse.append("line")
                                    .attr("class", function(d1,i) {
                                        return ".rowlink" + numindex;
                                    })
                                   .style("stroke-width", function(d1) {
                                        if (that.reverse){
                                            if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                                if (d1.thick == 0) return 0;
                                                if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray)) {
                                                    return 1;
                                                }
                                                return 0;
                                            }
                                            else {
                                                return 0;
                                            }
                                        }
                                        else return 4 - (d1.thick - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 4 
                                    })  //路径宽度
                                    .style("stroke", function(d1) {
                                        return that.lineColor(d1.allSigns[numindex] >= 0);
                                        if (d1.allSigns[numindex] >= 0) {
                                            return "#775AA9";
                                        }
                                        else {
                                            return "#FE8F00";
                                        }
                                    })
                                    .style("stroke-opacity", function(d1){ //不透明度
                                        return 1;
                                    })
                                    .attr("x1", function(d1){
                                        var rx = 0;
                                        var ry = 0;
                                        if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                            if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                                rx = that.widthScale(d1.data[numindex]);
                                                //rx = Math.abs(d1.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                            if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                                ry = that.widthScale(d1.data[(numindex+1)%dataNum]);
                                                //ry = Math.abs(d1.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        return d1.cx[numindex] + rx;
                                    })
                                    .attr("y1", function(d1){
                                        var rx = 0;
                                        var ry = 0;
                                        if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                            if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                                rx = that.widthScale(d1.data[numindex]);
                                                //rx = Math.abs(d1.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                            if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                                ry = that.widthScale(d1.data[(numindex+1)%dataNum]);
                                                //ry = Math.abs(d1.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        return d1.cy[numindex];
                                    })
                                    .attr("x2", function(d1){
                                        var rx = 0;
                                        var ry = 0;
                                        if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                            if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                                rx = that.widthScale(d1.data[numindex]);
                                                //rx = Math.abs(d1.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                            if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                                ry = that.widthScale(d1.data[(numindex+1)%dataNum]);
                                                //ry = Math.abs(d1.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        return d1.cx[numindex] - rx;
                                    })
                                    .attr("y2", function(d1){
                                        var rx = 0;
                                        var ry = 0;
                                        if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                            if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                                rx = that.widthScale(d1.data[numindex]);
                                                //rx = Math.abs(d1.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                            if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                                ry = that.widthScale(d1.data[(numindex+1)%dataNum]);
                                                //ry = Math.abs(d1.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        return d1.cy[numindex];
                                    })
                                    .style("transform-origin", function(d1) {
                                        return (d1.cx[numindex] / width * 100).toString() + "% "  +( d1.cy[numindex] / height * 100).toString() +"%";
                                    })
                                    .style("transform", function(d1) {
                                        var deg = 0;
                                        var source = [xScale(d1.source.x), yScale(d1.source.y)];
                                        var target = [xScale(d1.target.x), yScale(d1.target.y)];
                                        var x_distance = target[0] - source[0];
                                        var y_distance = target[1] - source[1];
                                        var c_distance = Math.pow(x_distance, 2) + Math.pow(y_distance, 2);
                                        var distance = Math.sqrt(c_distance);
                                        if (x_distance == 0) {
                                            deg = 90;
                                        }
                                        else if (y_distance == 0) {
                                           deg = 0;
                                        } 
                                        else {
                                            var xielv = y_distance / x_distance;
                                            var sinA = Math.abs(y_distance) / distance;
                                            if (xielv < 0) {
                                                deg = 90 - Math.asin(sinA) * 180 / Math.PI;
                                            }
                                            else {
                                                deg = Math.asin(sinA) * 180 / Math.PI - 90;
                                                //console.log("-deg", deg);
                                            }                
                                        }
                                        return "rotate("+ deg.toString() + "deg)";
                                    });
                        }

                        polygonup.selectAll(".ellipseup").remove();
                        for(var numindex = 0; numindex < dataNum; numindex++) {
                            var ellipse = polygonup.append("g")
                                .attr("class", function(d1,i){
                                    return "ellipseup" + numindex + " ellipseup";
                                });
                            ellipse.append("ellipse")
                                .attr("rx", function(d1){
                                    //console.log('d.data', d.data[numindex]);
                                    if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                        if (d1.thick == 0) return 0;
                                        if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray)){
                                            return that.widthScale(d1.data[numindex]);
                                            //return Math.abs(d1.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        return 0;
                                    }
                                    else {
                                        return 0;
                                    }            
                                })
                                .attr("ry", function(d1) {
                                    if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                        if (d1.thick == 0) return 0;
                                        if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray)){
                                            return that.widthScale(d1.data[(numindex+1)%dataNum]);
                                            //return Math.abs(d1.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        return 0;
                                    }
                                    else {
                                        return 0;
                                    }
                                })
                                .attr("cx", function(d1) {            
                                    return d1.cx[numindex];
                                })
                                .attr("cy", function(d1){
                                    return d1.cy[numindex];
                                })
                                .style("fill", function(d1) {
                                    return that.fillColor(d1.allSigns[numindex] >= 0);
                                    return "white";
                                    if (d1.allSigns[numindex] >= 0) {
                                        return "#775AA9";
                                    }
                                    else {
                                        return "#FE8F00";
                                    }
                                })
                                .style("stroke-width", function(d1) {
                                    return 1;
                                }) //路径宽度
                                .style("stroke", function(d1) {
                                    return that.strokeColor(d1.allSigns[numindex] >= 0);
                                    if (d1.allSigns[numindex] >= 0) {
                                        return "#775AA9";
                                    }
                                    else {
                                        return "#FE8F00";
                                    }
                                })
                                .style("transform-origin", function(d1) {
                                    return (d1.cx[numindex] / width * 100).toString() + "% "  +( d1.cy[numindex] / height * 100).toString() +"%";
                                })
                                .style("transform", function(d1) {
                                    var deg = 0;
                                    var source = [xScale(d1.source.x), yScale(d1.source.y)];
                                    var target = [xScale(d1.target.x), yScale(d1.target.y)];
                                    var x_distance = target[0] - source[0];
                                    var y_distance = target[1] - source[1];
                                    var c_distance = Math.pow(x_distance, 2) + Math.pow(y_distance, 2);
                                    var distance = Math.sqrt(c_distance);
                                    if (x_distance == 0) {
                                        deg = 90;
                                    }
                                    else if (y_distance == 0) {
                                        deg = 0;
                                    } 
                                    else {
                                        var xielv = y_distance / x_distance;
                                        var sinA = Math.abs(y_distance) / distance;
                                        if (xielv < 0) {
                                            deg = 90 - Math.asin(sinA) * 180 / Math.PI;
                                        }
                                        else {
                                            deg = Math.asin(sinA) * 180 / Math.PI - 90;
                                            //console.log("-deg", deg);
                                        }
                                    }
                                    return "rotate("+ deg.toString() + "deg)";
                                })
                                //添加hover
                                .on("mouseenter", function(d1, i) {
                                    //console.log('mouseenter', d);
                                    return tipsDiv
                                        .style('visibility', 'visible')
                                        //.style('left', (d3.event.clientX - 920) + 'px') //- 400
                                        //.style('top', (d3.event.clientY - 300) + 'px') //- 110
                                        .style('right', '14px')
                                        //.style('top', '224px')
                                        .style('bottom', '0px')
                                        .html('ROI: ' + (d1.source.index + 1) + '  <->  ' + (d1.target.index + 1) + '<br>' + 'value: ' + d1.thick.toFixed(5) + '<br>' + 'allDatas: ' + d1.data);//tooltip.html(text);
                                    })
                                    .on("mouseout", function(d1, i) {
                                        //return tooltip.style('visibility', 'hidden');
                                        return tipsDiv.style('visibility', 'hidden');
                                    })
                                    .call(that.drag);;
                                ellipse.append("line")
                                    .attr("class", function(d1,i) {
                                        return ".rowlink" + numindex;
                                    })
                                   .style("stroke-width", function(d1) {
                                        if (that.reverse){
                                            if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                                if (d1.thick == 0) return 0;
                                                if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray)) {
                                                    return 1;
                                                }
                                                return 0;
                                            }
                                            else {
                                                return 0;
                                            }
                                        }
                                        else return 4 - (d1.thick - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 4 
                                    })  //路径宽度
                                    .style("stroke", function(d1) {
                                        return that.lineColor(d1.allSigns[numindex] >= 0);
                                        if (d1.allSigns[numindex] >= 0) {
                                            return "#775AA9";
                                        }
                                        else {
                                            return "#FE8F00";
                                        }
                                    })
                                    .style("stroke-opacity", function(d1){ //不透明度
                                        return 1;
                                    })
                                    .attr("x1", function(d1){
                                        var rx = 0;
                                        var ry = 0;
                                        if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                            if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                                rx = that.widthScale(d1.data[numindex]);
                                                //rx = Math.abs(d1.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                            if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                                ry = that.widthScale(d1.data[(numindex+1)%dataNum]);
                                                //ry = Math.abs(d1.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        return d1.cx[numindex] + rx;
                                    })
                                    .attr("y1", function(d1){
                                        var rx = 0;
                                        var ry = 0;
                                        if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                            if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                                rx = that.widthScale(d1.data[numindex]);
                                                //rx = Math.abs(d1.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                            if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                                ry = that.widthScale(d1.data[(numindex+1)%dataNum]);
                                                //ry = Math.abs(d1.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        return d1.cy[numindex];
                                    })
                                    .attr("x2", function(d1){
                                        var rx = 0;
                                        var ry = 0;
                                        if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                            if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                                rx = that.widthScale(d1.data[numindex]);
                                                //rx = Math.abs(d1.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                            if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                                ry = that.widthScale(d1.data[(numindex+1)%dataNum]);
                                                //ry = Math.abs(d1.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        return d1.cx[numindex] - rx;
                                    })
                                    .attr("y2", function(d1){
                                        var rx = 0;
                                        var ry = 0;
                                        if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                            if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                                rx = that.widthScale(d1.data[numindex]);
                                                //rx = Math.abs(d1.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                            if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                                ry = that.widthScale(d1.data[(numindex+1)%dataNum]);
                                                //ry = Math.abs(d1.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                        }
                                        return d1.cy[numindex];
                                    })
                                    .style("transform-origin", function(d1) {
                                        return (d1.cx[numindex] / width * 100).toString() + "% "  +( d1.cy[numindex] / height * 100).toString() +"%";
                                    })
                                    .style("transform", function(d1) {
                                        var deg = 0;
                                        var source = [xScale(d1.source.x), yScale(d1.source.y)];
                                        var target = [xScale(d1.target.x), yScale(d1.target.y)];
                                        var x_distance = target[0] - source[0];
                                        var y_distance = target[1] - source[1];
                                        var c_distance = Math.pow(x_distance, 2) + Math.pow(y_distance, 2);
                                        var distance = Math.sqrt(c_distance);
                                        if (x_distance == 0) {
                                            deg = 90;
                                        }
                                        else if (y_distance == 0) {
                                           deg = 0;
                                        } 
                                        else {
                                            var xielv = y_distance / x_distance;
                                            var sinA = Math.abs(y_distance) / distance;
                                            if (xielv < 0) {
                                                deg = 90 - Math.asin(sinA) * 180 / Math.PI;
                                            }
                                            else {
                                                deg = Math.asin(sinA) * 180 / Math.PI - 90;
                                                //console.log("-deg", deg);
                                            }                
                                        }
                                        return "rotate("+ deg.toString() + "deg)";
                                    });
                            }

                        }
                        //选择的线显示,以及连接到的节点显示
                        svgLeft.selectAll(".polygonup").filter(function(d1) {
                            return that.selectedNodes[d1.source.index] == 1 || that.selectedNodes[d1.target.index] == 1;
                        }).style("opacity",  function(d1){
                            if (that.reverse){
                            if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                if (d1.thick == 0) return 0;
                                if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                    {
                                        d1.select = 1;
                                        svgLeft.selectAll(".nodeup").filter(function(d2, i){
                                            return d2.index == d1.source.index || d2.index == d1.target.index;
                                        }).style("opacity", 1);
                                        return 1;
                                    }
                                return 0;
                            }
                            else {
                                return 0;
                            }
                        }
                        else return 4 - (d1.thick - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 4 
                        });
                        //未选择的线不显示
                        svgLeft.selectAll(".polygonup").filter(function(d1) {
                            return !(that.selectedNodes[d1.source.index] == 1 || that.selectedNodes[d1.target.index] == 1);
                        }).style("opacity",  0);
                        //调整底层显示的虚线
                        svgLeft.selectAll(".dottedlink").style("stroke-opacity",  function(d1){
                            if(d1.full == true && d1.thick > 0 && d1.thick >= that.fillMin) return 1;
                            return 0;
                        })
                        .style("stroke-width", function(d1){
                            if(d1.full == true && d1.thick > 0 && d1.thick >= that.fillMin) return 1;
                            return 0;
                        });;
                        //选择的虚线显示
                        svgLeft.selectAll(".dottedlinkup").filter(function(d1) {
                            return that.selectedNodes[d1.source.index] == 1 || that.selectedNodes[d1.target.index] == 1;
                        }).style("stroke-opacity",  function(d1){
                            if(d1.full == true && d1.thick > 0 && d1.thick >= that.fillMin) return 1;
                            return 0;
                        })
                        .style("stroke-width", function(d1){
                            if(d1.full == true && d1.thick > 0 && d1.thick >= that.fillMin) return 1;
                            return 0;
                        });
                        //选择不到的虚线不显示
                        svgLeft.selectAll(".dottedlinkup").filter(function(d1) {
                            return !(that.selectedNodes[d1.source.index] == 1 || that.selectedNodes[d1.target.index] == 1);
                        }).style("stroke-opacity",  0).style("stroke-width", 0);

                        //click end
                        that.loading = false;
                        }, 1);
                    });
        /*
                //添加虚线
                var linkLeft = svgLeft.selectAll(".dottedlinkup")
                    .data(that.matrix.links)
                    .enter()
                    .append("line")
                    .attr("class", function(d,i) {
                        return "leftl" + that.clusterNo(d.source.index, clusters) + "l" + that.clusterNo(d.target.index, clusters) + "leftl" + that.clusterNo(d.source.index, clusters) + "leftl" + that.clusterNo(d.target.index, clusters) +" dottedlinkup";
                    })
                    .style("stroke-width", 1)  //路径宽度
                    .style("stroke", function(d){//路径颜色
                        return "red";
                    })
                    .style("stroke-opacity", function(d){ //不透明度
                        if(d.full == null) return 0;
                        return 1;
                    })
                    .style("stroke-dasharray", "3")
                    .sort(function(a, b) {
                        return a.thick - b.thick;
                    })
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
                    });*/

                //添加图标显示节点
                nodeLeftup.append("circle")
                    .attr("class", function(d,i){
                        return "lobe" + that.clusterNo(i, clusters) + "Circle" + that.index;
                    })
                    .attr("r", 9)
                    .style("fill", "white")
                    .style("stroke", "black")
                    .style("stroke-width", 2);

                //文字
                nodeLeftup.append("text")
                    .attr("class", function(d,i){
                        return "lobe" + that.clusterNo(i, clusters) + "Text" + that.index;
                    })
                    .attr("font-family", "Arial")
                    .attr("y", 6)
                    .style("text-anchor", "middle")
                    .style("stroke-width", 0)
                    .style("fill", "black")
                    .style("font-size", "13px")
                    .style("font-weight", 300)
                    .text(function(d) {
                        return d.index + 1;
                    });

                //添加图例
                var rx = 8;
                var ry = 6;
                var center = 16;
                var jianju = 28;
                var startpoint = 20;
                //toptitle = "Mean Male,Female Group";
                //pearlName = [{'name':'Strength,Length', "index": 0},{'name':'Length,Curvature', "index": 1},{'name':'Curvature,Strength', "index": 2}];
                var BEpoints = [{'index': 0}, {'index': 1}];
                if(pearlName.length <= 0) toptitle = "";
                if(1) {
                    //粗细不一的线
                    var legendpolygon = svgLeft.selectAll(".legendpolygon")
                        .data([sizeobj])
                        .enter()
                        .append("g")
                        .attr("class", function(d,i) {
                            return "legendpolygon";
                        });
                    legendpolygon.append("polygon")
                        .attr("class", function(d,i){
                            return +"legendpolygoneline";
                        })
                        .style("fill", function(d) {
                            return d3.rgb(205 - 1 * 138, 197 - 1 * 165, 224 - 1 * 85).toString();
                        })
                        .attr("points", function(d){
                            return "13," + startpoint + " 19," + startpoint + " 16.5," + ((pearlName.length + 1) * jianju + startpoint).toString() + " 15.5," + ((pearlName.length + 1) * jianju + startpoint).toString();
                        })
                        .style("opacity", function(d) {
                            return 1;
                        });
                    legendpolygon.append("text")
                        .attr("class", function(d,i){
                            return +"uplegText";
                        })
                        .attr("font-family", "Arial")
                        .attr("y", 6)
                        .style("stroke-width", 0)
                        .style("fill", "#8998AC")
                        .style("font-size", "10px")
                        .style("font-weight", 300)
                        .text(function(d) {
                            return toptitle;
                        })
                        .attr("text-anchor", "start")
                        .attr("transform", function(d){
                            return "translate(" +(center) + "," + (0) +")";
                        });
                    //添加椭圆
                    
                    var legendellipse = legendpolygon.selectAll(".legendellipse")
                        .data(pearlName)
                        .enter()
                        .append("g")
                        .attr("class", function(d,i){
                            return "legendellipse" + numindex + " legendellipse";
                        });

                    legendellipse.append("ellipse")
                        .attr("rx", function(d){
                            return rx;          
                        })
                        .attr("ry", function(d) {
                            return ry;
                        })
                        .attr("cx", function(d) {
                            return center;
                        })
                        .attr("cy", function(d){
                            return (d.index + 1) * jianju + startpoint;
                        })
                        .style("fill", function(d) {
                            //return "white";
                            return that.fillColor(d.index % 2 == 0);
                            return "#775AA9";
                            return "#FE8F00";
                        })
                        .style("stroke-width", function(d) {
                            return 1;
                        }) //路径宽度
                        .style("stroke", function(d) {
                            return that.strokeColor(d.index % 2 == 0);
                            return "white"
                            return "#775AA9";
                            return "#FE8F00";
                        });
                    //添加线
                    legendellipse.append("line")
                        .attr("class", function(d,i) {
                            return ".legendrowlink" + numindex;
                        })
                        .style("stroke-width", function(d) {
                            return 1;
                        })  //路径宽度
                        .style("stroke", function(d) {
                            return that.lineColor(d.index % 2 == 0);
                            return "black";
                            return "#775AA9";
                            return "#FE8F00";
                        })
                        .style("stroke-opacity", function(d){ //不透明度
                            return 1;
                        })
                        .attr("x1", function(d){
                            return center - rx;
                        })
                        .attr("y1", function(d){
                            return (d.index + 1) * jianju + startpoint;
                        })
                        .attr("x2", function(d){
                            return center + rx;
                        })
                        .attr("y2", function(d){
                            return (d.index + 1) * jianju + startpoint;
                        });
                    legendellipse.append("text")
                        .attr("class", function(d,i){
                            return +"sidelegText";
                        })
                        .attr("font-family", "Arial")
                        .attr("y", 6)
                        .style("stroke-width", 0)
                        .style("fill", "#8998AC")
                        .style("font-size", "10px")
                        .style("font-weight", 300)
                        .text(function(d) {
                            return d.name;
                        })
                        .attr("text-anchor", "start")
                        .attr("transform", function(d){
                            return "translate(" +(center + rx + 5) + "," + ((d.index + 1) * jianju + startpoint - 3) +")";
                        });
                    //添加节点
                    var legnodeLeft = svgLeft.selectAll(".legnode")
                        .data(BEpoints)
                        .enter()
                        .append("g")
                        .attr("class", "legnode")
                        .attr("cx", function(d){
                            return center;
                        })
                        .attr("cy", function(d){
                            return d.index * (pearlName.length + 1) * jianju + startpoint;
                        })
                        .attr("transform", function(d){
                            return "translate(" +center + "," + (d.index * (pearlName.length + 1) * jianju + startpoint) +")";
                        });

                    //添加图标显示节点
                    legnodeLeft.append("circle")
                        .attr("class", function(d,i){
                            return "legcircle";
                        })
                        .attr("r", 9)
                        .style("fill", "white")
                        .style("stroke", "black")
                        .style("stroke-width", 2);
                    //文字
                    legnodeLeft.append("text")
                        .attr("class", function(d,i){
                            return "legText";
                        })
                        .attr("font-family", "Arial")
                        .attr("y", 6)
                        .style("text-anchor", "middle")
                        .style("stroke-width", 0)
                        .style("fill", "black")
                        .style("font-size", "13px")
                        .style("font-weight", 300)
                        .text(function(d) {
                            return d.index + 1;
                        });
                }

                //动态改变线和节点的位置
                //forceLeft.on("tick", function(){
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

                    /*
                    polygon.attr("points", function(d){
                        return xScale(d.source.x).toString() + ',' + (yScale(d.source.y) - 2).toString() + " " + 
                                xScale(d.target.x).toString() + ',' + (yScale(d.target.y) - 1).toString() + " " +
                                xScale(d.target.x).toString() + ',' + (yScale(d.target.y) + 1).toString() + " " +
                                xScale(d.source.x).toString() + ',' + (yScale(d.source.y) + 2).toString() + " " ;
                    });
                    */
                    /*
                    attr("x", function(d){
                        return xScale(d.source.x);
                    })
                    .attr("y", function(d){
                        return yScale(d.source.y) - 2;
                    })
                    .attr("x1", function(d){
                        return xScale(d.target.x);
                    })
                    .attr("y1", function(d){
                        return yScale(d.target.y) - 1;
                    })
                    .attr("x2", function(d){
                        return xScale(d.target.x);
                    })
                    .attr("y2", function(d){
                        return yScale(d.target.y) + 1;
                    })
                    .attr("x3", function(d){
                        return xScale(d.source.x);
                    })
                    .attr("y3", function(d){
                        return yScale(d.source.y) + 2;
                    });
                    */
                        /*
                    ellipse[0].attr("cx", function(d) {
                        return (xScale(d.source.x) + xScale(d.target.x)) / 2;
                    })
                    .attr("cy", function(d){
                        return (yScale(d.source.y)+ yScale(d.target.y)) / 2;
                    })
                    ;
                    */
                    nodeLeft.attr("cx", function(d){
                        return xScale(d.x);
                    })
                    .attr("cy", function(d){
                        return yScale(d.y);
                    })
                    .attr("transform", function(d){
                        if(connection[d.index] <= 0) return "translate(" + xScale(d.x) + "," + yScale(d.y) +") " + "scale(0.2)";
                        return "translate(" + xScale(d.x) + "," + yScale(d.y) +")";
                    });

                    nodeLeftup.attr("cx", function(d){
                        return xScale(d.x);
                    })
                    .attr("cy", function(d){
                        return yScale(d.y);
                    })
                    .attr("transform", function(d){
                        if(connection[d.index] <= 0) return "translate(" + xScale(d.x) + "," + yScale(d.y) +") " + "scale(0.2)";
                        return "translate(" + xScale(d.x) + "," + yScale(d.y) +")";
                    });
                //});
               
                
                d3.select(this.$el)         
                    .select(s)
                    .select(".panel")
                    .select(".text1")
                    .html("Fill range: " + this.thick_range.r1 + "-" + this.thick_range.r2);

                this.$store.commit('setneedReDraw', false);
            }
        },
        watch: {
            strength: {
                deep: true,
                handler: function(newVal, oldVal) {
                    if(newVal && this.strength.allDatas.length > 0){
                        //console.log('strength', newVal);
                        this.loading = true;
                        //this.bus.$emit('loading', true);
                        //this.$forceUpdate();
                        //this.load = true;
                        //this.loading = false;
                        var that = this;
                        setTimeout(function(){
                            that.drawSidBySidePic();
                            that.loading = false;
                        }, 0);
                        /*
                        this.$nextTick(function(){
                            this.drawSidBySidePic();
                            this.loading = false;
                            //this.bus.$emit('loading', false);
                        });
                        */
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
                    that.fillMin = newVal[0] * (that.thick_range.r2 - that.thick_range.r1) / 100 + that.thick_range.r1;
                    that.fillMax = newVal[1] * (that.thick_range.r2 - that.thick_range.r1) / 100 + that.thick_range.r1 + 100;
                    svgLeft.selectAll(".link")
                        .style("stroke-width", function(d) {
                            return 0;
                            if (that.reverse){
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if (d.thick == 0) return 0;
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        return (d.thick - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 3 + 1;
                                    return 0;
                                }
                                else {
                                    return 0;
                                }
                            }
                            else return 4 - (d.thick - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 4
                        })
                        .style("stroke", function(d){
                            if(d.thick >= that.fillMin && d.thick <= that.fillMax){
                                if(d.sign == 1){
                                    return that.fillScaleOne(d.thick - that.fillMin, that.fillMax - that.fillMin);
                                }
                                else if(d.sign == -1) {
                                    return that.fillScaleTwo(d.thick - that.fillMin, that.fillMax - that.fillMin);
                                }
                                else {
                                    return that.fillScaleGray(d.thick - that.fillMin, that.fillMax - that.fillMin);
                                }
                            } else {
                                return d3.rgb(255, 255, 255).toString();
                            }
                        })
                        .style("stroke-opacity", function(d){
                            return 1;
                        });

                    svgLeft.selectAll(".link2")
                        .style("stroke-width", function(d) {
                            return 0;
                            if (that.reverse){
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if (d.thick == 0) return 0;
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        return 4;
                                    return 0;
                                }
                                else {
                                    return 0;
                                }
                            }
                            else return 4 - (d.thick - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 4
                        })
                        .style("stroke", function(d){
                            return d3.rgb(0, 0, 255).toString();
                        })
                        .style("stroke-opacity", function(d){
                            return 0;
                        });

                    //点的权重，选中的+3000
                    var connection = [];
                    for (var i = 0; i < 70; i++) {
                        if(that.selectedNodes[i] == 1) {
                            connection.push(3000);
                        }
                        else connection.push(0);
                    }
                    this.matrix.links.forEach(function(d,i){
                        if(d.thick != 0)
                        if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                            if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray)) {
                                connection[d.source.index]++;
                                connection[d.target.index]++;
                            }
                        }
                    });

                    var xScale = d3.scale.linear() //d3.scaleLinear()
                        .domain([0, this.scalex_range.x2])
                        .range([-100*this.scaleWidth/905 + 30, this.scaleWidth-100*this.scaleWidth/905 + 30]);
                    var yScale = d3.scale.linear() //d3.scaleLinear()
                        .domain([0,this.scaley_range.y2])
                        .range([-70*this.scaleHeight/1030 + 20, this.scaleHeight-70*this.scaleHeight/1030 + 20]);
                    svgLeft.selectAll(".polygon")
                    .attr("points", function(d){
                        //先判断前后
                        if(connection[d.source.index] >= connection[d.target.index])
                            return xScale(d.source.x).toString() + ',' + (yScale(d.source.y) - 2).toString() + " " + 
                                xScale(d.target.x).toString() + ',' + (yScale(d.target.y) - 0.5).toString() + " " +
                                xScale(d.target.x).toString() + ',' + (yScale(d.target.y) + 0.5).toString() + " " +
                                xScale(d.source.x).toString() + ',' + (yScale(d.source.y) + 2).toString() + " " ;
                        else return xScale(d.target.x).toString() + ',' + (yScale(d.target.y) - 2).toString() + " " + 
                                xScale(d.source.x).toString() + ',' + (yScale(d.source.y) - 0.5).toString() + " " +
                                xScale(d.source.x).toString() + ',' + (yScale(d.source.y) + 0.5).toString() + " " +
                                xScale(d.target.x).toString() + ',' + (yScale(d.target.y) + 2).toString() + " " ;
                    })
                    .style("opacity", function(d) {
                        if (that.reverse){
                            if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                if (d.thick == 0) return 0;
                                if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                    return 1;
                                return 0;
                            }
                            else {
                                return 0;
                            }
                        }
                        else return 4 - (d.thick - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 4 
                    }) ;

                    var dataNum = this.dataLength;
                    for(var numindex = 0; numindex < dataNum; numindex++) {
                    var classname = ".ellipse" + numindex;
                    svgLeft.selectAll(classname)
                        .attr("rx", function(d){
                            //console.log('d.data', d.data[numindex]);
                            if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                if (d.thick == 0) return 0;
                                if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                    return that.widthScale(d.data[numindex]);
                                    //return Math.abs(d.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                return 0;
                            }
                            else {
                                return 0;
                            }
                        })
                        .attr("ry", function(d) {
                            if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                if (d.thick == 0) return 0;
                                if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                    return that.widthScale(d.data[(numindex + 1) % dataNum]);
                                    //return Math.abs(d.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                return 0;
                            }
                            else {
                                return 0;
                            }
                        })
                        .attr("cx", function(d) {
                            if(connection[d.source.index] >= connection[d.target.index])
                                return (xScale(d.target.x) - xScale(d.source.x)) / (dataNum + 1) * (numindex + 1) + xScale(d.source.x);
                            else return (xScale(d.source.x) - xScale(d.target.x)) / (dataNum + 1) * (numindex + 1) + xScale(d.target.x);
                        })
                        .attr("cy", function(d){
                            if(connection[d.source.index] >= connection[d.target.index])
                                return (yScale(d.target.y) - yScale(d.source.y)) / (dataNum + 1) * (numindex + 1) + yScale(d.source.y);
                            return (yScale(d.source.y) - yScale(d.target.y)) / (dataNum + 1) * (numindex + 1) + yScale(d.target.y);
                        });
                    }
                    
                    svgLeft.selectAll(".node")
                    .sort(function(a, b) {
                        return connection[a.index] - connection[b.index];
                    });
                    
                }

            },
            ellipseWidth: {
                deep: true,
                handler: function(newVal, oldVal) {
                    //console.log("filterRate", newVal);
                    //console.log(d3.select(this.$el));
                    var s = ".draw-area-" + this.index;
                    var svgLeft = d3.select(this.$el)      
                        .select(s).select(".leftSVG");
                    var tipsDiv = d3.select(this.$el)         
                        .select(s)    /*返回第一个匹配的元素*/
                        .select('.leftSVG')
                        .select(".tipsDiv");
                    var that = this;
                    this.loading = true;
                    setTimeout(function(){
                    that.fillMin = that.filterRate[0] * (that.thick_range.r2 - that.thick_range.r1) / 100 + that.thick_range.r1;
                    that.fillMax = that.filterRate[1] * (that.thick_range.r2 - that.thick_range.r1) / 100 + that.thick_range.r1 + 100;
                    var dataNum = that.dataLength;
                    that.minx = that.ellipseWidth;
                    var height = window.screen.height * that.heightPer;
                    var width = height * 905 / 1030;
                    var maxx = 60;
                    //点的权重，选中的+3000
                    var connection = [];
                    for (var i = 0; i < 70; i++) {
                        if(that.selectedNodes[i] == 1) {
                            connection.push(3000);
                        }
                        else connection.push(0);
                    }
                    that.matrix.links.forEach(function(d,i){
                        if(d.thick != 0)
                        if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                            if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray)) {
                                connection[d.source.index]++;
                                connection[d.target.index]++;
                            }
                        }
                    });

                    var xScale = d3.scale.linear() //d3.scaleLinear()
                        .domain([0, that.scalex_range.x2])
                        .range([-100*that.scaleWidth/905 + 30, that.scaleWidth-100*that.scaleWidth/905 + 30]);
                    var yScale = d3.scale.linear() //d3.scaleLinear()
                        .domain([0,that.scaley_range.y2])
                        .range([-70*that.scaleHeight/1030 + 20, that.scaleHeight-70*that.scaleHeight/1030 + 20]);

                    that.widthScale = d3.scale.linear()
                        .domain([0, 1])
                        .range([that.ellipseWidthMin, that.ellipseWidth]);
                    
                    //重画所有的椭圆
                    var xs = []; //存储每个节点的x
                    var scores = []; //存储每个节点的score
                    for(var i = 0; i < 70; i++) {
                        xs.push(that.minx);
                        scores.push(0);
                    }
                    if(dataNum > 0) {
                        //对于每个节点计算它的score
                        for(var i = 0; i < 70; i++) {
                            //假设节点被选中，则其x=that.minx,否则计算x
                            if(that.selectedNodes[i] == 1) {
                                xs[i] = that.minx;
                            }
                            else {
                                //计算maxx
                                maxx = 999;
                                var count = 0;
                                that.matrix.links.filter(function(d1){
                                    return ((d1.source.index == i && connection[d1.source.index] >= connection[d1.target.index]) || (d1.target.index == i && connection[d1.source.index] <= connection[d1.target.index])) && Math.abs(d1.thick) > 0;
                                }).forEach(function(d1){
                                    var dx = xScale(d1.target.x) - xScale(d1.source.x);
                                    var dy = yScale(d1.target.y) - yScale(d1.source.y);
                                    var sqrtdistance = Math.sqrt(dx * dx + dy * dy);
                                    //maxx += sqrtdistance;
                                    for(var si = 0; si < dataNum; si++) {
                                        sqrtdistance -= (that.widthScale(d1.data[si]) * 2);
                                    }
                                    if(sqrtdistance < 0) sqrtdistance = 0;
                                    if(sqrtdistance < maxx) maxx = sqrtdistance;
                                    count++;
                                });
                                //if(count != 0) maxx = maxx / count;
                                maxx = maxx - that.consx * (dataNum - 1);
                                if(maxx < 0) maxx = 0;
                                //计算每一步寻找最大的score
                                for(var x = that.minx; x < maxx; x=x+that.step) {
                                    var score = 0;
                                    //找到所有与该节点相连的线，该节点重要性大于另一节点
                                    that.matrix.links.filter(function(d1){
                                        return ((d1.source.index == i && connection[d1.source.index] >= connection[d1.target.index]) || (d1.target.index == i && connection[d1.source.index] <= connection[d1.target.index])) && Math.abs(d1.thick) > 0;
                                    }).forEach(function(d2){
                                        //对于该线上的所有椭圆，计算minscore
                                        //节点原始坐标
                                        var originx = 0;
                                        var originy = 0;
                                        //延直线扩展的单位
                                        var dirx = 0;
                                        var diry = 0;
                                        if(d2.source.index == i) {
                                            originx = xScale(d2.source.x);
                                            originy = yScale(d2.source.y);
                                            var dx = xScale(d2.target.x) - xScale(d2.source.x);
                                            var dy = yScale(d2.target.y) - yScale(d2.source.y);
                                            var sqrtdistance = Math.sqrt(dx * dx + dy * dy);
                                            dirx = dx / sqrtdistance;
                                            diry = dy / sqrtdistance;
                                        }
                                        else {
                                            originx = xScale(d2.target.x);
                                            originy = yScale(d2.target.y);
                                            var dx = xScale(d2.source.x) - xScale(d2.target.x);
                                            var dy = yScale(d2.source.y) - yScale(d2.target.y);
                                            var sqrtdistance = Math.sqrt(dx * dx + dy * dy);
                                            dirx = dx / sqrtdistance;
                                            diry = dy / sqrtdistance;
                                        }
                                        //对于线上每个椭圆
                                        for (var j = 0; j < dataNum; j++) {
                                            var minscore = 9999;
                                            var x0 = originx + (x + j * that.consx) * dirx;
                                            var y0 = originy + (x + j * that.consx) * diry;
                                            //找到与该线条不同的所有线,计算点到直线的最小距离
                                            that.matrix.links.filter(function(d1){
                                                return !(d1.in == d2.in) && Math.abs(d1.thick) > 0;
                                            }).forEach(function(d1) {
                                                //var x0 = originx + (x + j * that.consx) * dirx;
                                                //var y0 = originy + (x + j * that.consx) * diry;
                                                var distance = that.caldistance(x0, y0, xScale(d1.source.x), yScale(d1.source.y), xScale(d1.target.x), yScale(d1.target.y));
                                                if (distance < minscore) {
                                                    minscore = distance;
                                                }
                                            });
                                            score = score + that.alpha * minscore;
                                            var minEM = 999;
                                            //找到高亮的所有线，计算点到线上椭圆的最小距离
                                            that.matrix.links.filter(function(d1){
                                                return (that.selectedNodes[d1.source.index] == 1 || that.selectedNodes[d1.target.index] == 1) && Math.abs(d1.thick) > 0;
                                            }).forEach(function(d1) {
                                                //计算出该条线上椭圆位置
                                                var newpoints;
                                                if(connection[d1.source.index] >= connection[d1.target.index]) {
                                                    //calposition(x0, y0, x1, y1, that.minx, constx, dataNum)
                                                    newpoints = that.calposition(xScale(d1.source.x), yScale(d1.source.y), xScale(d1.target.x), yScale(d1.target.y), that.minx, that.consx, dataNum);
                                                }
                                                else{
                                                    newpoints = that.calposition(xScale(d1.target.x), yScale(d1.target.y), xScale(d1.source.x), yScale(d1.source.y), that.minx, that.consx, dataNum);
                                                }
                                                for(var p in newpoints) {
                                                    var distance = that.pointdistance(newpoints[p][0], newpoints[p][1], x0, y0);
                                                    if (distance < minEM) {
                                                        minEM = distance;
                                                    }
                                                }
                                            });
                                            score = score + (1 - that.alpha) * minEM;
                                        }
                                    });
                                    if(score > scores[i]) {
                                        scores[i] = score;
                                        xs[i] = x;
                                    }
                                }
                            }
                        }
                    }
                    console.log('change slider xs', xs);
                    //重画所有的椭圆
                    var polygon = svgLeft.selectAll(".polygon");
                    var polygonup = svgLeft.selectAll(".polygonup");
                    polygon.selectAll(".ellipse").remove();
                    for(var numindex = 0; numindex < dataNum; numindex++) {
                        var classname = ".ellipse" + numindex;
                        var ellipse = polygon.append("g")
                            .attr("class", function(d1,i){
                                return "ellipse" + numindex + " ellipse";
                            });
                        ellipse.append("ellipse")
                            .attr("rx", function(d1){
                                //console.log('d.data', d.data[numindex]);
                                if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                    if (d1.thick == 0) return 0;
                                    if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray)){
                                        return that.widthScale(d1.data[numindex]);
                                        //return Math.abs(d1.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                    }
                                    return 0;
                                }
                                else {
                                    return 0;
                                }            
                            })
                            .attr("ry", function(d1) {
                                if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                    if (d1.thick == 0) return 0;
                                    if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray)){
                                        return that.widthScale(d1.data[(numindex+1)%dataNum]);
                                        //return Math.abs(d1.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                    }
                                    return 0;
                                }
                                else {
                                    return 0;
                                }
                            })
                            .attr("cx", function(d1) {            
                                var originx = 0;
                                var originy = 0;
                                var targetx = 0;
                                var targety = 0;
                                //延直线扩展的单位
                                var dirx = 0;
                                var diry = 0;
                                var noedeindex = 0;
                                var sqrtdistance = 0;
                                if(connection[d1.source.index] >= connection[d1.target.index]) {
                                    noedeindex = Number(d1.source.index);
                                    originx = xScale(d1.source.x);
                                    originy = yScale(d1.source.y);
                                    targetx = xScale(d1.target.x);
                                    targety = yScale(d1.target.y);
                                    var dx = xScale(d1.target.x) - xScale(d1.source.x);
                                    var dy = yScale(d1.target.y) - yScale(d1.source.y);
                                    sqrtdistance = Math.sqrt(dx * dx + dy * dy);
                                    if(sqrtdistance == 0) {
                                        d1.cx[numindex] = originx;
                                        return originx;
                                    }
                                    dirx = dx / sqrtdistance;
                                    diry = dy / sqrtdistance;
                                }
                                else {
                                    noedeindex = Number(d1.target.index);
                                    originx = xScale(d1.target.x);
                                    originy = yScale(d1.target.y);
                                    targetx = xScale(d1.source.x);
                                    targety = yScale(d1.source.y);
                                    var dx = xScale(d1.source.x) - xScale(d1.target.x);
                                    var dy = yScale(d1.source.y) - yScale(d1.target.y);
                                    sqrtdistance = Math.sqrt(dx * dx + dy * dy);
                                    if(sqrtdistance == 0) {
                                        d1.cx[numindex] = originx;
                                        return originx;
                                    }
                                    dirx = dx / sqrtdistance;
                                    diry = dy / sqrtdistance;
                                }
                                var forwardEllipse = 0;
                                var localEllipse = that.widthScale(d1.data[(numindex + 1) % dataNum]);
                                for(var ti = 0; ti < numindex; ti++) {
                                    forwardEllipse = that.widthScale(d1.data[(ti + 1) % dataNum]) * 2 + that.consx;
                                }
                                var result = originx + (xs[noedeindex] +forwardEllipse + localEllipse) * dirx;
                                if((xs[noedeindex] + forwardEllipse + localEllipse) >= sqrtdistance) {
                                    result = targetx;
                                    d1.full = true; //放不下
                                }
                                else {
                                    d1.full = false;
                                }
                                d1.cx[numindex] = result;
                                //d.cx.push(result);
                                //console.log('result', result);
                                return result;
                            })
                            .attr("cy", function(d1){
                                var originx = 0;
                                var originy = 0;
                                var targetx = 0;
                                var targety = 0;
                                //延直线扩展的单位
                                var dirx = 0;
                                var diry = 0;
                                var noedeindex = 0;
                                var sqrtdistance = 0;
                                if(connection[d1.source.index] >= connection[d1.target.index]) {
                                    noedeindex = Number(d1.source.index);
                                    originx = xScale(d1.source.x);
                                    originy = yScale(d1.source.y);
                                    targetx = xScale(d1.target.x);
                                    targety = yScale(d1.target.y);
                                    var dx = xScale(d1.target.x) - xScale(d1.source.x);
                                    var dy = yScale(d1.target.y) - yScale(d1.source.y);
                                    sqrtdistance = Math.sqrt(dx * dx + dy * dy);
                                    //console.log('sqrtdistance', sqrtdistance);
                                    if(sqrtdistance == 0) {
                                        d1.cy[numindex] = originy;
                                        return originy;
                                    }
                                    dirx = dx / sqrtdistance;
                                    diry = dy / sqrtdistance;
                                }
                                else {
                                    noedeindex = Number(d1.target.index);
                                    originx = xScale(d1.target.x);
                                    originy = yScale(d1.target.y);
                                    targetx = xScale(d1.source.x);
                                    targety = yScale(d1.source.y);
                                    var dx = xScale(d1.source.x) - xScale(d1.target.x);
                                    var dy = yScale(d1.source.y) - yScale(d1.target.y);
                                    sqrtdistance = Math.sqrt(dx * dx + dy * dy);
                                    //console.log('sqrtdistance', sqrtdistance);
                                    if(sqrtdistance == 0) {
                                        d1.cy[numindex] = originy;
                                        return originy;
                                    }
                                    dirx = dx / sqrtdistance;
                                    diry = dy / sqrtdistance;
                                }
                                var forwardEllipse = 0;
                                var localEllipse = that.widthScale(d1.data[(numindex + 1) % dataNum]);
                                for(var ti = 0; ti < numindex; ti++) {
                                    forwardEllipse = that.widthScale(d1.data[(ti + 1) % dataNum]) * 2 + that.consx;
                                }
                                var result = originy + (xs[noedeindex] +forwardEllipse + localEllipse) * diry;
                                if((xs[noedeindex] + forwardEllipse + localEllipse) >= sqrtdistance) {
                                    result = targety;
                                    d1.full = true; //放不下
                                }
                                else {
                                    d1.full = false;
                                }
                                d1.cy[numindex] = result;
                                return result;
                            })
                            .style("fill", function(d1) {
                                return that.fillColor(d1.allSigns[numindex] >= 0);
                                return "white";
                                if (d1.allSigns[numindex] >= 0) {
                                    return "#775AA9";
                                }
                                else {
                                    return "#FE8F00";
                                }
                            })
                            .style("stroke-width", function(d1) {
                                return 1;
                            }) //路径宽度
                            .style("stroke", function(d1) {
                                return that.strokeColor(d1.allSigns[numindex] >= 0);
                                if (d1.allSigns[numindex] >= 0) {
                                    return "#775AA9";
                                }
                                else {
                                    return "#FE8F00";
                                }
                            })
                            .style("transform-origin", function(d1) {
                                return (d1.cx[numindex] / width * 100).toString() + "% "  +( d1.cy[numindex] / height * 100).toString() +"%";
                            })
                            .style("transform", function(d1) {
                                var deg = 0;
                                var source = [xScale(d1.source.x), yScale(d1.source.y)];
                                var target = [xScale(d1.target.x), yScale(d1.target.y)];
                                var x_distance = target[0] - source[0];
                                var y_distance = target[1] - source[1];
                                var c_distance = Math.pow(x_distance, 2) + Math.pow(y_distance, 2);
                                var distance = Math.sqrt(c_distance);
                                if (x_distance == 0) {
                                    deg = 90;
                                }
                                else if (y_distance == 0) {
                                    deg = 0;
                                } 
                                else {
                                    var xielv = y_distance / x_distance;
                                    var sinA = Math.abs(y_distance) / distance;
                                    if (xielv < 0) {
                                        deg = 90 - Math.asin(sinA) * 180 / Math.PI;
                                    }
                                    else {
                                        deg = Math.asin(sinA) * 180 / Math.PI - 90;
                                        //console.log("-deg", deg);
                                    }
                                }
                                return "rotate("+ deg.toString() + "deg)";
                            })
                            //添加hover
                            .on("mouseenter", function(d1, i) {
                                //console.log('mouseenter', d);
                                return tipsDiv
                                        .style('visibility', 'visible')
                                        //.style('left', (d3.event.clientX - 920) + 'px') //- 400
                                        //.style('top', (d3.event.clientY - 300) + 'px') //- 110
                                        .style('right', '14px')
                                        //.style('top', '224px')
                                        .style('bottom', '0px')
                                        .html('ROI: ' + (d1.source.index + 1) + '  <->  ' + (d1.target.index + 1) + '<br>' + 'value: ' + d1.thick.toFixed(5) + '<br>' + 'allDatas: ' + d1.data);//tooltip.html(text);
                            })
                            .on("mouseout", function(d1, i) {
                                //return tooltip.style('visibility', 'hidden');
                                return tipsDiv.style('visibility', 'hidden');
                            });
                            ellipse.append("line")
                                .attr("class", function(d1,i) {
                                    return ".rowlink" + numindex;
                                })
                               .style("stroke-width", function(d1) {
                                    if (that.reverse){
                                        if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                            if (d1.thick == 0) return 0;
                                            if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray)) {
                                                return 1;
                                            }
                                            return 0;
                                        }
                                        else {
                                            return 0;
                                        }
                                    }
                                    else return 4 - (d1.thick - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 4 
                                })  //路径宽度
                                .style("stroke", function(d1) {
                                    return that.lineColor(d1.allSigns[numindex] >= 0);
                                    if (d1.allSigns[numindex] >= 0) {
                                        return "#775AA9";
                                    }
                                    else {
                                        return "#FE8F00";
                                    }
                                })
                                .style("stroke-opacity", function(d1){ //不透明度
                                    return 1;
                                })
                                .attr("x1", function(d1){
                                    var rx = 0;
                                    var ry = 0;
                                    if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                        if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                            rx = that.widthScale(d1.data[numindex]);
                                            //rx = Math.abs(d1.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                    }
                                    if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                        if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                            ry = that.widthScale(d1.data[(numindex+1)%dataNum]);
                                            //ry = Math.abs(d1.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                    }
                                    return d1.cx[numindex] + rx;
                                })
                                .attr("y1", function(d1){
                                    var rx = 0;
                                    var ry = 0;
                                    if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                        if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                            rx = that.widthScale(d1.data[numindex]);
                                            //rx = Math.abs(d1.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                    }
                                    if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                        if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                            ry = that.widthScale(d1.data[(numindex+1)%dataNum]);
                                            //ry = Math.abs(d1.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                    }
                                    return d1.cy[numindex];
                                })
                                .attr("x2", function(d1){
                                    var rx = 0;
                                    var ry = 0;
                                    if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                        if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                            rx = that.widthScale(d1.data[numindex]);
                                            //rx = Math.abs(d1.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                    }
                                    if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                        if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                            ry = that.widthScale(d1.data[(numindex+1)%dataNum]);
                                            //ry = Math.abs(d1.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                    }
                                    return d1.cx[numindex] - rx;
                                })
                                .attr("y2", function(d1){
                                    var rx = 0;
                                    var ry = 0;
                                    if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                        if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                            rx = that.widthScale(d1.data[numindex]);
                                            //rx = Math.abs(d1.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                    }
                                    if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                        if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                            ry = that.widthScale(d1.data[(numindex+1)%dataNum]);
                                            //ry = Math.abs(d1.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                    }
                                    return d1.cy[numindex];
                                })
                                .style("transform-origin", function(d1) {
                                    return (d1.cx[numindex] / width * 100).toString() + "% "  +( d1.cy[numindex] / height * 100).toString() +"%";
                                })
                                .style("transform", function(d1) {
                                    var deg = 0;
                                    var source = [xScale(d1.source.x), yScale(d1.source.y)];
                                    var target = [xScale(d1.target.x), yScale(d1.target.y)];
                                    var x_distance = target[0] - source[0];
                                    var y_distance = target[1] - source[1];
                                    var c_distance = Math.pow(x_distance, 2) + Math.pow(y_distance, 2);
                                    var distance = Math.sqrt(c_distance);
                                    if (x_distance == 0) {
                                        deg = 90;
                                    }
                                    else if (y_distance == 0) {
                                       deg = 0;
                                    } 
                                    else {
                                        var xielv = y_distance / x_distance;
                                        var sinA = Math.abs(y_distance) / distance;
                                        if (xielv < 0) {
                                            deg = 90 - Math.asin(sinA) * 180 / Math.PI;
                                        }
                                        else {
                                            deg = Math.asin(sinA) * 180 / Math.PI - 90;
                                            //console.log("-deg", deg);
                                        }                
                                    }
                                    return "rotate("+ deg.toString() + "deg)";
                                });
                    }
                    polygonup.selectAll(".ellipseup").remove();
                    for(var numindex = 0; numindex < dataNum; numindex++) {
                        var ellipse = polygonup.append("g")
                            .attr("class", function(d1,i){
                                return "ellipseup" + numindex + " ellipseup";
                            });
                        ellipse.append("ellipse")
                            .attr("rx", function(d1){
                                //console.log('d.data', d.data[numindex]);
                                if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                    if (d1.thick == 0) return 0;
                                    if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray)){
                                        return that.widthScale(d1.data[numindex]);
                                        //return Math.abs(d1.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                    }
                                    return 0;
                                }
                                else {
                                    return 0;
                                }            
                            })
                            .attr("ry", function(d1) {
                                if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                    if (d1.thick == 0) return 0;
                                    if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray)){
                                        return that.widthScale(d1.data[(numindex+1)%dataNum]);
                                        //return Math.abs(d1.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                    }
                                    return 0;
                                }
                                else {
                                    return 0;
                                }
                            })
                            .attr("cx", function(d1) {            
                                return d1.cx[numindex];
                            })
                            .attr("cy", function(d1){
                                return d1.cy[numindex];
                            })
                            .style("fill", function(d1) {
                                return that.fillColor(d1.allSigns[numindex] >= 0);
                                return "white";
                                if (d1.allSigns[numindex] >= 0) {
                                    return "#775AA9";
                                }
                                else {
                                    return "#FE8F00";
                                }
                            })
                            .style("stroke-width", function(d1) {
                                return 1;
                            }) //路径宽度
                            .style("stroke", function(d1) {
                                return that.strokeColor(d1.allSigns[numindex] >= 0);
                                if (d1.allSigns[numindex] >= 0) {
                                    return "#775AA9";
                                }
                                else {
                                    return "#FE8F00";
                                }
                            })
                            .style("transform-origin", function(d1) {
                                return (d1.cx[numindex] / width * 100).toString() + "% "  +( d1.cy[numindex] / height * 100).toString() +"%";
                            })
                            .style("transform", function(d1) {
                                var deg = 0;
                                var source = [xScale(d1.source.x), yScale(d1.source.y)];
                                var target = [xScale(d1.target.x), yScale(d1.target.y)];
                                var x_distance = target[0] - source[0];
                                var y_distance = target[1] - source[1];
                                var c_distance = Math.pow(x_distance, 2) + Math.pow(y_distance, 2);
                                var distance = Math.sqrt(c_distance);
                                if (x_distance == 0) {
                                    deg = 90;
                                }
                                else if (y_distance == 0) {
                                    deg = 0;
                                } 
                                else {
                                    var xielv = y_distance / x_distance;
                                    var sinA = Math.abs(y_distance) / distance;
                                    if (xielv < 0) {
                                        deg = 90 - Math.asin(sinA) * 180 / Math.PI;
                                    }
                                    else {
                                        deg = Math.asin(sinA) * 180 / Math.PI - 90;
                                        //console.log("-deg", deg);
                                    }
                                }
                                return "rotate("+ deg.toString() + "deg)";
                            })
                            //添加hover
                            .on("mouseenter", function(d1, i) {
                                //console.log('mouseenter', d);
                                return tipsDiv
                                    .style('visibility', 'visible')
                                    //.style('left', (d3.event.clientX - 920) + 'px') //- 400
                                    //.style('top', (d3.event.clientY - 300) + 'px') //- 110
                                    .style('right', '14px')
                                    //.style('top', '224px')
                                    .style('bottom', '0px')
                                    .html('ROI: ' + (d1.source.index + 1) + '  <->  ' + (d1.target.index + 1) + '<br>' + 'value: ' + d1.thick.toFixed(5) + '<br>' + 'allDatas: ' + d1.data);//tooltip.html(text);
                            })
                            .on("mouseout", function(d1, i) {
                                //return tooltip.style('visibility', 'hidden');
                                return tipsDiv.style('visibility', 'hidden');
                            })
                            .call(that.drag);;
                        ellipse.append("line")
                            .attr("class", function(d1,i) {
                                return ".rowlink" + numindex;
                            })
                            .style("stroke-width", function(d1) {
                                if (that.reverse){
                                    if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                        if (d1.thick == 0) return 0;
                                        if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray)) {
                                            return 1;
                                        }
                                        return 0;
                                    }
                                    else {
                                        return 0;
                                    }
                                }
                                else return 4 - (d1.thick - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 4 
                            })  //路径宽度
                            .style("stroke", function(d1) {
                                return that.lineColor(d1.allSigns[numindex] >= 0);
                                if (d1.allSigns[numindex] >= 0) {
                                    return "#775AA9";
                                }
                                else {
                                    return "#FE8F00";
                                }
                            })
                            .style("stroke-opacity", function(d1){ //不透明度
                                return 1;
                            })
                            .attr("x1", function(d1){
                                var rx = 0;
                                var ry = 0;
                                if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                    if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                        rx = that.widthScale(d1.data[numindex]);
                                        //rx = Math.abs(d1.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                    if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                        ry = that.widthScale(d1.data[(numindex+1)%dataNum]);
                                        //ry = Math.abs(d1.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                return d1.cx[numindex] + rx;
                            })
                            .attr("y1", function(d1){
                                var rx = 0;
                                var ry = 0;
                                if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                    if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                        rx = that.widthScale(d1.data[numindex]);
                                        //rx = Math.abs(d1.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                    if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                        ry = that.widthScale(d1.data[(numindex+1)%dataNum]);
                                        //ry = Math.abs(d1.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                return d1.cy[numindex];
                            })
                            .attr("x2", function(d1){
                                var rx = 0;
                                var ry = 0;
                                if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                    if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                        rx = that.widthScale(d1.data[numindex]);
                                        //rx = Math.abs(d1.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                    if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                        ry = that.widthScale(d1.data[(numindex+1)%dataNum]);
                                        //ry = Math.abs(d1.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth /100 + that.ellipseWidthMin/100;
                                }
                                return d1.cx[numindex] - rx;
                            })
                            .attr("y2", function(d1){
                                var rx = 0;
                                var ry = 0;
                                if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                    if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                        rx = that.widthScale(d1.data[numindex]);
                                        //rx = Math.abs(d1.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                if (d1.thick >= that.fillMin && d1.thick <= that.fillMax){
                                    if ((d1.sign == 1 && that.showPurple) || (d1.sign == -1 && that.showOrange) || (d1.sign == 0 && that.showGray))
                                        ry = that.widthScale(d1.data[(numindex+1)%dataNum]);
                                        //ry = Math.abs(d1.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                return d1.cy[numindex];
                            })
                            .style("transform-origin", function(d1) {
                                return (d1.cx[numindex] / width * 100).toString() + "% "  +( d1.cy[numindex] / height * 100).toString() +"%";
                            })
                            .style("transform", function(d1) {
                                var deg = 0;
                                var source = [xScale(d1.source.x), yScale(d1.source.y)];
                                var target = [xScale(d1.target.x), yScale(d1.target.y)];
                                var x_distance = target[0] - source[0];
                                var y_distance = target[1] - source[1];
                                var c_distance = Math.pow(x_distance, 2) + Math.pow(y_distance, 2);
                                var distance = Math.sqrt(c_distance);
                                if (x_distance == 0) {
                                    deg = 90;
                                }
                                else if (y_distance == 0) {
                                   deg = 0;
                                } 
                                else {
                                    var xielv = y_distance / x_distance;
                                    var sinA = Math.abs(y_distance) / distance;
                                    if (xielv < 0) {
                                        deg = 90 - Math.asin(sinA) * 180 / Math.PI;
                                    }
                                    else {
                                        deg = Math.asin(sinA) * 180 / Math.PI - 90;
                                        //console.log("-deg", deg);
                                    }                
                                }
                                return "rotate("+ deg.toString() + "deg)";
                            });
                        
                    }
                    //调整底层显示的虚线
                        svgLeft.selectAll(".dottedlink").style("stroke-opacity",  function(d1){
                            if(d1.full == true && d1.thick > 0 && d1.thick >= that.fillMin) return 1;
                            return 0;
                        })
                        .style("stroke-width", function(d1){
                            if(d1.full == true && d1.thick > 0 && d1.thick >= that.fillMin) return 1;
                            return 0;
                        });
                        //选择的虚线显示
                        svgLeft.selectAll(".dottedlinkup").filter(function(d1) {
                            return that.selectedNodes[d1.source.index] == 1 || that.selectedNodes[d1.target.index] == 1;
                        }).style("stroke-opacity",  function(d1){
                            if(d1.full == true && d1.thick > 0 && d1.thick >= that.fillMin) return 1;
                            return 0;
                        })
                        .style("stroke-width", function(d1){
                            if(d1.full == true && d1.thick > 0 && d1.thick >= that.fillMin) return 1;
                            return 0;
                        });
                        //选择不到的虚线不显示
                        svgLeft.selectAll(".dottedlinkup").filter(function(d1) {
                            return !(that.selectedNodes[d1.source.index] == 1 || that.selectedNodes[d1.target.index] == 1);
                        }).style("stroke-opacity",  0).style("stroke-width", 0);
                    
                    /*
                    var polygon = svgLeft.selectAll(".polygon");
                    for(var numindex = 0; numindex < dataNum; numindex++) {
                        var classname = ".ellipse" + numindex;
                        polygon.select(classname).select("ellipse")
                        .attr("rx", function(d){
                            //console.log('d.data', d.data[numindex]);
                            if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                if (d.thick == 0) return 0;
                                if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                    return that.widthScale(d.data[numindex]);
                                    //return Math.abs(d.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin / 100;
                                return 0;
                            }
                            else {
                                return 0;
                            }
                        })
                        .attr("ry", function(d) {
                            if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                if (d.thick == 0) return 0;
                                if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                    return that.widthScale(d.data[(numindex + 1) % dataNum]);
                                    //return Math.abs(d.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 +  that.ellipseWidthMin / 100;
                                return 0;
                            }
                            else {
                                return 0;
                            }
                        });
                        polygon.select(classname).select("line")
                            .attr("x1", function(d){
                                var rx = 0;
                                var ry = 0;
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        rx = that.widthScale(d.data[numindex]);
                                        //rx = Math.abs(d.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin /100;
                                }
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        ry = that.widthScale(d.data[(numindex + 1) % dataNum]);
                                        //ry = Math.abs(d.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                return d.cx[numindex] + rx;
                            })
                            .attr("x2", function(d){
                                var rx = 0;
                                var ry = 0;
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        rx = that.widthScale(d.data[numindex]);
                                        //rx = Math.abs(d.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        ry = that.widthScale(d.data[(numindex + 1) % dataNum]);
                                        //ry = Math.abs(d.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                return d.cx[numindex] - rx;
                            });

                    }
                    var polygonup = svgLeft.selectAll(".polygonup");
                    for(var numindex = 0; numindex < dataNum; numindex++) {
                        var classname = ".ellipseup" + numindex;
                        polygonup.select(classname).select("ellipse")
                        .attr("rx", function(d){
                            //console.log('d.data', d.data[numindex]);
                            if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                if (d.thick == 0) return 0;
                                if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                    return that.widthScale(d.data[numindex]);
                                    //return Math.abs(d.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                return 0;
                            }
                            else {
                                return 0;
                            }
                        })
                        .attr("ry", function(d) {
                            if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                if (d.thick == 0) return 0;
                                if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                    return that.widthScale(d.data[(numindex + 1) % dataNum]);
                                    //return Math.abs(d.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 +  that.ellipseWidthMin/100;
                                return 0;
                            }
                            else {
                                return 0;
                            }
                        });

                        polygonup.select(classname).select("line")
                            .attr("x1", function(d){
                                var rx = 0;
                                var ry = 0;
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        rx = that.widthScale(d.data[numindex]);
                                        //rx = Math.abs(d.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        ry = that.widthScale(d.data[(numindex + 1) % dataNum]);
                                        //ry = Math.abs(d.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                return d.cx[numindex] + rx;
                            })
                            .attr("x2", function(d){
                                var rx = 0;
                                var ry = 0;
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        rx = that.widthScale(d.data[numindex]);
                                        //rx = Math.abs(d.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        ry = that.widthScale(d.data[(numindex + 1) % dataNum]);
                                        //ry = Math.abs(d.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                return d.cx[numindex] - rx;
                            });
                    */
                    that.loading = false;
                    }, 1);
                }
            },
            ellipseWidthMin: {
                deep: true,
                handler: function(newVal, oldVal) {
                    //console.log("filterRate", newVal);
                    //console.log(d3.select(this.$el));
                    var s = ".draw-area-" + this.index;
                    var svgLeft = d3.select(this.$el)      
                        .select(s).select(".leftSVG");
                    var that = this;
                    that.fillMin = this.filterRate[0] * (that.thick_range.r2 - that.thick_range.r1) / 100 + that.thick_range.r1;
                    that.fillMax = this.filterRate[1] * (that.thick_range.r2 - that.thick_range.r1) / 100 + that.thick_range.r1 + 100;
                    var dataNum = this.dataLength;
                    var polygon = svgLeft.selectAll(".polygon");
                    for(var numindex = 0; numindex < dataNum; numindex++) {
                        var classname = ".ellipse" + numindex;
                        polygon.select(classname).select("ellipse")
                        .attr("rx", function(d){
                            //console.log('d.data', d.data[numindex]);
                            if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                if (d.thick == 0) return 0;
                                if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                    return that.widthScale(d.data[numindex]);
                                    //return Math.abs(d.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                return 0;
                            }
                            else {
                                return 0;
                            }
                        })
                        .attr("ry", function(d) {
                            if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                if (d.thick == 0) return 0;
                                if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                    return that.widthScale(d.data[(numindex + 1) % dataNum]);
                                    //return Math.abs(d.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 +  that.ellipseWidthMin/100;
                                return 0;
                            }
                            else {
                                return 0;
                            }
                        });
                        polygon.select(classname).select("line")
                            .attr("x1", function(d){
                                var rx = 0;
                                var ry = 0;
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        rx = that.widthScale(d.data[numindex]);
                                        //rx = Math.abs(d.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        ry = that.widthScale(d.data[(numindex + 1) % dataNum]);
                                        //ry = Math.abs(d.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                return d.cx[numindex] + rx;
                            })
                            .attr("x2", function(d){
                                var rx = 0;
                                var ry = 0;
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        rx = that.widthScale(d.data[numindex]);
                                        //rx = Math.abs(d.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        ry = that.widthScale(d.data[(numindex + 1) % dataNum]);
                                        //ry = Math.abs(d.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                return d.cx[numindex] - rx;
                            });

                    }
                    var polygonup = svgLeft.selectAll(".polygonup");
                    for(var numindex = 0; numindex < dataNum; numindex++) {
                        var classname = ".ellipseup" + numindex;
                        polygonup.select(classname).select("ellipse")
                        .attr("rx", function(d){
                            //console.log('d.data', d.data[numindex]);
                            if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                if (d.thick == 0) return 0;
                                if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                    return that.widthScale(d.data[numindex]);
                                    //return Math.abs(d.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                return 0;
                            }
                            else {
                                return 0;
                            }
                        })
                        .attr("ry", function(d) {
                            if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                if (d.thick == 0) return 0;
                                if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                    return that.widthScale(d.data[(numindex + 1) % dataNum]);
                                    //return Math.abs(d.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 +  that.ellipseWidthMin/100;
                                return 0;
                            }
                            else {
                                return 0;
                            }
                        });

                        polygonup.select(classname).select("line")
                            .attr("x1", function(d){
                                var rx = 0;
                                var ry = 0;
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        rx = that.widthScale(d.data[numindex]);
                                        //rx = Math.abs(d.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        ry = that.widthScale(d.data[(numindex + 1) % dataNum]);
                                        //ry = Math.abs(d.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                return d.cx[numindex] + rx;
                            })
                            .attr("x2", function(d){
                                var rx = 0;
                                var ry = 0;
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        rx = that.widthScale(d.data[numindex]);
                                        //rx = Math.abs(d.data[numindex]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        ry = that.widthScale(d.data[(numindex + 1) % dataNum]);
                                        //ry = Math.abs(d.data[(numindex + 1) % dataNum]) * (that.ellipseMax - that.ellipseMin) * that.ellipseWidth / 100 + that.ellipseWidthMin/100;
                                }
                                return d.cx[numindex] - rx;
                            });
                    }
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
                    that.fillMin = that.filterRate[0] * (that.thick_range.r2 - that.thick_range.r1) / 100 + that.thick_range.r1;
                    that.fillMax = that.filterRate[1] * (that.thick_range.r2 - that.thick_range.r1) / 100 + that.thick_range.r1 + 100;
                    svgLeft.selectAll(".link")
                        .style("stroke-width", function(d) {
                            if (that.reverse){
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if (d.thick == 0) return 0;
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        return (d.thick - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 3 + 1;
                                    return 0;
                                }
                                else {
                                    return 0;
                                }
                            }
                            else return 4 - (d.thick - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 4
                        })
                        .style("stroke", function(d){
                            if(d.thick >= that.fillMin && d.thick <= that.fillMax){
                                if(d.sign == 1){
                                    return that.fillScaleOne(d.thick - that.fillMin, that.fillMax - that.fillMin);
                                }
                                else if(d.sign == -1) {
                                    return that.fillScaleTwo(d.thick - that.fillMin, that.fillMax - that.fillMin);
                                }
                                else {
                                    return that.fillScaleGray(d.thick - that.fillMin, that.fillMax - that.fillMin);
                                }
                            } else {
                                return d3.rgb(255, 255, 255).toString();
                            }
                        })
                        .style("stroke-opacity", function(d){
                            return 1;
                        });

                    svgLeft.selectAll(".link2")
                        .style("stroke-width", function(d) {
                            if (that.reverse){
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if (d.thick == 0) return 0;
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        return 4;
                                    return 0;
                                }
                                else {
                                    return 0;
                                }
                            }
                            else return 4 - (d.thick - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 4
                        })
                        .style("stroke", function(d){
                            return d3.rgb(0, 0, 255).toString();
                        })
                        .style("stroke-opacity", function(d){
                            return 0;
                        });
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
                    that.fillMin = that.filterRate[0] * (that.thick_range.r2 - that.thick_range.r1) / 100 + that.thick_range.r1;
                    that.fillMax = that.filterRate[1] * (that.thick_range.r2 - that.thick_range.r1) / 100 + that.thick_range.r1 + 100;
                    svgLeft.selectAll(".link")
                        .style("stroke-width", function(d) {
                            if (that.reverse){
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if (d.thick == 0) return 0;
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        return (d.thick - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 3 + 1;
                                    return 0;
                                }
                                else {
                                    return 0;
                                }
                            }
                            else return 4 - (d.thick - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 4
                        })
                        .style("stroke", function(d){
                            if(d.thick >= that.fillMin && d.thick <= that.fillMax){
                                if(d.sign == 1){
                                    return that.fillScaleOne(d.thick - that.fillMin, that.fillMax - that.fillMin);
                                }
                                else if(d.sign == -1) {
                                    return that.fillScaleTwo(d.thick - that.fillMin, that.fillMax - that.fillMin);
                                }
                                else {
                                    return that.fillScaleGray(d.thick - that.fillMin, that.fillMax - that.fillMin);
                                }
                            } else {
                                return d3.rgb(255, 255, 255).toString();
                            }
                        })
                        .style("stroke-opacity", function(d){
                            return 1;
                        });

                    svgLeft.selectAll(".link2")
                        .style("stroke-width", function(d) {
                            if (that.reverse){
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if (d.thick == 0) return 0;
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        return 4;
                                    return 0;
                                }
                                else {
                                    return 0;
                                }
                            }
                            else return 4 - (d.thick - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 4
                        })
                        .style("stroke", function(d){
                            return d3.rgb(0, 0, 255).toString();
                        })
                        .style("stroke-opacity", function(d){
                            return 0;
                        });
                }
            },
            showGray: {
                deep: true,
                handler: function(newVal, oldVal) {
                    //console.log("filterRate", newVal);
                    //console.log(d3.select(this.$el));
                    var s = ".draw-area-" + this.index;
                    var svgLeft = d3.select(this.$el)      
                        .select(s).select(".leftSVG");
                    var that = this;
                    that.fillMin = that.filterRate[0] * (that.thick_range.r2 - that.thick_range.r1) / 100 + that.thick_range.r1;
                    that.fillMax = that.filterRate[1] * (that.thick_range.r2 - that.thick_range.r1) / 100 + that.thick_range.r1 + 100;
                    svgLeft.selectAll(".link")
                        .style("stroke-width", function(d) {
                            if (that.reverse){
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if (d.thick == 0) return 0;
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        return (d.thick - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 3 + 1;
                                    return 0;
                                }
                                else {
                                    return 0;
                                }
                            }
                            else return 4 - (d.thick - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 4
                        })
                        .style("stroke", function(d){
                            if(d.thick >= that.fillMin && d.thick <= that.fillMax){
                                if(d.sign == 1){
                                    return that.fillScaleOne(d.thick - that.fillMin, that.fillMax - that.fillMin);
                                }
                                else if(d.sign == -1) {
                                    return that.fillScaleTwo(d.thick - that.fillMin, that.fillMax - that.fillMin);
                                }
                                else {
                                    return that.fillScaleGray(d.thick - that.fillMin, that.fillMax - that.fillMin);
                                }
                            } else {
                                return d3.rgb(255, 255, 255).toString();
                            }
                        })
                        .style("stroke-opacity", function(d){
                            return 1;
                        });

                    svgLeft.selectAll(".link2")
                        .style("stroke-width", function(d) {
                            if (that.reverse){
                                if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                    if (d.thick == 0) return 0;
                                    if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray))
                                        return 4;
                                    return 0;
                                }
                                else {
                                    return 0;
                                }
                            }
                            else return 4 - (d.thick - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 4
                        })
                        .style("stroke", function(d){
                            return d3.rgb(0, 0, 255).toString();
                        })
                        .style("stroke-opacity", function(d){
                            return 0;
                        });
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
                                return (d.thick - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 3 + 1;
                            else return 4 - (d.thick - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 4; 
                        });
                }
            }
        }
    }
</script>