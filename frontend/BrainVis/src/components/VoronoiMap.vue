<template>
  <g></g>
</template>

<script>
import { mapState } from 'vuex';
import * as d3Base from 'd3'
import {voronoiMapSimulation} from 'd3-voronoi-map'
import C from './constants.js'

const d3 = Object.assign(d3Base, {voronoiMapSimulation})

export default {
  name: 'VoronoiMap',
  data() {
    return {
      sector_keys: ["A", "B", "C", "D", "E", "F"],
      gap_size: 2,
      anchors: [],
      complete_type: {
        "A": "Transaction",
        "B": "Momentum",
        "C": "Value",
        "D": "Growth",
        "E": "Profit",
        "F": "Liquidity",
      },
    }
  },
  props: {
    svg_width: {
      type: Number,
      default: 0
    },
    datapoint: {
      type: Object,
      default: {}
    },
    date: {
      type: String
    },
    sign: {
      type: String
    },
    inner_circle_r: {
      type: Number,
      default: 0,
    },
    pos_or_neg: {
      type: Number,
      default: 1
    }
  },
  computed: {
    ...mapState(['selected_factors']),
    max_sector_size: function () {
      return (this.svg_width) / 2;
    }
  },
  mounted() {
    this.$nextTick(function foo() {
      this.initLayout();
      this.drawCircle(this.datapoint);
    });
  },
  methods: {
    initLayout() {
      var svg_width = this.svg_width;
      var svg_height = this.svg_width;

      this.svg = d3.select(this.$el)
        .attr("width", svg_width)
        .attr("height", svg_height);

      this.fillScale = d3.scaleOrdinal()
        .domain(["A", "B", "C", "D", "E", "F"])
        // .range(["#CD6600", "#0047B6", "#84a7b2","#3690c0", "#EEAD0E", "#996699"]);
        .range(C.FACTOR_TYPE_COLORMAP)

      this.container = this.svg.append("g")
        .classed("container", true);
        // .attr("transform", "translate("+[svg_width/2, svg_height/2]+")");

      // this.container.append("g")
      //   .classed('cells', true);
    },
    // Entry point to draw a complete circle with six sectors
    drawCircle(datapoint) {
      var sectors_info = datapoint

      var sum_size = sectors_info['sum']
      var sizes = this.sector_keys.map(k => Math.abs(sectors_info[k].size));
      var max_size = Math.max(...sizes);
      var sector_size_scaler =  d3.scaleSqrt()
        .domain([0, max_size])
        .range([this.inner_circle_r, this.inner_circle_r + this.max_sector_size])
      // var sector_size_scaler =  d3.scaleSqrt().domain([0, max_size]).range([0, this.max_sector_size])

      // Draw six sectors
      this.sector_keys.forEach((k, i) => {
        var sector_info = sectors_info[k];
        var size = Math.abs(sector_info.size);
        var outer_r = sector_size_scaler(size);
        var inner_r = sector_size_scaler(size * sector_info.others);
        this.drawSector(sector_info, inner_r, outer_r, i, k);
      })

      // draw the circle representing the bias
      var bias_circle = this.container.selectAll("circle")
        .data([this.inner_circle_r])
        .enter()
        .append("circle")
        .attr("cx", 0)
        .attr("cy", 0)
        .attr("r", this.inner_circle_r + 1.7)
        .style('stroke', 'black')
        //.style('stroke', sectors_info.bias*this.pos_or_neg>0 ? 'black' : 'white')
        .style("fill", "white");

      // Update anchors after drawing all sectors
      // Notify FactorAxis a set of anchors have been computed
      this.$emit("AnchorsComputed", {
        anchors: this.anchors,
        date: this.date,
        sign: this.sign,
      })

    },
    shrinkTowardsCenterByLen(point, shrink_len) {
      var x = point[0];
      var y = point[1];
      var len = Math.sqrt(Math.pow(x, 2) + Math.pow(y, 2));
      var ux = x / len;
      var uy = y / len;
      var nx = x - ux * shrink_len;
      var ny = y - uy * shrink_len;
      return [nx, ny]
    },
    // Draw a sector, including the triangle and the voronoi map
    drawSector(sector_info, inner_r, outer_r, i, key) {
      var start_ang = i*Math.PI/3;
      var end_ang = (i+1)*Math.PI/3;

      // shift the sector a bit from the center
      var sx = this.gap_size * Math.cos((start_ang+end_ang)/2);
      var sy = this.gap_size * Math.sin((start_ang+end_ang)/2);

      var sector = this.container.append("g")
        .classed("sector", true)
        .attr("transform", "translate("+[sx, sy]+")");

      var triangle = sector.append("g")
        .classed("triangle", true)

      var cells = sector.append("g")
        .classed("cells", true)

      var fill_color = d3.rgb(this.fillScale(key)).brighter(0.5);

      // When there are more than 1 factors, use the voronoi map
      if (sector_info.top5.length > 1){
        // Draw the voronoi map
        var polygon = this.calculateRingSectorPolygon(
          start_ang, end_ang, inner_r, outer_r
        );
        var top5_data = sector_info.top5.map(d => {
          return {
            weight: d.ratio,
            name: d.factor_name,
            size: sector_info.size
          }
        })

        this.drawVoronoi(polygon, top5_data, cells, fill_color, key);

        // Draw the triangle
        if (inner_r > 0) {
          var point1 = this.shrinkTowardsCenterByLen(polygon[0].slice(), this.gap_size)
          var point2 = this.shrinkTowardsCenterByLen(polygon[1].slice(), this.gap_size)
          var points = [[0,0], point1, point2];

          triangle.append("path")
            .attr("d", "M"+points.join(",")+"z")
            .style("fill", 'grey')
            // .style('stroke', "black")
            // .style('stroke-width', '1px');
        }
      // When there is only one, simulation can sometimes incur a bug
      // So draw the outer region manually
      }else if (sector_info.top5.length === 1) {
        // Draw the outer region
        var polygon = this.calculateRingSectorPolygon(
          start_ang, end_ang, inner_r, outer_r
        );

        var temp = cells.selectAll(".cell").data([polygon])
        this.drawClosedPath(temp, sector_info, cells, fill_color, key);

        // Draw the triangle
        if (inner_r > 0) {
          var point1 = this.shrinkTowardsCenterByLen(polygon[0].slice(), this.gap_size)
          var point2 = this.shrinkTowardsCenterByLen(polygon[1].slice(), this.gap_size)
          var points = [[0,0], point1, point2];

          triangle.append("path")
            .attr("d", "M"+points.join(",")+"z")
            .style("fill", 'grey')
            // .style('stroke', "black")
            // .style('stroke-width', '1px')
        }
      // If there is no top5, draw the grey area only
      }else {
        var points = this.calculateRingSectorPolygon(
          start_ang, end_ang, 0, outer_r
        );
        triangle.append("path")
          .attr("d", "M"+points.join(",")+"z")
          .style("fill", 'grey')
          // .style('stroke', "black")
          // .style('stroke-width', '1px');
      }
    },
    // start_ang and end_ang are in radian
    calculateRingSectorPolygon(start_ang, end_ang, inner_r, outer_r) {
      var increment = Math.PI/60;
      var polygon = [];

      // add the inner points
      // The polygon can only be convex, and thus the inner boundary cannot be an arc.
      if (inner_r === 0) {
        polygon.push([0,0]);
      }else {
        polygon.push([inner_r*Math.cos(start_ang), inner_r*Math.sin(start_ang)]);
        polygon.push([inner_r*Math.cos(end_ang), inner_r*Math.sin(end_ang)]);
      }

      // add the outer points
      for (var a=end_ang; a>=start_ang; a-=increment) {
        polygon.push(
          [outer_r*Math.cos(a), outer_r*Math.sin(a)]
        )
      }
      return polygon
    },
    // https://stackoverflow.com/questions/12062561/calculate-svg-path-centroid-with-d3-js
    getBoundingBoxCenter(selection) {
      // get the DOM element from a D3 selection
      // you could also use "this" inside .each()
      // var element = selection.node();
      var element = selection;
      // use the native SVG interface to get the bounding box
      var bbox = element.getBBox();
      // return the center of the bounding box
      return [bbox.x + bbox.width/2, bbox.y + bbox.height/2];
    },
    // Draw a closed path and some other events needed for top 5 factors
    drawClosedPath(cells, single_top, ele, fill_color, sector_category_key){
      var that = this;
      cells.enter()
        .append("path")
          .classed("cell", true)
        .merge(cells)
          .attr("d", function(d){ return "M"+d.join(",")+"z"; })
          .style("fill", fill_color)
          // .style("fill-opacity", "0.4")
          .style('stroke', "white")
          .style('stroke-width', '1px')
          .attr('pointer-events', 'all')
          .on("mouseover", function(d) {
            // d3.select(this).style("fill", d3.select(this).attr('stroke')).attr('opacity', 0.3);
            d3.select(this)
              // .style("fill-opacity", "0.8")
              .style("cursor", "pointer")
              .style("stroke", "red")
              .style("stroke-width", 2)
            if (single_top !== null) {
              that.$emit("HoverFactor", single_top.top5[0].factor_name)
            } else {
              that.$emit("HoverFactor", d.site.originalObject.data.originalData.name)
            }
          })
          .on("mouseout", function(d) {
            // d3.select(this).style("fill", "none").attr('opacity', 1);
            d3.select(this)
              // .style("fill-opacity", "0.4")
              .style('stroke', "white")
              .style('stroke-width', '1px')
              .style("cursor", "default")
            that.$emit("LeaveFactor")
          })
          .each(function(d) {
            var center = that.getBoundingBoxCenter(this);
            var this_name, this_weight, this_sum;
            if (single_top === null) {
              this_weight = d.site.originalObject.data.originalData.weight;
              this_name = d.site.originalObject.data.originalData.name.toString();
              this_sum = d.site.originalObject.data.originalData.size;
            } else {
              this_weight = single_top.top5[0].ratio;
              this_name = single_top.top5[0].factor_name.toString();
              this_sum = single_top.size;
            }
            that.anchors.push({
              name: this_name,
              center: center
            });
            d3.select(this)
              .on("click", function(d) {
                let factor_name = this_name;
                let isin = false;
                for(let i=0;i<that.selected_factors.length;i++){
                    if(that.selected_factors[i] === factor_name){
                        isin = true;
                        break;
                    }
                }
                if(isin){
                    that.$store.commit('rm_selected_factor', factor_name);
                }
                else{
                    that.$store.commit('add_select_factor', factor_name);
                }
              })
            d3.select(this).append("title")
              .classed("tooltip", true)
              .text(() => this_name+" ("+ that.complete_type[sector_category_key]+"):" + (this_weight * this_sum).toFixed(2) + "%");
          })
    },
    // draw voronoi map at the outer part of the sector
    // polygon: region boundary of the voronoi map (list of points)
    // data: data for the voronoi map
    // ele: HTML element to draw cells on
    drawVoronoi(polygon, data, ele, fill_color, sector_category_key) {
      var that = this;
      data.forEach(d => {
        if (d.weight < 1e-1) {
          // console.warn("HERE!", d.weight);
          d.weight = 1e-1;
        }
      })
      var simulation = d3.voronoiMapSimulation(data)
        .clip(polygon)
        .weight(function(d) {return d.weight})

      var state = simulation.state();

      // manually launch each iteration until the simulation ends
      var max_iters = 20, _iter=0;
      while (!state.ended) {
        _iter += 1;
        if(_iter > max_iters){
          break;
        }
        simulation.tick();
        state = simulation.state();
      }

      var cells = ele.selectAll(".cell")
        .data(state.polygons)

      this.drawClosedPath(cells, null, ele, fill_color, sector_category_key);

    }
  }
}
</script>
