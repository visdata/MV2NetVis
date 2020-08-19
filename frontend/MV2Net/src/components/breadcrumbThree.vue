<template>
  <div class="bread-sequence">
  </div>
</template>
<script>
import { select } from "d3-selection";

/**
 * Generic breadcrumb trailtwo component.
 */
export default {
  name: "breadcrumbThree",
  props: {
    /**
     * Items to be displayed
     */
    items: {
      required: true,
      type: Array
    },
    /**
     * Current item
     */
    from: {
      required: false,
      type: Object
    },
    /**
     * End text
     */
    endText: {
      required: false,
      type: String,
      default: ""
    },
    /**
     *  ColorGetter exposed by sunburst
     */
    colorGetter: {
      required: true,
      type: Function
    },
    /**
     *  Sunburst width
     */
    width: {
      required: false,
      type: Number
    },
    /**
     *  Bread crumb item width
     */
    itemWidth: {
      required: false,
      type: Number,
      default: 80
    },
    /**
     * Bread crumb item height
     */
    itemHeight: {
      required: false,
      type: Number,
      default: 23
    },
    /**
     * Spacing between breadcrumb items
     */
    spacing: {
      required: false,
      type: Number,
      default: 0
    },
    /**
     * With of tailing element
     */
    tailWidth: {
      required: false,
      type: Number,
      default: 5
    }
  },
  mounted() {
    var trailtwo = select(this.$el)
      .append("svg:svg")
      .attr("width", this.width)
      .attr("height", 20)
      .attr("class", "trailtwo")
      .style("overflow", "visible");
    trailtwo
      .append("svg:text")
      .attr("class", "endlabel")
      //.style("fill", "#8998AC")
      .style("font-size", "14px");
  },
  methods: {
    /**
     * @private
     */
    breadcrumbPoints(_, i) {
      var points = [];
      points.push("0,0");
      points.push(this.itemWidth + ",0");
      points.push(this.itemWidth + this.tailWidth + "," + this.itemHeight / 2);
      points.push(this.itemWidth + "," + this.itemHeight);
      points.push("0," + this.itemHeight);
      if (i > 0) {
        // Leftmost breadcrumb; don't include 6th vertex.
        points.push(this.tailWidth + "," + this.itemHeight / 2);
      }
      return points.join(" ");
    },

    /**
     * @private
     */
    draw() {
      if (this.items.length === 0) {
        select(this.$el)
          .select(".trailtwo")
          .style("visibility", "hidden");
        return;
      }
      

      //item长度
      var itemsLen = this.items.length;
      var name = "";
      var peopleNum = 0;
      for(var i in this.items) {
        if (i != 0 && i != itemsLen-1)
          name = name + this.items[i].name + '/';
        if (i == itemsLen - 1){
          name = name + this.items[i].name;
          //console.log("items", this.items[i]);
          peopleNum = this.items[i].data.size;
        }
          
      }

      //this.items = [this.items[itemsLen - 1]];
      //itemsLen = this.items.length;

      // Data join; key function combines name and depth (= position in sequence).
      var trailtwo = select(this.$el)
        .select(".trailtwo")
        .selectAll("g")
        .data(this.items, d => d.id);

      // Remove exiting nodes.
      trailtwo.exit().remove();

      // Add breadcrumb and label for entering nodes.
      var entering = trailtwo.enter().append("svg:g");

      entering
        .append("svg:polygon")
        .attr("points", this.breadcrumbPoints)
        .style("fill", this.colorGetter);

      var that = this;
      //添加字体
      
      entering
        .append("svg")
        .attr("x", this.tailWidth)
        .attr("y", 0)
        .attr("width", this.itemWidth - this.tailWidth)
        .attr("heigth", this.itemHeight)
        .insert("svg:text")
        .attr("x", this.itemWidth / 2)
        .attr("y", this.itemHeight / 2)
        .attr("dy", "0.25em")
        .attr("text-anchor", "middle")
        .style('fill', 'white')
        .each(function(d){
          console.log('entering', d);
        })
        .text(function(d,index){
          //return d.name.substr(0,2).toUpperCase();
          return d.firstName;
        });


        
      /*
      entering
        .append("svg")
        .attr("x", (that.itemWidth - this.tailWidth)/2)
        .attr("y", 0)
        .attr("width", function(d,index) {
          if(index == itemsLen-1) {
            return that.itemWidth * itemsLen - that.tailWidth;
          }
          return that.itemWidth - that.tailWidth;
        })
        .attr("heigth", this.itemHeight)
        .insert("svg:text")
        .attr("x", this.itemWidth / 2)
        .attr("y", this.itemHeight / 2)
        .attr("dy", "0.25em")
        .attr("text-anchor", "end") //字体对齐
        .each(function(d){
          console.log('entering', d);
        })
        .text(function(d,index){
          if(index == itemsLen-1) {
            return name;
          }
          return "";
          return d.name.substr(0,2);
        });
      */  
      let originIndex = this.from === null ? 0 : this.items.indexOf(this.from);
      originIndex = originIndex < 0 ? this.items.length : originIndex;
      // Merge enter and update selections; set position for all nodes.
      entering
        .merge(trailtwo)
        .attr(
          "transform",
          (d, i) => "translate(" + i * (this.itemWidth + this.spacing) + ", 0)"
        )
        .style(
          "opacity",
          d => (this.items.indexOf(d) >= originIndex ? 1 : 0.5)
        );

      // Now move and update the percentage at the end.
      select(this.$el)
        .select(".trailtwo")
        .select(".endlabel")
        .attr("x",this.items.length * (this.itemWidth + this.tailWidth / 2) + this.spacing * 2 + 2)
        .attr("y", this.itemHeight / 2)
        .attr("dy", "0.25em")
        .attr("text-anchor", "left")
        .text(function(d){
          return ": " + peopleNum.toString() + " subs";
        });


      select(this.$el)
        .select(".trailtwo")
        .style("visibility", "");
    }
  },
  watch: {
    width() {
      select(this.$el)
        .select(".trailtwo")
        .attr("width", this.width);
    },
    items: {
      deep: true,
      handler() {
        this.draw();
      }
    },
    from() {
      //console.log('from change');
      this.draw();
    }
  }
};
</script>
<style>
.bread-sequence {
  width: 50%;
  margin: 0px;
  overflow: visible;
}
svg
{
  overflow: visible;
}
text 
{
  font-size: 12px;

}
</style>
