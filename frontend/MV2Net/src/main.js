//核心文件，说明引用文件地址
// The Vue build version to load with the `import` command
// (runtime-only or standalone) has been set in webpack.base.conf with an alias.
import Vue from "vue";
import App from "./App";
import router from "./router";

import Vuex from 'vuex';
import store from './services/pipe-service';

import ECharts from "vue-echarts/components/ECharts";
import echarts from 'echarts';
import {HappyScroll} from 'vue-happy-scroll';
import ElementUI from 'element-ui';
import 'element-ui/lib/theme-chalk/index.css';
import 'vue-happy-scroll/docs/happy-scroll.css';
import 'vx-easyui/dist/themes/default/easyui.css';
import 'vx-easyui/dist/themes/icon.css';
import 'vx-easyui/dist/themes/vue.css';
import './mycss.css';
import EasyUI from 'vx-easyui';

Vue.use(EasyUI);
Vue.use(Vuex);
Vue.use(ElementUI);
Vue.component('chart', ECharts);
Vue.component('happy-scroll', HappyScroll);

Vue.config.productionTip = false;
Vue.prototype.$echarts = echarts;
Vue.prototype.bus = new Vue;
/* eslint-disable no-new */
new Vue({
  el: "#app",
  router,

  store,
  
  components: { App },
  template: "<App/>"
});
