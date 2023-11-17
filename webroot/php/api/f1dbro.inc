<?php
$user="root";
$password="f1";
$database="ergastdb";
$mysqli = mysqli_connect("localhost", $user, $password);
mysqli_set_charset($mysqli, "utf8");
@mysqli_select_db($mysqli, $database) or die( "Unable to select database");
?>
