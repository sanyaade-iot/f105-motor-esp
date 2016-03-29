var page_waveform = (function () {
	var wfm = {};

	var zoomResetFn;

	function buildChart(samples, xlabel, ylabel) {
		var data = [];
		samples.forEach(function (a, i) {
			data.push({x: i, y: a});
		});

		// Build the chart
		var plugins = [];
		var mql = window.matchMedia('screen and (min-width: 544px)');
		var isPhone = !mql.matches;

		if (!isPhone) {
			// larger than phone
			plugins.push(
				Chartist.plugins.ctAxisTitle({
					axisX: {
						axisTitle: xlabel,
						offset: {
							x: 0,
							y: 55
						}
					},
					axisY: {
						axisTitle: ylabel,
						flipText: true,
						offset: {
							x: 0,
							y: 15
						}
					}
				})
			);
		}

		// zoom
		plugins.push(Chartist.plugins.zoom({
			resetOnRightMouseBtn:true,
			onZoom: function(chart, reset) {
				zoomResetFn = reset;
			}
		}));

		new Chartist.Line('#chart', {
			series: [
				{
					name: 'a',
					data: data
				},
			]
		}, {
			showPoint: false,
			// showArea: true,
			fullWidth: true,
			chartPadding: (isPhone ? {right: 20, bottom: 5, left: 0} : {right: 25, bottom: 30, left: 25}),
			series: {
				'a': {
					lineSmooth: Chartist.Interpolation.monotoneCubic()
				}
			},
			axisX: {
				type: Chartist.AutoScaleAxis,
				onlyInteger: true
			},
			axisY: {
				type: Chartist.AutoScaleAxis,
				//onlyInteger: true
			},
			plugins: plugins
		});
	}

	function onRxData(resp, status) {
		if (status != 200) {
			// bad response
			alert("Request failed.");
			return;
		}

		var json = JSON.parse(resp);
		if (!json.success) {
			alert("Sampling failed.");
			return;
		}

		buildChart(json.samples, 'Sample number', 'Current - mA');
	}

	wfm.init = function() {
		//	var resp = {
		//		"samples": [1878, 1883, 1887, 1897, 1906, 1915, 1926, 1940, 1955, 1970, 1982, 1996, 2012, 2026, 2038, 2049],
		//		"success": true
		//	};

		function clickHdl() {
			var samples = $('#count').val();
			var freq = $('#freq').val();

			//http://192.168.1.13
			$().get('/api/raw.json?n='+samples+'&fs='+freq, onRxData, true, true);
		}

		$('#load').on('click', clickHdl);

		$('#count,#freq').on('keyup', function(e) {
			if (e.which == 13) {
				clickHdl();
			}
		});

		$('#chart').on('contextmenu', function(e) {
			zoomResetFn && zoomResetFn();
			zoomResetFn = null;
			e.preventDefault();
			return false;
		});
	};

	return wfm;
})();
