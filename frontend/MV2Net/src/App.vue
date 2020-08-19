<template>
    <!-- 总共运用了7个组件，分别是nav-bar, control-panel, overview, multi-axes, factor-aggrate, multi-lines, backtest -->
    <div id="app"><!--min-width: 1920px; min-height: 910px;-->
        <!--<nav-bar></nav-bar>-->
        <!-- row类总共有三列 style="margin: auto; margin-top: 3px; margin-left: 9px;" -->

        <div class="row" :style="{'height': '270px', 'padding': '0px'}">
            <div class="col-3" style="padding: 0px; background: rgb(249, 250, 252);">
                <subject-view-v-two></subject-view-v-two>
            </div>
            <div class="col-9" style="padding: 0px; background: rgb(249, 250, 252);">
                <feature-view-four></feature-view-four>
            </div>
        </div>
        
        <div class="row"
            :style="{'margin-top': '12px', 'height':'620px'}" 
        >
            <div class="col-10" style="background: rgb(249, 250, 252); padding: 0px;">
                <comparison-view-version-five></comparison-view-version-five>
            </div>
            <div class="col-2" style="background: rgb(249, 250, 252); padding: 0px;">
                <three-d-view></three-d-view>
            </div>
        </div>
        
        <!-- <router-view/> -->
    </div>
</template>

<script>
    import 'keen-ui/dist/keen-ui.min.css'
    import 'bootstrap/dist/css/bootstrap.min.css'
    //7个组件的定义。驼峰命名法。使用时大写转小写且用-连接
    import NavBar from '@/components/NavBar'
    import ControlPanel from '@/components/ControlPanel'
    import Overview from '@/components/Overview'
    import Backtest from '@/components/Backtest'
    import MultiAxes from "@/components/MultiAxes"
    import FactorAggragate from "@/components/FactorAggragate"
    import MultiLines from "@/components/MultiLines"
    import FeatureView from "@/components/FeatureView"
    import ViewSelecte from "@/components/ViewSelecte"
    import echartsPie from "@/components/echartsPie"
    import echartsScatter from "@/components/echartsScatter"
    import echartsScatterVersionTwo from "@/components/echartsScatterVersionTwo"
    import echartsScatterVersionThree from "@/components/echartsScatterVersionThree"
    import echartsScatterVersionFour from "@/components/echartsScatterVersionFour"
    import echartsScatterVersionFive from "@/components/echartsScatterVersionFive"
    import echartsScatterVersionSix from "@/components/echartsScatterVersionSix"
    import echartsScatterVersionSeven from "@/components/echartsScatterVersionSeven"
    //引入数据
    import dataGender from "../data/dataGender"
    import dataSicken from "../data/dataSicken"
    import sideBySide from "@/components/sideBySide"
    import comparisonView from "@/components/comparisonView"
    import comparisonViewVersionTwo from "@/components/comparisonViewVersionTwo"
    import comparisonViewVersionThree from "@/components/comparisonViewVersionThree"
    import comparisonViewVersionFour from "@/components/comparisonViewVersionFour"
    import comparisonViewVersionFive from "@/components/comparisonViewVersionFive"
    //import strength from "../data/strength"
    import subjectView from "@/components/subjectView"
    import subjectViewVTwo from "@/components/subjectViewVTwo"
    import featureViewTwo from "@/components/featureViewTwo"
    import featureViewThree from "@/components/featureViewThree"
    import featureViewFour from "@/components/featureViewFour"
    import threeDView from "@/components/threeDView"

    export default {
        name: "App",
        components: {
            NavBar,
            ControlPanel,
            Overview,
            Backtest,
            MultiAxes,
            FactorAggragate,
            MultiLines,
            FeatureView,
            ViewSelecte,
            echartsPie,
            echartsScatter,
            sideBySide,
            echartsScatterVersionTwo,
            echartsScatterVersionThree,
            echartsScatterVersionFour,
            echartsScatterVersionFive,
            echartsScatterVersionSix,
            echartsScatterVersionSeven,
            comparisonView,
            comparisonViewVersionTwo,
            comparisonViewVersionThree,
            comparisonViewVersionFour,
            subjectView,
            subjectViewVTwo,
            threeDView,
            featureViewTwo,
            featureViewThree,
            featureViewFour,
            comparisonViewVersionFive,
        },
        data () {
            return {
                dataGender,
                dataSicken,
                options: {},
                setting: {
                    width: 0,
                    height: 0,
                    navbar_height: 0,
                },
                //strength,
            }
        },
        //mounted在html渲染出来后进行操作
        mounted() {
            this.resize();
            //nextTick在下次DOM更新结束后延时回调
            this.$nextTick(function foo() {
                this.initialize();
            });
        },
        methods: {
            initialize() {
                this.resize();
                //addEventListener注册监听，事件（“resize”窗口大小调整）发生就会执行回调
                window.addEventListener("resize", () => {
                    this.resize();
                });
            },

            resize(){
                //屏幕长宽的变化随着窗口变化，存储它的长宽
                this.setting.navbar_height = 0;//document.querySelector(".navbar").clientHeight;
                this.setting.height = window.innerHeight;//- document.querySelector(".navbar").clientHeight;
                this.setting.width = window.innerWidth;
                this.$store.commit('height', this.setting.height);
                this.$store.commit('width', this.setting.width);
            }
        }
    };
</script>

<style>
    ::-webkit-scrollbar{
        display: none;
    }

    #app {
        font-family: "Avenir", Helvetica, Arial, sans-serif;
        /*font-family: Roboto, -apple-system, BlinkMacSystemFont, "Segoe UI", Oxygen, Ubuntu, Cantarell, "Fira Sans", "Droid Sans", Helvetica, Arial, sans-serif, "Apple Color Emoji", "Segoe UI Emoji", "Segoe UI Symbol";*/
        font-family: -apple-system,BlinkMacSystemFont,Segoe UI,Roboto,Oxygen,Ubuntu,Cantarell,Fira Sans,Droid Sans,Helvetica,Arial,sans-serif;
        -webkit-font-smoothing: antialiased;
        -moz-osx-font-smoothing: grayscale;
        /*text-align: center;*/
        color: #2c3e50;
        /*overflow:hidden;*/
        /*overflow-y: scroll; 
        white-space: nowrap;*/
        background: rgb(249, 250, 252);
    }

    text {
        font-size: 15px
    }

    .frame_header{
        align-items: center;
        background-color: #eeeeee;
        display: flex;
        font-size: 15px;
        line-height: 1.5;
        margin: 0;
        min-height: 52px;
        padding: rem-calc(12px 16px);
        position: relative;
        width: 100%;
    }

    .frame_header label{
        font-size: 18px;
        margin-left: 16px;
        margin-top: 6px;
    }
/*
    .checkbox {
        width: 15px;
        height: 15px;
        border: 2px solid rgba(128, 128, 128, 0.6);
        border-radius: 3px 3px 3px 3px;
    }

    .checkbox-checked {
        background: rgba(128, 128, 128, 0.4);
    }

    .radiobutton {
        width: 15px;
        height: 15px;
        border: 2px solid rgba(128, 128, 128, 0.6);
    }

    .radiobutton-inner {
        background-color: rgba(128, 128, 128, 0.4);
    }*/
</style>
