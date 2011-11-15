<?php
// We want to require this as early as possible, because it is in control of the
// authentication, login, logout, etc..
require_once('config.inc');
require_once('lib/h2oscore.pear.inc');
// If you have the 'oauth' PECL module, comment out the line above, and uncomment
// the one below.
//require_once('lib/h2oscore.pecl.inc');
?>
<!DOCTYPE html>
<html>
<head>
  <title>My Sweet App</title>
</head>
<body>
  <h1>My Sweet App</h1>
<?php if ($user): ?>

  <p>You are: <?php print $user->name; ?> (uid=<?php print $user->uid; ?>)</p>

  <?php
      // get my address
      $addresses = h2oscore('GET', 'address', array('limit' => 1, 'parameters[owned_by]' => 'me'));
      $address = $addresses[0];

      // get my latest score
      $score = h2oscore('POST', $address->uri . '/getLatestScore');
  ?>

  <p>Your address is:</p>

  <p><?php print $address->street; ?><br />
     <?php print $address->city; ?>, <?php print $address->province; ?> <?php print $address->postal_code; ?></p>

  <p>Your H2OScore is: <strong><?php print $score->percentile; ?></strong> (1-100, lower is better)</p>

  <p>You use <strong><?php print $score->gallons_per_day; ?> gallons per day</strong> (on average) which is <strong><?php print $score->percent_difference_parts[0]; ?>% <?php print $score->percent_difference_parts[1]; ?></strong> than the average household with <strong><?php print $score->people; ?> residents</strong>.</p>

  <p><a href="?logout=1">Logout</a> of My Sweet App.</p>

<?php else: ?>

  <p><strong>My Sweet App</strong> is the bossest app for H2OScore.com ever!</p>
  <p><a href="?login=1">Click here</a> to login via H2OScore in order to use it!</p>

<?php endif; ?>

</body>
</html>

