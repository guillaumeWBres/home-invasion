<meta charset="utf-8">
<html>
	<script charset="utf-8" src="d3/d3.min.js"></script>
	<link rel="stylesheet" href="fullcalendar-3.9.0/fullcalendar.css"/>
	<script src="fullcalendar-3.9.0/lib/jquery.min.js"></script>
	<script src="fullcalendar-3.9.0/lib/moment.min.js"></script>
	<script src="fullcalendar-3.9.0/fullcalendar.min.js"></script>

	<style>

		.header {
			padding: 10px;
			font-size: 10px;
			text-align: center;
		}

		.calendarColumn {
			float: right;
			padding: 10px;
			text-align: center;
			width: 40%
			margin-top: 10px;
			margin-bottom: 10px;
		}

		.calendarCardEffect {
			border-radius: 10px;
			border: 2px black solid;
			padding: 10px;
			margin-top: 0;
		}
		
		#calendar {
			max-width: 500px;
			margin: 0 auto;
		}

		.cardEffect {
			border-radius: 10px;
			border: 2px black solid;
			padding: 5px;
		}

		.networkColumn {
			float: left;
			width: 55%;
		}

		body {
			width: 75em;
			margin: 0 auto;
			font-family: Tahoma, Verdana, Arial, sans-serif;
			background: #f1f1f1;
		}
		
		table {
			width: 40%;
			border-collapse: collapse;
			margin-top: 15px;
			margin-bottom: 15px;
		}

		th {
			background: #333;
			color: white;
			font-weight: bold;
			cursor: s-resize;
			background-repeat: no-repeat;
			background-position: 3% center;
		}

		td, th {
			padding: 6px;
			border: 1px solid #ccc;
			text-align: left;
		}

		th.des:after {
			content: "\21E9";
		}

		th.aes:after {
			content: "\21E7";
		}

		.footer {
			padding: 20px;
			text-align: center;
			margin-top: 20px;
		}
	
		.footerCardEffect {
			background-color: white;
			border-radius: 10px;
			border: 2px black solid;
			padding: 10px;
			margin-top: 10px;
		}

		.mapArea {
			margin-top: 10px;
			padding: 5px;
		}

	</style>

	<head>
		
		<script>
			
			// read events
			var request = new XMLHttpRequest();
			request.open("GET", "events.json", false);
			request.send(null);
			var events = JSON.parse(request.responseText);

			// initialize calendar object once page is settled
			$(document).ready(function() {
				$('#calendar').fullCalendar({
					header: {
						left: "",
						right: "",
						center: "title",
					},
					defaultView: "agendaWeek", // weekly agenda only
					navLinks: false, // remove navigation bar
					editable: true,
					selectable: true,
					selectHelper: true,
					firstDay: 1, // Monday
					columnFormat: 'ddd D/M', // 3 letters then day/month
					businessHours: {
						dow: [1,2,3,4,5], // Monday->Friday
						start: '09:30', // start time
						end: '18:00', // end time
					},
					events: events // read from events.json 
				})
			});
		</script>
	</head>
	
	<body>

		<div class="header">
			<h1> Welcome ! </h1>
			<?php echo 'Hello toto'; ?>
		</div>

		<div class="networkColumn">
			
			<table id="networkStatusTable">
				<tr>
					<th>Node</th>
					<th>Status</th>
				</tr>
				<tr>
					<td>Base</td>
					<td>OK</td>
				</tr>
			</table>
		
			<table>
				<tr>
					<th> Network (PAN) ID </th>
					<th> Base station ID </th>
					<th> Encryption </th>
				</tr>
				<tr>
					<td> <input type="text" name="PANID" form="PANIDform" value="5110"/> </td>
					<td> <input type="text" name="BaseNID" form="BaseNIDform" value="1001"/> </td>
					<td> <input type="checkbox" name="Encrypt" form="encryptFrom"/> </td>
				</tr>
			</table>

			<div class="cardEffect">
				<p> Power saving options </p>
				<form>
					<form method="POST" id="PANIDform"></form>
					<form method="POST" id="BaseNIDform"></form>
					<input type="radio" name="sleepmode" value="disabled" checked> Disabled<br>
					<input type="radio" name="sleepmode" value="pin-enabled"> Pin enabled<br>
					<input type="radio" name="sleepmode" value="cyclic"> Cyclic<br>
					<input type="radio" name="sleepmode" value="cyclic-wakeup"> Wakable cyclic<br>
				</form>
			</div>

		</div>
		
		<div class="calendarColumn">
			<div class="cardEffect">
				<p> Active time zones </p>
				<div id="calendar"></div>
			</div>
		</div>

		<div class="footer">
			<p><em>This project is still work in progress</em></p>
			<p><a href="http://github.com/gwbres/home-automation">project-sources</a></p>
		</div>
		
		<div class="mapArea">
			<script src="mapDrawer.js"></script>
		</div>
		
	</body>
</html>
