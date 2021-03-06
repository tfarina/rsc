<?php

require_once 'config.php';

$id = -1;

if (!empty($_GET['id'])) {
  $id = $_GET['id'];
}

if ($id == -1) {
  header('Location: list.php');
}

if (!empty($_POST)) {
  $id = $_POST['id'];

  $query = "DELETE FROM bookmarks WHERE id='" . $id . "'";
  mysql_query($query) or die(mysql_error());

  $message = "Bookmark has been deleted successfully.";
  $_SESSION['msg-success'] = $message;
  header("Location: list.php");
  exit;
}
?>
<?php
include_once("header.php");
?>
  <nav class="navbar navbar-default navbar-fixed-top" role="navigation">
    <div class="container-fluid">
      <div class="navbar-header">
        <a class="navbar-brand" href="#">Bookmarks</a>
      </div>
      <div id="navbar" class="navbar-collapse collapse">
        <ul class="nav navbar-nav navbar-right">
          <li class="dropdown">
            <a class="dropdown-toggle" data-toggle="dropdown" href="#">
              <i class="fa fa-user fa-fw"></i>
              <i class="fa fa-caret-down"></i>
            </a>
            <ul class="dropdown-menu dropdown-user">
              <li><a href="account.php"><i class="fa fa-gear fa-fw"></i> My Account</a></li>
              <li class="divider"></li>
              <li><a href="logout.php"><i class="fa fa-sign-out fa-fw"></i> Log Out</a></li>
            </ul>
          </li>
        </ul>
      </div>
    </div>
  </nav>
  <div class="container-fluid">
    <div class="row">
      <div class="col-sm-3 col-md-2 sidebar">
        <ul class="nav nav-sidebar">
          <li class="active">
            <a href="list.php">My bookmarks</a>
          </li>
        </ul>
      </div>
      <div class="col-sm-9 col-sm-offset-3 col-md-10 col-md-offset-2 main">
        <h3 class="page-header">Delete bookmark</h3>
        <form action="" method="post">
          <input type="hidden" name="id" value="<?php echo $id; ?>" />
          <p class="alert alert-error">Are you sure you want to delete this bookmark?</p>
          <div class="form-group">
            <button type="submit" class="btn btn-danger">Yes</button>
            <a class="btn btn-default" href="list.php">No</a>
          </div>
        </form>
      </div>
    </div>
  </div>
<?php
include_once("footer.php");
?>
