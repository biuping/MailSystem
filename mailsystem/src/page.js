import Vue from 'vue'
import 'bootstrap/dist/css/bootstrap.css'
import sendmail from './components/SendMail.vue'
import '../static/css/page.css'
import VueRouter from 'vue-router'

Vue.use(VueRouter)

var router = new VueRouter({
    routes:[
        {path:'/sendmail',component:sendmail}
    ]
})

new Vue({
    el:'#nav',
    data:{
        flag:false
    },
    methods:{
        show:function(){
            this.flag=!this.flag
            this.$refs.main_frame.changeWidth(this.flag)
        }
    },
    router:router
    // render:h => h(nav)
})