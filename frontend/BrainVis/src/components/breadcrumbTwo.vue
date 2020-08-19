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
  name: "breadcrumbTwo",
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
      default: 30
    },
    /**
     * Spacing between breadcrumb items
     */
    spacing: {
      required: false,
      type: Number,
      default: 3
    },
    /**
     * With of tailing element
     */
    tailWidth: {
      required: false,
      type: Number,
      default: 10
    }
  },
  mounted() {
    var trailtwo = select(this.$el)
      .append("svg:svg")
      .attr("width", this.width)
      .attr("height", 40)
      .attr("class", "trailtwo")
      .style("overflow", "visible");
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
      console.log('this.items', this.items);
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
        .each(function(d){
          console.log('entering', d);
        })
        .text(d => d.name);

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
      console.log('from change');
      this.draw();
    }
  }
};
</script>
<style>
.bread-sequence {
  width: 50%;
  margin: 10px;
  overflow: visible;
}
text 
{
  font-size: 11px
}
</style>
