import Vue from 'vue'
import 'bootstrap/dist/css/bootstrap.css'
import nav from './components/Nav.vue'
import '../static/css/page.css'

new Vue({
    el:'#app',
    render:h => h(nav)
})