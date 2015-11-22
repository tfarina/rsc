<?php
define("UPLOAD_DIR", "/var/www/tfarina.tk/gallery2/gallery-images/");

// show upload form
if ($_SERVER["REQUEST_METHOD"] == "GET") {
?>
 <h3>Choose a file to upload: </h3>
<p>
<em>Only GIF, JPG, and PNG files are allowed.</em>
</p>
<form action="upload.php" method="post" enctype="multipart/form-data">
 <input type="file" name="myFile"/>
 <input type="submit" value="Upload"/>
</form>
<?php
}
// process file upload
else if ($_SERVER["REQUEST_METHOD"] == "POST" && !empty($_FILES["myFile"])) {
    $myFile = $_FILES["myFile"];
    if ($myFile["error"] !== UPLOAD_ERR_OK) {
        echo "<p>An error occurred.</p>";
        exit;
    }

    // verify the file type
    $fileType = exif_imagetype($_FILES["myFile"]["tmp_name"]);
    $allowed = array(IMAGETYPE_GIF, IMAGETYPE_JPEG, IMAGETYPE_PNG);
    if (!in_array($fileType, $allowed)) {
        echo "<p>File type is not permitted.</p>";
        exit;
    }

    // ensure a safe filename
    $name = preg_replace("/[^A-Z0-9._-]/i", "_", $myFile["name"]);

    // don't overwrite an existing file
    $i = 0;
    $parts = pathinfo($name);
    while (file_exists(UPLOAD_DIR . $name)) {
        $i++;
        $name = $parts["filename"] . "-" . $i . "." . $parts["extension"];
    }

    // preserve file from temporary directory
    $success = move_uploaded_file($myFile["tmp_name"], UPLOAD_DIR . $name);
    if (!$success) {
        echo "<p>Unable to save file.</p>";
        exit;
    }

    // set proper permissions on the new file
    chmod(UPLOAD_DIR . $name, 0644);

    echo "<p>The file ". $name ." has been uploaded.</p>";
}
