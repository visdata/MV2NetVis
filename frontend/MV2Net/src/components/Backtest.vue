<!-- 展示模板 -->
<!-- class="draw-are"作用是绘图区包括各种图形的绘制和鼠标事件 -->
<template>
    <div>
        <div class="frame_header">
            <label>Backtesting View</label>
        </div>
        <div class="draw-area" style="height:100%;">  
            <svg></svg>
        </div>
    </div>
</template>


<script>
    import * as d3 from "d3";
    import { mapState } from 'vuex';
    import C from './constants.js';

    export default {           /* export default 的用法：相当于提供一个接口给外界，让其他文件通过 import 来引入使用 */
        name: "Backtest",
        data() {
            return {

            }
        },
        computed: {          /* 用computed 接收，计算 */
            ...mapState(['width','height','end_date','data_backtest']),
            width_or_height(){
                return [this.width, this.height];
            },
        },
        watch:{              /* 如果watch监测的是一个对象的话，直接使用watch是不行的，此时我们可以借助于computed计算属性来完成 */
        //数据变化的话就用draw_backtest函数
            data_backtest: 'draw_backtest',
            width_or_height: 'draw_backtest',
            deep: true,
            immediate: true,
        },
        methods:{    /*调用对象上下文中的函数，还可以操作对象中包含的数据 */
            object2Array(p) {           /*  php对象和数组的相互转换，Array2object()、Object2array()  function? */
                var arr = [];
                for (var key in p) {
                    if (p.hasOwnProperty(key)) {
                        arr.push([key, p[key]]);
                    }
                }
                return arr;
            },
            //画图
            draw_backtest(){
                d3.select(this.$el)         
                    .select(".draw-area")    /*返回第一个匹配的元素 https://github.com/d3/d3/wiki/%E9%80%89%E6%8B%A9%E5%99%A8#d3_select*/
                    .selectAll("svg")     /* selectAll  选择在文档遍历次序中所有匹配的元素 */
                    .remove();

                if(this.data_backtest.length === 0){
                    return;
                }
                console.log("Backtest: get_data");
                var that = this;
                var summary_bt_fileds = [       /* 静态数据 summary?*/
                        "alpha", "annual_volatility", "annualized_returns", "beta", 
                        "max_drawdown", "sharpe", "volatility"
                    ];
                var summary_benchmark_fileds = [
                        "benchmark_annual_volatility", "benchmark_annualized_returns", "benchmark_volatility"
                    ];
                        
                var root_div = d3.select(this.$el);
                var bounding_rect = root_div.node().getBoundingClientRect(); 
                /*选择指定的节点,返回一个矩形对象，包含四个属性left、top、right和bottom。分别表示元素各边与页面上边和左边的距离。*/

                // var height = bounding_rect.height;
                
                var margin = { top: 5, right: 5, bottom: 5, left: 10 },    /*空间范围*/
                    width = bounding_rect.width  - margin.left - margin.right,
                    height = bounding_rect.height - margin.top - margin.bottom - C.FRAME_HEADER_HEIGHT
                    ;

                var svg = d3    /*svg是矢量图像，可以无限缩放，而且图像质量不会下降*/
                    .select(this.$el)
                    .select(".draw-area")
                    .append("svg")
                    .attr("width", width + margin.left + margin.right)
                    .attr("height", height + margin.top + margin.bottom)
                    .append("g")     /* 在 svg 中添加一个g，g是 svg 中的一个属性，是 group 的意思，它表示一组 */
                    .attr("transform", "translate(" + margin.left + "," + margin.top + ")");
                
                
                var return_vs_dates = [];

                // ########## get market index data  指标数据？
                var market_index_raw = this.object2Array(this.data_backtest[0]["benchmark"]);
                var market_index_starting = market_index_raw[0][1];
                var market_index_return_common = market_index_raw.map(d => {
                    if(d[0] < that.end_date){
                        d[1] = (d[1] - market_index_starting)/market_index_starting;
                        return d;
                    }
                });
                market_index_return_common = market_index_return_common.filter(d=>{
                    /* filter() 方法创建一个新的数组，新数组中的元素是通过检查指定数组中符合条件的所有元素; => 函数表示 */
                    return d !== undefined;
                })
                var market_index_starting_reset = (market_index_return_common[market_index_return_common.length - 1][1] + 1) * market_index_starting;
                var market_index_return_extra = market_index_raw.map(d => {
                    if(d[0] >= that.end_date){
                        d[1] = (d[1] - market_index_starting_reset)/market_index_starting_reset;
                        return d;
                    }
                });
                market_index_return_extra = market_index_return_extra.filter(d=>{
                    return d !== undefined;
                })
                
                return_vs_dates.push({             /*接收任意数量的参数，把它们逐个添加到数组末尾，并返回修改后数组的长度*/
                    "data": market_index_return_common,
                    "color":"#7cb9f2",
                    "selected_color":"#7cb9f2",
                    "opacity":1,
                    "selected_opacity":1,
                    "name": "benchmark",
                    "selected": false,
                    "type":"common",
                });
                return_vs_dates.push({
                    "data": market_index_return_extra,
                    "name": "benchmark",
                    "color":"#7cb9f2",
                    "selected_color":"#7cb9f2",
                    "opacity":1,
                    "selected_opacity":1,
                    "selected": false,
                    "type":"extra",
                });
                // ########## get market index data
                
                var all_return = market_index_return_common.map(d => d[1]);      /*forEach和map是数组的两个方法，作用都是遍历数组：https://www.jb51.net/article/150865.htm*/
                all_return = all_return.concat(market_index_return_extra.map(d => d[1]));
                var all_date = [];
                
                let name_index = 0;
                this.data_backtest.forEach(ele => {
                    let raw = ele["account"];
                    let selected_stocks = ele["selected_stocks"];
                    let selected_factors = ele["selected_factors"];
                    all_date = all_date.concat(Object.keys(raw));
                    let price_vs_date = that.object2Array(raw);

                    let starting_value = raw[Object.keys(raw)[0]];
                    var return_vs_date = price_vs_date.map(d => {
                        if(d[0] < that.end_date){
                            d[1] = (d[1] - starting_value)/starting_value;
                            return d;
                        }
                    });
                    return_vs_date = return_vs_date.filter(d=>{
                        return d !== undefined;
                    })

                    var starting_value_reset = (return_vs_date[return_vs_date.length - 1][1] + 1) * starting_value;
                    var return_vs_date_extra = price_vs_date.map(d => {
                        if(d[0] >= that.end_date){
                            d[1] = (d[1] - starting_value_reset)/starting_value_reset;
                            return d;
                        }
                    });
                    return_vs_date_extra = return_vs_date_extra.filter(d=>{
                        return d !== undefined;
                    })

                    let summary_bt_text = "";
                    let summary = ele["summary"];
                    for(let i=0;i<summary_bt_fileds.length;i++){
                        let n = summary_bt_fileds[i].split("_").join(" ");
                        summary_bt_text += "\n" + n + ": " + summary[summary_bt_fileds[i]] ;
                    }
                    return_vs_dates.push({
                        "data": return_vs_date,
                        "summary_bt_text": summary_bt_text,
                        "selected": false,
                        "type":"common",
                        "color":"grey",
                        "selected_color":"red",
                        "opacity":0.6,
                        "selected_opacity":1,
                        "name": "bt_"+name_index,
                        "selected_stocks": selected_stocks,
                        "selected_factors": selected_factors,
                    });
                    return_vs_dates.push({
                        "data": return_vs_date_extra,
                        "summary_bt_text": summary_bt_text,
                        "selected": false,
                        "type":"extra",
                        "color":"grey",
                        "selected_color":"red",
                        "opacity":0.6,
                        "selected_opacity":1,
                        "name": "bt_"+name_index,
                        "selected_stocks": selected_stocks,
                        "selected_factors": selected_factors,
                    });
                    all_return = all_return.concat(return_vs_date.map(d => d[1]));
                    all_return = all_return.concat(return_vs_date_extra.map(d => d[1]));
                    name_index += 1;
                });

                var max_return = Math.max(...all_return);
                var min_return = Math.min(...all_return);

                // console.log("max_return:",max_return)
                // console.log("min_return:",min_return)

                var parseTime = d3.timeParse("%Y-%m-%d");
                var xScale = d3.scaleTime()
                    .domain(d3.extent(all_date, d => parseTime(d)))
                    .range([0, width * 0.98]);

                var padding = max_return * 0.05;

                var yScale = d3.scaleLinear()
                    .domain([min_return - padding, max_return + padding])
                    .range([height - 10, 10])
                    ;

                svg.append("g")          // 添加坐标轴底部
                    .attr("transform", "translate("+[0, height * Math.abs(max_return + padding) / (Math.abs(max_return + padding) + Math.abs(min_return - padding)) - 10]+")")
                    .call(d3.axisBottom(xScale).ticks(5).tickFormat(d3.timeFormat("%Y.%m")))
                    .selectAll("text")
                    // .attr("transform", "rotate(-45)")
                    ; 
                svg.append("g")         // 添加坐标轴左侧
                    .call(d3.axisLeft(yScale).tickFormat(d => `${d*100}%`).ticks(5))
                    .selectAll('text')
                    .attr('x',40)
                    ;

                var tooltip = d3.select('body')
                    .append("div")
                    .style("position", "absolute")
                    .style("visibility", "hidden")
                    .style("background", "lightsteelblue")
                    .style("opacity", "0.8")
                    .style("padding", "5px")
                    ;

                var liner = d3.line()
                    .curve(d3.curveMonotoneX)
                    .x(d => {
                        return xScale(parseTime(d[0]))
                    })
                    .y(d => yScale(d[1]));

                // draw lines for stocks
                var lines = svg.append("g")
                    .classed("lines", true)
                    .selectAll("path")

                lines.data(return_vs_dates)        /*鼠标操作，数据变化*/
                    .enter()
                    .append("path")
                    .merge(lines)
                    .attr("class",d=>{
                        return d.name;
                    })
                    .attr("d", d => liner(d.data))
                    .style("stroke-dasharray", d=>{
                        if(d.type == "extra"){
                            return ("4,2");
                        }
                    })
                    .attr("stroke", d=>{
                        return d.color;
                    })
                    .attr("opacity",d=>{
                        return d.opacity;
                    })
                    .attr("stroke-width", "2")
                    .attr("fill", "none")
                    .on("mouseover", function(d) {  /*鼠标移动触发事件*/
                        if (!d.selected) {
                            d3.select(that.$el).selectAll('.'+d.name).style("cursor", "pointer"); 
                            d3.select(that.$el).selectAll('.'+d.name).attr("stroke-width", '4').attr("opacity",d.selected_opacity)
                        }
                        tooltip.style("visibility", "visible");
                    })
                    .on("mouseout", function(d) {   /*鼠标离开触发事件*/
                        if (!d.selected) {
                            d3.select(that.$el).selectAll('.'+d.name).style("cursor", "default"); 
                            d3.select(that.$el).selectAll('.'+d.name).attr("stroke-width", '2').attr("opacity",d.opacity)
                        }
                        tooltip.style("visibility", "hidden");
                    })
                    .on("mousemove", function(d) {
                        var svg_rect = svg.node().getBoundingClientRect();
                        var x = event.pageX-svg_rect.left;
                        var y = event.pageY-svg_rect.top + 10;
                        
                        var tooltip_txt = "";
                        // if(d.name !== "benchmark"){
                            // tooltip_txt += d.selected_stocks + "<br/>" + d.selected_factors + "<br/>";
                            // tooltip.style("top", (event.pageY-80)+"px").style("left", (event.pageX - 150)+"px");
                        // }
                        // else{
                            tooltip.style("top", (event.pageY-40)+"px").style("left", (event.pageX - 150)+"px");
                        // }
                        /* 以下内容返回鼠标移动时所在的日期以及回报率 */
                        tooltip_txt += "date: " + d3.timeFormat("%Y.%m.%d")(xScale.invert(x)) + "<br/>" +
                            "return: " +`${(yScale.invert(y)*100).toFixed(1)}%`;
                        tooltip.html(tooltip_txt);
                    })
                    .on("click", function(d) {    /*鼠标点击，颜色宽度等变换*/
                        if (d.selected) {
                            d3.select(that.$el).selectAll('.'+d.name).attr("stroke-width", '4').attr("stroke", d.color);
                        }
                        else {
                            d3.select(that.$el).selectAll('.'+d.name).attr("stroke-width", '2').attr("stroke", d.selected_color);
                        }
                        d.selected = !(d.selected)
                    })
                    // .each(function(d) {
                    //     d3.select(this).append("title")
                    //         .classed("tooltip", true)
                    //         .text(d => {
                    //             // return "Backtest Return: \n" + d.selected_stocks + "\n" + d.selected_factors + d.summary_bt_text;
                    //             return "Backtest Return: \n" + d.selected_stocks + "\n" + d.selected_factors;
                    //         });
                    // })
                    ;
                
                // ###### legend - start
                var legend_width = 25;
                var legend_height = 12;
                var legend_margin_left = bounding_rect.width - 5 * legend_width - margin.right;
                var legend = svg.append("g")   /* 模块： 右上角backtest&market index */
                    .selectAll('.legend')
                    .data(["Backtest", "Market Index"])
                    .enter()
                    .append('g')
                    .attr("class","legend")
                    .attr("transform","translate(" + legend_margin_left + "," + (margin.top + legend_height) + ")")
                    ;

                legend.append('line')
                    .attr("x1", 0)
                    .attr("x2", legend_width + "px")
                    .attr("y1", (d,i)=>{
                        return i * legend_height * 1.1 + "px"; 
                    })
                    .attr("y2", (d,i)=>{
                        return i * legend_height * 1.1 + "px"; 
                    })
                    .attr("stroke-width", 2)
                    .attr("stroke", function(d) { 
                        return {
                            "Backtest": "grey",
                            "Market Index": "#7cb9f2"
                        }[d];
                    })
                ;

                legend.append('text')         /* 用于对齐（开始，中间或结束对齐）相对于给定点的文本字符串 */
                    .attr("x", 1.5 * legend_width + "px")
                    .attr("y", (d,i)=>{
                        return i * legend_height * 1.1 + 0.4 * legend_height + "px";
                    })
                    .text(function (d, i) {
                        return d;
                    })
                    .style("text-anchor", "start")
                    .style("font-size", 1.1 * legend_height + "px");
                // ###### legend - end

            },
        }
    }
</script>