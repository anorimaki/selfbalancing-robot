import { Component, OnInit, OnDestroy,
		Input, Output, EventEmitter } from '@angular/core';
import { MatSlideToggleChange } from '@angular/material/material';
declare var CanvasJS: any;

export interface RbChartSerie {
	label: string;
	field: string;
	checked?: boolean;
	color?: number;
	type: number;
};

@Component( {
	selector: 'rb-char',
	templateUrl: './chart.component.html',
	styleUrls: ['./chart.component.css']
} )
export class RbChartComponent implements OnInit, OnDestroy {
	private static readonly COLORS: string[] = ['#b5030d', '#05F3Fd', '#1503Fd', '#F30300', '#00FF00'];

	@Input()
	public y: RbChartSerie[];

	@Input()
	public x: string;

	@Input()
	public enabled: boolean;

	@Output()
	private enable = new EventEmitter<void>();

	@Output()
	private disable = new EventEmitter<void>();

	private dataSize_: number;
	private chart: any;
	private chartOptions: any;
	private seriesData: any[][];

	constructor() {
		this.dataSize_ = 0;
	}

	ngOnInit(): void {
		this.seriesData = [];
		this.y.forEach( (serie, index) => {
			this.seriesData.push([]);
		});

		this.buildChar();
	}

	ngOnDestroy(): void {
	}

	set dataSize( x: number ) {
		this.dataSize_ = x;
	}

	insert( items: any[] ): void {
		if ( items.length > this.dataSize_ ) {
			items.splice( 0, items.length - this.dataSize_ );
		}

		this.seriesData.forEach( (values) => {
			const itemsToRemove = (values.length + items.length) - this.dataSize_;
			if ( itemsToRemove > 0 ) {
				values.splice( 0, itemsToRemove );
			}
		});

		items.forEach( item => {
			const x = item[this.x];
			this.y.forEach( (serie, index) => {
				const points = this.seriesData[index]
				points.push( {
					x: x,
					y: item[serie.field]
				});
			});
		});

		this.chart.render();
	}

	// Accessed from HTML template
	public onLabelSelectionChanged = ( index, event ) => {
		this.chart.render();
	}

	// Accessed from HTML template
	public onEnableChanged( event: MatSlideToggleChange ): void {
		if ( event.checked ) {
			this.enable.emit();
		}
		else {
			this.disable.emit();
		}
	}

	private buildChar(): void {
		const charData = [];
		const axisY = [];
		this.y.forEach( (serie, index) => {
			charData.push({
				type: 'line',
				visible: true,
				markerType: 'none',
				showInLegend: false,
				legendText: serie.label,
				xValueFormatString: '#.###',
				axisYIndex: serie.type,
				dataPoints: this.seriesData[index],
				color: RbChartComponent.COLORS[index]
			});
			axisY[serie.type] = {
				title: axisY[serie.type] ?
							(axisY[serie.type].title + ', ' + serie.label) :
							serie.label,
				titleFontWeight: 'bold'
			};
		});

		this.chartOptions = {
			zoomEnabled: true,
			axisY: axisY,
			axisX: {
				suffix: ' s',
			},
			data: charData
		};

		this.chart = new CanvasJS.Chart('chartdiv', this.chartOptions);
	}
}
