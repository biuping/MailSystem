import Vue from 'vue'
import './registerServiceWorker'
import store from './store'
import login from './components/Login.vue'
import 'bootstrap/dist/css/bootstrap.css'
import '../static/css/index.css'
import animated from 'animate.css'

Vue.config.productionTip = false

new Vue({
  store,
  // components:{login}
  render: h => h(login)
}).$mount('#app')
