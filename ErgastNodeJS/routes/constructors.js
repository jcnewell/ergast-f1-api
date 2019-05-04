const express = require("express");
const router = express.Router();
const path = require("path");

let MySQLCofiguration = require("../connection.js");


//Supported Function
function formattedConstructor(row)
{
    const constructor = row.map((row) => {
        return { 
            constructorId: row.constructorRef, 
            url: row.url, 
            name : row.name, 
            nationality : row.nationality 
        };
    });
    return constructor;
}


//API 2.0
router.get("/help", (req,res) => {
    res.status(200).sendFile(path.join( __dirname,"../public","constructors.html"));
});
// /constructors
router.get("/all", (req,res) => {

    let offset = (typeof req.query.offset != 'undefined') ? parseInt(req.query.offset) : 0;
    let limit = (typeof req.query.limit != 'undefined') ? parseInt(req.query.limit) : 30;

    let sql = `SELECT * FROM constructors ORDER BY name LIMIT ${offset}, ${limit}`;
    const conn = MySQLCofiguration.getMySQLConnection();
    conn.query(sql,(err,rows,fields) => {
        
        if(err){
            console.log("Failed to query for " + __filename.slice(__filename.lastIndexOf(path.sep)+1) + ": "+ err);
            res.status(500);
            res.end();
            return;
        }
        res.json({
            "MRData":{
                "limit":limit.toString(),
                "offset":offset.toString(),
                ConstructorTable : {
                    "Constructors" : formattedConstructor(rows)
                }
            }
        });
    });
});
// /constructor/2019
router.get("/:year([0-9]{4}|current)", (req,res) => {
    let offset = (typeof req.query.offset != 'undefined') ? parseInt(req.query.offset) : 0;
    let limit = (typeof req.query.limit != 'undefined') ? parseInt(req.query.limit) : 30;

    let sql =   `SELECT DISTINCT constructors.*
                FROM constructors,constructorResults,races
                WHERE constructors.constructorId = constructorResults.constructorId 
                AND constructorResults.raceId = races.raceId AND races.year = ?
                ORDER BY constructors.name
                LIMIT ${offset}, ${limit}`;
    const conn = MySQLCofiguration.getMySQLConnection();
    console.log("qui");
    let year = (req.params.year == "current") ? new Date().getFullYear().toString() : req.params.year;
    conn.query(sql, [year],(err,rows,fields) => {
        if(err){
            console.log("Failed to query for " + __filename.slice(__filename.lastIndexOf(path.sep)+1) + ": "+ err);
            res.status(500);
            res.end();
            return;
        }
        res.json({
            "MRData":{
                "limit":limit.toString(),
                "offset":offset.toString(),
                ConstructorTable : {
                    "season" : year,
                    "Constructors" : formattedConstructor(rows)
                }
            }
        });
    });
});

// /constructor/2019/2
router.get("/:year([0-9]{4}|current)/:round([0-9]{1,2})", (req,res) => {
    let offset = (typeof req.query.offset != 'undefined') ? parseInt(req.query.offset) : 0;
    let limit = (typeof req.query.limit != 'undefined') ? parseInt(req.query.limit) : 30;

    let sql =   `SELECT DISTINCT constructors.*
                FROM constructors,constructorResults,races
                WHERE constructors.constructorId = constructorResults.constructorId 
                AND constructorResults.raceId = races.raceId AND races.year = ? AND races.round = ?
                ORDER By constructors.name
                LIMIT ${offset}, ${limit}`;
    const conn = MySQLCofiguration.getMySQLConnection();
    let year = (req.params.year == "current") ? new Date().getFullYear().toString() : req.params.year;
    conn.query(sql, [year,req.params.round],(err,rows,fields) => {
        if(err){
            console.log("Failed to query for " + __filename.slice(__filename.lastIndexOf(path.sep)+1) + ": "+ err);
            res.status(500);
            res.end();
            return;
        }
        res.json({
            "MRData":{
                "limit":limit.toString(),
                "offset":offset.toString(),
                ConstructorTable : {
                    "season" : year,
                    "round" : req.params.round,
                    "Constructors" : formattedConstructor(rows)
                }
            }
        });
    });
});

// /constructors/name/mclaren
router.get("/name/:name", (req,res) => {
    let offset = (typeof req.query.offset != 'undefined') ? parseInt(req.query.offset) : 0;
    let limit = (typeof req.query.limit != 'undefined') ? parseInt(req.query.limit) : 30;

    let sql = `SELECT * FROM constructors WHERE constructorRef = ? LIMIT ${offset}, ${limit}`;
    const conn = MySQLCofiguration.getMySQLConnection();
    conn.query(sql, [req.params.name],(err,rows,fields) => {
        if(err){
            console.log("Failed to query for " + __filename.slice(__filename.lastIndexOf(path.sep)+1) + ": "+ err);
            res.status(500);
            res.end();
            return;
        }
        res.json({
            "MRData":{
                "limit":limit.toString(),
                "offset":offset.toString(),
                ConstructorTable : {
                    "constructorId" : req.params.name,
                    "Constructors" : formattedConstructor(rows)
                }
            }
        });
    });
});

// /constructor/constructorStandings/1
router.get("/constructorStandings/:position([0-9]{1,2})", (req,res) => {
    let offset = (typeof req.query.offset != 'undefined') ? parseInt(req.query.offset) : 0;
    let limit = (typeof req.query.limit != 'undefined') ? parseInt(req.query.limit) : 30;

    let sql =   `SELECT DISTINCT constructors.* 
                FROM constructors, constructorStandings,races
                WHERE constructorStandings.constructorId=constructors.constructorId AND constructorStandings.raceId=races.raceId AND (races.year, races.round) IN (SELECT year, MAX(round) FROM races GROUP BY year) 
                AND constructorStandings.positionText = ?
                ORDER BY constructors.name
                LIMIT ${offset}, ${limit}`;
    const conn = MySQLCofiguration.getMySQLConnection();
    conn.query(sql, [req.params.position],(err,rows,fields) => {
        if(err){
            console.log("Failed to query for " + __filename.slice(__filename.lastIndexOf(path.sep)+1) + ": "+ err);
            res.status(500);
            res.end();
            return;
        }
        res.json({
            "MRData":{
                "limit":limit.toString(),
                "offset":offset.toString(),
                ConstructorTable : {
                    "constructorStandings" : req.params.position,
                    "Constructors" : formattedConstructor(rows)
                }
            }
        });
    });
});


// /year/2019/round/2/circuit/circuitId/constructor/constructorId/driver/driverId/grid/position/result/position/fastest/rank/status/statusId
router.get(/^.{0,}$/, (req,res,next) => {
    let offset = (typeof req.query.offset != 'undefined') ? parseInt(req.query.offset) : 0;
    let limit = (typeof req.query.limit != 'undefined') ? parseInt(req.query.limit) : 30;
    let argument = req.originalUrl.substring(1).split("/");
    argument.shift();//remove constructors
    if(argument.length%2 != 0)
    {
        //url is badly built
        res.status(400).end();//TODO fix this
        return;
    }

    let sql = `SELECT DISTINCT constructors.* FROM constructors`;

    let circuit = null;
    let driver = null;
    let constructor = null;
    let grid = null;
    let result = null;
    let fastest = null;
    let status = null;
    let year = null;
    let round = null;

    for(let i = 0; i < argument.length - 1 && argument[i]!=""; i++)
    {
        switch(argument[i])
        {
            case "circuit" : circuit = argument[i+1]; break;
            case "driver" : driver = argument[i+1]; break;
            case "grid" : grid = argument[i+1]; break;
            case "result" : result = argument[i+1]; break;
            case "fastest" : fastest = argument[i+1]; break;
            case "status" : status = argument[i+1]; break;
            case "constructor" : constructor = argument[i+1]; break;
            case "year" : (argument[i+1] == "current") ? year = new Date().getFullYear().toString() : year = argument[i+1]; break;
            case "round" : round = argument[i+1]; break;
        }
    }

    if(circuit == null && driver == null && constructor == null && grid == null && result == null && fastest == null && status == null && year == null && round == null)
    {
        //url is badly built
        res.status(400).end();//TODO fix this
        return;
    }

    //Set the table
    if(arrayContains(argument,["year","driver","status","grid","result","circuit","fastest"])) sql += ", results";
    if(arrayContains(argument,["year","circuit","driverStandings","constructorStandings"])) sql += ", races";
    if(arrayContains(argument,["circuit"])) sql += ", circuits";
    if(arrayContains(argument,["driver"])) sql += ", drivers";

    sql+= " WHERE TRUE";
    //Set the join
    if(arrayContains(argument,["year","driver","status","grid","result","circuit","fastest"])) sql += " AND constructors.constructorId=results.constructorId";
    if(arrayContains(argument,["year","circuit"])) sql += " AND results.raceId=races.raceId";

    if(arrayContains(argument,["circuit"])) sql += ` AND races.circuitId=circuits.circuitId AND circuits.circuitRef='${circuit}'`;
    if(arrayContains(argument,["driver"])) sql += ` AND results.driverId=drivers.driverId AND drivers.driverRef='${driver}'`;
    if(arrayContains(argument,["grid"])) sql += ` AND results.grid='${grid}'`;
    if(arrayContains(argument,["result"])) sql += ` AND results.positionText='${result}'`;
    if(arrayContains(argument,["fastest"])) sql += ` AND results.rank='${fastest}'`;
    if(arrayContains(argument,["status"])) sql += ` AND results.statusId='${status}'`;
    if(arrayContains(argument,["year"])) sql += `  AND races.year='${year}'`;
    if(arrayContains(argument,["constructor"])) sql += ` AND constructors.constructorRef='${constructor}'`;
    if(arrayContains(argument,["round"])) sql += ` AND races.round='${round}'`;

    
    sql += ` ORDER BY constructors.name LIMIT ${offset}, ${limit}`;
    const conn = MySQLCofiguration.getMySQLConnection();
    conn.query(sql,(err,rows,fields) => {
        if(err){
            console.log("Failed to query for " + __filename.slice(__filename.lastIndexOf(path.sep)+1) + ": "+ err);
            res.status(500);
            res.end();
            return;
        }
            
        let json = {
            "MRData":{
                "limit":limit.toString(),
                "offset":offset.toString(),
                "ConstructorTable" : {}
            }
        };
        if(circuit!=null)
            json.MRData.ConstructorTable.circuitId = circuit;
        if(driver!=null)
            json.MRData.ConstructorTable.driverId = driver;
        if(constructor!=null)
            json.MRData.ConstructorTable.constructor = constructor;
        if(grid!=null)
            json.MRData.ConstructorTable.grid = grid;
        if(result!=null)
            json.MRData.ConstructorTable.result = result;
        if(fastest!=null)
            json.MRData.ConstructorTable.fastest = fastest;
        if(status!=null)
            json.MRData.ConstructorTable.status = status;
        if(year!=null)
            json.MRData.ConstructorTable.season = year;
        
        json.MRData.ConstructorTable.Constructors = formattedConstructor(rows);
        res.json(json);
    });
});

function arrayContains(mainArray,propertyArray)
{
    let contains = false;
    for(let j =0; j < mainArray.length && !contains; j++)
    {
        for(let i =0; i < propertyArray.length && !contains; i++)
        {
            if(mainArray[j] == propertyArray[i])
                contains = true;
        }
    }
    
    return contains;
}

module.exports = router;