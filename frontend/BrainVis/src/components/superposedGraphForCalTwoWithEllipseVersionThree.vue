<!--叠加图，strength从外部绑定-->
<template>
    <div style="text-align: left;">
        <div :class='setClass()' style="height:50%; position: relative;"> 
            <!--div class="loading" style="margin-top: 6px;" v-if="getLoading"></div-->
            <loading v-bind="{'loading': loading}"></loading>
            <div style="text-align: center; padding-top: 11px; margin-right: 24px; font-size: 16px; min-height: 46px;">{{titleName}}</div>
            <div class="leftSVG">
                <div class="tipsDiv" style="z-index: 10; color: black; font-size: 12px;text-align: right;position: absolute;visibility: hidden;"></div>
            </div>
            <!--div style="position: absolute; top: 53px; right: 28px; padding-left: 0px; z-index: 999;">
                <div>
                    <input class="colorcheckbox" type="checkbox" @click="changeFish()" />
                    <span :class="{'colorSpan':true, 'greyCircle':!asFish, 'hasOk': true, 'blueCircle':asFish}">
                        <span class="f-full"></span>
                    </span>
                    <span style="font-size: 10px; color: #8998AC; padding-left: 0px; vertical-align: text-bottom;">Fisheye</span>
                </div>
            </div-->
            <div class="panel" ref="myPanel" style="overflow: visible; margin-top: 50px; margin-left: 107px;">
                <div>
                    <div class="pearl" style="">Pearl<br>Size: </div>
                    <slider :min=0 :max=100 v-model="ellipseWidth" :dataMin="ellipseMin" :dataMax="ellipseMax" :style="{'width': '200px', 'float':'left'}"></slider>
                    <!--slider :min=0 :max=100 v-model="thetaMin" :dataMin="0" :dataMax="100" :style="{'width': '150px', 'margin-top':'20px'}"></slider>
                    <slider :min=0 :max=100 v-model="thetaoffset" :dataMin="0" :dataMax="100" :style="{'width': '150px', 'margin-top':'20px'}"></slider-->
                </div>
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
    import * as fe from "./fisheye.js"
    import NetService from '@/services/net-service'

    //import $ from "jquery"
    export default {
        name: 'superposedGraphForCalTwoWithEllipseVersionThree',
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
            }
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
                ellipseWidth: 5,
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
                asFish: false,
                pointPos: [],
                fullArr: [],
                thetaMin: 19,
                thetaoffset: 25,
                g1name: "",
                g2name: "",
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
            BezierFunction(p0, p1, p2, t) { //起始点，控制点，结束点
                var x = (1-t) * (1-t) * p0[0] + 2 * t * (1 - t) * p1[0] + t * t * p2[0];
                var y = (1-t) * (1-t) * p0[1] + 2 * t * (1 - t) * p1[1] + t * t * p2[1];
                return [x,y];
            },
            //椭圆边框颜色
            unfullstrokeColor(value){
                if (value) {
                    return "#775AA9";
                }
                else {
                    return "#FE8F00";
                }
            },
            strokeColor(value){
                if (value) {
                    return "#775AA9";
                }
                else {
                    return "#FE8F00";
                }
            },
            
            //椭圆填充颜色
            unfullfillColor(value) {
                return "white";
            },
            fillColor(value) {
                //return "white";
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
            changeFish() {
                this.asFish = !this.asFish;
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
                        if(i != 0) groupString += " vs. ";
                        groupString = groupString + groupArray[i] + " Group";
                    }
                    this.g1name = groupArray[0];
                    this.g2name = groupArray[1];
                    //groupString += " Group";
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
                var toptitle1 = "";
                var toptitle2 = "";
                var toptitle3 = "";
                var pearlName = [];
                //假设特征相同
                if(sameFeature) {
                    //toptitle = this.collection.allFunc[0] + " " + this.collection.allFeature[0];
                    toptitle1 = this.collection.allFeature[0];
                    toptitle2 = "t-test" ;
                    toptitle3 = this.collection.allFunc[0];
                    //珠子的名字为不同的组
                    for(var i in this.collection.allGroups) {
                        var obj = {};
                        obj.name = this.collection.allGroups[i][0] + " vs. " + this.collection.allGroups[i][1];
                        obj.index = Number(i);
                        pearlName.push(obj);
                    }

                }
                else {
                    toptitle1 = "";
                    toptitle2 = "t-test";
                    toptitle3 = this.collection.allFunc[0];// + " " + this.titleName;
                    //珠子的名字为不同的特征
                    var perlen = this.collection.allFeature.length;
                    for(var i in this.collection.allFeature) {
                        var obj = {};
                        obj.name = this.collection.allFeature[i];// + "," + this.collection.allFeature[(Number(i) + 1) % perlen];
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
                    d.selected = 0;
                    d.linkselected = 0;
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
                        d.origincx = [];
                        d.origincy = [];
                        d.allSigns = [];
                        d.full = [];
                        for (var a = 0; a < dataNum; a++) {
                            d.data[a] = 0;
                            d.allSigns[a] = 0;
                            //d.cx[a] = 0;
                            //d.origincx[a] = 0;
                            //d.cy[a] = 0;
                            //d.origincy[a] = 0;
                            //d.full[a] = 0;
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
                        d.origincx = [];
                        d.origincy = [];
                        d.allSigns = [];
                        d.full = [];
                        for (var a = 0; a < dataNum; a++) {
                            d.data.push(allDatas[a][i]);
                            d.allSigns.push(allSigns[a][i]);
                            //d.cx.push(10);
                            //d.cy.push(10);
                            //d.origincx.push(10);
                            //d.origincy.push(10);
                            //d.full.push(i % 2);
                        }
                    }
                    //in指示weight的index
                    d.in = i;
                    tmp.in = i;
                    d.controlPoint = [0, 0];
                    d.inverse = 0;
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
                
                console.log("width / x_range.x2", width, x_range.x2, width / x_range.x2);
                that.widthScale = d3.scale.linear()
                    .domain([0, 1])
                    .range([that.ellipseWidthMin / 2 * width / x_range.x2, that.ellipseWidth / 2 * width / x_range.x2]);
                
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
                        //var classname = d3.select(this).attr("class");
                        //var numindex = Number(classname.substr(9, classname.indexOf(" ") - 9));
                        //startx = d.cx[numindex];
                        //starty = d.cy[numindex];
                        //console.log('dragstart', d3.event.sourceEvent.clientX, d3.event.sourceEvent.clientY, d.cx[numindex], d.cy[numindex]);
                    })
                    .on('dragend', function(d){
                        //console.log('dragend', d3.event, d3.event);
                        //console.log("dragend", d);
                        var classname = d3.select(this).attr("class");
                        var numindex = Number(classname.substr(9, classname.indexOf(" ") - 9));
                        console.log("classname", numindex);
                        //startx = d.cx[numindex];
                        //starty = d.cy[numindex];
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

                        var result;
                        if(d.source.index == nodeNum) {
                            result = that.caldxdy(dis, xScale(d.source.x), yScale(d.source.y) , xScale(d.target.x), yScale(d.target.y));
                        }
                        else {
                            result = that.caldxdy(dis, xScale(d.target.x), yScale(d.target.y), xScale(d.source.x), yScale(d.source.y));
                        }
                        var minnodex = Math.min(xScale(d.source.x), xScale(d.target.x));
                        var maxnodex = Math.max(xScale(d.source.x), xScale(d.target.x));
                        var minnodey = Math.min(yScale(d.source.y), yScale(d.target.y));
                        var maxnodey = Math.max(yScale(d.source.y), yScale(d.target.y));
                        if(d.cx[numindex] + result[0] < minnodex) {
                            result[0] = minnodex - d.cx[numindex];
                        }
                        else if(d.cx[dataNum - 1] + result[0] > maxnodex) {
                            result[0] = maxnodex - d.cx[dataNum - 1];
                        }
                        if(d.cy[numindex] + result[1] < minnodey) {
                            result[1] = minnodey - d.cy[numindex];
                        }
                        else if(d.cy[dataNum - 1] + result[1] > maxnodey) {
                            result[1] = maxnodey - d.cy[dataNum - 1];
                        }

                        svgLeft.selectAll(".polygon").filter(function(d1) {
                            return d1.in == d.in;
                            return (d1.source.index == nodeNum && connection[d1.source.index] >= connection[d1.target.index]) || (d1.target.index == nodeNum && connection[d1.source.index] < connection[d1.target.index]);
                        }).selectAll("g")
                        .each(function(d1, i){
                            if(i >= numindex) {
                                d1.cx[i] = d1.cx[i] + result[0];
                                d1.cy[i] = d1.cy[i] + result[1];
                                d1.origincx[i] = d1.cx[i];
                                d1.origincy[i] = d1.cy[i];
                            }
                            
                        });
                    })
                    .on('drag', function(d){
                        var classname = d3.select(this).attr("class");
                        var numindex = Number(classname.substr(9, classname.indexOf(" ") - 9));
                        console.log("classname", numindex);
                        //startx = d.cx[numindex];
                        //starty = d.cy[numindex];
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

                        var result;
                        if(d.source.index == nodeNum) {
                            result = that.caldxdy(dis, xScale(d.source.x), yScale(d.source.y) , xScale(d.target.x), yScale(d.target.y));
                        }
                        else {
                            result = that.caldxdy(dis, xScale(d.target.x), yScale(d.target.y), xScale(d.source.x), yScale(d.source.y));
                        }
                        var minnodex = Math.min(xScale(d.source.x), xScale(d.target.x));
                        var maxnodex = Math.max(xScale(d.source.x), xScale(d.target.x));
                        var minnodey = Math.min(yScale(d.source.y), yScale(d.target.y));
                        var maxnodey = Math.max(yScale(d.source.y), yScale(d.target.y));
                        if(d.cx[numindex] + result[0] < minnodex) {
                            result[0] = minnodex - d.cx[numindex];
                        }
                        else if(d.cx[dataNum - 1] + result[0] > maxnodex) {
                            result[0] = maxnodex - d.cx[dataNum - 1];
                        }
                        if(d.cy[numindex] + result[1] < minnodey) {
                            result[1] = minnodey - d.cy[numindex];
                        }
                        else if(d.cy[dataNum - 1] + result[1] > maxnodey) {
                            result[1] = maxnodey - d.cy[dataNum - 1];
                        }

                        svgLeft.selectAll(".polygon").filter(function(d1) {
                            return d1.in == d.in;
                            return (d1.source.index == nodeNum && connection[d1.source.index] >= connection[d1.target.index]) || (d1.target.index == nodeNum && connection[d1.source.index] < connection[d1.target.index]);
                        }).selectAll("g")
                        .each(function(d1, i){
                            if(i >= numindex) {
                                var cx = d1.cx[i] + result[0];
                                var cy = d1.cy[i] + result[1];
                                d3.select(this).select("circle")
                                    .attr("cx", function(d2) { 
                                        return cx;
                                    })
                                    .attr("cy", function(d2) { 
                                        return cy;
                                    });
                            }
                            
                        });
                        /*
                        .style("transform", function(d1, i){
                            if(i >= numindex) {
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
                            }
                        });
                        */
                        svgLeft.selectAll(".polygonup").filter(function(d1) {
                            return d1.in == d.in;
                            return (d1.source.index == nodeNum && connection[d1.source.index] >= connection[d1.target.index]) || (d1.target.index == nodeNum && connection[d1.source.index] < connection[d1.target.index]);
                        }).selectAll("g")
                        .each(function(d1, i){
                           if(i >= numindex) {
                                var cx = d1.cx[i] + result[0];
                                var cy = d1.cy[i] + result[1];
                                d3.select(this).select("circle")
                                    .attr("cx", function(d2) { 
                                        return cx;
                                    })
                                    .attr("cy", function(d2) { 
                                        return cy;
                                    });
                            }
                            
                        });
                        /*
                        .style("transform", function(d1, i){
                            if(i >= numindex) {
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
                            }
                        });
                        */
                        //console.log('drag', d3.event.sourceEvent.offsetX, d3.event.sourceEvent.offsetY);
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
                    if (d.target == d.source) {
                        for (var a = 0; a < dataNum; a++) {
                            d.cx[a] = 0;
                            d.origincx[a] = 0;
                            d.cy[a] = 0;
                            d.origincy[a] = 0;
                            d.full[a] = 0;
                        }
                    } else {
                        for (var a = 0; a < dataNum; a++) {
                            d.cx.push(xScale(that.pointPos[i][a][0]));
                            d.cy.push(yScale(that.pointPos[i][a][1]));
                            d.origincx.push(xScale(that.pointPos[i][a][0]));
                            d.origincy.push(yScale(that.pointPos[i][a][1]));
                            d.full.push(that.fullArr[i]);
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

                for(var numindex = 0; numindex < dataNum; numindex++) {
                    var classname = ".ellipse" + numindex;
                    var ellipse = polygon.append("g")
                        .attr("class", function(d,i){
                            return "ellipse" + numindex + " ellipse";
                        });

                    ellipse.append("circle")
                        .attr("r", function(d){
                            //console.log('d.data', d.data[numindex]);
                            if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                if (d.thick == 0) return 0;
                                if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray)){
                                    if(d.full[numindex]) return that.widthScale(d.data[numindex]);
                                    return that.widthScale(0);
                                }
                                return 0;
                            }
                            else {
                                return 0;
                            }            
                        })
                        .attr("cx", function(d) {
                            //d.cx[numindex] = xScale((d.source.x + d.target.x) / 2);
                            //d.origincx[numindex] = xScale((d.source.x + d.target.x) / 2);
                            return (d.cx[numindex]);
                        })
                        .attr("cy", function(d){
                            //d.cy[numindex] = yScale((d.source.y + d.target.y) / 2);
                            //d.origincy[numindex] = yScale((d.source.y + d.target.y) / 2);
                            return (d.cy[numindex]);
                        })
                        .style("fill", function(d) {
                            if(d.full[numindex]) {
                                return that.fillColor(d.allSigns[numindex] >= 0);
                            }
                            return that.unfullfillColor(d.allSigns[numindex] >= 0);
                        })
                        .style("stroke-width", function(d) {
                            return 1;
                        }) //路径宽度
                        .style("stroke", function(d) {
                            if(d.full[numindex])
                                return that.strokeColor(d.allSigns[numindex] >= 0);
                            return that.unfullstrokeColor(d.allSigns[numindex] >= 0);
                        })
                        .style("transform-origin", function(d) {
                            return ((d.cx[numindex]) / width * 100).toString() + "% "  +( (d.cy[numindex]) / height * 100).toString() +"%";
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
                    }

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
                            }).style("opacity", 0).each(function(d1){
                                d1.selected = 0;
                                d1.linkselected = 0;
                            });
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
                            connection[i] = connection[i];
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
                        });
                        var polygonup = svgLeft.selectAll(".polygonup");
                        polygonup.filter(function(d1) {
                            return tmpselect[d1.target.index] != null ||  tmpselect[d1.source.index] != null;
                            //return d1.target.index == d.index || d1.source.index == d.index;
                        });
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
                                        }).style("opacity", 1).each(function(d2){
                                            d2.selected = 1;
                                        });
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
                        var unselected = svgLeft.selectAll(".polygonup").filter(function(d1) {
                            return !(that.selectedNodes[d1.source.index] == 1 || that.selectedNodes[d1.target.index] == 1);
                        }).style("opacity",  0)
                        /*
                            .style("transform", function(){
                                return "none";
                            });
                        */
                        .each(function(d1){
                            var source = that.deepCopy(d1.source);
                            var target = that.deepCopy(d1.target);
                            if(connection[target.index] > connection[source.index]) {
                                source = that.deepCopy(d1.target);
                                target = that.deepCopy(d1.source);
                            }
                            //假设第一圆距离该点的距离比最后一个圆远，旋转
                            var firstdis = Math.sqrt((d1.origincx[0] - xScale(source.x)) * (d1.origincx[0] - xScale(source.x)) + (d1.origincy[0] - yScale(source.y)) * (d1.origincy[0] - yScale(source.y)));
                            var lastdis = Math.sqrt((d1.origincx[dataNum - 1] - xScale(source.x)) * (d1.origincx[dataNum - 1] - xScale(source.x)) + (d1.origincy[dataNum - 1] - yScale(source.y)) * (d1.origincy[dataNum - 1] - yScale(source.y)));
                            if(firstdis > lastdis) {
                                d1.origincx.reverse();
                                d1.origincy.reverse();
                                for(var numindex = 0; numindex < dataNum; numindex++) {
                                    var ellipseName = ".ellipseup" + numindex;
                                    d3.select(this).select(ellipseName).select("circle")
                                        .attr("cx", function() { 
                                            d1.cx[numindex] = d1.origincx[numindex];
                                            return d1.cx[numindex]; 
                                        })
                                        .attr("cy", function() { 
                                            d1.cy[numindex] = d1.origincy[numindex];
                                            return d1.cy[numindex]; 
                                        });
                                }
                            }
                        });
                        //未选择的圆位置恢复，多边形透明度回复
                        for(var numindex = 0; numindex < dataNum; numindex++) {
                            var ellipseName = ".ellipseup" + numindex;
                            unselected.select(ellipseName).select("circle")
                                .attr("cx", function(d1) { 
                                    d1.cx[numindex] = d1.origincx[numindex];
                                    return d1.origincx[numindex]; 
                                })
                                .attr("cy", function(d1) { 
                                    d1.cy[numindex] = d1.origincy[numindex];
                                    return d1.origincy[numindex]; 
                                })
                                .attr("r", function(d2){
                                    //console.log('d.data', d.data[numindex]);
                                        if (d2.thick >= that.fillMin && d2.thick <= that.fillMax){
                                            if (d2.thick == 0) return 0;
                                            if ((d2.sign == 1 && that.showPurple) || (d2.sign == -1 && that.showOrange) || (d2.sign == 0 && that.showGray)){
                                                if(d2.full[numindex])   return that.widthScale(d2.data[numindex]);
                                                return that.widthScale(0);
                                            }
                                            return 0;
                                        }
                                        else {
                                            return 0;
                                        }
                                    })
                                    .style("fill", function(d2) {
                                        if(d2.full[numindex]) return that.fillColor(d2.allSigns[numindex] >= 0);
                                        return that.unfullfillColor(d2.allSigns[numindex] >= 0);
                                    })
                                    .style("stroke", function(d2) {
                                        if(d2.full[numindex]) return that.strokeColor(d2.allSigns[numindex] >= 0);
                                        return that.unfullstrokeColor(d2.allSigns[numindex] >= 0);
                                    });
                        }
                        unselected.select("polygon").style("opacity", 1);
                        unselected.select("path").style("stroke-width", 0);
                        
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
                    .style("transform-origin", function(d) {
                        return ((xScale(d.source.x) + xScale(d.target.x)) / 2 / width * 100).toString() + "% "  +( (yScale(d.source.y) + yScale(d.target.y)) / 2 / height * 100).toString() +"%";
                    })
                    .on("mouseenter", function(d, i) {
                        d3.select(this).select("path").style("stroke", "red");
                        d3.select(this).select("polygon").style("fill","red");
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
                        d3.select(this).select("path").style("stroke", "rgb(198, 198, 198)");
                        d3.select(this).select("polygon").style("fill","rgb(198, 198, 198)");
                        if(d.thick > 0) {
                            //该边未被选中
                            if(that.selectedNodes[d.source.index] == null && that.selectedNodes[d.target.index] == null && that.selectedLink[d.in] == null) {
                                d3.select(this).style("opacity", 0);
                            }
                            /*
                            //判断连接的点是否被选中
                            svgLeft.selectAll(".nodeup").filter(function(d1, i){
                               return d1.index == d.target.index || d1.index == d.source.index;
                            }).style("opacity", function(d2){
                                if(that.selectedNodes[d2.index]) return 1;
                                var linked = svgLeft.selectAll(".polygonup").filter(function(d1, i){
                                    return (((d1.source.index == d2.index && that.selectedNodes[d1.target.index]) || (d1.target.index == d.index && that.selectedNodes[d1.source.index])) && d1.thick > 0) || ((d1.source.index == d2.index || d1.target.index == d2.index) && that.selectedLink[d1.in] == 1);
                                });
                                if(linked[0].length == 0) return 0;
                                return 1;
                            })
                            .select("circle").style("stroke","black");

                            svgLeft.selectAll(".nodeup").filter(function(d1, i){
                               return that.selectedNodes[d1.index] == 1;
                            }).style("opacity", 1)
                              .select("circle").style("stroke","red");
                              */
                            
                            var thisnode = svgLeft.selectAll(".nodeup").filter(function(d1, i){
                               return d1.index == d.target.index;
                            }).style("opacity", function(d1){
                                if(d1.selected == 1 || that.selectedNodes[d1.index]) return 1;
                                return 0;
                            }).select("circle").style("stroke", function(d1){
                                if(d1.linkselected) return "red";
                                if(that.selectedNodes[d1.index] == null) return "black";
                                return "red";
                            });
                            var thissourcenode = svgLeft.selectAll(".nodeup").filter(function(d1, i){
                                return d1.index == d.source.index;
                            }).style("opacity", function(d1){
                                if(d1.selected == 1 || that.selectedNodes[d1.index]) return 1;
                                return 0;
                            }).select("circle").style("stroke", function(d1){
                                if(d1.linkselected) return "red";
                                if(that.selectedNodes[d1.index] == null) return "black";
                                return "red";
                            });
                            /*
                        //如果是选择的圆圈变黑，否则都变黑且沉下去
                        if(that.selectedNodes[d.source.index] == 1 || that.selectedLink[d.in] == 1) {
                            //thissourcenode.select("circle").style("stroke","black");
                        }
                        else {
                            //如果不与选择到的节点相连
                            var linked = svgLeft.selectAll(".polygonup").filter(function(d1, i){
                                return ((d1.source.index == d.source.index && that.selectedNodes[d1.target.index]) || (d1.target.index == d.source.index && that.selectedNodes[d1.source.index])) && d1.thick > 0;
                            });
                            if(linked[0].length == 0) {
                                thissourcenode.style("opacity", 0);
                            }
                            
                            //if(that.selectedNodes[d.target.index] == null) {
                            //    thissourcenode.style("opacity", 0);
                            //}
                            

                            thissourcenode.select("circle").style("stroke","black");
                        }

                        if(that.selectedNodes[d.target.index] == 1 || that.selectedLink[d.in] == 1) {
                            //thisnode.select("circle").style("stroke","black");
                        }
                        else {
                            //如果不与选择到的节点相连
                            var linked = svgLeft.selectAll(".polygonup").filter(function(d1, i){
                                return ((d1.source.index == d.target.index && that.selectedNodes[d1.target.index]) || (d1.target.index == d.target.index && that.selectedNodes[d1.source.index])) && d1.thick > 0;
                            });
                            if(linked[0].length == 0) {
                                thissourcenode.style("opacity", 0);
                            }
                            
                            //if(that.selectedNodes[d.source.index] == null) {
                             //   thisnode.style("opacity", 0);
                            //}
                            
                            thisnode.select("circle").style("stroke","black");
                        }
                        */
                        }
                        
                    });
                polygonup.append("path")
                    .attr("d", function(d2){
                        var da = "M" + xScale(d2.source.x).toString() + " " + yScale(d2.source.y).toString() + "L" + xScale(d2.target.x) + " " + yScale(d2.target.y);
                        return da;
                    })
                    .style("fill", "none")
                    .style("stroke", "none")
                    .style("stroke-width", 0);
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
                            connection[i] = connection[i];
                        }
                        that.selectedNodes = {};
                        that.selectedLink = {};
                        that.selectedLink[d.in] = 1;

                        that.$store.commit('set_pic', [that.g1name, that.g2name, Number(d.source.index) + 1, Number(d.target.index) + 1]);


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
                        }).style("opacity", 0).each(function(d1){
                            d1.selected = 0;
                            d1.linkselected = 0;
                        });
                        svgLeft.selectAll(".nodeup").filter(function(d1, i){
                            return that.selectedNodes[d1.index] == null;
                        }).select("circle").style("stroke", "black");

                        //未选择的线不显示
                        //svgLeft.selectAll(".polygonup").style("opacity",  0);
                        var unselected = svgLeft.selectAll(".polygonup").style("opacity",  0)
                            /*
                            .style("transform", function(){
                                return "none";
                            });
                            */
                            .each(function(d1){
                            var source = that.deepCopy(d1.source);
                            var target = that.deepCopy(d1.target);
                            if(connection[target.index] > connection[source.index]) {
                                source = that.deepCopy(d1.target);
                                target = that.deepCopy(d1.source);
                            }
                            //假设第一圆距离该点的距离比最后一个圆远，旋转
                            var firstdis = Math.sqrt((d1.origincx[0] - xScale(source.x)) * (d1.origincx[0] - xScale(source.x)) + (d1.origincy[0] - yScale(source.y)) * (d1.origincy[0] - yScale(source.y)));
                            var lastdis = Math.sqrt((d1.origincx[dataNum - 1] - xScale(source.x)) * (d1.origincx[dataNum - 1] - xScale(source.x)) + (d1.origincy[dataNum - 1] - yScale(source.y)) * (d1.origincy[dataNum - 1] - yScale(source.y)));
                            if(firstdis > lastdis) {
                                d1.origincx.reverse();
                                d1.origincy.reverse();
                                for(var numindex = 0; numindex < dataNum; numindex++) {
                                    var ellipseName = ".ellipseup" + numindex;
                                    d3.select(this).select(ellipseName).select("circle")
                                        .attr("cx", function() { 
                                            d1.cx[numindex] = d1.origincx[numindex];
                                            return d1.cx[numindex]; 
                                        })
                                        .attr("cy", function() { 
                                            d1.cy[numindex] = d1.origincy[numindex];
                                            return d1.cy[numindex]; 
                                        });
                                }
                            }
                        });
                        //未选择的圆位置恢复，多边形透明度回复
                        for(var numindex = 0; numindex < dataNum; numindex++) {
                            var ellipseName = ".ellipseup" + numindex;
                            unselected.select(ellipseName).select("circle")
                                .attr("cx", function(d1) { 
                                    d1.cx[numindex] = d1.origincx[numindex];
                                    return d1.origincx[numindex]; 
                                })
                                .attr("cy", function(d1) { 
                                    d1.cy[numindex] = d1.origincy[numindex];
                                    return d1.origincy[numindex]; 
                                })
                                .attr("r", function(d2){
                                    //console.log('d.data', d.data[numindex]);
                                        if (d2.thick >= that.fillMin && d2.thick <= that.fillMax){
                                            if (d2.thick == 0) return 0;
                                            if ((d2.sign == 1 && that.showPurple) || (d2.sign == -1 && that.showOrange) || (d2.sign == 0 && that.showGray)){
                                                if(d2.full[numindex])   return that.widthScale(d2.data[numindex]);
                                                return that.widthScale(0);
                                            }
                                            return 0;
                                        }
                                        else {
                                            return 0;
                                        }
                                    })
                                    .style("fill", function(d2) {
                                        if(d2.full[numindex]) return that.fillColor(d2.allSigns[numindex] >= 0);
                                        return that.unfullfillColor(d2.allSigns[numindex] >= 0);
                                    })
                                    .style("stroke", function(d2) {
                                        if(d2.full[numindex]) return that.strokeColor(d2.allSigns[numindex] >= 0);
                                        return that.unfullstrokeColor(d2.allSigns[numindex] >= 0);
                                    });
                        }

                        unselected.select("polygon").style("opacity", 1);
                        unselected.select("path").style("stroke-width", 0);
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
                                        }).each(function(d2){
                                            d2.selected = 1;
                                            d2.linkselected = 1;
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
                        }

                         var source = that.deepCopy(d.source);
                        var target = that.deepCopy(d.target);
                        if(connection[target.index] > connection[source.index]) {
                            source = that.deepCopy(d.target);
                            target = that.deepCopy(d.source);
                        }
                        //点位置变化
                        for(var numindex = 0; numindex < dataNum; numindex++) {
                            var ellipseName = ".ellipseup" + numindex;
                            var resultx = (xScale(target.x) - xScale(source.x)) * (numindex + 1) * 1.0 / (dataNum + 1) + xScale(source.x);
                            var resulty = (yScale(target.y) - yScale(source.y)) * (numindex + 1) * 1.0 / (dataNum + 1) + yScale(source.y);
                            console.log("resultx", resultx, "resulty", resulty);
                            svgLeft.selectAll(".polygonup").filter(function(d1) {
                                return d.source.index == d1.source.index && d.target.index == d1.target.index;
                            }).select(ellipseName).select("circle")
                                .attr("cx", function() { 
                                    d.cx[numindex] = resultx;
                                    return resultx; 
                                })
                                .attr("cy", function() { 
                                    d.cy[numindex] = resulty;
                                    return resulty; 
                                })
                                .attr("r", function(d2){
                                //console.log('d.data', d.data[numindex]);
                                    if (d2.thick >= that.fillMin && d2.thick <= that.fillMax){
                                        if (d2.thick == 0) return 0;
                                        if ((d2.sign == 1 && that.showPurple) || (d2.sign == -1 && that.showOrange) || (d2.sign == 0 && that.showGray)){
                                            //if(d2.full[numindex])   
                                            return that.widthScale(d2.data[numindex]);
                                            return that.widthScale(0);
                                        }
                                        return 0;
                                    }
                                    else {
                                        return 0;
                                    }
                                })
                                .style("fill", function(d2) {
                                    //if(d2.full[numindex]) 
                                    return that.fillColor(d2.allSigns[numindex] >= 0);
                                    return that.unfullfillColor(d2.allSigns[numindex] >= 0);
                                })
                                .style("stroke", function(d2) {
                                    //if(d2.full[numindex]) 
                                    return that.strokeColor(d2.allSigns[numindex] >= 0);
                                    return that.unfullstrokeColor(d2.allSigns[numindex] >= 0);
                                });
                            }

                    });
                
                
                //添加椭圆
                for(var numindex = 0; numindex < dataNum; numindex++) {
                    var classname = ".ellipseup" + numindex;
                    var num = numindex;
                    var ellipseup = polygonup.append("g")
                        .attr("class", function(d,i){
                            return "ellipseup" + numindex + " ellipseup";
                        })
                        .call(that.drag);


                    ellipseup.append("circle")
                        .attr("r", function(d){
                            //console.log('d.data', d.data[numindex]);
                            if (d.thick >= that.fillMin && d.thick <= that.fillMax){
                                if (d.thick == 0) return 0;
                                if ((d.sign == 1 && that.showPurple) || (d.sign == -1 && that.showOrange) || (d.sign == 0 && that.showGray)){
                                    if(d.full[numindex])   return that.widthScale(d.data[numindex]);
                                    return that.widthScale(0);
                                }
                                return 0;
                            }
                            else {
                                return 0;
                            }
                            
                        })
                        .attr("cx", function(d) {
                            return (d.cx[numindex]);
                        })
                        .attr("cy", function(d){
                            return (d.cy[numindex]);
                        })
                        .style("transform-origin", function(d) {
                            return ((d.cx[numindex])/ width * 100).toString() + "% "  +( (d.cy[numindex]) / height * 100).toString() +"%";
                        })
                        .style("fill", function(d) {
                            if(d.full[numindex]) return that.fillColor(d.allSigns[numindex] >= 0);
                            return that.unfullfillColor(d.allSigns[numindex] >= 0);
                        })
                        .style("stroke-width", function(d) {
                            return 1;
                        }) //路径宽度
                        .style("stroke", function(d) {
                            if(d.full[numindex]) return that.strokeColor(d.allSigns[numindex] >= 0);
                            return that.unfullstrokeColor(d.allSigns[numindex] >= 0);
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
                        //.call(that.drag);
                        //.call(that.drag);

                    }

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
                        //与该点连接的边显示并变红
                        svgLeft.selectAll(".polygonup").filter(function(d1, i){
                            return (d1.source.index == d.index || d1.target.index == d.index) && d1.thick > 0;
                        }).style("opacity", 1)
                        .each(function(){
                            d3.select(this).select("path").style("stroke", "red");
                            d3.select(this).select("polygon").style("fill","red");
                        });
                    })
                    .on("mouseout", function(d, i) {
                        //与该点连接的边除去要显示的沉下
                        svgLeft.selectAll(".polygonup").filter(function(d1, i){
                            return (d1.source.index == d.index || d1.target.index == d.index) && !that.selectedLink[d1.in] && !that.selectedNodes[d1.source.index] && !that.selectedNodes[d1.target.index];
                        }).style("opacity", 0);
                        svgLeft.selectAll(".polygonup").filter(function(d1, i){
                            return d1.source.index == d.index || d1.target.index == d.index;
                        }).each(function(){
                            d3.select(this).select("path").style("stroke", "rgb(198, 198, 198)");
                            d3.select(this).select("polygon").style("fill","rgb(198, 198, 198)");
                        });
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
                            connection[d.index] = connection[d.index];
                        }
                        else{
                            that.selectedNodes[d.index] = 1;
                            connection[d.index] = connection[d.index];
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
                        }).style("opacity", 0).each(function(d1){
                            d1.selected = 0;
                            d1.linkselected = 0;
                        });
                        svgLeft.selectAll(".nodeup").filter(function(d1, i){
                            return that.selectedNodes[d1.index] == null;
                        }).select("circle").style("stroke", "black");
                        //根据重要性重画与该边连接的polygon
                        //逆时针从小到大排序
                        var thetaArr = [];
                        var thetaSub = [];
                        var thisnodelink = polygonup.filter(function(d1) {
                            return (d1.target.index == d.index || d1.source.index == d.index) && d1.thick > 0;
                        }).each(function(d1) {
                            var sourcenode = that.deepCopy(d1.source);
                            var targetnode = that.deepCopy(d1.target);
                            if(d1.target.index == d.index) {
                                sourcenode = that.deepCopy(d1.target);
                                targetnode = that.deepCopy(d1.source);
                            }
                            var dx = xScale(targetnode.x) - xScale(sourcenode.x);
                            var dy = yScale(targetnode.y )- yScale(sourcenode.y);
                            //console.log("sourcenode", sourcenode, "targetnode", targetnode);
                            var theta = (Math.atan2(-dy, dx) * 180 / Math.PI + 360) % 360;  //-180 ~ 180
                            d1.theta = theta;
                            thetaArr.push(theta);
                        });
                        thisnodelink.sort(function(a, b) {
                            return a.theta - b.theta;
                        });
                        var edges = thisnodelink[0].length;
                        thetaArr.sort(function(i, j){
                            return i - j;
                        });
                       
                        for(var ti in thetaArr) {
                            var thetatmp = (thetaArr[(Number(ti) + 1) % edges] - thetaArr[ti] + 360) % 360;
                            thetaSub.push(thetatmp);
                        }
                        //var that.thetaMin =30; // Math.min(...thetaSub);
                        //var that.thetaMin = Math.min(that.thetaMin, 360 / edges);
                        //var that.thetaoffset = 10;
                        //var thetaLeft = Math.min(360, (Math.max(...thetaArr) - Math.min(...thetaArr)) + 179);
                        var thetaLeft = 360;
                        var edgeLeft = edges;
                        var LMin = that.widthScale(1) * (dataNum + 1) * 2;
                        console.log("LMin", LMin, "ellipseWidth", that.ellipseWidth);
                        var colortmp = ["red", "orange", "yellow","green", "blue", "purple", "black", "gray"];

                         console.log("thisnodelink", thisnodelink, "thetaArr", thetaArr, "thetaSub", thetaSub);
                        thisnodelink.each(function(d1, i) {
                            /*
                            if(i == 0) {
                                edgeLeft -= 1;
                                return;
                            }
                            */
                            //该边与上一条边的角度
                            var theta = thetaSub[(Number(i) - 1 + edges) % edges];
                            //变化角度
                            var distheta = 0;
                            var fullUse = 0;
                            //增大theta, theta比设定的值大或者比均分的值大
                            if(theta < Math.min(that.thetaMin, thetaLeft / edgeLeft) || Math.abs(theta - Math.min(that.thetaMin, thetaLeft / edgeLeft)) < 0.000001) {
                                distheta = Math.min(that.thetaMin, thetaLeft / edgeLeft) - theta;
                            }
                            //减小theta，theta最大可到均分的剩余或者是均分
                            else if(theta > Math.max(thetaLeft - that.thetaMin * (edgeLeft - 1), thetaLeft / edgeLeft) || Math.abs(theta - Math.max(thetaLeft - that.thetaMin * (edgeLeft - 1), thetaLeft / edgeLeft)) < 0.000001) {
                                distheta = Math.max(thetaLeft - that.thetaMin * (edgeLeft - 1), thetaLeft / edgeLeft) - theta;
                            }
                            var maxtheta = 140;
                            if(distheta >= maxtheta && (Math.abs(distheta - maxtheta) >= Math.min(that.thetaMin, thetaLeft / edgeLeft))) {
                                fullUse = distheta - maxtheta;
                                distheta = maxtheta;
                                //LMin *= 2;
                                
                            } else if (distheta <= -maxtheta && (Math.abs(distheta + maxtheta) >= Math.min(that.thetaMin, thetaLeft / edgeLeft))) {
                                fullUse = -maxtheta - distheta;
                                distheta = -maxtheta;
                                //LMin *= 2;
                            }
                            if(distheta > thetaLeft) {
                                distheta = thetaLeft;
                            }
                            console.log("thetaLeft", thetaLeft, "theta", theta, "that.thetaMin", that.thetaMin, "thetaLeft / edgeLeft", thetaLeft / edgeLeft, "distheta", distheta, "Math.min", Math.min(that.thetaMin, thetaLeft / edgeLeft), "Math.max", Math.max(thetaLeft - that.thetaMin * (edgeLeft - 1), thetaLeft / edgeLeft));
                            //第三步
                            //如果改边不够长放置所有球
                            var dx = xScale(d1.source.x) - xScale(d1.target.x);
                            var dy = yScale(d1.target.y) - yScale(d1.target.y);
                            var L = Math.sqrt(dx * dx + dy * dy);
                            if(L < LMin && Math.abs(distheta) < that.thetaoffset) {
                                if(distheta >= 0) distheta = that.thetaoffset;
                                else distheta = -that.thetaoffset;
                            }
                            //找到控制点
                            var controlPointTheta = thetaArr[i] + distheta;
                            var control = LMin * 2;
                            var x = control * Math.cos(controlPointTheta / 180 * Math.PI);
                            var y = control * Math.sin(controlPointTheta / 180 * Math.PI);
                            console.log("x",x,"y",y, "controlPointTheta", controlPointTheta, "control", control, "distheta", distheta);
                            var controlPointX = xScale(d.x) + x;
                            var controlPointY = yScale(d.y) - y;
                            d1.controlPoint = [controlPointX, controlPointY];

                            var sourcenode = that.deepCopy(d1.source);
                            var targetnode = that.deepCopy(d1.target);
                            d1.inverse = 0;
                            if(d1.target.index == d.index) {
                                sourcenode = that.deepCopy(d1.target);
                                targetnode = that.deepCopy(d1.source);
                                d1.inverse = 1;
                            }
                            //边显示变形
                            d3.select(this).select("path").attr("d", function(d2){
                                var da = "M" + xScale(d.x).toString() + " " + yScale(d.y).toString() + "Q" + controlPointX.toString() + " " + controlPointY.toString() + " " + xScale(targetnode.x) + " " + yScale(targetnode.y);
                                return da;
                            })
                            .style("fill", "none")
                            .style("stroke", "rgb(180, 180, 180)")
                            /*
                            .style("stroke", function(){
                                return d3.rgb((255 / edges) * Number(i), 255, 255).toString();
                                var s = "rgb(" + ((255 / edges) * Number(i)).toString() + ", 255, 255)";
                            })
                            */
                            .style("stroke-width", 2);
                            //多边形线不显示
                            d3.select(this).select("polygon").style("opacity", 0);
                            //假设第一圆距离该点的距离比最后一个圆远，旋转
                            var firstdis = Math.sqrt((d1.origincx[0] - xScale(d.x)) * (d1.origincx[0] - xScale(d.x)) + (d1.origincy[0] - yScale(d.y)) * (d1.origincy[0] - yScale(d.y)));
                            var lastdis = Math.sqrt((d1.origincx[dataNum - 1] - xScale(d.x)) * (d1.origincx[dataNum - 1] - xScale(d.x)) + (d1.origincy[dataNum - 1] - yScale(d.y)) * (d1.origincy[dataNum - 1] - yScale(d.y)));
                            if(firstdis > lastdis) {
                                d1.origincx.reverse();
                                d1.origincy.reverse();
                                for(var numindex = 0; numindex < dataNum; numindex++) {
                                    var ellipseName = ".ellipseup" + numindex;
                                    d3.select(this).select(ellipseName).select("circle")
                                        .attr("cx", function() { 
                                            d1.cx[numindex] = d1.origincx[numindex];
                                            return d1.cx[numindex]; 
                                        })
                                        .attr("cy", function() { 
                                            d1.cy[numindex] = d1.origincy[numindex];
                                            return d1.cy[numindex]; 
                                        });
                                }
                            }
                            //点位置变化
                            for(var numindex = 0; numindex < dataNum; numindex++) {
                                var ellipseName = ".ellipseup" + numindex;
                                //var distance = Math.sqrt((d1.origincx[numindex] - xScale(sourcenode.x)) * (d1.origincx[numindex] - xScale(sourcenode.x)) + (d1.origincy[numindex] - yScale(sourcenode.y)) * (d1.origincy[numindex] - yScale(sourcenode.y)));
                                //var pd = Math.sqrt((xScale(targetnode.x) - xScale(sourcenode.x)) * (xScale(targetnode.x) - xScale(sourcenode.x)) + (yScale(targetnode.y) - yScale(sourcenode.y)) * (yScale(targetnode.y) - yScale(sourcenode.y)));
                                //var result = that.BezierFunction([xScale(d.x), yScale(d.y)], [controlPointX, controlPointY], [xScale(targetnode.x), yScale(targetnode.y)], distance/pd);
                                var result = that.BezierFunction([xScale(d.x), yScale(d.y)], [controlPointX, controlPointY], [xScale(targetnode.x), yScale(targetnode.y)], (numindex + 1) * 1.0 / (dataNum + 1));
                                d3.select(this).select(ellipseName).select("circle")
                                    .attr("cx", function() { 
                                        d1.cx[numindex] = result[0];
                                        return result[0]; 
                                    })
                                    .attr("cy", function() { 
                                        d1.cy[numindex] = result[1];
                                        return result[1]; 
                                    })
                                    .attr("r", function(d2){
                                    //console.log('d.data', d.data[numindex]);
                                        if (d2.thick >= that.fillMin && d2.thick <= that.fillMax){
                                            if (d2.thick == 0) return 0;
                                            if ((d2.sign == 1 && that.showPurple) || (d2.sign == -1 && that.showOrange) || (d2.sign == 0 && that.showGray)){
                                                //if(d2.full[numindex])   
                                                    return that.widthScale(d2.data[numindex]);
                                                return that.widthScale(0);
                                            }
                                            return 0;
                                        }
                                        else {
                                            return 0;
                                        }
                                    })
                                    .style("fill", function(d2) {
                                        //if(d2.full[numindex]) 
                                        return that.fillColor(d2.allSigns[numindex] >= 0);
                                        return that.unfullfillColor(d2.allSigns[numindex] >= 0);
                                    })
                                    .style("stroke", function(d2) {
                                        //if(d2.full[numindex]) 
                                        return that.strokeColor(d2.allSigns[numindex] >= 0);
                                        return that.unfullstrokeColor(d2.allSigns[numindex] >= 0);
                                    });
                            }
                            /*
                            //假设另一点重要性比该点大，旋转
                            if(connection[d.index] < connection[targetnode.index]) {
                                d3.select(this).style("transform", function(){
                                    return "rotate(180deg)";
                                });
                            }
                            else {
                                d3.select(this).style("transform", function(){
                                    return "none";
                                });
                            }
                            */
                            edgeLeft -= 1;
                            thetaLeft = thetaLeft - (theta + distheta) + fullUse;
                            //下一条边与该边角度变化
                            thetaSub[i] -= distheta;
                        });
                        /*
                        for(var ti in thisnodelink[0]) {
                            if(ti == 0) {
                                console.log("thisnodelink[0][ti]", thisnodelink[0][ti]);
                                thisnodelink[0][ti].children[0].style("fill", "red");
                                //thisnodelink[0][ti].selectAll("polygon").style("fill","red");
                            }
                        }
                        */
                        //console.log("thisnodelink", thisnodelink[0].length, "thetaArr", thetaArr, "thetaSub", thetaSub);
                        /*
                        polygonup.filter(function(d1) {
                            return d1.target.index == d.index || d1.source.index == d.index;
                        });
                        */
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
                                        }).style("opacity", 1).each(function(d2){
                                            d2.selected = 1;
                                        });
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
                        var unselected = svgLeft.selectAll(".polygonup").filter(function(d1) {
                            return !(that.selectedNodes[d1.source.index] == 1 || that.selectedNodes[d1.target.index] == 1);
                        }).style("opacity",  0)
                        /*
                            .style("transform", function(){
                                return "none";
                            });
                            */
                        .each(function(d1){
                            var source = that.deepCopy(d1.source);
                            var target = that.deepCopy(d1.target);
                            if(connection[target.index] > connection[source.index]) {
                                source = that.deepCopy(d1.target);
                                target = that.deepCopy(d1.source);
                            }
                            //假设第一圆距离该点的距离比最后一个圆远，旋转
                            var firstdis = Math.sqrt((d1.origincx[0] - xScale(source.x)) * (d1.origincx[0] - xScale(source.x)) + (d1.origincy[0] - yScale(source.y)) * (d1.origincy[0] - yScale(source.y)));
                            var lastdis = Math.sqrt((d1.origincx[dataNum - 1] - xScale(source.x)) * (d1.origincx[dataNum - 1] - xScale(source.x)) + (d1.origincy[dataNum - 1] - yScale(source.y)) * (d1.origincy[dataNum - 1] - yScale(source.y)));
                            if(firstdis > lastdis) {
                                d1.origincx.reverse();
                                d1.origincy.reverse();
                                for(var numindex = 0; numindex < dataNum; numindex++) {
                                    var ellipseName = ".ellipseup" + numindex;
                                    d3.select(this).select(ellipseName).select("circle")
                                        .attr("cx", function() { 
                                            d1.cx[numindex] = d1.origincx[numindex];
                                            return d1.cx[numindex]; 
                                        })
                                        .attr("cy", function() { 
                                            d1.cy[numindex] = d1.origincy[numindex];
                                            return d1.cy[numindex]; 
                                        });
                                }
                            }
                        });
                        //未选择的圆位置恢复，多边形透明度回复
                        for(var numindex = 0; numindex < dataNum; numindex++) {
                            var ellipseName = ".ellipseup" + numindex;
                            unselected.select(ellipseName).select("circle")
                                .attr("cx", function(d1) { 
                                    d1.cx[numindex] = d1.origincx[numindex];
                                    return d1.origincx[numindex]; 
                                })
                                .attr("cy", function(d1) { 
                                    d1.cy[numindex] = d1.origincy[numindex];
                                    return d1.origincy[numindex]; 
                                })
                                .attr("r", function(d2){
                                    //console.log('d.data', d.data[numindex]);
                                        if (d2.thick >= that.fillMin && d2.thick <= that.fillMax){
                                            if (d2.thick == 0) return 0;
                                            if ((d2.sign == 1 && that.showPurple) || (d2.sign == -1 && that.showOrange) || (d2.sign == 0 && that.showGray)){
                                                if(d2.full[numindex])   return that.widthScale(d2.data[numindex]);
                                                return that.widthScale(0);
                                            }
                                            return 0;
                                        }
                                        else {
                                            return 0;
                                        }
                                    })
                                    .style("fill", function(d2) {
                                        if(d2.full[numindex]) return that.fillColor(d2.allSigns[numindex] >= 0);
                                        return that.unfullfillColor(d2.allSigns[numindex] >= 0);
                                    })
                                    .style("stroke", function(d2) {
                                        if(d2.full[numindex]) return that.strokeColor(d2.allSigns[numindex] >= 0);
                                        return that.unfullstrokeColor(d2.allSigns[numindex] >= 0);
                                    });
                        }
                        unselected.select("polygon").style("opacity", 1);
                        unselected.select("path").style("stroke-width", 0);
                        //click end
                        that.loading = false;
                        }, 1);
                    });
       
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

                //添加鱼眼
                var fisheye = d3.fisheye.circular()
                        .radius(100)
                        .distortion(2);

                svgLeft.on("mousemove", function() {
                    if(that.asFish) {
                        /*
                    var rexScale = d3.scale.linear() //d3.scaleLinear()
                        .domain([-100*width/905 + 30, width-100*width/905 + 30])
                        .range([0,x_range.x2]);
                    var reyScale = d3.scale.linear() //d3.scaleLinear()
                        .domain([-70*height/1030 + 20, height-70*height/1030 + 20])
                        .range([0,y_range.y2]);
                    */
                    //console.log("d3.mouse(this)", d3.mouse(this));
                    //fisheye.focus([rexScale(d3.mouse(this)[0]), reyScale(d3.mouse(this)[1])]);
                    fisheye.focus(d3.mouse(this));
                    //点变形
                    nodeLeftup
                        .each(function(d) { 
                            d.localx = xScale(d.x);
                            d.localy = yScale(d.y);
                            d.fisheye = fisheye(d); 
                        })
                        .attr("cx", function(d) { 
                            return (d.fisheye.x); 
                        })
                        .attr("cy", function(d) { 
                            return (d.fisheye.y); 
                        })
                        .attr("transform", function(d){
                            if(connection[d.index] <= 0) return "translate(" + (d.fisheye.x) + "," + (d.fisheye.y) +") " + "scale(0)";
                            return "translate(" + (d.fisheye.x) + "," + (d.fisheye.y) +")";
                        });
                    nodeLeft
                        .each(function(d) { 
                            d.localx = xScale(d.x);
                            d.localy = yScale(d.y);
                            d.fisheye = fisheye(d); 
                        })
                        .attr("cx", function(d) { return (d.fisheye.x); })
                        .attr("cy", function(d) { return (d.fisheye.y); })
                        .attr("transform", function(d){
                            if(connection[d.index] <= 0) return "translate(" + (d.fisheye.x) + "," + (d.fisheye.y) +") " + "scale(0)";
                            return "translate(" + (d.fisheye.x) + "," + (d.fisheye.y) +")";
                        });

                    //边上球变形
                    for(var numindex = 0; numindex < dataNum; numindex++) {
                        var circleName = ".ellipse" + numindex;
                        svgLeft.selectAll(".polygon").selectAll(circleName)
                            .each(function(d) { 
                                d.localx = (d.cx[numindex]);
                                d.localy = (d.cy[numindex]);
                                d.fisheye = fisheye(d); 
                            })
                            .selectAll("circle")
                            .attr("cx", function(d) { 
                                return (d.fisheye.x); 
                            })
                            .attr("cy", function(d) { 
                                return (d.fisheye.y); 
                            });
                        var circleNameUp = ".ellipseup" + numindex;
                        svgLeft.selectAll(".polygonup").selectAll(circleNameUp)
                            .each(function(d) { 
                                d.localx = (d.cx[numindex]);
                                d.localy = (d.cy[numindex]);
                                d.fisheye = fisheye(d); 
                            })
                            .selectAll("circle")
                            .attr("cx", function(d) { 
                                return (d.fisheye.x); 
                            })
                            .attr("cy", function(d) { 
                                return (d.fisheye.y); 
                            });
                    }

                    //path变形
                    svgLeft.selectAll(".polygonup").select("path").attr("d", function(d){
                        var da;
                        if(d.inverse) {
                            da = "M" + d.target.fisheye.x + " " + d.target.fisheye.y + "Q" + d.controlPoint[0] + " " + d.controlPoint[1] + " " + d.source.fisheye.x + " " + d.source.fisheye.y;
                        }
                        else da = "M" + d.source.fisheye.x + " " + d.source.fisheye.y + "Q" + d.controlPoint[0] + " " + d.controlPoint[1] + " " + d.target.fisheye.x + " " + d.target.fisheye.y;
                        return da;
                    });
                    

                    //边变形
                    svgLeft.selectAll(".polygon").selectAll("polygon")
                        .attr("points", function(d){
                        var source = [d.source.fisheye.x, d.source.fisheye.y];
                        var target = [d.target.fisheye.x, d.target.fisheye.y];
                        var x_distance = target[0] - source[0];
                        var y_distance = target[1] - source[1];
                        var c_distance = Math.pow(x_distance, 2) + Math.pow(y_distance, 2);
                        var distance = Math.sqrt(c_distance);

                        //当y_distance = 0时，即平行于x轴, y值直接变化
                        if(y_distance == 0) {
                             if(connection[d.source.index] >= connection[d.target.index])
                            return d.source.fisheye.x.toString() + ',' + (d.source.fisheye.y - 2).toString() + " " + 
                                d.target.fisheye.x.toString() + ',' + (d.target.fisheye.y - 0.5).toString() + " " +
                                d.target.fisheye.x.toString() + ',' + (d.target.fisheye.y + 0.5).toString() + " " +
                                d.source.fisheye.x.toString() + ',' + (d.source.fisheye.y + 2).toString() + " " ;
                        else return d.target.fisheye.x.toString() + ',' + (d.target.fisheye.y - 2).toString() + " " + 
                                d.source.fisheye.x.toString() + ',' + (d.source.fisheye.y - 0.5).toString() + " " +
                                d.source.fisheye.x.toString() + ',' + (d.source.fisheye.y + 0.5).toString() + " " +
                                d.target.fisheye.x.toString() + ',' + (d.target.fisheye.y + 2).toString() + " " ;
                        }
                        //当x_disatnce = 0, 即垂直于x轴时，x值变化
                        if(x_distance == 0) {
                             if(connection[d.source.index] >= connection[d.target.index])
                            return (d.source.fisheye.x - 2).toString() + ',' + (d.source.fisheye.y).toString() + " " + 
                                (d.target.fisheye.x - 0.5).toString() + ',' + (d.target.fisheye.y).toString() + " " +
                                (d.target.fisheye.x + 0.5).toString() + ',' + (d.target.fisheye.y).toString() + " " +
                                (d.source.fisheye.x + 2).toString() + ',' + (d.source.fisheye.y).toString() + " " ;
                        else return (d.target.fisheye.x - 2).toString() + ',' + (d.target.fisheye.y).toString() + " " + 
                                (d.source.fisheye.x - 0.5).toString() + ',' + (d.source.fisheye.y).toString() + " " +
                                (d.source.fisheye.x + 0.5).toString() + ',' + (d.source.fisheye.y).toString() + " " +
                                (d.target.fisheye.x + 2).toString() + ',' + (d.target.fisheye.y).toString() + " " ;
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
                    });
                    svgLeft.selectAll(".polygonup").selectAll("polygon")
                        .attr("points", function(d){
                        var source = [d.source.fisheye.x, d.source.fisheye.y];
                        var target = [d.target.fisheye.x, d.target.fisheye.y];
                        var x_distance = target[0] - source[0];
                        var y_distance = target[1] - source[1];
                        var c_distance = Math.pow(x_distance, 2) + Math.pow(y_distance, 2);
                        var distance = Math.sqrt(c_distance);

                        //当y_distance = 0时，即平行于x轴, y值直接变化
                        if(y_distance == 0) {
                             if(connection[d.source.index] >= connection[d.target.index])
                            return d.source.fisheye.x.toString() + ',' + (d.source.fisheye.y - 2).toString() + " " + 
                                d.target.fisheye.x.toString() + ',' + (d.target.fisheye.y - 0.5).toString() + " " +
                                d.target.fisheye.x.toString() + ',' + (d.target.fisheye.y + 0.5).toString() + " " +
                                d.source.fisheye.x.toString() + ',' + (d.source.fisheye.y + 2).toString() + " " ;
                        else return d.target.fisheye.x.toString() + ',' + (d.target.fisheye.y - 2).toString() + " " + 
                                d.source.fisheye.x.toString() + ',' + (d.source.fisheye.y - 0.5).toString() + " " +
                                d.source.fisheye.x.toString() + ',' + (d.source.fisheye.y + 0.5).toString() + " " +
                                d.target.fisheye.x.toString() + ',' + (d.target.fisheye.y + 2).toString() + " " ;
                        }
                        //当x_disatnce = 0, 即垂直于x轴时，x值变化
                        if(x_distance == 0) {
                             if(connection[d.source.index] >= connection[d.target.index])
                            return (d.source.fisheye.x - 2).toString() + ',' + (d.source.fisheye.y).toString() + " " + 
                                (d.target.fisheye.x - 0.5).toString() + ',' + (d.target.fisheye.y).toString() + " " +
                                (d.target.fisheye.x + 0.5).toString() + ',' + (d.target.fisheye.y).toString() + " " +
                                (d.source.fisheye.x + 2).toString() + ',' + (d.source.fisheye.y).toString() + " " ;
                        else return (d.target.fisheye.x - 2).toString() + ',' + (d.target.fisheye.y).toString() + " " + 
                                (d.source.fisheye.x - 0.5).toString() + ',' + (d.source.fisheye.y).toString() + " " +
                                (d.source.fisheye.x + 0.5).toString() + ',' + (d.source.fisheye.y).toString() + " " +
                                (d.target.fisheye.x + 2).toString() + ',' + (d.target.fisheye.y).toString() + " " ;
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
                    });
                     /*   
                    nodeLeft.selectAll("circle")
                        .each(function(d) { d.fisheye = fisheye(d); })
                        .attr("cx", function(d) { return d.fisheye.x; })
                        .attr("cy", function(d) { return d.fisheye.y; })
                        .attr("r", function(d) { return d.fisheye.z * 4.5; });
                    svgLeft.selectAll(".polygon").selectAll(".ellipse").selectAll("circle")
                        .each(function(d) { d.fisheye = fisheye(d); })
                        .attr("cx", function(d) { return d.fisheye.x; })
                        .attr("cy", function(d) { return d.fisheye.y; })
                        .attr("r", function(d) { return d.fisheye.z * 4.5; });
                    */
                    /*
                    svgLeft.selectAll(".polygonup").selectAll(".ellipseup").selectAll("circle")
                        .each(function(d) { d.fisheye = fisheye(d); })
                        .attr("cx", function(d) { return d.fisheye.x; })
                        .attr("cy", function(d) { return d.fisheye.y; })
                        .attr("r", function(d) { return d.fisheye.z * 4.5; });
                        
                    link.attr("x1", function(d) { return d.source.fisheye.x; })
                        .attr("y1", function(d) { return d.source.fisheye.y; })
                        .attr("x2", function(d) { return d.target.fisheye.x; })
                        .attr("y2", function(d) { return d.target.fisheye.y; });
                        */
                    }
                });
                /*
                svgLeft.append('path').attr({  
                    id: 'mypath',  
                    d: 'M50 100Q350 50 350 250'  //当前点 控制点 结束点
                })
                .style("fill", "none")
                .style("stroke", "black")
                .style("stroke-width", 2);
                */
                //添加图例
                var rx = 8;
                var ry = 6;
                var center = 30;
                var jianju = 28;
                var startpoint = 59;
                //toptitle = "Mean Male,Female Group";
                //pearlName = [{'name':'Strength,Length', "index": 0},{'name':'Length,Curvature', "index": 1},{'name':'Curvature,Strength', "index": 2}];
                var BEpoints = [{'index': 0}, {'index': 1}];
                if(pearlName.length <= 0) toptitle1 = toptitle2 = toptitle3 = "";
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
                            return "legendpolygoneline";
                        })
                        .style("fill", function(d) {
                            return d3.rgb(205 - 1 * 138, 197 - 1 * 165, 224 - 1 * 85).toString();
                        })
                        .attr("points", function(d){
                            return "27," + startpoint + " 33," + startpoint + " 30.5," + ((pearlName.length + 1) * jianju + startpoint).toString() + " 29.5," + ((pearlName.length + 1) * jianju + startpoint).toString();
                        })
                        .style("opacity", function(d) {
                            return 1;
                        });
                    legendpolygon.append("text")
                        .attr("class", function(d,i){
                            return "uplegText";
                        })
                        .attr("font-family", "Arial")
                        .attr("y", 0)
                        .style("stroke-width", 0)
                        //.style("fill", "#8998AC")
                        .style("font-size", "14px")
                        .style("font-weight", 300)
                        .html(function(d) {
                            return toptitle1;
                        })
                        .attr("text-anchor", "middle")
                        .attr("transform", function(d){
                            
                            return "translate(" +(center) + "," + (0) +")";
                        });
                    legendpolygon.append("text")
                        .attr("class", function(d,i){
                            return "uplegText";
                        })
                        .attr("font-family", "Arial")
                        .attr("y", 15)
                        .style("stroke-width", 0)
                        //.style("fill", "#8998AC")
                        .style("font-size", "14px")
                        .style("font-weight", 300)
                        .html(function(d) {
                            return toptitle2;
                        })
                        .attr("text-anchor", "middle")
                        .attr("transform", function(d){
                            
                            return "translate(" +(center) + "," + (0) +")";
                        });
                    legendpolygon.append("text")
                        .attr("class", function(d,i){
                            return "uplegText";
                        })
                        .attr("font-family", "Arial")
                        .attr("y", 30)
                        .style("stroke-width", 0)
                        //.style("fill", "#8998AC")
                        .style("font-size", "14px")
                        .style("font-weight", 300)
                        .html(function(d) {
                            return toptitle3;
                        })
                        .attr("text-anchor", "middle")
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

                    legendellipse.append("circle")
                        .attr("r", function(d){
                            return rx;          
                        })
                        /*
                        .attr("ry", function(d) {
                            return ry;
                        })
                        */
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
                        //.style("fill", "#8998AC")
                        .style("font-size", "14px")
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

                    nodeLeft.attr("cx", function(d){
                        return xScale(d.x);
                    })
                    .attr("cy", function(d){
                        return yScale(d.y);
                    })
                    .attr("transform", function(d){
                        if(connection[d.index] <= 0) return "translate(" + xScale(d.x) + "," + yScale(d.y) +") " + "scale(0)";
                        return "translate(" + xScale(d.x) + "," + yScale(d.y) +")";
                    });

                    nodeLeftup.attr("cx", function(d){
                        return xScale(d.x);
                    })
                    .attr("cy", function(d){
                        return yScale(d.y);
                    })
                    .attr("transform", function(d){
                        if(connection[d.index] <= 0) return "translate(" + xScale(d.x) + "," + yScale(d.y) +") " + "scale(0)";
                        return "translate(" + xScale(d.x) + "," + yScale(d.y) +")";
                    });
               
                /*
                d3.select(this.$el)         
                    .select(s)
                    .select(".panel")
                    .select(".text1")
                    .html("Fill range: " + this.thick_range.r1 + "-" + this.thick_range.r2);
                */
                this.$store.commit('setneedReDraw', false);
            }
        },
        watch: {
            /*
            pointPos: {
                deep: true,
                handler: function(newVal, oldVal) {

                }
            },
            */
            strength: {
                deep: true,
                handler: function(newVal, oldVal) {
                    if(newVal && this.strength.allDatas.length > 0){
                        //console.log('strength', newVal);
                        this.loading = true;
                        var that = this;
                        NetService.getCirclePoint(this.strength.data, this.strength.allDatas.length, this.ellipseWidth, (res, err) => {
                            if(err){
                                console.error(err);
                                return 0;
                            }
                            if(!res.data){
                                console.log('data not exists!');
                                return;
                            }
                            console.log('getCirclePoint', res.data);
                            that.pointPos = res.data.circlePoints;
                            that.fullArr = res.data.full;
                            that.drawSidBySidePic();
                            that.loading = false;

                        });
                    }
                    else if(newVal && this.strength.allDatas.length == 0){
                        //this.loading = true;
                        var that = this;
                        setTimeout(function(){
                            that.drawSidBySidePic();
                            that.loading = false;
                        }, 0);
                    }
                }
            },
            ellipseWidth: {
                deep: true,
                handler: function(newVal, oldVal) {
                    if(this.strength.allDatas.length > 0){
                        //console.log('strength', newVal);
                        this.loading = true;
                        var that = this;
                        NetService.getCirclePoint(this.strength.data, this.strength.allDatas.length, this.ellipseWidth, (res, err) => {
                            if(err){
                                console.error(err);
                                return 0;
                            }
                            if(!res.data){
                                console.log('data not exists!');
                                return;
                            }
                            console.log('getCirclePoint', res.data);
                            that.pointPos = res.data.circlePoints;
                            that.fullArr = res.data.full;
                            that.drawSidBySidePic();
                            that.loading = false;

                        });
                    }
                }
            },
        }
    }
</script>