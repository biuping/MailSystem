<template>
    <div class="mail_frame" ref="main_frame">
        <ul class="mailbox">
            <li v-for="(mail,index) in maillist" :key="index">
                <div class="singleMail">
                    <span class="label label-primary">{{mail.title}}</span>
                    <span class="label label-info">发件人：{{mail.sender}}</span>
                </div>
            </li>
        </ul>
    </div>
</template>

<style scoped>
    .mail_frame{
        position: relative;
        height: 99%;
        top: 1%;
        left: 1%;
        width: 85%;
        background-color: rgba(156, 122, 97, 0.1);
        border-radius: 2%;
        padding: 0%;
    }
    li{
        list-style: none;
    }
    .mailbox{
        position: relative;
        left: 1%;
        top: 1%;
        padding: 0%;
    }
    .singleMail{
        position: relative;
        margin: 1%;
        background-color: cadetblue;
    }
</style>
<script>
export default {
    data(){
        return{
            flag:false,
            maillist:[{title:'first mail',sender:'tom'},{title:'second mail',sender:'mike'},{title:'third mail',sender:'jay'}]
        }
    },
    methods:{
        show:function(){
            this.flag=!this.flag
        },
        changeWidth:function(flag){
            console.log(flag)
            if(flag){
                this.$refs.main_frame.style.width="85%"
            }else{
                this.$refs.main_frame.style.width="95%"
            }
        }
    }
}
</script>