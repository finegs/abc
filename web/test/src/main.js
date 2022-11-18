 import { createApp } from 'vue'
//import Vue from 'vue'
import App from './App.vue'
// Import Bootstrap and BootstrapVue CSS files (order is important)
//import { BootstrapVue, IconsPlugin } from 'bootstrap-vue'

//import './app.scss'
import VGrid from "@revolist/vue3-datagrid"
import MyGrid from './MyGrid.vue'

createApp(App).mount('#app')

//Vue.config.productionTip = false

// Make BootstrapVue available throughout your project
//Vue.use(BootstrapVue)
// Optionally install the BootstrapVue icon components plugin
//Vue.use(IconsPlugin)

// new Vue({
//     render: h => h(App),
// }).$mount('#app')

createApp(MyGrid).mount('#mygrid')
