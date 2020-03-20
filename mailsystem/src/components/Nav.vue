<template>
    <div class="nav_frame">
        <div class="open_frame">
            <button class="btn btn-info openBtn" @click="show">
                <span class="glyphicon glyphicon-off" aria-hidden="true"></span>
            </button>
        </div>
        <div class="nav_li" v-if="flag">

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
        width: 15%;
        background-color: rgb(12, 110, 110);
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