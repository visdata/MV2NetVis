<template>
    <div>
        <div class="draw-area" style="height:100%;">  
            <div style="float:left;" id="leftSVG"></div>
            <div style="float:left;" id="rightSVG"></div>
            <div style="float:left;" id="panel" ref="myPanel">
                <div id="slider"></div>
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
    import matrix from "../../data/matrix"
    import backgroundPic from "../assets/brain_xy.png"
    //import $ from "jquery"
    export default {
        name: 'sideBySide',
        props: {
            strength: {
                required: false,
                type: Object
            },
        },
        data() {
            return {
                //strength,
                matrix,
                backgroundPic: require("../assets/brain_xy.png"),
                isFocus: false,
            }
        },
        mounted() {
            this.drawSidBySidePic()
        },
        methods: {
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
            fillScale(weight, maxWeight) {
                var g = 245 * (1 - weight / maxWeight);
                return d3.rgb(255, g, g).toString();
            },
            changeColorFillScale(weight, maxWeight) {
                var g = 245 * (1 - weight / maxWeight);
                return d3.rgb(g, 255, g).toString();
            },
            highFillScale(weight, maxWeight) {
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
                d3.select(this.$el)         
                    .select(".draw-area")    /*返回第一个匹配的元素*/
                    .select("#leftSVG")
                    .selectAll("svg")     /* selectAll  选择在文档遍历次序中所有匹配的元素 */
                    .remove();
                d3.select(this.$el)         
                    .select(".draw-area")    /*返回第一个匹配的元素*/
                    .select("#rightSVG")
                    .selectAll("svg")     /* selectAll */
                    .remove();

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

                var height = window.screen.height*0.35;
                var width = height * 905 / 1030;
                
                //构造一个新的序数比例尺，有20种类型的颜色
                //var color = d3.scaleOrdinal(d3.schemeCategory20);
                var color = d3.scale.category20();
                console.log(color);

                //使用物理模拟排放链接节点的位置
                //电荷强度、链接强度、链接距离、重力大小、布局大小
                //var forceLeft = d3.forceSimulation();
                //var forceRight = d3.forceSimulation();

                //添加svg
                var svgLeft = d3.select("#leftSVG")
                    .append("svg")
                    .attr("width", width)
                    .attr("height", height);
                var svgRight = d3.select("#rightSVG")
                    .append("svg")
                    .attr("width", width)
                    .attr("height", height);
                var svgLegend = d3.select("#panel")
                    .append("svg")
                    .attr("width", 100 * width / 905)
                    .attr("height", 40 * height / 1030)
                    .append("g")

                var panel = this.$refs.myPanel;

                var weights; //定义权重
                var indexWeightLeft = {};
                var indexWeightRight = {};
                var selectedNodes = {}; //eg:{1:1,2:0}
                var total_range;
                var left_range, right_range;
                var x_range = {"x1":1000, "x2":0};
                var y_range = {"y1":1000, "y2":0};
                var z_range = {"z1":1000, "z2":0};
                var fillMax = 0, fillMin = 0;
                var filter_value = 0;
                
                //更新filter_value
                if (this.getQueryString("filter") != null) {
                    filter_value = Number(this.getQueryString("filter"));
                    console.log(filter_value);
                }
                
                
                console.log(this.strength.left[0]);
                //读取weight，且变为整数
                for(var i = 0; i < this.strength.left.length; i++) {
                    indexWeightLeft[i] = parseInt(this.strength.left[i]);
                }
                for(var i = 0; i < this.strength.right.length; i++) {
                    indexWeightRight[i] = parseInt(this.strength.right[i]);
                }
                //console.log(indexWeightRight[0]);
                //读取matrix里的node,节点位置，确定值域范围
                matrix.nodes.forEach(function(d){
                    x_range.x1 = x_range.x1 < d.x ? x_range.x1 : d.x;
                    x_range.x2 = x_range.x2 > d.x ? x_range.x2 : d.x;
                    y_range.y1 = y_range.y1 < d.y ? y_range.y1 : d.y;
                    y_range.y2 = y_range.y2 > d.y ? y_range.y2 : d.y;
                    z_range.z1 = z_range.z1 < d.z ? z_range.z1 : d.z;
                    z_range.z2 = z_range.z2 > d.z ? z_range.z2 : d.z;
                });

                //增添一属性用来记录右图的roi连接及对应强度
                matrix.linksRight = new Array();
                //range记录weight的最大最小值
                var range = {"r1":10000, "r2":0};
                //将weight记录进links，左图roi连接及对应强度，并计算range。source < target上三角矩阵
                matrix.links.forEach(function(d,i) {
                    var tmp = {};
                    tmp.target = d.target;
                    tmp.source = d.source;
                    //自己到自己以及无weight的情况皆为0
                    if (d.target == d.source || indexWeightLeft == null) {
                        d.weight = 0;
                    } else {
                        d.weight = indexWeightLeft[i];
                        range.r1 = Math.min(range.r1,indexWeightLeft[i]);
                        range.r2 = Math.max(range.r2,indexWeightLeft[i]);
                    }
                    if (d.target == d.source || indexWeightLeft == null) {
                        tmp.weight = 0;
                    } else {
                        tmp.weight = indexWeightRight[i];
                        range.r1 = Math.min(range.r1,indexWeightRight[i]);
                        range.r2 = Math.max(range.r2,indexWeightRight[i]);
                    }
                    //in指示weight的index
                    d.in = i;
                    tmp.in = i;
                    matrix.linksRight.push(tmp);
                });
                //console.log(matrix.links[1]);
                //console.log(matrix.linksRight[1]);
                left_range = range;
                right_range = range;
                total_range = range;

                
                var xScale = d3.scale.linear() //d3.scaleLinear()
                    .domain([0,x_range.x2])
                    .range([-100*width/905, width-100*width/905]);
                var yScale = d3.scale.linear() //d3.scaleLinear()
                    .domain([0,y_range.y2])
                    .range([-70*height/1030, height-70*height/1030]);
                
                //添加脑的背景图
                console.log(this.backgroundPic);
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
                    .nodes(matrix.nodes)
                    .links(matrix.links)
                    .start();
                /*
                var forceLeft = d3.forceSimulation(matrix.nodes)
                    .force("charge", d3.forceManyBody())
                    .force("link", d3.forceLink(matrix.links))
                    .restart();
                */

                var that = this;
                //添加线
                var linkLeft = svgLeft.selectAll(".link")
                    .data(matrix.links)
                    .enter()
                    .append("line")
                    .attr("class", function(d,i) {
                        return "leftl" + that.clusterNo(d.source.index, clusters) + "l" + that.clusterNo(d.target.index, clusters) + " leftl" + that.clusterNo(d.source.index, clusters) + " leftl" + that.clusterNo(d.target.index, clusters) +" link";
                    })
                    .style("stroke-width", 3)  //路径宽度
                    .style("stroke", function(d) { //路径
                        return that.fillScale(d.weight - fillMin, total_range.r2 - fillMin);
                    })
                    .style("stroke-opacity", function(d){
                        if (d.weight == 0) {
                            return 0;
                        }
                        return d.weight < filter_value ? 0 : 1;
                    })
                    .sort(function(a, b) {
                        return a.weight - b.weight;
                    })
                    //添加点击事件
                    .on("click", function(d){
                        console.log(d);
                        if(true){
                            var target = d.target.index;
                            var source = d.source.index;
                            //该图的线条 变粗以及变色
                            d3.select(this)
                                .style("stroke-width", 4)
                                .style("stroke", function(d){
                                    return that.changeColorFillScale(d.weight - fillMin, total_range.r2 - fillMin);
                                })
                                .style("stroke-opacity", function(d){
                                    return d.weight < filter_value ? 0 : 1;
                                });
                            //对应线条显示
                            svgRight.selectAll(".link")
                                .filter(function(d1) {
                                    if (d1.target.index == target && d1.source.index == source) {
                                        return this;
                                        }
                                    return null;
                                })
                                .style("stroke-width", 4)
                                .style("stroke", function(d) {
                                    return that.changeColorFillScale(d.weight - fillMin, total_range.r2 - fillMin);
                                })
                                .style("stroke-opacity", function(d) {
                                    return d.weight < filter_value ? 0 : 1;
                                });
                        }
                    });

                //添加节点
                var nodeLeft = svgLeft.selectAll(".node")
                    .data(matrix.nodes)
                    .enter()
                    .append("g")
                    .attr("class", "node")
                    .attr("id", function(d,i){
                        return "node"+i;
                    })
                    .on("click", function(d){
                        console.log(d);
                        var e = d3.event;
                        //e.shiftKey,同时按下shift和点击
                        if (e.shiftKey) {
                            //已经选择过一次，则取消选择
                            if (selectedNodes[d.index] != null) {
                                delete selectedNodes[d.index];
                            }
                            else selectedNodes[d.index] = 1;

                            //选择选中的所有node变红
                            if (that.count(selectedNodes) == 1) {
                                svgLeft.selectAll(".node")
                                    .filter(function(d1, i){
                                        return selectedNodes[i] == 1;
                                    })
                                    .select("circle")
                                    .style("fill", "red");

                                //选中的线变蓝
                                svgLeft.selectAll(".link")
                                    .filter(function(d1) {
                                        if (d1.select == 1){
                                            return this;
                                        }
                                        return null;
                                    })
                                    .style("stroke-width", 6)
                                    .style("stroke", function(d){
                                        return that.highFillScale(d.weight - fillMin, total_range.r2 - fillMin);
                                    })
                                    .style("stroke-opacity", function(d){
                                        return d.weight < filter_value ? 0 : 1;
                                    });

                                svgRight.selectAll(".node")
                                    .filter(function(d1, i){
                                        return selectedNodes[i] == 1;
                                    })
                                    .select("circle")
                                    .style("fill", "red");

                                //选中的线变蓝
                                svgRight.selectAll(".link")
                                    .filter(function(d1) {
                                        if (d1.select == 1){
                                            return this;
                                        }
                                        return null;
                                    })
                                    .style("stroke-width", 6)
                                    .style("stroke", function(d){
                                        return that.highFillScale(d.weight - fillMin, total_range.r2 - fillMin);
                                    })
                                    .style("stroke-opacity", function(d){
                                        return d.weight < filter_value ? 0 : 1;
                                    });
                            }
                            else {
                                svgLeft.selectAll(".node")
                                    .select("circle")
                                    .style("fill", "white");
                                svgLeft.selectAll(".link")
                                    .filter(function(d1){
                                        if (selectedNodes[d1.target.index] != null && selectedNodes[d1.source.index] != null || d1.select == 1) {
                                            d1.select = 1;
                                            return this;
                                        }
                                        return null;
                                    })
                                    .style("stroke-width", 6)
                                    .style("stroke", function(d1){
                                        return that.highFillScale(d1.weight - fillMin, total_range.r2 - fillMin);
                                    })
                                    .style("stroke-opacity", function(d1) {
                                        return d1.weight < filter_value ? 0 : 1;
                                    })
                                    .sort(function(d1) {
                                        svgLeft.selectAll(".link").sort(function(a){
                                            if (a != d1) return -1;
                                            return 1;
                                        })
                                    });

                                svgRight.selectAll(".node")
                                    .select("circle")
                                    .style("fill", "white");
                                svgRight.selectAll(".link")
                                    .filter(function(d1){
                                        if (selectedNodes[d1.target.index] != null && selectedNodes[d1.source.index] != null || d1.select == 1) {
                                            d1.select = 1;
                                            return this;
                                        }
                                        return null;
                                    })
                                    .style("stroke-width", 6)
                                    .style("stroke", function(d1){
                                        return that.highFillScale(d1.weight - fillMin, total_range.r2 - fillMin);
                                    })
                                    .style("stroke-opacity", function(d1) {
                                        return d1.weight < filter_value ? 0 : 1;
                                    })
                                    .sort(function(d1) {
                                        svgRight.selectAll(".link").sort(function(a){
                                            if (a != d1) return -1;
                                            return 1;
                                        })
                                    });

                                //清空
                                selectedNodes = {};
                            }
                            //停止该事件的传播
                            d3.event.stopPropagation();
                        }
                        else {
                            var neighbors = {};
                            var neighbors1 = {};
                            svgLeft.selectAll(".link").filter(function(d1){
                                if((d1.target == d || d1.source == d) && indexWeightLeft[d1.in] != null) {
                                    neighbors[d1.target.index] = 1;
                                    neighbors[d1.source.index] = 1;
                                    d1.select = 1;
                                    return this;
                                }
                                else {
                                    return null;
                                }
                            })
                            .style("stroke-width", 6)
                            .style("stroke", function(d1){
                                return that.highFillScale(d1.weight - fillMin, total_range.r2 - fillMin);
                            })
                            .style("stroke-opacity", function(d1){
                                return d1.weight < filter_value ? 0 : 1;
                            });

                            d3.select(this).select("circle").style("fill", "red");

                            svgRight.selectAll(".link").filter(function(d1){
                                if((d1.target == d || d1.source == d) && indexWeightLeft[d1.in] != null) {
                                    neighbors[d1.target.index] = 1;
                                    neighbors[d1.source.index] = 1;
                                    d1.select = 1;
                                    return this;
                                }
                                else {
                                    return null;
                                }
                            })
                            .style("stroke-width", 6)
                            .style("stroke", function(d1){
                                return that.highFillScale(d1.weight - fillMin, total_range.r2 - fillMin);
                            })
                            .style("stroke-opacity", function(d1){
                                return d1.weight < filter_value ? 0 : 1;
                            });

                        }
                    });

                //添加图标显示节点
                nodeLeft.append("circle")
                    .attr("class", function(d,i){
                        return "lobe" + that.clusterNo(i, clusters) + "Circle";
                    })
                    .attr("r", 9)
                    .style("fill", "white")
                    .style("stroke", "black")
                    .style("stroke-width", 2);

                //文字
                nodeLeft.append("text")
                    .attr("class", function(d,i){
                        return "lobe" + that.clusterNo(i, clusters) + "Text";
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

                svgRight.append("image").attr("class","background")
                    .attr("xlink:href", this.backgroundPic)
                    .attr("x", 0).attr("y", 0)
                    .attr("width", width)
                    .attr("height",height);

                //使用物理模拟排放链接节点的位置
                //电荷强度、链接强度、链接距离、重力大小、布局大小
                var forceRight = d3.layout.force()
                    .charge(0)
                    .linkStrength(0)
                    .linkDistance(0)
                    .gravity(0)
                    .size([width, height]);

                forceRight
                    .nodes(matrix.nodes)
                    .links(matrix.linksRight)
                    .start();
                /*
                var forceRight = d3.forceSimulation(matrix.nodes)
                    .force("link", d3.forceLink(matrix.linksRight))
                    .restart();
                */
                var linkRight = svgRight.selectAll(".link")
                    .data(matrix.linksRight)
                    .enter()
                    .append("line")
                    .attr("class", function(d,i) {
                        return "rightl" + that.clusterNo(d.source.index, clusters) + "l" + that.clusterNo(d.target.index, clusters) + " rightl" + that.clusterNo(d.source.index, clusters) + " rightl" + that.clusterNo(d.target.index, clusters) +" link";
                    })
                    .style("stroke-width", 3)
                    .style("stroke", function(d) {
                        return that.fillScale(d.weight - fillMin, total_range.r2 - fillMin);
                    })
                    .style("stroke-opacity", function(d){ //不透明度
                        if (d.weight == 0) {
                            return 0;
                        }
                        return d.weight < filter_value ? 0 : 1;
                    })
                    .sort(function(a, b) {
                        return a.weight - b.weight;
                    });

                var nodeRight = svgRight.selectAll(".node")
                    .data(matrix.nodes)
                    .enter()
                    .append("g")
                    .attr("class", "node")
                    .attr("id", function(d,i){
                        return "node"+i;
                    });

                nodeRight.append("circle")
                    .attr("class", function(d,i){
                        return "lobe" + that.clusterNo(i, clusters) + "Circle";
                    })
                    .attr("r", 9)
                    .style("fill", "white")
                    .style("stroke", "black")
                    .style("stroke-width", 2);

                nodeRight.append("text")
                    .attr("class", function(d,i){
                        return "lobe" + that.clusterNo(i, clusters) + "Text";
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

                forceRight.on("tick", function(){
                    linkRight.attr("x1", function(d){
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

                    nodeRight.attr("cx", function(d){
                        return xScale(d.x);
                    })
                    .attr("cy", function(d){
                        return yScale(d.y);
                    })
                    .attr("transform", function(d){
                        return "translate(" + xScale(d.x) + "," + yScale(d.y) +")";
                    });
                });

                fillMax = left_range.r2;
                /*
                //slider
                var legendRedDiv = d3.select("#slider")
                    .append("div")
                    .attr("id", "sliderRange");

                var leftSpan = d3.select("#slider")
                    .append("span")
                    .attr("id", "leftSpan");

                var rightSpan = d3.select("#slider")
                    .append("span")
                    .attr("id", "rightSpan");

                //d3.select("#slider span:first").attr("id", "leftSpan");
                //d3.select("#slider span:last").attr("id", "rightSpan");
                //d3.select("#slider div").attr("id", "sliderRange");
                d3.select("#panel").append("span")
                    .attr("id", "legendMinText");
                d3.select("#panel").append("span")
                    .attr("id", "legendMaxText");
                d3.select("#panel").append("div")
                    .attr("id", "legendText")
                    .attr("class", "text1")
                    .html("Fill range: " + fillMin + "-" + left_range.r2);

                d3.select("#slider").call(
                    d3.slider().axis(true)
                        .value([0, left_range.r2])
                        .on("slide", function(event, value) {
                            d3.select("#legendMaxText").text(values[1]);
                            d3.select("#legendMinText").text(values[0]);
                            d3.select("#legendText").html("Fill range: " + values[0] + "-" + values[1]);
                            fillMin = values[0];
                            fillMax = values[1];
                            //slide();
                            svgLeft.selectAll(".link")
                                .style("stroke", function(d){
                                    if(d.weight < fillMin) {
                                        if (that.isFocus) {
                                            return that.fillScale(0, total_range.r2);
                                        }
                                        else {
                                            if(d.select == 1) {
                                                return that.highFillScale(0, total_range.r2);
                                            }
                                            else {
                                                return that.fillScale(0, total_range.r2);
                                            }
                                        }
                                    }
                                    else if (d.weight > fillMax) {
                                        if (that.isFocus) {
                                            return that.fillScale(total_range.r2, total_range.r2);
                                        }
                                        else {
                                            if (d.select == 1) {
                                                return that.highFillScale(total_range.r2, total_range.r2);
                                            }
                                            else {
                                                return that.fillScale(total_range.r2, total_range.r2);
                                            }
                                        }
                                    }
                                    else {
                                        if (that.isFocus) {
                                            return that.fillScale(d.weight - fillMin, fillMax - fillMin);
                                        }
                                        else {
                                            if (d.select == 1) {
                                                return that.highFillScale(d.weight - fillMin, fillMax - fillMin);
                                            }
                                            else {
                                                return that.fillScale(d.weight - fillMin, fillMax - fillMin);
                                            }
                                        }
                                    }
                                });
                            svgRight.selectAll(".link")
                                .style("stroke", function(d){
                                    if(d.weight < fillMin) {
                                        if (that.isFocus) {
                                            return that.fillScale(0, total_range.r2);
                                        }
                                        else {
                                            if(d.select == 1) {
                                                return that.highFillScale(0, total_range.r2);
                                            }
                                            else {
                                                return that.fillScale(0, total_range.r2);
                                            }
                                        }
                                    }
                                    else if (d.weight > fillMax) {
                                        if (that.isFocus) {
                                            return that.fillScale(total_range.r2, total_range.r2);
                                        }
                                        else {
                                            if (d.select == 1) {
                                                return that.highFillScale(total_range.r2, total_range.r2);
                                            }
                                            else {
                                                return that.fillScale(total_range.r2, total_range.r2);
                                            }
                                        }
                                    }
                                    else {
                                        if (that.isFocus) {
                                            return that.fillScale(d.weight - fillMin, fillMax - fillMin);
                                        }
                                        else {
                                            if (d.select == 1) {
                                                return that.highFillScale(d.weight - fillMin, fillMax - fillMin);
                                            }
                                            else {
                                                return that.fillScale(d.weight - fillMin, fillMax - fillMin);
                                            }
                                        }
                                    }
                                });

                        //var redDivWidth = parseInt(d3.select("#slider").style("width")) - parseInt(d3.select("#rightSpan").style("left"));
                        //legendRedDiv.style("width", redDivWidth + "px");
                    })
                );*/
/*
                slider({
                    range: true,
                    min: 0,
                    max: left_range.r2,
                    values: [0, left_range.r2],
                    slide: function(event, ui) {
                        d3.select("#legendMaxText").text(ui.values[1]);
                        d3.select("#legendMinText").text(ui.values[0]);
                        d3.select("#legendText").html("Fill range: " + ui.values[0] + "-" + ui.values[1]);
                        fillMin = ui.values[0];
                        fillMax = ui.values[1];
                        //slide();
                        svgLeft.selectAll(".link")
                            .style("stroke", function(d){
                                if(d.weight < fillMin) {
                                    if (that.isFocus) {
                                        return that.fillScale(0, total_range.r2);
                                    }
                                    else {
                                        if(d.select == 1) {
                                            return that.highFillScale(0, total_range.r2);
                                        }
                                        else {
                                            return that.fillScale(0, total_range.r2);
                                        }
                                    }
                                }
                                else if (d.weight > fillMax) {
                                    if (that.isFocus) {
                                        return that.fillScale(total_range.r2, total_range.r2);
                                    }
                                    else {
                                        if (d.select == 1) {
                                            return that.highFillScale(total_range.r2, total_range.r2);
                                        }
                                        else {
                                            return that.fillScale(total_range.r2, total_range.r2);
                                        }
                                    }
                                }
                                else {
                                    if (that.isFocus) {
                                        return that.fillScale(d.weight - fillMin, fillMax - fillMin);
                                    }
                                    else {
                                        if (d.select == 1) {
                                            return that.highFillScale(d.weight - fillMin, fillMax - fillMin);
                                        }
                                        else {
                                            return that.fillScale(d.weight - fillMin, fillMax - fillMin);
                                        }
                                    }
                                }
                            });
                        svgRight.selectAll(".link")
                            .style("stroke", function(d){
                                if(d.weight < fillMin) {
                                    if (that.isFocus) {
                                        return that.fillScale(0, total_range.r2);
                                    }
                                    else {
                                        if(d.select == 1) {
                                            return that.highFillScale(0, total_range.r2);
                                        }
                                        else {
                                            return that.fillScale(0, total_range.r2);
                                        }
                                    }
                                }
                                else if (d.weight > fillMax) {
                                    if (that.isFocus) {
                                        return that.fillScale(total_range.r2, total_range.r2);
                                    }
                                    else {
                                        if (d.select == 1) {
                                            return that.highFillScale(total_range.r2, total_range.r2);
                                        }
                                        else {
                                            return that.fillScale(total_range.r2, total_range.r2);
                                        }
                                    }
                                }
                                else {
                                    if (that.isFocus) {
                                        return that.fillScale(d.weight - fillMin, fillMax - fillMin);
                                    }
                                    else {
                                        if (d.select == 1) {
                                            return that.highFillScale(d.weight - fillMin, fillMax - fillMin);
                                        }
                                        else {
                                            return that.fillScale(d.weight - fillMin, fillMax - fillMin);
                                        }
                                    }
                                }
                            });

                        var redDivWidth = parseInt(d3.select("#slider").style("width")) - parseInt(d3.select("#rightSpan").style("left"));
                        legendRedDiv.style("width", redDivWidth + "px");
                    },
                    stop: function(event, ui) {
                        var redDivWidth = parseInt(d3.select("#slider").style("width")) - parseInt(d3.select("#rightSpan").style("left"));
                        legendRedDiv.style("width", redDivWidth + "px");
                    }
                });
*/
            }
        }
    }
</script>