<template>
    <div>
        <div class="frame_header">
            <label>View Selete</label>
            <div style="
                padding-left: 50px;
                display:inline;
                ">
                <div class="custom-control custom-radio custom-control-inline">
                    <input 
                        type="radio" 
                        class="custom-control-input"
                        id = "abstraction_view" 
                        :checked="view_selection=='abstraction_view'"
                        @click="changeRadioSelection('abstraction_view')"
                        name="importance_radio_btn_group">
                    <label class="custom-control-label" for="abstraction_view">Abstraction View</label>
                </div>
                <div class="custom-control custom-radio custom-control-inline">
                    <input type="radio" class="custom-control-input" id="3d_view" 
                        :checked="view_selection=='3d_view'" 
                        @click="changeRadioSelection('3d_view')"
                        name="importance_radio_btn_group">
                    <label class="custom-control-label" for="3d_view">3D View</label>
                </div>
            </div>
        </div>
        <div class="draw-area">
            <svg></svg>
        </div>
    </div>
</template>

<script>
    import { mapState } from 'vuex';
    import $ from 'jquery';
    import C from './constants.js';
    const fix_prompt_height = 20, fix_prompt_top = 10;
    export default{
        name: "ViewSelecte",
        data(){
            return {
                view_selection: 'abstraction_view',
            };
        },
        computed: {
            ...mapState(['width','height','selected_factors']),
            data_overview(){
                return this.$store.state.data_overview;
            },
            width_or_height(){
                return [this.width, this.height];
            }
        },
        mounted(){

        },
        watch:{
            data_overview: 'draw_overview',
            width_or_height: 'draw_overview',
            selected_factors: 'draw_overview',
            deep: true,
            immediate: true
        },
        methods:{
            changeRadioSelection(radio_selection){
                this.view_selection=radio_selection;
                this.draw_overview();
            },

            draw_overview(){
               
            },
        }
    }
</script>

<style>
    svg {
        font: 10px sans-serif;
        shape-rendering: crispEdges;
    }

    .axis_overview path {
        fill: none;
        stroke: #000;
    }

    .axis_overview line {
        fill: none;
        stroke: #000;
    }

    .axis_overview path.domain {
        stroke: none;
    }

    .ui-collapsible__body{
        padding: 0%;
    }

    .ui-collapsible__header {
        font-size: 18px;
    }
</style>
