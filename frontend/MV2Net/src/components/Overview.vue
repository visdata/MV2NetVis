<template>
    <div>
        <div class="frame_header">
            <label>Factor View</label>
            <div style="
                padding-left: 50px;
                display:inline;
                ">
                <label style="margin-right:10px">Importance</label>
                <div class="custom-control custom-radio custom-control-inline">
                    <input 
                        type="radio" 
                        class="custom-control-input" 
                        id="importance_contri" 
                        :checked="importance_selection=='importance_contri'" 
                        @click="changeRadioSelection('importance_contri')"
                        name="importance_radio_btn_group">
                    <label class="custom-control-label" for="importance_contri">contribution</label>
                </div>
                <div class="custom-control custom-radio custom-control-inline">
                    <input type="radio" class="custom-control-input" id="importance_w" 
                        :checked="importance_selection=='importance_w'" 
                        @click="changeRadioSelection('importance_w')"
                        name="importance_radio_btn_group">
                    <label class="custom-control-label" for="importance_w">weight</label>
                </div>
                <div class="custom-control custom-radio custom-control-inline">
                    <input type="radio" class="custom-control-input" 
                        id="importance_v" 
                        :checked="importance_selection=='importance_v'" 
                        @click="changeRadioSelection('importance_v')"
                        name="importance_radio_btn_group">
                    <label class="custom-control-label" for="importance_v">value</label>
                </div>
            </div>
            <div style="
                display:inline;
                margin-left: 30px;
                ">
                <label for="cirlce_range_slider">Filter</label>
            </div>
            <div style="
                display:inline;
                margin-left: 20px;
                margin-top: 6px;
                ">
                <input type="range" @change="changeSlider" class="custom-range" id="cirlce_range_slider" value="0">
            </div>
            <label>>={{slider_value}}%</label>
        </div>
        <div class="draw-area">
            <svg></svg>
        </div>
    </div>
</template>

<script>
    import { mapState } from 'vuex';
    import $ from 'jquery';
    import C from './constants.js';
    const fix_prompt_height = 20, fix_prompt_top = 10;
    export default{
        name: "Overview",
        data(){
            return {
                importance_selection: 'importance_contri',
                slider_value: 0,
            };
        },
        computed: {
            ...mapState(['width','height','selected_factors']),
            data_overview(){
                return this.$store.state.data_overview;
            },
            width_or_height(){
                return [this.width, this.height];
            }
        },
        mounted(){

        },
        watch:{
            data_overview: 'draw_overview',
            width_or_height: 'draw_overview',
            selected_factors: 'draw_overview',
            deep: true,
            immediate: true
        },
        methods:{
            changeSlider(){
                this.slider_value = $('#cirlce_range_slider').val();
                this.draw_overview();
            },

            changeRadioSelection(radio_selection){
                this.importance_selection=radio_selection;
                this.draw_overview();
            },

            draw_overview(){
                let data = this.data_overview;
                if(data === undefined){
                    return;
                }
                var percent_circle = this.slider_value / 100;

                var radio_selection = undefined;
                switch(this.importance_selection){
                    case 'importance_contri': radio_selection = "contri"; break;
                    case 'importance_w': radio_selection = "weight"; break;
                    case 'importance_v': radio_selection = "value"; break;
                };
                var selection_positive = radio_selection + "_positive",
                    selection_negative = radio_selection + "_negative";

                var d3 = d3v3;
                var abs_contri_max = 0, sensitivity_max = 0;
                var abs_contri_list = [];
                data.map(item=>{
                    for(let d in item){
                        if(d !== "date"){
                            let abs_contri_positive = Math.abs(item[d][selection_positive]);
                            let abs_contri_negative = Math.abs(item[d][selection_negative]);
                            if(abs_contri_positive > 0){
                                abs_contri_list.push(abs_contri_positive);
                            }
                            if(abs_contri_negative > 0){
                                abs_contri_list.push(abs_contri_negative);
                            }
                            
                            abs_contri_max = abs_contri_positive > abs_contri_max? abs_contri_positive: abs_contri_max;
                            abs_contri_max = abs_contri_negative > abs_contri_max? abs_contri_negative: abs_contri_max;
                            sensitivity_max = item[d].sensitivity > sensitivity_max? item[d].sensitivity: sensitivity_max;
                        }
                    }
                });
                abs_contri_list.sort(function(a, b){
                    return a - b; 
                });

                // console.log(abs_contri_list);

                var positive_r_sum_max = 0, negative_r_sum_max = 0;
                var minimal_contri = abs_contri_list[Math.floor(abs_contri_list.length * percent_circle)];
                // console.log("percent_circle: ",percent_circle);
                // console.log("minimal_contri: ",minimal_contri);
                data.map(item=>{
                    let positive_r_sum = 0,
                        negative_r_sum = 0;
                    for(let d in item){
                        if(d !== "date"){
                            let abs_contri_positive = Math.abs(item[d][selection_positive]);
                            let abs_contri_negative = Math.abs(item[d][selection_negative]);
                            abs_contri_positive = abs_contri_positive < minimal_contri ? 0: abs_contri_positive;
                            positive_r_sum += Math.sqrt(abs_contri_positive / Math.PI);
                            abs_contri_negative = abs_contri_negative < minimal_contri ? 0: abs_contri_negative;
                            negative_r_sum += Math.sqrt(abs_contri_negative / Math.PI);
                        }
                    }
                    positive_r_sum_max = positive_r_sum > positive_r_sum_max? positive_r_sum: positive_r_sum_max;
                    negative_r_sum_max = negative_r_sum > negative_r_sum_max? negative_r_sum: negative_r_sum_max;
                });

                // console.log("positive_r_sum_max: ",positive_r_sum_max);
                // console.log("negative_r_sum_max: ",negative_r_sum_max);
                var root_div = d3.select(this.$el);
                var bounding_rect = root_div.node().getBoundingClientRect()

                var margin = { top: 25, right: 0, bottom: 25, left: 10 },
                    legends = [
                        "transaction_friction_factor",
                        "momentum_factor",
                        "value_factor",
                        "growth_factor",
                        "profit_factor",
                        "financial_liquidity_factor",
                    ],
                    legend_shorten = {
                        "transaction_friction_factor": "Transaction",
                        "momentum_factor": "Momentum",
                        "value_factor": "Value",
                        "growth_factor": "Growth",
                        "profit_factor": "Profit",
                        "financial_liquidity_factor": "Liquidity",
                    },
                    legends_width = bounding_rect.width / 12,
                    legends_fond_size = legends_width / 7,

                    // tick_size = 30,
                    // width = tick_size * data.length - margin.left - margin.right,

                    // width = bounding_rect.width - legends_width - margin.left - margin.right,
                    x_axis_height = 15,
                    x_axis_top_padding = 5,
                    x_axis_bottom_padding = 20,
                    width = bounding_rect.width  - margin.left - margin.right,
                    height = bounding_rect.height - margin.top - margin.bottom - C.FRAME_HEADER_HEIGHT,
                    height_for_circles = height - x_axis_height - legends_fond_size,
                    height_for_positive_circles = height_for_circles * positive_r_sum_max / (positive_r_sum_max + negative_r_sum_max),
                    height_for_negative_circles = height_for_circles * negative_r_sum_max / (positive_r_sum_max + negative_r_sum_max)
                    ;

                var opacity_scale = d3.scale
                    .linear()
                    .domain([0, sensitivity_max])
                    .range([C.MIN_OPACITY, C.MAX_OPACITY]);

                var parse = d3.time.format("%Y-%m-%d").parse;

                var r_scale = d3.scale
                    .linear()
                    .domain([0, positive_r_sum_max + negative_r_sum_max])
                    .range([0, height_for_circles])
                    ;
                
                var colors = d3.scale.ordinal()
                    .domain(legends)
                    .range(C.FACTOR_TYPE_COLORMAP);
                
                d3.select(this.$el)
                    .select(".draw-area")
                    .select("svg")
                    .remove();

                var root_svg = d3
                    .select(this.$el)
                    .select(".draw-area")
                    .append("svg")
                    // .attr("width", width + margin.left + margin.right + legends_width)
                    .attr("width", width + margin.left + margin.right)
                    .attr("height", height + margin.top + margin.bottom)
                    ;
                
                // #### Linear Gradient
                var paddings = [
                    0, width / 5.9, width / 5.9, width / 6.3, width / 6.4, width / 6.6, 
                ];

                var legend = root_svg.append("g")
                    .selectAll(".legend")
                    .data(legends)
                    .enter()
                    .append("g")
                    .attr("class", "legend")
                    .attr("transform", function(d,i){return "translate(" + (margin.left + width / 25 + i * paddings[i]) + "," + legends_fond_size + ")"});

                legend.append('defs').append('linearGradient')
                    .attr('id', (d)=>{
                        return 'legend_' + d; 
                    });

                for(let i=0; i<legends.length;i++){
                    let gradient = legend.select("#legend_" + legends[i]);
                    gradient.append('stop')
                        .style('stop-color', C.FACTOR_TYPE_COLORMAP[i])
                        .attr('offset', '0');
                    gradient.append('stop')
                        .style('stop-color', C.FACTOR_TYPE_COLORMAP_MIN_OPACITY[i])
                        .attr('offset', '1');
                    ;
                }
                legend.append('rect')
                    .style("fill", (d)=>{
                        return "url(#"+ 'legend_' + d + ")"
                    })
                    .attr('x', 0)
                    .attr('y', 0)
                    .attr('width', legends_width)
                    .attr('height', legends_fond_size);
                
                legend.append('text')
                    .attr("x", (d,i)=>{
                        return legends_width * 1.05;
                    })
                    .attr("y", legends_fond_size)
                    .text(function (d, i) {
                        return legend_shorten[d];
                    })
                    .attr("class", "textselected")
                    .style("text-anchor", "start")
                    .style("font-size", 1.1 * legends_fond_size);
                // #### Linear Gradient


                // drawing circles
                var svg = root_svg.append("g")
                    .attr("transform", "translate(" + margin.left + "," + (margin.top + legends_fond_size) + ")");

                var all_data = [];
                var dates_set = new Set();

                // Transpose the data into layers
                d3.layout.stack()(
                    [
                        'QRG', 'CRG', 'saleinv', 'salecash', 'CFdebt', 'QR', 'CR', 'RDsales', 'RD', 'cashpr', 
                        'cash', 'CT', 'PA', 'ROA', 'ROE', 'ACCP', 'ACC', 'TAXchg', 'PMG', 'SgINVg', 'SG', 'INVchg',
                        'INVG', 'BVEG', 'LG', 'AG', 'SP', 'DP', 'OCFP', 'CFP', 'EP', 'LEV', 'AM', 'BM', 'lagretn', 
                        'imom', 'momchg', 'mom6', 'mom12', 'age', 'sharechg', 'LM', 'illq', 'retnmax', 'std_dvol', 
                        'volumed', 'std_turn', 'turn', 'coskew', 'skew', 'idskew', 'total_vol', 'idvol', 'betad', 'beta', 
                        'size'
                    ]
                    .map(function (factor) {
                        return data.map(function (d) {
                            // console.log(d[factor]);
                            dates_set.add(d.date);
                            if(d[factor] !== undefined){
                                if (d[factor][selection_positive] > 0) {
                                    let contri = Math.abs(d[factor][selection_positive]);
                                    contri = contri < minimal_contri ? 0: contri;
                                    let res = { 
                                        x: parse(d.date), 
                                        dt: d.date,
                                        v: d[factor][selection_positive],
                                        y: r_scale(Math.sqrt(contri / Math.PI)), 
                                        s: +1, 
                                        t: d[factor]["type"], 
                                        l: factor,
                                        sen: d[factor]["sensitivity"]
                                    };
                                    all_data.push(res);
                                    return res;
                                } else {
                                    return { x: parse(d.date), y:0};
                                }
                            }
                            return { x: parse(d.date), y:0};
                        });
                    })
                );

                var dataset_negative = d3.layout.stack()(
                    [
                        "size","beta","betad","idvol","total_vol","idskew","skew","coskew","turn","std_turn",
                        "volumed","std_dvol","retnmax","illq","LM","sharechg","age","mom12","mom6","momchg",
                        "imom","lagretn","BM","AM","LEV","EP","CFP","OCFP","DP","SP","AG","LG","BVEG","INVG",
                        "INVchg","SG","SgINVg","PMG","TAXchg","ACC","ACCP","ROE","ROA","PA","CT","cash","cashpr",
                        "RD","RDsales","CR","QR","CFdebt","salecash","saleinv","CRG","QRG"
                    ]
                    .map(function (factor) {
                        return data.map(function (d) {
                            // console.log(d[factor]);
                            if(d[factor] !== undefined){
                                if (d[factor][selection_negative] < 0) {
                                    let contri = Math.abs(d[factor][selection_negative]);
                                    contri = contri < minimal_contri ? 0: contri;
                                    let res = { 
                                        x: parse(d.date), 
                                        dt: d.date,
                                        v: d[factor][selection_negative],
                                        y: r_scale(Math.sqrt(contri / Math.PI)), 
                                        s: -1, 
                                        t: d[factor]["type"], 
                                        l: factor,
                                        sen: d[factor]["sensitivity"]
                                    };
                                    all_data.push(res);
                                    return res;
                                } else {
                                    return { x: parse(d.date), y: 0};
                                }
                            }
                            return { x: parse(d.date), y: 0};
                        });
                    })
                );

                dates_set = [...dates_set];
                dates_set.sort();

                var y_positive = d3.scale
                    .linear()
                    // .domain([0, height / 2])
                    .domain([0, height_for_positive_circles])
                    .range([height_for_positive_circles, 0]);
                
                var y_negative = d3.scale
                    .linear()
                    .domain([0, height_for_negative_circles])
                    .range([height - height_for_negative_circles - legends_fond_size, height - legends_fond_size]);

                // Set x, y and colors
                var x = d3.scale
                    .ordinal()
                    .domain(dates_set.map(function (d) {
                        return parse(d);
                    }))
                    .rangeRoundBands([20, width - 20]);

                var xAxis = d3.svg
                    .axis()
                    .scale(x)
                    .orient("bottom")
                    .tickFormat(d3.time.format("%m"))
                    ;
                
                var xAxis_year = d3.svg
                    .axis()
                    .scale(x)
                    .orient("bottom")
                    .tickFormat(d3.time.format("%Y"))
                    .tickSize(0)
                    ;

                svg
                    .append("g")
                    .attr("class", "axis_overview")
                    // .attr("transform", "translate(0," + height / 2 + ")")
                    .attr("transform", "translate(0," + height_for_positive_circles + ")")
                    .call(xAxis)
                    .selectAll("text")
                    // .style("text-anchor", "end")
                    // .attr("dx", "-.8em")
                    // .attr("dy", ".15em")
                    // .attr("transform", "rotate(-25)")
                    ;
                svg
                    .append("g")
                    .attr("class", "axis_overview")
                    // .attr("transform", "translate(0," + height / 2 + ")")
                    .attr("transform", "translate(0," + (height_for_positive_circles + x_axis_bottom_padding)  + ")")
                    .call(xAxis_year)
                    .selectAll("text")
                    .each(function (d,i) { 
                        if(i !== 0 && d.getMonth() !== 0){
                            d3.select(this).remove();
                        }
                    });
                
                // For connect
                var lines = svg.append("g")
                    .classed("lines", true);
                var all_lines = [];
                var add_line = (factor_name)=>{
                    let points_positive = [];
                    let exist_dts_positive = new Set();
                    svg.selectAll('circle.'+factor_name)
                        .each((d,i)=>{
                            if(d.s > 0 && d.y !== 0){
                                // console.log("d.x: ", d.x);
                                // console.log("d.y: ", d.y);
                                // console.log("d.y0: ", d.y0);
                                exist_dts_positive.add(d.dt);
                                let x_temp = x(d.x) + x.rangeBand() / 2;
                                let y_temp = y_positive(d.y0 + d.y / 2 + x_axis_top_padding);
                                points_positive.push([x_temp, y_temp]);
                            }
                        })
                    ;
                    dates_set.forEach((dt)=>{
                        if(! exist_dts_positive.has(dt)){
                            let x_temp = x(parse(dt)) + x.rangeBand() / 2;
                            let y_temp = y_positive(0);
                            points_positive.push([x_temp, y_temp]);
                        }
                    })

                    let points_negative = [];
                    let exist_dts_negative = new Set();
                    svg.selectAll('circle.'+factor_name)
                        .each((d,i)=>{
                            if(d.s < 0 && d.y !== 0){
                                let x_temp = x(d.x) + x.rangeBand() / 2;
                                exist_dts_negative.add(d.dt);
                                let y_temp = y_negative(d.y0 + d.y / 2 + x_axis_bottom_padding);
                                points_negative.push([x_temp, y_temp]);
                            }
                        })
                    ;
                    dates_set.forEach((dt)=>{
                        if(! exist_dts_negative.has(dt)){
                            let x_temp = x(parse(dt)) + x.rangeBand() / 2;
                            let y_temp = y_negative(0);
                            points_negative.push([x_temp, y_temp]);
                        }
                    })

                    points_positive.sort(function(a, b){return a[0] - b[0]});
                    points_negative.sort(function(a, b){return a[0] - b[0]});
                    all_lines.push(points_positive);
                    all_lines.push(points_negative);
                };
                var draw_lines = (all_lines)=>{
                    lines.selectAll(".line")
                        .remove();
                    lines.selectAll(".line")
                        .data(all_lines)
                        .enter()
                        .append("path")
                        .classed("line", true)
                        .attr("d", d3.svg.line()
                                    .x(function(d) { return d[0] }) 
                                    .y(function(d) { return d[1] }) 
                                    .interpolate("monotone")
                        )
                        .attr("stroke", (d,i)=>{
                            if(i%2==0){
                                return "red";
                            }
                            else{
                                return "green";
                            }
                        })
                        .attr("stroke-width", "1.5")
                        .attr("fill", "none")
                    ;
                };
                
                var update = svg
                    .selectAll('circle')
                    .data(all_data);
                var enter = update.enter();
                var exit = update.exit();

                var that = this;

                enter.append('circle')
                    .style('fill', function(d) {
                        return colors(d.t);
                    })
                    .attr('cx', function(d) {
                        return x(d.x) + x.rangeBand() / 2;
                    })
                    .attr('cy', function(d) {
                        // console.log(d);
                        if(d.s > 0){
                            return y_positive(d.y0 + d.y / 2 + x_axis_top_padding);
                        }
                        else{
                            return y_negative(d.y0 + d.y / 2 + x_axis_bottom_padding)
                        }
                    })
                    .attr('r', function(d) {
                        return d.y / 2;
                    })
                    .attr('class', function(d){
                        return d.l;
                    })
                    .style("fill-opacity", function(d){
                        return opacity_scale(Math.sqrt(d.sen));
                    })
                    .on("mouseover", function(d) {
                        // let factor_name = d3.select(this).attr("class");
                        let factor_name = d.l;
                        d3.select(this)
                            .style("cursor", "pointer") 
                            .style("stroke", "red")
                            .style("stroke-width", 2)
                            .style("stroke-opacity", 1)
                            ;
                        add_line(factor_name);
                        draw_lines(all_lines);
                    })
                    .on("mouseout", function(d) {
                        d3.select(this)
                            .style("stroke", "none")
                            .style("cursor", "default")
                            ;
                        while(all_lines.length > that.selected_factors.length * 2){
                            all_lines.pop();
                        }
                        draw_lines(all_lines);
                    })
                    .on("click", function(d){
                        let factor_name = d.l;
                        let isin = false;
                        for(let i=0;i<that.selected_factors.length;i++){
                            if(that.selected_factors[i] === factor_name){
                                isin = true;
                                break;
                            }
                        }
                        if(isin){
                            that.$store.commit('rm_selected_factor', d.l);
                        }
                        else{
                            that.$store.commit('add_select_factor', d.l);
                        }
                    })
                    .each(function(d){
                        d3.select(this).append("title")
                            .classed("tooltip", true)
                            .text(function(d) { 
                                let txt = d.l + '(' + legend_shorten[d.t] + '): ' + d.v.toFixed(2)
                                if(radio_selection === "contri"){
                                    txt += "%";
                                }
                                return txt;
                            });
                    })
                ;

                for(let i = 0;i < this.selected_factors.length;i++){
                    let factor_name = this.selected_factors[i];
                    add_line(factor_name);
                }
                draw_lines(all_lines);

                // ========== draw legends
                // var pad = height / 12;
                // var legend = svg.append("g").selectAll('.legend')
                //         .data(legends)
                //         .enter()
                //         .append('g')
                //         .attr("class", "legends")
                //         .attr("transform", function (d, i) {
                //         {
                //             return "translate("+ width + "," + (pad + 2 * i * pad - 10) + ")"
                //         }
                //     })
                    
                // legend.append('rect')
                //     .attr("x", 0)
                //     .attr("y", 0)
                //     .attr("width", legends_fond_size)
                //     .attr("height", legends_fond_size)
                //     .style("fill", function (d, i) {
                //         return colors(i)
                // })
                
                // legend.append('text')
                //     .attr("x", 2 * legends_fond_size)
                //     .attr("y", legends_fond_size)
                //     //.attr("dy", ".35em")
                //     .text(function (d, i) {
                //         // let parts = d.split("_");
                //         // return parts.slice(0, parts.length - 1).join(" ");
                //         return legend_shorten[d];
                //     })
                //     .attr("class", "textselected")
                //     .style("text-anchor", "start")
                //     .style("font-size", 1.1 * legends_fond_size);

            },
        }
    }
</script>

<style>
    svg {
        font: 10px sans-serif;
        shape-rendering: crispEdges;
    }

    .axis_overview path {
        fill: none;
        stroke: #000;
    }

    .axis_overview line {
        fill: none;
        stroke: #000;
    }

    .axis_overview path.domain {
        stroke: none;
    }

    .ui-collapsible__body{
        padding: 0%;
    }

    .ui-collapsible__header {
        font-size: 18px;
    }
</style>
