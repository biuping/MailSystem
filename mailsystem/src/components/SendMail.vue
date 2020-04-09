<template>
    <div class="sendmail_frame" ref="main_frame">
        
        <ul class="nav nav-pills">
            <li role="presentation" class="left"><button type="button" class="btn btn-primary">发送</button></li>
            <li role="presentation" class="left"><button type="button" class="btn btn-info" @click="saveDraft">存草稿</button></li>
            <li role="presentation" class="right"><button type="button" class="btn btn-danger" @click="refresh">
                <span class="glyphicon glyphicon-trash" aria-hidden="true"></span></button>
            </li>
        </ul>
        <div class="cutoff_line"></div>
        <div class="input-group recive">
            <span class="input-group-addon" id="basic-addon3">收件人</span>
            <input type="email" class="form-control" aria-describedby="basic-addon3" v-model="draftMail.recipient">
        </div>
        <div class="mail_group">
            <div class="input-group">
                <span class="input-group-addon " id="basic-addon">主题</span>               
                <input type="text" class="form-control" id="basic-url" aria-describedby="basic-addon" v-model="draftMail.theme">
               
                               
            </div>
            <div class="form-group">
                <input type="file" id="thisfile" ref="inputFile" style="display:none" @change="fileChange">
                <div id="uplode_group" class="uplode">
                    <button class="btn btn-info mybtn" id="input_display" @click="uploadFile" >
                        <span class="glyphicon glyphicon-folder-open myspan" aria-hidden="true"></span>上传附件
                    </button>
                    
                    <input type="text" name="" id="filePath" disabled v-model="fileName">
                </div>
                <textarea class="form-control" rows="22" v-model="draftMail.content"></textarea>
           </div>
        </div>

        <!-- <span class="label label-primary">Primary</span> -->
    </div>
</template>
<style scoped>
    .sendmail_frame{
        position: relative;
        height: 99%;
        top: 1%;
        left: 1%;
        width: 95%;
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
    .mail_group{
        position: relative;
        margin: 1%;
    }

    .inputfile_label{
        position: relative;
        float: left;
    }

    .uplode{
        position: relative;
        margin-top: 1%;
        margin-bottom: 1%;
    }
    .uplode input{
        position: relative;

        margin-bottom: 0%;
    }
    .mybtn{
        position: relative;
        vertical-align: middle;
    }
    .myspan{
        position: relative;
        margin: 2px;
    }
    

</style>
<script>
import savedraftutil from '../utils/saveDraftUtil'
export default {
    data(){
        return {
            flag:false,
            fileName:'',
            draftMail:{recipient:'',theme:'',attachment:Object,attachmentName:'',content:''}
        }
    },
    methods:{
        uploadFile:function(){
            this.$refs.inputFile.dispatchEvent(new MouseEvent('click'))
            
        },
        fileChange:function(e){
            this.fileName=this.$refs.inputFile.files[0].name
        },
        refresh:function(){
            this.$router.go(0)
        },
        saveDraft(){
            if(this.fileName!=''){
                this.draftMail.attachmentName=this.fileName
                this.draftMail.attachment=this.$refs.inputFile.files[0]
            }
            savedraftutil.saveData('draft_mail',this.draftMail)
            this.refresh()
            alert('保存草稿成功')
        }
       

    }
}
</script>
