<?php
if($driverStandings || $constructorStandings) error(400, "Bad Request: Results queries do not support standings qualifiers.");

$query = "SELECT SQL_CALC_FOUND_ROWS
  ra.year, ra.round, ra.name, ra.date, ra.time, ra.url, 
  ci.circuitRef, ci.name, ci.location, ci.country, ci.url, ci.lat, ci.lng, ci.alt,
  re.grid, re.positionText, re.positionOrder, re.number, re.points, re.laps, re.time, re.milliseconds, re.rank, re.fastestLap, re.fastestLapTime, re.fastestLapSpeed,
  dr.driverRef, dr.number, dr.code, dr.forename, dr.surname, dr.dob, dr.nationality, dr.url,
  st.statusId, st.status,
  co.constructorRef, co.name, co.nationality, co.url
  FROM races ra, circuits ci, results re, drivers dr, constructors co, status st";

$query .= " WHERE ra.circuitId=ci.circuitId AND ra.raceId=re.raceId AND re.driverId=dr.driverId AND re.constructorId=co.constructorId AND re.statusId=st.statusId";
if($year) $query .= " AND ra.year='$year'";
if($round) $query .= " AND ra.round='$round'";
if($circuit) $query .= " AND ci.circuitRef='$circuit'";
if($constructor) $query .= " AND co.constructorRef='$constructor'";
if($driver) $query .= " AND dr.driverRef='$driver'";
if($status) $query .= " AND re.statusId='$status'";
if(isset($grid)) $query .= " AND re.grid='$grid'";
if($fastest) $query .= " AND re.rank='$fastest'";
if($results) $query .= " AND re.positionText='$results'";
$query .= " ORDER BY ra.year, ra.round, re.positionOrder LIMIT $offset, $limit";

//echo "$query<br>\n<br>\n";
$result = mysql_query($query) or die('I cannot select from the database because: ' . mysql_error());

if($result) {
  $query = "SELECT FOUND_ROWS()";
  $found_rows_result = mysql_query($query) or die('I cannot select from the database because: ' . mysql_error());
  $row = mysql_fetch_row($found_rows_result);
  $total = $row[0];
  
  if(strcmp($format, "xml") == 0) {
    doXMLHeader($url, $series, $limit, $offset, $total);
    echo "\t<RaceTable";
    if(isset($year)) echo " season=\"$year\"";
    if(isset($round)) echo " round=\"$round\"";
    if(isset($circuit)) echo " circuitId=\"$circuit\"";
    if(isset($constructor)) echo " constructorId=\"$constructor\"";
    if(isset($driver)) echo " driverId=\"$driver\"";
    if(isset($grid)) echo " grid=\"$grid\"";
    if(isset($fastest)) echo " fastest=\"$fastest\"";
    if(isset($results)) echo " position=\"$results\"";
    if(isset($status)) echo " statusId=\"$status\"";
    echo ">\n";
    while($row = @mysql_fetch_array($result)) {
      $year = $row[0];
      $round = $row[1];
      $grid = $row[14];
      $positionText = $row[15];
      $position = $row[16];
      $number = $row[17];
      $points = $row[18];
      $laps = $row[19];
      $time = $row[20];
      $millis = $row[21];
      $rank = $row[22];
      $fastestLap = $row[23];
      $fastestLapTime = $row[24];
      $fastestLapSpeed = $row[25];
      $driverRef = $row[26];
      $permanentNumber = $row[27];
      $code = $row[28];
      $forename = $row[29];
      $surname = $row[30];
      $dob = $row[31];
      $nationality = $row[32];
      $driverUrl = $row[33];
      $statusId = $row[34];
      $status = $row[35];
      $constructorId=$row[36];
      $constructorName=$row[37];
      $constructorNationality = $row[38];
      $constructorUrl = $row[39];
      
      if($round != $currentRound || $year != $currentYear) {
        if($currentYear) {
          echo "\t\t\t</ResultsList>\n";
          echo "\t\t</Race>\n";
        }
        $currentRound = $round;
        $currentYear = $year;
        $raceName = $row[2];
        $date = $row[3];
        $start = $row[4];
        $raceUrl = $row[5];
        $circuitId = $row[6];
        $circuitName = $row[7];
        $locality = $row[8];
        $country = $row[9];
        $circuitUrl = $row[10];
        $lat = $row[11];
        $long = $row[12];
        echo "\t\t<Race season=\"$year\" round=\"$round\" url=\"$raceUrl\">\n";
        echo "\t\t\t<RaceName>$raceName</RaceName>\n";
        echo "\t\t\t<Circuit circuitId=\"$circuitId\" url=\"$circuitUrl\">\n";
        echo "\t\t\t\t<CircuitName>$circuitName</CircuitName>\n";
        echo "\t\t\t\t<Location lat=\"$lat\" long=\"$long\">\n";
        echo "\t\t\t\t\t<Locality>$locality</Locality>\n";
        echo "\t\t\t\t\t<Country>$country</Country>\n";
        echo "\t\t\t\t</Location>\n";
        echo "\t\t\t</Circuit>\n";
        echo "\t\t\t<Date>$date</Date>\n";
        if($start) {
          $start .= "Z";
          echo "\t\t\t<Time>$start</Time>\n";
        }
        echo "\t\t\t<ResultsList>\n";
      }
      echo "\t\t\t\t<Result number=\"$number\" position=\"$position\" positionText=\"$positionText\" points=\"$points\">\n";
      if($code) {
        echo "\t\t\t\t\t<Driver driverId=\"$driverRef\" code=\"$code\" url=\"$driverUrl\">\n";
      } else {
        echo "\t\t\t\t\t<Driver driverId=\"$driverRef\" url=\"$driverUrl\">\n";
      }
      if($permanentNumber) echo "\t\t\t\t\t\t<PermanentNumber>$permanentNumber</PermanentNumber>\n";
      echo "\t\t\t\t\t\t<GivenName>$forename</GivenName>\n";
      echo "\t\t\t\t\t\t<FamilyName>$surname</FamilyName>\n";
      echo "\t\t\t\t\t\t<DateOfBirth>$dob</DateOfBirth>\n";
      echo "\t\t\t\t\t\t<Nationality>$nationality</Nationality>\n";
      echo "\t\t\t\t\t</Driver>\n";
      echo "\t\t\t\t\t<Constructor constructorId=\"$constructorId\" url=\"$constructorUrl\">\n";
      echo "\t\t\t\t\t\t<Name>$constructorName</Name>\n";
      echo "\t\t\t\t\t\t<Nationality>$constructorNationality</Nationality>\n";
      echo "\t\t\t\t\t</Constructor>\n";
      echo "\t\t\t\t\t<Grid>$grid</Grid>\n";
      echo "\t\t\t\t\t<Laps>$laps</Laps>\n";
      echo "\t\t\t\t\t<Status statusId=\"$statusId\">$status</Status>\n";
      if($time) echo "\t\t\t\t\t<Time millis=\"$millis\">$time</Time>\n";
      if($rank) {
        echo "\t\t\t\t\t<FastestLap rank=\"$rank\" lap=\"$fastestLap\">\n";
        echo "\t\t\t\t\t\t<Time>$fastestLapTime</Time>\n";
        echo "\t\t\t\t\t\t<AverageSpeed units=\"kph\">$fastestLapSpeed</AverageSpeed>\n";
        echo "\t\t\t\t\t</FastestLap>\n";
      }
      echo "\t\t\t\t</Result>\n";
    }
    if($currentRound) {
      echo "\t\t\t</ResultsList>\n";
      echo "\t\t</Race>\n";
    }
	echo "\t</RaceTable>\n";
    echo "</MRData>\n";
  } elseif(strcmp($format, "json") == 0) {
    if($callback) {
      doJSONPHeader($url, $series, $limit, $offset, $total, $callback);
    } else {
      doJSONHeader($url, $series, $limit, $offset, $total);
    }
    echo "\"RaceTable\":{";
    if(isset($year)) echo "\"season\":\"$year\",";
    if(isset($round)) echo "\"round\":\"$round\",";
    if(isset($circuit)) echo "\"circuitId\":\"$circuit\",";
    if(isset($constructor)) echo "\"constructorId\":\"$constructor\",";
    if(isset($driver)) echo "\"driverId\":\"$driver\",";
    if(isset($grid)) echo "\"grid\":\"$grid\",";
    if(isset($fastest)) echo "\"fastest\":\"$fastest\",";
    if(isset($results)) echo "\"position\":\"$results\",";
    if(isset($status)) echo "\"status\":\"$status\",";
  
    $numRows = mysql_num_rows($result);
    echo "\"Races\":[";
    if($numRows > 0) {
      while($row = @mysql_fetch_array($result)) {
        $year = $row[0];
        $round = $row[1];
        $grid = $row[14];
        $positionText = $row[15];
        $position = $row[16];
        $number = $row[17];
        $points = $row[18];
        $laps = $row[19];
        $time = $row[20];
        $millis = $row[21];
        $rank = $row[22];
        $fastestLap = $row[23];
        $fastestLapTime = $row[24];
        $fastestLapSpeed = $row[25];
        $driverRef = $row[26];
        $permanentNumber = $row[27];
        $code = $row[28];
        $forename = $row[29];
        $surname = $row[30];
        $dob = $row[31];
        $nationality = $row[32];
        $driverUrl = escape($row[33]);
        $statusId = $row[34];
        $status = $row[35];
        $constructorId=$row[36];
        $constructorName=$row[37];
        $constructorNationality = $row[38];
        $constructorUrl=escape($row[39]);
        
        if($round != $currentRound || $year != $currentYear) {
          if($currentYear) {
            echo "}]},";
          }
          $currentRound = $round;
          $currentYear = $year;
          $raceName = $row[2];
          $date = $row[3];
          $start = $row[4];
          $raceUrl = escape($row[5]);
          $circuitId = $row[6];
          $circuitName = $row[7];
          $locality = $row[8];
          $country = $row[9];
          $circuitUrl = $row[10];
          $lat = $row[11];
          $long = $row[12];
          echo "{";
          echo "\"season\":\"$year\",";
          echo "\"round\":\"$round\",";
          if($raceUrl) echo "\"url\":\"$raceUrl\",";
          echo "\"raceName\":\"$raceName\",";
          echo "\"Circuit\":{";
          echo "\"circuitId\":\"$circuitId\",";
          echo "\"url\":\"$circuitUrl\",";
          echo "\"circuitName\":\"$circuitName\",";
          echo "\"Location\":{";
          echo "\"lat\":\"$lat\",";
          echo "\"long\":\"$long\",";
          echo "\"locality\":\"$locality\",";
          echo "\"country\":\"$country\"";
          echo "}";
          echo "},";
          echo "\"date\":\"$date\",";
          if($start) {
            $start .= "Z";
            echo "\"time\":\"$start\",";
          }
          echo "\"Results\":[";
        } else {
          if($currentYear) {
            echo "},";
          }
        }
        echo "{";     
        echo "\"number\":\"$number\",";
        echo "\"position\":\"$position\",";
        echo "\"positionText\":\"$positionText\",";
        echo "\"points\":\"$points\",";
        echo "\"Driver\":{";
        echo "\"driverId\":\"$driverRef\",";
        if($permanentNumber) echo "\"permanentNumber\":\"$permanentNumber\",";
        if($code) echo "\"code\":\"$code\",";
        echo "\"url\":\"$driverUrl\",";
        echo "\"givenName\":\"$forename\",";
        echo "\"familyName\":\"$surname\",";
        echo "\"dateOfBirth\":\"$dob\",";
        echo "\"nationality\":\"$nationality\"";
        echo "},";
        echo "\"Constructor\":{";
        echo "\"constructorId\":\"$constructorId\",";
        echo "\"url\":\"$constructorUrl\",";
        echo "\"name\":\"$constructorName\",";
        echo "\"nationality\":\"$constructorNationality\"";
        echo "},";
        echo "\"grid\":\"$grid\",";
        echo "\"laps\":\"$laps\",";
        echo "\"status\":\"$status\"";
        if($time) {
          echo ",\"Time\":{";
          echo "\"millis\":\"$millis\",";
          echo "\"time\":\"$time\"";
          echo "}";
        }
        if($rank) {
          echo ",\"FastestLap\":{";
          echo "\"rank\":\"$rank\",";
          echo "\"lap\":\"$fastestLap\",";
          echo "\"Time\":{";
          echo "\"time\":\"$fastestLapTime\"";          
          echo "},";
          echo "\"AverageSpeed\":{";
          echo "\"units\":\"kph\",";    
          echo "\"speed\":\"$fastestLapSpeed\"";          
          echo "}";
          echo "}";
        }
      }
      echo "}]}";
    }
    echo "]}}}";
    if($callback) {
      echo ")";
    }
  }
} else {
  error(404, "Not Found");
}

?>