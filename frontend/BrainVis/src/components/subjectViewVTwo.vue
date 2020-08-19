<template>
    <div style="height: 100%;">
        <div style="height: 100%;">
            <div class="mycard" style="margin-left: 30px; margin-top: 10px; height: 100%;">
                <div style="padding-top: 5px; padding-left: 5px;">
                    <p class="mytitle">Group Selection</p>
                </div>
                <!--选择性别还是是否患病-->
                <div :style="{'margin-top': '-10px', 'z-index': '99', 'font-size':'8px', 'margin-left':'5px', 'position':'absolute', 'bottom':'12px'}">
                    <!--div style="float: left; width: 43px">
                        <div class="subjectItem" v-for="(item, index) in pro_selections_items">
                            Layer{{index+1}}
                        </div>
                    </div-->
                    <div class="subjectItem">Subject hierarchy:</div>
                    <vuedraggable class="ui-controlgroup ui-listview" v-model="pro_selections_items" :style="{'list-style-type':'none', 'margin':'0px', 'padding':'0px', 'text-align':'left', 'float': 'left'}">
                        <transition-group>
                        <div class="subjectItem" v-for="(item, index) in pro_selections_items" :key="item.message" style="">
                            <input class="mycheckbox" type="checkbox" @click="multipleSelect(item, index)" />
                            <span :class="{'orderSpan':true, 'f-full':true, 'radiobutton-checked':selectedArr >= index}" style="margin-right: 4px;">
                                <span class="f-full"></span>
                            </span>
                            <!--span class="orderSpan">{{ item.order }}</span-->
                            <span v-for="(pro, index) in item.children" style="margin-left: -4px;">
                                <!--span v-if="index!=0">/</span-->
                                <span style="text-decoration: underline;"> 
                                    {{ item.message == "Ages" ? item.children[index] : item.children[index].substr(0,1) }}
                                </span>
                                <span style="margin-left: -3px;"> 
                                    {{ item.message == "Ages" ? (index==(item.children.length-1) ? '' : '/') :(index==(item.children.length-1) ? item.children[index].substr(1, item.children[index].length-1) : (item.children[index] == "eMCI" ? '/' : item.children[index].substr(1, item.children[index].length-1) + '/'))}} 
                                </span>
                            </span>
                            <!--
                            <span v-for="(pro, index) in item.children" v-bind:style="{color: item.color[index]}">
                                <span v-if="index!=0">/</span>
                                <span style="text-decoration: underline;"> 
                                    {{ item.children[index].substr(0,1) }}
                                </span>
                                <span style="margin-left: -3px;"> 
                                    {{ item.children[index].substr(1, item.children[index].length-1) }} 
                                </span>
                            </span>
                            -->
                        </div>
                    </transition-group>
                    </vuedraggable>
                    <!--ul class="ui-controlgroup ui-listview" :style="{'list-style-type':'none', 'margin':'1px', 'padding':'0px', 'text-align':'left'}">
                        <li class="dragitem" v-for="(item, index) in pro_selections_items" :key="item.index">
                            <input type="checkbox" @click="multipleSelect(item, item.checkval)" :checked="item.checkval" :style="{'opacity': '0', 'width':'12px','height':'12px', 'position':'absolute'}" />
                            <span :style="{'display':'inline-block', 'width':'10px','height':'10px', 'display':'inline-block', 'outline':'1px solid black', 'text-align':'center', 'font-size':'5px', 'vertical-align':'top'}">{{ item.order }}</span>
                            <span>
                            <span v-bind:style="{color: item.color[0]}" style="text-decoration: underline;"> 
                                {{ item.children[0].substr(0,1) }}
                            </span>
                            <span v-bind:style="{color: item.color[0]}" style="margin-left: -3px;"> 
                                {{ item.children[0].substr(1,item.children[0].length-1) }} 
                            </span>
                            </span>
                            <span>/</span>
                            <span>
                            <span v-bind:style="{color: item.color[1]}" style="margin-left: 0px; text-decoration: underline;">
                                 {{ item.children[1].substr(0,1) }} 
                            </span>
                            <span v-bind:style="{color: item.color[1]}" style="margin-left: -3px;"> 
                                {{ item.children[1].substr(1,item.children[1].length-1) }} 
                            </span>
                            </span>
                        </li>
                    </ul-->
                    <!--button class="btn btn-secondary reloadButton" @click="reload_properties()" style="margin-top: 4px;">
                        reload
                    </button-->
                </div>
                <!--第一列上sunburst-->
                <sunburst-four :data="sunburstData" :datadepth="datadepth" :show='true' :style="{'margin-top': '-40px','float':'left', 'padding-bottom':'0px'}" >
                    <!-- Add behaviors -->
                    <template slot-scope='{ on, actions }'>
                        <highlightOnHover v-bind='{ on, actions }'/>
                        <zoomOnClick v-bind='{ on, actions }'/>
                        <markedOnClick v-bind='{ on, actions }'/>
                    </template>
                    <!-- 信息 -->
                    <nodeInfoDisplayerTwo slot='top' slot-scope='{ nodes }' :current='nodes.mouseOver' :root='nodes.root' description=' subjects' />
                    <!-- 层次 -->
                    <breadcrumbTrail slot='legend' slot-scope='{ nodes, colorGetter, width }' :current='nodes.mouseOver' :root='nodes.root' :colorGetter='colorGetter' :from='nodes.clicked' :width='width' />
                    <breadcrumbTrailThree slot='selectionone' slot-scope='{ nodes, colorGetter, width }' :colorGetter='colorGetter' :width='80' :index='0'/>
                    <breadcrumbTrailThree slot='selectiontwo' slot-scope='{ nodes, colorGetter, width }' :colorGetter='colorGetter' :width='80' :index='1'/>
                </sunburst-four>
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
    import breadcrumbTrailThree from './breadcrumbTrailThree'
    import breadcrumbTrail from './breadcrumbTrail'
    import sunburstTwo from './sunburstTwo'
    import sunburstThree from './sunburstThree'
    import sunburstFour from "./sunburstFour"
    import nodeInfoDisplayerTwo from './nodeInfoDisplayerTwo'
    import vuedraggable from 'vuedraggable'

    export default {
        name: "subjectViewVTwo",
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
            sunburstThree,
            sunburstFour,
            nodeInfoDisplayerTwo,
            breadcrumbTrailThree,
            vuedraggable
        },
        data () {
            return {
                datadepth: 1,
                sunburstData: {
                    "name": "all",
                    "size": 0,
                    "index": "0",
                    "depth": 0,
                    "peer": 1,
                    "firstName": '',
                },
                pro_selections_items: [],
                
                handleArr: [], //选中数组
                handleNum: 0, //选中的顺序

                selectedArr: -1,
            }
        },
        created() {
            //this.getPointData()
            NetService.getAllProperties((res, err) => {
                if(err){
                    console.error(err);
                    return;
                }
                if(!res.data){
                    console.log('data not exists!');
                    return;
                }
                console.log('getAllProperties', res.data.data);
                //{"code":0,"data":[{"message":"Sicken","checkval":false,"order":""},{"message":"Gender","checkval":false,"order":""},{"message":"Ages","checkval":false,"order":""},{"message":"Others","checkval":false,"order":""}],"message":"\u83b7\u53d6\u6210\u529f"}
                this.pro_selections_items = res.data.data;
                /*
                for (var i in this.pro_selections_items) {
                    this.pro_selections_items[i].color = ['#8998AC', '#8998AC'];
                }
                */
                console.log('getAllProperties', this.pro_selections_items);
                
            });
        },
        //mounted在html渲染出来后进行操作
        mounted() {
            
        },
        computed: {
            ...mapState(['linksVal', 'testVal', 'features', 'subjects_valid', 'selected_subjects', 'selected_subjects_name', 'lambda']),
        },
        methods: {
            /*
            onDragOver(status){
                console.log('onDragStart', this);
                var indicator = $('.indicator');
                indicator.css({
                    display: 'block',
                    left: $(this).offsetLeft - 10,
                    top: $(this).offsetTop + $(this. offsetHeight - 5)
                });
            },
            onDragLeave(status) {
                console.log('onDragLeave', status);
                var indicator = $('.indicator');
                indicator.hidden = true;
            },
            onDrop(source) {
                console.log('source', source);
                $(source).insetAfter(this);
                var indicator = $('.indicator');
                indicator.hidden = true;
            },
            */
            addIndex(obj, j) {
                obj.depth = j;
                if(obj.hasOwnProperty('children')) {
                    var len = obj.children.length;
                    for(var i in obj.children) {
                        obj.children[i].index = i;
                        obj.children[i].peer = len;
                        var uppername = "";
                        if(obj.children[i].name.indexOf("0s") != -1) {
                            uppername = obj.children[i].name;
                        }
                        else uppername = obj.children[i].name.substr(0,1).toUpperCase();
                        obj.children[i].firstName = obj.firstName + uppername;
                        this.addIndex(obj.children[i], j+1);
                    }
                }
            },
            reload_properties(){
                this.datadepth = 1;
                this.sunburstData = {
                    "name": "all",
                    "size": 0,
                    "index": "0",
                    "depth": 0,
                    "peer": 1,
                    "firstName": '',
                };
                
                this.handleArr = [];
                this.handleNum = 0;
                for(var a in this.pro_selections_items){
                    this.pro_selections_items[a].checkval = false;
                    this.pro_selections_items[a].order = '';
                }
                this.$store.commit('reset_selected_subjects');
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
            multipleSelect_old(val, status){
                val.checkval = !status; //反选
                if (val.checkval){
                    this.handleArr.push(val); //选中则添加进数组
                    val.order = (this.handleNum <= this.handleArr.length ? this.handleNum+=1 : this.handleNum = this.handleArr.length);
                }
                else {
                    //反选删除选中的数组
                    this.handleArr.splice(this.handleArr.indexOf(val), 1);
                    //循环绑定的数据来判断顺序值是否需要减少
                    for (let item in this.pro_selections_items) {
                        if (this.pro_selections_items[item].order > 0 && this.pro_selections_items[item].order > val.order) {
                            this.pro_selections_items[item].order--;
                            this.handleNum--;
                        }
                    }
                    //同步数组长度
                    this.handleNum = this.handleArr.length;
                    val.order = ''; //反选清空顺序值
                }
                for (let item in this.pro_selections_items) {
                    //设置颜色
                    if (this.pro_selections_items[item].order <= 0) {
                        this.pro_selections_items[item].color = ['#8998AC', '#8998AC'];
                    }
                    else if (this.pro_selections_items[item].order == 1) {
                        this.pro_selections_items[item].color = ['rgb(253, 196, 141)', 'rgb(253, 234, 215)'];
                    }
                    else if (this.pro_selections_items[item].order == 2) {
                        this.pro_selections_items[item].color = ['rgb(194, 179, 215)', 'rgb(236, 228, 243)'];
                    }
                    else if (this.pro_selections_items[item].order == 3) {
                        this.pro_selections_items[item].color = ['rgb(180, 224, 180)', 'rgb(229, 245, 229)'];
                    }
                    else {
                        this.pro_selections_items[item].color = 'rgb(207, 196, 141)';
                    }
                } 
                //更新propertiesData
                this.datadepth = this.handleArr.length + 1;
                NetService.updateSunburstData(this.handleArr, (res, err) => {
                    if(err){
                        console.error(err);
                        return;
                    }
                    if(!res.data){
                        console.log('data not exists!');
                        return;
                    }
                    console.log('updateSunburstData', res.data.data);
                    //{"code":0,"data":{"name":"all","children":[{"name":"healthy","children":[{"name":"female","size":2},{"name":"male","size":2}]},{"name":"AD","children":[{"name":"female","size":5},{"name":"male","size":7}]}]},"propertiesArr":["a","b","c"],"message":"获取成功"}
                    var sunData = res.data.data;
                    sunData.firstName = ''; //sunData.name.substr(0,1).toUpperCase();
                    this.addIndex(sunData, 0);
                    sunData.index = "0"; //在同辈中排第几
                    sunData.peer = "1"; //同辈份的总数
                    console.log('sunData ', sunData);
                    this.sunburstData = sunData; //res.data.data;
                });
                
            },
            multipleSelect(val, index){
                if (this.selectedArr > index) {
                    this.selectedArr = index;
                }
                else if(this.selectedArr == index) {
                    this.selectedArr = this.selectedArr - 1;
                }
                else {
                    this.selectedArr = index;
                }
                this.handleArr = [];
                if (this.selectedArr < 0) {
                    this.datadepth = 1;
                    this.sunburstData = {
                        "name": "all",
                        "size": 0,
                        "index": "0",
                        "depth": 0,
                        "peer": 1,
                        "firstName": '',
                    };
                    
                }
                else {
                    for(var i = 0; i <= this.selectedArr; i++) {
                        this.handleArr.push(this.pro_selections_items[i]);
                    }
                    //更新propertiesData
                    this.datadepth = this.handleArr.length + 1;
                NetService.updateSunburstData(this.handleArr, (res, err) => {
                    if(err){
                        console.error(err);
                        return;
                    }
                    if(!res.data){
                        console.log('data not exists!');
                        return;
                    }
                    console.log('updateSunburstData');
                    //{"code":0,"data":{"name":"all","children":[{"name":"healthy","children":[{"name":"female","size":2},{"name":"male","size":2}]},{"name":"AD","children":[{"name":"female","size":5},{"name":"male","size":7}]}]},"propertiesArr":["a","b","c"],"message":"获取成功"}
                    var sunData = res.data.data;
                    sunData.firstName = ''; //sunData.name.substr(0,1).toUpperCase();
                    this.addIndex(sunData, 0);
                    sunData.index = "0"; //在同辈中排第几
                    sunData.peer = "1"; //同辈份的总数
                    console.log('sunData ', sunData);
                    this.sunburstData = sunData; //res.data.data;
                    
                });
                }
            },
        },
        watch: {
            pro_selections_items: {
                deep: true,
                handler: function(newVal, oldVal) {
                    console.log('pro_selections_items', newVal);
                    //清空选择
                    this.selectedArr = -1;
                    this.handleArr = [];
                }
            }
        }
    }
</script>

<style>
.indicator{
    position:absolute;
    font-size:9px;
    width:10px;
    height:10px;
    display:none;
    color:red;
    /*left: -12px;
    top: 5px;*/
}
</style>