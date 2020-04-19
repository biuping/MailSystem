<template>
    <div id="nav">
        <div class="open_frame">
            <router-link to="/mainpage/mailbox" class="myrouter" title="收件箱"><span class="glyphicon glyphicon-envelope icon_span fa-4x" aria-hidden="true" @click='rebackMail'></span><span class="icon_span"></span></router-link>
            <router-link to="/mainpage/sendmail" class="myrouter" title="发送邮件"><span class="glyphicon glyphicon-pencil icon_span fa-4x" aria-hidden="true"></span><span class="icon_span"></span></router-link>
            <router-link to="/mainpage/draft" class="myrouter" title="草稿"><span class="glyphicon glyphicon-inbox icon_span" aria-hidden="true"></span><span class="icon_span"></span></router-link>
        </div>

        <router-view class="router_view " ref="main_frame"></router-view>
    </div>
</template>
<style scoped>
    #nav{
        position: relative;
        width: 100%;
        height: 100%;
        display: flex;
        flex-direction: row;
    }
    .open_frame{
        position: relative;
        top: 0%;
        left: 0%;
        height: 100%;
        width: 3%;
        background-color: rgba(89, 104, 110, 0.5);
        display: flex;
        flex-direction: column;
    }
    .myrouter{
        margin-top: 20px;
        margin-bottom: 50%;
        width: 100%;
        border-color: transparent;
        box-shadow:none;
        text-align: center;
    }
    .myrouter span{
        margin-left: 5px;
        font-size: 35px;
        color: rgb(83, 139, 243);
    }
    .myrouter span:hover{
        color: rgb(223, 152, 20);
    }

    .nav_li{
        position: relative;
        top: 0%;
        margin-left: 0%;
        margin-right: 0%;
        /* float: left; */
        height: 100%;
        width: 10%;
        background-color: rgb(158, 155, 126);
    }
    .nav_li ul{
        position: relative;
        left: 0%;
        padding: 0%;
        width: 100%;
        margin: 2px;
    }
    .nav_li li{
        position: relative;
        list-style: none;
        left: 10%;
        margin: 10px;
        margin-top: 10%;
        width: 100%;
        height: 20%;
        font-size: 20px;
        vertical-align: middle;
    }
    a{
        vertical-align: middle;
    }
    .icon_span{
        vertical-align: middle;
        margin-right: 5px;
    }

    .v-leave-to {
        opacity: 0;
        transform: translateX(-150px);
        position: absolute;
    }
    .v-leave-active{
        transition: all 0.8s ease;
    }
</style>
<script>

export default {
    data(){
        return{

        }
    },
    methods:{
        rebackMail:function(){
            console.log(1)
            console.log(this.$refs.main_frame.backMailBox)

            if(this.$refs.main_frame.backMailBox!=undefined){
                console.log("backmail")
                this.$refs.main_frame.backMailBox()
            }
            
        }
    },
    // router:mainRouter
}
</script>