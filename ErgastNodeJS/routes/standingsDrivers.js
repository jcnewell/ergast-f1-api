const express = require("express");
const router = express.Router();
const path = require("path");

let MySQLCofiguration = require("../connection.js");

//Supported Function
function formattedConstructorStandings(rows)
{
    const position = rows.map((row) => {
        return { 
            position : row.position.toString(),
            positionText : row.positionText,
            points : row.points.toString(),
            wins : row.wins.toString(),
            Driver : {
                driverId : row.driverRef,
                permanentNumber : (row.number != null) ? row.number.toString() : "",
                code : (row.code != null) ? row.code : "",
                url : row.url,
                givenName : row.forename,
                familyName : row.surname,
                dateOfBirth : row.dob,
                nationality : row.nationality
            }
        };
    });
    return position;
};

function formattedStandingsLists(rows)
{
    const position = rows.map((row) => {
        return { 
            season : row.year.toString(),
            round : row.round.toString(),
            DriverStandings : [
                { 
                    position : row.position.toString(),
                    positionText : row.positionText,
                    points : row.points.toString(),
                    wins : row.wins.toString(),
                    Driver : {
                        driverId : row.driverRef,
                        permanentNumber : (row.number != null) ? row.number.toString() : "",
                        code : (row.code != null) ? row.code : "",
                        url : row.url,
                        givenName : row.forename,
                        familyName : row.surname,
                        dateOfBirth : row.dob,
                        nationality : row.nationality
                    }
                }
            ]
        }
    });
    return position;
}

//API 2.0
router.get("/help", (req,res) => {
    res.status(200).sendFile(path.join( __dirname,"../public","standingsDrivers.html"));
});

// /standings/drivers/2019
router.get("/:year([0-9]{4}|current)", (req,res) => {
    let offset = (typeof req.query.offset != 'undefined') ? parseInt(req.query.offset) : 0;
    let limit = (typeof req.query.limit != 'undefined') ? parseInt(req.query.limit) : 30;

    let sql =   `SELECT d.driverId, d.driverRef, d.number, d.code, d.forename, d.surname,DATE_FORMAT(d.dob, '%Y-%m-%d') AS 'dob', d.nationality, d.url, 
                ds.points,ds.position,ds.positionText,ds.wins, r.year,r.round
                FROM driverStandings AS ds ,drivers AS d,races AS r
                WHERE ds.raceId =  (SELECT races.raceId 
                                    FROM races
                                    WHERE races.year = ? AND races.round=  (SELECT MAX(round) 
                                                                            FROM driverStandings ds, races r 
                                                                            WHERE ds.raceId=r.raceId AND r.year= ?))
                AND d.driverId = ds.driverId AND r.raceId = ds.raceId
                ORDER BY ds.position
                LIMIT ${offset}, ${limit};`;
    const conn = MySQLCofiguration.getMySQLConnection();
    let year = (req.params.year == "current") ? new Date().getFullYear().toString() : req.params.year;
    conn.query(sql,[year,req.params.year],(err,rows,fields) => {
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
                StandingsTable : {
                    "season" : year,
                    "StandingsLists" : []
                }
            }
        };
        
        if(rows.length > 0)
        {
            json.MRData.StandingsTable.StandingsLists = [{
                "season" : year,
                "round" : rows[0].round.toString(),
                "DriverStandings" : formattedConstructorStandings(rows)
            }];
        }
        res.json(json);
        
    });
});

// /standings/drivers/2019/driver/leclerc
router.get("/:year([0-9]{4}|current)/driver/:driver", (req,res) => {
    let offset = (typeof req.query.offset != 'undefined') ? parseInt(req.query.offset) : 0;
    let limit = (typeof req.query.limit != 'undefined') ? parseInt(req.query.limit) : 30;

    let sql =   `SELECT d.driverId, d.driverRef, d.number, d.code, d.forename, d.surname,DATE_FORMAT(d.dob, '%Y-%m-%d') AS 'dob', d.nationality, d.url, 
                ds.points,ds.position,ds.positionText,ds.wins, r.year,r.round
                FROM driverStandings AS ds ,drivers AS d,races AS r
                WHERE ds.raceId =  (SELECT races.raceId 
                                    FROM races
                                    WHERE races.year = ? AND races.round=  (SELECT MAX(round) 
                                                                            FROM driverStandings ds, races r 
                                                                            WHERE ds.raceId=r.raceId AND r.year= ?))
                AND d.driverId = ds.driverId AND r.raceId = ds.raceId 
                AND d.driverRef = ?
                ORDER BY ds.position
                LIMIT ${offset}, ${limit};`;
    const conn = MySQLCofiguration.getMySQLConnection();
    let year = (req.params.year == "current") ? new Date().getFullYear().toString() : req.params.year;
    conn.query(sql,[year,year,req.params.driver],(err,rows,fields) => {
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
                StandingsTable : {
                    "season" : year,
                    "driverId" : req.params.driver,
                    "StandingsLists" : []
                }
            }
        };
        
        if(rows.length > 0)
        {
            json.MRData.StandingsTable.StandingsLists = [{
                "season" : year,
                "round" : rows[0].round.toString(),
                "DriverStandings" : formattedConstructorStandings(rows)
            }];
        }
        res.json(json);
        
    });
});

// /standings/drivers/2018/2
router.get("/:year([0-9]{4}|current)/:round([0-9]{1,2})", (req,res) => {
    let offset = (typeof req.query.offset != 'undefined') ? parseInt(req.query.offset) : 0;
    let limit = (typeof req.query.limit != 'undefined') ? parseInt(req.query.limit) : 30;

    let sql =   `SELECT d.driverId, d.driverRef, d.number, d.code, d.forename, d.surname,DATE_FORMAT(d.dob, '%Y-%m-%d') AS 'dob', d.nationality, d.url, 
                ds.points,ds.position,ds.positionText,ds.wins, r.year,r.round
                FROM driverStandings AS ds ,drivers AS d,races AS r
                WHERE ds.raceId =  (SELECT races.raceId 
                                    FROM races
                                    WHERE races.year = ? AND races.round= ?)
                AND d.driverId = ds.driverId AND r.raceId = ds.raceId
                ORDER BY ds.position
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

        let json = {
            "MRData":{
                "limit":limit.toString(),
                "offset":offset.toString(),
                StandingsTable : {
                    "season" : year,
                    "round" : req.params.round,
                    "StandingsLists" : []
                }
            }
        };
        
        if(rows.length > 0)
        {
            json.MRData.StandingsTable.StandingsLists = [{
                "season" : year,
                "round" : rows[0].round.toString(),
                "DriverStandings" : formattedConstructorStandings(rows)
            }];
        }
        res.json(json);
        
    });
});

// /standings/drivers/2018/2/driver/leclerc
router.get("/:year([0-9]{4}|current)/:round([0-9]{1,2})/driver/:driver", (req,res) => {
    let offset = (typeof req.query.offset != 'undefined') ? parseInt(req.query.offset) : 0;
    let limit = (typeof req.query.limit != 'undefined') ? parseInt(req.query.limit) : 30;

    let sql =   `SELECT d.driverId, d.driverRef, d.number, d.code, d.forename, d.surname,DATE_FORMAT(d.dob, '%Y-%m-%d') AS 'dob', d.nationality, d.url, 
                ds.points,ds.position,ds.positionText,ds.wins, r.year,r.round
                FROM driverStandings AS ds ,drivers AS d,races AS r
                WHERE ds.raceId =  (SELECT races.raceId 
                                    FROM races
                                    WHERE races.year = ? AND races.round= ?)
                AND d.driverId = ds.driverId AND r.raceId = ds.raceId 
                AND d.driverRef = ?
                ORDER BY ds.position
                LIMIT ${offset}, ${limit};`;
    const conn = MySQLCofiguration.getMySQLConnection();
    let year = (req.params.year == "current") ? new Date().getFullYear().toString() : req.params.year;
    conn.query(sql,[year,req.params.round,req.params.driver],(err,rows,fields) => {
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
                StandingsTable : {
                    "season" : year,
                    "round" : req.params.round,
                    "driverId" : req.params.driver,
                    "StandingsLists" : []
                }
            }
        };
        
        if(rows.length > 0)
        {
            json.MRData.StandingsTable.StandingsLists = [{
                "season" : year,
                "round" : rows[0].round.toString(),
                "DriverStandings" : formattedConstructorStandings(rows)
            }];
        }
        res.json(json);
    });
});

// /standings/drivers/position/1
router.get("/position/:position([0-9]{1,3})", (req,res) => {
    let offset = (typeof req.query.offset != 'undefined') ? parseInt(req.query.offset) : 0;
    let limit = (typeof req.query.limit != 'undefined') ? parseInt(req.query.limit) : 30;

    let sql =   `SELECT d.driverId, d.driverRef, d.number, d.code, d.forename, d.surname,DATE_FORMAT(d.dob, '%Y-%m-%d') AS 'dob', d.nationality, d.url, 
                    ds.points,ds.position,ds.positionText,ds.wins, r.year,r.round
                FROM drivers d, driverStandings ds, races r
                WHERE ds.raceId=r.raceId AND ds.driverId=d.driverId AND ds.positionText = ? AND (r.year, r.round) IN 	(SELECT year, MAX(round) 
                                                                                                                        FROM races 
                                                                                                                        GROUP BY year) 
                ORDER BY r.year, ds.position 
                LIMIT ${offset}, ${limit};`;
    const conn = MySQLCofiguration.getMySQLConnection();
    conn.query(sql,[req.params.position],(err,rows,fields) => {
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
                StandingsTable : {
                    "driverStandings" : req.params.position,
                    "StandingsLists" : []
                }
            }
        };

        if(rows.length > 0)
        {
            json.MRData.StandingsTable.StandingsLists = formattedStandingsLists(rows);
        }
        res.json(json);
    });
});

// /standings/drivers/position/1/driver/vettel
router.get("/position/:position([0-9]{1,3})/driver/:driver", (req,res) => {
    let offset = (typeof req.query.offset != 'undefined') ? parseInt(req.query.offset) : 0;
    let limit = (typeof req.query.limit != 'undefined') ? parseInt(req.query.limit) : 30;

    let sql =   `SELECT d.driverId, d.driverRef, d.number, d.code, d.forename, d.surname,DATE_FORMAT(d.dob, '%Y-%m-%d') AS 'dob', d.nationality, d.url, 
                    ds.points,ds.position,ds.positionText,ds.wins, r.year,r.round
                FROM drivers d, driverStandings ds, races r
                WHERE ds.raceId=r.raceId AND ds.driverId=d.driverId AND ds.positionText = ? AND d.driverRef = ? AND (r.year, r.round) IN 	(SELECT year, MAX(round) 
                                                                                                                                            FROM races 
                                                                                                                                            GROUP BY year) 
                ORDER BY r.year, ds.position 
                LIMIT ${offset}, ${limit};`;
    const conn = MySQLCofiguration.getMySQLConnection();
    conn.query(sql,[req.params.position,req.params.driver],(err,rows,fields) => {
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
                StandingsTable : {
                    "driverId" : req.params.driver,
                    "driverStandings" : req.params.position,
                    "StandingsLists" : []
                }
            }
        };

        if(rows.length > 0)
        {
            json.MRData.StandingsTable.StandingsLists = formattedStandingsLists(rows);
        }
        res.json(json);
    });
});

// /standings/drivers/vettel
router.get("/:driver", (req,res) => {
    let offset = (typeof req.query.offset != 'undefined') ? parseInt(req.query.offset) : 0;
    let limit = (typeof req.query.limit != 'undefined') ? parseInt(req.query.limit) : 30;

    let sql =   `SELECT d.driverId, d.driverRef, d.number, d.code, d.forename, d.surname,DATE_FORMAT(d.dob, '%Y-%m-%d') AS 'dob', d.nationality, d.url, 
                    ds.points,ds.position,ds.positionText,ds.wins, r.year,r.round
                FROM drivers d, driverStandings ds, races r
                WHERE ds.raceId=r.raceId AND ds.driverId=d.driverId AND  d.driverRef = ? AND (r.year, r.round) IN 	(SELECT year, MAX(round) 
                                                                                                                    FROM races 
                                                                                                                    GROUP BY year) 
                ORDER BY r.year, ds.position 
                LIMIT ${offset}, ${limit};`;
    const conn = MySQLCofiguration.getMySQLConnection();
    conn.query(sql,[req.params.driver],(err,rows,fields) => {
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
                StandingsTable : {
                    "driverId" : req.params.driver,
                    "StandingsLists" : []
                }
            }
        };

        if(rows.length > 0)
        {
            json.MRData.StandingsTable.StandingsLists = formattedStandingsLists(rows);
        }
        res.json(json);
    });
})
module.exports = router;