<template>
    <div class="mail_frame animated" ref="main_frame">
        <div class="mailbox">
            <ul class="mail_list">
                <transition-group >
                    <li v-for="(mail,index) in maillist" :key="mail.id" class="animated" >
                        <div class="singleMail">
                            <div class="mailTitle">
                                <span class="label ">{{mail.title}}</span>
                                <input type="checkbox" v-model="checkModel" :value="index">
                            </div>
                            
                            <div class="cutoff_line"></div>
                            <span class="label mailSender">发件人：{{mail.sender}}</span>
                        </div>
                    </li>
                </transition-group>
                
            </ul>
            <button type="button" class="btn btn-primary btn_checkall" @click="checkAll" ref="checkbtn">全选</button>
            <button type="button" class="btn btn-danger btn_delete" @click="deleteMail">
                <span class="glyphicon glyphicon-trash" aria-hidden="true"></span>
            </button>
        </div>       
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
        padding: 0%;
        height: 10%;
    }
    .mailbox{
        position: relative;
        margin: 0%;
        width: 95%;
        height: 100%;
        left: 2%;
        top: 0%;
        padding: 0%;
        
    }
    .mail_list{
        position: relative;
        overflow-y: scroll;
        height: 90%;
        width: 100%;
        padding: 0%;
    }
    .singleMail{
        position: relative;
        border-radius: 10px;
        display: flex;
        flex-direction: column;
        height: 100%;
        margin-top: 1%;
        background-color:  rgba(80, 159, 190, 0.1);
        cursor: pointer;
    }
    .singleMail:hover{
        background-color:  rgba(80, 159, 190, 0.3);
    }
    .cutoff_line{
        position: relative;
        width: 100%;
        height: 1px;
        background-color: rgb(161, 174, 192);
    }
    .mailTitle{
        position: relative;
        width: 100;
        margin: 10px 1% 5px 1%;
        text-align: left;
        padding: 0%;
    }
    .mailTitle span{
        margin: 0;
        padding: 0%;
        font-size: 20px;
        color: black;
    }
    .mailTitle input[type=checkbox]{
        position: absolute;
        width: 20px;
        height: 20px;
        right: 10px;
        margin: 0;
    }
    .mailSender{
        position: relative;
        top: 5px;
        margin: 0 1% 5px 1%;
        text-align: left;
        font-size: 15px;
        color: rgb(93, 168, 137);
        padding: 0%;
    }
    .btn_checkall{
        position: absolute;
        left: 1%;
        bottom: 2%;
    }
    .btn_delete{
        position: absolute;
        left: 8%;
        bottom: 2%;
    }
    .v-enter,
    .v-leave-to {
      opacity: 0;
      transform: translateX(150px);
    }

    /* v-enter-active 【入场动画的时间段】 */
    /* v-leave-active 【离场动画的时间段】 */
    .v-enter-active,
    .v-leave-active{
      transition: all 0.8s ease;
    }
</style>
<script>
export default {
    data(){
        return{
            flag:true,
            maillist:[{id:1,title:'first mail',sender:'tom'},{id:2,title:'second mail',sender:'mike'},{id:3,title:'third mail',sender:'jay'},
                {id:4,title:'first mail',sender:'tom'},{id:5,title:'second mail',sender:'mike'},{id:6,title:'third mail',sender:'jay'},
                {id:7,title:'first mail',sender:'tom'},{id:8,title:'second mail',sender:'mike'},{id:9,title:'third mail',sender:'jay'}
            ],
            allchecked:false,   //是否全选判断
            checkModel:[]       //双向绑定选取的数组
        }
    },
    methods:{
        show:function(){
            this.flag=!this.flag
        },
        changeWidth:function(flag){
            if(flag){
                
                this.$refs.main_frame.style.width="85%"
            }else{
                this.$refs.main_frame.style.width="95%"
            }
        },
        checkAll:function(){
            if(this.allchecked){
                this.checkModel=[]
                this.$refs.checkbtn.textContent='全选'
            }else{
                this.$refs.checkbtn.textContent='全不选'
                this.checkModel=[]
                this.maillist.forEach((item)=>{
                    this.checkModel.push(this.maillist.indexOf(item))
                })
            }
        },
        deleteMail:function(){
            // 通过index删除mail：存在问题，邮件index是变化的，
            // 比如要删除index为1和3，如果同时勾选删除(在checkModel数组中1在前的情况)，删除了1，导致index=3的变为index=2
            // 但是checkModel中没有变化，还是3，所以删除错误
            // 所以用originlength保持最开始maillist的长度
            const originlength = this.maillist.length
            if(this.allchecked){
                this.$refs.checkbtn.textContent='全选'
            }
            this.checkModel.forEach((index)=>{
                let deleteIndex = index-(originlength-this.maillist.length)
                this.maillist.splice(deleteIndex,1)
            })
            this.checkModel=[]
        }
    },
    watch:{
        checkModel(){
            if(this.checkModel.length==this.maillist.length){
                this.allchecked=true
            }else{
                this.allchecked=false
            }
        }
    }
}
</script>