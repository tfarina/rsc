<?php

require_once 'config.php';

if (is_user_logged_in()) {
  header('Location: list.php');
  exit;
}

$debug = false;

if (isset($_POST['login'])) {
  foreach($_POST as $key => $value) {
    //$data[$key] = filter($value); // post variables are filtered
    $data[$key] = mysql_real_escape_string($value);
  }

  $valid = true;

  $email = $data['email'];
  $emailError = "";
  if (empty($email)) {
    $emailError = "Please enter your email.";
    $valid = false;
  } else {
    $email = test_input($email);
    if (!is_valid_email_address($email)) {
      $emailError = "Please enter a valid email address.";
      $valid = false;
    } elseif (!email_exists($email)) {
      $emailError = "Sorry, we don't recognize this email.";
      $valid = false;
    }
  }

  $plain_password = $data['password'];
  $passwordError = "";
  if (empty($plain_password)) {
    $passwordError = "Please enter your password.";
    $valid = false;
  } else {
    $plain_password = test_input($plain_password);
  }

  // End of data validation.
  if ($valid) {
    $query = "SELECT * FROM user WHERE email='" . $email . "';";
    $result = mysql_query($query) or die(mysql_error());
    $num_rows = mysql_num_rows($result);
    if ($num_rows == 1) {
      $row = mysql_fetch_assoc($result);
      mysql_free_result($result);
      if (!password_verify($plain_password, $row['password'])) {
	$passwordError = "The email and password you entered don't match.";
        $valid = false;
      } else {
        $_SESSION['userid'] = $row['user_id'];
	$_SESSION['fullname'] = $row['fullname'];
	header("location: list.php");
	exit;
      }
    }
  }
}

include_once("header.php");
?>
    <nav class="navbar navbar-default navbar-fixed-top">
      <div class="container">
        <div class="navbar-header">
          <button type="button" class="navbar-toggle collapsed" data-toggle="collapse"
                  data-target="#navbar" aria-expanded="false" aria-controls="navbar">
            <span class="sr-only">Toggle navigation</span>
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
          </button>
          <a class="navbar-brand" href="index.php">Bookmarks</a>
        </div>
        <div id="navbar" class="collapse navbar-collapse">
          <ul class="nav navbar-nav navbar-right">
            <li><a href="pricing.php">Pricing</a></li>
            <li class="active"><a href="login.php">Log in</a></li>
            <li><a href="signup.php">Sign up</a></li>
          </ul>
        </div>
      </div>
    </nav>

<!-- Begin page content -->
<div class="container">
  <div class="row">
    <div class="panel-signin">
      <div class="panel panel-default">
        <div class="panel-body">
          <form action="" method="post" class="form-signin form-horizontal">
            <h2 class="form-signin-heading">Log in </h2>
            <div class="form-group <?php echo !empty($emailError) ? 'has-error' : ''; ?>">
              <input type="email" class="form-control" id="user-email" name="email"
                     value="<?php echo $_POST['email']; ?>"
                     placeholder="Email" required autofocus/>
              <?php if (!empty($emailError)) { ?>
              <span class="help-block"><?php echo $emailError; ?></span>
              <?php } ?>
            </div>
            <div class="form-group <?php echo !empty($passwordError) ? 'has-error' : ''; ?>">
              <input type="password" class="form-control" id="user-password" name="password"
                     placeholder="Password" required/>
              <?php if (!empty($passwordError)) { ?>
              <span class="help-block"><?php echo $passwordError; ?></span>
              <?php } ?>
            </div>
            <div class="form-group">
              <button type="submit" name="login" class="btn btn-lg btn-block btn-primary">Log in</button>
            </div>
          </form>
        </div>
      </div>
    </div>
    <div class="panel panel-default panel-signup">
      <div class="panel-body">
        <span>Don't have an account? <a href="signup.php">Sign up</a></span>
      </div>
    </div>
  </div>
</div>
<!-- End page content -->

<footer class="footer">
  <div class="container">
    <span class="copyright-text">&copy; 2016 Bookmarks.</span>
  </div>
</footer>

<?php
include_once("footer.php");
?>
