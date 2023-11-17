<?php

function clean($string) {
  include("f1dbro.inc");
  $string = trim($string);
  #if(get_magic_quotes_gpc()) {
  #  if(ini_get('magic_quotes_sybase')) {
  #    $string = str_replace("''", "'", $string);
  #  } else {
  $string = stripslashes($string);
  #  }
  #}
  $string = mysqli_real_escape_string($mysqli, $string);
  return $string;
}

function currentYear() {
  $query = "SELECT MAX(races.year) FROM races, results WHERE races.raceId=results.raceId";
  include("f1dbro.inc");
  $result = mysqli_query($mysqli, $query) or die('I cannot select from the database because: ' . mysql_error());
  if($result) {
    $row = @mysqli_fetch_array($result);
    $year = $row[0];
    return $year;
  }
}

function lastRound($year) {
  $query = "SELECT MAX(races.round) FROM races, results WHERE races.year='$year' AND races.raceId=results.raceId";
  include("f1dbro.inc");
  $result = mysqli_query($mysqli, $query) or die('I cannot select from the database because: ' . mysql_error());
  if($result) {
    $row = @mysqli_fetch_array($result);
    $last = $row[0];
    return $last;
  }
}

function nextRound($year) {
  $query = "SELECT MAX(round) FROM races WHERE year='$year'";
  include("f1dbro.inc");
  $result = mysqli_query($mysqli, $query) or die('I cannot select from the database because: ' . mysql_error());
  if($result) {
    $row = @mysqli_fetch_array($result);
    $final = $row[0];
    $last = lastRound($year);
    if($last < $final) {
      $next['year'] = $year;
      $next['round'] = $last + 1;
    } else {
      $next['year'] = $year + 1;
      $next['round'] = 1;
    }
    return $next;
  }
}

function doXMLHeader($url, $series, $limit, $offset, $total) {
  header('Content-type: application/xml; charset=utf-8');
  header('Access-Control-Allow-Origin: *');
  //header('Expires: '.gmdate('D, d M Y H:i:s \G\M\T', time() + (300)));
  header("Cache-Control: public, max-age=300");
  
  echo "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
  echo "<?xml-stylesheet type=\"text/xsl\" href=\"/schemas/mrd-1.5.xsl\"?>\n";
  echo "<MRData xmlns=\"http://ergast.com/mrd/1.5\" series=\"$series\" url=\"$url\" limit=\"$limit\" offset=\"$offset\" total=\"$total\">\n";
}

function doJSONHeader($url, $series, $limit, $offset, $total) {
  header('Content-type: application/json; charset=utf-8');
  header('Access-Control-Allow-Origin: *');
  //header('Expires: '.gmdate('D, d M Y H:i:s \G\M\T', time() + (300)));
  header("Cache-Control: public, max-age=300");
  
  echo "{";
  echo "\"MRData\":{";
  echo "\"xmlns\":\"http:\\/\\/ergast.com\\/mrd\\/1.5\",";
  echo "\"series\":\"$series\",";
  echo "\"url\":\"$url\",";
  echo "\"limit\":\"$limit\",";
  echo "\"offset\":\"$offset\",";
  echo "\"total\":\"$total\",";
}

function doJSONPHeader($url, $series, $limit, $offset, $total, $callback) {
  header('Content-type: application/javascript; charset=utf-8');
  header('Access-Control-Allow-Origin: *');
  echo "$callback({";
  echo "\"MRData\":{";
  echo "\"xmlns\":\"http:\\/\\/ergast.com\\/mrd\\/1.5\",";
  echo "\"series\":\"$series\",";
  echo "\"url\":\"$url\",";
  echo "\"limit\":\"$limit\",";
  echo "\"offset\":\"$offset\",";
  echo "\"total\":\"$total\",";
}

function escape($string) {
  $string = str_replace("/", "\/", $string);
  return $string;
}

function error($code, $message) {
header("HTTP/1.0 $code $message");
?>
<html>
  <head>
    <title>Error</title>
  </head>
  <body>
    <h3><?php echo $message ?></h3>
  </body>
</html>
<?php
  exit(0);
}

function error500() {
header("HTTP/1.0 500 Internal Server Error");
?>
<html>
  <head>
    <title>Internal Server Error</title>
  </head>
  <body>
    <h3>Internal Server Error. Please contact admin@ergast.com with details.</h3>
  </body>
</html>
<?php
  exit(0);
}

// https://gist.github.com/1217080
function isValidCallback($callback) {
//   $reserved = array(
//       'break',
//       'do',
//       'instanceof',
//       'typeof',
//       'case',
//       'else',
//       'new',
//       'var',
//       'catch',
//       'finally',
//       'return',
//       'void',
//       'continue',
//       'for',
//       'switch',
//       'while',
//       'debugger',
//       'function',
//       'this',
//       'with',
//       'default',
//       'if',
//       'throw',
//       'delete',
//       'in',
//       'try',
//       'class',
//       'enum',
//       'extends',
//       'super',
//       'const',
//       'export',
//       'import',
//       'implements',
//       'let',
//       'private',
//       'public',
//       'yield',
//       'interface',
//       'package',
//       'protected',
//       'static',
//       'null',
//       'true',
//       'false'
//   );

//   foreach(explode('.', $callback) as $identifier) {
//     if(!preg_match('/^[a-zA-Z_$][0-9a-zA-Z_$]*(?:\[(?:".+"|\'.+\'|\d+)\])*?$/', $identifier)) {
//       return false;
//     }
//     if(in_array($identifier, $reserved)) {
//       return false;
//     }
//   }

  return true;
}

?>
