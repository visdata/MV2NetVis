/**
 * Renderless component providing the zoom on click behavior.
 *Can be used as a default slot of sunburst component.
 */
export default {
  name: "markedOnClick",
  props: {
    /**
     *  Sunburst event listener. Same as component $on method.
     */
    on: {
      required: true,
      type: Function
    },
    /**
     *  Sunburst actions. Typically provided by sunburst default slot.
     */
    actions: {
      required: true,
      type: Object
    }
  },

  render: () => null,

  created() {
    this.on("clickNode", ({ node }) => {
      //this.actions.zoomToNode(node);
      //node.height == 0代表最后一层
      if(node.height == 0 && node.depth != 0) {
        /*
        var n = node;
        var name = n.data.name;
        while(n.parent != null){
          n = n.parent;
          name = n.data.name + '-' + name;
        }*/
        //this.$store.commit('add_selected_subjects', node);
      }
      console.log(node);
    });
  }
};
