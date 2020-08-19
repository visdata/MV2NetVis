<template>
  <div style="width:100%" id='glyph_area'>
    <div style="width:100%;height:10px">
      <p style='text-align:left;padding-left:20px;font-weight:bold;cursor:pointer;'
         v-bind:style="{color:selected?'red':'black'}"
         v-on:click="toggleStockSelection(stock_name)">
      {{stock_name}}
      </p>
    </div>
    <div style="overflow:auto">
      <svg>
        <g v-bind:transform='"translate(5," + height/2 + ")"'>
          <g id="glyphs">
            <g class="glyph"
              v-for="d in data_glyphs"
              v-bind:transform='"translate("+d.sx+","+d.sy+")"'
            >
              <voronoi-map
                v-bind:svg_width="d.size"
                v-bind:datapoint="d.data"
                v-bind:date="d.date"
                v-bind:sign="d.sign"
                v-bind:inner_circle_r="d.inner_circle_r"
                v-bind:pos_or_neg="d.pos_or_neg"
                v-on:AnchorsComputed="setAnchor"
                v-on:HoverFactor="addOneFactorConnection"
                v-on:LeaveFactor="drawFactorConnection"
              >
              </voronoi-map>
            </g>
          </g>
        </g>
      </svg>
    </div>
  </div>
</template>

<script>
import { mapState } from 'vuex';
import * as d3 from 'd3'
import VoronoiMap from '@/components/VoronoiMap';
import C from './constants.js'

// if error_ratio > ERROR_RATIO_THRESHOLD, then the y position will be fixed
const ERROR_RATIO_THRESHOLD = 50;
const X_OFFSET = 50;

export default {
  name: 'FactorAxis',
  components: {
    VoronoiMap
  },
  props: {
    input_data: {
      type: Object,
    }
  },
  computed: {
    ...mapState(['start_date','end_date', 'selected_factors', 'selected_stocks']),
  },
  watch: {
    input_data: 'initialize',
    selected_factors: 'drawFactorConnection',
    selected_stocks: "updateStockSelection",
  },
  data() {
    return {
      datapointKeys: ['positive', 'negative'],
      height:0,
      width:0,
      data_glyphs: [], // datapoints for voronoi map
      dates: [], // dates in the given data
      anchors: {}, // stores the information needed for each connectFactor(), indexed by dates
      stock_name: "",
      last_len: 0,
      selected: false, // whether this stock is selected in E
    }
  },
  mounted() {
    this.$nextTick(function foo() {
      this.initializeDrawingArea();
      this.initialize();
      this.updateStockSelection();
    })
  },
  methods: {
    maxOfObject(p) {
      var max = -1e7
      for (var key in p) {
        if (p.hasOwnProperty(key)) {
          if (p[key] > max) {
            max = p[key]
          }
        }
      }
      return max
    },
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
    updateStockSelection() {
      this.selected = this.checkStockSelection(this.stock_name);
    },
    toggleStockSelection(stock_name) {
      var isin = this.checkStockSelection(stock_name);
      if (isin) {
        this.$store.commit('rm_selected_stock', stock_name);
      }else {
        this.$store.commit('add_select_stock', stock_name);
      }
    },
    initializeDrawingArea() {
      var glyphSVG = d3.select(this.$el).select("svg")
      this.drawArea = glyphSVG.select("g")
      var drawArea = this.drawArea
      this.xAxis = drawArea.append("g")
        .classed("x-axis", true)
      this.yAxis = drawArea.append("g")
        .classed("y-axis", true)

      // For connectFactor
      this.lines = drawArea.append("g")
        .classed("lines", true);

      // For drawCloseLine
      this.close = drawArea.append("g")
        .classed("close",true)
      this.close_true = this.close.append("g")
        .classed("close-true", true)
      this.close_predicted = this.close.append("g")
        .classed("close_predicted", true)

      // For drawErrorBars
      this.errorBars = drawArea.append('g')
        .classed('error-bars', true)
    },
    calculateErrorRatio(stock) {
      // calculate the offset_y for each month of the stock
      for (var idx = 0; idx < stock.length; ++idx) {
        var datapoint = stock[idx];
        var close_data = this.object2Array(datapoint.close).map(d => d[1]);
        var predicted_close_data = this.object2Array(datapoint.predicted_close).map(d => d[1]);
        var close_starting = close_data[0];
        var close_ending = close_data[close_data.length-1];
        var predicted_close_ending = predicted_close_data[predicted_close_data.length-1];
        stock[idx].error_ratio = Math.abs((close_ending - predicted_close_ending) / close_starting);
      }
      return stock;
    },
    initialize() {
      // initialize the data
      this.stock_name = Object.keys(this.input_data)[0]
      this.data = this.input_data[this.stock_name]
      // this following line does not work as a way to reload all the voronoi maps
      this.data_glyphs = []
      this.dates = []
      this.anchors = {}

      var stock = this.data;

      var margin = {
        top: 10,
        right: 10,
        bottom: 10,
        left: 10
      };

      // var month_width = 80;
      // var width = month_width * stock.length - margin.right - margin.left;
      // var height = 600 - margin.top - margin.bottom;
      // var width = document.getElementById('glyph_area').clientWidth - margin.left - margin.right;
      // var height = document.getElementById('glyph_area').clientHeight;

      var root_div = d3.select(this.$el);
      var bounding_rect = root_div.node().getBoundingClientRect();
      var width = bounding_rect.width - margin.left - margin.right;
      // var height = bounding_rect.height;
      var height = C.FACTOR_AXIS_HEIGHT;

      this.width = width;
      this.height = height;

      var glyphSVG = d3.select(this.$el).select("svg")
        .attr("width", width)
        .attr("height", height)

      var allBias = [];
      var allSum = [];
      stock.forEach((value) => {
        allBias.push(Math.abs(value['positive']['bias']));
        allSum.push(value['positive']['sum']);
        allSum.push(Math.abs(value['negative']['sum']));
      });

      // var maxSum = Math.max(...allSum);
      // var positionScale = d3.scaleLinear().domain([0, maxSum]).range([0, height/2-50]);
      // var maxBias = Math.max(...allBias);
      // var biasScale = d3.scaleLinear().domain([0, maxBias]).range([100, 20]);

      // stock = this.calculateErrorRatio(stock);
      var max_pct_change = Math.max(...stock.map(d=>Math.abs(d.pct_change)));
      if(max_pct_change > ERROR_RATIO_THRESHOLD) {
        var glyph_y_scale = d3.scaleLinear().domain([0, ERROR_RATIO_THRESHOLD, max_pct_change]).range([C.GLYPH_CENTER_TO_BAR, height/2-C.MARGIN_HEIGHT_FOR_GLYPH, height/2-C.MARGIN_HEIGHT_FOR_GLYPH]);
        var glyph_y_scale_for_axis = d3.scaleLinear().domain([-ERROR_RATIO_THRESHOLD, ERROR_RATIO_THRESHOLD]).range([-(height/2-C.MARGIN_HEIGHT_FOR_GLYPH), height/2-C.MARGIN_HEIGHT_FOR_GLYPH])
        var error_bar_opcaity_scale = d3.scaleLinear().domain([0, ERROR_RATIO_THRESHOLD, max_pct_change]).range([0.2, 1, 1]);
      } else if(max_pct_change <= ERROR_RATIO_THRESHOLD) {
        var glyph_y_scale = d3.scaleLinear().domain([0, max_pct_change]).range([C.GLYPH_CENTER_TO_BAR, height/2-C.MARGIN_HEIGHT_FOR_GLYPH]);
        var glyph_y_scale_for_axis = d3.scaleLinear().domain([-max_pct_change, max_pct_change]).range([-(height/2-C.MARGIN_HEIGHT_FOR_GLYPH), height/2-C.MARGIN_HEIGHT_FOR_GLYPH])
        var error_bar_opcaity_scale = d3.scaleLinear().domain([0, max_pct_change]).range([0.2, 1]);
      }

      var maxSum = Math.max(...allSum);
      var glyph_size_scale = d3.scaleSqrt().domain([0, maxSum]).range([0, C.MAX_GLYPH_RADIUS]);

      var maxBias = Math.max(...allBias);
      var glyph_inner_circle_scale = d3.scaleSqrt().domain([0, maxBias]).range([0,10]);

      var maxError = Math.max(...stock.map(d=>Math.abs(d.error)))
      var error_bar_width_scale = d3.scaleLinear().domain([0, maxError]).range([0, 20]);

      var drawArea = this.drawArea

      // Draw the time axis
      this.parseTime = d3.timeParse("%Y-%m-%d");
      var prev_monthday = (dt_str) => {
        let parts = dt_str.split("-");
        let dt = new Date(parseInt(parts[0]), parseInt(parts[1]), parseInt(parts[2]));
        dt.setDate(1);
        dt.setMonth(dt.getMonth()-1);
        return dt.getFullYear() + "-" + dt.getMonth() + "-" + dt.getDate();
      }
      var month_count = (d1, d2)=>{
        let months;
        let parts_d1 = d1.split("-");
        let parts_d2 = d2.split("-");
        d1 = new Date(parseInt(parts_d1[0]), parseInt(parts_d1[1]), parseInt(parts_d1[2]));
        d2 = new Date(parseInt(parts_d2[0]), parseInt(parts_d2[1]), parseInt(parts_d2[2]));
        months = (d2.getFullYear() - d1.getFullYear()) * 12;
        months -= d1.getMonth() + 1;
        months += d2.getMonth();
        return months <= 0 ? 0 : months;
      }

      this.x_scale = d3.scaleTime()
        // .domain(d3.extent(stock, (d) => this.parseTime(d['date'])))
        .domain([this.parseTime(this.start_date), this.parseTime(prev_monthday(this.end_date))])
        .range([X_OFFSET, width-50]);

      // max of relative values (divided by the starting value)
      // var maxClose = d3.max(this.data, d => this.maxOfObject(d['close']) / d['close'][Object.keys(d['close'])[0]])
      // var maxPredictedClose = d3.max(this.data, d => this.maxOfObject(d['predicted_close']) / d['predicted_close'][Object.keys(d['predicted_close'])[0]])
      // calculate the scale used for lines
      // this.yScale = d3.scaleLinear()
      //  .domain([-Math.max(maxClose, maxPredictedClose), Math.max(maxClose, maxPredictedClose)])
      //  .range([height/2, -height/2])
      // this.xAxis.call(d3.axisBottom(this.x_scale).ticks(stock.length).tickFormat(d3.timeFormat("%Y.%m")));
      this.xAxis.call(d3.axisBottom(this.x_scale).ticks(month_count(this.start_date, this.end_date))
        .tickFormat(d3.timeFormat("%m")))
        .selectAll('text')
          .attr('x',20);
      this.yAxis.call(d3.axisLeft(glyph_y_scale_for_axis)
        .tickFormat(d => `${d}%`).ticks(5))
        .selectAll('text')
          .attr('x',35);
      // this.yAxis.call(d3.axisLeft(this.yScale));

      // this.drawCloseLine();

      // For the factors in each month, draw glyphs for positive and negative respectively
      var glyphs = drawArea.select("#glyphs");
      for (var idx = 0; idx < stock.length; ++idx) {
        var datapoint = stock[idx];
        // calculate the x position of the glyph
        // var sx = idx * month_width + X_OFFSET;
        var month = this.parseTime(datapoint['date'])
        month.setDate(1)
        var sx = this.x_scale(month)
        // Get dates
        var date = datapoint.date;
        this.dates.push(date)
        this.anchors[date] = {}
        this.anchors[date]['offset'] = {}
        this.anchors[date]["anchors"] = {}

        // Handle the positive and negative respectively
        this.datapointKeys.forEach((k, i) => {
          var glyphInfo = datapoint[k];

          // var glyphPosition = glyphInfo['sum'];
          // var glyph_size = biasScale(Math.abs(glyphInfo['bias']));

          // calculate the y position of the glyph
          var sy;
          if (k === "positive") {
            // sy = - positionScale(Math.abs(glyphPosition));
            sy = - glyph_y_scale(Math.abs(datapoint.pct_change));
          }else {
            // sy = + positionScale(Math.abs(glyphPosition));
            sy = + glyph_y_scale(Math.abs(datapoint.pct_change));
          }

          var glyph_size = glyph_size_scale(Math.abs(glyphInfo['sum']));
          var inner_circle_r = glyph_inner_circle_scale(Math.abs(glyphInfo['bias']));

          var data_glyph = {
            data: glyphInfo,
            date: date,
            sign: k,
            sx: sx,
            sy: sy,
            size: glyph_size,
            inner_circle_r: inner_circle_r,
            pos_or_neg: k==="positive" ? +1 : -1
          };
          this.data_glyphs.push(data_glyph);

          // Store position information for connectFactor
          this.anchors[date]['offset'][k] = {}
          this.anchors[date]['offset'][k]["sx"] = sx
          this.anchors[date]['offset'][k]["sy"] = sy
        })
      }

      // Draw Error Bars
      var errorBars = this.errorBars.selectAll('.error-bars')
        .data(stock)
      errorBars.enter()
        .append('rect')
        .attr('x', (d, i) => this.x_scale(this.parseTime(d.date)) - error_bar_width_scale(Math.abs(d.error))/2)
        .attr('width', (d) => error_bar_width_scale(Math.abs(d.error)))
        .attr('y', (d) => - glyph_y_scale(Math.abs(d.pct_change)) + C.GLYPH_CENTER_TO_BAR)
        .attr('height', (d) => 2 * (glyph_y_scale(Math.abs(d.pct_change)) - C.GLYPH_CENTER_TO_BAR))
        .style('fill',(d) => d.pct_change > 0? '#f26b44': '#7ac89f')
        .style('opacity', '0.8')
        //.style('opacity', (d) => error_bar_opcaity_scale(Math.abs(d.pct_change)))
        // .on("mouseover", function(d) {
        //   d3.select(this)
        //     .style("stroke", "black")
        //     .style("stroke-width", 2)
        //     .style("stroke-opacity", 1)
        // })
        // .on("mouseout", function(d) {
        //   d3.select(this)
        //     .style("stroke", "none")
        // })
        .each(function(d) {
          d3.select(this).append("title")
            .classed("tooltip", true)
            .text(() => "stock price: " + d.pct_change.toFixed(2) + "%\nprediction error: " + d.error.toFixed(2) + "%")
            //.text(() => "error: " + d.error.toFixed(2) + "%\nabsolute error: " + d.error_abs.toFixed(2) + "%")
        })
    },
    setAnchor(data) {
      var anchors = data.anchors;
      var date = data.date;
      var sign = data.sign;
      this.anchors[date]["anchors"][sign] = anchors;
    },
    vectorAdd(v1, v2) {
      var v = v1.slice;
      for (var i = 0; i < v1.length; i ++) {
        v[i] = v[i] + v2[i]
      }
      return v
    },
    drawFactorConnection() {
      this.clearConnectFactor();
      this.connectFactor(this.selected_factors);
    },
    addOneFactorConnection(name) {
      var factors = [];
      for (var i = 0; i < this.selected_factors.length; i ++) {
        factors.push(this.selected_factors[i]);
      }
      factors.push(name)
      this.clearConnectFactor();
      this.connectFactor(factors);
    },
    // Used to connect factors at different dates and show their changes
    connectFactor(names) {
      var lines_data = []

      for (var i = 0; i < names.length; i ++) {
        var name = names[i];
        var points = [];
        this.dates.forEach(d => {
          var offsets = this.anchors[d].offset;
          var anchors = this.anchors[d].anchors;
          for (var i = 0; i < this.datapointKeys.length; i++) {
            var k = this.datapointKeys[i];
            var tops = anchors[k];
            var offset = offsets[k]
            var flag = false;
            for (var j = 0; j < tops.length; j++) {
              var factor = tops[j];
              if (factor.name.localeCompare(name) === 0) {
                points.push([offset.sx+factor.center[0], offset.sy+factor.center[1]]);
                flag = true;
                break;
              }
            }
            if (flag) {break;}
            // If no factor in tops of this date, add a default position
            if (i === this.datapointKeys.length-1) {
              points.push([offset.sx, 0])
            }
          }
        })
        lines_data.push(points)
      }

      var lines = this.lines.selectAll(".line")
        .data(lines_data)
      lines.enter()
        .append("path")
        .classed("line", true)
        .merge(lines)
          .attr("d", d3.line()
                       .curve(d3.curveMonotoneX)
                       .x(function(d) { return d[0]; })
                       .y(function(d) { return d[1]; })
          )
          .attr("stroke", "red")
          .attr("stroke-width", "1.5")
          .attr("fill", "none");
    },
    clearConnectFactor() {
      var lines = this.lines.selectAll(".line")
        .data([]);
      lines.exit()
        .remove();
    },
    // To draw the lines of close and predicted_close
    drawCloseLine() {
      var closeLine = d3.line()
        .x(d => this.x_scale(this.parseTime(d[0])))
        .y(d => this.yScale(d[1]))

      // array of arraies of poiints, draw price line for each month separatly.
      var close_data = [];
      var predicted_close_data = [];
      this.data.forEach(d => {
        // close_data = close_data.concat(this.object2Array(d.close))
        // predicted_close_data = predicted_close_data.concat(this.object2Array(d.predicted_close))
        var close_this_month = this.object2Array(d.close);
        var predicted_close_this_month = this.object2Array(d.predicted_close);
        var starting_value = close_this_month[0][1];
        close_data.push(close_this_month.map(d => {
          d[1] = (d[1] - starting_value)/starting_value;
          return d;
        }))
        predicted_close_data.push(predicted_close_this_month.map(d => {
          d[1] = (d[1] - starting_value)/starting_value;
          return d;
        }))
      });

      var close_true_line = this.close_true.selectAll("path")

      close_true_line.data(close_data)
        .enter()
          .append("path")
        .merge(close_true_line)
          .attr("class", "line")
          .attr("d", closeLine)
          .attr("stroke", "red")
          .attr("stroke-width", "1")
          .attr("fill", "none");

      var close_predicted_line = this.close_predicted.selectAll("path")

      close_predicted_line.data(predicted_close_data)
        .enter()
          .append("path")
        .merge(close_predicted_line)
          .attr("class", "line")
          .attr("d", closeLine)
          .attr("stroke", "blue")
          .attr("stroke-width", "1")
          .attr("fill", "none");
    }
  }
}

</script>