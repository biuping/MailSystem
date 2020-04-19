
const path = require('path')

const htmlwebpackplugin = require('html-webpack-plugin')
const VueLoaderPlugin = require('vue-loader/lib/plugin')

module.exports={
	entry: {main:'./src/main.js'},  //入口表示webpack打包文件
	output:{
		// 输出文件配置
		path:path.resolve(__dirname, './dist'),  // 指定打包到的路径
		filename: '[name]-[hash].js'  //打包的文件名
	},
	devServer:{
		open:'chrome',
		port:8006,
		contentBase:'static',
		hot:true
	},
	plugins:[
		new htmlwebpackplugin({  // 创建一个在内存中生成HTML的插件
			template: path.resolve(__dirname,'./static/index.html'),   //指定模板路径
			filename: 'index.html', //指定生成的页面名称
			inject:true,
			chunks:['main']
		}),
		new VueLoaderPlugin()
	],
	module:{
		rules:[
			// 利用正则匹配，/表示开始匹配 $/表示结束
			{test:/\.css$/,use:[{loader:'style-loader'},{loader:'css-loader'}]},
			{test:/\.(jpg|png|gif|jpeg|bmp)$/,
			use:{loader:'url-loader', options:{
				limit:false,
				name:'[path][name].[ext]',
			}}}  ,//因为url-loader和file-loader是绑定的所以不用引入file-loader
			{test:/\.(ttf|eot|svg|woff|woff2)$/,use:{loader:'url-loader'}}, // 处理字体文件
			{test:/\.js$/,exclude:/node_modules/,use:{loader:'babel-loader'}},  //配置babel转换高级的js文件
			{test:/\.vue$/,use:{loader:'vue-loader'}}
		]
	},
	resolve:{
		alias:{
			'vue$':"vue/dist/vue.js"
		}
	}
}