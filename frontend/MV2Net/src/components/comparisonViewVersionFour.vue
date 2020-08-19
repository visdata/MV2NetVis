<template>
	<div style="height: 100%">
        <div style="padding: 0px;width: 74%; float: left; height: 100%;">
            <div style="height: 100%;">
                <div class="mycard" style="padding-right: 5px; height: 100%; margin-left: 30px; margin-right: 0px;">
                    <div :style="{'padding-top': '5px', 'padding-left':'5px'}">
                        <p class="mytitle">Network Comparison</p>
                    </div>
                    <div>
                        <div class="mypannel" style="float: left;width: 105%; margin-top: -10px;margin-bottom: 1px; font-size: 5px; margin-left: -5px;">
                            
                            <!--el-select v-model="showFunction" placeholder="请选择">
                                <el-option
                                  v-for="item in showFunctionPros"
                                  :key="item.index"
                                  :label="item.message"
                                  :value="item.index">
                                </el-option>
                            </el-select-->
                            <!--div class="ui-select__label-text" style="float: left;"></div-->
                            <ui-select
                                label="Method: "
                                name="showFunction"
                                :options="showFunctionPros"
                                v-model="showFunction"
                                style="text-align: left; float: left;"
                            ></ui-select>
                            <ui-select
                                label="View: "
                                name="eyesidePros"
                                :options="eyesidePros"
                                v-model="eyeside"
                                style="text-align: left; float: left;"
                            ></ui-select>
                            <span style="float: left; font-size: 13px; padding-top: 6px; margin-left: 7px; color: #8998AC; margin-right: -6px;">Univariate - </span>
                            <ui-select
                                label="Edge Stat.: "
                                name="linksPros"
                                :options="linksPros"
                                v-model="linksVal"
                                style="text-align: left; float: left;"
                            ></ui-select>
                            <ui-select
                                label="Algorithm: "
                                name="testValPros"
                                :options="testValPros"
                                v-model="testVal"
                                style="text-align: left; float: left;"
                            ></ui-select>
                            <span style="float: left; font-size: 13px; padding-top: 6px; margin-left: 7px; color: #8998AC; margin-right: -6px;">Multivariate - </span>
                            <ui-select
                                label="Edge Stat.: "
                                name="multiValPros"
                                :options="multiValPros"
                                v-model="multiVal "
                                style="text-align: left; float: left;"
                            ></ui-select>
                            <ui-select
                                label="Clustering: "
                                name="multiGroupValPros"
                                :options="multiGroupValPros"
                                v-model="multiGroupVal"
                                style="text-align: left; float: left;"
                            ></ui-select>
                            <!--div style="text-align: left; float: left;border-right: 1px solid rgba(0,0,0,0.1);">
                                <div v-for="pro in showFunctionPros" :key="pro.index" style="margin-bottom:0px; float: left; ">
                                    <RadioButton  name="showFunction" :inputId="pro.message" :value="pro.index" v-model="showFunction"></RadioButton>
                                    <Label for="pro">{{pro.message}}</Label>
                                </div>
                            </div>
                            <div style="float: left; padding-left: 10px; border-right: 1px solid rgba(0,0,0,0.1);">
                                <div v-for="pro in eyesidePros" :key="pro.index" style="margin-bottom:0px; float: left; ">
                                    <RadioButton  name="eyeside" :inputId="pro.message" :value="pro.index" v-model="eyeside"></RadioButton>
                                    <Label for="pro">{{pro.message}}</Label>
                                </div>
                            </div>
                            <div style="float: left; padding-left: 10px; border-right: 1px solid rgba(0,0,0,0.1);">
                                <div v-for="pro in linksPros" :key="pro.index" style="margin-bottom:0px; float: left; ">
                                    <RadioButton  name="group1" :inputId="pro.message" :value="pro.index" v-model="linksVal"></RadioButton>
                                    <Label for="pro">{{pro.message}}</Label>
                                </div>
                            </div>
                            <div style="float: left; padding-left: 10px;">
                            <div style="text-align: left; float: left; margin-right: 10px; border-right: 1px solid rgba(0,0,0,0.1)">
                                <div v-for="pro in testValPros" :key="pro.index" style="margin-bottom:0px; float: left;">
                                    <RadioButton  name="group2" :inputId="pro.message" :value="pro.index" v-model="testVal"></RadioButton>
                                    <Label for="pro">{{pro.message}}</Label>
                                </div>
                            </div>
                            <div style="text-align: left; float: left; margin-right: 10px; border-right: 1px solid rgba(0,0,0,0.1)">
                                <div v-for="pro in multiGroupValPros" :key="pro.index" style="margin-bottom:0px; float: left;">
                                    <RadioButton  name="multiGroupVal" :inputId="pro.message" :value="pro.index" v-model="multiGroupVal"></RadioButton>
                                    <Label for="pro">{{pro.message}}</Label>
                                </div>
                            </div>
                            <div style="text-align: left; float: left; margin-right: 10px;border-right: 1px solid rgba(0,0,0,0.1);">
                                <div v-for="pro in multiValPros" :key="pro.index" style="margin-bottom:0px; float: left;">
                                    <RadioButton  name="group5" :inputId="pro.message" :value="pro.index" v-model="multiVal"></RadioButton>
                                    <Label for="pro">{{pro.message}}</Label>
                                </div>
                            </div>
                            </div-->
                            <div style="float: left; height: 31px">
                            <!--div style="float: left; margin-right: 10px;">
                                <label style="float: left; margin-top: 5px; margin-right: 8px; margin-bottom: 0px;">lambda:</label>
                                <div :style="{'float':'left', 'margin-top':'5px'}">
                                    <slider :min=-0.000000001 :max=100 v-model = "lambda" :style="{'width': '100px'}"></slider>
                                </div>
                            </div-->
                            <div style="float: left; margin-right: 10px; margin-left: 5px; margin-top: 1px;">
                                <label style="float: left; margin-top: 5px; margin-right: 8px; margin-bottom: 0px; font-size: 10px; width: auto; color: #8998AC;">Z-value:</label>
                                <div :style="{'float':'left', 'margin-top':'8px'}">
                                    <slider :min=0 :max=100 v-model = "ROIZValue" :dataMin=0.0 :dataMax=5.0 :style="{'width': '55px'}"></slider>
                                </div>
                            </div>
                            <div style="float: left; margin-right: 10px; margin-top: 1px;">
                                <label style="float: left; margin-top: 5px; margin-right: 8px; margin-bottom: 0px; font-size: 10px; width: auto; color: #8998AC;">Radius:</label>
                                <div :style="{'float':'left', 'margin-top':'8px'}">
                                    <slider :min=0 :max=100 v-model = "DBScanRadius" :dataMin=0.0 :dataMax=44.38 :style="{'width': '55px'}"></slider>
                                </div>
                            </div>
                            </div>
                        </div>

                        <div style="float: left; padding-left: 5px;margin-top: -10px;">
                        <div style="height: 575px; width: 1105px;">
                            <happy-scroll color="rgba(0,0,0,0.5)" resize>
                                <div :style="{'width': (/*feature_values.length + */Math.max(singlefeatures.length, multifeatures.length)) * 220 + 'px', 'height': '570px'}">
                                    <vuedraggable :style="{'float':'left', 'width': Math.max(singlefeatures.length, multifeatures.length) * 220 + 'px'}" v-model="singlefeatures">
                                        <transition-group>
                                            <div v-for="(feature, index) in singlefeatures" :key="index">
                                                <div style="width: 220px; height: 555px;margin: auto; float: left;text-align: center;padding-bottom: 0px; z-index: 0;" :class="{'mySelected': index===selected_index}">
                                                    <!--div style="border: 1px solid rgba(0,0,0,0.05); border-bottom: none; color: #8998AC;">{{showFunctionPros[Number(feature.showFunction)].label}}</div-->
                                                    <div style="border: 1px solid rgba(0,0,0,0.05); border-bottom: none; position: relative;">
                                                        <loading v-bind="{'loading': feature.singleLoading}"></loading>
                                                        <button class="mydeletebtn" @click="deleteSingle(index)" :style="{'position': 'absolute', 'top':'5px', 'left': '200px',}">
                                                            ×
                                                        </button>
                                                        <div style="float: left; z-index: 999; width: 100%; text-align: center; padding-left: 5px; border-bottom: 1px solid rgba(0,0,0,0.05);">
                                                            <div>
                                                                <CheckBox :value="(index*2).toString()" name="group3" v-model="selectValue" :multiple="true"> </CheckBox>
                                                                <Label style="padding-right: 0px;">{{feature.showFunction == 0? "Univariate view" : (feature.g1name + " Group")}}</Label>
                                                                
                                                            </div>
                                                            <!--
                                                            <Label style="padding-right: 0px; margin-top: -17px; margin-left: 28px; position: absolute;">({{feature.g1name}},{{feature.g2name}})</Label>
                                                            <Label style="padding-right: 0px; margin-top: -4px; margin-left: 28px; position: absolute;">({{feature.singleFunc}})</Label>
                                                            -->
                                                        </div>
                                                        <div style="min-height: 275px;" @click="changeSelected(index)">
                                                            <div style="position: absolute; left: 171px; top: 244px;">
                                                                <CheckBox :value="index.toString()" name="sync + index.toString()" v-model="feature.sync" :multiple="true"></CheckBox>
                                                                <span style="font-size: 10px;color: #8998AC;">Sync</span>
                                                            </div>
                                                            <superposed-graph-version-five v-bind="{'strength':singlefeatures[index].data, 'filterdata':singlefeatures[index].filterSingleData, 'sidebyside':singlefeatures[index].sameFunc, 'sidebysideMax':singlefeatures[index].sidebysideMax, 'sidebysideMin':singlefeatures[index].sidebysideMin, 'sidebysideTrueMax':singlefeatures[index].sidebysideTrueMax, 'Func': singlefeatures[index].singleFunc, 'eyeside':singlefeatures[index].eyeside, 'g1name': singlefeatures[index].g1name, 'g2name': singlefeatures[index].g2name, 'selected': selectValue.indexOf((index * 2).toString()) != -1,'filterRate': feature.singlefilterRate}" :index='index.toString()' :heightPer='0.2' :featureName='feature.name'></superposed-graph-version-five>
                                                        </div>
                                                    </div>
                                                    <div style="border: 1px solid rgba(0,0,0,0.05); position: relative;">
                                                        <loading v-bind="{'loading': feature.showFunction == 0 ? feature.multiLoading : feature.singleLoading}"></loading>
                                                        <div v-show="feature.message != 'Success'">
                                                            <div class="message">{{feature.message}}</div>
                                                        </div>
                                                        <div style="float: left; z-index: 999; width: 100%; text-align: center; padding-left: 5px; border-bottom: 1px solid rgba(0,0,0,0.05);">
                                                            <div>
                                                                <CheckBox :value="(index*2+1).toString()" name="group3" v-model="selectValue" :multiple="true"> </CheckBox>
                                                                
                                                                <Label style="padding-right: 0px;">{{feature.showFunction == 0? ("Multivariate view:" ): (feature.g2name + " Group:")}}</Label>
                                                                <label>{{feature.filtermultidataLen[0]}}</label>
                                                            </div>
                                                            <!--Label style="padding-right: 0px; margin-top: -17px; margin-left: 14px; position: absolute;">({{feature.multiFunc}})</Label-->
                                                            <!--button class="btn btn-secondary mybtn" @click="deleteMulti(index)">
                                                                -
                                                            </button-->
                                                        </div>
                                                        <div style="min-height: 275px;" @click="changeSelected(index)">
                                                            <!--superposed-graph-version-three v-bind="{'strength':singlefeatures[index].data, 'filterdata':singlefeatures[index].filterSingleData}" :index='index.toString()' :heightPer='0.2' :featureName='feature.name'></superposed-graph-version-three-->
                                                            
                                                            <superposed-graph-for-multi-version-four v-bind="{'strength':singlefeatures[index].data, 'filterdata':singlefeatures[index].filterMultiData, 'sidebyside':singlefeatures[index].sameFunc, 'sidebysideMax':singlefeatures[index].sidebysideMax, 'sidebysideMin':singlefeatures[index].sidebysideMin, 'sidebysideTrueMax':singlefeatures[index].sidebysideTrueMax, 'Func': singlefeatures[index].multiFunc, 'eyeside':singlefeatures[index].eyeside, 'g1name': singlefeatures[index].g1name, 'g2name': singlefeatures[index].g2name, 'selected': selectValue.indexOf((index * 2 + 1).toString()) != -1, 'filterRate': feature.sync.length > 0 ? feature.singlefilterRate : feature.multifilterRate, 'filterdataLen':feature.filtermultidataLen}" :index='index.toString()' :heightPer='0.2' :featureName='feature.name'></superposed-graph-for-multi-version-four>
                                                        </div>
                                                    </div>
                                                </div>
                                            </div>
                                        </transition-group>
                                    </vuedraggable>
                            <!--
                            <div :style="{'border': '1px dotted rgba(255,0,0,0.5)', 'width' : feature_values.length * 184 + 1 + 'px', 'float':'left'}">
                            <div v-for="(feature_value, index) in feature_values" >
                                <div style="width: 184px;margin: auto; float: left;text-align: center;padding-bottom: 0px;">
                                    <div style="border: 1px solid rgba(0,0,0,0.05);">
                                        <div style="float: left; z-index: 999; width: 95px; text-align: left; padding-left: 5px;">
                                            <div>
                                            <button class="mybtn" @click="deleteValues(index)">
                                                ×
                                            </button>
                                            <CheckBox :value="feature_value" name="group3" v-model="selectValue" :multiple="true"> </CheckBox>
                                            <Label style="padding-right: 0px;">{{localFeature.name}}</Label>
                                            </div>
                                            <Label style="padding-right: 0px; margin-top: -17px; margin-left: 28px; position: absolute;">({{localFeature.singleFunc}})</Label>
                                        </div>
                                        <div style="min-height: 210px;">
                                            <superposed-graph-version-three v-bind="{'strength':localFeature.data}" :index='feature_value' :heightPer='0.2' :featureName='localFeature.name'></superposed-graph-version-three>
                                        </div>
                                    </div>
                                    <div style="border: 1px solid rgba(0,0,0,0.05); padding-bottom: 0px;">
                                        <div style="float: left; z-index: 999; width: 95px; text-align: left; padding-left: 5px;">
                                            <div>
                                            <CheckBox :value="(Number(feature_value)+5).toString()" name="group4" v-model="selectValue" :multiple="true"></CheckBox>
                                            <Label>multi-{{localFeature.name}}</Label>
                                            </div>
                                            <Label style="padding-right: 0px; margin-top: -17px; margin-left: 14px; position: absolute;">({{localFeature.multiFunc}})</Label>
                                        </div>
                                        <div style="min-height: 210px;">
                                        <superposed-graph-for-multi-version-two v-bind="{'strength':localFeature.data}" :index='feature_value' :heightPer='0.2' :featureName='localFeature.name'></superposed-graph-for-multi-version-two>
                                        </div>
                                    </div>
                                </div>
                            </div>
                            </div>
                        -->
                            <!--
                            <div v-for="(feature, index) in multifeatures" >
                                <div style="width: 184px;margin: auto; float: left;text-align: center;padding-bottom: 0px;">
                                    <div style="border: 1px solid rgba(0,0,0,0.05);">
                                        <div style="float: left; z-index: 999; width: 100px; text-align: left; padding-left: 5px;">
                                            <CheckBox :value="(index + 10 + singlefeatures.length).toString()" name="group3" v-model="selectValue" :multiple="true"> </CheckBox>
                                            <Label style="width: 48px; padding-right: 0px;">m_{{feature.name}}</Label>
                                            <button class="btn btn-secondary mybtn" @click="deleteMulti(index)">
                                                -
                                            </button>
                                        </div>
                                        <div style="min-height: 230px;">
                                            <superposed-graph-for-multi-version-two v-bind="{'strength':feature.data}" :index='feature.index' :heightPer='0.2'></superposed-graph-for-multi-version-two>
                                        </div>
                                    </div>
                                </div>
                            </div>
                            -->
                        </div>
                        </happy-scroll>
                        </div>
                        </div>
                        <!--
                        <div style="height: 469px; width: 927px; float: left;">
                            <happy-scroll color="rgba(0,0,0,0.5)" size="5" >
                                <div :style="{'height': '5000px'}">
                                    <div>
                                        <div v-for="feature_value in feature_values" >
                                            <div style="width: 100%;margin: auto; float: left;text-align: center;padding-bottom: 10px;">
                                                <div>
                                                    <CheckBox :value="feature_value" name="group3" v-model="selectValue" :multiple="true"></CheckBox>
                                                    <Label style="width: 200px">{{features[feature_value].name}}</Label>
                                                </div>
                                                <superposed-graph v-bind="{'strength':features[feature_value].data}" :index='feature_value' :heightPer='0.2'></superposed-graph>
                                            </div>
                                            <div style="width: 300px;margin: auto; float: left;text-align: center;padding-bottom: 10px;height: 516px;">
                                                <div>
                                                    <Label style="width: 200px">muti-{{features[feature_value].name}}</Label>
                                                </div>
                                                <superposed-graph-for-multi v-bind="{'strength':features[feature_value].data}" :index='feature_value' :heightPer='0.2'></superposed-graph-for-multi>
                                            </div>
                                        </div>
                                    </div>
                                </div>
                            </happy-scroll>
                        </div>
                        -->
                    </div>
                </div>
            </div>
    </div style="height: 100%;">
        <div style="padding: 0px;width: 26%;float: left; height: 100%;">
            <div class="mycard" style="padding-left: 5px; padding-right: 5px; height: 100%; margin-right: 0px;">
                <div :style="{'padding-top': '5px', 'padding-left':'5px'}">
                    <p class="mytitle">High-Order Composite</p>
                </div>
                <!--div class="mypannel" style="text-align: left; height: 30px; margin-top: -10px;  margin-bottom: 1px; padding-left: 5px;">
                    <div style="height: 30px; border-right: 1px solid rgba(0,0,0,0.1);float: left;margin-right: 10px;">
                        <div v-for="pro in fuc" :key="pro.id" style="margin-bottom:0px; float: left;">
                            <RadioButton name="group2" :inputId="pro.id" :value="pro.id" v-model="fucValue"></RadioButton>
                            <Label for="pro">{{pro.name}}</Label>
                        </div>
                    </div>
                    <div style="width: 150px; float: left;">
                        <label style="float: left; margin-top: 5px; margin-right: 8px; margin-bottom: 0px; font-size: 10px; width: auto; color: #8998AC;">filter:</label>
                        <div :style="{'float':'left', 'margin-top':'5px'}">
                            <slider :min=0 :max=100 v-model = "filterRate[0]" :dataMin="minForTotalData" :dataMax="maxForTotalData" :style="{'width': '100px'}"></slider>
                        </div>
                    </div>
                </div-->
                <!--
                <div>{{totalData.allFeature}}</div>
                <div>({{totalData.allFunc}})</div>
                <div>({{totalData.allGroups }})</div>
                <div>({{totalData.collection }})</div>
            -->
             
                <div style="float: left; width: 100%; margin-top: 0px; margin-left: 8px;">
                    <superposed-graph-for-cal-two-with-ellipse-version-two v-bind="{'strength':totalData, 'filterRate':filterRate, 'collection': totalData.collection}" :heightPer='0.4' :index="String(10)"></superposed-graph-for-cal-two-with-ellipse-version-two>
                    <!--superposed-graph-for-cal-two v-bind="{'strength':totalData, 'filterRate':filterRate}" :heightPer='0.4' :index="String(10)"></superposed-graph-for-cal-two-->
                </div>
            </div>
        </div>    
    </div>
</template>

<script>
    //echarts
    import superposedGraph from "./superposedGraph"
    import superposedGraphForMulti from "./superposedGraphForMulti"
    import superposedGraphForMultiVersionTwo from "./superposedGraphForMultiVersionTwo"
    import superposedGraphForMultiVersionThree from "./superposedGraphForMultiVersionThree"
    import superposedGraphForMultiVersionFour from "./superposedGraphForMultiVersionFour"
    import superposedGraphForCal from "./superposedGraphForCal"
    import superposedGraphForCalTwo from "./superposedGraphForCalTwo"
    import strength from "../../data/strength"
    import { mapState } from 'vuex'
    import NetService from '@/services/net-service'
    import slider from './slider'
    import superposedGraphVersionTwo from "./superposedGraphVersionTwo"
    import superposedGraphVersionThree from "./superposedGraphVersionThree"
    import superposedGraphVersionFour from "./superposedGraphVersionFour"
    import superposedGraphVersionFive from "./superposedGraphVersionFive"
    import superposedGraphForDupli from "./superposedGraphForDupli"
    import superposedGraphForMultiDupli from "./superposedGraphForMultiDupli"
    import superposedGraphForCalTwoWithEllipse from "./superposedGraphForCalTwoWithEllipse"
    import superposedGraphForCalTwoWithEllipseVersionTwo from "./superposedGraphForCalTwoWithEllipseVersionTwo"
    import vuedraggable from 'vuedraggable'
    import { UiAutocomplete, UiCollapsible, UiAlert, UiSelect, UiButton, UiDatepicker, UiRadioGroup} from 'keen-ui'
    import loading from "./loading"

    export default {
        name: "comparisonViewVersionFour",
        components: {
            superposedGraph,
            superposedGraphForMulti,
            superposedGraphForMultiVersionTwo,
            superposedGraphForMultiVersionThree,
            superposedGraphForMultiVersionFour,
            superposedGraphForCal,
            superposedGraphForCalTwo,
            superposedGraphVersionTwo,
            superposedGraphVersionThree,
            superposedGraphVersionFour,
            superposedGraphVersionFive,
            superposedGraphForDupli,
            superposedGraphForMultiDupli,
            superposedGraphForCalTwoWithEllipse,
            superposedGraphForCalTwoWithEllipseVersionTwo,
            slider,
            vuedraggable,
            UiAutocomplete,
            UiCollapsible,
            UiAlert,
            UiSelect,
            UiButton,
            UiDatepicker,
            UiRadioGroup,
            loading
        },
        data () {
            return {
                strength,
                lambda: 50,
                //ROIZValue: 20,
                //DBScanRadius: 50,
                totalData: {},
                showFunctionPros: [
                    {
                        label: "Uni-&multi-coding",//单变量多变量
                        index: "0",
                    },
                    {
                        label: "Side-by-side",//两个分组
                        index: "1",
                    },
                ],
                eyesidePros: [
                    {
                        "label": "Axial",//单变量多变量
                        "index": "0",
                    },
                    {
                        "label": "Sagittal",//两个分组
                        "index": "1",
                    },
                    {
                        "label": "Coronal",//单变量多变量
                        "index": "2",
                    }
                ],
                //linksVal: "0",
                linksPros: [
                    {
                        "label": "Mean",//"平均特征值",
                        "index": "0",
                    },
                    {
                        "label": "Std.",//"特征值总方差",
                        "index": "1",
                    },
                    {
                        "label": "#Subject",
                        "index": "2",
                    },
                    {
                        "label": 'p-value',
                        "index": "3",
                    }
                ],
                //testVal: "0",
                testValPros: [
                    {
                        "label": "t-test",
                        "index": "0",
                    },
                    {
                        "label": "welch",
                        "index": "1",
                    },

                    /*
                    {
                        "message": "mann-whiteney",
                        "index": "2",
                    }
                    */
                ],
                multiGroupValPros: [
                    {
                        "label": "ROI",
                        "index": "0",
                    },
                    {
                        "label": "Lobe",
                        "index": "1",
                    },
                ],
                //multiVal: "0",
                multiValPros: [
                    {
                        "label": "Weight",
                        "index": "0",
                    },
                    {
                        "label": "Accuracy",
                        "index": "1",
                    },
                    {
                        "label": "p-value",
                        "index": "2",
                    },
                ],
                height: {
                    height: '',
                },
                values: [],
                fuc: [
                {
                    id: "0",
                    name: '+',
                },
                {
                    id: "1",
                    name: '-'
                },
                {
                    id: "2",
                    name: '*'
                },
                {
                    id: "3",
                    name: "/"
                }
                ],
                fucValue: "0",
                newName: "",
                selectValue: [],
                filterRate: [0, 100],
                duplicateData: [new Array(), new Array(), new Array(), new Array(), new Array(), new Array(), new Array(), new Array(), new Array(), new Array()],
                items: [],
                maxForTotalData: 0.0,
                minForTotalData: 0.0,
                
            }
        },
        created(){
            //this.$store.commit('set_features_data', [this.strength, this.strength,this.strength,this.strength,this.strength]);
            this.setTotalDataZero();
        },
        computed: {
            ...mapState(['pValueThreshold', 'validThreshold', 'features', 'localFeature', 'feature_values', 'multifeatures', 'selected_index', 'feature_index', 'needReDraw']),
            singlefeatures: {
                get() {
                    return this.$store.state.singlefeatures;
                },
                set(val) {
                    this.$store.commit('set_singlefeatures', val);
                    console.log('set');
                }
            },
            showFunction:{
                get() {
                    return this.showFunctionPros[Number(this.$store.state.showFunction)];
                },
                set(newVal) {
                    console.log("showFunction", newVal.index);
                    this.$store.commit('set_showFunction', newVal.index);
                    if(this.singlefeatures.length > 0){
                        this.$store.commit('set_selectedFeature_data_data', [this.$store.state.linksVal, this.selected_index]);
                        this.$store.commit('set_selectedFeature_data_multidata', [this.multiVal.index, this.selected_index]);
                    }
                    this.calSuperposedGraph();
                }
            },
            eyeside:{
                get() {
                    return this.eyesidePros[Number(this.$store.state.eyeside)];
                },
                set(newVal) {
                    this.$store.commit('set_eyeside', newVal.index);
                }
            },
            linksVal: {
                get() {
                    return this.linksPros[Number(this.$store.state.linksVal)];
                },
                set(newVal) {
                    this.$store.commit('set_linksVal', newVal.index);
                    if(this.singlefeatures.length > 0)
                        this.$store.commit('set_selectedFeature_data_data', [newVal.index, this.selected_index]);
                    this.calSuperposedGraph();
                }
            },
            testVal: {
                get() {
                    return this.testValPros[Number(this.$store.state.testVal)];
                },
                set(newVal) {
                    this.$store.commit('set_testVal', newVal.index);
                    if(this.singlefeatures.length > 0) {
                        var localindex = this.selected_index;
                        var index = this.feature_index;
                        //添加单变量
                        this.$store.commit('set_selectedFeature_singleLoading', [localindex, true]);
                        NetService.singleAnalyzeForOnefeature(index, this.pValueThreshold[Number(index)], this.validThreshold[Number(index)], this.testVal.index, (res, err) => {
                            if(err){
                                console.error(err);
                                return;
                            }
                            if(!res.data){
                                console.log('data not exists!');
                                return;
                            }
                            console.log('singleAnalyzeForOnefeature', res, localindex);
                            //{"code":0,"data":{"originPvalue":[2.0], "p-value":[1.0], "Mean":[508.4],"Var.":[508.4],"totalPeople":[504]},"message":"获取成功"}
                            this.$store.commit('set_selectedFeature_data_for_single', [res.data.data, localindex]);
                            this.$store.commit('set_selectedFeature_data_data', [this.linksVal.index, localindex]);
                            this.calSuperposedGraph();
                        });
                    }
                }
            },
            multiGroupVal: {
                get() {
                    return this.multiGroupValPros[Number(this.$store.state.multiGroupVal)];
                },
                set(newVal) {
                    this.$store.commit('set_multiGroupVal', newVal.index);
                    if(this.singlefeatures.length > 0) {
                        var localindex = this.selected_index;
                        var index = this.feature_index;
                        //添加多变量
                        this.$store.commit('set_selectedFeature_multiLoading', [localindex, true]);
                        NetService.multiAnalyzeForOnefeature(index, this.pValueThreshold[Number(index)], this.validThreshold[Number(index)], this.$store.state.multiGroupVal, this.$store.state.ROIZValue, this.$store.state.DBScanRadius, (res, err) => {
                            if(err){
                                console.error(err);
                                return;
                            }
                            if(!res.data){
                                console.log('data not exists!');
                                return;
                            }
                            console.log('multiAnalyzeForOnefeature', res.data, localindex);
                            //{"code":0,"data":{"accuracy":[0.5,..],"AccSelectConnection":[1,...], "weight":[508.4,...], "WeightSelectConnection": [1,..]},"message":"获取成功"}
                            this.$store.commit('set_selectedFeature_data_for_multi', [res.data.data, localindex]);
                            this.$store.commit('set_selectedFeature_data_multidata', [this.multiVal.index, localindex]);
                            this.calSuperposedGraph();
                        });
                    }
                }
            },
            multiVal: {
                get() {
                    return this.multiValPros[Number(this.$store.state.multiVal)];
                },
                set(newVal) {
                    if(this.singlefeatures.length > 0)
                        this.$store.commit('set_selectedFeature_data_multidata', [newVal.index, this.selected_index]);
                    this.$store.commit('set_multiVal', newVal.index);
                    this.calSuperposedGraph();
                }
            },
            ROIZValue: {
                get() {
                    return this.$store.state.ROIZValue;
                },
                set(newVal) {
                    console.log("ROIZValue change: " + newVal);
                    this.$store.commit('set_ROIZValue', newVal);
                    if(this.singlefeatures.length > 0){
                    var localindex = this.selected_index;
                    var index = this.feature_index;
                    this.$store.commit('set_selectedFeature_multiLoading', [localindex, true]);
                    NetService.multiAnalyzeForOnefeature(index, this.pValueThreshold[Number(index)], this.validThreshold[Number(index)], this.$store.state.multiGroupVal, this.$store.state.ROIZValue, this.$store.state.DBScanRadius, (res, err) => {
                        if(err){
                            console.error(err);
                            return;
                        }
                        if(!res.data){
                            console.log('data not exists!');
                            return;
                        }
                        console.log('multiAnalyzeForOnefeature', res.data, localindex);
                        //{"code":0,"data":{"accuracy":[0.5,..],"AccSelectConnection":[1,...], "weight":[508.4,...], "WeightSelectConnection": [1,..]},"message":"获取成功"}
                        this.$store.commit('set_selectedFeature_data_for_multi', [res.data.data, localindex]);
                        this.$store.commit('set_selectedFeature_data_multidata', [this.multiVal.index, localindex]);
                        this.calSuperposedGraph();
                    });            
                    }
                }
            },
            DBScanRadius: {
                get() {
                    return this.$store.state.DBScanRadius;
                },
                set(newVal) {
                    console.log("DBScanRadius change: " + newVal);
                    this.$store.commit('set_DBScanRadius', newVal);
                    if(this.singlefeatures.length > 0) {
                        var localindex = this.selected_index;
                        var index = this.feature_index;
                        //获取多变量
                        this.$store.commit('set_selectedFeature_multiLoading', [localindex, true]);
                        NetService.multiAnalyzeForOnefeature(index, this.pValueThreshold[Number(index)], this.validThreshold[Number(index)], this.$store.state.multiGroupVal, this.$store.state.ROIZValue, this.$store.state.DBScanRadius, (res, err) => {
                            if(err){
                                console.error(err);
                                return;
                            }
                            if(!res.data){
                                console.log('data not exists!');
                                return;
                            }
                            console.log('multiAnalyzeForOnefeature', res.data, localindex);
                            //{"code":0,"data":{"accuracy":[0.5,..],"AccSelectConnection":[1,...], "weight":[508.4,...], "WeightSelectConnection": [1,..]},"message":"获取成功"}
                            this.$store.commit('set_selectedFeature_data_for_multi', [res.data.data, localindex]);
                            this.$store.commit('set_selectedFeature_data_multidata', [this.multiVal.index, localindex]);
                            this.calSuperposedGraph();
                        });
                    }
                }
            }

        },
        //mounted在html渲染出来后进行操作
        mounted() {
            
        },
        methods: {
            reload() {
                this.$forceUpdate();
            },
            //直接相加减
            addData(data1, data2) {
                var result = [];
                for(var i in data1) {
                    var tmp = data1[i] + data2[i];
                    result.push(tmp);
                }
                return result;
            },
            subData(data1, data2) {
                var result = [];
                for(var i in data1) {
                    var tmp = data1[i] - data2[i];
                    result.push(tmp);
                }
                return result;
            },
            mulData(data1, data2) {
                var result = [];
                for(var i in data1) {
                    var tmp = data1[i] * data2[i];
                    result.push(tmp);
                }
                return result;
            },
            devData(data1, data2) {
                var result = [];
                for(var i in data1) {
                    var tmp = 0;
                    if(data2[i] != 0) tmp = data1[i] / data2[i];
                    result.push(tmp);
                }
                return result;
            },
            minData(data1, data2) {
                var result = [];
                for(var i in data1) {
                    var tmp;
                    if(Math.abs(data1[i]) <=  Math.abs(data2[i])) {
                        tmp = data1[i];
                    }
                    else {
                        tmp = data2[i];
                    }
                    //var tmp = Math.min(data1[i], data2[i]);
                    result.push(tmp);
                }
                return result;
            },
            //归一化后相加减
            normalAddData(data1, data2) {
                var max1 = Math.max(...data1);
                var min1 = Math.min(...data1);
                var max2 = Math.max(...data2);
                var min2 = Math.min(...data2)
                var result = [];
                for(var i in data1) {
                    var tmp1 = (data1[i] - min1) / (max1 - min1);
                    var tmp2 = (data2[i] - min2) / (max2 - min2);
                    var tmp = tmp1 + tmp2;
                    result.push(tmp);
                }
                return result;
            },
            normalSubData(data1, data2) {
                var max1 = Math.max(...data1);
                var min1 = Math.min(...data1);
                var max2 = Math.max(...data2);
                var min2 = Math.min(...data2)
                var result = [];
                for(var i in data1) {
                    var tmp1 = (data1[i] - min1) / (max1 - min1);
                    var tmp2 = (data2[i] - min2) / (max2 - min2);
                    var tmp = tmp1 - tmp2;
                    result.push(tmp);
                }
                return result;
            },
            normalMulData(data1, data2) {
                var max1 = Math.max(...data1);
                var min1 = Math.min(...data1);
                var max2 = Math.max(...data2);
                var min2 = Math.min(...data2)
                var result = [];
                for(var i in data1) {
                    var tmp1 = (data1[i] - min1) / (max1 - min1);
                    var tmp2 = (data2[i] - min2) / (max2 - min2);
                    var tmp = tmp1 * tmp2;
                    result.push(tmp);
                }
                return result;
            },
            normalDevData(data1, data2) {
                var max1 = Math.max(...data1);
                var min1 = Math.min(...data1);
                var max2 = Math.max(...data2);
                var min2 = Math.min(...data2)
                var result = [];
                for(var i in data1) {
                    var tmp1 = (data1[i] - min1) / (max1 - min1);
                    var tmp2 = (data2[i] - min2) / (max2 - min2);
                    if (tmp2 == 0) {
                        result.push(0);
                    }
                    else{
                        var tmp = tmp1 / tmp2;
                        result.push(tmp);
                    }
                }
                return result;
            },
            normalize(data) {
                var result = [];
                var max = Math.max(...data);
                //var min = Math.min(...data);
                for (var i in data) {
                    //result.push((data[i] - min) / (max - min));
                    result.push((data[i]) / (max));
                }
                return result;
            },
            calData(data1, data2, index) {
                //都是单变量
                if(data1.single && data2.single) {
                    if(data1.Func == "p-value" && data2.Func == "p-value") {
                        return this.minData(data1.data, data2.data);
                    }
                    if (index == 0) {
                        return this.addData(data1.data, data2.data);
                    } else if (index == 1) {
                        return this.subData(data1.data, data2.data);
                    } else if (index == 2) {
                        return this.mulData(data1.data, data2.data);
                    } else if (index == 3) {
                        return this.devData(data1.data, data2.data);
                    }
                }
                if (index == 0) {
                    return this.addData(data1.data, data2.data);
                } else if (index == 1) {
                    return this.subData(data1.data, data2.data);
                } else if (index == 2) {
                    return this.mulData(data1.data, data2.data);
                } else if (index == 3) {
                    return this.devData(data1.data, data2.data);
                }
            },
            calMoreData(datas, index) {
                var result = this.deepCopy(datas[0]);
                result.allFunc = [result.Func];
                result.allFeature = [result.name];
                result.allGroups = [result.groupname];
                result.allDatas = [];

                var same = true;
                var tmp = datas[0].Func;
                for(var i in datas) {
                    result.allDatas.push(datas[i].originData);
                    if (tmp != datas[i].Func) {
                        same = false;
                    }
                    if(i > 0) {
                        result.data = this.minData(result.data, datas[i].data);
                        /*
                        if(result.Func == "p-value" && datas[i].Func == "p-value") {
                            result.data = this.minData(result.data, datas[i].data);
                        }
                        else{
                            if (index == 0) {
                                result.data = this.addData(result.data, datas[i].data);
                            } else if (index == 1) {
                                result.data = this.subData(result.data, datas[i].data);
                            } else if (index == 2) {
                                result.data = this.mulData(result.data, datas[i].data);
                            } else if (index == 3) {
                                result.data = this.devData(result.data, datas[i].data);
                            }
                        }
                        */
                        result.Func = datas[i].Func;
                        result.allFunc.push(result.Func);
                        result.allFeature.push(datas[i].name);
                        result.allGroups.push(datas[i].groupname);
                        //result.allFunc = result.allFunc + "&" + result.Func;
                        //result.allFeature = result.allFeature + "&" + datas[i].name; 
                    }
                }
                result.sametype = same;
                return result;
            },
            calSign(datas) {
                var len = datas.length;
                var result = [];
                for (var i in datas[0]) {
                    var same = true;
                    var start = 0;
                    for(var j in datas) {
                        //找到第一个非0的
                        if(datas[j][i] == 0) {
                            start++;
                        } else {
                            break;
                        }
                    }
                    //全0
                    if(start == datas.length) {
                        result.push(1);
                        continue;
                    }
                    //假设start < datas.length
                    for(var j = start; j < datas.length; j++) {
                        //符号为0时当作相同
                        if( (datas[j][i] != 0) && ((datas[j][i]>=0) != (datas[start][i]>=0))) {
                            same = false;
                            break;
                        }

                    }
                    if (same && datas[start][i]>=0) {
                        result.push(1);
                    }
                    else if (same) {
                        result.push(-1);
                    }
                    else {
                        result.push(0);
                    }
                    /*
                    for(var j in datas) {
                        if(j > 0) {
                            if((datas[j][i]>=0) != (datas[j-1][i]>=0)) {
                                same = false;
                                break;
                            }
                        }
                    }
                    if (same && datas[0][i]>=0) {
                        result.push(1);
                    }
                    else if (same) {
                        result.push(-1);
                    }
                    else {
                        result.push(0);
                    }
                    */
                    
                }
                return result;
            },
            setTotalDataZero() {
                var zero = [];
                for(var i = 0; i < 2485; i++) {
                    zero.push(0);
                }
                /*
                this.totalData.groupOneData = zero;
                this.totalData.groupTwoData = zero;
                this.totalData.pvalue = zero;
                this.totalData.data = zero;
                */
                this.totalData = new Object();
                //this.totalData.signOne = zero;
                //this.totalData.signTwo = zero;
                this.totalData.sign = zero;
                this.totalData.data = zero;
                this.totalData.allDatas = [];
                this.totalData.allSigns= [];
                this.totalData.collection = {};
                this.totalData.collection.allFunc = []; //收集所有统计量 
                this.totalData.collection.allFeature = []; //收集所有特征值
                this.totalData.collection.allGroups = []; //收集所有组别
                this.totalData.sametype = false;
            },
            deepCopy(parm){
                let res;
                if(Object.prototype.toString.call(parm) == "[object Array]"){   //数组
                    res = [];
                    for(var i = 0;i < parm.length;i++){
                        res[i] = this.deepCopy(parm[i]);
                    }
                }else if(Object.prototype.toString.call(parm) == "[object Object]"){  //对象
                    res = {};
                    for(var i in parm){
                        res[i] = this.deepCopy(parm[i]);
                    }
                }else{
                    return parm;
                }
                return res;
            },
            duplicate(feature_value, type_value) {
                var index = feature_value;
                if(type_value == 1) { //multi
                    index = Number(index) + 5;
                }
                //console.log('index', index);
                this.duplicateData[index].push(this.deepCopy(this.features[feature_value]));
            },
            deleteDuplicate(feature_value, type_value, dupliIndex) {
                var index = feature_value;
                if(type_value == 1) { //multi
                    index = Number(index) + 5;
                }
                var res = this.deepCopy(this.duplicateData[index].filter(function(x,i) {return i != dupliIndex;}));
                while(this.duplicateData[index].length != 0){
                    this.duplicateData[index].pop();
                }
                for(var i in res) {
                    this.duplicateData[index].push(res[i]);
                }
            },
            deleteSingle(index) {
                this.$store.commit('delete_singlefeatures', index);
                this.selectValue = [];
                //this.reload();
            },
            deleteMulti(index) {
                this.$store.commit('delete_multifeatures', index);
            },
            deleteValues(index){
                this.$store.commit('delete_feature_values', index);
            },
            changeSelected(index) {
                this.$store.commit('set_selected_index', index);
            },
            subArray(array1, array2) {
                var res = new Array();
                for(var i in array1) {
                    res[i] = array1[i] - array2[i];
                }
                //console.log('subArray', res);
                return res;
            },
            calSuperposedGraph() {
                if(this.selectValue.length == 0) {
                    this.setTotalDataZero();
                    return;
                }
                var data = [];
                for(var i in this.selectValue) {
                    data[i] = new Object();
                    /*
                    if(this.selectValue[i] < 5) { //单变量
                        if(this.localFeature.singleFunc == "Mean" || this.localFeature.singleFunc == "Var." ) {
                            data[i].data = this.normalize(this.localFeature.filterSingleData);
                        } else {
                            data[i].data = this.deepCopy(this.localFeature.filterSingleData);
                        }
                        data[i].originData = this.deepCopy(this.localFeature.data.data);
                        //data[i].sign = this.deepCopy(this.subArray(this.features[this.selectValue[i]].data.groupOneData, this.features[this.selectValue[i]].data.groupTwoData));
                        data[i].sign = this.deepCopy(this.localFeature.data.data);
                        data[i].Func = this.localFeature.singleFunc;
                        data[i].single = true;
                        data[i].name = this.localFeature.name;
                    }
                    else if (this.selectValue[i] < 10) { //多变量
                        data[i].data = this.deepCopy(this.localFeature.filterMultiData);
                        data[i].sign = this.deepCopy(this.localFeature.data.weight);
                        data[i].originData = this.deepCopy(this.localFeature.data.multidata);
                        data[i].Func = this.localFeature.multiFunc;
                        data[i].single = false;
                        data[i].name = this.localFeature.name;
                    }
                    */
                    if (this.selectValue[i] % 2 == 0) { //单变量
                        var index = Math.floor(this.selectValue[i] / 2);
                        if (this.singlefeatures[index].singleFunc == "Mean" || this.singlefeatures[index].singleFunc == "Var.")
                            data[i].data = this.deepCopy(this.singlefeatures[index].filterSingleData); //无椭圆时normalize，有椭圆时deecopy
                        else data[i].data = this.deepCopy(this.singlefeatures[index].filterSingleData);
                        data[i].originData = this.deepCopy(this.singlefeatures[index].data.data);
                        data[i].sign = this.deepCopy(this.singlefeatures[index].filterSingleData);
                        data[i].Func = this.singlefeatures[index].singleFunc;
                        data[i].single = true;
                        data[i].name = this.singlefeatures[index].name;
                        data[i].groupname = [this.singlefeatures[index].g1name, this.singlefeatures[index].g2name];
                    }
                    else { //多变量
                        var index = Math.floor(this.selectValue[i] / 2);
                        data[i].data = this.deepCopy(this.singlefeatures[index].filterMultiData);
                        var multi = [];
                        for(var mi in this.singlefeatures[index].data.multisign){
                            var mtmp = this.singlefeatures[index].data.multisign[mi] * Math.abs(this.singlefeatures[index].filterMultiData[mi]);
                            multi.push(mtmp);
                        }
                        /*
                        if(this.singlefeatures[index].showFunction == 0) {
                            data[i].sign = this.deepCopy(this.singlefeatures[index].data.multisign);
                        }
                        else {
                            data[i].sign = this.deepCopy(this.singlefeatures[index].data.multisign);
                        }
                        */
                        data[i].sign = multi;
                        data[i].originData = this.deepCopy(this.singlefeatures[index].data.multidata);
                        data[i].Func = this.singlefeatures[index].multiFunc;
                        data[i].single = false;
                        data[i].name = this.singlefeatures[index].name;
                        data[i].groupname = [this.singlefeatures[index].g1name, this.singlefeatures[index].g2name];
                    }
                }
                if(this.selectValue.length == 1) {
                    //叠加视图显示第一个视图
                    this.totalData = new Object();
                    this.totalData.data = this.deepCopy(data[0].data);
                    //this.totalData.signOne = this.deepCopy(data[0].sign);
                    //this.totalData.signTwo = this.deepCopy(data[0].sign);
                    this.totalData.sign = this.calSign([data[0].sign]);
                    this.totalData.collection = {};
                    this.totalData.collection.allFunc = [data[0].Func];
                    this.totalData.collection.allFeature = [data[0].name];
                    this.totalData.collection.allGroups = [data[0].groupname];
                    this.totalData.allDatas = [this.deepCopy(data[0].originData)];
                    this.totalData.allSigns = [data[0].sign];
                    this.totalData.sametype = true;
                }
                /*
                else if(this.selectValue.length == 2) {
                    this.totalData = new Object();
                    //this.totalData.signOne = this.deepCopy(data[0].sign);
                    //this.totalData.signTwo = this.deepCopy(data[1].sign);
                    this.totalData.sign = this.calSign([data[0].sign, data[1].sign]);
                    this.totalData.data = this.calData(data[0], data[1], this.fucValue); 
                }
                */
                else {
                    this.totalData = new Object();
                    var signs = [];
                    for(var i in data) {
                        signs.push(data[i].sign);
                    }
                    this.totalData.allSigns = signs;
                    this.totalData.sign = this.calSign(signs);
                    var result = this.calMoreData(data, this.fucValue); 
                    this.totalData.data = result.data;
                    this.totalData.collection = {};
                    this.totalData.collection.allFunc = result.allFunc;
                    this.totalData.collection.allFeature = result.allFeature;
                    this.totalData.collection.allGroups = result.allGroups;
                    this.totalData.allDatas = result.allDatas;
                    this.totalData.sametype = result.sametype;
                }
                //console.log('totalData', this.totalData);
            }
        },
        watch: {
            /*
            lambda: {
                deep: true,
                handler: function(newVal, oldVal) {
                    console.log("lambda change: " + newVal);
                    this.$store.commit('set_lambda', newVal);
                    NetService.update2D(this.pValueThreshold, this.validThreshold, this.testVal,this.lambda, this.ROIZValue, this.DBScanRadius, (res, err) => {
                        if(err){
                            console.error(err);
                            return;0
                        }
                        if(!res.data){
                            console.log('data not exists!');
                            return;
                        }
                        console.log('update2D_test', res.data);
                        this.$store.commit('set_features_data', [res.data.curvature, res.data.entropy, res.data.torsion,res.data.strength, res.data.length]);
                        this.$store.commit('set_features_data_data', this.linksVal);
                        this.$store.commit('set_features_data_multidata', this.multiVal);
                        this.calSuperposedGraph();
                    });
                }
            },*/
            /*
            ROIZValue: {
                deep: true,
                handler: function(newVal, oldVal) {
                    console.log("ROIZValue change: " + newVal);
                    this.$store.commit('set_ROIZValue', newVal);
                    if(this.feature_values.length > 0) {
                        var localindex = this.selected_index;
                        var index = this.feature_index;
                        //获取多变量
                        NetService.multiAnalyzeForOnefeature(index, this.pValueThreshold[Number(index)], this.validThreshold[Number(index)], this.ROIZValue, this.DBScanRadius, (res, err) => {
                            if(err){
                                console.error(err);
                                return;
                            }
                            if(!res.data){
                                console.log('data not exists!');
                                return;
                            }
                            console.log('multiAnalyzeForOnefeature', res.data, localindex);
                            //{"code":0,"data":{"accuracy":[0.5,..],"AccSelectConnection":[1,...], "weight":[508.4,...], "WeightSelectConnection": [1,..]},"message":"获取成功"}
                            this.$store.commit('set_selectedFeature_data_for_multi', [res.data.data, localindex]);
                            this.$store.commit('set_selectedFeature_data_multidata', [this.multiVal, localindex]);
                            this.calSuperposedGraph();
                        });
                    }
                }
            },
            */
            /*
            DBScanRadius: {
                deep: true,
                handler: function(newVal, oldVal) {
                    console.log("DBScanRadius change: " + newVal);
                    this.$store.commit('set_DBScanRadius', newVal);
                    if(this.singlefeatures.length > 0) {
                        var localindex = this.selected_index;
                        var index = this.feature_index;
                        //获取多变量
                        NetService.multiAnalyzeForOnefeature(index, this.pValueThreshold[Number(index)], this.validThreshold[Number(index)], this.ROIZValue, this.DBScanRadius, (res, err) => {
                            if(err){
                                console.error(err);
                                return;
                            }
                            if(!res.data){
                                console.log('data not exists!');
                                return;
                            }
                            console.log('multiAnalyzeForOnefeature', res.data, localindex);
                            //{"code":0,"data":{"accuracy":[0.5,..],"AccSelectConnection":[1,...], "weight":[508.4,...], "WeightSelectConnection": [1,..]},"message":"获取成功"}
                            this.$store.commit('set_selectedFeature_data_for_multi', [res.data.data, localindex]);
                            this.$store.commit('set_selectedFeature_data_multidata', [this.multiVal, localindex]);
                            this.calSuperposedGraph();
                        });
                    }
                }
            },
            */
            /*
            linksVal: {
                deep: true,
                handler: function(newVal, oldVal) {
                    this.$store.commit('set_linksVal', newVal);
                    if(this.singlefeatures.length > 0)
                        this.$store.commit('set_selectedFeature_data_data', [newVal, this.selected_index]);
                    this.calSuperposedGraph();
                }
            },
            */
            /*
            multiVal: {
                deep: true,
                handler: function(newVal, oldVal) {
                    if(this.singlefeatures.length > 0)
                        this.$store.commit('set_selectedFeature_data_multidata', [newVal, this.selected_index]);
                    this.$store.commit('set_multiVal', newVal);
                    this.calSuperposedGraph();
                }
            },
            */
            /*
            testVal: {
                deep: true,
                handler: function(newVal, oldVal) {
                    this.$store.commit('set_testVal', newVal);
                    if(this.singlefeatures.length > 0) {
                        var localindex = this.selected_index;
                        var index = this.feature_index;
                        //添加单变量
                        NetService.singleAnalyzeForOnefeature(index, this.pValueThreshold[Number(index)], this.validThreshold[Number(index)], this.testVal, (res, err) => {
                            if(err){
                                console.error(err);
                                return;
                            }
                            if(!res.data){
                                console.log('data not exists!');
                                return;
                            }
                            console.log('singleAnalyzeForOnefeature', res, localindex);
                            //{"code":0,"data":{"originPvalue":[2.0], "p-value":[1.0], "Mean":[508.4],"Var.":[508.4],"totalPeople":[504]},"message":"获取成功"}
                            this.$store.commit('set_selectedFeature_data_for_single', [res.data.data, localindex]);
                            this.$store.commit('set_selectedFeature_data_data', [this.linksVal, localindex]);
                        });
                    }
                }
            },
            */
            features:{
                deep: true,
                handler: function(newVal, oldVal) {
                    //console.log('comparsionViewFeatures', newVal);
                    this.calSuperposedGraph();
                }
            },
            localFeature: {
                deep: true,
                handler: function(newVal, oldVal) {
                    this.calSuperposedGraph();
                }
            },
            fucValue: {
                deep: true,
                handler: function(newVal, oldVal) {
                    this.calSuperposedGraph();
                }
            },
            feature_values: {
                deep: true,
                handler: function(newVal, oldVal) {
                    //删除不在feature_Values的linksVal元素
                    //console.log('features_values添加删除', newVal);
                    var tmp = [];
                    /*
                    for(var i in this.selectValue) {
                        if(newVal.indexOf(this.selectValue[i]) != -1) {
                            //console.log('添加');
                            tmp.push(this.selectValue[i]);
                        }
                    }
                    */
                    this.selectValue = tmp;
                    
                }
            },
            selectValue: {
                deep: true,
                handler: function(newVal, oldVal) {
                    console.log('selectValue', newVal);
                    this.calSuperposedGraph();
                    /*
                    if(newVal.length > 2) {
                        this.selectValue = [];
                    }
                    else if(newVal.length == 0) {
                        this.setTotalDataZero();        
                    }
                    else {
                        this.calSuperposedGraph();
                    }
                    */
                    /*
                    else if(newVal.length == 1) {
                        //叠加视图显示第一个视图
                        this.totalData = this.deepCopy(this.features[newVal[0] % 5].data);
                    }
                    else if(newVal.length == 2) {
                        this.totalData.groupOneData = this.normalize(this.features[newVal[0] % 5].data.data);
                        this.totalData.groupTwoData = this.normalize(this.features[newVal[1] % 5].data.data);
                        //根据符号计算
                        this.totalData.data = this.calData(this.features[newVal[0] % 5].data.data, this.features[newVal[1] % 5].data.data, this.fucValue);
                        this.totalData.pvalue = this.calData(this.features[newVal[0] % 5].data.pvalue, this.features[newVal[1] % 5].data.pvalue, this.fucValue);
                    }
                    */
                }
            },
            needReDraw: {
                deep: true,
                handler: function(newVal, oldVal) {
                    console.log('needReDraw', newVal);
                    if(newVal) this.calSuperposedGraph();
                }
            }
        }
    }
</script>

<style>
    
</style>