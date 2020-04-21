<template>
    <form class="sendmail_frame" ref="main_frame">
        
        <ul class="nav nav-pills">
            <li role="presentation" class="left"><button type="submit" class="btn btn-primary" @click="sendMail($event)">发送</button></li>
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
                <input type="file" id="thisfile" ref="inputFile" style="display:none" @change="fileChange($event)">
                <div id="uplode_group" class="uplode">
                    <button class="btn btn-info mybtn" id="input_display" @click="uploadFile" type="button">
                        <span class="glyphicon glyphicon-folder-open myspan" aria-hidden="true"></span>上传附件
                    </button>
                    
                    <input type="text" name="" id="filePath" disabled v-model="fileName">
                </div>
                <textarea class="form-control" rows="22" v-model="draftMail.content"></textarea>
           </div>
        </div>

        <!-- <span class="label label-primary">Primary</span> -->
    </form>
</template>
<style scoped>
    .sendmail_frame{
        position: relative;
        height: 99%;
        top: 1%;
        left: 1%;
        width: 95%;
        background-color: rgba(156, 122, 97, 0.3);
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
        height: 2px;
        background-color: rgb(30, 31, 24);
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
import Axios from 'axios'
import Vue from 'vue'
import qs from 'querystring'
Axios.defaults.headers.post['Content-Type']='multipart/form-data'

Vue.prototype.$http = Axios
export default {
    data(){
        return {
            flag:false,
            fileName:'',
            file:'',
            draftMail:{recipient:'',theme:'',attachment:Object,attachmentName:'',content:''}
        }
    },
    methods:{
        uploadFile:function(){
            this.$refs.inputFile.dispatchEvent(new MouseEvent('click'))
            
        },
        fileChange:function(e){
            this.fileName=this.$refs.inputFile.files[0].name
            this.file=e.target.files[0]
            console.log(this.file)
        },
        refresh:function(){
            this.$router.go(0)
        },
        saveDraft(){
            if(this.fileName!=''){
                this.draftMail.attachmentName=this.fileName
                this.draftMail.attachment=this.$refs.inputFile.files[0]
                console.log(this.$refs.inputFile.files[0])
            }
            savedraftutil.saveData('draft_mail',this.draftMail)
            this.refresh()
            alert('保存草稿成功')
        },
        sendMail:function(event){
            event.preventDefault()
            let formData = new FormData()
            let userid = savedraftutil.readData('userid')
            formData.append("id",userid)

            formData.append("attachment",this.file)           
            formData.append("recver",this.draftMail.recipient)
            formData.append("content",this.draftMail.content)
            formData.append("theme",this.draftMail.theme)
            let config={
                headers:{
                    'Content-Type':'multipart/form-data'
                }
            }
            const url = "http://127.0.0.1:8006/send_mail_with_attach"
            this.$http.post(url,formData,config).then(function(res){
                console.log(res)
                if(res.status==200){
                    console.log("success send")
                }
            })

        }
       

    }
}
</script>
