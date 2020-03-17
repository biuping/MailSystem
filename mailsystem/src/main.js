import Vue from 'vue'
import './registerServiceWorker'
import store from './store'
import login from './components/Login.vue'
import 'bootstrap/dist/css/bootstrap.css'

Vue.config.productionTip = false

new Vue({
  store,
  render: h => h(login)
}).$mount('#app')
