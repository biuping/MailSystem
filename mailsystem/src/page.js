import Vue from 'vue'
import 'bootstrap/dist/css/bootstrap.css'
import sendmail from './components/SendMail.vue'
import mailbox from './components/MailBox.vue'
import '../static/css/page.css'
import VueRouter from 'vue-router'

Vue.use(VueRouter)

var router = new VueRouter({
    routes:[
        {path:'/sendmail',component:sendmail},
        {path:'/mailbox',component:mailbox}
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
            if(this.$refs.main_frame!=undefined)
                this.$refs.main_frame.changeWidth(this.flag)
        }
    },
    router:router
    // render:h => h(nav)
})