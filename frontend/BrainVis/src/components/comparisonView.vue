<template>
	<div>
        <h3>Comparison View</h3>
        <div>
            <div style="text-align: left; width: 770px; float: left;">
                <p>选择一个展示的属性</p>
                <div v-for="pro in linksPros" :key="pro.index" style="margin-bottom:10px; ">
                    <RadioButton name="group1" :inputId="pro.message" :value="pro.index" v-model="linksVal"></RadioButton>
                    <Label for="pro" style="width: 200px">{{pro.message}}</Label>
                </div>
                <!--假设linksVal = 0时不显示
                <p v-if="linksVal">You selected: {{linksVal}}</p>
                -->
            </div>
            <div :style="{'height': '150px', 'width': '330px', 'float':'left'}">
                <happy-scroll color="rgba(0,0,0,0.5)" size="5" >
                    <div :style="{'height': '2500px'}">
                    <div>
                        <div v-for="feature in showFeatures" :key="feature.index" style="margin-bottom:10px">
                            <CheckBox :value="feature.index" :multiple="true" v-model="values"></CheckBox>
                            <Label>{{feature.name}}</Label>
                        </div>
                    </div>
                    </div>
                </happy-scroll>
            </div>
            <div :style="{'width': '1100px', 'float':'left'}">
                <div style="width: 300px;margin: auto; float: left;text-align: center;">
                    <superposed-graph v-bind="{'strength':leftData}" :heightPer='0.4'></superposed-graph>
                </div>
                <div style="width: 60px;margin: auto; float: left;text-align: center;padding-top: 20%;">
                    <ComboBox valueField="id" textField="name"
                        :virtualScroll="true"
                        :data="fuc" 
                        v-model="fucValue"
                        style="width: 60px"></ComboBox>
                </div>
                <div style="width: 300px;margin: auto; float: left;text-align: center;">
                    <superposed-graph v-bind="{'strength':rightData}" :heightPer='0.4'></superposed-graph>
                </div>
                <div style="width: 20px;margin: auto; float: left;text-align: center;padding-top: 20%;">
                    <p>=</p>
                </div>
                <div style="width: 300px;margin: auto; float: left;text-align: center;">
                    <superposed-graph v-bind="{'strength':calData}" :heightPer='0.4'></superposed-graph>
                </div>
                <div style="width: 120px;margin: auto; float: left;text-align: center;padding-top: 20%;">
                    <Label style="width: 130px;">Save result as:</Label>
                    <TextBox v-model="newName" placeholder="write a name" style="width: 140px;">
                        <Addon>
                            <LinkButton @click="addShowFeatures()" iconCls="icon-add" :btnStyle="{borderRadius:0,borderWidth:'0 0 0 1px',width:'50px',height:'30px'}">Add</LinkButton>
                        </Addon>
                    </TextBox>
                </div>
            </div>
        </div>
        <!--
        <div :style="{'height': '800px', 'width': '330px', 'float':'left'}">
            <happy-scroll color="rgba(0,0,0,0.5)" size="5" >
                <div :style="{'height': '2500px'}">
                <div>
                    <div v-for="feature_value in feature_values" style="width: 300px;margin: auto; float: left;text-align: center;padding-bottom: 10px;">
                        <div>{{features[feature_value].name}}</div>
                        <superposed-graph v-bind="{strength}" :index='feature_value' :heightPer='0.4'></superposed-graph>
                    </div>
                </div>
            </div>
            </happy-scroll>
        </div>
        -->
    </div>
</template>

<script>
    //echarts
    import superposedGraph from "./superposedGraph"
    import strength from "../../data/strength"
    import { mapState } from 'vuex'

    export default {
        name: "comparisonView",
        components: {
            superposedGraph,
        },
        data () {
            return {
                strength,
                leftData: {},
                rightData: {},
                calData: {},
                linksVal: "0",
                linksPros: [
                    {
                        "message": "平均特征值",
                        "index": "0",
                    },
                    {
                        "message": "特征值总方差",
                        "index": "1",
                    },
                    {
                        "message": "比较的总人数",
                        "index": "2",
                    }
                ],
                height: {
                    height: '',
                },
                values: [],
                showFeatures: [
                {
                    index: "0",
                    name: 'curvature',
                    data: {}
                },
                {
                    index:"1",
                    name: 'entropy',
                    data: {}
                },
                {
                    index:"2",
                    name: 'torsion',
                    data: {}
                },
                {
                    index:"3",
                    name: 'strength',
                    data: {}
                },
                {
                    index:"4",
                    name: 'length',
                    data: {}
                }],
                fuc: [
                {
                    id: 0,
                    name: '+',
                },
                {
                    id: 1,
                    name: '-'
                }
                ],
                fucValue: 0,
                newName: "",
            }
        },
        created(){
            this.leftData = this.strength;
            this.rightData = this.strength;
            this.calData = this.strength;
            for (var i in this.showFeatures) {
                this.showFeatures[i].data = this.strength;
            }
        },
        computed: {
            ...mapState(['features', 'feature_values']),
        },
        //mounted在html渲染出来后进行操作
        mounted() {
            
        },
        methods: {
            addData(data1, data2) {
                var result = {};
                return result;
            },
            subData(data1, data2) {
                var result = {};
                return result;
            },
            addShowFeatures(){
                console.log(this.newName);
                var obj = {};
                obj.index = String(this.showFeatures.length);
                obj.name = this.newName;
                obj.data = {};
                this.showFeatures.push(obj);
                this.newName = "";
                console.log(this.showFeatures);
            }
        },
        watch: {
            feature_values:{
                deep: true,
                handler: function(newVal, oldVal) {
                    if(newVal)
                        this.height.height = 500 * Math.ceil(newVal.length / 2) + 'px';
                    else this.height = '';
                }
            },
            values: {
                deep: true,
                handler: function(newVal, oldVal) {
                    if(newVal.length > 2) {
                        this.values = [];
                    }
                    else if(newVal.length == 0) {
                        this.leftData = {};
                        this.rightData = {};
                        this.calData = {};
                    }
                    else if(newVal.length == 1) {
                        this.leftData = this.showFeatures[newVal[0]].data;
                        this.rightData = {};
                        this.calData = {};
                    }
                    else {
                        this.leftData = this.showFeatures[newVal[0]].data;
                        this.rightData = this.showFeatures[newVal[1]].data;
                        if(this.fucValue == 0){
                            this.calData = this.addData(this.leftData, this.rightData);
                        }
                        else {
                            this.calData = this.subData(this.leftData, this.rightData);
                        }
                    }
                }
            },
            fucValue: {
                deep: true,
                handler: function(newVal, oldVal) {
                    if(newVal == 0){
                        this.calData = this.addData(this.leftData, this.rightData);
                    }
                    else {
                        this.calData = this.subData(this.leftData, this.rightData);
                    }
                }
            }
        }
    }
</script>
