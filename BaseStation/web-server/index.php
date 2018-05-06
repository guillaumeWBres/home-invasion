<meta charset="utf-8">
<html>
	<link rel="stylesheet" href="fullcalendar-3.9.0/fullcalendar.css"/>
	<script src="fullcalendar-3.9.0/lib/jquery.min.js"></script>
	<script src="fullcalendar-3.9.0/lib/moment.min.js"></script>
	<script src="fullcalendar-3.9.0/fullcalendar.min.js"></script>

	<style>

		.tab {
			float: left;
			border: 1px solid #ccc;
			background-color: #f1f1f1;
			width: 30%;
			height: 300px;
		}

		.tab button {
			display: block;
			background-color: inherit;
			color: black;
			width: 100%;
			text-align: left;
			border: none;
			outline: none;
			cursor: pointer;
			padding: 14px 16px;
			transition: 0.3s;
		}

		.tab button:hover {
			background-color: #ddd;
		}

		.tab button.active {
			background-color: #ccc;
		}

		.tabcontent {
			display: none;
			margin-top: 10px;
			padding: 6px 12px;
			border: 1px solid #ccc;
			border-top: none;
			float: right;
			width: 60%;
		}

		.header {
			padding: 10px;
			font-size: 10px;
			text-align: center;
		}

		#calendar {
			max-width: 800px;
			margin: 0 auto;
		}

		body {
			width: 75em;
			margin: 0 auto;
			font-family: Tahoma, Verdana, Arial, sans-serif;
			background: #f1f1f1;
		}
		
		table {
			float: right;
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
			text-align: center;
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
			float: left;
			width: 30%;
		}
	
		.mapArea {
			margin-top: 20px;
			padding: 5px;
			float: right;
		}

	</style>

	<head>
		<script src="http://d3js.org/d3.v3.js"></script>
		
		<script>
			// page has been loaded: request events from .db	
			$(document).ready(function(){

				// build calendar object
				var calendar = $('#calendar').fullCalendar({
					contentHeight: 450,
					aspectRatio: 1.35,

					header: {
						left: "customPrev",
						right: "customNext",
						center: "title,month,agendaWeek",
					},

					defaultView: "month", // weekly agenda only
					slotEventOverlap: true,
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

					customButtons: {
						customPrev: {
							text: '<',
							click: function(){
								var view = $('#calendar').fullCalendar('getView');
								var start = view.start.format('YYYY-MM-DD');
								var end = view.end.format('YYYY-MM-DD');
								$.ajax({
									url: 'http://127.0.0.1/events.php',
									data: 'vStart='+start+'&vEnd='+end,
									type: "POST",
									dataType: 'json',
									success: function(json){
										// add new events to already known events
										$('#calendar').fullCalendar('eventRender');
									},
								});
								$('#calendar').fullCalendar('prev');
							},
						},

						customNext: {
							text: '>',
							click: function(){
								$('#calendar').fullCalendar('next');
								var view = $('#calendar').fullCalendar('getView');
							},
						},
					},
				
					events: {
						url: 'events.php',
						type: 'POST',
						data: function(){
							// builds proper POST parameters for AJAX 
							// vStart: first entry of current view 
							// vEnd: last entry of current view
							// 'YYYY-MM-DD' format used for mysql->query

							var date = new Date();
							var M1 = moment(date.toDateString()).subtract(date.getDay(),'days').format('YYYY-MM-DD');
							var M2 = moment(date.toDateString()).add(7-date.getDay(),'days').format('YYYY-MM-DD');

							return {
								vStart: M1,
								vEnd: M2
							};
						},
					},

					//custom event visualization
					//eventRender: function(event, element){
					//},

					// called on user timezone selection
					select : function(start, end, allDay){
						var title = prompt("Set title:");
						if (title){ // from USER
							// format for SQL
							var start = start.format('YYYY-MM-DD HH:mm:ss');
							var end = end.format('YYYY-MM-DD HH:mm:ss');
							// pass to PHP via AJAX
							$.ajax({
								url: 'http://127.0.0.1/add_events.php',
								data: 'title='+title+'&start='+start+'&end='+end,
								type: "POST",
							});

							$('#calendar').fullCalendar('renderEvent', 
							{
								title: title,
								start: start,
								end: end,
								allDay: allDay
							},
							true // make event "stick"
							);
						}
					},
					
					// modify .db content when event has been resized 
					eventResize: function(event){
						var title = event.title;
						var start = event.start.format('YYYY-MM-DD HH:mm:ss');
						var end = event.end.format('YYYY-MM-DD HH:mm:ss');
						$.ajax({
							url: 'http://127.0.0.1/modify_events.php',
							data: 'title='+title+'&start='+start+'&end='+end,
							type: "POST",
						});
					},
					
					// modify .db content when event has been dropped 
					eventDrop: function(event){
						var title = event.title;
						var start = event.start.format('YYYY-MM-DD HH:mm:ss');
						var end = event.end.format('YYYY-MM-DD HH:mm:ss');
						$.ajax({
							url: 'http://127.0.0.1/modify_events.php',
							data: 'title='+title+'&start='+start+'&end='+end,
							type: "POST",
						});
					},

					// rename event when it has been clicked
					eventClick: function(event, jsEvent, view){
						var pTitle = event.title;
						var nName = prompt('Enter new name..');
						if (nName){
							$.ajax({
								url: 'http://127.0.0.1/rename_event.php',
								data: 'previous='+pTitle+'&new='+nName,
								type: "POST",
							});
						}
					},

				}); // calendar constructor

				tabManagement(10,'networkMgmt');
			}); // docIsReady
		</script>
	</head>
	
	<body>

		<div class="header">
			<h1> Welcome ! </h1>
		</div>

		<div class="tab">
			<button class="tablinks" onclick="tabManagement(event, 'networkMgmt')">Network management</button>
			<button class="tablinks" onclick="tabManagement(event, 'scheduler')">Scheduler</button>
			<button class="tablinks" onclick="tabManagement(event, 'plot')">Statistics</button>

			<script>
				function tabManagement(event, tab){
					var i, tabcontent, tablink;

					tabcontent = document.getElementsByClassName("tabcontent");
					for (i=0; i<tabcontent.length; i++)
						tabcontent[i].style.display = "none";

					tablinks = document.getElementsByClassName("tablinks");
					for (i=0; i<tablinks.length; i++)
						tablinks[i].className = tablinks[i].className.replace(" active", "");

					document.getElementById(tab).style.display = "block";
					//event.currentTarget.className += " active";
				}
			</script>
		</div>

		<div id="networkMgmt" class="tabcontent">
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
					<th> Power management </th>
					<th> Encryption </th>
				</tr>
				<tr>
					<td> <input type="text" name="networkPanID" form="networkPanID" value="5110"/> </td>
					<td> <input type="text" name="baseID" form="baseID" value="1001"/> </td>
					<td> 
						<input type="radio" name="powerMgmtRadio" value="disabled" checked/> Disabled 
						<input type="radio" name="powerMgmtRadio" value="cyclic"/> Cyclic hibernation 
					</td>
					<td> <input type="checkbox" name="encryptionRadio" form="encryptionRadio"/> </td>
				</tr>
			</table>
				
			<div id="mapArea" class="tabcontent">
				<script src="network.js"></script>
			</div>
		
		</div>
		
		<div id="scheduler" class="tabcontent">
			<p> Active time zones </p>
			<div id="calendar"></div>
		</div>

		<style>
			.heatMapRect {
				stroke: #E6E6E6;
				stroke-width: 2px;
			}

		</style>
		<div id="chart"></div>
		<div id="dataset-picker"></div>
		<script src="plots.js"></script>
		
		<div id="footer" class="creditsTab">
			<p><em>This project is still work in progress</em></p>
			<p><a href="http://github.com/gwbres/home-automation">project-sources</a></p>
		</div>
		
	</body>
</html>
