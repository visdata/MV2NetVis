<template>
  <div style="overflow-x:hidden;width:100%">
    <div style="overflow-x:hidden" class="draw-area">
      <svg>

      </svg>
    </div>
  </div>
</template>


<script>
import { mapState } from 'vuex';
import * as d3 from 'd3'

export default {
  name: 'ReturnLines',
  props: {
    input_data: {
      type: Object,
    },
  },
  computed: {
    ...mapState(['selected_stocks']),
  },
  watch: {
    selected_stocks: "updateLinesColor"
  },
  data() {
    return {
      stock_name: "",
    }
  },
  mounted() {
    this.$nextTick(function foo() {
      this.initialize();
      this.checkStockSelection();
    })
  },
  methods: {
    object2Array(p) {
      var arr = []
      for (var key in p) {
        if (p.hasOwnProperty(key)) {
          arr.push([key, p[key]])
        }
      }
      return arr
    },
    checkStockSelection(stock_name) {
      var isin = false;
      for (var i = 0; i < this.selected_stocks.length; i++) {
        if (this.selected_stocks[i].localeCompare(stock_name) === 0) {
          isin = true;
          break
        }
      }
      return isin;
    },
    updateLinesColor() {
      var that = this;

      this.lines.each(function(d) {
        var stock_name = d.name;
        var selected = that.checkStockSelection(stock_name);
        if (selected) {
          d3.select(this).attr("stroke", "red");
        }else {
          if(stock_name !== "Sector Index"){
            d3.select(this).attr("stroke", "grey");
          }
        }
      })
    },
    initialize(plate) {
      var that = this;
      var plate = this.input_data;
      this.stock_keys = Object.keys(this.input_data);

      var root_div = d3.select(this.$el);
      var bounding_rect = root_div.node().getBoundingClientRect()
      var margin = { top: 5, right: 5, bottom: 5, left: 10 }

      // var height = bounding_rect.height;
      var height = document.documentElement.clientHeight * 0.26;
      var width = bounding_rect.width - margin.left - margin.right;

      this.svg = root_div.select(".draw-area").select("svg")
        .attr("width", width)
        .attr("height", height)
        .append("g")
        .attr("transform", "translate(10,0)")
        .attr("width", width)
        .attr("height", height)

      width -= 30;

      // process data of stocks
      var all_date = [];
      this.stock_keys.forEach(k => {
        all_date = all_date.concat(Object.keys(plate[k]))
      })
      var return_data = [];
      this.stock_keys.forEach(k => {
        var raw = plate[k]
        var starting_value = raw[Object.keys(raw)[0]]
        var price_vs_date = this.object2Array(raw);
        var return_vs_date = price_vs_date.map(d => {
          d[1] = (d[1] - starting_value)/starting_value
          return d
        })
        return_data.push({
          name: k,
          data: return_vs_date,
          selected: false,
        })
      })

      // process data of market index
      // var market_index_raw = this.object2Array(this.market_index);
      // var market_index_starting = market_index_raw[0][1];
      // var market_index_return = market_index_raw.map(d => {
      //   d[1] = (d[1] - market_index_starting)/market_index_starting
      //   return d
      // })

      // Get the max of all the returns for scale
      // var all_return_abs = [];
      // return_data.forEach(d => {
      //   all_return_abs = all_return_abs.concat(d.data.map(d => Math.abs(d[1])))
      // })
      // all_return_abs = all_return_abs.concat(market_index_return.map(d => Math.abs(d[1])))
      // var max_return_abs = Math.max(...all_return_abs);
      var all_return = [];
      return_data.forEach(d => {
        all_return = all_return.concat(d.data.map(d => d[1]))
      })
      var max_return = Math.max(...all_return);
      var min_return = Math.min(...all_return);

      // prepare to draw
      this.parseTime = d3.timeParse("%Y-%m-%d");
      this.xScale = d3.scaleTime()
        .domain(d3.extent(all_date, d => this.parseTime(d)))
        .range([0, width]);

      var padding = max_return * 0.05;
      this.yScale = d3.scaleLinear()
        // .domain([-max_return_abs, max_return_abs])
        .domain([min_return - padding, max_return + padding])
        .range([height - 10, 10])

      this.svg.append("g")
        .attr("transform", "translate("+[0, height * Math.abs(max_return + padding) / (Math.abs(max_return + padding) + Math.abs(min_return - padding))]+")")
        .call(d3.axisBottom(this.xScale).ticks(5).tickFormat(d3.timeFormat("%Y.%m")))
        .selectAll("text")

      this.svg.append("g")
        .call(d3.axisLeft(this.yScale).tickFormat(d => `${d*100}%`).ticks(5))
        .selectAll('text')
        .attr('x',40);

      var liner = d3.line()
        .curve(d3.curveMonotoneX)
        .x(d => {
          return this.xScale(this.parseTime(d[0]))
        })
        .y(d => this.yScale(d[1]))

      // draw lines for stocks
      var lines = this.svg.append("g")
        .classed("lines", true)
        .selectAll("path")

      var tooltip = d3.select('body').append("div")
        .style("position", "absolute")
        .style("visibility", "hidden")
        .style("background", "lightsteelblue")
        .style("opacity", "0.8")
        .style("padding", "5px")

      this.lines = lines.data(return_data)
        .enter()
          .append("path")
        .merge(lines)
          .attr("d", d => liner(d.data))
          .attr("stroke", d=>{
            if(d.name !== "Sector Index"){
              return "grey";
            }else{
              return "#7cb9f2";
            }
          })
          .attr("opacity",d=>{
            return d.name == "Sector Index" ? "1": "0.6";
          })
          .attr("stroke-width", "2")
          .attr("fill", "none")
          .on("mouseover", function(d) {
            d3.select(this).style("cursor", "pointer");
            d3.select(this).attr("stroke-width", '4').attr("opacity","1");
            tooltip.style("visibility", "visible");
          })
          .on("mouseout", function(d) {
            let opacity = d.name == "Sector Index" ? "1": "0.6";
            d3.select(this).style("cursor", "default");
            d3.select(this).attr("stroke-width", '2').attr("opacity", opacity)
            tooltip.style("visibility", "hidden");
          })
          .on("mousemove", function(d) {
            var svg_rect = that.svg.node().getBoundingClientRect();
            var x = event.pageX-svg_rect.left;
            var y = event.pageY-svg_rect.top + 10;
            tooltip.style("top", (event.pageY-20)+"px").style("left", (event.pageX+10)+"px");
            tooltip.html(
              d.name + "<br/>" +
              "date: " + d3.timeFormat("%Y.%m.%d")(that.xScale.invert(x)) + "<br/>" +
              "return: " +`${(that.yScale.invert(y)*100).toFixed(1)}%`
            )
          })
          .on("click", function(d) {
            if(d.name == "Sector Index"){
              return;
            }else{
              var isin = that.checkStockSelection(d.name);
              that.$emit("SelectStock", [d.name, !isin]);
              that.updateLinesColor();
            }
          })

      // Legend
      var legend_width = 30;
      var legend_height = 12;
      var legend_margin_left = bounding_rect.width - 5 * legend_width - margin.right;

      var legend = this.svg.append("g")
          .selectAll('.legend')
          .data(["Selected", "Unselected", "Sector Index"])
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
                "Unselected": "grey",
                "Selected": "red",
                "Sector Index": "#7cb9f2"
              }[d]
          })
      ;

      legend.append('text')
          .attr("x", 1.5 * legend_width + "px")
          .attr("y", (d,i)=>{
              return i * legend_height * 1.1 + 0.4 * legend_height + "px";
          })
          .text(function (d, i) {
              return d;
          })
          .style("text-anchor", "start")
          .style("font-size", 1.1 * legend_height + "px");


      // draw the market index
      // var market_index_paths = this.svg.append("g")
      //   .classed("market-index", true)
      //   .selectAll("path")

      // market_index_paths.data([market_index_return])
      //   .enter()
      //     .append("path")
      //   .merge(market_index_paths)
      //     .attr("d", d => liner(d))
      //     .attr("stroke", "red")
      //     .attr("stroke-width", "1")
      //     .attr("fill", "none")
      //     .each(function(d) {
      //       d3.select(this).append("title")
      //         .classed("tooltip", true)
      //         .text("market index");
      //     });
      //     .on("mouseover", function(d) {
      //       d3.select(this).attr("stroke-width", '3')
      //     })
      //     .on("mouseout", function(d) {
      //       d3.select(this).attr("stroke-width", '1')
      //     })
    },
  }
}

</script>
