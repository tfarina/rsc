<?php
include('config.php');
if (isset($_GET['id'])) {
  if (isset($_POST['submitted'])) {
    foreach ($_POST as $key => $value) {
      $_POST[$key] = mysql_real_escape_string($value);
    }

    $sql = "UPDATE bookmarks SET url='" . $_POST['url'] . "', title='" . $_POST['title'] . "' WHERE id='" . $_GET['id'] . "'";
    mysql_query($sql) or die(mysql_error());

    echo mysql_affected_rows() ? "Bookmark edited. <br />"
                               : "Nothing changed. <br />";
    echo "<a href='list.php'>Back To Listing</a>";
  }

  $row = mysql_fetch_array(mysql_query("SELECT * FROM bookmarks WHERE id='" . $_GET['id'] . "'"));
?>

<h2> Edit bookmark </h2>
<form action='' method='POST'>
  <table>
    <tr>
      <td>Title</td>
      <td><input type="text" name='title' value="<?php echo stripslashes($row['title']) ?>"/></td>
    </tr>
    <tr>
      <td>URL</td>
      <td><input type="text" name='url' value="<?php echo stripslashes($row['url']) ?>"/></td>
    </tr>
    <tr colspan=2>
      <td><input type='submit' value='Save' /><input type='hidden' value='1' name='submitted' /></td>
    </tr>
  </table>
</form>

<?php } ?>
