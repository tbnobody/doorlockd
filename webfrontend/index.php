<?php
	function tellLock($pCommand, $pUser, $pPass, $pIp){

		$json = '{
			"user":' . json_encode( $pUser ) . ',
			"password":' . json_encode( $pPass ) . ',
			"command":' . json_encode( $pCommand ) . ',
			"ip":' . json_encode( $pIp ) . '
		}'."\n";

		$address = "127.0.0.1";
		$port = "5555";

		$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
		if ($socket === false) {
			echo "socket_create() failed: " . socket_strerror(socket_last_error()) . "\n";
		}

		$result = socket_connect($socket, $address, $port);
		if ($result === false) {
			echo "socket_connect() failed: ($result) " . socket_strerror(socket_last_error($socket)) . "\n";
		}

		socket_write($socket, $json, strlen($json));

		$result = socket_read($socket, 1024);

		socket_close($socket);

		return $result;
	}

	$showLoginForm = false;
	$showSuccess = false;
	$showFailure = false;
	$isApi = false;

	$pIp = $_SERVER[ 'REMOTE_ADDR' ];

	if( $_SERVER[ 'REQUEST_METHOD' ] == "POST" ) {

		if (array_key_exists("user", $_POST)
			&& array_key_exists('pass', $_POST)
			&& array_key_exists('command', $_POST)
			&& array_key_exists('api', $_POST))
		{
			$pUser = $_POST[ 'user' ];
			$pPass = $_POST[ 'pass' ];
			$pCommand = $_POST[ 'command' ];
			$pApi = $_POST[ 'api' ];

			if ($pApi == "true")
			{
				$isApi = true;
			}

			$jsonResponse = json_decode(tellLock($pCommand, $pUser, $pPass, $pIp), true);
			if ($jsonResponse == null || !isset($jsonResponse['message']) || !isset($jsonResponse['code'])) {
				$showFailure = true;
				$failureMsg = 'Error parsing JSON response';
			} else {
				$failureMsg = $jsonResponse['message'];
				$code = $jsonResponse['code'];
				$showSuccess = ($code == 0);
				$showFailure = !$showSuccess;
			}
		} else {
			$failureMsg = 'Invalid Request';
			$showFailure = true;
		}

	} else {
		$showLoginForm = true;
	}
if ($isApi == false) {
?>
<!DOCTYPE html>
<html>

<head>
<title>Login</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
<meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
</head>

<body>

	<style>
		* {
			font: normal 30px Arial,sans-serif;
		}
		body {
			background-color: #037;
			color: white;
			background-image: url('logo.svg' );
			background-repeat: repeat;
			background-size: 300%;
			background-position: -200px -100px;
		}
		form {
			position: relative;
			display: block;
			width: auto;
			text-align: center;
		}
		input {
			position: relative;
			display: block;
			width: auto;
			width: 100%;
		}
		button {
			width: 100%;
			margin-top: 40px;
		}
	</style>

	<?php if ($showLoginForm): ?>

		<form name="login" method="post" action="/">
			<input type="hidden" name="api" value="false">

			<label for="user">User</label>
			<input id="user" type="text" name="user">

			<label for="pass">Pass</label>
			<input id="pass" type="password" name="pass">

			<button name="command" value="unlock">Open</button>
			<hr/>
			<button name="command" value="lock">Lock</button>
		</form>

	<?php elseif( $showSuccess ): ?>

		<h1>Welcome Cpt. Cook</h1>

	<?php elseif( $showFailure ): ?>

		<h1>Something went wrong: <?php echo $failureMsg; ?></h1>

	<?php endif; ?>

</body>

</html>
<?php
} else {
	echo $code;
}
?>
