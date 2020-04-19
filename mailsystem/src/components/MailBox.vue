<template>
    <div class="mail_frame animated" ref="main_frame">
        <div class="mailbox" v-if="flag">
            <ul class="mail_list">
                <transition-group >
					
                    <li v-for="(mail,index) in maillist" :key="mail.id" class="animated">
						
                        <input type="checkbox" v-model="checkModel" :value="index">
                        <div class="singleMail" @click="toMail(mail,index)">
							<!-- <router-link :to="{path:'/mailbox/'+mail.id,params:{mail:mail.theme}}" > -->
                                <div class="mailTitle">
                                    <span class="label ">{{mail.theme}}</span>								                                
                                </div>
                                
                                <div class="cutoff_line"></div>
                                <span class="label mailSender">发件人：{{mail.sender}}</span>
							<!-- </router-link>	 -->
							
                        </div>
						
						
                    </li>
                </transition-group>
                
            </ul>
            <button type="button" class="btn btn-primary btn_checkall" @click="checkAll" ref="checkbtn">全选</button>
            <button type="button" class="btn btn-danger btn_delete" @click="deleteMail">
                <span class="glyphicon glyphicon-trash" aria-hidden="true"></span>
            </button>
            
        </div>
        <!-- <router-view ref="mailshow" v-show="childFlag"></router-view> -->
        <mailcom v-show="childFlag" ref="mailshow" @backMailBox='backMailBox' @deleteFromMail='deleteFromMail'></mailcom>
    </div>
</template>

<style scoped>
    .mail_frame{
        position: relative;
        height: 99%;
        top: 1%;
        left: 1%;
        width: 95%;
        background-color: rgba(68, 68, 67, 0.2);
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
        background-color:  rgba(228, 235, 238, 0.8);
        cursor: pointer;
    }
    .singleMail:hover{
        background-color:  rgba(105, 165, 189, 0.5);
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
    input[type=checkbox]{
        position: relative;
        width: 20px;
        height: 20px;
        right: 2%;
		margin-right: 0.625rem;
		margin-top: 0.25rem;
		top: 8%;
		z-index: 1;
		float: right;
    }
    .mailSender{
        position: relative;
        top: 5px;
        margin: 0 1% 5px 1%;
        text-align: left;
        font-size: 15px;
        color: rgb(218, 51, 22);
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
import mailcom from './Mail.vue'

export default {   
    data(){
        return{
            flag:true,
            maillist:[{id:1,theme:'firstmail',sender:'tom',content:'内容',attatchmentName:'',attatchment:''},
			{id:2,theme:'second mail',sender:'mike'},{id:3,theme:'third mail',sender:'jay'},
                {id:4,theme:'first mail',sender:'tom'},{id:5,theme:'second mail',sender:'mike'},{id:6,theme:'third mail',sender:'jay'},
                {id:7,theme:'first mail',sender:'tom'},{id:8,theme:'second mail',sender:'mike'},{id:9,theme:'third mail',sender:'jay'}
            ],
            allchecked:false,   //是否全选判断
            childFlag:false,
			choosedMailindex:0,
            checkModel:[]       //双向绑定选取的数组
        }
    },
	routes:[		
        {children:[{path:'mailbox/:id',component:mailcom}]}
	],
    methods:{
        show:function(){
            this.flag=!this.flag
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
        },
        toMail:function(mailinfo,index){
            this.flag=!this.flag
            this.childFlag=!this.childFlag
			this.choosedMailindex=index

			this.$refs.mailshow.showMail(mailinfo)
        },
		backMailBox:function(){
            if(this.flag==false){
                this.flag=!this.flag
                this.childFlag=!this.childFlag
            }

        },
        deleteFromMail:function(){
            this.maillist.splice(this.choosedMailindex,1)
            this.backMailBox()
        }
    },
	components:{
		mailcom
	},
    watch:{
        checkModel(){
            if(this.checkModel.length==this.maillist.length){
                this.allchecked=true
            }else{
                this.allchecked=false
            }
        },
		$route:{
			
			handler(val,oldval){
			
				this.flag=true
				console.log(this.$route.params.deleteFlag)
			},
			// deep:true
		}
    },
	mounted() {
        console.log(this.$route.params.childF)
        console.log('ch'+this.childFlag)
		this.flag=true
	}
}
</script>