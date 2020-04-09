export default{
    readData(key){
        return JSON.parse(window.localStorage.getItem(key)||'[]')
    },
    saveData(key,data){
        window.localStorage.setItem(key,JSON.stringify(data))
    },
    deleteData(key){
        window.localStorage.removeItem(key)
    }
}