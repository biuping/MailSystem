<template>
    <div class="nav_frame">
        <div class="open_frame">
            <button class="btn btn-info openBtn" @click="show">
                <span class="glyphicon glyphicon-off" aria-hidden="true"></span>
            </button>
        </div>
        
        <div class="nav_li" v-if="flag">
            <ul>
                <li><a href=""><span class="glyphicon glyphicon-envelope icon_span" aria-hidden="true"></span><span class="icon_span">收件箱</span></a></li>
                <li><a href=""><span class="glyphicon glyphicon-pencil icon_span" aria-hidden="true"></span><span class="icon_span">写邮件</span></a></li>
                <li><a href=""><span class="glyphicon glyphicon-inbox icon_span" aria-hidden="true"></span><span class="icon_span">草稿箱</span></a></li>
            </ul>
        </div>
    </div>
</template>

<style scoped>
    .nav_frame{
        position: relative;
        height: 100%;
    }
    .open_frame{
        position: relative;
        top: 0%;
        left: 0%;
        height: 100%;
        width: 3%;
        background-color: rgb(89, 97, 97);
        float: left;
    }
    .openBtn{
        position: relative;
        top: 50%;
        transform: translateY(-25%);
        margin: 0 auto;
        width: 100%;
    }
    .nav_li{
        position: relative;
        top: 0%;
        margin-left: 0%;
        float: left;
        height: 100%;
        width: 10%;
        background-color: rgb(158, 155, 126);
    }
    ul{
        position: relative;
        left: 0%;
        padding: 0%;
        width: 100%;
        margin: 2px;
    }
    li{
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
</style>
<script>
export default {
    data(){
        return{
            flag:false
        }
    },
    methods:{
        show:function(){
            this.flag=!this.flag
        }
    }
}
</script>