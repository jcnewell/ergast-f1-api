const express = require("express");
const router = express.Router();
const path = require("path");

let MySQLCofiguration = require("../connection.js");

//Supported Function
function formattedRaces(rows)
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
        time : rows[0].raceTime + "Z",
        Laps : formattedLaps(rows)
    };
}

function formattedLaps(rows)
{
    let Laps =[];
    let currentLap = 0;
    rows.forEach(element => {
        if(element.lap != currentLap)
        {
            let t = {
                number : element.lap.toString(),
                Timings : formattedTiming(rows, element.lap)
            }
            Laps.push(t);
            currentLap = element.lap;
        }
    });

    return Laps;

}

function formattedTiming(rows,lap)
{
    let timing = [];

    rows.forEach(element => {
        if(element.lap == lap)
        {
            let t = {
                driverId : element.driverRef,
                position : element.position.toString(),
                time : element.time
            }
            timing.push(t);
        }
    });
    return timing;
}

//API 2.0

// /laps/2019/2/driver/hamilton
router.get("/:year([0-9]{4}|current)/:round([0-9]{1,2})/driver/:driver", (req,res) => {
    let offset = (typeof req.query.offset != 'undefined') ? parseInt(req.query.offset) : 0;
    let limit = (typeof req.query.limit != 'undefined') ? parseInt(req.query.limit) : 30;

    let sql =   `SELECT
                ra.year, ra.round, ra.name, DATE_FORMAT(ra.date, '%Y-%m-%d') AS 'date', DATE_FORMAT(ra.time, '%H:%i:%S') AS 'raceTime', ra.url, 
                ci.circuitRef, ci.name AS 'circuitName', ci.location, ci.country, ci.url AS 'circuitUrl', ci.lat, ci.lng, ci.alt,
                dr.driverRef,
                la.lap, la.position, la.time
                FROM lapTimes la, races ra, circuits ci, drivers dr
                WHERE ra.circuitId=ci.circuitId AND la.driverId=dr.driverId AND la.raceId=ra.raceId 
                AND ra.year = ? AND ra.round = ? AND dr.driverRef = ?
                ORDER BY la.lap, la.position
                LIMIT ${offset}, ${limit};`;
    const conn = MySQLCofiguration.getMySQLConnection();
    let year = (req.params.year == "current") ? new Date().getFullYear().toString() : req.params.year;
    conn.query(sql,[year,req.params.round, req.params.driver],(err,rows,fields) => {
        
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
                    "driverId" : req.params.driver,
                    "Races" : [formattedRaces(rows)]
                }
            }
        });
    });
});

// /laps/2019/2/lap/1/driver/hamilton
router.get("/:year([0-9]{4}|current)/:round([0-9]{1,2})/lap/:lap([0-9]{1,2})/driver/:driver", (req,res) => {
    let offset = (typeof req.query.offset != 'undefined') ? parseInt(req.query.offset) : 0;
    let limit = (typeof req.query.limit != 'undefined') ? parseInt(req.query.limit) : 30;

    let sql =   `SELECT
                ra.year, ra.round, ra.name, DATE_FORMAT(ra.date, '%Y-%m-%d') AS 'date', DATE_FORMAT(ra.time, '%H:%i:%S') AS 'raceTime', ra.url, 
                ci.circuitRef, ci.name AS 'circuitName', ci.location, ci.country, ci.url AS 'circuitUrl', ci.lat, ci.lng, ci.alt,
                dr.driverRef,
                la.lap, la.position, la.time
                FROM lapTimes la, races ra, circuits ci, drivers dr
                WHERE ra.circuitId=ci.circuitId AND la.driverId=dr.driverId AND la.raceId=ra.raceId 
                AND ra.year = ? AND ra.round = ? AND la.lap = ? AND dr.driverRef = ?
                ORDER BY la.lap, la.position
                LIMIT ${offset}, ${limit};`;
    const conn = MySQLCofiguration.getMySQLConnection();
    let year = (req.params.year == "current") ? new Date().getFullYear().toString() : req.params.year;
    conn.query(sql,[year,req.params.round,req.params.lap, req.params.driver],(err,rows,fields) => {
        
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
                    "driverId" : req.params.driver,
                    "Races" : [formattedRaces(rows)]
                }
            }
        });
    });
});

// /laps/2019/2/lap/1
router.get("/:year([0-9]{4}|current)/:round([0-9]{1,2})/lap/:lap([0-9]{1,2})", (req,res) => {
    let offset = (typeof req.query.offset != 'undefined') ? parseInt(req.query.offset) : 0;
    let limit = (typeof req.query.limit != 'undefined') ? parseInt(req.query.limit) : 30;

    let sql =   `SELECT
                ra.year, ra.round, ra.name, DATE_FORMAT(ra.date, '%Y-%m-%d') AS 'date', DATE_FORMAT(ra.time, '%H:%i:%S') AS 'raceTime', ra.url, 
                ci.circuitRef, ci.name AS 'circuitName', ci.location, ci.country, ci.url AS 'circuitUrl', ci.lat, ci.lng, ci.alt,
                dr.driverRef,
                la.lap, la.position, la.time
                FROM lapTimes la, races ra, circuits ci, drivers dr
                WHERE ra.circuitId=ci.circuitId AND la.driverId=dr.driverId AND la.raceId=ra.raceId 
                AND ra.year = ? AND ra.round = ? AND la.lap = ?
                ORDER BY la.lap, la.position
                LIMIT ${offset}, ${limit};`;
    const conn = MySQLCofiguration.getMySQLConnection();
    let year = (req.params.year == "current") ? new Date().getFullYear().toString() : req.params.year;
    conn.query(sql,[year,req.params.round,req.params.lap],(err,rows,fields) => {
        
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
                    "Races" : [formattedRaces(rows)]
                }
            }
        });
    });
});

// /laps/2019/2
router.get("/:year([0-9]{4}|current)/:round([0-9]{1,2})", (req,res) => {
    let offset = (typeof req.query.offset != 'undefined') ? parseInt(req.query.offset) : 0;
    let limit = (typeof req.query.limit != 'undefined') ? parseInt(req.query.limit) : 30;

    let sql =   `SELECT
                ra.year, ra.round, ra.name, DATE_FORMAT(ra.date, '%Y-%m-%d') AS 'date', DATE_FORMAT(ra.time, '%H:%i:%S') AS 'raceTime', ra.url, 
                ci.circuitRef, ci.name AS 'circuitName', ci.location, ci.country, ci.url AS 'circuitUrl', ci.lat, ci.lng, ci.alt,
                dr.driverRef,
                la.lap, la.position, la.time
                FROM lapTimes la, races ra, circuits ci, drivers dr
                WHERE ra.circuitId=ci.circuitId AND la.driverId=dr.driverId AND la.raceId=ra.raceId 
                AND ra.year = ? AND ra.round = ?
                ORDER BY la.lap, la.position
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
                    "Races" : [formattedRaces(rows)]
                }
            }
        });
    });
});

router.get("/help", (req,res) => {
    res.status(200).sendFile(path.join( __dirname,"../public","laps.html"));
});

module.exports = router;