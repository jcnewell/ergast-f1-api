const express = require("express");
const router = express.Router();
const path = require("path");

let MySQLCofiguration = require("../connection.js");

//Supported Function
function formattedRaces(rows)
{
    return rows.map((row) => {
        return { 
            season : row.year.toString(),
            round : row.round.toString(),
            url: row.url,
            raceName : row.name,
            Circuit : {
                circuitId : row.circuitRef,
                url : row.circuitUrl,
                circuitName : row.circuitName,
                Location : {
                    lat : row.lat.toString(),
                    long : row.lng.toString(),
                    alt :  (row.alt != null) ? row.alt.toString() : "N/D",
                    locality : row.location,
                    country : row.country
                }
            },
            date : row.date,
            time : row.time + "Z"
        };
    });
}
//API 2.0
router.get("/help", (req,res) => {
    res.status(200).sendFile(path.join( __dirname,"../public","raceSchedule.html"));
});

// /races/2019
router.get("/:year([0-9]{4}|current)", (req,res) => {
    let offset = (typeof req.query.offset != 'undefined') ? parseInt(req.query.offset) : 0;
    let limit = (typeof req.query.limit != 'undefined') ? parseInt(req.query.limit) : 30;

    let sql =   `SELECT ra.year, ra.round, ra.name, DATE_FORMAT(ra.date, '%Y-%m-%d') AS 'date', DATE_FORMAT(ra.time, '%H:%i:%S') AS 'time', ra.url, c.*
                FROM races ra, circuits c 
                WHERE ra.circuitId=c.circuitId AND ra.year = ?
                LIMIT ${offset}, ${limit};`;
    const conn = MySQLCofiguration.getMySQLConnection();
    let year = (req.params.year == "current") ? new Date().getFullYear().toString() : req.params.year;
    conn.query(sql,[year],(err,rows,fields) => {
        
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
                RaceTable : {
                    "season" : year,
                    "Races" : formattedRaces(rows)
                }
            }
        });
    });
});
// /races/2019/2
router.get("/:year([0-9]{4}|current)/:round([0-9]{1,2})", (req,res) => {
    let offset = (typeof req.query.offset != 'undefined') ? parseInt(req.query.offset) : 0;
    let limit = (typeof req.query.limit != 'undefined') ? parseInt(req.query.limit) : 30;

    let sql =   `SELECT ra.year, ra.round, ra.name, DATE_FORMAT(ra.date, '%Y-%m-%d') AS 'date', DATE_FORMAT(ra.time, '%H:%i:%S') AS 'time', ra.url, c.*
                FROM races ra, circuits c 
                WHERE ra.circuitId=c.circuitId AND ra.year = ? AND ra.round = ?
                LIMIT ${offset}, ${limit};`;
    const conn = MySQLCofiguration.getMySQLConnection();
    let year = (req.params.year == "current") ? new Date().getFullYear().toString() : req.params.year;
    conn.query(sql,[year,req.params.round],(err,rows,fields) => {
        
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
                RaceTable : {
                    "season" : year,
                    "round" : req.params.round,
                    "Races" : formattedRaces(rows)
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
    argument.shift();//remove races
    if(argument.length%2 != 0)
    {
        //url is badly built
        res.status(400).end();//TODO fix this
        return;
    }

    let sql = `SELECT ra.year, ra.round, ra.name, DATE_FORMAT(ra.date, '%Y-%m-%d') AS 'date', DATE_FORMAT(ra.time, '%H:%i:%S') AS 'time', ra.url, c.*
              FROM races ra, circuits c`;

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

    if(circuit == null && driver == null && constructor == null && grid == null && result == null && fastest == null && status == null && year == null )
    {
        //url is badly built
        res.status(400).end();//TODO fix this
        return;
    }

    //Set the table
    if(arrayContains(argument,["constructor","grid","result","status","fastest","driver"])) sql += ", results re";
    if(arrayContains(argument,["driver"])) sql += ", drivers";
    if(arrayContains(argument,["constructor"])) sql += ", constructors";

    sql+= " WHERE  ra.circuitId=c.circuitId";


    //Set the join
    if(arrayContains(argument,["constructor","grid","result","status","fastest","driver"])) sql += " AND ra.raceId=re.raceId";
    if(arrayContains(argument,["circuit"])) sql += ` AND c.circuitRef='${circuit}'`;
    if(arrayContains(argument,["driver"])) sql += ` AND re.driverId=drivers.driverId AND drivers.driverRef='${driver}'`;
    if(arrayContains(argument,["grid"])) sql += ` AND re.grid='${grid}'`;
    if(arrayContains(argument,["result"])) sql += ` AND re.positionText='${result}'`;
    if(arrayContains(argument,["fastest"])) sql += ` AND re.rank='${fastest}'`;
    if(arrayContains(argument,["status"])) sql += ` AND re.statusId='${status}'`;
    if(arrayContains(argument,["year"])) sql += ` AND ra.year='${year}'`;
    if(arrayContains(argument,["round"])) sql += ` AND ra.round='$round'`;
    if(arrayContains(argument,["constructor"])) sql += `AND re.constructorId=constructors.constructorId AND constructors.constructorRef='${constructor}'`;

    
    sql += ` ORDER BY ra.year, ra.round LIMIT ${offset}, ${limit}`;
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
                "RaceTable": {}
            }
        };
        if(circuit!=null)
            json.MRData.RaceTable.circuitId = circuit;
        if(driver!=null)
            json.MRData.RaceTable.driverId = driver;
        if(constructor!=null)
            json.MRData.RaceTable.constructor = constructor;
        if(grid!=null)
            json.MRData.RaceTable.grid = grid;
        if(result!=null)
            json.MRData.RaceTable.result = result;
        if(fastest!=null)
            json.MRData.RaceTable.fastest = fastest;
        if(status!=null)
            json.MRData.RaceTable.status = status;
        if(year!=null)
            json.MRData.RaceTable.season = year;
        if(round!=null)
            json.MRData.RaceTable.round = round;
        
        json.MRData.RaceTable.Races = formattedRaces(rows);

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