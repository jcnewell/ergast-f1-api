const express = require("express");
const router = express.Router();
const path = require("path");

let MySQLCofiguration = require("../connection.js");

//Supported Function
function formattedRaces(rows)
{
    if(rows.length > 0)
    {
        return { 
            season : rows[0].year.toString(),
            round : rows[0].round.toString(),
            url: rows[0].url,
            raceName : rows[0].name,
            Circuit : {
                circuitId : rows[0].circuitRef,
                url : rows[0].circuitUrl,
                circuitName : rows[0].circuitName,
                Location : {
                    lat : rows[0].lat.toString(),
                    long : rows[0].lng.toString(),
                    alt :  (rows[0].alt != null) ? rows[0].alt.toString() : "N/D",
                    locality : rows[0].location,
                    country : rows[0].country
                }
            },
            date : rows[0].date,
            time : rows[0].time + "Z",
            Pitstops : formattedPitStops(rows)
        };
    }
    else
        return {};
}

function formattedPitStops(row)
{
    const pitstops = row.map((row) => {
        return { 
            driverId: row.driverRef, 
            lap: row.lap.toString(), 
            stop : row.stop.toString(), 
            time : row.time, 
            duration : row.duration 
        };
    });
    return pitstops;
}

//API 2.0

// /pitstops/2019/2
router.get("/:year([0-9]{4}|current)/:round([0-9]{1,2})", (req,res) => {
    let offset = (typeof req.query.offset != 'undefined') ? parseInt(req.query.offset) : 0;
    let limit = (typeof req.query.limit != 'undefined') ? parseInt(req.query.limit) : 30;

    let sql =   `SELECT
                ra.year, ra.round, ra.name, DATE_FORMAT(ra.date, '%Y-%m-%d') AS 'date', DATE_FORMAT(ra.time, '%H:%i:%S') AS 'time', ra.url, 
                ci.circuitRef, ci.name AS 'circuitName', ci.location, ci.country, ci.url AS 'circuitUrl', ci.lat, ci.lng, ci.alt,
                dr.driverRef,
                pi.stop, pi.lap, pi.time, pi.duration
                FROM pitStops pi, races ra, circuits ci, drivers dr
                WHERE ra.circuitId=ci.circuitId AND pi.driverId=dr.driverId AND pi.raceId=ra.raceId 
                AND ra.year = ? AND ra.round = ?
                ORDER BY (pi.time)
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
                    "season": year,
                    "round": req.params.round,
                    "Races" : formattedRaces(rows)
                }
            }
        });
    });
});

// /pitstops/2019/2/pitstop/1
router.get("/:year([0-9]{4}|current)/:round([0-9]{1,2})/pitstop/:stop([0-9]{1,2})", (req,res) => {
    let offset = (typeof req.query.offset != 'undefined') ? parseInt(req.query.offset) : 0;
    let limit = (typeof req.query.limit != 'undefined') ? parseInt(req.query.limit) : 30;

    let sql =   `SELECT
                ra.year, ra.round, ra.name, DATE_FORMAT(ra.date, '%Y-%m-%d') AS 'date', DATE_FORMAT(ra.time, '%H:%i:%S') AS 'time', ra.url, 
                ci.circuitRef, ci.name AS 'circuitName', ci.location, ci.country, ci.url AS 'circuitUrl', ci.lat, ci.lng, ci.alt,
                dr.driverRef,
                pi.stop, pi.lap, pi.time, pi.duration
                FROM pitStops pi, races ra, circuits ci, drivers dr
                WHERE ra.circuitId=ci.circuitId AND pi.driverId=dr.driverId AND pi.raceId=ra.raceId 
                AND ra.year = ? AND ra.round = ? AND pi.stop = ?
                ORDER BY (pi.time)
                LIMIT ${offset}, ${limit};`;
    const conn = MySQLCofiguration.getMySQLConnection();
    let year = (req.params.year == "current") ? new Date().getFullYear().toString() : req.params.year;
    conn.query(sql,[year,req.params.round,req.params.stop],(err,rows,fields) => {
        
        if(err){
            console.log("Failed to query for " + __filename.slice(__filename.lastIndexOf(path.sep)+1) + ": "+ err);
            res.status(500);
            res.end();
            return;
        }
        //res.json(rows);
        res.json({
            "MRData":{
                "limit":limit.toString(),
                "offset":offset.toString(),
                RaceTable : {
                    "season": year,
                    "round": req.params.round,
                    "stop": req.params.stop,
                    "Races" : formattedRaces(rows)
                }
            }
        });
    });
});

// /pitstops/2019/2/pitstop/1/driver/stroll
router.get("/:year([0-9]{4}|current)/:round([0-9]{1,2})/pitstop/:stop([0-9]{1,2})/driver/:name", (req,res) => {
    let offset = (typeof req.query.offset != 'undefined') ? parseInt(req.query.offset) : 0;
    let limit = (typeof req.query.limit != 'undefined') ? parseInt(req.query.limit) : 30;

    let sql =   `SELECT
                ra.year, ra.round, ra.name, DATE_FORMAT(ra.date, '%Y-%m-%d') AS 'date', DATE_FORMAT(ra.time, '%H:%i:%S') AS 'time', ra.url, 
                ci.circuitRef, ci.name AS 'circuitName', ci.location, ci.country, ci.url AS 'circuitUrl', ci.lat, ci.lng, ci.alt,
                dr.driverRef,
                pi.stop, pi.lap, pi.time, pi.duration
                FROM pitStops pi, races ra, circuits ci, drivers dr
                WHERE ra.circuitId=ci.circuitId AND pi.driverId=dr.driverId AND pi.raceId=ra.raceId 
                AND ra.year = ? AND ra.round = ? AND pi.stop = ? AND dr.driverRef = ?
                ORDER BY (pi.time)
                LIMIT ${offset}, ${limit};`;
    const conn = MySQLCofiguration.getMySQLConnection();
    let year = (req.params.year == "current") ? new Date().getFullYear().toString() : req.params.year;
    conn.query(sql,[year,req.params.round,req.params.stop,req.params.name],(err,rows,fields) => {
        
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
                    "season": year,
                    "round": req.params.round,
                    "stop": req.params.stop,
                    "driverId" : req.params.name,
                    "Races" : formattedRaces(rows)
                }
            }
        });
    });
});

// /pitstops/2019/2/driver/vettel
router.get("/:year([0-9]{4}|current)/:round([0-9]{1,2})/driver/:name", (req,res) => {
    let offset = (typeof req.query.offset != 'undefined') ? parseInt(req.query.offset) : 0;
    let limit = (typeof req.query.limit != 'undefined') ? parseInt(req.query.limit) : 30;

    let sql =   `SELECT
                ra.year, ra.round, ra.name, DATE_FORMAT(ra.date, '%Y-%m-%d') AS 'date', DATE_FORMAT(ra.time, '%H:%i:%S') AS 'time', ra.url, 
                ci.circuitRef, ci.name AS 'circuitName', ci.location, ci.country, ci.url AS 'circuitUrl', ci.lat, ci.lng, ci.alt,
                dr.driverRef,
                pi.stop, pi.lap, pi.time, pi.duration
                FROM pitStops pi, races ra, circuits ci, drivers dr
                WHERE ra.circuitId=ci.circuitId AND pi.driverId=dr.driverId AND pi.raceId=ra.raceId 
                AND ra.year = ? AND ra.round = ? AND dr.driverRef = ?
                ORDER BY (pi.time)
                LIMIT ${offset}, ${limit};`;
    const conn = MySQLCofiguration.getMySQLConnection();
    let year = (req.params.year == "current") ? new Date().getFullYear().toString() : req.params.year;
    conn.query(sql,[year,req.params.round,req.params.name],(err,rows,fields) => {
        
        if(err){
            console.log("Failed to query for " + __filename.slice(__filename.lastIndexOf(path.sep)+1) + ": "+ err);
            res.status(500);
            res.end();
            return;
        }
        //res.json(rows);
        res.json({
            "MRData":{
                "limit":limit.toString(),
                "offset":offset.toString(),
                RaceTable : {
                    "season": year,
                    "round": req.params.round,
                    "driverId": req.params.name,
                    "Races" : formattedRaces(rows)
                }
            }
        });
    });
});

// /pitstops/2019/2/lap/1
router.get("/:year([0-9]{4}|current)/:round([0-9]{1,2})/lap/:numberLap([0-9]{1,2})", (req,res) => {
    let offset = (typeof req.query.offset != 'undefined') ? parseInt(req.query.offset) : 0;
    let limit = (typeof req.query.limit != 'undefined') ? parseInt(req.query.limit) : 30;

    let sql =   `SELECT
                ra.year, ra.round, ra.name, DATE_FORMAT(ra.date, '%Y-%m-%d') AS 'date', DATE_FORMAT(ra.time, '%H:%i:%S') AS 'time', ra.url, 
                ci.circuitRef, ci.name AS 'circuitName', ci.location, ci.country, ci.url AS 'circuitUrl', ci.lat, ci.lng, ci.alt,
                dr.driverRef,
                pi.stop, pi.lap, pi.time, pi.duration
                FROM pitStops pi, races ra, circuits ci, drivers dr
                WHERE ra.circuitId=ci.circuitId AND pi.driverId=dr.driverId AND pi.raceId=ra.raceId 
                AND ra.year = ? AND ra.round = ? AND pi.lap = ?
                ORDER BY (pi.time)
                LIMIT ${offset}, ${limit};`;
    const conn = MySQLCofiguration.getMySQLConnection();
    let year = (req.params.year == "current") ? new Date().getFullYear().toString() : req.params.year;
    conn.query(sql,[year,req.params.round,req.params.numberLap],(err,rows,fields) => {
        
        if(err){
            console.log("Failed to query for " + __filename.slice(__filename.lastIndexOf(path.sep)+1) + ": "+ err);
            res.status(500);
            res.end();
            return;
        }
        //res.json(rows);
        res.json({
            "MRData":{
                "limit":limit.toString(),
                "offset":offset.toString(),
                RaceTable : {
                    "season": year,
                    "round": req.params.round,
                    "lap": req.params.numberLap,
                    "Races" : formattedRaces(rows)
                }
            }
        });
    });
});

// /pitstops/2019/2/lap/1/driver/stroll
router.get("/:year([0-9]{4}|current)/:round([0-9]{1,2})/lap/:numberLap([0-9]{1,2})/driver/:name", (req,res) => {
    let offset = (typeof req.query.offset != 'undefined') ? parseInt(req.query.offset) : 0;
    let limit = (typeof req.query.limit != 'undefined') ? parseInt(req.query.limit) : 30;

    let sql =   `SELECT
                ra.year, ra.round, ra.name, DATE_FORMAT(ra.date, '%Y-%m-%d') AS 'date', DATE_FORMAT(ra.time, '%H:%i:%S') AS 'time', ra.url, 
                ci.circuitRef, ci.name AS 'circuitName', ci.location, ci.country, ci.url AS 'circuitUrl', ci.lat, ci.lng, ci.alt,
                dr.driverRef,
                pi.stop, pi.lap, pi.time, pi.duration
                FROM pitStops pi, races ra, circuits ci, drivers dr
                WHERE ra.circuitId=ci.circuitId AND pi.driverId=dr.driverId AND pi.raceId=ra.raceId 
                AND ra.year = ? AND ra.round = ? AND pi.lap = ? AND dr.driverRef = ?
                ORDER BY (pi.time)
                LIMIT ${offset}, ${limit};`;
    const conn = MySQLCofiguration.getMySQLConnection();
    let year = (req.params.year == "current") ? new Date().getFullYear().toString() : req.params.year;
    conn.query(sql,[year,req.params.round,req.params.numberLap,req.params.name],(err,rows,fields) => {
        
        if(err){
            console.log("Failed to query for " + __filename.slice(__filename.lastIndexOf(path.sep)+1) + ": "+ err);
            res.status(500);
            res.end();
            return;
        }
        //res.json(rows);
        res.json({
            "MRData":{
                "limit":limit.toString(),
                "offset":offset.toString(),
                RaceTable : {
                    "season": year,
                    "round": req.params.round,
                    "lap": req.params.numberLap,
                    "driverId" : req.params.name,
                    "Races" : formattedRaces(rows)
                }
            }
        });
    });
});

router.get("/help", (req,res) => {
    res.status(200).sendFile(path.join( __dirname,"../public","pitstop.html"));
});

module.exports = router;