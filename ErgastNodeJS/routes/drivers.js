const express = require("express");
const router = express.Router();
const path = require("path");

let MySQLCofiguration = require("../connection.js");

//Supported Function
function formattedDriver(row)
{
    const driver = row.map((row) => {
        return { 
            driverId: row.driverRef, 
            url: row.url, 
            givenName : row.forename, 
            familyName : row.surname, 
            dateOfBirth : row.date, 
            nationality : row.nationality 
        };
    });
    return driver;
}

//API 2.0
router.get("/help", (req,res) => {
    res.status(200).sendFile(path.join( __dirname,"../public","drivers.html"));
});

// /drivers
router.get("/all", (req,res) => {
    let offset = (typeof req.query.offset != 'undefined') ? parseInt(req.query.offset) : 0;
    let limit = (typeof req.query.limit != 'undefined') ? parseInt(req.query.limit) : 30;

    let sql = `SELECT *,DATE_FORMAT(dob, '%Y-%m-%d') AS 'date' FROM drivers ORDER BY surname LIMIT ${offset}, ${limit}`;
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
                DriverTable : {
                    "Drivers" : formattedDriver(rows)
                }
            }
        });
    });
});

// /drivers/2019
router.get("/:year([0-9]{4}|current)", (req,res) => {
    let offset = (typeof req.query.offset != 'undefined') ? parseInt(req.query.offset) : 0;
    let limit = (typeof req.query.limit != 'undefined') ? parseInt(req.query.limit) : 30;

    let sql =   `SELECT DISTINCT drivers.*, DATE_FORMAT(drivers.dob, '%Y-%m-%d') AS 'date'
                FROM drivers,results,races
                WHERE drivers.driverId = results.driverId AND results.raceId = races.raceId 
                AND races.year = ?
                ORDER BY drivers.surname 
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
        console.log(rows);
        res.json({
            "MRData":{
                "limit":limit.toString(),
                "offset":offset.toString(),
                DriverTable : {
                    "season" : year,
                    "Drivers" : formattedDriver(rows)
                }
            }
        });
    });
});

// /drivers/2019/2
router.get("/:year([0-9]{4}|current)/:round([0-9]{1,2})", (req,res) => {
    let offset = (typeof req.query.offset != 'undefined') ? parseInt(req.query.offset) : 0;
    let limit = (typeof req.query.limit != 'undefined') ? parseInt(req.query.limit) : 30;

    let sql =   `SELECT DISTINCT drivers.*, DATE_FORMAT(drivers.dob, '%Y-%m-%d') AS 'date'
                FROM drivers,results,races
                WHERE drivers.driverId = results.driverId AND results.raceId = races.raceId 
                AND races.year = ? AND races.round = ?
                ORDER BY drivers.surname 
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
                DriverTable : {
                    "season" : year,
                    "round" : req.params.round,
                    "Drivers" : formattedDriver(rows)
                }
            }
        });
    });
});

// /drivers/vettel
router.get("/:name", (req,res) => {
    let offset = (typeof req.query.offset != 'undefined') ? parseInt(req.query.offset) : 0;
    let limit = (typeof req.query.limit != 'undefined') ? parseInt(req.query.limit) : 30;

    let sql = `SELECT * FROM drivers WHERE driverRef = ? LIMIT ${offset}, ${limit}`;
    const conn = MySQLCofiguration.getMySQLConnection();
    conn.query(sql,[req.params.name],(err,rows,fields) => {
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
                DriverTable : {
                    "Drivers" : formattedDriver(rows)
                }
            }
        });
    });
});

module.exports = router;

/*
http://ergast.com/api/f1/drivers DONE
http://ergast.com/api/f1/2010/drivers DONE
http://ergast.com/api/f1/2010/2/drivers DONE
http://ergast.com/api/f1/drivers/alonso DONE

Driver lists can be refined by adding one or more of the following criteria:
/circuits/<circuitId>
/constructors/<constructorId>
/drivers/<driverId>
/grid/<position>
/results/<position>
/fastest/<rank>
/status/<statusId>
*/