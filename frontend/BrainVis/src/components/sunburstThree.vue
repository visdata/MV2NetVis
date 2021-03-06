<template>
  <div class="graph">
    <div class="viewport" v-resize.throttle.250="resize" :style="{'width':'45%', 'margin':'auto', 'padding-left':'17px', 'z-index': '999'}">

      <!-- Use this slot to add information on top of the graph -->
      <slot name="top" :colorGetter="colorGetter" :nodes="graphNodes" :actions="actions">
      </slot>

      <!-- Use this slot to add behaviors to the sunburst -->
      <slot :on="on" :actions="actions">
      </slot>

    </div>

    <!-- Use this slot to add information on top or bottom of the graph-->
    <div :style="{'min-height':'30px', 'position':'absolute', 'bottom':'5px', 'right':'0px'}">
    <slot  name="legend" :colorGetter="colorGetter" :nodes="graphNodes" :actions="actions">
    </slot>
    </div>

    <div :style="{'text-align':'left', 'height':'0px', 'position':'absolute', 'right':'38px', 'top':'26px', 'font-size':'10px', 'color':'#8998AC'}">
      <!--div>selected subjects:</div-->
      <div v-bind:style="{'font-size' : subjects_valid ? '0px' : '9px', 'color':'red'}">多属性不一致</div>
      <slot name="selectionone" :width="width" :colorGetter="colorGetter" :nodes="graphNodes" :actions="actions">
      </slot>
      <slot name="selectiontwo" :width="width" :colorGetter="colorGetter" :nodes="graphNodes" :actions="actions">
      </slot>
    </div>
  </div>
</template>
<script>
/*eslint no-unused-vars: ["error", { "varsIgnorePattern": "transition" }]*/

import resize from "vue-resize-directive";
import { select } from "d3-selection";
import { scaleLinear, scaleSqrt } from "vue-d3-sunburst/node_modules/d3-scale";
import { hierarchy, partition } from "vue-d3-sunburst/node_modules/d3-hierarchy";
import { interpolate } from "vue-d3-sunburst/node_modules/d3-interpolate";
import { arc } from "d3-shape";
import { transition } from "d3-transition";
//import { colorSchemes } from "vue-d3-sunburst/src/infra/colorSchemes";
import { colorSchemes } from "./colorSchemesTwo";
import { mapState } from 'vuex';

function recursiveName(node) {
  const res = node
    .ancestors()
    .map(node => node.data.name)
    .join("-");
  return res;
}

function copyCurrentValues(to, from) {
  const { x0, x1, y0, y1 } = from;
  to._current = { x0, x1, y0, y1 };
}

function labelVisible(d) {
  return d.y1 <= 3 && d.y0 >= 1 && (d.y1 - d.y0) * (d.x1 - d.x0) > 0.03;
}
function labelTransform(d, r) {
  const x = (((d.x0 + d.x1) / 2) * 180) / Math.PI;
  const y = ((d.y0 + d.y1) / 2) * 1;
  //console.log("xy", d);
  return `rotate(${x - 90}) translate(${y},0) rotate(${x < 180 ? 0 : 180})`;
}

function arc2Tween(arcSunburst, d, indx) {
  const positionsKeys = ["x0", "x1", "y0", "y1"];
  const interpolates = {};
  positionsKeys.forEach(key => {
    interpolates[key] = interpolate(this._current[key], d[key]); //函数
  });
  copyCurrentValues(this, d);
  //console.log('this', this, interpolates);
  return function(t) {
    const tmp = {};
    positionsKeys.forEach(key => {
      tmp[key] = interpolates[key](t);
    });
    //console.log('tmp', tmp);//x0y0x1y1具体值从0到1
    //console.log('arcSunburst', arcSunburst(tmp, indx)); //生成d，弧的变换方法
    return arcSunburst(tmp, indx);
  };
}

const useNameForColor = d => d.name;
const useDepthForColor = d => d.depth;
const useDepthAndIndexForColor = d => [d.depth, d.index, d.peer];

export default {
  name: "sunburstThree",

  inject: {
    defaultSchemeColor: {
      default: Object.keys(colorSchemes)[8]
    }
  },

  props: {
    /**
     * Sunburst data where children property is a array containing children.
     */
    data: {
      type: Object,
      required: false
    },
    /**
     * D3 color scheme to be used.
     */
    colorScheme: {
      type: String,
      required: false,
      default() {
        return this.defaultSchemeColor;
      },
      validator: value => Object.keys(colorSchemes).indexOf(value) !== -1
    },
    /**
     * Function used to map an item and its color.
     * (nodeD3: Object) => category: Number | String
     * By default use the node name
     */
    getCategoryForColor: {
      type: Function,
      required: false,
      default: useDepthAndIndexForColor //useDepthForColor // useNameForColor
    },
    /**
     * Minimal arc angle to be displayed (in radian).
     */
    minAngleDisplayed: {
      type: Number,
      required: false,
      default: 0.005
    },
    /**
     * Function used to identify an arc, will be used during graph updates.
     * (nodeD3: Object) => id: String
     * @default id based on recursive agregation of node parent name
     */
    arcIdentification: {
      type: Function,
      required: false,
      default: recursiveName
    },
    /**
     *  Duration for in animation in milliseconds
     */
    inAnimationDuration: {
      type: Number,
      required: false,
      default: 100
    },
    /**
     *  Duration for out animation in milliseconds
     */
    outAnimationDuration: {
      type: Number,
      required: false,
      default: 1000
    },

    datadepth: {
      type: Number,
      required: false,
      default: 2
    }
  },

  directives: {
    resize
  },

  data() {
    const scaleX = scaleLinear()
      .range([0, 2 * Math.PI])
      .clamp(true);

    const scaleY = scaleLinear().range([0, 1]); //scaleSqrt().range([0, 1]);

    //生成弧
    this.arcSunburst = arc()
      .startAngle(d => scaleX(d.x0))
      .endAngle(d => scaleX(d.x1))
      .innerRadius(d => Math.max(0, scaleY(d.y0)))
      .outerRadius(d => Math.max(0, scaleY(d.y1)));

    this.scaleX = scaleX;
    this.scaleY = scaleY;

    return {
      /**
       * @private
       */
      graphNodes: {
        clicked: null,
        mouseOver: null,
        zoomed: null,
        root: null,
        highlighted: null
      },

      /**
       * @private
       */
      width: null,

      /**
       * @private
       */
      height: null
    };
  },

  mounted() {
    var that = this;
    const [viewport] = this.$el.getElementsByClassName("viewport");
    this.viewport = viewport;
    this.vis = select(viewport)
      .append("svg")
      .style("overflow", "visible")
      .attr("class", "root")
      .attr("width", function(d){
        return "100%";
      })
      .attr("height", function(d){
        return "100%";
      })
      .append("g");

    select(viewport).on("mouseleave", () => {
      this.$emit("mouseLeave");
      this.graphNodes.mouseOver = null;
    });

    this.resize();
  },

  methods: {
    recursiveName(node) {
      const res = node
        .ancestors()
        .map(node => node.data.name)
        .join("-");
      return res;
    },
    /**
     * @private
     */
    getSize() {
      var width = this.viewport.clientWidth;
      var height = this.viewport.clientHeight ;
      return { width, height };
    },

    /**
     * @private
     */
    onData(data) {
      if (!data) {
        this.vis.selectAll("path").remove();
        Object.keys(this.graphNodes).forEach(k => (this.graphNodes[k] = null));
        return;
      }

      //等级,会生成x0,y0,x1,y1信息,y为0，0.25，0.5，0.75，1
      this.root = hierarchy(data)
        .sum(d => d.size)
        .sort((a, b) => b.value - a.value);

      //console.log('root', this.root);

      this.nodes = partition()(this.root)
        .descendants()
        .filter(
          d => Math.abs(this.scaleX(d.x1 - d.x0)) > this.minAngleDisplayed
        );

      /*
      for (var i in this.nodes) {
        console.log('y0,y1', this.nodes[i].y0, this.nodes[i].y1);
        this.nodes[i].y0 = (this.nodes[i].y0 - 0.15) / 0.85;
        this.nodes[i].y1 = (this.nodes[i].y1 - 0.15) / 0.85;
      }
      */

      //console.log('nodes', this.nodes);

      const pathes = this.getPathes();
      const colorGetter = this.colorGetter;
      const mouseOver = this.mouseOver.bind(this);
      const click = this.click.bind(this);
      const { arcSunburst } = this;
      const format = d3.format(',d');

      //console.log('arcSunburst', arcSunburst);

      //绘制圆弧
      pathes
        .enter()
        .append("path")
        .style("opacity", 1)
        .on("mouseover", mouseOver)
        .on("click", click)
        .each(function(d) {
          copyCurrentValues(this, d);
          //d.data.depth = d.depth;
          //console.log("d", d);
        })
        .merge(pathes)
        .style("fill", d => colorGetter(d.data))
        .transition("enter")
        .duration(this.inAnimationDuration)
        .attrTween("d", function(d, index) {
          return arc2Tween.call(this, arcSunburst, d, index);
        });

      var size0 = this.getSize();
      //console.log('size', size0);
      /*
      pathes
        .enter()
        .append("svg")
        .attr("x", function(d,index){
          //console.log('x0', size0.width);
          return Number(d.x0) * Number(size0.width);
        })
        .attr("y", function(d,index){
          return Number(d.y0) * Number(size0.height);
        })
        .attr("width", 50)
        .attr("heigth", 50)
        .insert("svg:text")
        .attr("x", 10)
        .attr("y", 10)
        .attr("dy", "0.25em")
        .attr("text-anchor", "middle")
        .each(function(d){
          
        })
        .text(function(d,index){
          return d.data.name;
        });
      */
      
      const scaleX = scaleLinear()
        .range([0, 2 * Math.PI])
        .clamp(true);
      const scaleY = scaleLinear().range([0, 1]);//scaleSqrt().range([0, 1]); //用面积表征圆而非半径
    
      //console.log("radius", this.radius);
      var that = this;
      //添加字体
      this.vis
        .selectAll("text")
        .remove();

      this.vis
        .selectAll("text")
        .data(this.nodes, this.arcIdentification)
        .enter()
        .append("text")
        .each(function(d) {
          copyCurrentValues(this, d);
          var startAngle = scaleX(d.x0);
          var endAngle = scaleX(d.x1);
          d.angle = (startAngle + endAngle) / 2;
        })
        .attr("dy", ".55em")
        .style("font-size", "11px")
        .style('fill', 'white')
        .attr("transform", function(d, index) {
          var result =  "rotate(" +　(index==0 ? d.angle * 180 / Math.PI : d.angle * 180 / Math.PI - 10) + ")";
          var sX = index==0 ? 3 : 0;
          var sY = index==0 ? 0 : (-1 * (Math.max(0, scaleY(d.y1))) * that.width  * that.datadepth / 3 / 2 + 12); //+ 5
          result += "translate(" +  sX +"," + sY + ")";
          //if (d.angle >= Math.PI * 3 / 4 && d.angle <= Math.PI * 5 / 4 )
          if (d.angle == Math.PI)
            result += "rotate(180)";
          return result;
        })
        .text(function(d) {
          //return d.data.name.substr(0,1).toUpperCase();
          return d.data.firstName;
        });
      
      pathes.exit().remove();

      this.graphNodes.root = this.nodes[0];
    },

    

    /**
     * @private
     */
    //选择所有的path并且添加node与获得名字方法
    getPathes() {
      return this.vis
        .selectAll("path")
        .data(this.nodes, this.arcIdentification);
    },

    /**
     * @private
     */
    resize() {
      console.log('resize');
      const { width, height } = this.getSize();
      this.vis
        .attr("width", width)
        .attr("height", height)
        .attr("transform", `translate(${width / 2}, ${height / 2 - 15} )`);

      this.radius = Math.min(width* this.datadepth / 3, height* this.datadepth / 3) / 2;
      const [scaleYMin] = this.scaleY.range();
      this.scaleY.range([scaleYMin, this.radius]);

      this.onData(this.data);
      this.width = width;
      this.height = height;
    },

    /**
     * @private
     */
    reDraw() {
      this.onData(this.data);
    },

    /**
     * @private
     */
    mouseOver(value) {
      this.graphNodes.mouseOver = value;
      /**
       * Fired when mouse is over a sunburst node.
       * @param {Object} value - {node, sunburst} The corresponding node and sunburst component
       */
      this.$emit("mouseOverNode", { node: value, sunburst: this });
    },

    /**
     * @private
     */
    click(value) {
      this.graphNodes.clicked = value;
      /**
       * Fired when sunburst node is clicked.
       * @param {Object} value - {node, sunburst} The corresponding node and sunburst component
       */
      this.$emit("clickNode", { node: value, sunburst: this });
    },

    /**
     * Highlight the arc path leading to a given node.
     * @param {Object} node the D3 node to highlight
     * @param {Number} opacity opacity of the none highlighted nodes, default to 0.3
     */
    //高亮
    highlightPath(node, opacity = 0.9) {
      const sequenceArray = node.ancestors();
      //console.log('sequenceArray', sequenceArray);
      var selectArray = [[], []];

      for(var i in this.selected_subjects) {
        selectArray[i] = this.selected_subjects[i].ancestors();
      }

      var that = this;
      //不在路径内的颜色变淡（透明度 0.9
      this.vis
        .selectAll("path")
        .filter(function(d){
          var name = that.recursiveName(d);
            for(var i in sequenceArray) {
              if(that.recursiveName(sequenceArray[i]) == name) {
                return false;
              }
            }
            for(var i in selectArray[0]) {
              if(that.recursiveName(selectArray[0][i]) == name) {
                return false;
              }
            }
            for(var i in selectArray[1]) {
              if(that.recursiveName(selectArray[1][i]) == name) {
                return false;
              }
            }
            return true;
          //return sequenceArray.indexOf(d) === -1 && selectArray[0].indexOf(d) === -1 && selectArray[1].indexOf(d) === -1;
        })
        .transition()
        .duration(this.inAnimationDuration)
        .style("opacity", opacity)
        .style("stroke", "none");

      //在路径内的透明度不变，边框变粉
      this.vis
        .selectAll("path")
        .filter(function(d){
          var name = that.recursiveName(d);
          var isIn = false;
            for(var i in sequenceArray) {
              if(that.recursiveName(sequenceArray[i]) == name) {
                //return false;
                isIn = true;
                break;
              }
            }
            for(var i in selectArray[0]) {
              if(that.recursiveName(selectArray[0][i]) == name) {
                return false;
              }
            }
            for(var i in selectArray[1]) {
              if(that.recursiveName(selectArray[1][i]) == name) {
                return false;
              }
            }
            return isIn && true;
          //return sequenceArray.indexOf(d) >= 0 && selectArray[0].indexOf(d) === -1 && selectArray[1].indexOf(d) === -1;
        })
        .style("opacity", 1)
        .style("stroke", "pink");

      this.graphNodes.highlighted = node;
    },

    /**
     * Zoom to a given node.
     * @param {Object} node the D3 node to zoom to.
     */
    zoomToNode(node) {
      //最外一层
      if(node.height == 0 && node.depth != 0) {
        this.$store.commit('add_selected_subjects', node);
      }
      /*
      //缩放，最外一层不缩放
      if(node.height != 0){
      this.vis
        .transition("zoom")
        .duration(750)
        .tween("scale", () => {
          const xd = interpolate(this.scaleX.domain(), [node.x0, node.x1]);
          const yd = interpolate(this.scaleY.domain(), [node.y0, 1]);
          const yr = interpolate(this.scaleY.range(), [
            node.y0 ? 20 : 0,
            this.radius
          ]);

          return t => {
            this.scaleX.domain(xd(t));
            this.scaleY.domain(yd(t)).range(yr(t));
          };
        })
        .selectAll("path")
        .attrTween("d", nd => () => this.arcSunburst(nd));

      this.graphNodes.zoomed = node;
      }
      */
      /*
      //变红
      var selectArray = [[], []];
      for(var i in this.selected_subjects) {
        selectArray[i] = this.selected_subjects[i].ancestors();
      }
      this.vis
        .selectAll("path")
        .filter(function(d){
          return selectArray[0].indexOf(d) >= 0 || selectArray[1].indexOf(d) >= 0;
        })
        .transition()
        .style("stroke", "red");
      */
    },

    /**
     * Reset the highlighted path
     */
    resetHighlight() {
      //回复正常
      var selectArray = [[], []];

      for(var i in this.selected_subjects) {
        selectArray[i] = this.selected_subjects[i].ancestors();
      }

      var that = this;
      this.vis
        .selectAll("path")
        .filter(function(d){
          var name = that.recursiveName(d);
            for(var i in selectArray[0]) {
              if(that.recursiveName(selectArray[0][i]) == name) {
                return false;
              }
            }
            for(var i in selectArray[1]) {
              if(that.recursiveName(selectArray[1][i]) == name) {
                return false;
              }
            }
            return true;
          //return selectArray[0].indexOf(d) === -1 && selectArray[1].indexOf(d) === -1;
        })
        .transition()
        .duration(this.outAnimationDuration)
        .style("opacity", 1)
        .style("stroke", "none");
    }
  },

  computed: {
    ...mapState(['subjects_valid', 'selected_subjects']),
    /**
     * @private
     */
    actions() {
      const { highlightPath, zoomToNode, resetHighlight } = this;
      return { highlightPath, zoomToNode, resetHighlight };
    },

    /**
     * @private
     */
    on() {
      return this.$on.bind(this);
    },

    /**
     * @private
     */
    colorGetter() {
      const colorScale = colorSchemes[this.colorScheme].scale;
      return d => colorScale(this.getCategoryForColor(d));
    },
  },

  watch: {
    datadepth: {
      deep: true,
      handler(current) {
        this.resize();
      }
    },
    data: {
      handler(current) {
        //console.log('current', current);
        this.onData(current);
      },
      deep: true
    },
    selected_subjects: {
      deep: true,
      handler(current) {
        var selectArray = [[], []];
        for(var i in current) {
          //selectArray[i] = current[i].ancestors(); //路径也红
          selectArray[i] = [current[i]];
        }
        var that = this;
        this.vis
          .selectAll("path")
          .filter(function(d){
            var name = that.recursiveName(d);
            for(var i in selectArray[0]) {
              if(that.recursiveName(selectArray[0][i]) == name) {
                return true;
              }
            }
            for(var i in selectArray[1]) {
              if(that.recursiveName(selectArray[1][i]) == name) {
                return true;
              }
            }
            return false;
            //return selectArray[0].indexOf(d) >= 0 || selectArray[1].indexOf(d) >= 0;
          })
          .transition()
          .attr("transform", function(d){
            //console.log('transform', d);
            //`rotate(${x - 90}) translate(${y},0) rotate(${x < 180 ? 0 : 180})`
            return `scale(${1.4 - d.depth * 0.1})`
          })
          .style("stroke", "gray");

        this.vis
          .selectAll("path")
          .filter(function(d){
            var name = that.recursiveName(d);
            for(var i in selectArray[0]) {
              if(that.recursiveName(selectArray[0][i]) == name) {
                return false;
              }
            }
            for(var i in selectArray[1]) {
              if(that.recursiveName(selectArray[1][i]) == name) {
                return false;
              }
            }
            return true;
            //return selectArray[0].indexOf(d) === -1 && selectArray[1].indexOf(d) === -1;
          })
          .transition()
          .attr("transform", `scale(1)`)
          .style("stroke", "none");
      }
    },
    colorGetter(value) {
      this.getPathes().style("fill", d => value(d.data));
    },

    minAngleDisplayed() {
      this.reDraw();
    },
  }
};
</script>

<style>
.graph {
  height: 100%;
  width: 100%;
  display: flex;
  flex-flow: column wrap;
}

.viewport {
  width: 100%;
  flex: 1 1 auto;
}
</style>
