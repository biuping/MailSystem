<template>
    
    <div id="login-group">
        <form action="">
            <transition enter-active-class="fadeInDown" :="{enter:400}">
                <div class="headline animated" v-if="flag" >樱花邮箱</div>
            </transition>

            <transition  enter-active-class="slideInRight" :="{endter:400}">
                <div class="account-group animated" v-if="flag" >
                    <label v-if="flag"><span class="glyphicon glyphicon-user" aria-hidden="true"></span>用户</label>
                    <input type="email" class="form-control" id="exampleInputEmail1" placeholder="Email">
                </div>
            </transition>

            <transition  enter-active-class="slideInLeft" :="{endter:400}">
                
                <div class="pwd-group animated" v-if="flag">
                    <label >
                        <span class="glyphicon glyphicon-lock" aria-hidden="true"></span>密码
                    </label>
                    
                    <input type="password" class="form-control" id="exampleInputPassword1" placeholder="Password">
                </div>
            </transition>
            <transition  enter-active-class="flipInY" :="{endter:400}">             
               <button type="submit" class="btn btn-info animated" v-if="flag">登录</button>
            </transition>     
            
        </form>
        <div class="bg" ></div>
        
    </div>
    
</template>
<style scoped>
    #login-group{
        position: relative;
        top: 50%;
        margin: 0 auto;
        transform: translateY(-50%);
        width: 40%;
        height: 35%;
    }
    .bg{
        position: absolute;
        top: 0%;
        left: 0%;
        margin: 0;
        padding: 0;
        width: 100%;
        height: 100%;
        animation: frameAnimation 1.5s ease;
        animation-fill-mode:forwards;
        border-radius: 15px;
        z-index: -1;
    }
    .account-group,.pwd-group{
        position: relative;
        margin: 0 auto;
        left: 16%;
    }
    .headline{
        position: relative;
        top: 10%;
        left: 40%;
        padding-top: 3%;
        color:rgb(243, 9, 99); 
        font:30px Georgia, "Times New Roman", Times, serif; text-decoration:none;
        font-weight: bolder;
    }
    label{
        top: 10%;
        display: inline;
        color: rgb(14, 2, 4);
    }
    input{
        display: inline;
        top: 10%;
        width: 50%;
        margin: 20px;
    }
    button{
        position: relative;
        margin: 10px auto;
        left: 35%;
        width: 25%;

    }

    @keyframes frameAnimation{
        0%{
            background-color: rgba(0, 0, 0,0);
        }
        100%{
            background-color: rgba(0, 0, 0,0.3);
        }
    }
    
</style>
<script>
    export default{
        data(){
            return{
                flag:false
            }
        },
        methods:{
            show:function(){
                this.flag=!this.flag
            }
        },
        mounted(){
            
            this.flag=true
        }
    }

</script>
