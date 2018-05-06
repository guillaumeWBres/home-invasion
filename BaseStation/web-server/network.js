// 1. retrieve data
var xhttp = new XMLHttpRequest();
xhttp.onreadystatechange = function() {
	if (this.readyState == 4 && this.status == 200) {
		var json = xhttp.responseText;
		var Network = JSON.parse(json);	
		var EndDevices = NetworkGetEndDevices(Network);
		var BaseStation = NetworkGetCoordinator(Network);
		NetworkAreaDrawer("mapArea", Network);
	}
};
xhttp.open("GET", "network.json", true);
xhttp.send();

// Returns number of nodes contained in network
function NetworkLength(Network) {
	return Network.Nodes.length;
}

// Returns n(th) node within Network
function NetworkGetNode(Network, n){
	return Network.Nodes[n];
}

// Returns current Network PAN ID number
function NetworkPAN(Network){
	return Network.PAN_ID;
}

// Returns all end devices in Network
function NetworkGetEndDevices(Network){
	var results = [];
	for (i=0; i<NetworkLength(Network); i++){
		if (Network.Nodes[i].Role == "End device")
			results.push(Network.Nodes[i]);
	}
	return results;
}

// Returns network coordinator
function NetworkGetCoordinator(Network){
	for (i=0; i<NetworkLength(Network); i++){
		if (Network.Nodes[i].Role == "Coordinator")
			return Network.Nodes[i];
	}
}

// Draws map in designated Area
function NetworkAreaDrawer(area, network){
	// doc dimensions	
	var width = 500;
	var height = 500;

	// nodes dimensions
	var nodeR = width/30;
	var nodeRfocused = nodeR*4;

	// doors dimensions
	var vDoorWidth = width*0.015;
	var vDoorHeight = height*0.1;
	
	// append svg object
	var svgContainer = d3.select("body").append("svg")
		.attr("width", width)
		.attr("height", height);

	// draw area
	var LinesCoords = [
		{"x1":0,"y1":height*0.2,"x2":width*0.8,"y2":height*0.2},
		{"x1":width*0.7,"y1":height*0.2,"x2":width*0.7,"y2":0},
		{"x1":width*0.7,"y1":0,"x2":width,"y2":0},
		
		{"x1":width,"y1":0,"x2":width,"y2":height*0.2},
		{"x1":width,"y1":height*0.2,"x2":width*0.7,"y2":height*0.2},
		{"x1":width*0.85,"y1":height*0.2,"x2":width*0.85,"y2":height*0.3},
		{"x1":width*0.85,"y1":height*0.3,"x2":width*0.1,"y2":height*0.3},
		{"x1":width*0.1,"y1":height*0.3,"x2":width*0.1,"y2":height*0.55},
		{"x1":width*0.1,"y1":height*0.55,"x2":width*0,"y2":height*0.55},
		
		{"x1":width*0,"y1":height*0.55,"x2":width*0,"y2":height*0.2},
		{"x1":width*0,"y1":height*0.55,"x2":width*0,"y2":height*0.75},
		{"x1":width*0,"y1":height*0.75,"x2":width*0.8,"y2":height*0.75},
		{"x1":width*0.8,"y1":height*0.75,"x2":width*0.8,"y2":height*0.55},
		{"x1":width*0.8,"y1":height*0.55,"x2":width*0.1,"y2":height*0.55},
		
		{"x1":width*0,"y1":height*0.75,"x2":width*0,"y2":height},
		{"x1":width*0,"y1":height,"x2":width*0.8,"y2":height},
		{"x1":width*0.8,"y1":height,"x2":width*0.8,"y2":height*0.75},
	];

	var lines = svgContainer.selectAll("line")
		.data(LinesCoords)
		.enter()
		.append("line");
	
	var linesAttributes = lines 
		.attr("x1", function(d) {return d.x1;})
		.attr("x2", function(d) {return d.x2;})
		.attr("y1", function(d) {return d.y1;})
		.attr("y2", function(d) {return d.y2;})
		.attr("stroke-width", 2)
		.attr("stroke", "black");

	// draw doors
	var DoorsCoords = [
		{"x":0,"y":height*0.25,"width":vDoorWidth, "height":vDoorHeight},
		{"x":width*0.09,"y":height*0.33,"width":vDoorWidth, "height":vDoorHeight},
		{"x":width*0.025,"y":height*0.535,"width":width*0.06, "height":height*0.015},
		{"x":width*0.35,"y":height*0.735,"width":width*0.1, "height":height*0.015},
		{"x":width*0.79,"y":height*0.58,"width":vDoorWidth, "height":vDoorHeight*1.25},
		{"x":width*0.79,"y":height*0.8,"width":vDoorWidth, "height":vDoorHeight},
	];

	var doorsRect = svgContainer.selectAll("rect")
		.data(DoorsCoords)
		.enter()
		.append("rect");

	var doorsRectAttributes = doorsRect
		.attr("x", function(d) {return d.x})
		.attr("y", function(d) {return d.y})
		.attr("width", function(d) {return d.width})
		.attr("height", function(d) {return d.height})

	// draw circles on nodes positions
	var CirclesCoords = [
		{"cx":width*0.05,"cy":height*0.25,"radius":nodeR,"color":"lightgreen"},
		{"cx":width*0.75,"cy":height*0.8,"radius":nodeR,"color":"lightgreen"},
		{"cx":width*0.2,"cy":height*0.6,"radius":nodeR,"color":"crimson"}
	];

	var focused = [false,false,false];

	var nodesCircles = svgContainer.selectAll("circle")
		.data(CirclesCoords)
		.enter()
		.append("circle")
		.on("mouseover", function(d, index){
			d3.select(this)
				.transition()
					.duration(700)
					.attr("r",nodeRfocused); // modify radius
		})
		.on("mouseout", function(d, index){
			d3.select(this)
				.transition().duration(700)
					.attr("r",nodeR); // modify radius
		})
		.on("click", function(d, index){
		});

	var circlesAttributes = nodesCircles
		.attr("cx", function(d) {return d.cx;})
		.attr("cy", function(d) {return d.cy;})
		.attr("r", function(d) {return d.radius;})
		.attr("fill", function(d) {return d.color;});
}
