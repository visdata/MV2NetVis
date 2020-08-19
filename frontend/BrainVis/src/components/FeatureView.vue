<template>
    <div style="font-size:15px">
        <!-- subject selection row -->
        <div
            :style="{
                'margin-top': subject_select_row_margin_top + 'px',
                'min-height': subject_select_row_min_height +'px',
                }">
            <div> 
                <ui-select
                    floating-label
                    label="Select a subject"
                    :options="subjects"
                    v-model="subject_selection"
                    style="width: 90%;"
                ></ui-select>
            </div>
            <div> 
                <ui-button
                    color="primary"
                    @click="append_subject"
                    size="small"
                    icon="add"
                    style="width: 90%;"
                >
                add
                </ui-button>
            </div>
        </div>

        <!-- ROI input row -->
        <div
            :style="{
                'margin-top': roi_input_row_margin_top + 'px',
                'min-height': roi_input_row_min_height +'px',
                }">
            <div> 
                <div>beginROI: <br><input v-model = 'beginROI'></div>
                <div>endROI: <br><input v-model = 'endROI'></div>
            </div>
        </div>

        <!-- feature selete row -->
        <div id="strength"
            :style="{
                'margin-top': feature_select_row_margin_top + 'px',
                'min-height': feature_select_row_min_height +'px',
                'display': 'inline',
            }">
            <ui-radio-group
                name="strength_selection"
                :options="strength_options"
                v-model="strength_selection"
            >
            Choose strength
            </ui-radio-group>
        </div>
        <div id="length">
            <div
            :style="{
                'margin-top': feature_select_row_margin_top + 'px',
                'min-height': feature_select_row_min_height +'px',
                'display': 'inline',
            }">
            <ui-radio-group
                name="length_selection"
                :options="length_options"
                v-model="length_selection"
            >
            Choose length
            </ui-radio-group>
            </div>
        </div>
        <div id="curvature">
            <div
            :style="{
                'margin-top': feature_select_row_margin_top + 'px',
                'min-height': feature_select_row_min_height +'px',
                'display': 'inline',
            }">
            <ui-radio-group
                name="curvature_selection"
                :options="curvature_options"
                v-model="curvature_selection"
            >
            Choose curvature
            </ui-radio-group>
            </div>
        </div>
        <div id="torsion">
            <div
            :style="{
                'margin-top': feature_select_row_margin_top + 'px',
                'min-height': feature_select_row_min_height +'px',
                'display': 'inline',
            }">
            <ui-radio-group
                name="torsion_selection"
                :options="torsion_options"
                v-model="torsion_selection"
            >
            Choose torsion
            </ui-radio-group>
            </div>
        </div>
        <div id="entropy">
            <div
            :style="{
                'margin-top': feature_select_row_margin_top + 'px',
                'min-height': feature_select_row_min_height +'px',
                'display': 'inline',
            }">
            <ui-radio-group
                name="entropy_selection"
                :options="entropy_options"
                v-model="entropy_selection"
            >
            Choose entropy
            </ui-radio-group>
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
    //请求后端服务
    import NetService from '@/services/net-service';
    import { mapState } from 'vuex';

    export default {
        name: 'FeatureView',
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
                subjects: [],
                subject_selection: '',
                disabled_draw_btn: false,
                disabled_backtext_btn: false,
                beginROI: 0,
                endROI: 0,
                curvature_selection: 'all',
                curvature_options: [
                    {
                        label: 'All',
                        value: 'all'
                    },
                    {
                        label: 'High',
                        value: 'high'
                    },
                    {
                        label: 'Middle',
                        value: 'middle'
                    },
                    {
                        label: 'Low',
                        value: 'low'
                    },
                ],
                torsion_selection: 'all',
                torsion_options: [
                    {
                        label: 'All',
                        value: 'all'
                    },
                    {
                        label: 'High',
                        value: 'high'
                    },
                    {
                        label: 'Middle',
                        value: 'middle'
                    },
                    {
                        label: 'Low',
                        value: 'low'
                    },
                ],
                entropy_selection: 'all',
                entropy_options: [
                    {
                        label: 'All',
                        value: 'all'
                    },
                    {
                        label: 'High',
                        value: 'high'
                    },
                    {
                        label: 'Middle',
                        value: 'middle'
                    },
                    {
                        label: 'Low',
                        value: 'low'
                    },
                ],
                length_selection: 'all',
                length_options: [
                    {
                        label: 'All',
                        value: 'all'
                    },
                    {
                        label: 'High',
                        value: 'high'
                    },
                    {
                        label: 'Middle',
                        value: 'middle'
                    },
                    {
                        label: 'Low',
                        value: 'low'
                    },
                ],
                strength_selection: 'all',
                strength_options: [
                    {
                        label: 'All',
                        value: 'all'
                    },
                    {
                        label: 'High',
                        value: 'high'
                    },
                    {
                        label: 'Middle',
                        value: 'middle'
                    },
                    {
                        label: 'Low',
                        value: 'low'
                    },
                ],
                // layout params
                subject_select_row_min_height: 100,
                subject_select_row_margin_top: 10,
                roi_input_row_min_height: 100,
                roi_input_row_margin_top: 10,
                feature_select_row_min_height: 70,
                feature_select_row_margin_top: -20
            }
        },
        mounted(){
            this.$nextTick(()=>{
                this.init();
            })
        },
        computed: {
            ...mapState(['height', 'stock_list']),

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
                let top_parts_height = this.subject_select_row_min_height + this.roi_input_row_min_heigth + 
                    feature_select_row_min_height * 5;
                let top_parts_margin = this.subject_select_row_margin_top + this.roi_input_row_margin_top + 
                    this.feature_select_row_margin_top * 5;
                return this.height - top_parts_height - top_parts_margin - 56 - 30;
            }
        },
        methods: {
            init(){
                NetService.all_industries((res, err) => {
                    if(err){
                        console.error(err);
                        return;
                    }
                    if(!res.data){
                        console.log('data not exists!');
                        return;
                    }
                    this.subjects = res.data;
                });
            },

            append_subject(){

            },

            draw(){
               
            },
        }
    }
</script>
