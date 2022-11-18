<template>
  <v-grid
    class="list"
    theme="compact"
    resize="true"
    readonly="true"
    filter="true"
    :auto-size-column="{mode:'autoSizeOnTextOverlap'}"
    :source="rows"
    :columns="columns"
  ></v-grid>
</template>

<script>
import VGrid from "@revolist/vue3-datagrid";

function generateFakeDataRows(rowsNumber) {
  const result = [];
  const all = rowsNumber;
  for (let j = 0; j < all; j++) {
    let row = j;
    if (!result[row]) {
      result[row] = {};
    }
    result[row]["name"] = `I am row ${row}`;
    result[row]["value"] = row;
  }
  return result;
}

export default {
  name: "App",
  data() {
    return {
      rows: generateFakeDataRows(1000000),
      columns: [ 
        { name:'Name', prop: 'name', autoSize:true, sortable:true, order:'asc', filter:'string'}, 
        { name:'Value', prop: 'value', autoSize:true, sortable:true, order:'asc', filter:'number'} 
        ],
    };
  },
  components: {
    VGrid,
  },
};
</script>


<style>
html,
body {
  height: 100%;
  padding: 0;
  margin: 0;
}
.list .row {
  display: flex;
  width:fit-content;
}
.list .data-cell {
    width:fit-content;
}

.list .row ,
.data-header-cell {
  position: relative !important;
  width: auto !important;
}

.header-wrapper {
  display: flexbox;
}
</style>