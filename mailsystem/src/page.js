import Vue from 'vue'
import 'bootstrap/dist/css/bootstrap.css'
import sendmail from './components/SendMail.vue'
import mailbox from './components/MailBox.vue'
import mail from './components/Mail.vue'
import '../static/css/page.css'
import VueRouter from 'vue-router'
import animated from 'animate.css'
import draft from './components/Draft.vue'

Vue.use(VueRouter)

var router = new VueRouter({
    routes:[
        {path:'/sendmail',component:sendmail},
        {path:'/mailbox',component:mailbox,children:[
            {path:'/:id',component:mail}
        ]},
        {path:'/draft',component:draft}
    ]
})

var vm=new Vue({
    el:'#nav',
    data:{
        flag:false
    },
    methods:{
        show:function(){
            if(this.flag){
                this.flag=false
                this.flag=true
            }else{
                this.flag=true
            }
            
        }
    },
    router:router
    // render:h => h(nav)
})