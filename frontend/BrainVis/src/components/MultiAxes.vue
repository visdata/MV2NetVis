<template>
  <div>
    <div v-if="show_draw_area">
      <div v-for="plate in vue_data">
        <ui-collapsible open v-bind:title="plate.name+' Sector'" style="width:100%">
          <div
            v-for="stock in plate.data"
          >
            <factor-axis
              v-bind:input_data="stock"
              v-bind:key="key_count"
            >
            </factor-axis>
          </div>
        </ui-collapsible>
      </div>
    </div>

    <div v-if="show_frame_header_title" >
      <div class="frame_header" style="float:left;">
        <label style="width:100%; text-align:left;">Stock View</label>
      </div>
    </div>

  </div>
</template>

<script>
import * as d3 from "d3"
import {UiCollapsible} from 'keen-ui';
import FactorAxis from "@/components/FactorAxis";
import { mapState } from 'vuex';

export default {
  name:"MultiAxes",
  components: {
    UiCollapsible,
    FactorAxis,
  },
  data() {
    return {
      // data for vue element (v-for etc.)
      vue_data: [],
      key_count: 100000,
    }
  },
  computed: {
    ...mapState(['data_individuals']),
    show_frame_header_title(){
        if(this.data_individuals === undefined){
          return true;
        }
        else{
          return false;
        }
      },
      show_draw_area(){
        if(this.data_individuals === undefined){
          return false;
        }
        else{
          return true;
        }
      },
  },
  watch:{
      data_individuals: 'get_data',
      deep: true,
      immediate: true
  },
  methods:{
      get_data(){
        console.log("data_bunch_backtest:",this.$store.state.data_bunch_backtest);
        this.vue_data = [];
        Object.entries(this.data_individuals).forEach(([key, value]) => {
          this.vue_data.push({
            name: key,
            data: value
          })
        });
        this.key_count += 1;

        // Sort the stocks by error_abs in each plate.
        // this.sortStocks();
      },
      sortStocks() {
        this.vue_data.forEach(plate => {
          plate.data.sort((a, b) => {
            var a_err = this.getCumErr(a[Object.keys(a)[0]]);
            var b_err = this.getCumErr(b[Object.keys(b)[0]]);
            return a_err - b_err
          })
        })

      },
      getCumErr(stock) {
        var cum_error = d3.sum(stock, d => d.error_abs)
        return cum_error;
      }
  }
  // mounted() {
  //   this.$nextTick(function foo() {
  //     var that = this;
  //     d3.json("/static/temp(1).json", function(err, json) {
  //       if (err) {console.error(err);}
  //       console.log(json);
  //       Object.entries(json).forEach(([key, value]) => {
  //         console.log(key, value);
  //         that.vue_data.push({
  //           name: key,
  //           data: value
  //         })
  //       });
  //     })
  //   })
  // },
}
</script>

<style>
    .ui-collapsible__body{
        padding: 0%;
    }

    .ui-collapsible__header {
        font-size: 18px;
    }
</style>
