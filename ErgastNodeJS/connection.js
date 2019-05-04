const mysql = require("mysql");

const pool = mysql.createPool({
    connectionLimit:20,
    host: "localhost",
    //host: "ubuntuserver.local",
    user: "root",
    password: "f1",
    database: "ergastdb"
});

function getMySQLConnection(){
    return pool;
}

module.exports = {
    getMySQLConnection : function()
    {
        return getMySQLConnection();
    }
}