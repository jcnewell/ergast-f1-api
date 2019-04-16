<?php
include("functions.inc");

$urlComponents = parse_url($_SERVER['REQUEST_URI']);
$path = strtolower(urldecode($urlComponents['path']));
$url = "http://ergast.com" . $path;
$period = strrpos($path, ".");
if($period !== FALSE) {
  $format = substr($path, $period + 1);
  $path = substr($path, 0, $period);
  if(strcmp($format, "xml") != 0 && strcmp($format, "json") != 0) error(404, "Format not found.");
} else {
  $format = "xml";  // Default. 
}

parse_str($urlComponents['query'], $fields);

if(array_key_exists('limit', $fields)) {
  $limit = intval($fields['limit']);
}
if(array_key_exists('offset', $fields)) {
  $offset = intval($fields['offset']);
}
if(!isset($limit)) $limit = 30;
#if($limit > 1000) $limit = 1000;
if(!isset($offset)) $offset = 0;

$callback = $fields['callback'];
if(isset($callback)) {
  if(!isValidCallback($callback)) {
    error(400, "Bad Request");
  }
}

$segments = explode("/", $path);
$series = $segments[2];

if(strcmp($series, "f1") == 0) {
  include("f1dbro.inc");
} else if(strcmp($series, "fe") == 0) {
  include("fedbro.inc");
} else {
  error(404, "Series Not Found: $series");
}

$n = 3;
$key = "races";
$year = NULL;
$round = NULL;

if(strcmp($segments[3], "current") == 0) {
  $year = currentYear();
  $n = 4;
  $key = "races";
} elseif(strlen($segments[3]) == 4 && is_numeric($segments[3])) {
  $year = intval($segments[3]);
  $n = 4;
  $key = "races";
}

if($n == 4) {
  if(strcmp($segments[4], "last") == 0) {
    $round = lastRound($year);
    if(!$round) error(404, "Round Not Found"); 
    $n = 5;
  } elseif(strcmp($segments[4], "next") == 0) {
    $next = nextRound($year);
    $year = $next['year'];
    $round = $next['round'];
    $n = 5;
  } elseif(strlen($segments[4]) < 3 && is_numeric($segments[4])) {
    $round = intval($segments[4]);
    $n = 5;
  }
}

$driver = NULL;
$constructor = NULL;
$circuit = NULL;
$status = NULL;
$results = NULL;
$laps = NULL;
$pitstops = NULL;
$driverStandings = NULL;
$constructorStandings = NULL;
$grid = NULL;
$fastest = NULL;
$qualifying = NULL;

for($i=$n; $i<$n+17; $i+=2) {
  if(isset($segments[$i])) {
    $key = $segments[$i];
    switch($key) {
      case "drivers":
        if($driver) {
          error(400, "Bad Request");
        } else {
          if(isset($segments[$i+1])) {
            $driver = clean($segments[$i+1]);
          } else { 
            break 2;
          }        
        }
        break;
      case "constructors":
        if($constructor) {
          error(400, "Bad Request");
        } else {
          if($segments[$i+1]) {
            $constructor = clean($segments[$i+1]);
          } else {
            break 2;
          }        
        }
        break;
      case "circuits":
        if($circuit) {
          error(400, "Bad Request");
        } else {
          if($segments[$i+1]) {
            $circuit = clean($segments[$i+1]);
          } else {
            break 2;
          }        
        }
        break;
      case "status":
        if($status) {
          error(400, "Bad Request");
        } else {
          if($segments[$i+1]) {
            $status = clean($segments[$i+1]);
          } else {
            break 2;
          }        
        }
        break;  
      case "results":
        if($results) {
          error(400, "Bad Request");
        } else {
          if($segments[$i+1]) {
            if(is_numeric($segments[$i+1])) {
              $results = intval($segments[$i+1]);
            } else {
              error(400, "Bad Request");
            }
          } else {
            break 2;
          }        
        }
        break;
      case "laps":
        if($laps) {
          error(400, "Bad Request");
        } else {
          if($segments[$i+1]) {
            if(is_numeric($segments[$i+1])) {
              $laps = intval($segments[$i+1]);
            } else {
              error(400, "Bad Request");
            }
          } else {
            break 2;
          }
        }
        break;
      case "pitstops":
        if($pitstops) {
          error(400, "Bad Request");
        } else {
          if($segments[$i+1]) {
            if(is_numeric($segments[$i+1])) {
              $pitstops = intval($segments[$i+1]);
            } else {
              error(400, "Bad Request");
            }
          } else {
            break 2;
          }
        }
        break;  
      case "driverstandings":
        if($driverStandings) {
          error(400, "Bad Request");
        } else {
          if($segments[$i+1]) {
            if(is_numeric($segments[$i+1])) {
              $driverStandings = intval($segments[$i+1]);
            } else {
              error(400, "Bad Request");
            }
          } else {
            break 2;
          }        
        }
        break;
      case "constructorstandings":
        if($constructorStandings) {
          error(400, "Bad Request");
        } else {
          if($segments[$i+1]) {
            if(is_numeric($segments[$i+1])) {
              $constructorStandings = intval($segments[$i+1]);
            } else {
              error(400, "Bad Request");
            }
          } else {
            break 2;
          }        
        }
        break;
      case "grid":
        if($grid) {
          error(400, "Bad Request");
        } else {
          if(isset($segments[$i+1]) && is_numeric($segments[$i+1])) {
            $grid = intval($segments[$i+1]);
          } else {
            // "grid" cannot be last element.
            error(400, "Bad Request");
          }        
        }
        break;
      case "fastest":
        if($fastest) {
          error(400, "Bad Request");
        } else {
          if($segments[$i+1] && is_numeric($segments[$i+1])) {
            $fastest = intval($segments[$i+1]);
          } else {
            // "fastest" cannot be last element.
            error(400, "Bad Request");
          }        
        }
        break;
      case "qualifying":
        if($qualifying) {
          error(400, "Bad Request");
        } else {
          if($segments[$i+1]) {
            if(is_numeric($segments[$i+1])) {
              $qualifying = intval($segments[$i+1]);
            } else {
              error(400, "Bad Request");
            }
          } else {
            break 2;
          }        
        }
        break;
        //if(isset($segments[$i+1])) {
        //  // "qualifying" can only be last segment.
        //  error(400, "Bad Request");
        //} else {
        //  break 2;
        //}
        //break;
      case "seasons":
        if(isset($segments[$i+1])) {
          // "seasons" can only be last segment.
          error(400, "Bad Request");
        } else {
          break 2;
        }
        break;
      case "races":
        if(isset($segments[$i+1])) {
          // "races" can only be last segment.
          error(400, "Bad Request");
        } else {
          break 2;
        }
        break;
      default:
        error(400, "Bad Request");
    }
  } else {
    break;
  }
}

if($limit > 1000) {
  if(strcmp($key, "laps") == 0) {
    $limit = 2000;
  } else {
    $limit = 1000;
  }
}

switch($key) {
  case "drivers":
    include("DriverTable.inc");
    break;
  case "constructors":
    include("ConstructorTable.inc");
    break;
  case "circuits":
    include("CircuitTable.inc");
    break;
  case "status":
    include("StatusTable.inc");
    break;
  case "races":
    include("RaceTable.inc");
    break;    
  case "results":
    include("RaceResults.inc");
    break;
   case "laps":
    include("LapTimes.inc");
    break;
  case "pitstops":
    include("PitStops.inc");
    break;
  case "qualifying":
    include("Qualifying.inc");    
    break;
  case "driverstandings":
    include("DriverStandings.inc");    
    break;
  case "constructorstandings":
    include("ConstructorStandings.inc");    
    break;
  case "seasons":
    include("SeasonTable.inc");    
    break;
  default:
    error(400, "Bad Request");
}
exit();
?>
