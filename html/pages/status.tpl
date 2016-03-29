<!doctype html>
<html>
<head>
	<meta charset="utf-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
	<meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">

	<title>Home - Current analyzer</title>

	<link href="/css/app.css" rel="stylesheet">

	
	<script src="/js/all.js"></script>
</head>
<body class="page-home">
<div id="outer">
<nav id="menu">
	<div id="brand" onclick="$('#menu').toggleClass('expanded')">Current analyzer</div>
	<a href="/" class="selected">Home</a><a href="/wifi">WiFi config</a><a href="/waveform">Waveform</a></nav>
<div id="content">

<h1>System Status</h1>

<div class="Box">
	<h2>Runtime</h2>
	<table>
		<tbody>
		<tr>
			<th>Uptime:</th>
			<td id="uptime">%uptime%</td>
		</tr>
		<tr>
			<th>Free heap:</th>
			<td id="heap">%heap%</td>
		</tr>
		</tbody>
	</table>
</div>

<div class="Box">
	<h2>WiFi</h2>
	<table>
		<tbody>
		<tr>
			<th>WiFi mode:</th>
			<td id="wmode">%wifiMode%</td>
		</tr>
		</tbody>
	</table>
</div>

<!-- WiFi info is read & updated using AJAX -->

<div class="Box sta-only" style="display:none">
	<h2>WiFi Station</h2>
	<table>
		<tbody>
		<tr>
			<th>SSID:</th>
			<td id="staSSID"></td>
		</tr>
		<tr>
			<th>RSSI:</th>
			<td>
				<span id="staRSSIperc"></span>,
				<span id="staRSSI"></span>
			</td>
		</tr>
		<tr>
			<th>MAC:</th>
			<td id="staMAC"></td>
		</tr>
		</tbody>
	</table>
</div>

<div class="Box ap-only" style="display:none">
	<h2>WiFi AP</h2>
	<table>
		<tbody>
		<tr>
			<th>SSID:</th>
			<td id="apSSID"></td>
		</tr>
		<tr>
			<th>Hidden:</th>
			<td id="apHidden"></td>
		</tr>
		<tr>
			<th>Auth. mode:</th>
			<td id="apAuth"></td>
		</tr>
		<tr class="ap-auth-only">
			<th>Password:</th>
			<td id="apPwd"></td>
		</tr>
		<tr>
			<th>Channel:</th>
			<td id="apChan"></td>
		</tr>
		<tr>
			<th>MAC:</th>
			<td id="apMAC"></td>
		</tr>
		</tbody>
	</table>
</div>

<script>
	$().ready(page_status.init);
</script>

</div>
</div>
</body>
</html>