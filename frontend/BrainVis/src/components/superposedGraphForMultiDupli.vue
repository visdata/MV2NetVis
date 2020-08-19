<!--叠加图，strength从外部绑定-->
<template>
    <div style="text-align: left;">
        <div :class='setClass()' style="height:50%;">  
            <div class="leftSVG" style="padding-left: 18px;">
                <div class="tipsDiv" style="z-index: 10; color: black; font-size: 12px; font-weight: bold; background-color: rgb(180,180,180);position: absolute;visibility: hidden;"></div>
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
    import backgroundPic from "../assets/brain_xy.png"
    import slider from './slider'

    //import $ from "jquery"
    export default {
        name: 'superposedGraphForMultiDupli',
        components: {
            slider,
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
            }
        },
        data() {
            return {
                //strength,
                matrix: {},
                backgroundPic: require("../assets/brain_xy.png"),
                isFocus: false,
                filterRate: [0, 100],
                weight_range: {},
                thick_range: {},
                reverse: true,
            }
        },
        created() {
            this.matrix = this.deepCopy(originmatrix);
        },
        //mounted在html渲染出来后进行操作
        mounted() {
            if(this.strength.hasOwnProperty('accuracy'))
                this.drawSidBySidePic();
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
                    .attr("height", height);

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
                //var groupOneData = {}; //组1数据
                //var groupTwoData = {};
                var selectedNodes = {}; //eg:{1:1,2:0}
                var left_range, right_range;
                var x_range = {"x1":1000, "x2":0};
                var y_range = {"y1":1000, "y2":0};
                var z_range = {"z1":1000, "z2":0};
                var fillMax = 0, fillMin = 0;
                var filter_value = 0;  //weight低于该值则不显示
                var accuracy = {};
                //console.log(this.strength.pvalue[0]);
                //读取weight，thickness
                for(var i = 0; i < this.strength.pvalue.length; i++) {
                    indexWeight[i] = this.strength.selectConnection[i];
                    indexThickness[i] = this.strength.multidata[i];
                    //accuracy[i] = this.strength.accuracy[i];
                }

                //console.log('indexWeight', indexWeight);

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
                    if (d.target == d.source || indexWeight == null) {
                        d.weight = 0;
                        d.thick = 0;
                    } else {
                        d.weight = indexWeight[i];

                        d.thick = indexThickness[i];
                        rangeThick.r1 = Math.min(rangeThick.r1, Math.abs(indexThickness[i])); //记录绝对值的大小
                        rangeThick.r2 = Math.max(rangeThick.r2, Math.abs(indexThickness[i]));
                    }
                    //in指示weight的index
                    //d.accuracy = accuracy[i];
                    d.in = i;
                    tmp.in = i;
                });

                this.thick_range = rangeThick;
                fillMin = this.filterRate[0] * (this.thick_range.r2 - this.thick_range.r1) / 100 + this.thick_range.r1;
                fillMax = this.filterRate[1] * (this.thick_range.r2 - this.thick_range.r1) / 100 + this.thick_range.r1;

                var xScale = d3.scale.linear() //d3.scaleLinear()
                    .domain([0,x_range.x2])
                    .range([-100*width/905, width-100*width/905]);
                var yScale = d3.scale.linear() //d3.scaleLinear()
                    .domain([0,y_range.y2])
                    .range([-70*height/1030, height-70*height/1030]);
                
                //添加脑的背景图
                //console.log(this.backgroundPic);
                svgLeft.append("image").attr("class","background")
                    .attr("xlink:href", this.backgroundPic)
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
                    .select('.leftSVG')
                    .select(".tipsDiv");

                
                var that = this;
                //添加线
                var linkLeft = svgLeft.selectAll(".link")
                    .data(that.matrix.links)
                    .enter()
                    .append("line")
                    .attr("class", function(d,i) {
                        return "leftl" + that.clusterNo(d.source.index, clusters) + "l" + that.clusterNo(d.target.index, clusters) + "leftl" + that.clusterNo(d.source.index, clusters) + "leftl" + that.clusterNo(d.target.index, clusters) +" link";
                    })
                    .style("stroke-width", function(d) {
                        if(d.weight > 0) { //weight为是否显示，特征值为粗细
                            if (that.reverse)
                                return (Math.abs(d.thick) - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 4;
                            else return 4 - (Math.abs(d.thick) - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 4 
                        }
                        else return 0;
                    })  //路径宽度
                    .style("stroke", function(d){//路径颜色
                        if(Math.abs(d.thick) >= fillMin && Math.abs(d.thick) <= fillMax){
                            if(d.thick >= 0){
                                return that.fillScaleRed(Math.abs(d.thick) - fillMin, fillMax - fillMin);
                            } else {
                                return that.fillScaleGreen(Math.abs(d.thick) - fillMin, fillMax - fillMin);
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
                    .on("mouseenter", function(d, i) {
                        //console.log('mouseenter', d);
                        return tipsDiv
                            .style('visibility', 'visible')
                            .style('left', (d3.event.clientX - 70) + 'px')
                            .style('top', (d3.event.clientY - 300) + 'px')
                            .html('ROI: ' + (d.source.index + 1) + '  <->  ' + (d.target.index + 1) + '<br>' + 'multivalue: ' + d.thick.toFixed(5) + '<br>');//tooltip.html(text);
                    })
                    .on("mouseout", function(d, i) {
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
                        
                    })
                    .on("mouseenter", function(d, i) {
                       this.firstElementChild.setAttribute('r', 9);
                       this.lastElementChild.style.fontSize = "15px";
                    })
                    .on("mouseout", function(d, i) {
                       this.firstElementChild.setAttribute('r', 2);
                       this.lastElementChild.style.fontSize = "0px";
                    });

                //添加图标显示节点
                nodeLeft.append("circle")
                    .attr("class", function(d,i){
                        return "lobe" + that.clusterNo(i, clusters) + "Circle" + that.index;
                    })
                    .attr("r", 2)
                    .style("fill", "white")
                    .style("stroke", "black")
                    .style("stroke-width", 1);

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
                    .style("font-size", "0px")
                    .style("font-weight", 300)
                    .text(function(d) {
                        return d.index + 1;
                    });

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
                        //console.log('strength', newVal);
                        this.drawSidBySidePic();
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
                    svgLeft.selectAll(".link")
                        .style("stroke-width", function(d) {
                            if (that.reverse) {
                                if (Math.abs(d.thick) >= fillMin && Math.abs(d.thick) <= fillMax)
                                    return (Math.abs(d.thick) - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 4;
                                else {
                                    return 0;
                                }
                            }
                            else return 4 - (Math.abs(d.thick) - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 4 
                        })
                        .style("stroke", function(d){
                            if(Math.abs(d.thick) >= fillMin && Math.abs(d.thick) <= fillMax){
                                if(d.thick >= 0){
                                    return that.fillScaleRed(Math.abs(d.thick) - fillMin, fillMax - fillMin);
                                } else {
                                    return that.fillScaleGreen(Math.abs(d.thick) - fillMin, fillMax - fillMin);
                                }
                            } else {
                                //return d3.rgb(125, 125, 125).toString();
                                return d3.rgb(255, 255, 255).toString();
                            }
                        })
                        .style("stroke-opacity", function(d){
                            return 1;
                        });
                }
            },
            reverse: {
                deep: true,
                handler: function(newVal, oldVal) {
                   // console.log("reverse", newVal);
                    var s = ".draw-area-" + this.index;
                    var svgLeft = d3.select(this.$el)         
                        .select(s).select(".leftSVG");
                    var that = this;
                    svgLeft.selectAll(".link")
                        .style("stroke-width", function(d) {
                            if (newVal)
                                return (Math.abs(d.thick) - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 4;
                            else return 4 - (Math.abs(d.thick) - that.thick_range.r1) / (that.thick_range.r2 - that.thick_range.r1) * 4 
                        });
                }
            }
        }
    }
</script>