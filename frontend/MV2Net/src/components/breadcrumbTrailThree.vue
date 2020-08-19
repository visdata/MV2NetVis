<template>
  <breadcrumbThree
    :style="{'order':order, 'margin': '0px', 'margin-bottom':'4px'}"
    v-bind="{colorGetter, endText, from: fromItem, itemHeight, itemWidth, items, spacing, tailWidth, width}"
  />
</template>
<script>
import breadcrumbThree from "./breadcrumbThree";
import { mapState } from 'vuex';

/**
 * Breadcrumb trail component displaying path between root node and current node.
 * Can be used as a "legend" slot of sunburst component.
 */
export default {
  name: "breadcrumbTrailThree",
  components: {
    breadcrumbThree
  },
  props: {
    /**
     *  Root node
     */
    root: {
      required: false,
      type: Object
    },
    /**
     *  Current node
     */
    current: {
      required: false,
      type: Object
    },
    /**
     *  Reference node, parents nodes of the from node will have an opacity below 1
     */
    from: {
      required: false,
      type: Object
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
     *  Css Order. If 1 the slot is displayed below the sunburst, if 0 the slot is displayed on top the sunburst
     */
    order: {
      required: false,
      type: Number,
      default: 1
    },
    /**
     *  Bread crumb item width
     */
    itemWidth: {
      required: false,
      type: Number,
      default: 28 //100
    },
    /**
     * Bread crumb item height
     */
    itemHeight: {
      required: false,
      type: Number,
      default: 20
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
    //右边小三角的长度
    tailWidth: {
      required: false,
      type: Number,
      default: 0
    },
    index: {
      required: false,
      type: Number,
    }
  },
  computed: {
    ...mapState(['selected_subjects']),
    items() {
      //假设current为空返回空否则获取其先辈资料
      //const { current } = this;
      var tmp = this.selected_subjects[this.index];
      //console.log('current', tmp);
      //this.changePath = false;
      /*
      if(current){
        if(current.height == 0 && current.depth != 0) {
          var n = current;
          var name = n.data.name;
          while(n.parent != null){
            n = n.parent;
            name = n.data.name + '-' + name;
          }
          if((this.selected_subjects.length) % 2 == this.index)
            this.$store.commit('add_selected_subjects', name);
          //假设变化,1变为当前，-1变为空          
        }
      }
      console.log('selected_subjects_change', this.selected_subjects_change[this.index]);
      if (this.selected_subjects_change[this.index] == -1) {
        this.changePath = true;
        return [];
      }
      else if (this.selected_subjects_change[this.index] == 1) {
        this.changePath = true;
      }
      else {
        this.changePath = false;
      }*/
      return !tmp
        ? []
        : tmp
            .ancestors()
            .reverse()
            .filter(function(d, index) {
              //只剩最后一个
              return d.height == 0; 
              //去掉第一个
              return index != 0;
            })
            .map(d => ({
              id: d.data.name + d.depth,
              data: d.data,
              name: d.data.name,
              depth: d.depth,
              index: d.data.index,
              peer: d.data.peer,
              firstName: d.data.firstName,
            }));
    },
    fromItem() {
      const { from } = this;
      if (!from) {
        return null;
      }
      return this.items.find(it => it.data === from.data);
    },
    endText() {
      const { current } = this;
      if (!current) {
        return "";
      }
      const percentage = (100 * current.value) / this.root.value;
      return `${percentage.toPrecision(3)}% of total`;
    }
  },
  watch: {
    
  }
};
</script>
