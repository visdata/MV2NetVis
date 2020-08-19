<template>
    <div class="slider" ref="slider">
        <div>
        <div class="process" :style="{width}"></div>
        <div class="thunk" ref="trunk" :style="{left}">
          <div class="block"></div>
          <div class="tips" ref="tips" style="visibility: visible;">
            <!--<span>{{Math.floor(scale*100)}}%</span>
            <i class="fas fa-caret-down" ></i>-->
          </div>
        </div>
        <div style="padding-top: 5px; margin-left: -20%; width: 150%; text-align: center;">
        <!--
          <span style="font-size: 8px; color: #3B7CFF;">{{ispvalue ? (Math.abs(Number(dataMax) - Number(dataMin)) == 0 ? Math.pow(10 , -Number(dataMin)).toPrecision(3) : Math.pow(10 , -(Number(per) / 100 * (Number(dataMax) - Number(dataMin)) + Number(dataMin))).toPrecision(3)) : (Math.abs(Number(dataMax) - Number(dataMin)) == 0 ? Number(dataMin).toPrecision(3) : (Number(per) / 100 * (Number(dataMax) - Number(dataMin)) + Number(dataMin)).toPrecision(3))}}</span>
          <span style="color: #8998AC; font-size: 8px;">({{ispvalue ? Math.pow(10, -dataMin).toPrecision(3) : Number(dataMin).toPrecision(3)}}-{{ispvalue ? Math.pow(10, -dataMax).toPrecision(3) : Number(dataMax).toPrecision(3)}})</span>
        -->
          <span style="font-size: 8px; color: #3B7CFF;">{{ispvalue ? (Math.abs(Number(dataMax) - Number(dataMin)) == 0 ? getString(Math.pow(Math.E , -Number(dataMin))) : getString(Math.pow(Math.E, -(Number(per) / 100 * (Number(dataMax) - Number(dataMin)) + Number(dataMin))))) : (Math.abs(Number(dataMax) - Number(dataMin)) == 0 ? getString(Number(dataMin)) : getString(Number(per) / 100 * (Number(dataMax) - Number(dataMin)) + Number(dataMin)))}}</span>
          <span style="font-size: 8px;">({{ispvalue ? getString(Math.pow(Math.E, -dataMin)) : getString(Number(dataMin))}}~{{ispvalue ? getString(Math.pow(Math.E, -dataMax)) : getString(Number(dataMax))}})</span>
        </div>
        <!--
        <div style="padding-top: 5px;">
          <div style="width: 40%; float: left; text-align: left; font-size: 5px;">
            {{dataMin}}
          </div>
          <div style="width: 60%; float: left; text-align: right; font-size: 5px;">
            {{dataMax}}
          </div>
        </div>
      -->
    </div>
  </div>
</template>
<script>
  /*
  * min 进度条最小值
  * max 进度条最大值
  * v-model 对当前值进行双向绑定实时显示拖拽进度
  * */
  export default{
      name: "slider",
      props: {
        min: {
          required: false,
          type: Number
        },
        max: {
          required: false,
          type: Number
        },
        value: {
          required: false,
          type: Number
        },
        dataMin: {
          required: false,
        },
        dataMax: {
          required: false,
        },
        ispvalue:{
          required: false,
          type: Number,
          default: 0,
        },
        message: {
          required: false,
          type: String,
          default: "message",
        }
      },//['min','max','value', 'dataMin', 'dataMax'],
      data(){
        return{
          slider:null,        //滚动条DOM元素
          thunk:null,         //拖拽DOM元素
          tips:null,
          per:this.value,     //当前值
        }
      },
      //渲染到页面的时候
      mounted () {
        this.slider = this.$refs.slider;
        this.thunk = this.$refs.trunk;
        var _this = this;
        this.thunk.onmousedown = function (e) {
            var width = parseInt(_this.width);
            var disX = e.clientX;
            document.onmousemove = function(e){
                // value, left, width
                // 当value变化的时候，会通过计算属性修改left，width

                // 拖拽的时候获取的新width
                var newWidth = e.clientX - disX + width;
                // 拖拽的时候得到新的百分比
                var scale = newWidth / _this.slider.offsetWidth;
                scale = scale.toFixed(2);
                _this.per = Math.ceil((_this.max - _this.min) * scale + _this.min);
                _this.per = Math.max(_this.per,_this.min);
                _this.per = Math.min(_this.per,_this.max);
                //_this.$emit('input', _this.per);
            }
            document.onmouseup = function(){
                _this.$emit('input', _this.per);
                document.onmousemove = document.onmouseup = null;
            }
            return false;
        };
        this.tips = this.$refs.tips;
        this.thunk.onmouseover = function(e) {
            //console.log(_this.tips);
            _this.tips.style.visibility = 'visible';
        };
        this.thunk.onmouseout = function(e) {
            _this.tips.style.visibility = 'visible';
        }
      },
      computed:{
        // 设置一个百分比，提供计算slider进度宽度和trunk的left值
        // 对应公式为  当前值-最小值/最大值-最小值 = slider进度width / slider总width
        // trunk left =  slider进度width + trunk宽度/2
        scale(){
          return ((this.per - this.min) / (this.max - this.min)).toFixed(2);
        },
        width(){
          if(this.slider){
            return this.slider.offsetWidth * this.scale + 'px';
          }else{
            return 0 + 'px'
          }
        },
       left(){
          if(this.slider){
            return this.slider.offsetWidth * this.scale -  this.thunk.offsetWidth/2  + 'px';
          }else{
            return 0 + 'px'
          }
        },
      },
      methods: {
        getString(num) {
          if(num == 0) return num;
          if(Math.abs(Number(this.dataMax) - Number(this.dataMin)) <= 1) {
            return num.toPrecision(2); //保留两位有效数字
          }
          if(Math.abs(num) >= 10000) {
            return num.toPrecision(2); //保留两位有效数字
          }
          if(Math.abs(num) < 0.01) {
            return num.toExponential(2); //科学计数法
          }
          return num.toFixed(2);
        }
      },
      watch: {
        value:{
          deep: true,
          handler: function(newVal, oldVal) {
            //console.log('slider value change', newVal);
            this.per = newVal;
          }
        }
      }
  }
</script>
<style>
  .box{margin:100px auto 0;width:80%}
  .clear:after{content:'';display:block;clear:both}
  .slider{position:relative;margin:auto;width:400px;height:5px;background:#e4e7ed;border-radius:2px;cursor:pointer;margin-top: 5px;}
  .slider .process{position:absolute;left:0;top:0;width:112px;height:5px;border-radius:3px;background:rgb(59, 124, 244)}
  .slider .thunk{position:absolute;left:100px;top:-4px;width:20px;height:20px}
  .slider .block{width:3px;height:13px;border:2px solid rgb(103, 125, 174);background:rgb(103, 125, 174);transition:.2s all; margin-left: 6px;}
  .slider .tips{position:absolute;left:-7px;bottom:14px;min-width:15px;text-align:center;padding:4px 8px;border-radius:5px;height:24px;font-size: 5px;}
  .slider .tips i{position:absolute;margin-left:-5px;left:50%;bottom:-9px;font-size:16px;color:#000}
  .slider .block:hover{transform:scale(1.1);opacity:.6}
</style>
