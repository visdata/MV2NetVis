<template>
  <div
    class="infornation-sunburst"
    v-if="show"
  >
    <span>{{displayPercentage}}</span>
    <br/>
    <span>subjects</span>
    <!--
    <br /> {{description}}
    <br /><span
      v-if="showAllNumber"
      class="detail"
    >({{current.value}} / {{base}})</span>
  -->
  </div>
</template>
<script>
/**
 * Component that display the percentage value of the current node relative to root.
 * Can be used as a "top" slot of sunburst component.
 */
export default {
  name: "nodeInfoDisplayerTwo",
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
     *  Clicked node
     */
    clicked: {
      required: false,
      type: Object
    },
    /**
     *  Text to be displayed
     */
    description: {
      required: true,
      type: String
    },
    /**
     *  Show fraction format of size if true
     */
    showAllNumber: {
      required: false,
      type: Boolean,
      default: true
    }
  },
  computed: {
    /**
     * @private
     */
    base() {
      return this.clicked ? this.clicked.value : this.root.value;
    },
    percentage() {
      //显示所有值
      if(this.root == null) return null;
      return this.base;
      if (this.current == null || this.root == null) {
        return null;
      }
      //显示百分比
      return (100 * this.current.value) / this.base;
    },
    displayPercentage() {
      const percentage = this.percentage;
      if(percentage == 0) return null;
      return percentage === null ? null : `${percentage}`;
      return percentage === null ? null : `${percentage.toPrecision(3)} %`;
    },
    show() {
      const percentage = this.percentage;
      //return percentage !== null;
      return percentage !== 0;
    }
  }
};
</script>
<style>

</style>
