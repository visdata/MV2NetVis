<template>
    <div style="height: 100%">
        <div style="height: 100%">
            <div class="mycard" :style="{'height':'100%', 'padding':'5px', 'margin-right':'22px'}">
                <div :style="{'padding-top': '5px', 'padding-left':'5px'}">
                    <p class="mytitle">Fiber Tract Detail</p>
                </div>
                <div class="mypannel" style="text-align: left; height: 30px; margin-top: -10px;  margin-bottom: 1px;">
                    <ui-select
                        label="Feature Type: "
                        name="fucValue"
                        :options="fuc"
                        v-model="fucValue"
                        style="text-align: left; float: left;"
                    ></ui-select>
                    <ui-select
                        label="Stat.: "
                        name="methodValue"
                        :options="method"
                        v-model="methodValue"
                        style="text-align: left; float: left;"
                    ></ui-select>
                    <!--div style="height: 30px; float: left;margin-right: 10px;">
                        <div v-for="pro in fuc" :key="pro.id" style="margin-bottom:0px; float: left;">
                            <RadioButton name="group2" :inputId="pro.id" :value="pro.id" v-model="fucValue"></RadioButton>
                            <Label for="pro">{{pro.name}}</Label>
                        </div>
                    </div-->
                </div>
                <div style="height: 287px; overflow: hidden; border-bottom: 1px solid rgba(0, 0, 0, 0.1); float: left; width: 100%;">
                    <div style="text-align: center; font-size: 14px;" v-if="g1name">{{g1name}} Group: #{{beginROI}}<->#{{endROI}}</div>
                    <!--div style="position: absolute; z-index: 999; top: 320px; left: 20px;">
                        <span style="width: 16px; height: 16px; display: inline-block; background-color: #775AA9; border-radius: 50%"></span>
                        <span style="font-size: 10px; vertical-align: text-bottom;">{{selected_subjects.length > 0?selected_subjects[0].data.firstName : ""}}</span>
                    </div-->
                    <img v-if="g1name" :style="{'height':'257px', 'padding-left': '20px'}" :src="require('../assets/'+ picBgOne + '.png')"></img>
                    <div class="legend1">
                        <div style="margin-left: -50%; margin-bottom: 4px;">{{methodValue.label}}</div>
                        <div>
                            <div id="lengendPurple"></div>
                            <div style="float: left; height: 79px; margin-left: 2px;">
                                <div class="spanLeft">1</div>
                                <div class="spanMiddle">0.22</div>
                                <div class="spanRight">
                                    <div style="position: absolute; bottom: 0px;">0.006</div>
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
                <div style="height: 287px; overflow: hidden;float: left; width: 100%;">
                    <div style="text-align: center; padding-top: 10px; font-size: 14px;" v-if="g2name">{{g2name}} Group: #{{beginROI}}<->#{{endROI}}</div>
                    <!--div style="position: absolute; z-index: 999; top: 606px; left: 20px; ">
                        <span style="width: 16px; height: 16px; display: inline-block; background-color: #FE8F00; border-radius: 50%"></span>
                        <span style="font-size: 10px; vertical-align: text-bottom;">{{selected_subjects.length > 1 ? selected_subjects[1].data.firstName : ""}}</span>
                    </div-->
                    <img v-if="g2name" :style="{'height':'257px', 'padding-left': '20px'}" :src="require('../assets/'+ picBgTwo + '.png')"></img>
                    <div class="legend2">
                        <div style="margin-left: -50%; margin-bottom: 4px;">{{methodValue.label}}</div>
                        <div>
                            <div id="lengendOrange"></div>
                            <div style="float: left; height: 79px; margin-left: 2px;">
                                <div class="spanLeft">1</div>
                                <div class="spanMiddle">0.22</div>
                                <div class="spanRight">
                                    <div style="position: absolute; bottom: 0px;">0.006</div>
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
                <!--img :style="{'height':'215px'}" :src="backgroundPic"></img-->
            </div>
        </div>
    </div>
</template>

<script>
    //echarts
    import echarts from 'echarts'
    import 'echarts/lib/chart/scatter'
    import 'echarts/lib/component/tooltip'
    import 'echarts/lib/component/legend'
    import 'echarts/lib/component/title'
    import 'echarts/lib/component/visualMap'
    import dataGender from "../../data/dataGender"
    import dataSicken from "../../data/dataSicken"
    import {
        //breadcrumbTrail,
        highlightOnHover,
        nodeInfoDisplayer,
        sunburst,
        zoomOnClick
    } from 'vue-d3-sunburst'
    import "vue-d3-sunburst/dist/vue-d3-sunburst.css"
    import {UiCheckbox, UiRadioGroup, UiButton} from 'keen-ui'
    import NetService from '@/services/net-service'
    import markedOnClick from './markedOnClick.js'
    import { mapState } from 'vuex'
    import slider from './slider'
    import verticalSlider from './verticalSlider'
    import breadcrumbTrailTwo from './breadcrumbTrailTwo'
    import breadcrumbTrail from './breadcrumbTrail'
    import sunburstTwo from './sunburstTwo'
    import { UiAutocomplete, UiCollapsible, UiAlert, UiSelect, UiDatepicker} from 'keen-ui'
    //import backgroundPic from "../assets/healthy_AD15_29_2.png"

    export default {
        name: "threeDView",
        components: {
            breadcrumbTrail,
            highlightOnHover,
            nodeInfoDisplayer,
            sunburst,
            zoomOnClick,
            UiRadioGroup,
            UiCheckbox,
            markedOnClick,
            UiButton,
            slider,
            verticalSlider,
            breadcrumbTrailTwo,
            sunburstTwo,
            UiAutocomplete,
            UiCollapsible,
            UiAlert,
            UiSelect,
            UiDatepicker,
        },
        data () {
            return {
                //backgroundPicOne: null,//require("../assets/healthy_AD15_29_2.png"),
                //backgroundPicTwo: null,//require("../assets/healthy_AD15_29_2.png"),
                fuc: [
                {
                    id: "0",
                    label: 'FA',
                    index: 4,
                },
                {
                    id: "1",
                    label: 'MD',
                    index: 5,
                },
                {
                    id: "2",
                    label: 'RD',
                    index: 6
                },
                {
                    id: "3",
                    label: "AXD",
                    index: 7,
                }
                ],
                fucValue: {
                    id: "0",
                    label: 'FA',
                    index: 0,
                },
                method: [
                {
                    id: "0",
                    label: 'p-value',
                    index: 2,
                },
                {
                    id: "1",
                    label: 'Mean',
                    index: 0,
                },
                {
                    id: "2",
                    label: 'Std.',
                    index: 1,
                },
                ],
                methodValue: {
                    id: "0",
                    label: 'p-value',
                    index: 2,
                },
            }
        },
        created() {
            //console.log('b2', this.picBgTwo);
            //this.backgroundPicOne = require("@/assets/healthy_AD15_29_2.png");
            //this.backgroundPicTwo = require(this.picBgTwo);
        },
        //mounted在html渲染出来后进行操作
        mounted() {
            
        },
        computed: {
            ...mapState(['picBgOne', 'picBgTwo', 'selected_subjects', 'beginROI', 'endROI', 'g1name', 'g2name']),
        },
        methods: {
        },
        watch: {
            fucValue: {
                deep: true,
                handler: function(newVal, oldVal) {
                    var index = this.methodValue.index * 8 + this.fucValue.index;
                    this.$store.commit('set_picindex', index);
                }
            },
            methodValue: {
                deep: true,
                handler: function(newVal, oldVal) {
                    var index = this.methodValue.index * 8 + this.fucValue.index;
                    this.$store.commit('set_picindex', index);
                }
            },
        }
    }
</script>
