import { scaleOrdinal,scaleSequential } from "vue-d3-sunburst/node_modules/d3-scale";
import {
  schemeAccent,
  schemeCategory10,
  schemePaired,
  schemePastel1,
  schemePastel2,
  schemeSet1,
  schemeSet2,
  schemeSet3,
  schemeBlues,
} from "d3-scale-chromatic";
import * as d3 from "d3";

const rawColorSchemes = {
  schemeAccent,
  schemeCategory10,
  schemePaired,
  schemePastel1,
  schemePastel2,
  schemeSet1,
  schemeSet2,
  schemeSet3,
  schemeBlues,
  //blue: d3.schemeBlues[9],
};

function toDisplayName(value) {
  return value
    .replace(/(^.+)([A-Z])/g, (_, m1, m2) => m1 + " " + m2)
    .replace(/(^.+)([1-9])(?:[0-9]*)/g, (_, m1, m2) => m1 + " " + m2)
    .replace(/^\w/, c => c.toUpperCase());
}


function getcolor(c) {
  //console.log('getcolorc', c); //这边是array,颜色数组
  function scale(s){
    //console.log('getcolor ', s); //d是从上层传下来的比如depth
    var depth = s[0];
    var index = s[1];
    var peer = s[2];
    //origin
    /*
    var computeDepthOne = d3.interpolate(d3.hsl(29.244, 0.967, 0.759), d3.hsl(29.244, 0.967, 0.8));
    var computeDepthTwo = d3.interpolate(d3.hsl(265.263, 0.306, 0.757), d3.hsl(265.263, 0.306, 0.8));
    var computeDepthThree = d3.interpolate(d3.hsl(120, 0.407, 0.64), d3.hsl(120, 0.407, 0.7));
    var computeDepthFour = d3.interpolate(d3.hsl(50, 0.407, 0.64), d3.hsl(50, 0.407, 0.7));
    */
    //console.log('blue', d3.rgb(36, 83, 163).hsl(), d3.rgb(72, 126, 204).hsl());
    //console.log('green', d3.rgb(1, 178, 184).hsl(), d3.rgb(78, 212, 215).hsl());
    //console.log('yellow', d3.rgb(254, 202, 1).hsl(), d3.rgb(255, 223, 123).hsl());
    
    var computeDepthOne = d3.interpolate(d3.hsl(45.668, 0.992, 0.5), d3.hsl(45.668, 0.992, 0.55));
    var computeDepthTwo = d3.interpolate(d3.hsl(181.967, 0.989, 0.33), d3.hsl(181.967, 0.989, 0.345));
    var computeDepthThree = d3.interpolate(d3.hsl(217.795, 0.638, 0.200), d3.hsl(217.795, 0.638, 0.250));
    var computeDepthFour = d3.interpolate(d3.hsl(50, 0.407, 0.64), d3.hsl(50, 0.407, 0.7));

    if(depth == 0) {
      //var compute=d3.interpolate(d3.hsl(29.244, 0.967, 0.759), d3.hsl(29.244, 0.967, 1));
      //console.log('compute', compute(0.3));
      //console.log('rbg2hsl', d3.rgb(255, 0, 0).hsl()); //0,1,0.5
      //return compute(0.7);
      //return d3.rgb(200, 200, 200).toString();
      return d3.rgb(255, 255, 255).toString();
      //return d3.rgb(240, 240, 240).toString();
    }
    else if(depth == 1) {
      //console.log('rbg2hsl', d3.rgb(253, 192, 134).hsl());
      //return d3.rgb(253, 192, 134).toString(); // 29.244 0.967 0.759
      return computeDepthOne((index + 1) / peer * 0.7);
    }
    else if(depth == 2) {
      //console.log('rbg2hsl', d3.rgb(190, 174, 212).hsl()); 
      //return d3.rgb(190, 174, 212).toString(); //265.263 0.306 0.757
      return computeDepthTwo((index + 1) / peer * 0.7);
    }
    else if(depth == 3) {
      //console.log('rbg2hsl', d3.rgb(127, 201, 127).hsl());
      //return d3.rgb(127, 201, 127).toString(); // 120 0.407 0.64
      return computeDepthThree((index + 1) / peer * 0.7);
    }
    else if(depth == 4) {
      return computeDepthFour((index + 1) / peer * 0.7);
    }
    return d3.rgb(0, 255, 0).toString();
  }
  return scale;
}

const colorSchemes = {};
Object.keys(rawColorSchemes).forEach(key => { //key eg:schemeAccent
  colorSchemes[key] = {
    name: toDisplayName(key),
    //scale: scaleSequential(rawColorSchemes[key]) //scale函数返回颜色
    scale: getcolor(rawColorSchemes[key])
  };
  //console.log('key ', key);
  //console.log('rawColorSchemes ', rawColorSchemes[key]); //颜色数组
  //console.log('scaleOrdinal ', scaleOrdinal(rawColorSchemes[key])); //根据返回的数据对颜色数组进行选择进行选择
  //console.log('red', d3.rgb(255,0,0));
  //console.log("colorSchemes ", scaleOrdinal(rawColorSchemes[key])('hhh'));
  //console.log('blue ', schemeBlues[9]);
});

export { colorSchemes };
