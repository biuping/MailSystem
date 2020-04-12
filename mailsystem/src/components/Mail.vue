<template>
    <div>
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
		     <div id="uplode_group" class="uplode">
		         <button class="btn btn-info mybtn" id="input_display" >
		             <span class="glyphicon glyphicon-folder-open myspan" aria-hidden="true"></span>下载附件
		         </button>
		         
		         <input type="text" name="" id="filePath" disabled >
		     </div>
		     <textarea class="form-control" rows="22" readonly="" v-model="mail.content"></textarea>
		</div>
    </div>
</template>
<style scoped>
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
        height: 1px;
        background-color: blue;
    }
    input:disabled{
        cursor: auto;
    }
    .rec_label{
        position: absolute;
        width: 5%;
        top: 1%;
		left: 2%;
    }
	.recipient{
		position: absolute;
		top: -5%;
		left: 6%;
		padding: 0;
		width: 15%;
		border-bottom: 0.0625rem solid #ff0000;
		border-left: 0 transparent;
		border-top: 0 transparent;
		border-right: 0 transparent;
		border-radius: 0;
		background-color: transparent;
		outline: none;
	}
	.form-group{
		position: relative;
		margin: 2%;
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
export default {
    data(){
		return{
			mail:Object
		}
	},
	methods:{
		showMail(mail){
			this.mail=mail
			console.log(mail.theme)
		}
	}
}
</script>