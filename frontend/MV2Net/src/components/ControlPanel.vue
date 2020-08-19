<template>
    <div style="font-size:15px">
        <!-- industry selection row -->
        <div
            :style="{
                'margin-top':industry_select_row_margin_top + 'px',
                'min-height': industry_select_row_min_height +'px',
                }">
            <div> 
                <ui-select
                    floating-label
                    label="Select a industry"
                    :options="industries"
                    v-model="industry_selection"
                    style="width: 90%;"
                ></ui-select>
            </div>
            <div> 
                <ui-button
                    color="primary"
                    @click="append_industry"
                    size="small"
                    icon="add"
                    style="width: 90%;"
                >
                add
                </ui-button>
            </div>
        </div>

        <!-- stock selection row -->
        <!-- <div style="width: 97%; height: 7%;" > -->
        <div
            :style="{
                'margin-top':stock_select_row_margin_top + 'px',
                'min-height': stock_select_row_min_height +'px',
                }">
            <div> 
            <!-- <div style="border-radius: 5px; background-color: grey; width: 53%; height: 47%; float:left; margin-top: 4%;">
                <p style="font-weight: bold; font-size: 24px; color: white; padding-left: 10px;">Exchanges List</p>
            </div> -->
            <ui-autocomplete
                floating-label
                label="Enter stock security"
                name="stock_security"
                :suggestions="stock_securities"
                v-model="stock_security"
                style="width: 90%;"
            >
            </ui-autocomplete>
            </div>
            <div> 
                <ui-button
                    color="primary"
                    size="small"
                    icon="add"
                    @click="append_stock"
                    style="width: 90%;"
                >
                add
                </ui-button>
            </div>
        </div>

        <!-- date picker row -->
        <div
            :style="{
                'margin-top':date_picker_row_margin_top + 'px',
                'min-height': date_picker_row_min_height +'px',
            }">
            <div> 
                <ui-datepicker
                    placeholder="Select a start date"
                    v-model="start_date"
                    style="width: 90%; "
                >
                Start Date
                </ui-datepicker>
            </div>
            <div> 
                <ui-datepicker
                    placeholder="Select a end date"
                    v-model="end_date"
                    style="width: 90%;"
                >
                End Date
                </ui-datepicker>
            </div>
        </div>

        <!-- model select row -->
        <div
            :style="{
                'margin-top':model_select_row_margin_top + 'px',
                'min-height': model_select_row_min_height +'px',
            }">
            <ui-radio-group
                name="model_selection"
                :options="model_options"
                v-model="model_selection"
            >
            Choose Model
            </ui-radio-group>
        </div>

        <!-- table row -->
        <div 
            style="overflow:auto;"
            :style="{'height':table_row_height + 'px'}">
            <div
                v-for="(ts_codes, industry, iindex) in get_stocks_with_type"
                v-bind:key="iindex"
            >
                <ui-collapsible v-bind:title="industry" style="width: 90%;">
                    <div
                        class="row"
                        v-for="(ts_code, index) in ts_codes"
                        v-bind:key="index"
                    >
                        <ui-alert
                            @dismiss="drop_stock(ts_code)"
                            remove-icon v-show="true"
                            style="
                                margin-left:5%;
                                margin-right:5%;
                                margin-bottom:1%;
                                "
                        >
                            {{ts_code}}
                        </ui-alert>
                    </div>
                </ui-collapsible>
            </div>
        </div>
        
        <!-- confirm row -->
        <ui-button
            color="primary"
            icon="arrow_forward"
            iconPosition="right"
            size="normal"
            style="position:absolute; width: 80%;"
            :style="{'top': control_panel_height - 36 - 20 +'px'}"
            :disabled="get_disabled_draw_btn"
            @click="draw"
        >
        draw
        </ui-button>
    </div>
</template>
<script>
    import { UiAutocomplete, UiCollapsible, UiAlert, UiSelect, UiButton, UiDatepicker, UiRadioGroup} from 'keen-ui';
    import NetService from '@/services/net-service';
    import { mapState } from 'vuex';

    export default {
        name: 'ControlPanel',
        components: {
            UiAutocomplete,
            UiCollapsible,
            UiAlert,
            UiSelect,
            UiButton,
            UiDatepicker,
            UiRadioGroup,
        },
        data() {
            return {
                industries: [],
                // 不分类别
                stock_securities: [],
                // store the selection temporary
                stock_security: null,
                // store the selection temporary
                industry_selection: '',
                start_date: new Date(2015, 0, 1),
                end_date: new Date(2016, 0, 1),
                disabled_draw_btn: false,
                disabled_backtest_btn: false,
                model_selection: 'lasso_cv',
                model_options: [
                    {
                        label: 'LassoCV',
                        value: 'lasso_cv'
                    },
                    {
                        label: 'ELNetApp',
                        value: 'elastic_net'
                    },
                    {
                        label: 'Lasso',
                        value: 'lasso'
                    },
                ],
                // layout params
                industry_select_row_min_height: 100,
                industry_select_row_margin_top: 10,
                stock_select_row_min_height: 100,
                stock_select_row_margin_top: 10,
                date_picker_row_min_height: 150,
                date_picker_row_margin_top: 30,
                model_select_row_min_height: 70,
                model_select_row_margin_top: -20,
            }
        },
        mounted(){
            this.$nextTick(()=>{
                this.init();
            })
        },
        computed: {
            ...mapState(['height', 'stock_list']),
            get_stocks_with_type(){
                let res = {};
                let a = this.stock_list;
                for(let i in a){
                    let industry = a[i].industry;
                    if(res[industry] === undefined){
                        res[industry] = [];
                    }
                    res[industry].push(a[i].ts_code);
                }
                return res;
            },

            get_disabled_draw_btn(){
                return this.disabled_draw_btn;
            },

            get_disabled_backtest_btn(){
                return this.disabled_backtest_btn;
            },

            control_panel_height(){
                return this.height;
            },

            table_row_height(){
                let top_parts_height = this.industry_select_row_min_height + this.stock_select_row_min_height +
                    this.date_picker_row_min_height + this.model_select_row_min_height;
                let top_parts_margin = this.industry_select_row_margin_top + this.stock_select_row_margin_top + 
                    this.date_picker_row_margin_top + this.model_select_row_margin_top;

                return this.height - top_parts_height - top_parts_margin - 56 - 30;
            }
        },
        methods: {
            init(){
                NetService.all_securities((res, err) => {
                    if(err){
                        console.error(err);
                        return;
                    }
                    if(!res.data){
                        console.log('data not exists!');
                        return;
                    }
                    this.stock_securities = res.data;
                });
                NetService.all_industries((res, err) => {
                    if(err){
                        console.error(err);
                        return;
                    }
                    if(!res.data){
                        console.log('data not exists!');
                        return;
                    }
                    this.industries = res.data;
                });
            },

            append_industry(){
                NetService.get_stocks_info(this.industry_selection, (res, err) => {
                    if(err){
                        console.error(err);
                        return;
                    }
                    if(!res.data){
                        console.log('data not exists!');
                        return;
                    }

                    for(let i in res.data){
                        this.$store.commit('append_stock', res.data[i]);
                    }
                });
            },

            append_stock() {
                NetService.get_stock_info(this.stock_security, (res, err) => {
                    if(err){
                        console.error(err);
                        return;
                    }
                    if(!res.data){
                        console.log('data not exists!');
                        return;
                    }

                    this.$store.commit('append_stock', res.data);
                });
            },

            // 根据股票代码删除
            drop_stock(ts_code){
                this.$store.commit('drop_stock', ts_code);
            },
            

            draw(){
                var that = this;
                var prefixInteger=(num, n)=>{
                    return (Array(n).join(0) + num).slice(-n);
                }
                new Promise(function(resolve){

                    that.disabled_draw_btn = true;

                    if(that.stock_list.length <=0 ){
                        alert("Please choose some securities!");
                        resolve();
                    }
                    else if(!(that.start_date instanceof Date) || !(that.end_date instanceof Date)){
                        alert("Please choose proper date range!");
                        resolve();
                    }
                    else{

                        let start_date = that.start_date.getFullYear() + '-' + prefixInteger(that.start_date.getMonth() + 1, 2) + '-01' ;
                        let end_date = that.end_date.getFullYear() + '-' + prefixInteger(that.end_date.getMonth() + 1, 2) + '-01';
                        if(start_date >= end_date){
                            alert("Please choose proper date range!");
                            resolve();
                        }
                        else{
                            that.$store.commit('start_date', start_date);
                            that.$store.commit('end_date', end_date);

                            let kwargs = {};
                            let global_index = 0;
                            for(let i in that.stock_list){
                                let ts_code = that.stock_list[i]['ts_code'];
                                let industry = that.stock_list[i]['industry'];
                                // kwargs[industry + '_' + (that.stock_list.length - 1 - i)] = ts_code
                                kwargs[global_index + '_' + industry + '_' + i] = ts_code
                                global_index += 1;
                            }
                            kwargs["train_model"] = that.model_selection;
                            console.log("kwargs: ", kwargs);

                            NetService.get_prediction_data(start_date, end_date, kwargs, (res, err) => {
                                if(err){
                                    console.error(err);
                                    resolve()
                                    return;
                                }
                                if(!res.data){
                                    console.log('data not exists!');
                                    resolve()
                                    return;
                                }
                                that.$store.commit('draw_overview', res.data["overview"]);
                                that.$store.commit('draw_individuals', res.data["individuals"]);
                                that.$store.commit('draw_aggragate', res.data["overview_aggregate"]);
                                that.$store.commit('reset_selected_factors');
                                that.$store.commit('reset_selected_stocks');
                                that.$store.commit('reset_bunch_backtest');
                                that.$store.commit('reset_backtest');
                                resolve()
                            });
                        }
                    }
                }).then(function(){
                    that.disabled_draw_btn = false;
                })
            },
        }
    }
</script>
