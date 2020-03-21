<template>
    <div class="sendmail_frame" ref="main_frame">
        
        <ul class="nav nav-pills">
            <li role="presentation" class="left"><button type="button" class="btn btn-primary">发送</button></li>
            <li role="presentation" class="left"><button type="button" class="btn btn-info">存草稿</button></li>
            <li role="presentation" class="right"><button type="button" class="btn btn-danger"><span class="glyphicon glyphicon-trash" aria-hidden="true"></span></button></li>
        </ul>
        <div class="cutoff_line"></div>
        <div class="input-group recive">
            <span class="input-group-addon" id="basic-addon3">收件人</span>
            <input type="email" class="form-control" id="basic-url" aria-describedby="basic-addon3">
        </div>
        <div>
            <div class="input-group">
                <span class="input-group-addon " id="basic-addon">主题</span>
                <input type="email" class="form-control" id="basic-url" aria-describedby="basic-addon3">               
            </div>
            <div class="form-group">
                <label class="inputfile_label"><span class="glyphicon glyphicon-folder-open" aria-hidden="true"></span>上传附件</label>
                <input type="file" id="inputFile">
                <textarea class="form-control" rows="3"></textarea>
           </div>
        </div>

        <!-- <span class="label label-primary">Primary</span> -->
    </div>
</template>
<style scoped>
    .sendmail_frame{
        position: relative;
        height: 100%;
        top: 1%;
        left: 1%;
        width: 98%;
        height: 98%;
        background-color: rgba(156, 122, 97, 0.1);
        border-radius: 2%;
    }
    ul{
        position: relative;
        top: 0%;
        margin: 1%;
        width: 100%;

    }
    .left{
       position: relative;
       margin-right: 2%;

    }
    .right{
        position: absolute;
        right: 2%;
    }
    .cutoff_line{
        width: 100%;
        height: 1px;
        background-color: blue;
    }
    .recive{
        position: relative;
        margin-top: 1%;
        margin-left: 1%;
        width: 20%;
    }
    .inputfile_label{
        position: relative;
        margin: 1%;
        float: left;
    }
    #inputFile{
        position: relative;
        float: left;
        margin-top: 1%;
        /* margin-top: 1%; */
    }
</style>
<script>
export default {
    data(){
        return {
            flag:false
        }
    },
    mounted(){
        if(this.flag){
            this.$refs.main_frame.style.width="85%"
        }else{
            this.$refs.main_frame.style.width="95%"
        }
    },
    methods:{
        changeWidth:function(flag){
            if(flag){
                this.$refs.main_frame.style.width="85%"
            }else{
                this.$refs.main_frame.style.width="95%"
            }
        }
    }
}
</script>
