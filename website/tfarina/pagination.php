<?php

// https://www.developphp.com/video/PHP/Pagination-MySQLi-Google-Style-Paged-Results-Tutorial
// http://www.phpfreaks.com/tutorial/basic-pagination

// mysql> CREATE USER 'testuser'@'localhost' IDENTIFIED BY 'testpass';
// mysql> GRANT ALL PRIVILEGES ON test.* TO 'testuser'@'localhost';
// mysql> FLUSH PRIVILEGES;

$CONF['db_host'] = "localhost";
$CONF['db_username'] = "testuser";
$CONF['db_password'] = "testpass";
$CONF['db_name'] = "test";

$link = @mysql_connect($CONF['db_host'], $CONF['db_username'], $CONF['db_password']);
if (!$link) {
  die('Could not connect to mysql: ' . mysql_error());
}

if (!mysql_select_db($CONF['db_name'], $link)) {
  die ('Could not select database ' . $CONF['db_name'] . ': ' . mysql_error());
}

// Find out how many rows are in the table.
$query = "SELECT COUNT(id) AS articles_count FROM articles";
$result = mysql_query($query) or die (mysql_error());
$row = mysql_fetch_array($result);
$t_articles_count = $row['articles_count'];

//echo $t_articles_count;

// The number of entries to show per page.
$perpage = 10;

//echo "\n";

// Find out how many pages we will have.
$lastpage = ceil($t_articles_count / $perpage);
    
//echo $lastpage;

if ($lastpage < 1) {
  $lastpage = 1;
} 

// The page currently returned.
$pagenum = 1;

// Get the current page.
if (isset($_GET['page']) && is_numeric($_GET['page'])) {
  $pagenum = preg_replace('#[^0-9]#', '', $_GET['page']);
}

//echo $pagenum;

// Make sure $pagenum isn't before the first page.
if ($pagenum < 1) {
  $pagenum = 1;
}

// Make sure $pagenum isn't past the last page.
if ($pagenum > $lastpage) {
  $pagenum = $lastpage;
}

$offset = ($pagenum - 1) * $perpage;

$query = "SELECT * FROM articles LIMIT $offset, $perpage";
$result = mysql_query($query) or die (mysql_error());

$start = $offset + 1;
$to = (($perpage * $pagenum) > $t_articles_count) ? $t_articles_count : ($perpage * $pagenum);

$paginationCtrls = '';

// If there is more than one page worth of results then we have to show
// the pagination system, otherwise it is not worth to show it.
if ($lastpage > 1) {
  if ($pagenum > 1) {
    $previous = $pagenum - 1;
   
    $paginationCtrls .= '<a href="'.$_SERVER['PHP_SELF'].'?page=1">First</a> &nbsp; ';
    $paginationCtrls .= '<a href="'.$_SERVER['PHP_SELF'].'?page='.$previous.'">Previous</a> &nbsp; &nbsp; ';

    for ($i = $pagenum - 4; $i < $pagenum; $i++) {
      if ($i > 0) {
        $paginationCtrls .= '<a href="' . $_SERVER['PHP_SELF'] . '?page=' . $i . '">' . $i . '</a> &nbsp; ';
      }
    }
  }

  $paginationCtrls .= ''. $pagenum .' &nbsp; ';

  for ($i = $pagenum + 1; $i <= $lastpage; $i++) {
    $paginationCtrls .= '<a href="' . $_SERVER['PHP_SELF'] . '?page=' . $i . '">' . $i . '</a> &nbsp; ';

    if ($i >= $pagenum + 4) {
      break;
    }
  }

  if ($pagenum != $lastpage) {
    $next = $pagenum + 1;

    $paginationCtrls .= '&nbsp; <a href="'.$_SERVER['PHP_SELF'].'?page='.$next.'">Next</a> &nbsp; ';
    $paginationCtrls .= '<a href="'.$_SERVER['PHP_SELF'].'?page='.$lastpage.'">Last</a>';
  }
}

?>
<!DOCTYPE html>
<html lang="en">
<head>
  <title>Pagination</title>
</head>
<body>
<?php 
echo "<p>Showing " . $start . " - " . $to . " of " . $t_articles_count . " results</p>";

while ($row = mysql_fetch_assoc($result)) {
  echo "<p>" . $row['body'] . "</p>\n";
}
?>
  <div><?php echo $paginationCtrls; ?></div>
</body>
</html>
