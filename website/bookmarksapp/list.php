<?php

include('config.php');

$query = "SELECT * FROM bookmarks ORDER BY dt DESC";
$result = mysql_query($query) or die(mysql_error());

$bookmarks = array();
while ($row = mysql_fetch_assoc($result)) {
  $bookmarks[] = $row;
}

mysql_free_result($result);

include_once("header.php");
?>
<div class="main">
  <table class="pure-table pure-table-horizontal">
    <thead>
      <tr>
        <th>Title</th>
        <th>URL</th>
        <th>Date Created</th>
        <th>Actions</th>
      </tr>
    </thead>
    <tbody>
    <?php foreach ($bookmarks as $bookmark) { ?>
      <tr>
        <td><a href="<?php echo $bookmark['url']; ?>"><?php echo $bookmark['title']; ?></a></td>
        <td><?php echo $bookmark['url']; ?></td>
        <td><?php echo $bookmark['dt']; ?></td>
        <td>
          <a href="edit.php?id=<?php echo $bookmark['id']; ?>">Edit</a>  <a href="delete.php?id=<?php echo $bookmark['id']; ?>">Delete</a>
        </td>
      </tr>
    <?php } ?>
    </tbody>
  </table>
</div>
<?php
include_once("footer.php");
?>
