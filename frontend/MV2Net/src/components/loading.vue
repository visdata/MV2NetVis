<template>
  <div class="markbox" v-show="loading">
    <div class="sun-loading"></div>
  </div>
</template>


<script>
export default {
    name: 'loading',
    props: {
        loading: {
            required: false,
            type: Boolean,
            default: false,
        },
    },
    data() {
        return {
            //loading: false
        }
    },
    created() {
        var that = this;
        /*
        this.bus.$on('loading', function(data) {
            that.loading = !!data;
        })
        */
    },
    watch: {
        loading: {
                deep: true,
                handler: function(newVal, oldVal) {
                    console.log("load change", newVal);
                    //this.loading = newVal;
                }
            },
    }
}
</script>

<style>
.sun-loading{ 
    width: 100%;
    height: 100%;    
    position: absolute; 
    top: 0;    
    left: 0;   
    line-height:56px;   
    color:#fff; 
    padding-left:60px;  
    font-size:15px; 
    background: white url(../assets/loader.gif) no-repeat 50% 50%; 
    opacity: 0.7;   z-index:9999;   
    -moz-border-radius:20px;    
    -webkit-border-radius:20px; 
    border-radius:20px; 
    filter:progid:DXImageTransform.Microsoft.Alpha(opacity=70);
}
</style>