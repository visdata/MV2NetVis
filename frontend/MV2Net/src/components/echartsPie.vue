<template>
	<div>
        <chart :options='options' :auto-resize='true' :style="{width: '200px', height: '200px', margin: 'auto'}"></chart>
    </div>
</template>

<script>
    //echarts
    import 'echarts/lib/chart/pie'
    import 'echarts/lib/component/tooltip'
    import 'echarts/lib/component/legend'
    import 'echarts/lib/component/title'

    export default {
        name: "echartsPie",
        components: {
        },
        data () {
            return {
                options: {},
            }
        },
        //mounted在html渲染出来后进行操作
        mounted() {
            this.options = {
                //标题
                title: {
                    text: '饼图',
                    x: 'center',
                    textStyle: {
                        color: '#fff',
                        fontSize: 18,
                        fontWeight: 'normal'
                    }
                },
                //工具提示
                tooltip: {
                    trigger: 'item',
                    formatter: '{a}<br/>{b} : {c} ({d}%)'
                },
                //图例说明
                legend: {
                    show: true,
                    x: 'center',
                    bottom: 20,
                    data: ['a', 'b', 'c', 'd', 'e'],
                    textStyle:{
                        color: '#fff',
                        fontSize: 20
                    }
                },
                //各部分颜色
                color: ['#18DBDF','#F6EF7B','#3DE16F','#EF69FB','#FE5679'],
                //拖曳时重新渲染（默认关闭
                calculable: true,
                //工具箱
                toolbox: {
                    show: true,
                    feature: {
                        mark: {show: true},
                        dataView: {show: true, readOnly: false},
                        magicType:{
                            show: true,
                            type: ['pie', 'funnel'],
                            option: {
                                funnel: {
                                    x: '25%',
                                    width: '50%',
                                    funnelAlign: 'left',
                                    max: 1548
                                }
                            }
                        },
                        restore: {show: true},
                        saveAsImage: {show: true}
                    }
                },
                lable: {
                    show: true,
                    fontSize: 20
                },
                series: [
                    {
                        itemStyle: {
                            normal: {
                                label: {
                                    show: true,
                                    formatter: '{b} : {c} ({d}%)'
                                },
                                labelLine: {
                                    show: true
                                }
                            }
                        },
                        name:'访问来源',
                        //类型： 这厮饼图
                        type: 'pie',
                        radius: '55%',
                        center: ['50%','60%'],
                        //数据
                        data:[
                            {value:335, name:'a'},
                            {value:310, name:'b'},
                            {value:234, name:'c'},
                            {value:135, name:'d'},
                            {value:1548, name:'e'}
                        ]
                    }
                ]
            }
        }
    }
</script>
