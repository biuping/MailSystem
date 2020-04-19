import Vue from 'vue'
import VueRouter from 'vue-router'
import mainPage from '../components/Main.vue'
import login from '../components/Login.vue'
import mailbox from '../components/MailBox.vue'
import sendmail from '../components/SendMail.vue'
import draft from '../components/Draft.vue'
import wel from '../components/Welcome.vue'

Vue.use(VueRouter)
var router = new VueRouter({
    // mode: 'history',
    base: './',
    routes:[
        {path:'/mainpage',component:mainPage,children:[
            {path:'/',component:wel},
            {path:'/mainpage/mailbox',component:mailbox},
            {path:'/mainpage/sendmail',component:sendmail},
            {path:'/mainpage/draft',component:draft}
        ]},
        {path:'/',component:login}
    ]
})
export default router