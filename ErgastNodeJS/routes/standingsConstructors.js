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
            Constructor : {
                constructorId : row.constructorRef,
                url : row.url,
                name : row.name,
                nationality : row.nationality
            }
        };
    });
    return position;
}

function formattedStandingsLists(rows)
{
    const winnerThisYear = rows.map((row) => {
        return { 
            season : row.year.toString(),
            round : row.round.toString(),
            ConstructorStandings : [{
                position : row.position.toString(),
                positionText : row.positionText,
                points : row.points.toString(),
                wins : row.wins.toString(),
                Constructor : {
                    constructorId : row.constructorRef,
                    url : row.url,
                    name : row.name,
                    nationality : row.nationality
                }
            }]
        };
    });
    return winnerThisYear;
}

//API 2.0
router.get("/help", (req,res) => {
    res.status(200).sendFile(path.join( __dirname,"../public","standingsConstructors.html"));
});

// /standings/constructors/2019
router.get("/:year([0-9]{4}|current)", (req,res) => {
    let offset = (typeof req.query.offset != 'undefined') ? parseInt(req.query.offset) : 0;
    let limit = (typeof req.query.limit != 'undefined') ? parseInt(req.query.limit) : 30;

    let sql =   `SELECT DISTINCT c.constructorRef, c.name, c.nationality, c.url, cs.points, cs.position, cs.positionText, cs.wins, r.year, r.round
                FROM constructors AS c, constructorStandings AS cs, races AS r
                WHERE cs.raceId=r.raceId AND cs.constructorId=c.constructorId
                AND r.year = ? AND r.round=(SELECT MAX(round) FROM driverStandings ds, races r WHERE ds.raceId=r.raceId AND r.year = ? )
                ORDER BY r.year, cs.position
                LIMIT ${offset}, ${limit};`;
    const conn = MySQLCofiguration.getMySQLConnection();
    let year = (req.params.year == "current") ? new Date().getFullYear().toString() : req.params.year;
    conn.query(sql,[year,year],(err,rows,fields) => {
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
                "ConstructorStandings" : formattedConstructorStandings(rows)
            }];
        }
        res.json(json);
        
    });
});

// /standings/constructors/2019/constructor/ferrari
router.get("/:year([0-9]{4}|current)/constructor/:constructor", (req,res) => {
    let offset = (typeof req.query.offset != 'undefined') ? parseInt(req.query.offset) : 0;
    let limit = (typeof req.query.limit != 'undefined') ? parseInt(req.query.limit) : 30;

    let sql =   `SELECT DISTINCT c.constructorRef, c.name, c.nationality, c.url, cs.points, cs.position, cs.positionText, cs.wins, r.year, r.round
                FROM constructors AS c, constructorStandings AS cs, races AS r
                WHERE cs.raceId=r.raceId AND cs.constructorId=c.constructorId
                AND r.year = ? AND r.round=(SELECT MAX(round) FROM driverStandings ds, races r WHERE ds.raceId=r.raceId AND r.year = ? )
                AND c.constructorRef = ?
                ORDER BY r.year, cs.position
                LIMIT ${offset}, ${limit};`;
    const conn = MySQLCofiguration.getMySQLConnection();
    let year = (req.params.year == "current") ? new Date().getFullYear().toString() : req.params.year;
    conn.query(sql,[year,year,req.params.constructor],(err,rows,fields) => {
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
                    "constructorId" : req.params.constructor,
                    "StandingsLists" : []
                }
            }
        };

        if(rows.length > 0)
        {
            json.MRData.StandingsTable.StandingsLists = [{
                "season" : year,
                "round" : rows[0].round.toString(),
                "ConstructorStandings" : formattedConstructorStandings(rows)
            }];
        }
        res.json(json);
        
    });
});

// /standings/constructors/2018/2
router.get("/:year([0-9]{4}|current)/:round([0-9]{1,2})", (req,res) => {
    let offset = (typeof req.query.offset != 'undefined') ? parseInt(req.query.offset) : 0;
    let limit = (typeof req.query.limit != 'undefined') ? parseInt(req.query.limit) : 30;

    let sql =   `SELECT DISTINCT c.constructorRef, c.name, c.nationality, c.url, cs.points, cs.position, cs.positionText, cs.wins, r.year, r.round
                FROM constructors AS c, constructorStandings AS cs, races AS r
                WHERE cs.raceId=r.raceId AND cs.constructorId=c.constructorId
                AND r.year = ? AND r.round = ?
                ORDER BY r.year, cs.position
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
                "round" : req.params.round,
                "ConstructorStandings" : formattedConstructorStandings(rows)
            }];
        }
        res.json(json);
        
    });
});

// /standings/constructors/2018/2/constructor/ferrari
router.get("/:year([0-9]{4}|current)/:round([0-9]{1,2})/constructor/:constructor", (req,res) => {
    let offset = (typeof req.query.offset != 'undefined') ? parseInt(req.query.offset) : 0;
    let limit = (typeof req.query.limit != 'undefined') ? parseInt(req.query.limit) : 30;

    let sql =   `SELECT DISTINCT c.constructorRef, c.name, c.nationality, c.url, cs.points, cs.position, cs.positionText, cs.wins, r.year, r.round
                FROM constructors AS c, constructorStandings AS cs, races AS r
                WHERE cs.raceId=r.raceId AND cs.constructorId=c.constructorId
                AND r.year = ? AND r.round = ? AND c.constructorRef = ?
                ORDER BY r.year, cs.position
                LIMIT ${offset}, ${limit};`;
    const conn = MySQLCofiguration.getMySQLConnection();
    let year = (req.params.year == "current") ? new Date().getFullYear().toString() : req.params.year;
    conn.query(sql,[year,req.params.round,req.params.constructor],(err,rows,fields) => {
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
                    "constructorId" : req.params.constructor,
                    "StandingsLists" : []
                }
            }
        };

        if(rows.length > 0)
        {
            json.MRData.StandingsTable.StandingsLists = [{
                "season" : year,
                "round" : req.params.round,
                "ConstructorStandings" : formattedConstructorStandings(rows)
            }];
        }
        res.json(json);
        
    });
});

// /standings/constructors/position/1
router.get("/position/:position([0-9]{1,2})", (req,res) => {
    let offset = (typeof req.query.offset != 'undefined') ? parseInt(req.query.offset) : 0;
    let limit = (typeof req.query.limit != 'undefined') ? parseInt(req.query.limit) : 30;

    let sql =   `SELECT DISTINCT c.constructorRef,c.name, c.nationality,c.url,cs.points,cs.position,cs.positionText,cs.wins,r.year,r.round
                FROM constructors c, constructorStandings cs, races r
                WHERE cs.raceId = r.raceId AND cs.constructorId = c.constructorId AND (r.year , r.round) IN (SELECT  year, MAX(round)  FROM races GROUP BY year)
                AND cs.positionText = ?
                ORDER BY r.year , cs.position
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
                    "constructorStandings" : req.params.position,
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

// /standings/constructors/position/1/constructor/ferrari
router.get("/position/:position([0-9]{1,2})/constructor/:constructor", (req,res) => {
    let offset = (typeof req.query.offset != 'undefined') ? parseInt(req.query.offset) : 0;
    let limit = (typeof req.query.limit != 'undefined') ? parseInt(req.query.limit) : 30;

    let sql =   `SELECT DISTINCT c.constructorRef,c.name, c.nationality,c.url,cs.points,cs.position,cs.positionText,cs.wins,r.year,r.round
                FROM constructors c, constructorStandings cs, races r
                WHERE cs.raceId = r.raceId AND cs.constructorId = c.constructorId AND (r.year , r.round) IN (SELECT  year, MAX(round)  FROM races GROUP BY year)
                AND cs.positionText = ? AND c.constructorRef = ?
                ORDER BY r.year , cs.position
                LIMIT ${offset}, ${limit};`;
    const conn = MySQLCofiguration.getMySQLConnection();
    conn.query(sql,[req.params.position,req.params.constructor],(err,rows,fields) => {
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
                    "constructorId" : req.params.constructor,
                    "constructorStandings" : req.params.position,
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

// /standings/constructors/ferrari
router.get("/:constructor", (req,res) => {
    let offset = (typeof req.query.offset != 'undefined') ? parseInt(req.query.offset) : 0;
    let limit = (typeof req.query.limit != 'undefined') ? parseInt(req.query.limit) : 30;

    let sql =   `SELECT DISTINCT c.constructorRef, c.name, c.nationality, c.url, cs.points, cs.position, cs.positionText, cs.wins, r.year, r.round 
                FROM constructors c, constructorStandings cs, races r 
                WHERE cs.raceId=r.raceId AND cs.constructorId=c.constructorId AND (r.year, r.round) IN (SELECT year, MAX(round) FROM races GROUP BY year) 
                AND c.constructorRef = ?
                ORDER BY r.year, cs.position 
                LIMIT ${offset}, ${limit};`;
    const conn = MySQLCofiguration.getMySQLConnection();
    conn.query(sql,[req.params.constructor],(err,rows,fields) => {
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
                    "constructorId" : req.params.constructor,
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

module.exports = router;