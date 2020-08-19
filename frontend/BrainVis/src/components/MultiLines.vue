<template>
  <div class="overflow:auto; overflow-x:hidden">
    <div id="bars" class="draw-area" v-if="show_draw_area"
      style="float:left;width:79%; overflow:auto"
      :style="{'height': (multilines_height) + 'px'}"
      >
      <div v-for="plate in vue_data" >
        <ui-collapsible v-bind:title="plate.name+' Return'" v-bind:open="true" style="width:100%">
          <return-lines
            v-bind:input_data="plate.data"
            v-bind:key="key_count"
            v-on:SelectStock="selectStock"
          >
          </return-lines>
        </ui-collapsible>
      </div>
    </div>

    <div style="float:left;width:79%;height:100%" v-if="show_frame_header_title" >
    <div class="frame_header" style="float:left;">
      <!-- <label style="font-weight: bold; width:100%; text-align:left;">Stock Returns</label> -->
      <label style="width:100%; text-align:left;">Stock Return</label>
     </div>
    </div>

    <div class="frame_header" style="width:21%;float:left"
      :style="{'top':factor_aggragate_height}"
    >
        <label style="width:100%;padding-right:35%;text-align:right;">Portfolio</label>
    </div>

    <div style="float:left;width:21%; overflow:auto;margin-top:-2px;"
        :style="{'height': (multilines_height - frame_header_height - 32) + 'px'}"
    >
        <v-table
            is-horizontal-resize
            is-vertical-resize
            :title-row-height=0
            :row-click="del_table_row"
            style="width:100%;"
            :columns="columns"
            :table-data="tableData"
            row-hover-color="#eee"
        ></v-table>
    </div>
    <div style="float:left;width:21%;overflow:hidden">
        <ui-button
            color="primary"
            icon="arrow_downward"
            iconPosition="right"
            size="small"
            :disabled="get_disabled_draw_btn"
            @click="backtest"
            style="width:100%; height:32px"
            >
            BT
        </ui-button>
    </div>
  </div>
</template>

<script>
import * as d3 from "d3"
import { mapState } from 'vuex';
import NetService from '@/services/net-service';
import {UiCollapsible,UiButton} from 'keen-ui';
import ReturnLines from '@/components/ReturnLines';
import { VTable } from 'vue-easytable';
import C from './constants.js';

export default {
  name: "MultiLines",
  components: {
    UiCollapsible,
    UiButton,
    ReturnLines,
    VTable,
  },
  data() {
    return {
      // data for vue element (v-for etc.)
      vue_data: [],
      disabled_draw_btn: false,
      key_count: 2000000,
      columns: [
          {field: 'stock', title: 'Stocks', width: 50, titleAlign: 'center', columnAlign: 'center',isResize:true},
      ],
      frame_header_height: C.FRAME_HEADER_HEIGHT,
    }
  },
  computed: {
      ...mapState(['height', 'start_date', 'end_date', 'selected_factors', 'selected_stocks','data_bunch_backtest', 'data_backtest']),
      tableData(){
          let res = [];
          for(let i=0;i<this.selected_stocks.length;i++){
              res.push({"stock": this.selected_stocks[i]});
          }
          return res;
      },
      get_disabled_draw_btn(){
          return this.disabled_draw_btn;
      },
      multilines_height(){
          return Math.floor((this.height - 15) * 0.35);
      },
      factor_aggragate_height(){
          return Math.floor((this.height - 15) * 0.41);
      },
      show_frame_header_title(){
        if(this.data_bunch_backtest === undefined){
          return true;
        }
        else{
          return false;
        }
      },
      show_draw_area(){
        if(this.data_bunch_backtest === undefined){
          return false;
        }
        else{
          return true;
        }
      }
  },
  watch:{
      data_bunch_backtest: 'get_data',
      deep: true,
      immediate: true,
  },
  methods:{
      get_data(){
        if(this.data_bunch_backtest == undefined){
          return;
        }
        console.log("MultiLines: get_data");
        this.vue_data = [];
        var that = this;
        Object.entries(this.data_bunch_backtest).forEach(([key, value]) => {
          that.vue_data.push({
            name: key,
            data: value
          })
        });
        this.key_count += 1;
      },
      selectStock(arr){
        var name = arr[0];
        var selected = arr[1];
        // var index_in_list = this.selected_stocks.indexOf(name);
        // // If cancel selection and the stock is in list
        // if (!selected && index_in_list >= 0) {
        //   this.selected_stocks.splice(index_in_list, 1);
        // }
        // // If add selection and the stock is not in list
        // if (selected && index_in_list < 0) {
        //   this.selected_stocks.push(name);
        // }

        // If cancel selection and the stock is in list
        if (!selected) {
          this.$store.commit('rm_selected_stock', name);
        }
        else{
          this.$store.commit('add_select_stock', name);
        }
      },
      del_table_row(rowIndex, rowData, column) {
        console.log(rowData["stock"]);
        this.$store.commit('rm_selected_stock', rowData["stock"]);
      },
      backtest(){
          var that = this;
          var next_4monthday = (dt_str) => {
            let parts = dt_str.split("-");
            let dt = new Date(parseInt(parts[0]), parseInt(parts[1]) - 1, parseInt(parts[2]));
            dt.setDate(1);
            let new_month = dt.getMonth() + 4;
            let new_year;
            if(new_month > 11){
              new_year = dt.getFullYear() + 1;
            }
            else{
              new_year = dt.getFullYear();
            }
            return new_year + "-" + (new_month % 12 + 1) + "-01";
          }
          new Promise(function(resolve){
              that.disabled_draw_btn = true;
              // action
              let kwargs = {};
              for(let i in that.selected_stocks){
                  let ts_code = that.selected_stocks[i];
                  kwargs['ts_code_' + i] = ts_code;
              }
              for(let i in that.selected_factors){
                  let factor = that.selected_factors[i];
                  kwargs['factor_' + i] = factor;
              }
              kwargs["prediction_model"] = 'ols';

              NetService.get_backtest(that.start_date, next_4monthday(that.end_date), kwargs, (res, err) => {
                  if(err){
                      console.error(err);
                      resolve();
                      return
                  }
                  if(!res.data){
                      console.log('data not exists!');
                      resolve();
                      return;
                  }
                  res.data["selected_stocks"] = JSON.parse(JSON.stringify(that.selected_stocks));
                  res.data["selected_factors"] = JSON.parse(JSON.stringify(that.selected_factors));
                  that.$store.commit('draw_backtest', res.data);
                  resolve();
              });
            }).then(function(){
                that.disabled_draw_btn = false;
            })
      }
  }

  // mounted() {
  //   this.$nextTick(function foo() {
  //     var that = this;
  //     d3.json("/static/data_for_E.json", function(err, json) {
  //       if (err) {console.error(err);}

  //       Object.entries(json).forEach(([key, value]) => {
  //         if (key !== "market_index") {
  //           that.vue_data.push({
  //             name: key,
  //             data: value
  //           })
  //         }
  //         // console.log(that.vue_data);
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