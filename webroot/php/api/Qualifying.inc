<?php
if($driverStandings || $constructorStandings) error(400, "Bad Request: Qualifying queries do not support standings qualifiers.");

$query = "SELECT SQL_CALC_FOUND_ROWS
  ra.year, ra.round, ra.name, ra.date, ra.time, ra.url, 
  ci.circuitRef, ci.name, ci.location, ci.country, ci.url, ci.lat, ci.lng, ci.alt,
  qu.number, qu.position, qu.q1, qu.q2, qu.q3,
  dr.driverRef, dr.number, dr.code, dr.forename, dr.surname, dr.dob, dr.nationality, dr.url,
  co.constructorRef, co.name, co.nationality, co.url
  FROM races ra, circuits ci, qualifying qu, drivers dr, constructors co"; 
if($grid || $results || $status || $fastest) $query .= ", results re";

$query .= " WHERE ra.circuitId=ci.circuitId AND qu.raceId=ra.raceId AND qu.driverId=dr.driverId AND qu.constructorId=co.constructorId";
if($grid || $results || $status || $fastest) $query .= " AND re.raceId=qu.raceId AND re.driverId=qu.driverId AND re.constructorId=qu.constructorId ";
if($year)    $query .= " AND ra.year='$year'";
if($round)   $query .= " AND ra.round='$round'";
if($circuit) $query .= " AND ci.circuitRef='$circuit'";
if($constructor) $query .= " AND co.constructorRef='$constructor'";
if($driver)  $query .= " AND dr.driverRef='$driver'";
if($status) $query .= " AND re.statusId='$status'";
if($grid)    $query .= " AND re.grid='$grid'";
if($fastest) $query .= " AND re.rank='$fastest'";
if($results)  $query .= " AND re.positionText='$results'";
$query .= " ORDER BY ra.year, ra.round, qu.position LIMIT $offset, $limit";

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
      $number = $row[14];
      $position = $row[15];
      $q1 = $row[16];
      $q2 = $row[17];
      $q3 = $row[18];
      $driverRef = $row[19];
      $permanentNumber = $row[20];
      $code = $row[21];
      $forename = $row[22];
      $surname = $row[23];
      $dob = $row[24];
      $nationality = $row[25];
      $driverUrl = $row[26];
      $constructorRef = $row[27];
      $constructorName = $row[28];
      $constructorNationality = $row[29];
      $constructorUrl = $row[30];
      
      if($round != $currentRound || $year != $currentYear) {
        if($currentYear) {
          echo "\t\t\t</QualifyingList>\n";
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
        echo "\t\t\t<QualifyingList>\n";
      }
      echo "\t\t\t\t<QualifyingResult number=\"$number\" position=\"$position\">\n";
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
      echo "\t\t\t\t\t<Constructor constructorId=\"$constructorRef\" url=\"$constructorUrl\">\n";
      echo "\t\t\t\t\t\t<Name>$constructorName</Name>\n";
      echo "\t\t\t\t\t\t<Nationality>$constructorNationality</Nationality>\n";
      echo "\t\t\t\t\t</Constructor>\n";
      if($q1) echo "\t\t\t\t\t<Q1>$q1</Q1>\n";
      if($q2) echo "\t\t\t\t\t<Q2>$q2</Q2>\n";
      if($q3) echo "\t\t\t\t\t<Q3>$q3</Q3>\n";
      echo "\t\t\t\t</QualifyingResult>\n";
    }
    if($currentRound) {
      echo "\t\t\t</QualifyingList>\n";
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
        $number = $row[14];
        $position = $row[15];
        $q1 = $row[16];
        $q2 = $row[17];
        $q3 = $row[18];
        $driverRef = $row[19];
        $permanentNumber = $row[20];
        $code = $row[21];
        $forename = $row[22];
        $surname = $row[23];
        $dob = $row[24];
        $nationality = $row[25];
        $driverUrl = $row[26];
        $constructorRef = $row[27];
        $constructorName = $row[28];
        $constructorNationality = $row[29];
        $constructorUrl = $row[30];
        
        if($round != $currentRound || $year != $currentYear) {
          if($currentYear) {
            echo "}]},";
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
          echo "}},";
          echo "\"date\":\"$date\",";
          if($start) {
            $start .= "Z";
            echo "\"time\":\"$start\",";
          }
          echo "\"QualifyingResults\":[";
        } else {
          if($currentYear) {
            echo "},";
          }
        }
        echo "{";     
        echo "\"number\":\"$number\",";
        echo "\"position\":\"$position\",";
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
        echo "\"constructorId\":\"$constructorRef\",";
        echo "\"url\":\"$constructorUrl\",";
        echo "\"name\":\"$constructorName\",";
        echo "\"nationality\":\"$constructorNationality\"";
        echo "},";
        echo "\"Q1\":\"$q1\"";
        if($q2) echo ",\"Q2\":\"$q2\"";
        if($q3) echo ",\"Q3\":\"$q3\"";
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