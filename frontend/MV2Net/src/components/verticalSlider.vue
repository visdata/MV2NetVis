<template>
    <div class="slider2" ref="slider2">
        <div class="process" :style="{height}"></div>
        <div class="thunk" ref="trunk" :style="{top}">
            <div class="block"></div>
            <div class="tips" ref="tips">
              <span>{{spanmessage}}:</span>
              <span>{{Math.floor(scale*100)}}%</span>
              <i class="fas fa-caret-down" ></i>
            </div>
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
      name: "verticalSlider",
      props:['min','max','value','message'],
      data(){
        return{
          slider:null,        //滚动条DOM元素
          tips:null,
          thunk:null,         //拖拽DOM元素
          per:this.value,     //当前值
          spanmessage: this.message,
        }
      },
      //渲染到页面的时候
      mounted () {
        this.slider = this.$refs.slider2;
        this.thunk = this.$refs.trunk;
        var _this = this;
        this.thunk.onmousedown = function (e) {
            var height = parseInt(_this.height);
            var disY = e.clientY;
            document.onmousemove = function(e){
                // value, left, width
                // 当value变化的时候，会通过计算属性修改left，width

                // 拖拽的时候获取的新width
                var newHeight = e.clientY - disY + height;
                // 拖拽的时候得到新的百分比
                var scale = newHeight / _this.slider.offsetHeight;
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
            _this.tips.style.visibility = 'hidden';
        }
      },
      computed:{
        // 设置一个百分比，提供计算slider进度宽度和trunk的left值
        // 对应公式为  当前值-最小值/最大值-最小值 = slider进度width / slider总width
        // trunk left =  slider进度width + trunk宽度/2
        scale(){
          return ((this.per - this.min) / (this.max - this.min)).toFixed(2);
        },
        height(){
          if(this.slider){
            return this.slider.offsetHeight * this.scale + 'px';
          }else{
            return 0 + 'px'
          }
        },
       top(){
          if(this.slider){
            return this.slider.offsetHeight * this.scale -  this.thunk.offsetHeight/2  + 'px';
          }else{
            return 0 + 'px'
          }
        }
      },
  }
</script>
<style>
  .box{margin:100px auto 0;width:80%}
  .clear:after{content:'';display:block;clear:both}
  .slider2{position:relative;margin:20px 0;width:10px;height:400px;background:#e4e7ed;border-radius:5px;cursor:pointer}
  .slider2 .process{position:absolute;left:0;top:0;width:10px;height:112px;border-radius:5px;background:#409eff}
  .slider2 .thunk{position:absolute;left:-5px;top:100px;width:20px;height:20px}
  .slider2 .block{width:20px;height:20px;border-radius:50%;border:2px solid #409eff;background:rgba(255,255,255,1);transition:.2s all}
  .slider2 .tips{position:absolute;left:-7px;bottom:21px;min-width:15px;text-align:center;padding:4px 8px;background:rgba(129,129,129,0.4);border-radius:5px;height:24px;color:#fff;font-size: 12px;height: 40px; visibility: hidden;}
  .slider2 .tips i{position:absolute;margin-left:-5px;left:50%;bottom:-9px;font-size:16px;color:#000}
  .slider2 .block:hover{transform:scale(1.1);opacity:.6}
</style>
