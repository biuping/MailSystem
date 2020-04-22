<template>
    <div class="mail_bg">
        <div class="theme">
            <label class="theme_label">{{this.mail.theme}}</label>
        </div>
        <div class="cutoff_line"></div>
        <div class="recipient_frame">
            <label class="rec_label" >发件人:</label>
            <input type="text" class="recipient" readonly="readonly" v-model="mail.sender">                          
        </div>
		<div class="form-group">
		     <input type="file" id="thisfile" ref="inputFile" style="display:none" >
			 
		     <div id="uplode_group" class="uplode" v-show="aShow">
				<span class="glyphicon glyphicon-save myspan" aria-hidden="true">附件</span>
				<ul class="attachment_ul">
					<li v-for="(value,index) in this.mail.attachments" :key="index" @click="download(index,value.name)">
						{{value.name}}  {{transform(value.size)}}
					</li>
				</ul>
		     </div>
		     <textarea class="form-control" rows="22" readonly="" v-model="mail.content"></textarea>
		</div>
		<div class="button_frame">
			<button type="button" class="btn btn-info" @click="comeback">返回</button>
			<button type="button" class="btn btn-danger"  title="删除" @click="deleteMail">
			    <span class="glyphicon glyphicon-trash" aria-hidden="true"></span>
			</button>
		</div>
    </div>
</template>
<style scoped>
	.mail_bg{
		position: absolute;
		background-color: rgba(124, 124, 122, 0.6);
		width: 100%;
		height: 100%;
		border-radius: 2%;
	}
    .recipient_frame{
        position: relative;
        padding: 0%;
        margin-top: 1%;
		height: 1.25rem;
    }
    .theme{
        position: relative;
        margin: 2%;
        margin-bottom: 0%;
    }
    .theme_label{
        font-size: 25px;
        font-family:STHeiti;
        color: rgb(12, 44, 92);
    }
    .cutoff_line{
        width: 100%;
        height: 3px;
        background-color: rgb(248, 216, 200);
    }
    input:disabled{
        cursor: auto;
    }
    .rec_label{
		position: absolute;
		color: rgb(14, 45, 224);
        width: 5%;
        top: 1%;
		left: 2%;
		font-size: 20px;
    }
	.recipient{
		position: absolute;
		top: -50%;
		left: 7%;
		padding: 0;
		width: 80%;
		border-bottom: 2px solid #ff000075;
		border-left: 0 transparent;
		border-top: 0 transparent;
		border-right: 0 transparent;
		border-radius: 0;
		background-color: transparent;
		outline: none;
		font-size: 25px;

	}
	.form-group{
		position: relative;
		margin: 2%;
		margin-bottom: 0;
	}
	.inputfile_label{
	    position: relative;
	    float: left;
	}
	
	.uplode{
	    position: relative;
	    margin-top: 1%;
	    margin-bottom: 0;
		display: flex;
		flex-direction: row;
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
		font-size: 25px;
		color: rgb(11, 104, 47);
	}
	.button_frame{
		position: relative;
		width: 10%;
		margin-top: 2%;
		margin-left: 88%;
	}
	.button_frame button{
		position: relative;
		margin-left: 20px;
	}
	.attachment_ul{
		list-style: none;
		margin-left: 0;
		padding-left: 10px;
		/* background-color: brown; */
	}
	li{
		display: inline;
		line-height: 10px;
		float: left;
		margin-right: 20px;
		border: 2px solid #210beb75;
		border-radius: 8px;
		text-align: center;
		font-size: 20px;
		padding: 10px;
		background-color: rgba(248, 216, 200, 0.623);
	}
	li:hover{
		cursor: pointer;
		background-color: rgba(119, 189, 230, 0.459);
	}
</style>
<script>
import Axios from 'axios'
import Vue from 'vue'
Vue.prototype.$axios = Axios
import saveutil from '../utils/saveDraftUtil'
export default {
    data(){
		return{
			mail:Object,
			aShow:false
		}
	},
	methods:{
		showMail(mail){
			this.mail=mail
			this.aShow=false
			// 有附件才显示附件一行
			if(this.mail.attachments.length>0){
				this.aShow=true
			}
		},
		comeback:function(){
			this.$emit('backMailBox')
		},
		deleteMail:function(){
			var r = confirm("确认删除邮件？")
			if(r){
				const url = "http://127.0.0.1:8006/delete_mail"
				let userid = saveutil.readData('userid')
				let data = JSON.stringify({
					"id":userid,
					"mail_id":[this.mail.mail_id]
				})
				this.$axios({
					method:'post',
					url:url,
					data:data,
					headers:{'Content-Type':'application/json'}
				}).then(function(response){
						console.log("SUCCESS")
						let jstring = JSON.stringify(response.data)
						let info=JSON.parse(jstring)
						console.log(info)
						console.log(response)
					}.bind(this)).catch(function(error){
						console.log("ERROR")
						console.log(error)
					})
				this.$emit('deleteFromMail')
			}
			
		},
		download:function(index,fileName){
			const url = "http://127.0.0.1:8006/download_attach"
			let self = this
			let userid = saveutil.readData('userid')
			let data = JSON.stringify({
				"id":userid,
				"mailId":this.mail.mail_id,
				"attach_index":index
			})
			console.log(data)
			self.$axios({
				method:"post",
				url:url,
				data:data,
				headers:{'Content-Type':'application/json'},
				responseType:"blob"
			}).then(
				function(response){
					if(response.status==200){
						console.log('download')
						let res=response.data
						//创建一个a标签进行下载，但是不会跳转
						let url = window.URL.createObjectURL(new Blob([res]))
						let link = document.createElement("a")
						link.style.display="none"
						link.href=url
						link.setAttribute("download",fileName)
						document.body.appendChild(link)
						link.click()
					}					

				}
			).catch(function(err){
				alert('网络连接错误')
				console.log(err)
			})
		},
        transform:function(filesize){
			let fileSize = parseInt(filesize)
            var size = ""
            if(fileSize<0.1*1024){  //小于0.1KB，则转化成B
                size = fileSize.toFixed(2) + "B"
            }else if(fileSize < 0.1 * 1024 * 1024){            //小于0.1MB，则转化成KB
                size = (fileSize/1024).toFixed(2) + "KB"
            }else if(fileSize < 0.1 * 1024 * 1024 * 1024){        //小于0.1GB，则转化成MB
                size = (fileSize/(1024 * 1024)).toFixed(2) + "MB"
            }else{                                            //其他转化成GB
                size = (fileSize/(1024 * 1024 * 1024)).toFixed(2) + "GB"
            }
            var sizeStr = size + "";                        //转成字符串
            var index = sizeStr.indexOf(".");                    //获取小数点处的索引
            var dou = sizeStr.substr(index + 1 ,2)            //获取小数点后两位的值
            if(dou == "00"){                                //判断后两位是否为00，如果是则删除00                
                return sizeStr.substring(0, index) + sizeStr.substr(index + 3, 2)
            }
            return size
        }
	}
}
</script>