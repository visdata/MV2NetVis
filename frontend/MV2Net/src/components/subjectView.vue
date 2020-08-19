<template>
    <div>
        <div class="card border-light mb-3">
            <div class="card-header">Subject View</div>
            <div class="card-body">
                <!--选择性别还是是否患病-->
                <div :style="{'margin-top': '-10px', 'float':'left', 'z-index': '99', 'width':'25%'}">
                    <ul class="ui-controlgroup ui-listview" :style="{'list-style-type':'none', 'margin':'1px', 'padding':'0px', 'text-align':'left'}">
                        <li v-for="(item, index) in pro_selections_items" :key="item.index">
                            <input type="checkbox" @click="multipleSelect(item, item.checkval)" :checked="item.checkval" :style="{'opacity': '0', 'width':'17px','height':'19px', 'position':'absolute','left':'20px'}" />
                            <span :style="{'display':'inline-block', 'width':'13px','height':'13px', 'display':'inline-block', 'outline':'1px solid black', 'text-align':'center', 'font-size':'12px', 'font-weight':'bolder', 'font':'small-caption','vertical-align':'top'}">{{ item.order }}</span>
                            <span> {{ item.message }} </span>
                        </li>
                    </ul>
                    <button class="btn btn-secondary" @click="reload_properties()" :style="{'width': '70px'}">
                        reload
                    </button>
                </div>
                <!--第一列上sunburst-->
                <sunburst-two :data="sunburstData" :show='true' :style="{'margin-top': '-10px','height': '220px', 'width':'75%','float':'left'}" >
                    <!-- Add behaviors -->
                    <template slot-scope='{ on, actions }'>
                        <highlightOnHover v-bind='{ on, actions }'/>
                        <zoomOnClick v-bind='{ on, actions }'/>
                        <markedOnClick v-bind='{ on, actions }'/>
                    </template>
                    <!-- 信息 -->
                    <nodeInfoDisplayer slot='top' slot-scope='{ nodes }' :current='nodes.mouseOver' :root='nodes.root' description='of visits begin' />
                    <!-- 层次 -->
                    <breadcrumbTrail slot='legend' slot-scope='{ nodes, colorGetter, width }' :current='nodes.mouseOver' :root='nodes.root' :colorGetter='colorGetter' :from='nodes.clicked' :width='width' />
                    <breadcrumbTrailTwo slot='selectionone' slot-scope='{ nodes, colorGetter, width }' :colorGetter='colorGetter' :width='width' :index='0'/>
                    <breadcrumbTrailTwo slot='selectiontwo' slot-scope='{ nodes, colorGetter, width }' :colorGetter='colorGetter' :width='width' :index='1'/>
                </sunburst-two>
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

    export default {
        name: "subjectView",
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
            sunburstTwo
        },
        data () {
            return {
                sunburstData: {
                    "name": "all",
                    "size": 87,
                    "index": "0",
                    "depth": 0,
                    "peer": 1,
                },
                pro_selections_items: [],
                
                handleArr: [], //选中数组
                handleNum: 0, //选中的顺序
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
                console.log('getAllProperties');
                //{"code":0,"data":[{"message":"Sicken","checkval":false,"order":""},{"message":"Gender","checkval":false,"order":""},{"message":"Ages","checkval":false,"order":""},{"message":"Others","checkval":false,"order":""}],"message":"\u83b7\u53d6\u6210\u529f"}
                this.pro_selections_items = res.data.data;


            });
        },
        //mounted在html渲染出来后进行操作
        mounted() {
            
        },
        computed: {
            ...mapState(['linksVal', 'testVal', 'features', 'subjects_valid', 'selected_subjects', 'selected_subjects_name', 'lambda']),
        },
        methods: {
            addIndex(obj, j) {
                obj.depth = j;
                if(obj.hasOwnProperty('children')) {
                    var len = obj.children.length;
                    for(var i in obj.children) {
                        obj.children[i].index = i;
                        obj.children[i].peer = len;
                        this.addIndex(obj.children[i], j+1);
                    }
                }
            },
            reload_properties(){
                this.sunburstData = {
                    "name": "all",
                    "size": 87,
                    "index": "0",
                    "depth": 0,
                    "peer": 1,
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
            multipleSelect(val, status){
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
                //更新propertiesData
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
                    this.addIndex(sunData, 0);
                    sunData.index = "0"; //在同辈中排第几
                    sunData.peer = "1"; //同辈份的总数
                    console.log('sunData ', sunData);
                    this.sunburstData = sunData; //res.data.data;
                });
            },
        },
        watch: {
            
        }
    }
</script>
