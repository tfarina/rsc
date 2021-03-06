<?php

require_once 'config.php';

if (is_user_logged_in()) {
  header('Location: list.php');
  exit;
}

if (isset($_POST['signup'])) {
  foreach($_POST as $key => $value) {
    $data[$key] = mysql_real_escape_string($value);
  }

  $nameError = "";
  $emailError = "";
  $passwordError = "";

  $valid = true;

  $fullname = $data['fullname'];
  if (empty($fullname)) {
    $nameError = "What's your name?";
    $valid = false;
  }

  $email = $data['email'];
  if (empty($email)) {
    $emailError = "Please enter your email.";
    $valid = false;
  } elseif (!is_valid_email_address($email)) {
    $emailError = "Please enter a valid email address.";
    $valid = false;
  } else {
    if (email_exists($email)) {
      $emailError = "This email is already registered.";
      $valid = false;
    }
  }

  $plain_password = $data['password'];
  if (empty($plain_password) || strlen($plain_password) < 6) {
    $passwordError = "Your password must be at least 6 characters.";
    $valid = false;
  }

  if ($valid) {
    $secure_password = password_hash($plain_password, PASSWORD_BCRYPT, array('cost' => 12));

    $query = "INSERT INTO user (fullname, email, password) VALUES ";
    $query .= "('$fullname', '$email', '$secure_password')";
    mysql_query($query) or die(mysql_error());

    $subject = "[bookmarks]: Welcome new member";
    $message = "You are in!\n\n";
    $message .= "Go ahead and start adding bookmarks.\n";
    $headers = "From: Thiago Farina <thiago.farina@gmail.com>";
    mail($email, $subject, $message, $headers);

    $user_id = mysql_insert_id();
    if (isset($user_id)) {
      $_SESSION['userid'] = $user_id;
      $_SESSION['fullname'] = $fullname;
      header("location: list.php");
      exit;
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
            <li><a href="login.php">Log in</a></li>
            <li class="active"><a href="signup.php">Sign up</a></li>
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
            <form id="new_user" action="" method="post" role="form">
              <h3>Join us today.</h3>
              <br/>
              <div class="form-group <?php echo !empty($nameError) ? 'has-error' : ''; ?>">
                <input type="text" class="form-control" id="user-fullname" name="fullname"
                       value="<?php echo $_POST['fullname']; ?>"
                       placeholder="Full name" required autofocus/>
                <?php if (!empty($nameError)) { ?>
                <span class="help-block"><?php echo $nameError; ?></span>
                <?php } ?>
              </div>
              <div class="form-group <?php echo !empty($emailError) ? 'has-error' : ''; ?>">
                <input type="email" class="form-control" id="user-email" name="email"
                       value="<?php echo $_POST['email']; ?>"
                       placeholder="Email" required />
                <?php if (!empty($emailError)) { ?>
                <span class="help-block"><?php echo $emailError; ?></span>
                <?php } ?>
              </div>
              <div class="form-group <?php echo !empty($passwordError) ? 'has-error' : ''; ?>">
                <input type="password" class="form-control" id="user-password" name="password"
                       placeholder="Password" required />
                <?php if (!empty($passwordError)) { ?>
                <span class="help-block"><?php echo $passwordError; ?></span>
                <?php } ?>
              </div>
              <div class="form-group">
                <button type="submit" name="signup" class="btn btn-lg btn-primary btn-block">Sign up</button>
              </div>
              <p class="text-muted text-center">
                By signing up, you agree to our <br/>
                <a href="terms.php">Terms</a> and
                <a href="privacy.php">Privacy Policy</a>.
              </p>
            </form>
          </div>
        </div>
      </div>
      <div class="panel panel-default panel-signup">
        <div class="panel-body">
          <span>Have an account? <a href="login.php">Log in</a></span>
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

</body>
</html>
