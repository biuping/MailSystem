<template>
    <div id="login-group">
        <form action="">
            <div class="account-group">
                <span class="glyphicon glyphicon-user" aria-hidden="true">用户</span>
                <input type="email" class="form-control" id="exampleInputEmail1" placeholder="Email">
            </div>
            <div class="pwd-group">
                <span class="glyphicon glyphicon-lock" aria-hidden="true">密码</span>
                <input type="password" class="form-control" id="exampleInputPassword1" placeholder="Password">
            </div>
             <button type="submit" class="btn btn-info">登录</button>
        </form>
    </div>
</template>
<style scoped>
    #login-group{
        position: relative;
        top: 50%;
        margin: 0 auto;
        transform: translateY(-50%);
        width: 40%;
        height: 30%;
        background-color: rgba(0, 0, 0,0.1);

    }
    .account-group,.pwd-group{
        position: relative;
        margin: 0 auto;
        left: 16%;
    }
    span{
        top: 10%;
        display: inline;
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
    
</style>
<script>

</script>
