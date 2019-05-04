const express = require("express");
const app = express();
const path = require("path");

const morgan = require("morgan");
app.use(morgan("dev"));
const PORT = 8732;



const routesDrivers = require("./routes/drivers.js");
const routesConsturctors = require("./routes/constructors.js");
const routesPitStop = require("./routes/pitstops.js");
const routesLaps = require("./routes/laps.js");
const routesStandingConstructors = require("./routes/standingsConstructors.js");
const routesStandingDrivers = require("./routes/standingsDrivers.js");
const routesRaceSchedule = require("./routes/raceSchedule.js");

//add Filters
app.use("/drivers", routesDrivers);
app.use("/constructors", routesConsturctors);//DONE
app.use("/pitstops", routesPitStop);//DONE
app.use("/laps", routesLaps);//DONE
app.use("/standings/constructors", routesStandingConstructors);//DONE
app.use("/standings/drivers",routesStandingDrivers);//DONE BUT THE PART OF CONSTRUCTORS IS MISSING
app.use("/races",routesRaceSchedule);//DONE



app.get("/", (req,res) => {
    res.status(200).sendFile(path.join( __dirname,"public","index.html"));
});

app.use(function(req, res, next) {
    res.status(404).send("<h3>Bad Request</h3>");
});

app.listen(PORT,() => {
    console.log(`Server is listening on port ${PORT}`);
});
/*
TODO:
Season List
Race Results
Qualifying Results
Driver Information
Circuit Information
Finishing Status
*/