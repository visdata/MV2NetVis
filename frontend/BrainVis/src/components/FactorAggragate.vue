<template>
    <div >
        <div class="frame_header">
            <!-- <label style="font-weight: bold;">Aggragate</label> -->
            <label style="width:100%;text-align:left;">Factor List</label>
            <label style="width:100%;padding-right:5%;text-align:right;">Factors</label>
        </div>

        <div id="bars" class="draw-area" style="float:left;width:79%;"
            :style="{'height': (factor_aggragate_height - frame_header_height) + 'px'}"
        >
            <svg></svg>
        </div>

        <div style="float:left;width:21%; overflow:auto; margin-top:-2px;"
            :style="{'height': (factor_aggragate_height - frame_header_height - 32) + 'px'}"
        >
            <v-table
                is-horizontal-resize
                is-vertical-resize
                :title-row-height=0
                :row-click="del_table_row"
                style="width:100%;"
                :columns="columns"
                :table-data="tableData"
                row-hover-color="#eee"
            ></v-table>
        </div>
        <div style="float:left;width:21%;overflow:hidden">
            <ui-button 
                color="primary" 
                size="small" 
                icon="arrow_downward"
                iconPosition="right"
                :disabled="get_disabled_draw_btn"
                @click="getDataBunchBacktest"
                style="width:100%;height:32px"
                >
                draw
            </ui-button>
        </div>
    </div>
</template>

<script>
    import * as d3 from "d3";
    import { UiButton } from 'keen-ui';
    import 'vue-easytable/libs/themes-base/index.css'
    import { VTable } from 'vue-easytable'
    import { mapState } from 'vuex';
    import NetService from '@/services/net-service';
    import C from './constants.js';

    export default{
        name: "FactorAggragate",
        components: {
            UiButton,
            VTable,
        },
        data(){
            return {
                disabled_draw_btn: false,
                svg_width_percent: 0.8,
                columns: [
                    {field: 'factor', title: 'Factors', width: 50, titleAlign: 'center', columnAlign: 'center',isResize:true},
                ],
                frame_header_height: C.FRAME_HEADER_HEIGHT,
            };
        },
        computed: {
            ...mapState(['start_date', 'end_date', 'stock_list','width','height','selected_factors', 'data_aggragate']),
            width_or_height(){
                return [this.width, this.height];
            },
            get_disabled_draw_btn(){
                return this.disabled_draw_btn;
            },
            tableData(){
                let res = [];
                for(let i=0;i<this.selected_factors.length;i++){
                    res.push({"factor": this.selected_factors[i]});
                }
                return res;
            },
            factor_aggragate_height(){
                return Math.floor((this.height - 15) * 0.41);
            },
        },
        watch:{
            data_aggragate: 'draw_aggragate',
            width_or_height: 'draw_aggragate',
            selected_factors: 'draw_aggragate',
            deep: true,
            immediate: true
        },
        mounted(){

        },
        methods:{
            del_table_row(rowIndex, rowData, column){
                this.$store.commit('rm_selected_factor', rowData["factor"]);
            },

            getDataBunchBacktest(){
                var that = this;
                new Promise(function(resolve){
                    that.disabled_draw_btn = true;
                    // action
                    let start_date = that.start_date;
                    let end_date = that.end_date;
                    let factors_list = that.selected_factors;
                    let kwargs = {};
                    for(let i in that.stock_list){
                        let ts_code = that.stock_list[i]['ts_code'];
                        let industry = that.stock_list[i]['industry'];
                        kwargs[industry + '_' + i] = ts_code
                    }
                    for(let i in factors_list){
                        let factor = factors_list[i];
                        kwargs['factor_' + i] = factor;
                    }
                    kwargs["prediction_model"] = 'ols';
                    NetService.get_bunch_single_backtest(start_date, end_date, kwargs, (res, err) => {
                        if(err){
                            console.error(err);
                            resolve();
                            return
                        }
                        if(!res.data){
                            console.log('data not exists!');
                            resolve();
                            return
                        }
                        // console.log("test bunch backtest: ",res.data);
                        that.$store.commit('draw_bunch_backtest', res.data);
                        that.$store.commit('reset_selected_stocks');
                        resolve()
                    });
                }).then(function(){
                    that.disabled_draw_btn = false;
                })
            },

            draw_aggragate(){
                // console.log('data_aggragate:', this.data_aggragate);
                let rawdata = this.data_aggragate;
                if(rawdata === undefined){
                    return;
                }

                var root_div = d3.select(this.$el);
                var bounding_rect = root_div.node().getBoundingClientRect()

                var margin = {
                    top: 5,
                    right: 15,
                    bottom: 10,
                    left: 5
                },
                width = bounding_rect.width * this.svg_width_percent - margin.left - margin.right,
                // adjustable
                // height = 600 - margin.top - margin.bottom;
                height = bounding_rect.height - margin.top - margin.bottom - C.FRAME_HEADER_HEIGHT;
                
                d3.select(this.$el)
                    .select(".draw-area")
                    .select("svg")
                    .remove();

                var svg = d3.select(this.$el)
                    .select(".draw-area")
                    .append("svg")
                    .attr("width", width + margin.left + margin.right)
                    .attr("height", height + margin.top + margin.bottom )
                    .append("g")
                    .attr("transform", "translate(0," + margin.top + ")");

                // var tooltip = d3.select("body").append("div")
                //     .attr("class", "tooltip")
                //     .style("opacity", 0);

                var allValues = [];
                var data = rawdata['negative'];
                var bandNum = -1;
                Object.keys(data).forEach((key) => {
                    allValues = allValues.concat(d3.min(Object.values(data[key])));
                    bandNum = Math.max(bandNum, Object.keys(data[key]).length);
                });
                var bandWidth = width / bandNum / 2;
                var data = rawdata['positive'];
                Object.keys(data).forEach((key) => {
                    allValues = allValues.concat(d3.max(Object.values(data[key])));
                });

                var pad = height/12;
                var yScale = d3.scaleLinear()
                        .domain([0, d3.max(d3.extent(allValues).map(d => Math.abs(d)))])
                        .range([pad, 0]);
                
                var fill = d3.scaleOrdinal()
                        .domain(["A", "B", "C", "D", "E", "F"])
                        .range(["#84a7b2", "#0047B6", "#CD6600", "#3690c0", "#EEAD0E", "#996699"]);
                
                // var legends = {
                //     "transaction_friction_factor": "Transaction",
                //     "momentum_factor": "Momentum",
                //     "value_factor": "Value",
                //     "growth_factor": "Growth",
                //     "profit_factor": "Profit",
                //     "financial_liquidity_factor": "Liquidity",
                // };

                // var legend = svg.append("g").selectAll('.legend')
                //         .data(Object.values(legends))
                //         .enter()
                //         .append('g')
                //         .attr("class", "legends")
                //         .attr("transform", (d, i) => "translate(0," + (pad + 2 * i * pad - 10) + ")");
                    
                // legend.append('rect')
                //     .attr("x", 0)
                //     .attr("y", 0)
                //     .attr("width", 15)
                //     .attr("height", 15)
                //     .style("fill", (d, i) => fill(i))
                //     .on('mouseover',function(d) {
                //         d3.select(this)
                //             .style("cursor", "pointer");
                //         // tooltip.transition()
                //         //     .duration(200)
                //         //     .style("opacity", .9);
                //         // tooltip.html(d)
                //         //     .style("left", (d3.event.pageX) + "px")
                //         //     .style("top", (d3.event.pageY - 28) + "px");
                //     })
                //     .on('mouseout', function() {
                //         d3.select(this)
                //             .style("cursor", "default");
                //         // tooltip.transition()
                //         //     .duration(500)
                //         //     .style("opacity", 0);
                //     })
                //     .each(function(d){
                //         d3.select(this).append("title")
                //             .classed("tooltip", true)
                //             .text(function(d) { return d});
                //     });

                var that = this;

                Object.keys(rawdata).forEach((type, typeIdx) => {
                    var data = rawdata[type];

                    Object.keys(data).forEach((key, idx) => {
                        var xScale = d3.scaleBand()
                            .domain(Object.keys(data[key]))
                            .range([10, width])
                            .padding(1);
                        
                        var classData = [];
                        Object.keys(data[key]).forEach((factor) => {
                            var temp = {};
                            temp.name = factor;
                            temp.value = data[key][factor];
                            classData.push(temp);
                        });
                        
                        svg.append('g')
                            .selectAll(".bar")
                            .data(classData)
                            .enter()
                            .append("rect")
                            .attr("class", d => d.name)
                            .attr("x", d => xScale(d.name)- bandWidth/2)
                            .attr("width", bandWidth)
                            .attr("y", d =>  d.value > 0? yScale(d.value) + 2 * idx * pad:pad + 2 * idx * pad) 
                            .attr("height", d => pad - yScale(Math.abs(d.value)))
                            // .style('fill',d=>{
                            //     if(that.selected_factors.indexOf(d.name) >= 0){
                            //         return d3.rgb(fill(key)).darker(2);
                            //     }
                            //     else{
                            //         return fill(key);
                            //     }
                            // })
                            .style('fill', fill(key))
                            .on('click',function clickevent() {
                                var factor_name = d3.select(this).attr("class");
                                if(that.selected_factors.indexOf(factor_name) >= 0){
                                    d3.select(that.$el).selectAll("."+factor_name).style("stroke", "none");
                                    // d3.select(this).style('fill',fill(key));
                                    that.$store.commit('rm_selected_factor', factor_name);
                                } else {
                                    d3.select(that.$el).selectAll("."+factor_name)
                                        .style("stroke", 'red')
                                        // .style("stroke", d3.rgb(fill(key)).darker(1))
                                        .style("stroke-width", 2)
                                        .style("stroke-opacity", 1);
                                    // d3.select(this).style('fill', d3.rgb(fill(key)).darker(2));
                                    that.$store.commit('add_select_factor', factor_name);
                                }
                            })
                            .on('mouseover', function over() {     
                                var factor_name = d3.select(this).attr("class");
                                if(that.selected_factors.indexOf(factor_name) >= 0){
                                    d3.select(this)
                                        .style("cursor", "pointer");
                                } else {
                                    d3.select(this)
                                        .style("cursor", "pointer") 
                                        .style("stroke", 'red')
                                        // .style("stroke", d3.rgb(fill(key)).darker(1))
                                        .style("stroke-width", 2)
                                        .style("stroke-opacity", 1);
                                }
                                // tooltip.transition()
                                //     .duration(200)
                                //     .style("opacity", .9);
                                // tooltip.html(d.value.toFixed(4))
                                //     .style("left", (d3.event.pageX) + "px")
                                //     .style("top", (d3.event.pageY - 28) + "px");
                            })
                            .on('mouseout', function out() {
                                var factor_name = d3.select(this).attr("class");
                                if(that.selected_factors.indexOf(factor_name) >= 0){
                                    d3.select(this)
                                        .style("cursor", "default");
                                } else {
                                    d3.select(this)
                                        .style("cursor", "default")
                                        .style("stroke", "none");
                                }
                                // tooltip.transition()
                                //     .duration(500)
                                //     .style("opacity", 0);
                            })
                            .each(function(d){
                                d3.select(this).append("title")
                                    .classed("tooltip", true)
                                    .text(function(d) { return d.value.toFixed(2) + '%' });
                            });

                        if(typeIdx == 0){
                            svg.append("g")
                                .attr("transform", "translate(0," + (pad + 2 * idx * pad) + ")")
                                .call(d3.axisBottom(xScale))
                                .selectAll("text") 
                                .style("text-anchor", "middle")
                                .attr('x', d => {
                                    // return data[key][d] > 0? -5 : 20;
                                    return 0;
                                })
                                .attr('y', d =>{
                                    var posvalue = rawdata['positive'][key][d];
                                    var negvalue = Math.abs(rawdata['negative'][key][d]);
                                    if(posvalue > negvalue) {
                                        return pad / 2
                                    }
                                    else {
                                        return -(pad / 1.5)
                                    }
                                    // return data[key][d] > 0? (pad / 2) : -(pad / 1.5)
                                })
                                // .attr('transform', 'rotate(-65)')
                                .attr("transform", "rotate(-15)")
                        }

                    });

                });

                this.selected_factors.forEach(d=>{
                    // console.log(d);
                    d3.select(that.$el)
                        .selectAll("."+d)
                        .style('stroke', 'red')
                        // .style("stroke", () => {
                        //     var returncolor;
                        //     Object.keys(data).forEach((key) => {
                        //         if (Object.keys(data[key]).indexOf(d) >= 0){
                        //             returncolor = d3.rgb(fill(key)).darker(1);
                        //             return;
                        //         }
                        //     });
                        //     return returncolor;
                        // })
                        .style("stroke-width", 2)
                        .style("stroke-opacity", 1);
                });
                
            }
        }
    }
</script>

<style>
    /* div.tooltip {
      position: absolute;
      text-align: center;
      width: 80px;
      height: 25px;
      padding: 2px;
      font: 12px sans-serif;
      background: lightsteelblue;
      border: 0px;
      border-radius: 8px;
      pointer-events: none;
    } */
</style>
